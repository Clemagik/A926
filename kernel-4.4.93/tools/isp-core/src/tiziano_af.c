#include <linux/kernel.h>
#include <linux/irqreturn.h>
#include <linux/dma-mapping.h>

#include "fix_point_calc.h"
#include "tiziano_map.h"
#include "../inc/tiziano_core.h"
#include "../inc/tiziano_sys.h"
#include "tiziano_core.h"
#include "tiziano_params.h"
#include "../inc/tiziano_isp.h"

#if 0
static tisp_af_attr af_attr;
static unsigned char af_set_trig = 0;
#endif

#if 0
/*****************AF Param********************/
typedef struct TizianoIsp_Af_Static_Param {
	unsigned int *a_af_zone;
	unsigned int *a_af_array_fird0;
	unsigned int *a_af_array_fird1;
	unsigned int *a_af_array_iird0;
	unsigned int *a_af_array_iird1;
	unsigned int *a_af_array_y_sum;
	unsigned int *a_af_array_high_luma_cnt;
	unsigned int *a_af_weight;
	unsigned int *a_af_fv;
	unsigned int *a_af_fvwmean;
	uint32_t *a_af_tilt;
	uint32_t *a_AfPointPos;
	uint32_t width;
	uint32_t height;
} Isp_Af_Static_Param;
Isp_Af_Static_Param IspAfStaticParam;
#endif
#if 0
uint32_t af_array_fird0[225], af_array_fird1[225], af_array_iird0[225], af_array_iird1[225], af_array_y_sum[225], af_array_high_luma_cnt[225];

/******Static Param********/
static unsigned int stAFParam_Zone[36] = {
	3, 8, 1, 8,   //v_start, zone_v_num, h_start, zone_h_num
	240,240,240,240,240,240,240,224, 240,240,240,240,240,240,240, //h_zone pixel number
	134,134,134,134,134,134,134,131, 134,134,134,134,134,134,134, //v_zone pixel number
	1,0           //af_freq, af_stop
};
static unsigned int stAFParam_ThresEnable[13] = {
	1, 1, 1, 1,                          //IIR Order En
	200,                                 //HIGH_LUMA_TH
	0, 0, 0, 0,                          //LDG_EN: f0,f1,i0,i1
	0, 0, 0, 0                           //COR_EN: f0,f1,i0,i1
};

//V: Complement,2.9bit
//H: Complement,3.8bit
//Ldg: Thlow1(8bit),  Thlow2(8bit),  SlpLow(4.4bit),  GainLow(8bit),
//     ThHigh1(8bit), ThHigh2(8bit), SlpHigh(4.4bit), GainHigh(8bit)
//Coring: ThLow(9bit), ThHigh(9bit), Slp(4.4bit), Lmt(8bit)
static unsigned int stAFParam_FIR0_V[5] = {2026, 128, 300, 128, 2026};
static unsigned int stAFParam_FIR0_Ldg[8] = {0, 8, 0, 255, 235, 255, 176, 35};
static unsigned int stAFParam_FIR0_Coring[4]= {255,510,16,255};
static unsigned int stAFParam_FIR1_V[5] = {2027, 1930, 317, 1930, 2027};
static unsigned int stAFParam_FIR1_Ldg[8] = {0, 36, 0, 255, 220, 240, 160, 35};
static unsigned int stAFParam_FIR1_Coring[4]= {255,510,16,255};
static unsigned int stAFParam_IIR0_H[10] = {38, 256, 512, 256, 214, 1939, 256, 256, 83, 86};
static unsigned int stAFParam_IIR0_Ldg[8] = {0, 30, 0, 255, 230, 245, 240, 15};
static unsigned int stAFParam_IIR0_Coring[4]= {255,510,16,255};
static unsigned int stAFParam_IIR1_H[10] = {238, 256, 1536, 256, 474, 1826, 256, 1792, 222, 239};
static unsigned int stAFParam_IIR1_Ldg[8] = {0, 25, 0, 255, 220, 250, 128, 15};
static unsigned int stAFParam_IIR1_Coring[4]= {255,510,16,255};

/******Algorithm Param********/
static uint32_t AFParam_PointPos[2] = {10, 0};
static uint32_t AFParam_Fv_Alt = 0;
/* Alpha,Belta,Delta,Theta,BlendShift */
static uint32_t AFParam_Tilt[5] = {19, 58, 42, 22, 6}; //[0 ~ 1] * 64 6bit point float

static unsigned int AFParam_FvWmean[15] = {512, 512, 512, 512, 512, 512, 512, 512, 512, 512, 512, 512, 512, 512, 512};
static unsigned int AFParam_Fv[3]={0,0,0};
/* Fv1-->Alpha*h1+(1-Alpha)*v1 */
/* Fv2-->Belta*h2+(1-Belta)*v2 */
/* Fv -->Delta*Fv1+Theta*Fv2 */

static unsigned int AFWeight_Param[225]={
	2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
	2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
	2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
	2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
	2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
	2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
	2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
	2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
	2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
	2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
	2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
	2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
	2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
	2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
	2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2
};
#endif

