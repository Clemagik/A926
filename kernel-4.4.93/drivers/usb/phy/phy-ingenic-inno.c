#include <linux/module.h>
#include <linux/gpio/consumer.h>
#include <linux/spinlock.h>
#include <linux/workqueue.h>
#include <linux/usb/phy.h>
#include <linux/platform_device.h>
#include <linux/usb/gadget.h>
#include <linux/usb/otg.h>
#include <linux/of.h>
#include <linux/clk.h>
#include <linux/of_gpio.h>
#include <linux/of_device.h>
#include <linux/gpio.h>
#include <linux/delay.h>
#include <linux/regmap.h>
#include <linux/mfd/syscon.h>
#include <soc/cpm.h>

#define usb_phy_readb(addr)	readb((addr))
#define usb_phy_writeb(val, addr) writeb(val,(addr))

#define PHY_TX_HS_STRENGTH_CONF	(0x40)
#define PHY_EYES_MAP_ADJ_CONF	(0x60)

#define PHY_RX_SQU_TRI		(0x64)
#define PHY_RX_SQU_TRI_112MV    (0x0)
#define PHY_RX_SQU_TRI_125MV    (0x8)

struct ingenic_usb_phy {
	struct usb_phy phy;
	struct device *dev;
	struct clk *gate_clk;
	struct regmap *regmap;
	void __iomem *base;

	/*otg phy*/
	spinlock_t phy_lock;
	struct gpio_desc	*gpiod_drvvbus;
	struct gpio_desc	*gpiod_id;
	struct gpio_desc	*gpiod_vbus;
#define USB_DETE_VBUS	0x1
#define USB_DETE_ID	0x2
	unsigned int usb_dete_state;
	struct delayed_work	work;

	enum usb_device_speed	roothub_port_speed;
};

static inline int inno_usbphy_read(struct usb_phy *x, u32 reg)
{
	struct ingenic_usb_phy *iphy = container_of(x, struct ingenic_usb_phy, phy);
	u32 val = 0;
	regmap_read(iphy->regmap, reg, &val);
	return val;
}

static inline int inno_usbphy_write(struct usb_phy *x, u32 val, u32 reg)
{
	struct ingenic_usb_phy *iphy = container_of(x, struct ingenic_usb_phy, phy);

	return regmap_write(iphy->regmap, reg, val);
}

static inline int inno_usbphy_update_bits(struct usb_phy *x, u32 reg, u32 mask, u32 bits)
{
	struct ingenic_usb_phy *iphy = container_of(x, struct ingenic_usb_phy, phy);

	return regmap_update_bits_check(iphy->regmap, reg, mask, bits, NULL);
}

static irqreturn_t usb_dete_irq_handler(int irq, void *data)
{
	struct ingenic_usb_phy *iphy = (struct ingenic_usb_phy *)data;

	schedule_delayed_work(&iphy->work, msecs_to_jiffies(100));
	return IRQ_HANDLED;
}

static void usb_dete_work(struct work_struct *work)
{
	struct ingenic_usb_phy *iphy =
		container_of(work, struct ingenic_usb_phy, work.work);
	struct usb_otg		*otg = iphy->phy.otg;
	int vbus = 0, id_level = 1, usb_state = 0;

	if (iphy->gpiod_vbus)
		vbus = gpiod_get_value_cansleep(iphy->gpiod_vbus);

	if (iphy->gpiod_id)
		id_level = gpiod_get_value_cansleep(iphy->gpiod_id);

	if (vbus && id_level)
		usb_state |= USB_DETE_VBUS;
	else
		usb_state &= ~USB_DETE_VBUS;

	if (id_level)
		usb_state |= USB_DETE_ID;
	else
		usb_state &= ~USB_DETE_ID;

	if (usb_state != iphy->usb_dete_state) {
		enum usb_phy_events	status;
		if (!(usb_state & USB_DETE_VBUS)) {
			usb_gadget_vbus_disconnect(otg->gadget);
			status = USB_EVENT_NONE;
			otg->state = OTG_STATE_B_IDLE;
			iphy->phy.last_event = status;
			if (otg->gadget)
				atomic_notifier_call_chain(&iphy->phy.notifier, status,
					otg->gadget);
		}

		if (!(usb_state & USB_DETE_ID)) {
			status = USB_EVENT_ID;
			otg->state = OTG_STATE_A_IDLE;
			iphy->phy.last_event = status;
			if (otg->host)
				atomic_notifier_call_chain(&iphy->phy.notifier, status,
						otg->host);
		}

		if (usb_state & USB_DETE_VBUS) {
			status = USB_EVENT_VBUS;
			otg->state = OTG_STATE_B_PERIPHERAL;
			iphy->phy.last_event = status;
			usb_gadget_vbus_connect(otg->gadget);
			if (otg->gadget)
				atomic_notifier_call_chain(&iphy->phy.notifier, status,
						otg->gadget);
		}
		iphy->usb_dete_state = usb_state;
	}
	return;
}

