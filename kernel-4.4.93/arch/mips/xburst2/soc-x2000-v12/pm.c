#include <linux/init.h>
#include <linux/pm.h>
#include <linux/suspend.h>

#include <soc/cache.h>
#include <soc/base.h>
#include <asm/io.h>
#include <soc/ddr.h>
#include <soc/base.h>
#include <soc/cpm.h>


#define DEBUG_PM

#define SLEEP_MEMORY_START		0xb2400000
#define SLEEP_MEMORY_END		0xb2407ff8
#define SLEEP_RESUME_SP			SLEEP_MEMORY_END
#define SLEEP_RESUME_BOOTUP_TEXT	SLEEP_MEMORY_START

#define SLEEP_CPU_RESUME_BOOTUP_TEXT	SLEEP_RESUME_BOOTUP_TEXT
#define SLEEP_CPU_RESUME_BOOTUP_LEN	64 // 16 instructions
#define SLEEP_CPU_RESUME_TEXT		(SLEEP_CPU_RESUME_BOOTUP_TEXT + SLEEP_CPU_RESUME_BOOTUP_LEN)
#define SLEEP_CPU_RESUME_LEN		(4096)
#define SLEEP_CPU_SLEEP_TEXT		(SLEEP_CPU_RESUME_TEXT + SLEEP_CPU_RESUME_LEN)
#define SLEEP_CPU_SLEEP_LEN		(3072)
#define SLEEP_CPU_RESUME_SP		SLEEP_RESUME_SP


#define GPIO_PE_BASE (0xb0010400)


volatile unsigned int clk_gate0;
volatile unsigned int clk_gate1;
volatile unsigned int gpio_pe_int;
volatile unsigned int gpio_pe_msk;
volatile unsigned int gpio_pe_pat1;
volatile unsigned int gpio_pe_pat0;
volatile unsigned int intc0_msk;
volatile unsigned int intc1_msk;
volatile unsigned int cpu_div;



/*************************************** debug ***************************************/

#define PRINT_DEBUG

#ifdef PRINT_DEBUG

#define OFF_TDR         (0x00)
#define OFF_LCR         (0x0C)
#define OFF_LSR         (0x14)
#define LSR_TDRQ        (1 << 5)
#define LSR_TEMT        (1 << 6)

#if defined(CONFIG_DT_HALLEY5_V10) || defined(CONFIG_DT_HALLEY5_V20)
#define U_IOBASE (UART3_IOBASE + 0xa0000000)
#elif defined(CONFIG_DT_ZEBRA) || defined(CONFIG_DT_ZEBRA_ISP) || defined(CONFIG_DT_ZEBRA_CIM)
#define U_IOBASE (UART2_IOBASE + 0xa0000000)
#else
#define U_IOBASE (UART2_IOBASE + 0xa0000000)
#endif

#define TCSM_PCHAR(x)                                                   \
	*((volatile unsigned int*)(U_IOBASE+OFF_TDR)) = x;              \
while ((*((volatile unsigned int*)(U_IOBASE + OFF_LSR)) & (LSR_TDRQ | LSR_TEMT)) != (LSR_TDRQ | LSR_TEMT))
#else
#define TCSM_PCHAR(x)
#endif

#define TCSM_DELAY(x)                                           \
	do{                                                     \
		register unsigned int i = x;                    \
		while(i--)                                      \
		__asm__ volatile(".set mips32\n\t"      \
				"nop\n\t"              \
				".set mips32");        \
	}while(0)                                               \

static inline void serial_put_hex(unsigned int x) {
	int i;
	unsigned int d;
	for(i = 7;i >= 0;i--) {
		d = (x  >> (i * 4)) & 0xf;
		if(d < 10) d += '0';
		else d += 'A' - 10;
		TCSM_PCHAR(d);
	}
	TCSM_PCHAR('\r');
	TCSM_PCHAR('\n');
}

/*************************************************************************************/
#define rtc_read_reg(reg) ({ \
	while (!((*(volatile unsigned int *)0xb0003000 >> 7) & 0x1) ); \
	*(volatile unsigned int *)reg;\
})

