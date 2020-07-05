
linux-4.4 X1000/X1500 camera source code

## X1000 camera controller
    CONFIG_VIDEO_INGENIC_X1000=y
    drivers/media/platform/soc_camera/ingenic/x1000/ingenic_camera.c

## platform board configuration:
### board config
    arch/mips/configs/halley2_linux_nor_defconfig

### board dts
    arch/mips/boot/dts/ingenic/halley2v20.dts
```
&i2c0 {
	status = "okay";
	clock-frequency = <100000>;
	timeout = <1000>;
	pinctrl-names = "default";
	pinctrl-0 = <&i2c0_pb>;

	gc2155: gc2155@0x3c {
		status = "okay";
		compatible = "ovti,gc2155";
		reg = <0x3c>;
		pinctrl-names = "default";
		pinctrl-0 = <&cim_pa>;

		resetb-gpios 	= <&gpd 5 GPIO_ACTIVE_LOW INGENIC_GPIO_NOBIAS>;
		pwdn-gpios 	= <&gpd 4 GPIO_ACTIVE_HIGH INGENIC_GPIO_NOBIAS>;
		vcc-en-gpios	= <&gpd 3 GPIO_ACTIVE_HIGH INGENIC_GPIO_NOBIAS>;

		port {
			gc2155_0: endpoint {
				remote-endpoint = <&cim_0>;
			};
		};
	};
};
```

## sensor driver
    CONFIG_SOC_CAMERA_GC2155=y
    drivers/media/i2c/soc_camera/gc2155.c