static int iphy_init(struct usb_phy *x)
{
	struct ingenic_usb_phy *iphy = container_of(x, struct ingenic_usb_phy, phy);
	u32 usbpcr, usbpcr1;
	unsigned long flags;
	u8 reg;

	if (!IS_ERR_OR_NULL(iphy->gate_clk))
		clk_prepare_enable(iphy->gate_clk);

	spin_lock_irqsave(&iphy->phy_lock, flags);
	usbpcr1 = inno_usbphy_read(x, CPM_USBPCR1);
	usbpcr1 &= ~(USBPCR1_DPPULLDOWN | USBPCR1_DMPULLDOWN);
	inno_usbphy_write(x, usbpcr1, CPM_USBPCR1);

	usbpcr = inno_usbphy_read(x, CPM_USBPCR);
	usbpcr &= ~USBPCR_IDPULLUP_MASK;
	usbpcr |= USBPCR_USB_MODE | USBPCR_IDPULLUP_OTG;
	inno_usbphy_write(x, usbpcr, CPM_USBPCR);
	inno_usbphy_update_bits(x, CPM_USBPCR1, USBPCR1_PORT_RST, USBPCR1_PORT_RST);
	inno_usbphy_update_bits(x, CPM_USBPCR, USBPCR_POR, USBPCR_POR);
	udelay(11);
	inno_usbphy_update_bits(x, CPM_USBPCR, USBPCR_POR, 0);
	udelay(11);
	inno_usbphy_update_bits(x, CPM_OPCR, OPCR_USB_SPENDN, OPCR_USB_SPENDN);
	udelay(501);
	inno_usbphy_update_bits(x, CPM_USBPCR1, USBPCR1_PORT_RST, 0);
	udelay(1);

	usbpcr1 = inno_usbphy_read(x, CPM_USBPCR1);
	usbpcr1 |= USBPCR1_DPPULLDOWN | USBPCR1_DMPULLDOWN;
	inno_usbphy_write(x, usbpcr1, CPM_USBPCR1);

	reg = usb_phy_readb(iphy->base + PHY_RX_SQU_TRI);
	reg &= ~(0xf << 3);
	reg |= PHY_RX_SQU_TRI_125MV << 3;
	usb_phy_writeb(reg,iphy->base + PHY_RX_SQU_TRI);

	spin_unlock_irqrestore(&iphy->phy_lock, flags);
	return 0;
}

static int iphy_set_suspend(struct usb_phy *x, int suspend);
static void iphy_shutdown(struct usb_phy *x)
{
	struct ingenic_usb_phy *iphy = container_of(x, struct ingenic_usb_phy, phy);

	iphy_set_suspend(x, 1);

	if (!IS_ERR_OR_NULL(iphy->gate_clk))
		clk_disable_unprepare(iphy->gate_clk);
}

static int iphy_set_vbus(struct usb_phy *x, int on)
{
	struct ingenic_usb_phy *iphy = container_of(x, struct ingenic_usb_phy, phy);

	if (!(IS_ERR_OR_NULL(iphy->gpiod_drvvbus))) {
		printk("OTG VBUS %s\n", on ? "ON" : "OFF");

#ifdef CONFIG_HALLEY5_V10_HOST_TO_4GMODULE
		if(!(IS_ENABLED(CONFIG_DT_HALLEY5_V10) && IS_ENABLED(CONFIG_USB_DWC2_HOST)))
#endif
			gpiod_set_value(iphy->gpiod_drvvbus, on);
	}
	return 0;
}