#define rtc_write_reg(reg, val) do{ \
	while (!((*(volatile unsigned int *)0xb0003000 >> 7) & 0x1) ); \
	*(volatile unsigned int *)0xb000303c = 0xa55a; \
	while (!((*(volatile unsigned int *)0xb000303c >> 31) & 0x1) ); \
	while (!((*(volatile unsigned int *)0xb0003000 >> 7) & 0x1) ); \
	*(volatile unsigned int *)reg = val; \
	while (!((*(volatile unsigned int *)0xb0003000 >> 7) & 0x1) ); \
}while(0)

static void enable_rtc_wakeup(void)
{
	unsigned int tmp;

	intc1_msk = *(volatile unsigned int *)0xb0001024;
	*(volatile unsigned int *)0xb0001024 = intc1_msk & ~(1<<0); // RTC INT MSK

	tmp = 0x00af0064;
	tmp &= ~(1 << 23);
	tmp &= ~(1 << 22);
	tmp &= ~(1 << 20);
	rtc_write_reg(0xb0003048, tmp);

	tmp = 0x2d52d2d0;
	tmp |= 1;
	rtc_write_reg(0xb000302c, tmp);
}


/*-----------------------------------------------------------------------------
 *  extern function declare
 *-----------------------------------------------------------------------------*/
#define  g_state  ((volatile suspend_state_t *)0xb2407ffc)
volatile unsigned int _regs_stack_0[136 / 4];
volatile unsigned int _regs_stack_1[136 / 4];
volatile unsigned int *_regs_stack;
//#define X2000_IDLE_PD
//#define PD_OTHER_MODULE

extern long long save_goto(unsigned int func);
extern int restore_goto(unsigned int func);


#define DDR_PHY_OFFSET  (-0x4e0000 + 0x1000)
#define DDRP_INNOPHY_RXDLL_DELAY_AL             (DDR_PHY_OFFSET + (0x48 << 2))
#define DDRP_INNOPHY_RXDLL_DELAY_AH             (DDR_PHY_OFFSET + (0x58 << 2))

static void ddrp_auto_calibration(void)
{
	unsigned int reg_val = ddr_readl(DDRP_INNOPHY_TRAINING_CTRL);
	unsigned int timeout = 0xffffff;
	unsigned int wait_cal_done = DDRP_CALIB_DONE_HDQCFA | DDRP_CALIB_DONE_LDQCFA;

	reg_val &= ~(DDRP_TRAINING_CTRL_DSCSE_BP);
	reg_val |= DDRP_TRAINING_CTRL_DSACE_START;
	ddr_writel(reg_val, DDRP_INNOPHY_TRAINING_CTRL);

	while(!((ddr_readl(DDRP_INNOPHY_CALIB_DONE) & wait_cal_done) == wait_cal_done) && --timeout) {
		TCSM_PCHAR('t');
	}

	if(!timeout) {
		TCSM_PCHAR('f');
	}
	ddr_writel(0, DDRP_INNOPHY_TRAINING_CTRL);
}




static void load_func_to_tcsm(unsigned int *tcsm_addr,unsigned int *f_addr,unsigned int size)
{
	unsigned int instr;
	int offset;
	int i;
#ifdef DEBUG_PM
	printk("tcsm addr = %p %p size = %d\n",tcsm_addr,f_addr,size);
#endif
	for(i = 0;i < size / 4;i++) {
		instr = f_addr[i];
		if((instr >> 26) == 2){
			offset = instr & 0x3ffffff;
			offset = (offset << 2) - ((unsigned int)f_addr & 0xfffffff);
			if(offset > 0) {
				offset = ((unsigned int)tcsm_addr & 0xfffffff) + offset;
				instr = (2 << 26) | (offset >> 2);
			}
		}
		tcsm_addr[i] = instr;
	}
}