/************************AF ALGORITHM************************/
void tisp_af_get_statistics(tisp_af_t *af, uint32_t *img, uint32_t v_zone, uint32_t h_zone)
{
	int i, j;
	/* printf("v_zone %d  h_zone %d\n",v_zone,h_zone ); */

	for (i = 0; i < v_zone; i++) {
		for (j = 0; j < h_zone; j++) {
			af->af_array_fird0[i * 15 + j] = *(img + i*h_zone*4 + j*4)
				& 0x3fffff;
			af->af_array_fird1[i * 15 + j] = (((*(img + i * h_zone * 4 + j * 4 + 1)) & 0xfff) << 10) | (*(img + i * h_zone * 4 + j * 4) >> 22);
			af->af_array_iird0[i * 15 + j] = (((*(img + i * h_zone * 4 + j * 4 + 1)) & 0xfffff000) >> 12) | (((*(img + i * h_zone * 4 + j * 4 + 2)) & 0x3) << 20);
			af->af_array_iird1[i * 15 + j] = (((*(img + i * h_zone * 4 + j * 4 + 2)) & 0xfffffc) >> 2) ;
			af->af_array_y_sum[i * 15 + j] = (((*(img + i * h_zone * 4 + j * 4 + 2)) & 0xff000000) >> 24) | (((*(img + i * h_zone * 4 + j * 4 + 3)) & 0x7fff) << 8);
			af->af_array_high_luma_cnt[i * 15 + j] = (((*(img + i * h_zone * 4 + j * 4 + 3)) & 0x3fff1000) >> 15);
		}
	}

#if 0
	ISP_ERROR("3\n");
	for (i = 0; i < 15; i++){
		for (j = 0; j < 15; j++){
			//ISP_ERROR(" %d ", af_array_fird0[i*15+j]);
			//ISP_ERROR(" %d ", af_array_fird1[i*15+j]);
			ISP_ERROR(" %d ", af_array_iird0[i*15+j]);
			//ISP_ERROR(" %d ", af_array_iird1[i*15+j]);
			//ISP_ERROR(" %d ", af_array_y_sum[i*15+j]);
			//ISP_ERROR(" %d ", af_array_high_luma_cnt[i*15+j]);
		}
		ISP_ERROR(" \n ");
	}
#endif
#if 0
	//running = 0;
	FILE *fp_f0 = NULL;
	FILE *fp_f1 = NULL;
	FILE *fp_i0 = NULL;
	FILE *fp_i1 = NULL;
	FILE *fp_y  = NULL;
	FILE *fp_hl = NULL;

	if((fp_f0 = fopen("fpga_af_fird0.dat","wb")) == NULL){
		printf("open save file error! \n");
	}
	if((fp_f1 = fopen("fpga_af_fird1.dat","wb")) == NULL){
		printf("open save file error! \n");
	}
	if((fp_i0 = fopen("fpga_af_iird0.dat","wb")) == NULL){
		printf("open save file error! \n");
	}
	if((fp_i1 = fopen("fpga_af_iird1.dat","wb")) == NULL){
		printf("open save file error! \n");
	}
	if((fp_y = fopen("fpga_af_y_sum.dat","wb")) == NULL){
		printf("open save file error! \n");
	}
	if((fp_hl = fopen("fpga_af_high_luma.dat","wb")) == NULL){
		printf("open save file error! \n");
	}
	char ch[9];
	for (i = 0; i < 15*15; i++) {
		snprintf(ch, sizeof(ch), "%06x,\n", af_array_fird0[i]);
		fwrite(ch, sizeof(ch), 1, fp_f0);
		snprintf(ch, sizeof(ch), "%06x,\n", af_array_fird1[i]);
		fwrite(ch, sizeof(ch), 1, fp_f1);
		snprintf(ch, sizeof(ch), "%06x,\n", af_array_iird0[i]);
		fwrite(ch, sizeof(ch), 1, fp_i0);
		snprintf(ch, sizeof(ch), "%06x,\n", af_array_iird1[i]);
		fwrite(ch, sizeof(ch), 1, fp_i1);
		snprintf(ch, sizeof(ch), "%06x,\n", af_array_y_sum[i]);
		fwrite(ch, sizeof(ch), 1, fp_y);
		snprintf(ch, sizeof(ch), "%06x,\n", af_array_high_luma_cnt[i]);
		fwrite(ch, sizeof(ch), 1, fp_hl);

	}
	fclose(fp_f0);
	fclose(fp_f1);
	fclose(fp_i0);
	fclose(fp_i1);
	fclose(fp_y);
	fclose(fp_hl);
}
#endif
}

//unsigned int FvWmean_num=0;
void Tiziano_af_fpga(tisp_af_t *af/*, Isp_Af_Static_Param IspAfStaticParam*/)
{
 	unsigned int v_zone = af->IspAfStaticParam.a_af_zone[1];
	unsigned int h_zone = af->IspAfStaticParam.a_af_zone[3];
	unsigned int *af_fird0 = af->IspAfStaticParam.a_af_array_fird0;
	unsigned int *af_fird1 = af->IspAfStaticParam.a_af_array_fird1;
	unsigned int *af_iird0 = af->IspAfStaticParam.a_af_array_iird0;
	unsigned int *af_iird1 = af->IspAfStaticParam.a_af_array_iird1;
	//unsigned int *af_y_sum = IspAfStaticParam.a_af_array_y_sum;
	//unsigned int *af_high_luma_cnt = IspAfStaticParam.a_af_array_high_luma_cnt;
	unsigned int *AFWeight = af->IspAfStaticParam.a_af_weight;
	unsigned int *Fv = af->IspAfStaticParam.a_af_fv;
	unsigned int *FvWmean = af->IspAfStaticParam.a_af_fvwmean;
	uint32_t Alpha = af->IspAfStaticParam.a_af_tilt[0];
	uint32_t Belta = af->IspAfStaticParam.a_af_tilt[1];
	uint32_t Delta = af->IspAfStaticParam.a_af_tilt[2];
	uint32_t Theta = af->IspAfStaticParam.a_af_tilt[3];
	unsigned int BlendShift = af->IspAfStaticParam.a_af_tilt[4];
	//uint32_t *AFPointPos = IspAfStaticParam.a_AfPoinstPos;
	unsigned int i,j;
	unsigned int SumV1, SumV2, SumH1, SumH2;
	unsigned int Fv1_n, Fv2_n;
	unsigned int SumWeightFv1 = 0;
	unsigned int SumWeightFv2 = 0;
	unsigned int SumWeight   = 0;
	unsigned int WeiSumFv1, WeiSumFv2, WeiSumFv; //weighted average
	unsigned int AriSumFv1, AriSumFv2, AriSumFv; //arithmetic Average

	for (i = 0;i < v_zone; i++){
		for (j = 0; j < h_zone; j++) {
			SumH1 = af_iird0[i * 15 + j];
			SumH2 = af_iird1[i * 15 + j];
			SumV1 = af_fird0[i * 15 + j];
			SumV2 = af_fird1[i * 15 + j];

			Fv1_n = fix_point_mult2_32(BlendShift, SumH1<<BlendShift, Alpha) +
				fix_point_mult2_32(BlendShift, SumV1<<BlendShift,
						   ((uint32_t)1<<BlendShift)-Alpha);
			Fv2_n = fix_point_mult2_32(BlendShift, SumH2<<BlendShift, Belta) +
				fix_point_mult2_32(BlendShift, SumV2<<BlendShift,
						   ((uint32_t)1<<BlendShift)-Belta);

			SumWeightFv1 += AFWeight[i * 15 + j] * (Fv1_n>>BlendShift);
			SumWeightFv2 += AFWeight[i * 15 + j] * (Fv2_n>>BlendShift);
			SumWeight += AFWeight[i * 15 + j];
		}
	}

	WeiSumFv1 = SumWeightFv1 / SumWeight;
	WeiSumFv2 = SumWeightFv2 / SumWeight;
	WeiSumFv = (fix_point_mult2_32(BlendShift, WeiSumFv1<<BlendShift, Delta) +
		 fix_point_mult2_32(BlendShift, WeiSumFv2<<BlendShift, Theta))>>BlendShift;

	AriSumFv1 = SumWeightFv1 / 8;
	AriSumFv2 = SumWeightFv2 / 8;
	AriSumFv = (fix_point_mult2_32(BlendShift, AriSumFv1<<BlendShift, Delta) +
		 fix_point_mult2_32(BlendShift, AriSumFv2<<BlendShift, Theta))>>BlendShift;

	for (i = 0; i < 14; i++) {
		*(FvWmean+i) = *(FvWmean+i+1);
	}
	*(FvWmean + 14) = WeiSumFv;

#if 0
	if (FvWmean_num < 15) {
		FvWmean_num++;
	} else {
		/* FvWmean */
		WeiSumFv = 0;
		SumFvWeight = 0;
		for (i = 0; i < 15; i++) {
			WeiSumFv += (i / 2 + 1) * (*(FvWmean + i));
			SumFvWeight += (i / 2 + 1);
		}
		WeiSumFv = WeiSumFv / SumFvWeight;
	}
#endif
	*Fv = WeiSumFv1 * h_zone * v_zone;
	*(Fv + 1) = WeiSumFv2 * h_zone * v_zone;
	*(Fv + 2) = WeiSumFv * h_zone * v_zone;
	af->AFParam_Fv_Alt = AriSumFv;
	/*printk("fv1 %d  fv2 %d  fv %d fv_alt1 %d  fv_alt2 %d fv_alt %d\n", *Fv, *(Fv+1), *(Fv+2),AriSumFv1,AriSumFv2,AFParam_Fv_Alt);*/
}

