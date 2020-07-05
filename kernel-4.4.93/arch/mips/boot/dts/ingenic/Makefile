dtb-$(CONFIG_JZ4740_QI_LB60)	+= qi_lb60.dtb
dtb-$(CONFIG_JZ4780_CI20)	+= ci20.dtb
dtb-$(CONFIG_DT_HALLEY2_V20)	+= halley2v20.dtb
dtb-$(CONFIG_DT_SEAL)		+= seal.dtb
dtb-$(CONFIG_DT_HALLEY5_V10)	+= halley5_v10.dtb
dtb-$(CONFIG_DT_HALLEY5_V20)	+= halley5_v20.dtb
dtb-$(CONFIG_DT_XXZA926)  += xxzA926.dtb
dtb-$(CONFIG_DT_ZEBRA)		+= zebra.dtb
dtb-$(CONFIG_DT_ZEBRA_ISP)	+= zebra_isp.dtb
dtb-$(CONFIG_DT_ZEBRA_CIM)	+= zebra_cim.dtb
dtb-$(CONFIG_DT_X1500_YAK)	+= yak.dtb

obj-y				+= $(patsubst %.dtb, %.dtb.o, $(dtb-y))

# Force kbuild to make empty built-in.o if necessary
obj-				+= dummy.o

always				:= $(dtb-y)
clean-files			:= *.dtb *.dtb.S