static void enable_pe31_int_low(void)
{
	unsigned int ahb2_intc = 0xb0001000;

	printk("intc0 src, ahb2_intc + 0x00: %08x\n", *(volatile unsigned int *)(ahb2_intc + 0x00));
	printk("intc0 msk, ahb2_intc + 0x04: %08x\n", *(volatile unsigned int *)(ahb2_intc + 0x04));
	printk("intc0 pnd, ahb2_intc + 0x10: %08x\n", *(volatile unsigned int *)(ahb2_intc + 0x10));

	printk("intc1 src, ahb2_intc + 0x20: %08x\n", *(volatile unsigned int *)(ahb2_intc + 0x20));
	printk("intc1 msk, ahb2_intc + 0x24: %08x\n", *(volatile unsigned int *)(ahb2_intc + 0x24));
	printk("intc1 pnd, ahb2_intc + 0x30: %08x\n", *(volatile unsigned int *)(ahb2_intc + 0x30));


	intc0_msk = *(volatile unsigned int *)0xb0001004;
	*(volatile unsigned int *)0xb0001004 = intc0_msk & ~(1<<13); // GPIO4 INT MSK

	printk("intc0 src, ahb2_intc + 0x00: %08x\n", *(volatile unsigned int *)(ahb2_intc + 0x00));
	printk("intc0 msk, ahb2_intc + 0x04: %08x\n", *(volatile unsigned int *)(ahb2_intc + 0x04));
	printk("intc0 pnd, ahb2_intc + 0x10: %08x\n", *(volatile unsigned int *)(ahb2_intc + 0x10));

	printk("intc1 src, ahb2_intc + 0x20: %08x\n", *(volatile unsigned int *)(ahb2_intc + 0x20));
	printk("intc1 msk, ahb2_intc + 0x24: %08x\n", *(volatile unsigned int *)(ahb2_intc + 0x24));
	printk("intc1 pnd, ahb2_intc + 0x30: %08x\n", *(volatile unsigned int *)(ahb2_intc + 0x30));

	gpio_pe_int = *(volatile unsigned int *) (GPIO_PE_BASE + 0x10);
	gpio_pe_msk = *(volatile unsigned int *) (GPIO_PE_BASE + 0x20);
	gpio_pe_pat1 = *(volatile unsigned int *)(GPIO_PE_BASE + 0x30);
	gpio_pe_pat0 = *(volatile unsigned int *)(GPIO_PE_BASE + 0x40);


	*(volatile unsigned int *)(GPIO_PE_BASE + 0x10) = gpio_pe_int | (1<<31);
	*(volatile unsigned int *)(GPIO_PE_BASE + 0x20) = gpio_pe_msk & ~(1<<31);
	*(volatile unsigned int *)(GPIO_PE_BASE + 0x30) = gpio_pe_pat1 & ~(1<<31);
	*(volatile unsigned int *)(GPIO_PE_BASE + 0x40) = gpio_pe_pat0 & ~(1<<31); // low level trigger int

}

static int soc_pm_config(void)
{
	clk_gate0 = cpm_inl(CPM_CLKGR);
	clk_gate1 = cpm_inl(CPM_CLKGR1);

	clk_gate0 |= 1 << 20;	// disable ost
	clk_gate0 &= ~(1 << 28);	// enable apb0, for ddr
	clk_gate0 &= ~(1 << 27);	// enable rtc
	clk_gate1 &= ~(1 << 26);	//initc

	cpm_outl(clk_gate0, CPM_CLKGR);
	cpm_outl(clk_gate1, CPM_CLKGR1);

	enable_pe31_int_low();

	enable_rtc_wakeup();

#ifdef PD_OTHER_MODULE
	{
		unsigned int gate0 = cpm_inl(CPM_CLKGR);
		unsigned int gate1 = cpm_inl(CPM_CLKGR1);

		cpm_outl(0, CPM_CLKGR);
		cpm_outl(0, CPM_CLKGR1);
		{
			/* vpu isp */
			unsigned int lcr = cpm_inl(CPM_LCR);
			*(volatile unsigned int *)0xb0000030 = 0xf | (1 << 29);

			lcr &=~ 0x3;
			lcr |= 0xf << 28; // low power mode: SLEEP
			cpm_outl(lcr, CPM_LCR);
			while (!(((cpm_inl(CPM_LCR) & 0xf000000) >> 24) == 0xf))
				printk("isp vpu:%x\n",cpm_inl(CPM_LCR));

		}

		/* codec */
		*(volatile unsigned int *)0xb0020000 = 0;

		/* memory power down */
		{

			*(volatile unsigned int *)0xb00000f8 = 0x0ffffff3;
			*(volatile unsigned int *)0xb00000fc = 0x3c0;

		}
		cpm_outl(gate0, CPM_CLKGR);
		cpm_outl(gate1, CPM_CLKGR1);
	}
#endif


	return 0;
}