int tisp_af_process_impl(tisp_af_t *af)
{
	/* IspAfStaticParam */
	af->IspAfStaticParam.a_af_zone = 	af->stAFParam_Zone;
	af->IspAfStaticParam.a_af_array_fird0 = af->af_array_fird0;
	af->IspAfStaticParam.a_af_array_fird1 = af->af_array_fird1;
	af->IspAfStaticParam.a_af_array_iird0 = af->af_array_iird0;
	af->IspAfStaticParam.a_af_array_iird1 = af->af_array_iird1;
	af->IspAfStaticParam.a_af_array_y_sum = af->af_array_y_sum;
	af->IspAfStaticParam.a_af_array_high_luma_cnt= af->af_array_high_luma_cnt;
	af->IspAfStaticParam.a_af_weight = af->AFWeight_Param;
	af->IspAfStaticParam.a_af_fv = af->AFParam_Fv;
	af->IspAfStaticParam.a_af_fvwmean = af->AFParam_FvWmean;
	af->IspAfStaticParam.a_af_tilt = af->AFParam_Tilt;
	af->IspAfStaticParam.a_AfPointPos = af->AFParam_PointPos;

	//Tiziano_af_fpga(af, IspAfStaticParam);
	Tiziano_af_fpga(af);

	return 0;
}

int af_interrupt_static(void *data)
{
	int num = 0;

	tisp_af_t *af = (tisp_af_t *)data;
	tisp_core_t *core = (tisp_core_t *)af->core;
	tisp_info_t * tispinfo = &core->tispinfo;

	num = system_reg_read(((tisp_core_t *)af->core)->priv_data, AF_STAT_ADDR_DMA_INFO);
	dma_cache_sync(NULL, (void*)tispinfo->buf_af_vaddr+num*4*1024, 4096, DMA_BIDIRECTIONAL);
	tisp_af_get_statistics(af, (void*)tispinfo->buf_af_vaddr+num*4*1024, af->stAFParam_Zone[1], af->stAFParam_Zone[3]);
	/* ISP_ERROR("^_^af interrupt^_^ num is %d \n",num); */

	tisp_af_process_impl(af);

	return IRQ_HANDLED;
}

int tiziano_af_params_refresh(tisp_af_t *af)
{
	memcpy(af->stAFParam_Zone, tparams.params_data.TISP_PARAM_AF_ZONE, sizeof(tparams.params_data.TISP_PARAM_AF_ZONE));
	memcpy(af->stAFParam_ThresEnable, tparams.params_data.TISP_PARAM_AF_TH_EN, sizeof(tparams.params_data.TISP_PARAM_AF_TH_EN));
	memcpy(af->stAFParam_FIR0_V, tparams.params_data.TISP_PARAM_AF_FIR0_V, sizeof(tparams.params_data.TISP_PARAM_AF_FIR0_V));
	memcpy(af->stAFParam_FIR0_Ldg, tparams.params_data.TISP_PARAM_AF_FIR0_LDG, sizeof(tparams.params_data.TISP_PARAM_AF_FIR0_LDG));
	memcpy(af->stAFParam_FIR0_Coring, tparams.params_data.TISP_PARAM_AF_FIR0_CORING, sizeof(tparams.params_data.TISP_PARAM_AF_FIR0_CORING));
	memcpy(af->stAFParam_FIR1_V, tparams.params_data.TISP_PARAM_AF_FIR1_V, sizeof(tparams.params_data.TISP_PARAM_AF_FIR1_V));
	memcpy(af->stAFParam_FIR1_Ldg, tparams.params_data.TISP_PARAM_AF_FIR1_LDG, sizeof(tparams.params_data.TISP_PARAM_AF_FIR1_LDG));
	memcpy(af->stAFParam_FIR1_Coring, tparams.params_data.TISP_PARAM_AF_FIR1_CORING, sizeof(tparams.params_data.TISP_PARAM_AF_FIR1_CORING));
	memcpy(af->stAFParam_IIR0_H, tparams.params_data.TISP_PARAM_AF_IIR0_H, sizeof(tparams.params_data.TISP_PARAM_AF_IIR0_H));
	memcpy(af->stAFParam_IIR0_Ldg, tparams.params_data.TISP_PARAM_AF_IIR0_LDG, sizeof(tparams.params_data.TISP_PARAM_AF_IIR0_LDG));
	memcpy(af->stAFParam_IIR0_Coring, tparams.params_data.TISP_PARAM_AF_IIR0_CORING, sizeof(tparams.params_data.TISP_PARAM_AF_IIR0_CORING));
	memcpy(af->stAFParam_IIR1_H, tparams.params_data.TISP_PARAM_AF_IIR1_H, sizeof(tparams.params_data.TISP_PARAM_AF_IIR1_H));
	memcpy(af->stAFParam_IIR1_Ldg, tparams.params_data.TISP_PARAM_AF_IIR1_LDG, sizeof(tparams.params_data.TISP_PARAM_AF_IIR1_LDG));
	memcpy(af->stAFParam_IIR1_Coring, tparams.params_data.TISP_PARAM_AF_IIR1_CORING, sizeof(tparams.params_data.TISP_PARAM_AF_IIR1_CORING));
	memcpy(af->AFParam_PointPos, tparams.params_data.TISP_PARAM_AF_POINTPOS, sizeof(tparams.params_data.TISP_PARAM_AF_POINTPOS));
	memcpy(af->AFParam_Tilt, tparams.params_data.TISP_PARAM_AF_TILT, sizeof(tparams.params_data.TISP_PARAM_AF_TILT));
	memcpy(af->AFParam_FvWmean, tparams.params_data.TISP_PARAM_AF_FVW_MEAN, sizeof(tparams.params_data.TISP_PARAM_AF_FVW_MEAN));
	memcpy(af->AFParam_Fv, tparams.params_data.TISP_PARAM_AF_FV, sizeof(tparams.params_data.TISP_PARAM_AF_FV));
	memcpy(af->AFWeight_Param, tparams.params_data.TISP_PARAM_AF_WEIGHT, sizeof(tparams.params_data.TISP_PARAM_AF_WEIGHT));

	return 0;
}

