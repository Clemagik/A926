/*
 * Core driver access RICOH619 power management chip.
 *
 * Copyright (C) 2020 Ingenic Semiconductor Co., Ltd.
 * Author: cljiang <conglong.jiang@ingenic.com>
 *
 * Based on code
 *	drivers/mfd/ricoh619.c
 *	Copyright (c) 2011-2012, NVIDIA CORPORATION.  All rights reserved.
 *	Author: Laxman dewangan <ldewangan@nvidia.com>
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
#include <linux/interrupt.h>
#include <linux/irq.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/err.h>
#include <linux/slab.h>
#include <linux/i2c.h>
#include <linux/mfd/core.h>
#include <linux/mfd/ricoh619.h>
#include <linux/regmap.h>
#include <linux/regulator/consumer.h>

static const struct mfd_cell ricoh619_cells[] = {
	{ .name = "ricoh619-regulator" },
	/*{ .name = "ricoh619-wdt" },
	  { .name = "ricoh619-pinctrl" },
	  { .name = "ricoh619-pm" }, */
};

static bool ricoh619_reg_hole(unsigned int reg)
{
#define RICOH619_REG_SINGLE_HOLE_NUM 16
	unsigned char single_hole[RICOH619_REG_SINGLE_HOLE_NUM] = {
		0x4, 0x8, 0x2b, 0x43, 0x49, 0x4b, 0x62, 0x63, 0x67, 0x8b,
            0x8f, 0x99, 0x9a, 0x9b, 0x9f, 0xb2, };
	int i;

	for (i = 0; i < RICOH619_REG_SINGLE_HOLE_NUM; i++)
		if ((unsigned char)reg == single_hole[i])
			return true;
	if (reg > RICOH619_MAX_REG)
		return true;
#undef RICOH619_REG_SINGLE_HOLE_NUM
	return false;
}

static bool ricoh619_opable_reg(struct device *dev, unsigned int reg)
{
	return !ricoh619_reg_hole(reg);
}

static bool ricoh619_volatile_reg(struct device *dev, unsigned int reg)
{
	switch (reg) {
	case RICOH619_WATCHDOGCNT:
	case RICOH619_DCIRQ:
	case RICOH619_IR_GPR:
	case RICOH619_IR_GPF:
	case RICOH619_MON_IOIN:
	case RICOH619_INTMON:
		return true;
	default:
		return false;
	}
}

static struct reg_default ricoh619_reg_default[RICOH619_REG_NUM];
static const struct regmap_config ricoh619_regmap_config = {
	.reg_bits	= 8,
	.val_bits	= 8,
	.volatile_reg	= ricoh619_volatile_reg,
	.writeable_reg  = ricoh619_opable_reg,
	.readable_reg	= ricoh619_opable_reg,
	.max_register	= RICOH619_MAX_REG,
	.reg_defaults	= ricoh619_reg_default,
	.num_reg_defaults = ARRAY_SIZE(ricoh619_reg_default),
	.cache_type	= REGCACHE_RBTREE,
};

static int __ricoh619_reg_read(struct i2c_client *i2c, u8 reg)
{
	return i2c_smbus_read_byte_data(i2c, reg);
}

static int __ricoh619_reg_write(struct i2c_client *i2c, u8 reg, u8 val)
{
	return i2c_smbus_write_byte_data(i2c, reg, val);
}

static void ricoh619_reg_default_init(struct i2c_client *i2c)
{
	unsigned int reg, def;
	int i;

	for (i = 0, reg = 0; i < RICOH619_REG_NUM &&
			reg <= RICOH619_MAX_REG; reg++) {
		if (ricoh619_reg_hole(reg) ||
				ricoh619_volatile_reg(NULL, reg))
			continue;
		ricoh619_reg_default[i].reg = reg;
		def = __ricoh619_reg_read(i2c, (u8)reg);
		if (def < 0) {
			dev_warn(&i2c->dev, "register %x read failed: %d\n", reg, def);
			ricoh619_reg_default[i++].def = 0;
		} else{
			ricoh619_reg_default[i++].def = def;
//            printk("ricoh619: reg: 0x%x , val: 0x%x\n", ricoh619_reg_default[i-1].reg, ricoh619_reg_default[i-1].def);
        }
	}
}