static int soc_pm_idle(void)
{
	unsigned int lcr = cpm_inl(CPM_LCR);
	unsigned int opcr = cpm_inl(CPM_OPCR);

	lcr &=~ 0x3; // low power mode: IDLE
	cpm_outl(lcr, CPM_LCR);

	opcr &= ~(1 << 30);
	opcr &= ~(1 << 31);
	cpm_outl(opcr, CPM_OPCR);

	return 0;
}

#ifdef X2000_IDLE_PD
static int soc_pm_idle_pd(void)
{
	unsigned int lcr = cpm_inl(CPM_LCR);
	unsigned int opcr = cpm_inl(CPM_OPCR);

	lcr &=~ 0x3; // low power mode: IDLE
	lcr |= 2;
	cpm_outl(lcr, CPM_LCR);

	opcr &= ~ (1<<31);
	opcr |= (1 << 30);
	opcr &= ~(1 << 26);	//l2c power down
	opcr |= 1 << 2; // select RTC clk
	cpm_outl(opcr, CPM_OPCR);

	return 0;
}
#endif

static int soc_pm_sleep(void)
{
	unsigned int lcr = cpm_inl(CPM_LCR);
	unsigned int opcr = cpm_inl(CPM_OPCR);

	lcr &=~ 0x3;
	lcr |= 1 << 0; // low power mode: SLEEP
	cpm_outl(lcr, CPM_LCR);

	opcr &= ~(1 << 31);
	opcr |= (1 << 30);
	opcr &= ~(1 << 26); //L2C power down
	opcr |= (1 << 21); // cpu 32k ram retention.
	opcr |= (1 << 3); // power down CPU
	opcr &= ~(1 << 4); // exclk disable;
	opcr |= (1 << 2); // select RTC clk
	opcr |= (1 << 22);
	opcr |= (1 << 20);
	cpm_outl(opcr, CPM_OPCR);

	return 0;
}

static int soc_post_wakeup(void)
{
	unsigned int lcr = cpm_inl(CPM_LCR);
	lcr &= ~0x3; // low power mode: IDLE
	cpm_outl(lcr, CPM_LCR);

	printk("post wakeup!\n");

#ifdef PD_OTHER_MODULE
	*(volatile unsigned int *)0xb00000f8 = 0;
	*(volatile unsigned int *)0xb00000fc = 0;
#endif

	clk_gate0 &= ~(1 << 20);
	cpm_outl(clk_gate0, CPM_CLKGR);
	cpm_outl(clk_gate1, CPM_CLKGR1);

	{
		/* after power down cpu by set PD in OPCR, resume cpu's frequency and L2C's freq */
		unsigned int val;

		/* change disable */
		val = cpm_inl(CPM_CPCCR);
		val &= ~(1 << 22);
		cpm_outl(val, CPM_CPCCR);

		/* resume cpu_div in CPCCR */
		val &= ~0xf;
		val |= cpu_div;
		cpm_outl(val, CPM_CPCCR);

		/* change enable */
		val |= (1 << 22);
		cpm_outl(val, CPM_CPCCR);

		while (cpm_inl(CPM_CPCSR) & 1);
	}
	return 0;
}

