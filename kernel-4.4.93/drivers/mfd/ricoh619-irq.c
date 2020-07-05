/*
 * Interrupt driver for RICOH619 power management chip.
 *
 * Copyright (C) 2020 Ingenic Semiconductor Co., Ltd.
 * Author: cljiang <conglong.jiang@ingenic.com>
 *
 * Based on code
 *	drivers/mfd/ricoh619-irq.c
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
#include <linux/i2c.h>
#include <linux/mfd/ricoh619.h>
#include <linux/regmap.h>
#include <linux/irqdomain.h>
#include <linux/of_irq.h>

struct ricoh619_irq_data {
	struct device *dev;
	struct regmap *regmap;
	struct mutex  irq_lock;
	struct irq_domain *irq_domain;
};

static struct ricoh619_irq_data *ricoh619 = NULL;

static irqreturn_t ricoh619_irq_thread_handler(int virq, void *data)
{
	struct ricoh619_irq_data *pdata = data;
	int enable, pending = 0, ret;

	might_sleep();

	mutex_lock(&pdata->irq_lock);

	ret = regmap_read(pdata->regmap, RICOH619_INTMON, &pending);
	ret |=regmap_read(pdata->regmap, RICOH619_INTEN, &enable);
	if (!ret) {
		dev_warn(pdata->dev, "irq read i2c reg faild: %d\n", ret);
		mutex_unlock(&pdata->irq_lock);
		return IRQ_HANDLED;
	}
	pending = (pending & enable);
	mutex_unlock(&pdata->irq_lock);

	while (pending) {
		int cur_irq , i;
		i = fls(pending) - 1;
		cur_irq = irq_find_mapping(pdata->irq_domain, i);
		handle_nested_irq(cur_irq);
		pending &= ~BIT(i);
	};

	return IRQ_HANDLED;
}

static int ricoh619_domain_xlate_onecell(struct irq_domain *d, struct device_node *ctrlr,
		const u32 *intspec, unsigned int intsize,
		unsigned long *out_hwirq, unsigned int *out_type)
{
	struct regmap *regmap = (struct regmap *)d->host_data;
	int ret;

	if (!regmap)
		return -EINVAL;

	ret = irq_domain_xlate_onecell(d, ctrlr, intspec, intsize,
			out_hwirq, out_type);
	if (ret)
		return ret;

	switch (*out_hwirq) {
	case RICOH619_IRQ_WD:
		break;
	case RICOH619_IRQ_POWER_ON:
	case RICOH619_IRQ_EXTIN:
	case RICOH619_IRQ_POWER_OFF:
		regmap_update_bits(regmap, RICOH619_INTEN, BIT(*out_hwirq), BIT(*out_hwirq));
		break;
	default:
		return -EINVAL;
	}
	return 0;
}

static const struct irq_domain_ops ricoh619_irq_domain_ops = {
	.xlate = ricoh619_domain_xlate_onecell,
};

int ricoh619_irq_init(struct i2c_client *i2c, struct regmap *regmap)
{
	int ret;

	i2c->irq = irq_of_parse_and_map(i2c->dev.of_node, 0);
	if (!i2c->irq)
		return 0;

	ricoh619 = devm_kzalloc(&i2c->dev, sizeof(struct ricoh619_irq_data), GFP_KERNEL);
	if (!ricoh619)
		return -ENOMEM;

	mutex_init(&ricoh619->irq_lock);
	ricoh619->regmap = regmap;

	regmap_write(ricoh619->regmap, RICOH619_INTEN, 0);
	regmap_write(ricoh619->regmap, RICOH619_INTPOL, 0);

	ret = devm_request_threaded_irq(&i2c->dev, i2c->irq, NULL, ricoh619_irq_thread_handler,
			IRQF_TRIGGER_LOW | IRQF_ONESHOT, i2c->name, ricoh619);
	if (ret) {
		dev_err(&i2c->dev, "irq request failed: %d\n", ret);
		return ret;
	}

	ricoh619->irq_domain = irq_domain_add_linear(i2c->dev.of_node, RICOH619_NR_IRQS,
			&ricoh619_irq_domain_ops, regmap);
	if (IS_ERR(ricoh619->irq_domain)) {
		ret = PTR_ERR(ricoh619->irq_domain);
		dev_err(&i2c->dev, "irq domain add failed: %d\n", ret);
		return ret;
	}

	ricoh619->dev = &i2c->dev;

	return 0;
}

void ricoh619_irq_deinit(void)
{
	if (ricoh619->irq_domain)
		irq_domain_remove(ricoh619->irq_domain);

	regmap_write(ricoh619->regmap, RICOH619_INTEN, 0);
}