struct sleep_slot_tab {
	u8 reg;
	u8 slot;
};

struct sleep_slot_tab ricoh619_sleep_slot_tab[] = {
    {0x16, 15},	//0.9
    {0x17, 15},	//3.3
    {0x18, 15}, //1.8
	{0x19, 15},	//1.2
	{0x1a, 15},	//1.2
};


void ricoh619_set_sleep_slot(struct i2c_client *i2c, struct sleep_slot_tab *sleep_slot_tab)
{
	struct sleep_slot_tab *sst;
	u8 val;
	unsigned int i;

	sst = sleep_slot_tab;

	for (i = 0; i < ARRAY_SIZE(ricoh619_sleep_slot_tab); i++) {
		val = __ricoh619_reg_read(i2c, sst[i].reg);
		val &= ~0xf;
		val |= sst[i].slot;
		printk("DCDC%d : 0x%x  %d", i + 1, val, sst[i].slot);
		__ricoh619_reg_write(i2c, sst[i].reg, val);
		val = __ricoh619_reg_read(i2c, sst[i].reg);
		printk("    val = %x\n", val);
	}

}

static int ricoh619_i2c_probe(struct i2c_client *i2c,
		const struct i2c_device_id *id)
{
	struct regmap *regmap = NULL;
	int ret;

	ricoh619_reg_default_init(i2c);

	regmap = devm_regmap_init_i2c(i2c, &ricoh619_regmap_config);
	if (IS_ERR(regmap)) {
		ret = PTR_ERR(regmap);
		dev_err(&i2c->dev, "regmap init failed: %d\n", ret);
		return ret;
	}
	i2c_set_clientdata(i2c, regmap);

	ret = ricoh619_irq_init(i2c, regmap);
	if (ret)
		return ret;


	ret = mfd_add_devices(&i2c->dev, -1, ricoh619_cells,
			ARRAY_SIZE(ricoh619_cells), NULL, 0, NULL);
	if (ret) {
		dev_err(&i2c->dev, "failed to add sub-devices: %d\n", ret);
		goto out_irq_domain_remove;
	}

	ricoh619_set_sleep_slot(i2c, ricoh619_sleep_slot_tab);

	dev_info(&i2c->dev, "%s success\n", __func__);

    /*__ricoh619_reg_write(i2c, RICOH619_DC5CTL, 0x03);*/
    /*__ricoh619_reg_write(i2c, RICOH619_DC5DAC, 0x30);*/
    __ricoh619_reg_write(i2c, RICOH619_DC5_SLOT, 0x2f);
    __ricoh619_reg_write(i2c, RICOH619_LDO1_SLOT, 0x2f);
    __ricoh619_reg_write(i2c, RICOH619_LDO3_SLOT, 0x2f);
    /*__ricoh619_reg_write(i2c, RICOH619_LDO3DAC, 0x4c);*/
    /*printk("cljiang======================RICOH619_DC1CTL = 0x%x\n", __ricoh619_reg_read(i2c, RICOH619_DC1CTL));*/
    /*printk("cljiang======================RICOH619_DC1DAC = 0x%x\n", __ricoh619_reg_read(i2c, RICOH619_DC1DAC));*/
    /*printk("cljiang======================RICOH619_DC2CTL = 0x%x\n", __ricoh619_reg_read(i2c, RICOH619_DC2CTL));*/
    /*printk("cljiang======================RICOH619_DC2DAC = 0x%x\n", __ricoh619_reg_read(i2c, RICOH619_DC2DAC));*/
    /*printk("cljiang======================RICOH619_DC3CTL = 0x%x\n", __ricoh619_reg_read(i2c, RICOH619_DC3CTL));*/
    /*printk("cljiang======================RICOH619_DC3DAC = 0x%x\n", __ricoh619_reg_read(i2c, RICOH619_DC3DAC));*/
    /*printk("cljiang======================RICOH619_DC4CTL = 0x%x\n", __ricoh619_reg_read(i2c, RICOH619_DC4CTL));*/
    /*printk("cljiang======================RICOH619_DC4DAC = 0x%x\n", __ricoh619_reg_read(i2c, RICOH619_DC4DAC));*/
    /*printk("cljiang======================RICOH619_DC5CTL = 0x%x\n", __ricoh619_reg_read(i2c, RICOH619_DC5CTL));*/
    /*printk("cljiang======================RICOH619_DC5DAC = 0x%x\n", __ricoh619_reg_read(i2c, RICOH619_DC5DAC));*/
    struct regulator *reg = devm_regulator_get(&i2c->dev, "ldo3_2v8");
    regulator_set_voltage(reg, 2800000, 2800000);
    regulator_enable(reg);

    /*reg = devm_regulator_get(&i2c->dev, "DC2_1v2");*/
    /*regulator_set_voltage(reg, 1200000, 1200000);*/
    /*regulator_enable(reg);*/

    /*reg = devm_regulator_get(&i2c->dev, "DC4_1v8");*/
    /*regulator_set_voltage(reg, 1800000, 1800000);*/
    /*regulator_enable(reg);*/

    printk("cljiang======================RICOH619_DC5CTL = 0x%x\n", __ricoh619_reg_read(i2c, RICOH619_DC5CTL));
    printk("cljiang======================RICOH619_DC5DAC = 0x%x\n", __ricoh619_reg_read(i2c, RICOH619_DC5DAC));
    reg = devm_regulator_get(&i2c->dev, "DC5_1v2");
    regulator_set_voltage(reg, 1200000, 1200000);
    regulator_enable(reg);
    printk("cljiang======================RICOH619_DC5CTL = 0x%x\n", __ricoh619_reg_read(i2c, RICOH619_DC5CTL));
    printk("cljiang======================RICOH619_DC5DAC = 0x%x\n", __ricoh619_reg_read(i2c, RICOH619_DC5DAC));

    /*printk("\n\n\n\n");*/
    /*printk("cljiang======================RICOH619_LDOEN1 = 0x%x\n", __ricoh619_reg_read(i2c, RICOH619_LDOEN1));*/
    /*printk("cljiang======================RICOH619_LDO3DAC = 0x%x\n", __ricoh619_reg_read(i2c, RICOH619_LDO3DAC));*/



	return 0;

out_irq_domain_remove:
	ricoh619_irq_deinit();
	return ret;
}