static noinline void cpu_resume_bootup(void)
{
	TCSM_PCHAR('X');
	/* set reset entry */
	*(volatile unsigned int *)0xb2200f00 = 0xbfc00000;

	__asm__ volatile(
		".set push	\n\t"
		".set mips32r2	\n\t"
		"move $29, %0	\n\t"
		"jr.hb   %1	\n\t"
		"nop		\n\t"
		".set pop	\n\t"
		:
		:"r" (SLEEP_CPU_RESUME_SP), "r"(SLEEP_CPU_RESUME_TEXT)
		:
		);

}
#define reg_ddr_phy(x)   (*(volatile unsigned int *)(0xb3011000 + ((x) << 2)))
static noinline void cpu_resume(void)
{
	unsigned int cpu_id;
	unsigned int ddrc_ctrl;

	TCSM_PCHAR('R');

	if (*g_state == PM_SUSPEND_MEM) {
		int tmp;

		/* enable pll */
		tmp = reg_ddr_phy(0x21);
		tmp &= ~(1 << 1);
		reg_ddr_phy(0x21) = tmp;

		while (!(reg_ddr_phy(0x32) & 0x8))
			serial_put_hex(reg_ddr_phy(0x32));

		/* dfi_init_start = 0, wait dfi_init_complete */
		*(volatile unsigned int *)0xb3012000 &= ~(1 << 3);
		while(!(*(volatile unsigned int *)0xb3012004 & 0x1));

		/* bufferen_core = 1 */
		tmp = rtc_read_reg(0xb0003048);
		tmp |= (1 << 21);
		rtc_write_reg(0xb0003048, tmp);

		ddrc_ctrl = ddr_readl(DDRC_CTRL);
		ddrc_ctrl &= ~(1<<5);
		ddr_writel(ddrc_ctrl, DDRC_CTRL);

		while(ddr_readl(DDRC_STATUS) & (1<<2));

		TCSM_PCHAR('1');

		ddr_writel(1, DDRC_AUTOSR_EN);
		while(ddr_readl(DDRC_STATUS) & (1<<2));

		TCSM_PCHAR('2');

		ddrp_auto_calibration();

		TCSM_PCHAR('3');
	}

	cpu_id = read_c0_ebase() & 0x3ff;

	if (cpu_id == 0) {
		_regs_stack = _regs_stack_0;
	} else if (cpu_id == 1) {
		_regs_stack = _regs_stack_1;
	}

	TCSM_PCHAR('4');

	__asm__ volatile(
		".set push	\n\t"
		".set mips32r2	\n\t"
		"jr.hb %0	\n\t"
		"nop		\n\t"
		".set pop 	\n\t"
		:
		: "r" (restore_goto)
		:
		);
}


static noinline void cpu_sleep(void)
{


	cpu_div = cpm_inl(CPM_CPCCR) & 0xf;

	blast_dcache32();
	blast_scache64();
	__sync();
	__fast_iob();

	{
		/* before power down cpu by set PD in OPCR, reduce cpu's frequency as the same as L2C's freq */
		unsigned int val, div;

		/* change disable */
		val = cpm_inl(CPM_CPCCR);
		val &= ~(1 << 22);
		cpm_outl(val, CPM_CPCCR);

		/* div cpu = div l2c */
		div = val & (0xf << 4);
		val &= ~0xf;
		val |= (div >> 4);
		cpm_outl(val, CPM_CPCCR);

		/* change enable */
		val |= (1 << 22);
		cpm_outl(val, CPM_CPCCR);

		while (cpm_inl(CPM_CPCSR) & 1);
	}

	TCSM_PCHAR('D');

	if (*g_state == PM_SUSPEND_MEM) {
		unsigned int tmp;
		unsigned int ddrc_ctrl;

		ddr_writel(0, DDRC_AUTOSR_EN);
		tmp = *(volatile unsigned int *)0xa0000000;

		/* DDR self refresh, */
		ddrc_ctrl = ddr_readl(DDRC_CTRL);
		ddrc_ctrl |= 1 << 5;
		ddr_writel(ddrc_ctrl, DDRC_CTRL);
		while(!(ddr_readl(DDRC_STATUS) & (1<<2)));


		/* bufferen_core = 0 */
		tmp = rtc_read_reg(0xb0003048);
		tmp &= ~(1 << 21);
		rtc_write_reg(0xb0003048, tmp);


		/* dfi_init_start = 1 */
		*(volatile unsigned int *)0xb3012000 |= (1 << 3);

		{
			int i;
			for (i = 0; i < 4; i++) {

				__asm__ volatile("ssnop\t\n");
				__asm__ volatile("ssnop\t\n");
				__asm__ volatile("ssnop\t\n");
				__asm__ volatile("ssnop\t\n");
				__asm__ volatile("ssnop\t\n");
				__asm__ volatile("ssnop\t\n");
				__asm__ volatile("ssnop\t\n");
				__asm__ volatile("ssnop\t\n");
			}
		}

		/* disable pll */
		reg_ddr_phy(0x21) |= (1 << 1);
	}

	TCSM_PCHAR('W');
	__asm__ volatile(
		".set push	\n\t"
		".set mips32r2	\n\t"
		"wait		\n\t"
		"nop		\n\t"
		"nop		\n\t"
		"nop		\n\t"
		".set pop	\n\t"
	);

	TCSM_PCHAR('N');

	TCSM_PCHAR('\r');
	TCSM_PCHAR('\n');
	TCSM_PCHAR('?');
	TCSM_PCHAR('?');
	TCSM_PCHAR('?');
	TCSM_PCHAR('\r');
	TCSM_PCHAR('\n');

	__asm__ volatile(
		".set push	\n\t"
		".set mips32r2	\n\t"
		"jr.hb %0	\n\t"
		"nop		\n\t"
		".set pop 	\n\t"
		:
		: "r" (SLEEP_CPU_RESUME_BOOTUP_TEXT)
		:
		);
	TCSM_PCHAR('F');
}