static int iphy_set_suspend(struct usb_phy *x, int suspend)
{
	struct ingenic_usb_phy *iphy = container_of(x, struct ingenic_usb_phy, phy);
	unsigned long flags;

	pr_info("usb phy suspend %d\n", suspend);

	spin_lock_irqsave(&iphy->phy_lock, flags);
	if (suspend)
		inno_usbphy_update_bits(x, CPM_OPCR, USBPCR1_PORT_RST, 0);

	inno_usbphy_update_bits(x, CPM_OPCR, OPCR_USB_SPENDN,
			suspend ? 0 : OPCR_USB_SPENDN);
	if (!suspend) {
		udelay(501);	/*500us and 10 utmi clock*/
		inno_usbphy_update_bits(x, CPM_OPCR, USBPCR1_PORT_RST, USBPCR1_PORT_RST);
		udelay(1);	/*1 us*/
		inno_usbphy_update_bits(x, CPM_OPCR, USBPCR1_PORT_RST, 0);
	}
	udelay(1); /*1us*/
	spin_unlock_irqrestore(&iphy->phy_lock, flags);

	return 0;
}

static int iphy_set_wakeup(struct usb_phy *x, bool enabled)
{
	struct ingenic_usb_phy *iphy = container_of(x, struct ingenic_usb_phy, phy);
	struct usb_otg *otg = iphy->phy.otg;
	enum usb_device_speed speed;

	if (enabled) {
		/*enable usb resume interrupt*/
		if (otg && otg->state >= OTG_STATE_A_IDLE)
			speed = iphy->roothub_port_speed;
		else
			speed = (otg && otg->gadget) ? otg->gadget->speed : USB_SPEED_FULL;
		inno_usbphy_update_bits(x, CPM_USBRDT, USBRDT_RESUME_INTEEN, USBRDT_RESUME_INTEEN);
	} else {
		/*disable usb resume interrupt*/
		inno_usbphy_update_bits(x, CPM_USBRDT, USBRDT_RESUME_INTEEN|USBRDT_RESUME_INTERCLR,
				USBRDT_RESUME_INTERCLR);
	}
	return 0;
}

static int iphy_set_host(struct usb_otg *otg, struct usb_bus *host)
{
	if (!otg)
		return -ENODEV;
	otg->host = host;
	return 0;
}

static int iphy_set_peripheral(struct usb_otg *otg, struct usb_gadget *gadget)
{
	if (!otg)
		return -ENODEV;
	otg->gadget = gadget;
	return 0;
}

static int iphy_notify_connect(struct usb_phy *x,
		enum usb_device_speed speed)
{
	struct ingenic_usb_phy *iphy = container_of(x, struct ingenic_usb_phy, phy);

	iphy->roothub_port_speed = speed;

	return 0;
}