#define DUMP_AF_REG(name) ISP_ERROR("0x%x: 0x%xx\n", name, system_reg_read(((tisp_core_t *)af->core)->priv_data, name));
void tiziano_af_dump(tisp_af_t *af)
{
	ISP_ERROR("-----af regs dump-----\n");
	DUMP_AF_REG(AF_STAT_ADDR_ZONE_NUM_START);
	DUMP_AF_REG(AF_STAT_ADDR_HOR_ZONE_SIZE_14);
	DUMP_AF_REG(AF_STAT_ADDR_HOR_ZONE_SIZE_58);
	DUMP_AF_REG(AF_STAT_ADDR_HOR_ZONE_SIZE_912);
	DUMP_AF_REG(AF_STAT_ADDR_HOR_ZONE_SIZE_1315);
	DUMP_AF_REG(AF_STAT_ADDR_VER_ZONE_SIZE_14);
	DUMP_AF_REG(AF_STAT_ADDR_VER_ZONE_SIZE_58);
	DUMP_AF_REG(AF_STAT_ADDR_VER_ZONE_SIZE_912);
	DUMP_AF_REG(AF_STAT_ADDR_VER_ZONE_SIZE_1315);
	DUMP_AF_REG(AF_STAT_ADDR_FREQ_IIR_EN_LUM_TH);
	DUMP_AF_REG(AF_STAT_ADDR_LDG_COR_EN);
	DUMP_AF_REG(AF_STAT_ADDR_FIR0_G1_G2);
	DUMP_AF_REG(AF_STAT_ADDR_FIR0_G3_G4);
	DUMP_AF_REG(AF_STAT_ADDR_FIR0_G5);
	DUMP_AF_REG(AF_STAT_ADDR_FIR1_G1_G2);
	DUMP_AF_REG(AF_STAT_ADDR_FIR1_G3_G4);
	DUMP_AF_REG(AF_STAT_ADDR_FIR1_G5);
	DUMP_AF_REG(AF_STAT_ADDR_IIR0_G1_G3);
	DUMP_AF_REG(AF_STAT_ADDR_IIR0_G4_G5);
	DUMP_AF_REG(AF_STAT_ADDR_IIR0_G6_G8);
	DUMP_AF_REG(AF_STAT_ADDR_IIR0_G9_G10);
	DUMP_AF_REG(AF_STAT_ADDR_IIR1_G1_G3);
	DUMP_AF_REG(AF_STAT_ADDR_IIR1_G4_G5);
	DUMP_AF_REG(AF_STAT_ADDR_IIR1_G6_G8);
	DUMP_AF_REG(AF_STAT_ADDR_IIR1_G9_G10);
	DUMP_AF_REG(AF_STAT_ADDR_LDG_FIR0_LOW);
	DUMP_AF_REG(AF_STAT_ADDR_LDG_FIR0_HIGH);
	DUMP_AF_REG(AF_STAT_ADDR_LDG_FIR1_LOW);
	DUMP_AF_REG(AF_STAT_ADDR_LDG_FIR1_HIGH);
	DUMP_AF_REG(AF_STAT_ADDR_LDG_IIR0_LOW);
	DUMP_AF_REG(AF_STAT_ADDR_LDG_IIR0_HIGH);
	DUMP_AF_REG(AF_STAT_ADDR_LDG_IIR1_LOW);
	DUMP_AF_REG(AF_STAT_ADDR_LDG_IIR1_HIGH);
	DUMP_AF_REG(AF_STAT_ADDR_COR_F0_TH_LH);
	DUMP_AF_REG(AF_STAT_ADDR_COR_F0_SLP_LIM);
	DUMP_AF_REG(AF_STAT_ADDR_COR_F1_TH_LH);
	DUMP_AF_REG(AF_STAT_ADDR_COR_F1_SLP_LIM);
	DUMP_AF_REG(AF_STAT_ADDR_COR_I0_TH_LH);
	DUMP_AF_REG(AF_STAT_ADDR_COR_I0_SLP_LIM);
	DUMP_AF_REG(AF_STAT_ADDR_COR_I1_TH_LH);
	DUMP_AF_REG(AF_STAT_ADDR_COR_I1_SLP_LIM);
}