int x2000_pm_enter(suspend_state_t state)
{
	unsigned int cpu_id;

	printk("x2000 pm enter!!\n");
	*g_state = state;
#ifdef DEBUG_PM
	printk("g_state addr = %p  val = %d\n", g_state, *g_state);
#endif

	load_func_to_tcsm((unsigned int *)SLEEP_CPU_RESUME_BOOTUP_TEXT, (unsigned int *)cpu_resume_bootup, SLEEP_CPU_RESUME_BOOTUP_LEN);
	load_func_to_tcsm((unsigned int *)SLEEP_CPU_RESUME_TEXT, (unsigned int *)cpu_resume, SLEEP_CPU_RESUME_LEN);
	load_func_to_tcsm((unsigned int *)SLEEP_CPU_SLEEP_TEXT, (unsigned int *)cpu_sleep, SLEEP_CPU_SLEEP_LEN);

	soc_pm_config();
	if (state == PM_SUSPEND_STANDBY) {
#ifdef X2000_IDLE_PD
		soc_pm_idle_pd();
#else
		soc_pm_idle();
#endif
	} else if (state == PM_SUSPEND_MEM) {
		soc_pm_sleep();
	} else {
		printk("WARNING : unsupport pm suspend state\n");
	}
#ifdef DEBUG_PM
	printk("LCR: %08x\n", cpm_inl(CPM_LCR));
	printk("OPCR: %08x\n", cpm_inl(CPM_OPCR));
#endif

	cpu_id = read_c0_ebase() & 0x1ff;

	/* set reset entry */
	*(volatile unsigned int *)0xb2200f00 = SLEEP_CPU_RESUME_BOOTUP_TEXT;

	printk("cpu_id = %d\n", cpu_id);
	if (cpu_id == 0) {
		_regs_stack = _regs_stack_0;
	} else if (cpu_id == 1) {
		_regs_stack = _regs_stack_1;
	}


	mb();
	save_goto((unsigned int)SLEEP_CPU_SLEEP_TEXT);
	mb();

	soc_post_wakeup();

	return 0;
}

static int x2000_pm_begin(suspend_state_t state)
{
	printk("x2000 suspend begin\n");
	return 0;
}

static void x2000_pm_end(void)
{
	printk("x2000 pm end!\n");
}

static int ingenic_pm_valid(suspend_state_t state)
{
	switch (state) {
		case PM_SUSPEND_ON:
		case PM_SUSPEND_STANDBY:
		case PM_SUSPEND_MEM:
			return 1;

		default:
			return 0;
	}
}
static const struct platform_suspend_ops x2000_pm_ops = {
	.valid		= ingenic_pm_valid,
	.begin		= x2000_pm_begin,
	.enter		= x2000_pm_enter,
	.end		= x2000_pm_end,
};

/*
 * Initialize suspend interface
 */
static int __init pm_init(void)
{

	suspend_set_ops(&x2000_pm_ops);


	return 0;
}

late_initcall(pm_init);