static int usb_phy_ingenic_probe(struct platform_device *pdev)
{
	struct ingenic_usb_phy *iphy;
	struct resource *res;
	int ret;

	iphy = (struct ingenic_usb_phy *)
		devm_kzalloc(&pdev->dev, sizeof(*iphy), GFP_KERNEL);
	if (!iphy)
		return -ENOMEM;

	res = platform_get_resource(pdev, IORESOURCE_MEM, 1);
	iphy->base = devm_ioremap_resource(&pdev->dev, res);
	if (IS_ERR(iphy->base))
		return PTR_ERR(iphy->base);

	iphy->phy.init = iphy_init;
	iphy->phy.shutdown = iphy_shutdown;
	iphy->phy.dev = &pdev->dev;
	iphy->regmap = syscon_regmap_lookup_by_phandle(pdev->dev.of_node, NULL);
	if (IS_ERR(iphy->regmap)) {
		dev_err(&pdev->dev, "failed to find regmap for usb phy %ld\n", PTR_ERR(iphy->regmap));
		return PTR_ERR(iphy->regmap);
	}

	spin_lock_init(&iphy->phy_lock);

	iphy->gpiod_id = devm_gpiod_get_optional(&pdev->dev,"ingenic,id-dete", GPIOD_IN);
	iphy->gpiod_vbus = devm_gpiod_get_optional(&pdev->dev,"ingenic,vbus-dete", GPIOD_ASIS);
	if (iphy->gpiod_id || iphy->gpiod_vbus)
		INIT_DELAYED_WORK(&iphy->work, usb_dete_work);

	if (!IS_ERR_OR_NULL(iphy->gpiod_id)) {
		ret = devm_request_irq(&pdev->dev,
				gpiod_to_irq(iphy->gpiod_id),
				usb_dete_irq_handler,
				IRQF_TRIGGER_RISING|IRQF_TRIGGER_FALLING|IRQF_ONESHOT,
				"id_dete",
				(void *)iphy);
		if (ret)
			return ret;
	} else {
		iphy->usb_dete_state |= USB_DETE_ID;
		iphy->gpiod_id = NULL;
	}

	if (!IS_ERR_OR_NULL(iphy->gpiod_vbus)) {
		ret = devm_request_irq(&pdev->dev,
				gpiod_to_irq(iphy->gpiod_vbus),
				usb_dete_irq_handler,
				IRQF_TRIGGER_RISING|IRQF_TRIGGER_FALLING|IRQF_ONESHOT,
				"vbus_dete",
				(void *)iphy);
		if (ret)
			return ret;
	} else {
		iphy->usb_dete_state &= ~USB_DETE_VBUS;
		iphy->gpiod_vbus = NULL;
	}

	iphy->gpiod_drvvbus = devm_gpiod_get_optional(&pdev->dev,"ingenic,drvvbus", GPIOD_OUT_LOW);
	if (IS_ERR_OR_NULL(iphy->gpiod_drvvbus))
		iphy->gpiod_drvvbus = NULL;
	iphy->phy.set_vbus = iphy_set_vbus;
	iphy->phy.set_suspend = iphy_set_suspend;
	iphy->phy.set_wakeup = iphy_set_wakeup;
	iphy->phy.notify_connect = iphy_notify_connect;
	iphy->phy.otg = devm_kzalloc(&pdev->dev, sizeof(*iphy->phy.otg),
			GFP_KERNEL);

	if (!iphy->phy.otg)
		return -ENOMEM;

	iphy->phy.otg->state		= OTG_STATE_UNDEFINED;
	iphy->phy.otg->usb_phy		= &iphy->phy;
	iphy->phy.otg->set_host	= iphy_set_host;
	iphy->phy.otg->set_peripheral	= iphy_set_peripheral;

	iphy->gate_clk = devm_clk_get(&pdev->dev, "gate_usbphy");
	if (IS_ERR_OR_NULL(iphy->gate_clk)){
		iphy->gate_clk = NULL;
		dev_err(&pdev->dev, "cannot get usbphy clock !\n");
		return -ENODEV;
	}

	ret = usb_add_phy_dev(&iphy->phy);
	if (ret) {
		dev_err(&pdev->dev, "can't register transceiver, err: %d\n",
				ret);
		return ret;
	}
	platform_set_drvdata(pdev, iphy);
	pr_info("inno phy probe success\n");
	return 0;
}

static int usb_phy_ingenic_remove(struct platform_device *pdev)
{
	struct ingenic_usb_phy *iphy = platform_get_drvdata(pdev);

	usb_remove_phy(&iphy->phy);
	return 0;
}

static const struct of_device_id of_matchs[] = {
	{ .compatible = "ingenic,innophy"},
	{ }
};
MODULE_DEVICE_TABLE(of, ingenic_xceiv_dt_ids);

static struct platform_driver usb_phy_ingenic_driver = {
	.probe		= usb_phy_ingenic_probe,
	.remove		= usb_phy_ingenic_remove,
	.driver		= {
		.name	= "usb_phy",
		.of_match_table = of_matchs,
	},
};

static int __init usb_phy_ingenic_init(void)
{
	return platform_driver_register(&usb_phy_ingenic_driver);
}
subsys_initcall(usb_phy_ingenic_init);

static void __exit usb_phy_ingenic_exit(void)
{
	platform_driver_unregister(&usb_phy_ingenic_driver);
}
module_exit(usb_phy_ingenic_exit);