int tiziano_af_set_regs(tisp_af_t *af)
{
	int32_t i;

	for (i = 0; i<af->stAFParam_Zone[3]; i++) {
		af->stAFParam_Zone[4 + i] = (af->IspAfStaticParam.width - 15) / af->stAFParam_Zone[3];
	}
	for (i = 0; i < af->stAFParam_Zone[1]; i++) {
		af->stAFParam_Zone[19 + i] = (af->IspAfStaticParam.height - 3) / af->stAFParam_Zone[1];
	}

	system_reg_write(((tisp_core_t *)af->core)->priv_data, AF_STAT_ADDR_ZONE_NUM_START,
			af->stAFParam_Zone[3] << 28 | af->stAFParam_Zone[2] << 16 |
			af->stAFParam_Zone[1] << 12 | af->stAFParam_Zone[0]);
	system_reg_write(((tisp_core_t *)af->core)->priv_data, AF_STAT_ADDR_HOR_ZONE_SIZE_14,
			af->stAFParam_Zone[7] << 24 | af->stAFParam_Zone[6] << 16 |
			af->stAFParam_Zone[5] << 8 | af->stAFParam_Zone[4]);
	system_reg_write(((tisp_core_t *)af->core)->priv_data, AF_STAT_ADDR_HOR_ZONE_SIZE_58,
			af->stAFParam_Zone[11] << 24 | af->stAFParam_Zone[10] << 16 |
			af->stAFParam_Zone[9] << 8 | af->stAFParam_Zone[8]);
	system_reg_write(((tisp_core_t *)af->core)->priv_data, AF_STAT_ADDR_HOR_ZONE_SIZE_912,
			af->stAFParam_Zone[15] << 24 | af->stAFParam_Zone[14] << 16 |
			af->stAFParam_Zone[13] << 8 | af->stAFParam_Zone[12]);
	system_reg_write(((tisp_core_t *)af->core)->priv_data, AF_STAT_ADDR_HOR_ZONE_SIZE_1315,
			af->stAFParam_Zone[18] << 16 | af->stAFParam_Zone[17] << 8 |
			af->stAFParam_Zone[16]);
	system_reg_write(((tisp_core_t *)af->core)->priv_data, AF_STAT_ADDR_VER_ZONE_SIZE_14,
			af->stAFParam_Zone[22] << 24 | af->stAFParam_Zone[21] << 16 |
			af->stAFParam_Zone[20] << 8 | af->stAFParam_Zone[19]);
	system_reg_write(((tisp_core_t *)af->core)->priv_data, AF_STAT_ADDR_VER_ZONE_SIZE_58,
			af->stAFParam_Zone[26] << 24 | af->stAFParam_Zone[25] << 16 |
			af->stAFParam_Zone[24] << 8 | af->stAFParam_Zone[23]);
	system_reg_write(((tisp_core_t *)af->core)->priv_data, AF_STAT_ADDR_VER_ZONE_SIZE_912,
			af->stAFParam_Zone[30] << 24 | af->stAFParam_Zone[29] << 16 |
			af->stAFParam_Zone[28] << 8 | af->stAFParam_Zone[27]);
	system_reg_write(((tisp_core_t *)af->core)->priv_data, AF_STAT_ADDR_VER_ZONE_SIZE_1315,
			af->stAFParam_Zone[33] << 16 | af->stAFParam_Zone[32] << 8 |
			af->stAFParam_Zone[31]);
	system_reg_write(((tisp_core_t *)af->core)->priv_data, AF_STAT_ADDR_FREQ_IIR_EN_LUM_TH,
			af->stAFParam_Zone[35] << 16 | af->stAFParam_ThresEnable[4] << 8 |
			af->stAFParam_ThresEnable[3] << 7 | af->stAFParam_ThresEnable[2] << 6 |
			af->stAFParam_ThresEnable[1] << 5 | af->stAFParam_ThresEnable[0] << 4 |
			af->stAFParam_Zone[34]);
	system_reg_write(((tisp_core_t *)af->core)->priv_data, AF_STAT_ADDR_LDG_COR_EN,
			af->stAFParam_ThresEnable[12] << 28 | af->stAFParam_ThresEnable[11] << 24 |
			af->stAFParam_ThresEnable[10] << 20 | af->stAFParam_ThresEnable[9] << 16 |
			af->stAFParam_ThresEnable[8] << 12 | af->stAFParam_ThresEnable[7] << 8 |
			af->stAFParam_ThresEnable[6] << 4 | af->stAFParam_ThresEnable[5]);
	system_reg_write(((tisp_core_t *)af->core)->priv_data, AF_STAT_ADDR_FIR0_G1_G2,
			af->stAFParam_FIR0_V[1] << 16 | af->stAFParam_FIR0_V[0]);
	system_reg_write(((tisp_core_t *)af->core)->priv_data, AF_STAT_ADDR_FIR0_G3_G4,
			af->stAFParam_FIR0_V[3] << 16 | af->stAFParam_FIR0_V[2]);
	system_reg_write(((tisp_core_t *)af->core)->priv_data, AF_STAT_ADDR_FIR0_G5, af->stAFParam_FIR0_V[4]);
	system_reg_write(((tisp_core_t *)af->core)->priv_data, AF_STAT_ADDR_FIR1_G1_G2,
			af->stAFParam_FIR1_V[1] << 16 | af->stAFParam_FIR1_V[0]);
	system_reg_write(((tisp_core_t *)af->core)->priv_data, AF_STAT_ADDR_FIR1_G3_G4,
			af->stAFParam_FIR1_V[3] << 16 | af->stAFParam_FIR1_V[2]);
	system_reg_write(((tisp_core_t *)af->core)->priv_data, AF_STAT_ADDR_FIR1_G5,
			af->stAFParam_FIR1_V[4]);
	system_reg_write(((tisp_core_t *)af->core)->priv_data, AF_STAT_ADDR_IIR0_G1_G3,
			af->stAFParam_IIR0_H[2] << 16 | af->stAFParam_IIR0_H[0]);
	system_reg_write(((tisp_core_t *)af->core)->priv_data, AF_STAT_ADDR_IIR0_G4_G5,
			af->stAFParam_IIR0_H[4] << 16 | af->stAFParam_IIR0_H[3]);
	system_reg_write(((tisp_core_t *)af->core)->priv_data, AF_STAT_ADDR_IIR0_G6_G8,
			af->stAFParam_IIR0_H[7] << 16 | af->stAFParam_IIR0_H[5]);
	system_reg_write(((tisp_core_t *)af->core)->priv_data, AF_STAT_ADDR_IIR0_G9_G10,
			af->stAFParam_IIR0_H[9] << 16 | af->stAFParam_IIR0_H[8]);
	system_reg_write(((tisp_core_t *)af->core)->priv_data, AF_STAT_ADDR_IIR1_G1_G3,
			af->stAFParam_IIR1_H[2] << 16 | af->stAFParam_IIR1_H[0]);
	system_reg_write(((tisp_core_t *)af->core)->priv_data, AF_STAT_ADDR_IIR1_G4_G5,
			af->stAFParam_IIR1_H[4] << 16 | af->stAFParam_IIR1_H[3]);
	system_reg_write(((tisp_core_t *)af->core)->priv_data, AF_STAT_ADDR_IIR1_G6_G8,
			af->stAFParam_IIR1_H[7] << 16 | af->stAFParam_IIR1_H[5]);
	system_reg_write(((tisp_core_t *)af->core)->priv_data, AF_STAT_ADDR_IIR1_G9_G10,
			af->stAFParam_IIR1_H[9] << 16 | af->stAFParam_IIR1_H[8]);
	system_reg_write(((tisp_core_t *)af->core)->priv_data, AF_STAT_ADDR_LDG_FIR0_LOW,
			af->stAFParam_FIR0_Ldg[3] << 24 | af->stAFParam_FIR0_Ldg[2] << 16 |
			af->stAFParam_FIR0_Ldg[1]<< 8|af->stAFParam_FIR0_Ldg[0]);
	system_reg_write(((tisp_core_t *)af->core)->priv_data, AF_STAT_ADDR_LDG_FIR0_HIGH,
			af->stAFParam_FIR0_Ldg[7] << 24 | af->stAFParam_FIR0_Ldg[6] << 16 |
			af->stAFParam_FIR0_Ldg[5]<< 8 | af->stAFParam_FIR0_Ldg[4]);
	system_reg_write(((tisp_core_t *)af->core)->priv_data, AF_STAT_ADDR_LDG_FIR1_LOW,
			af->stAFParam_FIR1_Ldg[3] << 24 | af->stAFParam_FIR1_Ldg[2] << 16 |
			af->stAFParam_FIR1_Ldg[1]<< 8|af->stAFParam_FIR1_Ldg[0]);
	system_reg_write(((tisp_core_t *)af->core)->priv_data, AF_STAT_ADDR_LDG_FIR1_HIGH,
			af->stAFParam_FIR1_Ldg[7] << 24 | af->stAFParam_FIR1_Ldg[6] << 16 |
			af->stAFParam_FIR1_Ldg[5]<< 8|af->stAFParam_FIR1_Ldg[4]);
	system_reg_write(((tisp_core_t *)af->core)->priv_data, AF_STAT_ADDR_LDG_IIR0_LOW,
			af->stAFParam_IIR0_Ldg[3] << 24 | af->stAFParam_IIR0_Ldg[2] << 16 |
			af->stAFParam_IIR0_Ldg[1]<< 8|af->stAFParam_IIR0_Ldg[0]);
	system_reg_write(((tisp_core_t *)af->core)->priv_data, AF_STAT_ADDR_LDG_IIR0_HIGH,
			af->stAFParam_IIR0_Ldg[7] << 24 | af->stAFParam_IIR0_Ldg[6] << 16 |
			af->stAFParam_IIR0_Ldg[5]<< 8|af->stAFParam_IIR0_Ldg[4]);
	system_reg_write(((tisp_core_t *)af->core)->priv_data, AF_STAT_ADDR_LDG_IIR1_LOW,
			af->stAFParam_IIR1_Ldg[3] << 24 | af->stAFParam_IIR1_Ldg[2] << 16 |
			af->stAFParam_IIR1_Ldg[1]<< 8|af->stAFParam_IIR1_Ldg[0]);
	system_reg_write(((tisp_core_t *)af->core)->priv_data, AF_STAT_ADDR_LDG_IIR1_HIGH,
			af->stAFParam_IIR1_Ldg[7] << 24 | af->stAFParam_IIR1_Ldg[6] << 16 |
			af->stAFParam_IIR1_Ldg[5]<< 8|af->stAFParam_IIR1_Ldg[4]);
	system_reg_write(((tisp_core_t *)af->core)->priv_data, AF_STAT_ADDR_COR_F0_TH_LH,
			af->stAFParam_FIR0_Coring[1] << 16 | af->stAFParam_FIR0_Coring[0]);
	system_reg_write(((tisp_core_t *)af->core)->priv_data, AF_STAT_ADDR_COR_F0_SLP_LIM,
			af->stAFParam_FIR0_Coring[3] << 16 | af->stAFParam_FIR0_Coring[2]);
	system_reg_write(((tisp_core_t *)af->core)->priv_data, AF_STAT_ADDR_COR_F1_TH_LH,
			af->stAFParam_FIR1_Coring[1] << 16 | af->stAFParam_FIR1_Coring[0]);
	system_reg_write(((tisp_core_t *)af->core)->priv_data, AF_STAT_ADDR_COR_F1_SLP_LIM,
			af->stAFParam_FIR1_Coring[3] << 16 | af->stAFParam_FIR1_Coring[2]);
	system_reg_write(((tisp_core_t *)af->core)->priv_data, AF_STAT_ADDR_COR_I0_TH_LH,
			af->stAFParam_IIR0_Coring[1] << 16 | af->stAFParam_IIR0_Coring[0]);
	system_reg_write(((tisp_core_t *)af->core)->priv_data, AF_STAT_ADDR_COR_I0_SLP_LIM,
			af->stAFParam_IIR0_Coring[3] << 16 | af->stAFParam_IIR0_Coring[2]);
	system_reg_write(((tisp_core_t *)af->core)->priv_data, AF_STAT_ADDR_COR_I1_TH_LH,
			af->stAFParam_IIR1_Coring[1] << 16 | af->stAFParam_IIR1_Coring[0]);
	system_reg_write(((tisp_core_t *)af->core)->priv_data, AF_STAT_ADDR_COR_I1_SLP_LIM,
			af->stAFParam_IIR1_Coring[3] << 16 | af->stAFParam_IIR1_Coring[2]);

	return 0;
}