static int ricoh619_i2c_remove(struct i2c_client *i2c)
{
	mfd_remove_devices(&i2c->dev);
	return 0;
}

static const struct of_device_id ricoh619_of_match[] = {
	{ .compatible = "ricoh,rn5t619" },
	{ }
};
MODULE_DEVICE_TABLE(of, ricoh619_of_match);

static const struct i2c_device_id rc5t619_i2c_id[] = {
	{.name = "rc5t619", .driver_data = 0},
	{}
};


static struct i2c_driver ricoh619_i2c_driver = {
	.driver = {
		.name = "ricoh619",
		.of_match_table = of_match_ptr(ricoh619_of_match),
	},
	.probe = ricoh619_i2c_probe,
	.remove = ricoh619_i2c_remove,
	.id_table = rc5t619_i2c_id,
};

static int __init ricoh619_i2c_init(void)
{
	int ret = -ENODEV;

	ret = i2c_add_driver(&ricoh619_i2c_driver);
	if (ret != 0)
		pr_err("Failed to register I2C driver: %d\n", ret);
	return ret;
}
subsys_initcall(ricoh619_i2c_init);

static void __exit ricoh619_i2c_exit(void)
{
	i2c_del_driver(&ricoh619_i2c_driver);
}

module_exit(ricoh619_i2c_exit);

MODULE_DESCRIPTION("Ricoh RICOH619 MFD driver");
MODULE_LICENSE("GPL v2");