int tiziano_af_init(tisp_af_t *af, uint32_t height,uint32_t width)
{

	tiziano_af_params_refresh(af);

	af->IspAfStaticParam.width = width;
	af->IspAfStaticParam.height = height;

	tiziano_af_set_regs(af);

	/* tiziano_af_dump(); */
	system_irq_func_set(((tisp_core_t *)af->core)->priv_data, TIZIANO_ISP_IRQ_AF_STATIC,af_interrupt_static, af);

	memset(&af->af_attr, 0, sizeof(af->af_attr));
	af->af_attr.af_enable = 1;
	af->af_attr.af_metrics_shift = 0;
	af->af_attr.af_delta = af->AFParam_Tilt[2];
	af->af_attr.af_theta = af->AFParam_Tilt[3];
	af->af_attr.af_hilight_th = af->stAFParam_ThresEnable[4];
	af->af_attr.af_alpha_alt = af->AFParam_Tilt[0];
	af->af_attr.af_hstart = af->stAFParam_Zone[2];
	af->af_attr.af_vstart = af->stAFParam_Zone[0];
	af->af_attr.af_stat_nodeh = af->stAFParam_Zone[3];
	af->af_attr.af_stat_nodev = af->stAFParam_Zone[1];

	af->af_set_trig = 0;

	return 0;
}

int tisp_af_get_metric(tisp_af_t *af, unsigned int *metric)
{
	*metric = af->AFParam_Fv[2] >> af->af_attr.af_metrics_shift;

	return 0;
}

int tisp_af_get_attr(tisp_af_t *af, tisp_af_attr *af_info)
{
	af_info->af_metrics = af->AFParam_Fv[2] >> af->af_attr.af_metrics_shift;
	af_info->af_metrics_alt = af->AFParam_Fv_Alt >> af->af_attr.af_metrics_shift;
	af_info->af_enable = af->af_attr.af_enable;
	af_info->af_metrics_shift = af->af_attr.af_metrics_shift;
	af_info->af_delta = af->AFParam_Tilt[2];
	af_info->af_theta = af->AFParam_Tilt[3];
	af_info->af_hilight_th = af->stAFParam_ThresEnable[4];
	af_info->af_alpha_alt = af->AFParam_Tilt[0];
	af_info->af_hstart = af->stAFParam_Zone[2];
	af_info->af_vstart = af->stAFParam_Zone[0];
	af_info->af_stat_nodeh = af->stAFParam_Zone[3];
	af_info->af_stat_nodev = af->stAFParam_Zone[1];

	return 0;
}

void tisp_af_set_attr_refresh(tisp_af_t *af)
{
	af->AFParam_Tilt[2] = af->af_attr.af_delta;
	af->AFParam_Tilt[3] = af->af_attr.af_theta;
	af->stAFParam_ThresEnable[4] = af->af_attr.af_hilight_th;
	af->AFParam_Tilt[0] = af->af_attr.af_alpha_alt;
	af->stAFParam_Zone[2] = af->af_attr.af_hstart;
	af->stAFParam_Zone[0] = af->af_attr.af_vstart;
	af->stAFParam_Zone[3] = af->af_attr.af_stat_nodeh;
	af->stAFParam_Zone[1] = af->af_attr.af_stat_nodev;

	tiziano_af_set_regs(af);
}

int tisp_af_set_attr(tisp_af_t *af, tisp_af_attr af_info)
{
	memcpy(&af->af_attr, &af_info, sizeof(af_info));
	tisp_af_set_attr_refresh(af);
	af->af_set_trig = 1;

	return 0;
}

void tiziano_af_dn_params_refresh(tisp_af_t *af)
{
	tiziano_af_params_refresh(af);
	if(af->af_set_trig){
		tisp_af_set_attr_refresh(af);
	}
	tiziano_af_set_regs(af);
}

int32_t tisp_af_param_array_get(tisp_af_t *af, int32_t id, void *buf, uint32_t *plen)
{
	uint32_t len;
	void *tmpbuf;

	switch (id) {
	case TISP_PARAM_AF_ZONE:
		len = sizeof(tparams.params_data.TISP_PARAM_AF_ZONE);
		tmpbuf = af->stAFParam_Zone;
		break;
	case TISP_PARAM_AF_TH_EN:
		len = sizeof(tparams.params_data.TISP_PARAM_AF_TH_EN);
		tmpbuf = af->stAFParam_ThresEnable;
		break;
	case TISP_PARAM_AF_FIR0_V:
		len = sizeof(tparams.params_data.TISP_PARAM_AF_FIR0_V);
		tmpbuf = af->stAFParam_FIR0_V;
		break;
	case TISP_PARAM_AF_FIR0_LDG:
		len = sizeof(tparams.params_data.TISP_PARAM_AF_FIR0_LDG);
		tmpbuf = af->stAFParam_FIR0_Ldg;
		break;
	case TISP_PARAM_AF_FIR0_CORING:
		len = sizeof(tparams.params_data.TISP_PARAM_AF_FIR0_CORING);
		tmpbuf = af->stAFParam_FIR0_Coring;
		break;
	case TISP_PARAM_AF_FIR1_V:
		len = sizeof(tparams.params_data.TISP_PARAM_AF_FIR1_V);
		tmpbuf = af->stAFParam_FIR1_V;
		break;
	case TISP_PARAM_AF_FIR1_LDG:
		len = sizeof(tparams.params_data.TISP_PARAM_AF_FIR1_LDG);
		tmpbuf = af->stAFParam_FIR1_Ldg;
		break;
	case TISP_PARAM_AF_FIR1_CORING:
		len = sizeof(tparams.params_data.TISP_PARAM_AF_FIR1_CORING);
		tmpbuf = af->stAFParam_FIR1_Coring;
		break;
	case TISP_PARAM_AF_IIR0_H:
		len = sizeof(tparams.params_data.TISP_PARAM_AF_IIR0_H);
		tmpbuf = af->stAFParam_IIR0_H;
		break;
	case TISP_PARAM_AF_IIR0_LDG:
		len = sizeof(tparams.params_data.TISP_PARAM_AF_IIR0_LDG);
		tmpbuf = af->stAFParam_IIR0_Ldg;
		break;
	case TISP_PARAM_AF_IIR0_CORING:
		len = sizeof(tparams.params_data.TISP_PARAM_AF_IIR0_CORING);
		tmpbuf = af->stAFParam_IIR0_Coring;
		break;
	case TISP_PARAM_AF_IIR1_H:
		len = sizeof(tparams.params_data.TISP_PARAM_AF_IIR1_H);
		tmpbuf = af->stAFParam_IIR1_H;
		break;
	case TISP_PARAM_AF_IIR1_LDG:
		len = sizeof(tparams.params_data.TISP_PARAM_AF_IIR1_LDG);
		tmpbuf = af->stAFParam_IIR1_Ldg;
		break;
	case TISP_PARAM_AF_IIR1_CORING:
		len = sizeof(tparams.params_data.TISP_PARAM_AF_IIR1_CORING);
		tmpbuf = af->stAFParam_IIR1_Coring;
		break;
	case TISP_PARAM_AF_POINTPOS:
		len = sizeof(tparams.params_data.TISP_PARAM_AF_POINTPOS);
		tmpbuf = af->AFParam_PointPos;
		break;
	case TISP_PARAM_AF_TILT:
		len = sizeof(tparams.params_data.TISP_PARAM_AF_TILT);
		tmpbuf = af->AFParam_Tilt;
		break;
	case TISP_PARAM_AF_FVW_MEAN:
		len = sizeof(tparams.params_data.TISP_PARAM_AF_FVW_MEAN);
		tmpbuf = af->AFParam_FvWmean;
		break;
	case TISP_PARAM_AF_FV:
		len = sizeof(tparams.params_data.TISP_PARAM_AF_FV);
		tmpbuf = af->AFParam_Fv;
		break;
	case TISP_PARAM_AF_WEIGHT:
		len = sizeof(tparams.params_data.TISP_PARAM_AF_WEIGHT);
		tmpbuf = af->AFWeight_Param;
		break;
	default:
		ISP_ERROR("%s,%d: af not support param id %d\n", __func__, __LINE__, id);
		return -1;
	}

	memcpy(buf, tmpbuf, len);
	*plen = len;

	return 0;
}

int32_t tisp_af_param_array_set(tisp_af_t *af, int32_t id, void *buf, uint32_t *plen)
{
	uint32_t len;
	void *tmpbuf;

	switch (id) {
	case TISP_PARAM_AF_ZONE:
		len = sizeof(tparams.params_data.TISP_PARAM_AF_ZONE);
		tmpbuf = af->stAFParam_Zone;
		break;
	case TISP_PARAM_AF_TH_EN:
		len = sizeof(tparams.params_data.TISP_PARAM_AF_TH_EN);
		tmpbuf = af->stAFParam_ThresEnable;
		break;
	case TISP_PARAM_AF_FIR0_V:
		len = sizeof(tparams.params_data.TISP_PARAM_AF_FIR0_V);
		tmpbuf = af->stAFParam_FIR0_V;
		break;
	case TISP_PARAM_AF_FIR0_LDG:
		len = sizeof(tparams.params_data.TISP_PARAM_AF_FIR0_LDG);
		tmpbuf = af->stAFParam_FIR0_Ldg;
		break;
	case TISP_PARAM_AF_FIR0_CORING:
		len = sizeof(tparams.params_data.TISP_PARAM_AF_FIR0_CORING);
		tmpbuf = af->stAFParam_FIR0_Coring;
		break;
	case TISP_PARAM_AF_FIR1_V:
		len = sizeof(tparams.params_data.TISP_PARAM_AF_FIR1_V);
		tmpbuf = af->stAFParam_FIR1_V;
		break;
	case TISP_PARAM_AF_FIR1_LDG:
		len = sizeof(tparams.params_data.TISP_PARAM_AF_FIR1_LDG);
		tmpbuf = af->stAFParam_FIR1_Ldg;
		break;
	case TISP_PARAM_AF_FIR1_CORING:
		len = sizeof(tparams.params_data.TISP_PARAM_AF_FIR1_CORING);
		tmpbuf = af->stAFParam_FIR1_Coring;
		break;
	case TISP_PARAM_AF_IIR0_H:
		len = sizeof(tparams.params_data.TISP_PARAM_AF_IIR0_H);
		tmpbuf = af->stAFParam_IIR0_H;
		break;
	case TISP_PARAM_AF_IIR0_LDG:
		len = sizeof(tparams.params_data.TISP_PARAM_AF_IIR0_LDG);
		tmpbuf = af->stAFParam_IIR0_Ldg;
		break;
	case TISP_PARAM_AF_IIR0_CORING:
		len = sizeof(tparams.params_data.TISP_PARAM_AF_IIR0_CORING);
		tmpbuf = af->stAFParam_IIR0_Coring;
		break;
	case TISP_PARAM_AF_IIR1_H:
		len = sizeof(tparams.params_data.TISP_PARAM_AF_IIR1_H);
		tmpbuf = af->stAFParam_IIR1_H;
		break;
	case TISP_PARAM_AF_IIR1_LDG:
		len = sizeof(tparams.params_data.TISP_PARAM_AF_IIR1_LDG);
		tmpbuf = af->stAFParam_IIR1_Ldg;
		break;
	case TISP_PARAM_AF_IIR1_CORING:
		len = sizeof(tparams.params_data.TISP_PARAM_AF_IIR1_CORING);
		tmpbuf = af->stAFParam_IIR1_Coring;
		break;
	case TISP_PARAM_AF_POINTPOS:
		len = sizeof(tparams.params_data.TISP_PARAM_AF_POINTPOS);
		tmpbuf = af->AFParam_PointPos;
		break;
	case TISP_PARAM_AF_TILT:
		len = sizeof(tparams.params_data.TISP_PARAM_AF_TILT);
		tmpbuf = af->AFParam_Tilt;
		break;
	case TISP_PARAM_AF_FVW_MEAN:
		len = sizeof(tparams.params_data.TISP_PARAM_AF_FVW_MEAN);
		tmpbuf = af->AFParam_FvWmean;
		break;
	case TISP_PARAM_AF_FV:
		len = sizeof(tparams.params_data.TISP_PARAM_AF_FV);
		tmpbuf = af->AFParam_Fv;
		break;
	case TISP_PARAM_AF_WEIGHT:
		len = sizeof(tparams.params_data.TISP_PARAM_AF_WEIGHT);
		tmpbuf = af->AFWeight_Param;
		break;
	default:
		ISP_ERROR("%s,%d: af not support param id %d\n", __func__, __LINE__, id);
		return -1;
	}

	memcpy(tmpbuf, buf, len);
	*plen = len;
	tiziano_af_set_regs(af);

	return 0;
}
