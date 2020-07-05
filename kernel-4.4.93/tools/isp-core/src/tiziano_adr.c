#include <linux/kernel.h>
#include <linux/irqreturn.h>
#include <linux/dma-mapping.h>

#include "fix_point_calc.h"
#include "../inc/tiziano_sys.h"
#include "tiziano_core.h"
#include "tiziano_adr.h"
#include "tiziano_params.h"
#include "tiziano_event.h"
#include "tiziano_core.h"
#include "tiziano_map.h"
#include "tiziano_gamma.h"


#if 0
static unsigned int ev_changed = 0;
static unsigned int ev_now;
static unsigned int width_def = 1920;
static unsigned int height_def = 1080;
uint32_t adr_ev_list[9];
uint32_t adr_ligb_list[9];
uint32_t adr_mapb1_list[9];
uint32_t adr_mapb2_list[9];
uint32_t adr_mapb3_list[9];
uint32_t adr_mapb4_list[9];
uint32_t adr_blp2_list[9];

Isp_Adr_Ct_Detect_Para TizianoAdrFpgaStructMe;

uint32_t param_adr_ct_par_array[8];
uint32_t param_adr_weight_20_lut_array[32];
uint32_t param_adr_weight_02_lut_array[32];
uint32_t param_adr_weight_12_lut_array[32];
uint32_t param_adr_weight_22_lut_array[32];
uint32_t param_adr_weight_21_lut_array[32];
uint32_t param_adr_ctc_kneepoint_array[8];
uint32_t param_adr_min_kneepoint_array[23];
uint32_t param_adr_map_kneepoint_array[23];
uint32_t param_adr_coc_kneepoint_array[21];
uint32_t param_adr_centre_w_dis_array[31];
uint32_t param_adr_contrast_w_dis_array[32];
uint32_t param_adr_stat_block_hist_diff_array[4];
#endif

#if 0
uint32_t adr_block_y[20];
uint32_t adr_block_hist[100];
uint32_t adr_hist[512];
uint32_t adr_para[12] = {4,5,270,384,26,38,13,0,1023,2047,1023,1432};//26,38,13,2047,1432
uint32_t adr_block_coord_m[5] = {0,270,540,810,1080};
uint32_t adr_block_coord_n[6] = {0,384,768,1152,1536,1920};

#endif

#if 0
uint32_t weightLUT20[32] = {0,0,0,0,0,1,1,1,1,1,2,2,2,3,3,3,3,3,4,4,4,4,4,5,5,5,5,5,5,5,5,5};
uint32_t weightLUT02[32] = {0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2};
uint32_t weightLUT12[32] = {0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1};
uint32_t weightLUT22[32] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1};
uint32_t weightLUT21[32] = {0,0,0,0,1,1,1,1,2,2,2,2,2,2,2,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3};
#endif

uint32_t ctc_kneepoint_x[4] = {154,339,589,958};
uint32_t ctc_kneepoint_mux[4] = {6809,5668,4194,2842};
uint32_t min_kneepoint_x[11] = {32,64,128,192,256,384,512,768,1024,1536,2048};
uint32_t min_kneepoint_y[11] = {32,64,128,192,256,384,512,768,1024,1536,2048};
/* {0,2,4,6,8,30,60,70,80,100,120}; */
uint32_t min_kneepoint_pow[12] = {5,5,6,6,6,7,7,8,8,9,9,11};
uint32_t map_kneepoint_x[11] = {32,64,128,192,256,384,512,768,1024,1536,2048};

uint32_t map_kneepoint_y[220] = {
	32,64,128,192,256,384,512,768,1024,1536,2048,
	32,64,128,192,256,384,512,768,1024,1536,2048,
	32,64,128,192,256,384,512,768,1024,1536,2048,
	32,64,128,192,256,384,512,768,1024,1536,2048,
	32,64,128,192,256,384,512,768,1024,1536,2048,
	32,64,128,192,256,384,512,768,1024,1536,2048,
	32,64,128,192,256,384,512,768,1024,1536,2048,
	32,64,128,192,256,384,512,768,1024,1536,2048,
	32,64,128,192,256,384,512,768,1024,1536,2048,
	32,64,128,192,256,384,512,768,1024,1536,2048,
	32,64,128,192,256,384,512,768,1024,1536,2048,
	32,64,128,192,256,384,512,768,1024,1536,2048,
	32,64,128,192,256,384,512,768,1024,1536,2048,
	32,64,128,192,256,384,512,768,1024,1536,2048,
	32,64,128,192,256,384,512,768,1024,1536,2048,
	32,64,128,192,256,384,512,768,1024,1536,2048,
	32,64,128,192,256,384,512,768,1024,1536,2048,
	32,64,128,192,256,384,512,768,1024,1536,2048,
	32,64,128,192,256,384,512,768,1024,1536,2048,
	32,64,128,192,256,384,512,768,1024,1536,2048,
};
uint32_t map_kneepoint_pow[12] = {5,5,6,6,6,7,7,8,8,9,9,11};
uint32_t coc_kneepoint_x[11] = {0,32,64,96,128,160,192,224,256,512,5120};
uint32_t coc_kneepoint_pow[10] = {5,5,5,5,5,5,5,5,8,8};
uint32_t centre_w_distance[31] = {7123,5227,4345,3764,3331,2984,2696,2449,2233,2041,
				  1868,1711,1567,1435,1311,1196,1088,987,891,800,
				  714,632,553,478,406,337,271,207,145,86,28};

uint32_t contrast_w_distance[32] = {336987,247968,206577,179313,158950,142690,129154,117558,107417,98404,
				    90295,82923,76167,69931,64141,58737,53671,48903,44401,40135,
				    36083,32224,28540,25016,21640,18398,15281,12280,9386,6591,
				    3890,1276};/* 1276 */

uint32_t adr_stat_block_hist_diff[4] = {157,350,608,1017};

uint32_t adr_tm_base_lut[9] = {0,203,394,645,1018,1557,2267,3136,4095};
uint32_t adr_gam_x[129] = {0,32,64,96,128,160,192,224,256,288,
			   320,352,384,416,448,480,512,544,576,608,
			   640,672,704,736,768,800,832,864,896,928,
			   960,992,1024,1056,1088,1120,1152,1184,1216,1248,
			   1280,1312,1344,1376,1408,1440,1472,1504,1536,1568,
			   1600,1632,1664,1696,1728,1760,1792,1824,1856,1888,
			   1920,1952,1984,2016,2048,2080,2112,2144,2176,2208,
			   2240,2272,2304,2336,2368,2400,2432,2464,2496,2528,
			   2560,2592,2624,2656,2688,2720,2752,2784,2816,2848,
			   2880,2912,2944,2976,3008,3040,3072,3104,3136,3168,
			   3200,3232,3264,3296,3328,3360,3392,3424,3456,3488,
			   3520,3552,3584,3616,3648,3680,3712,3744,3776,3808,
			   3840,3872,3904,3936,3968,4000,4032,4064,4095};
uint32_t adr_gam_y[129] = {0,59,133,218,304,392,480,570,660,748,837,921,1001,1076,1146,1215,1280,1344,1406,1468,1528,1584,1638,1688,1737,1782,1824,1866,1907,1945,1983,2020,2055,2090,2124,2159,2191,2224,2256,2287,2317,2348,2378,2407,2435,2463,2490,2517,2543,2569,2594,2619,2645,2668,2693,2716,2741,2765,2789,2812,2835,2858,2881,2903,2924,2947,2968,2990,3012,3033,3054,3075,3096,3116,3136,3156,3176,3196,3215,3235,3254,3273,3292,3311,3329,3348,3367,3386,3403,3422,3440,3458,3476,3494,3513,3530,3548,3566,3584,3601,3619,3637,3654,3672,3690,3707,3724,3742,3759,3777,3794,3811,3827,3844,3862,3879,3895,3912,3929,3945,3962,3979,3995,4012,4028,4045,4061,4078,4095};
uint32_t adr_light_end[29] = {0,0,0,930,45,
			      0,40,1000,200,40,100,
			      20,25,1000,1800,30,120,
			      40,60,110,160,
			      670,600,442,100,70,0,
			      0,5500};/* 7000 */
uint32_t adr_block_light[15] = {30,30,500,200,260,
				100,30,25,1920,1080,
				253,190,230,373,
				1};/* 157,193,258,409;203,190,250,373 */
uint32_t adr_map_mode[11] = {0,10000,10000,300,200,300,64,200,200,200,64};
uint32_t histSub_4096[9] = {0,512,1024,1536,2048,2560,3072,3584,4095};//16 * adr->histSub_256[9]
uint32_t histSub_4096_out[9];
uint32_t histSub_4096_diff[8] = {197,200,277,404,558,687,818,954};

int32_t tisp_adr_ev_update(tisp_adr_t *adr, uint64_t data1)
{
	adr->ev_changed = 1;
	adr->ev_now = data1 >> 10;

	return 0;
}

void tiziano_adr_get_data(tisp_adr_t *adr, uint32_t *img)
{
	int block_num_h = 4;
	int block_num_w = 5;
	int i,j,b,k;
	int value_tmp;

	img = img + 2;
	for (i = 0; i < block_num_h; i++) {
		for (j = 0; j < block_num_w; j++) {
			b = 4 * j + i;
			adr->adr_block_y[b] = *(img + 2 * i * block_num_w + 2 * j) & 0xffffff;
			adr->adr_block_hist[5 * b + 4] = *(img + 2 * i * block_num_w + 2 * j) >> 24;
			adr->adr_block_hist[5 * b + 3] = *(img + 2 * i * block_num_w + 2 * j + 1) & 0xff;
			adr->adr_block_hist[5 * b + 2] = (*(img + 2 * i * block_num_w + 2 * j + 1) >> 8) & 0xff;
			adr->adr_block_hist[5 * b + 1] = (*(img + 2 * i * block_num_w + 2 * j + 1) >> 16) & 0xff;
			adr->adr_block_hist[5 * b + 0] = *(img + 2 * i * block_num_w + 2 * j + 1) >> 24;
		}
	}
	img = img + 20 * 2;
	b = 512 / 5;
	for (k = 0; k < b; k++){
		adr->adr_hist[5 * k + 4] = *(img + 2 * k) & 0xfff;
		adr->adr_hist[5 * k + 3] = (*(img + 2 * k) >> 12) & 0xfff;
		value_tmp = *(img + 2 * k) >> 24;
		adr->adr_hist[5*k+2] = ((*(img + 2 * k + 1) & 0xf) << 8) | value_tmp;
		adr->adr_hist[5*k+1] = (*(img + 2 * k + 1) >> 4) & 0xfff;
		adr->adr_hist[5*k+0] = (*(img + 2 * k + 1) >> 16) & 0xfff;
	}
	adr->adr_hist[5*b+1] = (*(img + 2 * b + 1) >> 4) & 0xfff;
	adr->adr_hist[5*b+0] = (*(img + 2 * b + 1) >> 16) & 0xfff;
}

int32_t tiziano_adr_algorithm(tisp_adr_t *adr)
{
#if 1
	int i = 0;
	unsigned int adr_ev;
	adr_ev = adr->ev_now;

	if (adr->ev_changed == 1) {
		adr->ev_changed = 0;
		for (i = 0; i < 9; i++) {
			if (adr_ev <= (uint32_t)adr->adr_ev_list[i]) {
				if (i == 0) {
					adr->adr_light_end[28] = adr->adr_ligb_list[0];
					adr->adr_block_light[4] = adr->adr_blp2_list[0];
					adr->adr_block_light[10] = adr->adr_mapb1_list[0];
					adr->adr_block_light[11] = adr->adr_mapb2_list[0];
					adr->adr_block_light[12] = adr->adr_mapb3_list[0];
					adr->adr_block_light[13] = adr->adr_mapb4_list[0];
					break;
				}
				if (adr->adr_ev_list[i] == adr->adr_ev_list[i - 1]) {
					adr->adr_light_end[28] = adr->adr_ligb_list[i];
					adr->adr_block_light[4] = adr->adr_blp2_list[i];
					adr->adr_block_light[10] = adr->adr_mapb1_list[i];
					adr->adr_block_light[11] = adr->adr_mapb2_list[i];
					adr->adr_block_light[12] = adr->adr_mapb3_list[i];
					adr->adr_block_light[13] = adr->adr_mapb4_list[i];
					break;
				} else {
					adr->adr_light_end[28] = ISPINT(adr_ev,adr->adr_ev_list[i-1],
								   adr->adr_ev_list[i],adr->adr_ligb_list[i-1],
								   adr->adr_ligb_list[i]);
					adr->adr_block_light[4] = ISPINT(adr_ev,adr->adr_ev_list[i-1],
								    adr->adr_ev_list[i],adr->adr_blp2_list[i-1],
								    adr->adr_blp2_list[i]);
					adr->adr_block_light[10] = ISPINT(adr_ev,
								     adr->adr_ev_list[i-1],adr->adr_ev_list[i],
								     adr->adr_mapb1_list[i-1],adr->adr_mapb1_list[i]);
					adr->adr_block_light[11] = ISPINT(adr_ev,
								     adr->adr_ev_list[i-1],adr->adr_ev_list[i],
								     adr->adr_mapb2_list[i-1],adr->adr_mapb2_list[i]);
					adr->adr_block_light[12] = ISPINT(adr_ev,
								     adr->adr_ev_list[i-1],adr->adr_ev_list[i],
								     adr->adr_mapb3_list[i-1],adr->adr_mapb3_list[i]);
					adr->adr_block_light[13] = ISPINT(adr_ev,
								     adr->adr_ev_list[i-1],adr->adr_ev_list[i],
								     adr->adr_mapb4_list[i-1],adr->adr_mapb4_list[i]);
					break;
				}
			}
		}
		if (i == 9) {
			adr->adr_light_end[28] = adr->adr_ligb_list[8];
			adr->adr_block_light[4] = adr->adr_blp2_list[8];
			adr->adr_block_light[10] = adr->adr_mapb1_list[8];
			adr->adr_block_light[11] = adr->adr_mapb2_list[8];
			adr->adr_block_light[12] = adr->adr_mapb3_list[8];
			adr->adr_block_light[13] = adr->adr_mapb4_list[8];
		}
	}
#endif

	adr->TizianoAdrFpgaStructMe.CtcKneepointX 	= adr->ctc_kneepoint_x;
	adr->TizianoAdrFpgaStructMe.CtcKneepointMux 	= adr->ctc_kneepoint_mux;
	adr->TizianoAdrFpgaStructMe.MinKneepointX 	= adr->min_kneepoint_x;
	adr->TizianoAdrFpgaStructMe.MinKneepointY 	= adr->min_kneepoint_y;
	adr->TizianoAdrFpgaStructMe.MapKneepointX 	= adr->map_kneepoint_x;
	adr->TizianoAdrFpgaStructMe.MapKneepointY 	= adr->map_kneepoint_y;
	adr->TizianoAdrFpgaStructMe.ContrastWDistance 	= adr->contrast_w_distance;
	adr->TizianoAdrFpgaStructMe.AdrHist 		= adr->adr_hist;
	adr->TizianoAdrFpgaStructMe.AdrBlockY 		= adr->adr_block_y;
	adr->TizianoAdrFpgaStructMe.AdrBlockHist 	= adr->adr_block_hist;
	adr->TizianoAdrFpgaStructMe.TmBaseLut 		= adr->adr_tm_base_lut;
	adr->TizianoAdrFpgaStructMe.AdrGamX 		= adr->adr_gam_x;
	adr->TizianoAdrFpgaStructMe.AdrGamY 		= adr->adr_gam_y;
	adr->TizianoAdrFpgaStructMe.AdrMapMode 		= adr->adr_map_mode;
	adr->TizianoAdrFpgaStructMe.AdrLightEnd 	= adr->adr_light_end;
	adr->TizianoAdrFpgaStructMe.AdrBlockLight	= adr->adr_block_light;
	Tiziano_adr_fpga(adr->TizianoAdrFpgaStructMe);

	return 0;
}

int tisp_adr_process(void *cb_data, uint64_t data1,uint64_t data2,uint64_t data3,uint64_t data4)
{
	tisp_adr_t *adr = (tisp_adr_t *)cb_data;
	uint32_t i,j;

	tiziano_adr_algorithm(adr);
	for (j = 0; j < 20; j++) {
		for (i = 0; i < 5; i++) {
			system_reg_write(((tisp_core_t *)adr->core)->priv_data, ADR_ADDR_MAP_KPOINT_Y_P00_0 + (j * 24) +
					 i * 4,adr->map_kneepoint_y[j * 11 + i * 2 + 1] << 16 |
					 adr->map_kneepoint_y[j * 11 + i * 2 + 0] << 0);
		}
		system_reg_write(((tisp_core_t *)adr->core)->priv_data, ADR_ADDR_MAP_KPOINT_Y_P00_5 + j * 24,
				 adr->map_kneepoint_y[j * 11 + 10] << 0);
	}
	for (i = 0; i < 5; i++) {
		system_reg_write(((tisp_core_t *)adr->core)->priv_data, ADR_ADDR_MIN_KPOINT_Y_P0 + i*4,
				 adr->min_kneepoint_y[i*2+1] << 16 |
				 adr->min_kneepoint_y[i*2+0] << 0);
	}
	system_reg_write(((tisp_core_t *)adr->core)->priv_data, ADR_ADDR_MIN_KPOINT_Y_P5,
			 adr->min_kneepoint_y[10] <<  0);
	system_reg_write(((tisp_core_t *)adr->core)->priv_data, ADR_ADDR_CTC_KPOINT_X_P0,
			 adr->ctc_kneepoint_x[2] << 16 |
			 adr->ctc_kneepoint_x[1] <<  0);
	system_reg_write(((tisp_core_t *)adr->core)->priv_data, ADR_ADDR_CTC_KPOINT_X_P1,
			 adr->ctc_kneepoint_x[3] << 0);
	system_reg_write(((tisp_core_t *)adr->core)->priv_data, ADR_ADDR_CTC_KPOINT_MUX_P0,
			 adr->ctc_kneepoint_mux[2] << 0);
	system_reg_write(((tisp_core_t *)adr->core)->priv_data, ADR_ADDR_CTC_KPOINT_MUX_P1,
			 adr->ctc_kneepoint_mux[3] << 0);
	for (i = 0; i < 32; i++) {
		system_reg_write(((tisp_core_t *)adr->core)->priv_data, ADR_ADDR_CON_W_DIS_P00 + i * 4,
				 adr->contrast_w_distance[i] << 0);
	}
	system_reg_write(((tisp_core_t *)adr->core)->priv_data, ADR_ADDR_CTC_RATIO,
			 adr->param_adr_ct_par_array[2] << 16 |
			 adr->param_adr_ct_par_array[1] << 8 |
			 adr->param_adr_ct_par_array[0] << 0);

	return 0;
}

int32_t tiziano_adr_interrupt_static(void *data)
{
	uint32_t addr = 0;
	tisp_adr_t *adr = (tisp_adr_t *)data;
	tisp_core_t *core = adr->core;
	tisp_info_t *tispinfo = &core->tispinfo;
	tisp_event_t adr_event;

	addr = system_reg_read(((tisp_core_t *)adr->core)->priv_data, ADR_ADDR_FRAME_ADDR);
	if (addr == tispinfo->buf_adr_paddr + 0 * 4 * 1024) {
		dma_cache_sync(NULL,(void*)tispinfo->buf_adr_vaddr + 0 * 4 * 1024, 4096, DMA_BIDIRECTIONAL);
		tiziano_adr_get_data(adr, (void*)tispinfo->buf_adr_vaddr + 0 * 4 * 1024);
	}
	if (addr == tispinfo->buf_adr_paddr + 1 * 4 * 1024) {
		dma_cache_sync(NULL,(void*)tispinfo->buf_adr_vaddr + 1 * 4 * 1024, 4096, DMA_BIDIRECTIONAL);
		tiziano_adr_get_data(adr, (void*)tispinfo->buf_adr_vaddr + 1 * 4 * 1024);
	}
	if (addr == tispinfo->buf_adr_paddr + 2 * 4 * 1024) {
		dma_cache_sync(NULL,(void*)tispinfo->buf_adr_vaddr + 2 * 4 * 1024, 4096, DMA_BIDIRECTIONAL);
		tiziano_adr_get_data(adr, (void*)tispinfo->buf_adr_vaddr + 2 * 4 * 1024);
	}
	if (addr == tispinfo->buf_adr_paddr + 3 * 4 * 1024) {
		dma_cache_sync(NULL,(void*)tispinfo->buf_adr_vaddr + 3 * 4 * 1024, 4096, DMA_BIDIRECTIONAL);
		tiziano_adr_get_data(adr, (void*)tispinfo->buf_adr_vaddr + 3 * 4 * 1024);
	}
	adr_event.type = TISP_EVENT_TYPE_ADR_PROCESS;
	tisp_event_push(&core->tisp_event_mg, &adr_event);

	return IRQ_HANDLED;
}

int32_t tiziano_adr_params_init(tisp_adr_t *adr)
{
	uint32_t i;

	for (i = 0; i < 15; i++) {
		system_reg_write(((tisp_core_t *)adr->core)->priv_data, ADR_ADDR_CEN_DIS_P_0 + i * 4,
				 adr->param_adr_centre_w_dis_array[i * 2 + 1] << 16 |
				 adr->param_adr_centre_w_dis_array[i * 2 + 0] << 0);
	}
	system_reg_write(((tisp_core_t *)adr->core)->priv_data, ADR_ADDR_CEN_DIS_P_F,
			 adr->param_adr_centre_w_dis_array[30] << 0);
	system_reg_write(((tisp_core_t *)adr->core)->priv_data, ADR_ADDR_MAP_KPOINT_X_P0,
			 adr->param_adr_map_kneepoint_array[1] << 16 |
			 adr->param_adr_map_kneepoint_array[ 0] << 0);
	system_reg_write(((tisp_core_t *)adr->core)->priv_data, ADR_ADDR_MAP_KPOINT_X_P1,
			 adr->param_adr_map_kneepoint_array[3] << 16 |
			 adr->param_adr_map_kneepoint_array[2] << 0);
	system_reg_write(((tisp_core_t *)adr->core)->priv_data, ADR_ADDR_MAP_KPOINT_X_P2,
			 adr->param_adr_map_kneepoint_array[5] << 16 |
			 adr->param_adr_map_kneepoint_array[4] <<  0);
	system_reg_write(((tisp_core_t *)adr->core)->priv_data, ADR_ADDR_MAP_KPOINT_X_P3,
			 adr->param_adr_map_kneepoint_array[7] << 16 |
			 adr->param_adr_map_kneepoint_array[6] << 0);
	system_reg_write(((tisp_core_t *)adr->core)->priv_data, ADR_ADDR_MAP_KPOINT_X_P4,
			 adr->param_adr_map_kneepoint_array[9] << 16 |
			 adr->param_adr_map_kneepoint_array[8] <<  0);
	system_reg_write(((tisp_core_t *)adr->core)->priv_data, ADR_ADDR_MAP_KPOINT_X_P5,
			 adr->param_adr_map_kneepoint_array[10] << 0);
	system_reg_write(((tisp_core_t *)adr->core)->priv_data, ADR_ADDR_MAP_KPOINT_POW_P0,
			 adr->param_adr_map_kneepoint_array[14] << 24 |
			 adr->param_adr_map_kneepoint_array[13] << 16 |
			 adr->param_adr_map_kneepoint_array[12] << 8 |
			 adr->param_adr_map_kneepoint_array[11] << 0);
	system_reg_write(((tisp_core_t *)adr->core)->priv_data, ADR_ADDR_MAP_KPOINT_POW_P1,
			 adr->param_adr_map_kneepoint_array[18] << 24 |
			 adr->param_adr_map_kneepoint_array[17] << 16 |
			 adr->param_adr_map_kneepoint_array[16] << 8 |
			 adr->param_adr_map_kneepoint_array[15] << 0);
	system_reg_write(((tisp_core_t *)adr->core)->priv_data, ADR_ADDR_MAP_KPOINT_POW_P2,
			 adr->param_adr_map_kneepoint_array[22] << 24 |
			 adr->param_adr_map_kneepoint_array[21] << 16 |
			 adr->param_adr_map_kneepoint_array[20] << 8 |
			 adr->param_adr_map_kneepoint_array[19] << 0);
	system_reg_write(((tisp_core_t *)adr->core)->priv_data, ADR_ADDR_WEILUT20_P0,
			 adr->param_adr_weight_20_lut_array[3] << 24 |
			 adr->param_adr_weight_20_lut_array[2] << 16 |
			 adr->param_adr_weight_20_lut_array[1] << 8 |
			 adr->param_adr_weight_20_lut_array[0] << 0);
	system_reg_write(((tisp_core_t *)adr->core)->priv_data, ADR_ADDR_WEILUT20_P1,
			 adr->param_adr_weight_20_lut_array[7] << 24 |
			 adr->param_adr_weight_20_lut_array[6] << 16 |
			 adr->param_adr_weight_20_lut_array[5] << 8 |
			 adr->param_adr_weight_20_lut_array[4] << 0);
	system_reg_write(((tisp_core_t *)adr->core)->priv_data, ADR_ADDR_WEILUT20_P2,
			 adr->param_adr_weight_20_lut_array[11] << 24 |
			 adr->param_adr_weight_20_lut_array[10] << 16 |
			 adr->param_adr_weight_20_lut_array[9] << 8 |
			 adr->param_adr_weight_20_lut_array[8] <<  0);
	system_reg_write(((tisp_core_t *)adr->core)->priv_data, ADR_ADDR_WEILUT20_P3,
			 adr->param_adr_weight_20_lut_array[15] << 24 |
			 adr->param_adr_weight_20_lut_array[14] << 16 |
			 adr->param_adr_weight_20_lut_array[13] << 8 |
			 adr->param_adr_weight_20_lut_array[12] << 0);
	system_reg_write(((tisp_core_t *)adr->core)->priv_data, ADR_ADDR_WEILUT20_P4,
			 adr->param_adr_weight_20_lut_array[19] << 24 |
			 adr->param_adr_weight_20_lut_array[18] << 16 |
			 adr->param_adr_weight_20_lut_array[17] << 8 |
			 adr->param_adr_weight_20_lut_array[16] << 0);
	system_reg_write(((tisp_core_t *)adr->core)->priv_data, ADR_ADDR_WEILUT20_P5,
			 adr->param_adr_weight_20_lut_array[23] << 24 |
			 adr->param_adr_weight_20_lut_array[22] << 16 |
			 adr->param_adr_weight_20_lut_array[21] << 8 |
			 adr->param_adr_weight_20_lut_array[20] << 0);
	system_reg_write(((tisp_core_t *)adr->core)->priv_data, ADR_ADDR_WEILUT20_P6,
			 adr->param_adr_weight_20_lut_array[27] << 24 |
			 adr->param_adr_weight_20_lut_array[26] << 16 |
			 adr->param_adr_weight_20_lut_array[25] << 8 |
			 adr->param_adr_weight_20_lut_array[24] << 0);
	system_reg_write(((tisp_core_t *)adr->core)->priv_data, ADR_ADDR_WEILUT20_P7,
			 adr->param_adr_weight_20_lut_array[31] << 24 |
			 adr->param_adr_weight_20_lut_array[30] << 16 |
			 adr->param_adr_weight_20_lut_array[29] << 8 |
			 adr->param_adr_weight_20_lut_array[28] << 0);
	system_reg_write(((tisp_core_t *)adr->core)->priv_data, ADR_ADDR_WEILUT02_P0,
			 adr->param_adr_weight_02_lut_array[3] << 24 |
			 adr->param_adr_weight_02_lut_array[2] << 16 |
			 adr->param_adr_weight_02_lut_array[1] << 8 |
			 adr->param_adr_weight_02_lut_array[0] << 0);
	system_reg_write(((tisp_core_t *)adr->core)->priv_data, ADR_ADDR_WEILUT02_P1,
			 adr->param_adr_weight_02_lut_array[7] <<  24 |
			 adr->param_adr_weight_02_lut_array[6]  << 16 |
			 adr->param_adr_weight_02_lut_array[5] <<   8 |
			 adr->param_adr_weight_02_lut_array[4]  <<  0);
	system_reg_write(((tisp_core_t *)adr->core)->priv_data, ADR_ADDR_WEILUT02_P2,
			 adr->param_adr_weight_02_lut_array[11] << 24 |
			 adr->param_adr_weight_02_lut_array[10] << 16 |
			 adr->param_adr_weight_02_lut_array[9] << 8 |
			 adr->param_adr_weight_02_lut_array[8] << 0);
	system_reg_write(((tisp_core_t *)adr->core)->priv_data, ADR_ADDR_WEILUT02_P3,
			 adr->param_adr_weight_02_lut_array[15] << 24 |
			 adr->param_adr_weight_02_lut_array[14] << 16 |
			 adr->param_adr_weight_02_lut_array[13] << 8 |
			 adr->param_adr_weight_02_lut_array[12] << 0);
	system_reg_write(((tisp_core_t *)adr->core)->priv_data, ADR_ADDR_WEILUT02_P4,
			 adr->param_adr_weight_02_lut_array[19] << 24 |
			 adr->param_adr_weight_02_lut_array[18] << 16 |
			 adr->param_adr_weight_02_lut_array[17] << 8 |
			 adr->param_adr_weight_02_lut_array[16] << 0);
	system_reg_write(((tisp_core_t *)adr->core)->priv_data, ADR_ADDR_WEILUT02_P5,
			 adr->param_adr_weight_02_lut_array[23] << 24 |
			 adr->param_adr_weight_02_lut_array[22] << 16 |
			 adr->param_adr_weight_02_lut_array[21] << 8 |
			 adr->param_adr_weight_02_lut_array[20] << 0);
	system_reg_write(((tisp_core_t *)adr->core)->priv_data, ADR_ADDR_WEILUT02_P6,
			 adr->param_adr_weight_02_lut_array[27] << 24 |
			 adr->param_adr_weight_02_lut_array[26] << 16 |
			 adr->param_adr_weight_02_lut_array[25] << 8 |
			 adr->param_adr_weight_02_lut_array[24] << 0);
	system_reg_write(((tisp_core_t *)adr->core)->priv_data, ADR_ADDR_WEILUT02_P7,
			 adr->param_adr_weight_02_lut_array[31] << 24 |
			 adr->param_adr_weight_02_lut_array[30] << 16 |
			 adr->param_adr_weight_02_lut_array[29] << 8 |
			 adr->param_adr_weight_02_lut_array[28] << 0);
	system_reg_write(((tisp_core_t *)adr->core)->priv_data, ADR_ADDR_WEILUT12_P0,
			 adr->param_adr_weight_12_lut_array[3] << 24 |
			 adr->param_adr_weight_12_lut_array[2] << 16 |
			 adr->param_adr_weight_12_lut_array[1] << 8 |
			 adr->param_adr_weight_12_lut_array[0] << 0);
	system_reg_write(((tisp_core_t *)adr->core)->priv_data, ADR_ADDR_WEILUT12_P1,
			 adr->param_adr_weight_12_lut_array[7] << 24 |
			 adr->param_adr_weight_12_lut_array[6] << 16 |
			 adr->param_adr_weight_12_lut_array[5] << 8 |
			 adr->param_adr_weight_12_lut_array[4] << 0);
	system_reg_write(((tisp_core_t *)adr->core)->priv_data, ADR_ADDR_WEILUT12_P2,
			 adr->param_adr_weight_12_lut_array[11] << 24 |
			 adr->param_adr_weight_12_lut_array[10] << 16 |
			 adr->param_adr_weight_12_lut_array[9] << 8 |
			 adr->param_adr_weight_12_lut_array[8] << 0);
	system_reg_write(((tisp_core_t *)adr->core)->priv_data, ADR_ADDR_WEILUT12_P3,
			 adr->param_adr_weight_12_lut_array[15] << 24 |
			 adr->param_adr_weight_12_lut_array[14] << 16 |
			 adr->param_adr_weight_12_lut_array[13] << 8 |
			 adr->param_adr_weight_12_lut_array[12] << 0);
	system_reg_write(((tisp_core_t *)adr->core)->priv_data, ADR_ADDR_WEILUT12_P4,
			 adr->param_adr_weight_12_lut_array[19] << 24 |
			 adr->param_adr_weight_12_lut_array[18] << 16 |
			 adr->param_adr_weight_12_lut_array[17] << 8 |
			 adr->param_adr_weight_12_lut_array[16] << 0);
	system_reg_write(((tisp_core_t *)adr->core)->priv_data, ADR_ADDR_WEILUT12_P5,
			 adr->param_adr_weight_12_lut_array[23] << 24 |
			 adr->param_adr_weight_12_lut_array[22] << 16 |
			 adr->param_adr_weight_12_lut_array[21] << 8 |
			 adr->param_adr_weight_12_lut_array[20] << 0);
	system_reg_write(((tisp_core_t *)adr->core)->priv_data, ADR_ADDR_WEILUT12_P6,
			 adr->param_adr_weight_12_lut_array[27] << 24 |
			 adr->param_adr_weight_12_lut_array[26] << 16 |
			 adr->param_adr_weight_12_lut_array[25] << 8 |
			 adr->param_adr_weight_12_lut_array[24] << 0);
	system_reg_write(((tisp_core_t *)adr->core)->priv_data, ADR_ADDR_WEILUT12_P7,
			 adr->param_adr_weight_12_lut_array[31] << 24 |
			 adr->param_adr_weight_12_lut_array[30] << 16 |
			 adr->param_adr_weight_12_lut_array[29] << 8 |
			 adr->param_adr_weight_12_lut_array[28] << 0);
	system_reg_write(((tisp_core_t *)adr->core)->priv_data, ADR_ADDR_WEILUT22_P0,
			 adr->param_adr_weight_22_lut_array[3] << 24 |
			 adr->param_adr_weight_22_lut_array[2] << 16 |
			 adr->param_adr_weight_22_lut_array[1] << 8 |
			 adr->param_adr_weight_22_lut_array[0] << 0);
	system_reg_write(((tisp_core_t *)adr->core)->priv_data, ADR_ADDR_WEILUT22_P1,
			 adr->param_adr_weight_22_lut_array[7] <<  24 |
			 adr->param_adr_weight_22_lut_array[6]  << 16 |
			 adr->param_adr_weight_22_lut_array[5] <<   8 |
			 adr->param_adr_weight_22_lut_array[4]  <<  0);
	system_reg_write(((tisp_core_t *)adr->core)->priv_data, ADR_ADDR_WEILUT22_P2,
			 adr->param_adr_weight_22_lut_array[11] << 24 |
			 adr->param_adr_weight_22_lut_array[10] << 16 |
			 adr->param_adr_weight_22_lut_array[9] << 8 |
			 adr->param_adr_weight_22_lut_array[8] << 0);
	system_reg_write(((tisp_core_t *)adr->core)->priv_data, ADR_ADDR_WEILUT22_P3,
			 adr->param_adr_weight_22_lut_array[15] << 24 |
			 adr->param_adr_weight_22_lut_array[14] << 16 |
			 adr->param_adr_weight_22_lut_array[13] << 8 |
			 adr->param_adr_weight_22_lut_array[12] << 0);
	system_reg_write(((tisp_core_t *)adr->core)->priv_data, ADR_ADDR_WEILUT22_P4,
			 adr->param_adr_weight_22_lut_array[19] << 24 |
			 adr->param_adr_weight_22_lut_array[18] << 16 |
			 adr->param_adr_weight_22_lut_array[17] << 8 |
			 adr->param_adr_weight_22_lut_array[16] << 0);
	system_reg_write(((tisp_core_t *)adr->core)->priv_data, ADR_ADDR_WEILUT22_P5,
			 adr->param_adr_weight_22_lut_array[23] << 24 |
			 adr->param_adr_weight_22_lut_array[22] << 16 |
			 adr->param_adr_weight_22_lut_array[21] << 8 |
			 adr->param_adr_weight_22_lut_array[20] << 0);
	system_reg_write(((tisp_core_t *)adr->core)->priv_data, ADR_ADDR_WEILUT22_P6,
			 adr->param_adr_weight_22_lut_array[27] << 24 |
			 adr->param_adr_weight_22_lut_array[26] << 16 |
			 adr->param_adr_weight_22_lut_array[25] << 8 |
			 adr->param_adr_weight_22_lut_array[24] << 0);
	system_reg_write(((tisp_core_t *)adr->core)->priv_data, ADR_ADDR_WEILUT22_P7,
			 adr->param_adr_weight_22_lut_array[31] << 24 |
			 adr->param_adr_weight_22_lut_array[30] << 16 |
			 adr->param_adr_weight_22_lut_array[29] << 8 |
			 adr->param_adr_weight_22_lut_array[28] << 0);
	system_reg_write(((tisp_core_t *)adr->core)->priv_data, ADR_ADDR_WEILUT21_P0,
			 adr->param_adr_weight_21_lut_array[3] << 24 |
			 adr->param_adr_weight_21_lut_array[2] << 16 |
			 adr->param_adr_weight_21_lut_array[1] << 8 |
			 adr->param_adr_weight_21_lut_array[0] << 0);
	system_reg_write(((tisp_core_t *)adr->core)->priv_data, ADR_ADDR_WEILUT21_P1,
			 adr->param_adr_weight_21_lut_array[7] << 24 |
			 adr->param_adr_weight_21_lut_array[6] << 16 |
			 adr->param_adr_weight_21_lut_array[5] << 8 |
			 adr->param_adr_weight_21_lut_array[4] << 0);
	system_reg_write(((tisp_core_t *)adr->core)->priv_data, ADR_ADDR_WEILUT21_P2,
			 adr->param_adr_weight_21_lut_array[11] << 24 |
			 adr->param_adr_weight_21_lut_array[10] << 16 |
			 adr->param_adr_weight_21_lut_array[9] << 8 |
			 adr->param_adr_weight_21_lut_array[8] << 0);
	system_reg_write(((tisp_core_t *)adr->core)->priv_data, ADR_ADDR_WEILUT21_P3,
			 adr->param_adr_weight_21_lut_array[15] << 24 |
			 adr->param_adr_weight_21_lut_array[14] << 16 |
			 adr->param_adr_weight_21_lut_array[13] << 8 |
			 adr->param_adr_weight_21_lut_array[12] << 0);
	system_reg_write(((tisp_core_t *)adr->core)->priv_data, ADR_ADDR_WEILUT21_P4,
			 adr->param_adr_weight_21_lut_array[19] << 24 |
			 adr->param_adr_weight_21_lut_array[18] << 16 |
			 adr->param_adr_weight_21_lut_array[17] << 8 |
			 adr->param_adr_weight_21_lut_array[16] << 0);
	system_reg_write(((tisp_core_t *)adr->core)->priv_data, ADR_ADDR_WEILUT21_P5,
			 adr->param_adr_weight_21_lut_array[23] << 24 |
			 adr->param_adr_weight_21_lut_array[22] << 16 |
			 adr->param_adr_weight_21_lut_array[21] << 8 |
			 adr->param_adr_weight_21_lut_array[20] << 0);
	system_reg_write(((tisp_core_t *)adr->core)->priv_data, ADR_ADDR_WEILUT21_P6,
			 adr->param_adr_weight_21_lut_array[27] << 24 |
			 adr->param_adr_weight_21_lut_array[26] << 16 |
			 adr->param_adr_weight_21_lut_array[25] << 8 |
			 adr->param_adr_weight_21_lut_array[24] << 0);
	system_reg_write(((tisp_core_t *)adr->core)->priv_data, ADR_ADDR_WEILUT21_P7,
			 adr->param_adr_weight_21_lut_array[31] << 24 |
			 adr->param_adr_weight_21_lut_array[30] << 16 |
			 adr->param_adr_weight_21_lut_array[29] << 8 |
			 adr->param_adr_weight_21_lut_array[28] << 0);
	system_reg_write(((tisp_core_t *)adr->core)->priv_data, ADR_ADDR_MIN_KPOINT_X_P0,
			 adr->param_adr_min_kneepoint_array[1] << 16 |
			 adr->param_adr_min_kneepoint_array[0] << 0);
	system_reg_write(((tisp_core_t *)adr->core)->priv_data, ADR_ADDR_MIN_KPOINT_X_P1,
			 adr->param_adr_min_kneepoint_array[3] <<16 |
			 adr->param_adr_min_kneepoint_array[2] << 0);
	system_reg_write(((tisp_core_t *)adr->core)->priv_data, ADR_ADDR_MIN_KPOINT_X_P2,
			 adr->param_adr_min_kneepoint_array[5] << 16 |
			 adr->param_adr_min_kneepoint_array[4] << 0);
	system_reg_write(((tisp_core_t *)adr->core)->priv_data, ADR_ADDR_MIN_KPOINT_X_P3,
			 adr->param_adr_min_kneepoint_array[7] << 16 |
			 adr->param_adr_min_kneepoint_array[6] << 0);
	system_reg_write(((tisp_core_t *)adr->core)->priv_data, ADR_ADDR_MIN_KPOINT_X_P4,
			 adr->param_adr_min_kneepoint_array[9] << 16 |
			 adr->param_adr_min_kneepoint_array[8] << 0);
	system_reg_write(((tisp_core_t *)adr->core)->priv_data, ADR_ADDR_MIN_KPOINT_X_P5,
			 adr->param_adr_min_kneepoint_array[10] << 0);
	system_reg_write(((tisp_core_t *)adr->core)->priv_data, ADR_ADDR_MIN_KPOINT_POW_P0,
			 adr->param_adr_min_kneepoint_array[14] << 24 |
			 adr->param_adr_min_kneepoint_array[13] << 16 |
			 adr->param_adr_min_kneepoint_array[12] << 8 |
			 adr->param_adr_min_kneepoint_array[11] << 0);
	system_reg_write(((tisp_core_t *)adr->core)->priv_data, ADR_ADDR_MIN_KPOINT_POW_P1,
			 adr->param_adr_min_kneepoint_array[18] << 24 |
			 adr->param_adr_min_kneepoint_array[17] << 16 |
			 adr->param_adr_min_kneepoint_array[16] << 8 |
			 adr->param_adr_min_kneepoint_array[15] << 0);
	system_reg_write(((tisp_core_t *)adr->core)->priv_data, ADR_ADDR_MIN_KPOINT_POW_P2,
			 adr->param_adr_min_kneepoint_array[22] << 24 |
			 adr->param_adr_min_kneepoint_array[21] << 16 |
			 adr->param_adr_min_kneepoint_array[20] << 8 |
			 adr->param_adr_min_kneepoint_array[19] << 0);
	system_reg_write(((tisp_core_t *)adr->core)->priv_data, ADR_ADDR_COC_KPOINT_X_P0,
			 adr->param_adr_coc_kneepoint_array[1] << 16 |
			 adr->param_adr_coc_kneepoint_array[0] << 0);
	system_reg_write(((tisp_core_t *)adr->core)->priv_data, ADR_ADDR_COC_KPOINT_X_P1,
			 adr->param_adr_coc_kneepoint_array[3] << 16 |
			 adr->param_adr_coc_kneepoint_array[2] << 0);
	system_reg_write(((tisp_core_t *)adr->core)->priv_data, ADR_ADDR_COC_KPOINT_X_P2,
			 adr->param_adr_coc_kneepoint_array[5] << 16 |
			 adr->param_adr_coc_kneepoint_array[4] << 0);
	system_reg_write(((tisp_core_t *)adr->core)->priv_data, ADR_ADDR_COC_KPOINT_X_P3,
			 adr->param_adr_coc_kneepoint_array[7] << 16 |
			 adr->param_adr_coc_kneepoint_array[6] << 0);
	system_reg_write(((tisp_core_t *)adr->core)->priv_data, ADR_ADDR_COC_KPOINT_X_P4,
			 adr->param_adr_coc_kneepoint_array[9] << 16 |
			 adr->param_adr_coc_kneepoint_array[8] << 0);
	system_reg_write(((tisp_core_t *)adr->core)->priv_data, ADR_ADDR_COC_KPOINT_X_P5,
			 adr->param_adr_coc_kneepoint_array[10] << 0);
	system_reg_write(((tisp_core_t *)adr->core)->priv_data, ADR_ADDR_COC_KPOINT_POW_P0,
			 adr->param_adr_coc_kneepoint_array[14] << 24 |
			 adr->param_adr_coc_kneepoint_array[13] << 16 |
			 adr->param_adr_coc_kneepoint_array[12] << 8 |
			 adr->param_adr_coc_kneepoint_array[11] << 0);
	system_reg_write(((tisp_core_t *)adr->core)->priv_data, ADR_ADDR_COC_KPOINT_POW_P1,
			 adr->param_adr_coc_kneepoint_array[18] << 24 |
			 adr->param_adr_coc_kneepoint_array[17] << 16 |
			 adr->param_adr_coc_kneepoint_array[16] << 8 |
			 adr->param_adr_coc_kneepoint_array[15] << 0);
	system_reg_write(((tisp_core_t *)adr->core)->priv_data, ADR_ADDR_COC_KPOINT_POW_P2,
			 adr->param_adr_coc_kneepoint_array[20] << 8 |
			 adr->param_adr_coc_kneepoint_array[19] << 0);
	system_reg_write(((tisp_core_t *)adr->core)->priv_data, ADR_ADDR_COC_THRES,
			 adr->param_adr_ct_par_array[5] << 16 |
			 adr->param_adr_ct_par_array[4] << 0);
	system_reg_write(((tisp_core_t *)adr->core)->priv_data, ADR_ADDR_COC_STREN,
			 adr->param_adr_ct_par_array[7] << 16 |
			 adr->param_adr_ct_par_array[6] << 0);
	system_reg_write(((tisp_core_t *)adr->core)->priv_data, ADR_ADDR_COC_MODE,
			 adr->param_adr_ct_par_array[3] << 0);
	system_reg_write(((tisp_core_t *)adr->core)->priv_data, ADR_ADDR_STAT_BLOCK_DIFF_P_0,
			 adr->param_adr_stat_block_hist_diff_array[1] << 16 |
			 adr->param_adr_stat_block_hist_diff_array[0] << 0);
	system_reg_write(((tisp_core_t *)adr->core)->priv_data, ADR_ADDR_STAT_BLOCK_DIFF_P_1,
			 adr->param_adr_stat_block_hist_diff_array[3] << 16 |
			 adr->param_adr_stat_block_hist_diff_array[2] << 0);

	return 0;
}

int32_t tiziano_adr_gamma_refresh(tisp_adr_t *adr)
{
	tisp_core_t *core = (tisp_core_t *)adr->core;
	uint32_t i,j;
	uint32_t plen = 0;

	tisp_gamma_param_array_get(&core->gamma, TISP_PARAM_GAMMA_RGB_LUT_SADDR, adr->adr_gam_y, &plen);
	if(plen != sizeof(adr->adr_gam_y)) {
		printk("get gamma error!!!");
		return -1;
	}
	for (j = 0; j < 9; j++) {
		for (i = 0; i < 129; i++) {
			if (adr->histSub_4096[j] <= adr->adr_gam_y[i]) {
				if (i == 0) {
					adr->histSub_4096_out[j] = adr->adr_gam_x[0];
					break;
				}else{
					adr->histSub_4096_out[j] = ISPINT(adr->histSub_4096[j],
							adr->adr_gam_y[i-1],adr->adr_gam_y[i],
							adr->adr_gam_x[i-1],adr->adr_gam_x[i]);
					break;
				}
			}
		}
		if (i == 129) {
			adr->histSub_4096_out[j] = adr->adr_gam_x[128];
		}
		adr->adr_tm_base_lut[j] = adr->histSub_4096_out[j];
	}
	for (j = 0; j < 8; j++) {
		adr->histSub_4096_diff[j] = adr->histSub_4096_out[j+1] - adr->histSub_4096_out[j];
	}

	return 0;
}

int32_t tiziano_adr_params_refresh(tisp_adr_t *adr)
{
	memcpy(adr->param_adr_ct_par_array,
	       tparams.params_data.TISP_PARAM_ADR_CT_PAR_ARRAY,
	       sizeof(tparams.params_data.TISP_PARAM_ADR_CT_PAR_ARRAY));
	memcpy(adr->param_adr_weight_20_lut_array,
	       tparams.params_data.TISP_PARAM_ADR_WEIGHT_20_LUT_ARRAY,
	       sizeof(tparams.params_data.TISP_PARAM_ADR_WEIGHT_20_LUT_ARRAY));
	memcpy(adr->param_adr_weight_02_lut_array,
	       tparams.params_data.TISP_PARAM_ADR_WEIGHT_02_LUT_ARRAY,
	       sizeof(tparams.params_data.TISP_PARAM_ADR_WEIGHT_02_LUT_ARRAY));
	memcpy(adr->param_adr_weight_12_lut_array,
	       tparams.params_data.TISP_PARAM_ADR_WEIGHT_12_LUT_ARRAY,
	       sizeof(tparams.params_data.TISP_PARAM_ADR_WEIGHT_12_LUT_ARRAY));
	memcpy(adr->param_adr_weight_22_lut_array,
	       tparams.params_data.TISP_PARAM_ADR_WEIGHT_22_LUT_ARRAY,
	       sizeof(tparams.params_data.TISP_PARAM_ADR_WEIGHT_22_LUT_ARRAY));
	memcpy(adr->param_adr_weight_21_lut_array,
	       tparams.params_data.TISP_PARAM_ADR_WEIGHT_21_LUT_ARRAY,
	       sizeof(tparams.params_data.TISP_PARAM_ADR_WEIGHT_21_LUT_ARRAY));
	memcpy(adr->param_adr_ctc_kneepoint_array,
	       tparams.params_data.TISP_PARAM_ADR_CTC_KNEEPOINT_ARRAY,
	       sizeof(tparams.params_data.TISP_PARAM_ADR_CTC_KNEEPOINT_ARRAY));
	memcpy(adr->param_adr_min_kneepoint_array,
	       tparams.params_data.TISP_PARAM_ADR_MIN_KNEEPOINT_ARRAY,
	       sizeof(tparams.params_data.TISP_PARAM_ADR_MIN_KNEEPOINT_ARRAY));
	memcpy(adr->param_adr_map_kneepoint_array,
	       tparams.params_data.TISP_PARAM_ADR_MAP_KNEEPOINT_ARRAY,
	       sizeof(tparams.params_data.TISP_PARAM_ADR_MAP_KNEEPOINT_ARRAY));
	memcpy(adr->param_adr_coc_kneepoint_array,
	       tparams.params_data.TISP_PARAM_ADR_COC_KNEEPOINT_ARRAY,
	       sizeof(tparams.params_data.TISP_PARAM_ADR_COC_KNEEPOINT_ARRAY));
	memcpy(adr->param_adr_centre_w_dis_array,
	       tparams.params_data.TISP_PARAM_ADR_CENTRE_W_DIS_ARRAY,
	       sizeof(tparams.params_data.TISP_PARAM_ADR_CENTRE_W_DIS_ARRAY));
	memcpy(adr->param_adr_contrast_w_dis_array,
	       tparams.params_data.TISP_PARAM_ADR_CONTRAST_W_DIS_ARRAY,
	       sizeof(tparams.params_data.TISP_PARAM_ADR_CONTRAST_W_DIS_ARRAY));
	memcpy(adr->param_adr_stat_block_hist_diff_array,
	       tparams.params_data.TISP_PARAM_ADR_STAT_BLOCK_HIST_DIFF_ARRAY,
	       sizeof(tparams.params_data.TISP_PARAM_ADR_STAT_BLOCK_HIST_DIFF_ARRAY));
	memcpy(adr->adr_tm_base_lut,
	       tparams.params_data.TISP_PARAM_ADR_SOFT_TM_BASE_ARRAY,
	       sizeof(tparams.params_data.TISP_PARAM_ADR_SOFT_TM_BASE_ARRAY));
	memcpy(adr->adr_gam_x,
	       tparams.params_data.TISP_PARAM_ADR_SOFT_GAM_X_ARRAY,
	       sizeof(tparams.params_data.TISP_PARAM_ADR_SOFT_GAM_X_ARRAY));
	memcpy(adr->adr_gam_y,
	       tparams.params_data.TISP_PARAM_ADR_SOFT_GAM_Y_ARRAY,
	       sizeof(tparams.params_data.TISP_PARAM_ADR_SOFT_GAM_Y_ARRAY));
	memcpy(adr->adr_light_end,
	       tparams.params_data.TISP_PARAM_ADR_SOFT_LIGHT_END_ARRAY,
	       sizeof(tparams.params_data.TISP_PARAM_ADR_SOFT_LIGHT_END_ARRAY));
	memcpy(adr->adr_block_light,
	       tparams.params_data.TISP_PARAM_ADR_SOFT_BLOCK_LIGHT_ARRAY,
	       sizeof(tparams.params_data.TISP_PARAM_ADR_SOFT_BLOCK_LIGHT_ARRAY));
	memcpy(adr->adr_map_mode,
	       tparams.params_data.TISP_PARAM_ADR_SOFT_MAP_MODE_ARRAY,
	       sizeof(tparams.params_data.TISP_PARAM_ADR_SOFT_MAP_MODE_ARRAY));
	memcpy(adr->adr_ev_list,
	       tparams.params_data.TISP_PARAM_ADR_EV_LIST,
	       sizeof(tparams.params_data.TISP_PARAM_ADR_EV_LIST));
	memcpy(adr->adr_ligb_list,
	       tparams.params_data.TISP_PARAM_ADR_LIGB_LIST,
	       sizeof(tparams.params_data.TISP_PARAM_ADR_LIGB_LIST));
	memcpy(adr->adr_mapb1_list,
	       tparams.params_data.TISP_PARAM_ADR_MAPB1_LIST,
	       sizeof(tparams.params_data.TISP_PARAM_ADR_MAPB1_LIST));
	memcpy(adr->adr_mapb2_list,
	       tparams.params_data.TISP_PARAM_ADR_MAPB2_LIST,
	       sizeof(tparams.params_data.TISP_PARAM_ADR_MAPB2_LIST));
	memcpy(adr->adr_mapb3_list,
	       tparams.params_data.TISP_PARAM_ADR_MAPB3_LIST,
	       sizeof(tparams.params_data.TISP_PARAM_ADR_MAPB3_LIST));
	memcpy(adr->adr_mapb4_list,
	       tparams.params_data.TISP_PARAM_ADR_MAPB4_LIST,
	       sizeof(tparams.params_data.TISP_PARAM_ADR_MAPB4_LIST));
	memcpy(adr->adr_blp2_list,
	       tparams.params_data.TISP_PARAM_ADR_BLP2_LIST,
	       sizeof(tparams.params_data.TISP_PARAM_ADR_BLP2_LIST));

	adr->adr_block_light[8] = adr->width_def;
	adr->adr_block_light[9] = adr->height_def;

	tiziano_adr_gamma_refresh(adr);

	return 0;
}

int32_t tiziano_adr_dn_params_refresh(tisp_adr_t *adr)
{
	tiziano_adr_params_refresh(adr);

	return 0;
}

int32_t tiziano_adr_init(tisp_adr_t *adr, uint32_t width,uint32_t height)
{
	uint32_t i,j;
	uint32_t width_step;
	uint32_t height_step;
	tisp_core_t *core = adr->core;

	width_step = width/5;
	height_step = height/4;

	adr->adr_block_light[8] = width;
	adr->adr_block_light[9] = height;


	/* init default value. */
	adr->width_def = width;
	adr->height_def = height;
	memcpy(adr->ctc_kneepoint_x, ctc_kneepoint_x, sizeof(ctc_kneepoint_x));
	memcpy(adr->ctc_kneepoint_mux, ctc_kneepoint_mux, sizeof(ctc_kneepoint_mux));
	memcpy(adr->min_kneepoint_x, min_kneepoint_x, sizeof(min_kneepoint_x));
	memcpy(adr->min_kneepoint_y, min_kneepoint_y, sizeof(min_kneepoint_y));
	memcpy(adr->min_kneepoint_pow, min_kneepoint_pow, sizeof(min_kneepoint_pow));
	memcpy(adr->map_kneepoint_x, map_kneepoint_x, sizeof(map_kneepoint_x));
	memcpy(adr->map_kneepoint_y, map_kneepoint_y, sizeof(map_kneepoint_y));
	memcpy(adr->coc_kneepoint_x, coc_kneepoint_x, sizeof(coc_kneepoint_x));
	memcpy(adr->coc_kneepoint_pow, coc_kneepoint_pow, sizeof(coc_kneepoint_pow));
	memcpy(adr->centre_w_distance, centre_w_distance, sizeof(centre_w_distance));
	memcpy(adr->contrast_w_distance, contrast_w_distance, sizeof(contrast_w_distance));
	memcpy(adr->adr_stat_block_hist_diff, adr_stat_block_hist_diff, sizeof(adr_stat_block_hist_diff));

	memcpy(adr->adr_stat_block_hist_diff, adr_stat_block_hist_diff, sizeof(adr_stat_block_hist_diff));

	memcpy(adr->adr_tm_base_lut, adr_tm_base_lut, sizeof(adr_tm_base_lut));
	memcpy(adr->adr_gam_x, adr_gam_x, sizeof(adr_gam_x));
	memcpy(adr->adr_gam_y, adr_gam_y, sizeof(adr_gam_y));
	memcpy(adr->adr_light_end, adr_light_end, sizeof(adr_light_end));
	memcpy(adr->adr_block_light, adr_block_light, sizeof(adr_block_light));
	memcpy(adr->adr_map_mode, adr_map_mode, sizeof(adr_map_mode));
	memcpy(adr->histSub_4096, histSub_4096, sizeof(histSub_4096));
	memcpy(adr->histSub_4096_diff, histSub_4096_diff, sizeof(histSub_4096_diff));



	tiziano_adr_params_refresh(adr);
	system_reg_write(((tisp_core_t *)adr->core)->priv_data, ADR_ADDR_BLK_COORD_M_P_0, 1 * height_step << 16 | 0*height_step << 0);
	system_reg_write(((tisp_core_t *)adr->core)->priv_data, ADR_ADDR_BLK_COORD_M_P_1, 3 * height_step << 16 | 2*height_step << 0);
	system_reg_write(((tisp_core_t *)adr->core)->priv_data, ADR_ADDR_BLK_COORD_M_P_2, 4 * height_step << 0);
	system_reg_write(((tisp_core_t *)adr->core)->priv_data, ADR_ADDR_BLK_COORD_N_P_0, 1 * width_step << 16 | 0*width_step << 0);
	system_reg_write(((tisp_core_t *)adr->core)->priv_data, ADR_ADDR_BLK_COORD_N_P_1, 3 * width_step << 16 | 2*width_step << 0);
	system_reg_write(((tisp_core_t *)adr->core)->priv_data, ADR_ADDR_BLK_COORD_N_P_2, 5 * width_step << 16 | 4*width_step << 0);

	for (j = 0; j < 20; j++) {
		for (i = 0; i < 5; i++){
			system_reg_write(((tisp_core_t *)adr->core)->priv_data, ADR_ADDR_MAP_KPOINT_Y_P00_0 +
					 (j * 24) + i * 4,adr->map_kneepoint_y[j * 11 + i * 2 + 1]
					 << 16 | adr->map_kneepoint_y[j * 11 + i * 2 + 0] <<  0);
		}
		system_reg_write(((tisp_core_t *)adr->core)->priv_data, ADR_ADDR_MAP_KPOINT_Y_P00_5 +
				 j * 24,adr->map_kneepoint_y[j * 11 + 10] << 0);
	}

	for (i = 0; i < 5; i++) {
		system_reg_write(((tisp_core_t *)adr->core)->priv_data, ADR_ADDR_MIN_KPOINT_Y_P0 + i * 4,
				 adr->min_kneepoint_y[i * 2 + 1] << 16 |
				 adr->min_kneepoint_y[i * 2 + 0] <<  0);
	}
	system_reg_write(((tisp_core_t *)adr->core)->priv_data, ADR_ADDR_MIN_KPOINT_Y_P5, adr->min_kneepoint_y[10] << 0);
	system_reg_write(((tisp_core_t *)adr->core)->priv_data, ADR_ADDR_CTC_KPOINT_X_P0,
			 adr->param_adr_ctc_kneepoint_array[2] << 16 |
			 adr->param_adr_ctc_kneepoint_array[1] << 0);
	system_reg_write(((tisp_core_t *)adr->core)->priv_data, ADR_ADDR_CTC_KPOINT_X_P1,
			 adr->param_adr_ctc_kneepoint_array[3] << 0);
	system_reg_write(((tisp_core_t *)adr->core)->priv_data, ADR_ADDR_CTC_KPOINT_MUX_P0,
			 adr->param_adr_ctc_kneepoint_array[6] << 0);
	system_reg_write(((tisp_core_t *)adr->core)->priv_data, ADR_ADDR_CTC_KPOINT_MUX_P1,
			 adr->param_adr_ctc_kneepoint_array[7] << 0);

	for (i = 0; i < 32; i++) {
		system_reg_write(((tisp_core_t *)adr->core)->priv_data, ADR_ADDR_CON_W_DIS_P00 + i * 4,
				 adr->param_adr_contrast_w_dis_array[i] << 0);
	}

	system_reg_write(((tisp_core_t *)adr->core)->priv_data, ADR_ADDR_CTC_RATIO, adr->param_adr_ct_par_array[2] << 16 |
			 adr->param_adr_ct_par_array[1] << 8 | adr->param_adr_ct_par_array[0] << 0);

	tiziano_adr_params_init(adr);
	system_irq_func_set(((tisp_core_t *)adr->core)->priv_data, TIZIANO_ISP_IRQ_ADR_STATIC,
			    tiziano_adr_interrupt_static, adr);
	tisp_event_set_cb(&core->tisp_event_mg, TISP_EVENT_TYPE_ADR_PROCESS,tisp_adr_process, adr);

	return 0;
}

int32_t tisp_adr_param_array_get(tisp_adr_t *adr, int32_t id,void *buf,uint32_t *plen)
{
	uint32_t len;
	void *tmpbuf;

	switch (id) {
	case TISP_PARAM_ADR_CT_PAR_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_ADR_CT_PAR_ARRAY);
		tmpbuf = adr->param_adr_ct_par_array;
		break;
	case TISP_PARAM_ADR_WEIGHT_20_LUT_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_ADR_WEIGHT_20_LUT_ARRAY);
		tmpbuf = adr->param_adr_weight_20_lut_array;
		break;
	case TISP_PARAM_ADR_WEIGHT_02_LUT_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_ADR_WEIGHT_02_LUT_ARRAY);
		tmpbuf = adr->param_adr_weight_02_lut_array;
		break;
	case TISP_PARAM_ADR_WEIGHT_12_LUT_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_ADR_WEIGHT_12_LUT_ARRAY);
		tmpbuf = adr->param_adr_weight_12_lut_array;
		break;
	case TISP_PARAM_ADR_WEIGHT_22_LUT_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_ADR_WEIGHT_22_LUT_ARRAY);
		tmpbuf = adr->param_adr_weight_22_lut_array;
		break;
	case TISP_PARAM_ADR_WEIGHT_21_LUT_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_ADR_WEIGHT_21_LUT_ARRAY);
		tmpbuf = adr->param_adr_weight_21_lut_array;
		break;
	case TISP_PARAM_ADR_CTC_KNEEPOINT_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_ADR_CTC_KNEEPOINT_ARRAY);
		tmpbuf = adr->param_adr_ctc_kneepoint_array;
		break;
	case TISP_PARAM_ADR_MIN_KNEEPOINT_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_ADR_MIN_KNEEPOINT_ARRAY);
		tmpbuf = adr->param_adr_min_kneepoint_array;
		break;
	case TISP_PARAM_ADR_MAP_KNEEPOINT_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_ADR_MAP_KNEEPOINT_ARRAY);
		tmpbuf = adr->param_adr_map_kneepoint_array;
		break;
	case TISP_PARAM_ADR_COC_KNEEPOINT_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_ADR_COC_KNEEPOINT_ARRAY);
		tmpbuf = adr->param_adr_coc_kneepoint_array;
		break;
	case TISP_PARAM_ADR_CENTRE_W_DIS_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_ADR_CENTRE_W_DIS_ARRAY);
		tmpbuf = adr->param_adr_centre_w_dis_array;
		break;
	case TISP_PARAM_ADR_CONTRAST_W_DIS_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_ADR_CONTRAST_W_DIS_ARRAY);
		tmpbuf = adr->param_adr_contrast_w_dis_array;
		break;
	case TISP_PARAM_ADR_STAT_BLOCK_HIST_DIFF_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_ADR_STAT_BLOCK_HIST_DIFF_ARRAY);
		tmpbuf = adr->param_adr_stat_block_hist_diff_array;
		break;
	case TISP_PARAM_ADR_SOFT_TM_BASE_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_ADR_SOFT_TM_BASE_ARRAY);
		tmpbuf = adr->adr_tm_base_lut;
		break;
	case TISP_PARAM_ADR_SOFT_GAM_X_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_ADR_SOFT_GAM_X_ARRAY);
		tmpbuf = adr->adr_gam_x;
		break;
	case TISP_PARAM_ADR_SOFT_GAM_Y_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_ADR_SOFT_GAM_Y_ARRAY);
		tmpbuf = adr->adr_gam_y;
		break;
	case TISP_PARAM_ADR_SOFT_LIGHT_END_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_ADR_SOFT_LIGHT_END_ARRAY);
		tmpbuf = adr->adr_light_end;
		break;
	case TISP_PARAM_ADR_SOFT_BLOCK_LIGHT_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_ADR_SOFT_BLOCK_LIGHT_ARRAY);
		tmpbuf = adr->adr_block_light;
		break;
	case TISP_PARAM_ADR_SOFT_MAP_MODE_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_ADR_SOFT_MAP_MODE_ARRAY);
		tmpbuf = adr->adr_map_mode;
		break;
	case TISP_PARAM_ADR_EV_LIST:
		len = sizeof(tparams.params_data.TISP_PARAM_ADR_EV_LIST);
		tmpbuf = adr->adr_ev_list;
		break;
	case TISP_PARAM_ADR_LIGB_LIST:
		len = sizeof(tparams.params_data.TISP_PARAM_ADR_LIGB_LIST);
		tmpbuf = adr->adr_ligb_list;
		break;
	case TISP_PARAM_ADR_MAPB1_LIST:
		len = sizeof(tparams.params_data.TISP_PARAM_ADR_MAPB1_LIST);
		tmpbuf = adr->adr_mapb1_list;
		break;
	case TISP_PARAM_ADR_MAPB2_LIST:
		len = sizeof(tparams.params_data.TISP_PARAM_ADR_MAPB2_LIST);
		tmpbuf = adr->adr_mapb2_list;
		break;
	case TISP_PARAM_ADR_MAPB3_LIST:
		len = sizeof(tparams.params_data.TISP_PARAM_ADR_MAPB3_LIST);
		tmpbuf = adr->adr_mapb3_list;
		break;
	case TISP_PARAM_ADR_MAPB4_LIST:
		len = sizeof(tparams.params_data.TISP_PARAM_ADR_MAPB4_LIST);
		tmpbuf = adr->adr_mapb4_list;
		break;
	case TISP_PARAM_ADR_BLP2_LIST:
		len = sizeof(tparams.params_data.TISP_PARAM_ADR_BLP2_LIST);
		tmpbuf = adr->adr_blp2_list;
		break;
	default:
		ISP_ERROR("%s,%d: gib not support param id %d\n",__func__,__LINE__,id);
		return -1;
	}

	memcpy(buf,tmpbuf,len);
	*plen = len;

	return 0;
}

int32_t tisp_adr_param_array_set(tisp_adr_t *adr, int32_t id,void *buf,uint32_t *plen)
{
	uint32_t len;
	void *tmpbuf;

	switch (id) {
	case TISP_PARAM_ADR_CT_PAR_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_ADR_CT_PAR_ARRAY);
		tmpbuf = adr->param_adr_ct_par_array;
		memcpy(tmpbuf,buf,len);
		break;
	case TISP_PARAM_ADR_WEIGHT_20_LUT_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_ADR_WEIGHT_20_LUT_ARRAY);
		tmpbuf = adr->param_adr_weight_20_lut_array;
		memcpy(tmpbuf,buf,len);
		break;
	case TISP_PARAM_ADR_WEIGHT_02_LUT_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_ADR_WEIGHT_02_LUT_ARRAY);
		tmpbuf = adr->param_adr_weight_02_lut_array;
		memcpy(tmpbuf,buf,len);
		break;
	case TISP_PARAM_ADR_WEIGHT_12_LUT_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_ADR_WEIGHT_12_LUT_ARRAY);
		tmpbuf = adr->param_adr_weight_12_lut_array;
		memcpy(tmpbuf,buf,len);
		break;
	case TISP_PARAM_ADR_WEIGHT_22_LUT_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_ADR_WEIGHT_22_LUT_ARRAY);
		tmpbuf = adr->param_adr_weight_22_lut_array;
		memcpy(tmpbuf,buf,len);
		break;
	case TISP_PARAM_ADR_WEIGHT_21_LUT_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_ADR_WEIGHT_21_LUT_ARRAY);
		tmpbuf = adr->param_adr_weight_21_lut_array;
		memcpy(tmpbuf,buf,len);
		break;
	case TISP_PARAM_ADR_CTC_KNEEPOINT_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_ADR_CTC_KNEEPOINT_ARRAY);
		tmpbuf = adr->param_adr_ctc_kneepoint_array;
		memcpy(tmpbuf,buf,len);
		break;
	case TISP_PARAM_ADR_MIN_KNEEPOINT_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_ADR_MIN_KNEEPOINT_ARRAY);
		tmpbuf = adr->param_adr_min_kneepoint_array;
		memcpy(tmpbuf,buf,len);
		break;
	case TISP_PARAM_ADR_MAP_KNEEPOINT_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_ADR_MAP_KNEEPOINT_ARRAY);
		tmpbuf = adr->param_adr_map_kneepoint_array;
		memcpy(tmpbuf,buf,len);
		break;
	case TISP_PARAM_ADR_COC_KNEEPOINT_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_ADR_COC_KNEEPOINT_ARRAY);
		tmpbuf = adr->param_adr_coc_kneepoint_array;
		memcpy(tmpbuf,buf,len);
		break;
	case TISP_PARAM_ADR_CENTRE_W_DIS_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_ADR_CENTRE_W_DIS_ARRAY);
		tmpbuf = adr->param_adr_centre_w_dis_array;
		memcpy(tmpbuf,buf,len);
		break;
	case TISP_PARAM_ADR_CONTRAST_W_DIS_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_ADR_CONTRAST_W_DIS_ARRAY);
		tmpbuf = adr->param_adr_contrast_w_dis_array;
		memcpy(tmpbuf,buf,len);
		break;
	case TISP_PARAM_ADR_STAT_BLOCK_HIST_DIFF_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_ADR_STAT_BLOCK_HIST_DIFF_ARRAY);
		tmpbuf = adr->param_adr_stat_block_hist_diff_array;
		memcpy(tmpbuf,buf,len);
		break;
	case TISP_PARAM_ADR_SOFT_TM_BASE_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_ADR_SOFT_TM_BASE_ARRAY);
		tmpbuf = adr->adr_tm_base_lut;
		memcpy(tmpbuf,buf,len);
		break;
	case TISP_PARAM_ADR_SOFT_GAM_X_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_ADR_SOFT_GAM_X_ARRAY);
		tmpbuf = adr->adr_gam_x;
		memcpy(tmpbuf,buf,len);
		break;
	case TISP_PARAM_ADR_SOFT_GAM_Y_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_ADR_SOFT_GAM_Y_ARRAY);
		tmpbuf = adr->adr_gam_y;
		memcpy(tmpbuf,buf,len);
		break;
	case TISP_PARAM_ADR_SOFT_LIGHT_END_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_ADR_SOFT_LIGHT_END_ARRAY);
		tmpbuf = adr->adr_light_end;
		memcpy(tmpbuf,buf,len);
		break;
	case TISP_PARAM_ADR_SOFT_BLOCK_LIGHT_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_ADR_SOFT_BLOCK_LIGHT_ARRAY);
		tmpbuf = adr->adr_block_light;
		memcpy(tmpbuf,buf,len);
		break;
	case TISP_PARAM_ADR_SOFT_MAP_MODE_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_ADR_SOFT_MAP_MODE_ARRAY);
		tmpbuf = adr->adr_map_mode;
		memcpy(tmpbuf,buf,len);
		break;
	case TISP_PARAM_ADR_EV_LIST:
		len = sizeof(tparams.params_data.TISP_PARAM_ADR_EV_LIST);
		tmpbuf = adr->adr_ev_list;
		memcpy(tmpbuf,buf,len);
		break;
	case TISP_PARAM_ADR_LIGB_LIST:
		len = sizeof(tparams.params_data.TISP_PARAM_ADR_LIGB_LIST);
		tmpbuf = adr->adr_ligb_list;
		memcpy(tmpbuf,buf,len);
		break;
	case TISP_PARAM_ADR_MAPB1_LIST:
		len = sizeof(tparams.params_data.TISP_PARAM_ADR_MAPB1_LIST);
		tmpbuf = adr->adr_mapb1_list;
		memcpy(tmpbuf,buf,len);
		break;
	case TISP_PARAM_ADR_MAPB2_LIST:
		len = sizeof(tparams.params_data.TISP_PARAM_ADR_MAPB2_LIST);
		tmpbuf = adr->adr_mapb2_list;
		memcpy(tmpbuf,buf,len);
		break;
	case TISP_PARAM_ADR_MAPB3_LIST:
		len = sizeof(tparams.params_data.TISP_PARAM_ADR_MAPB3_LIST);
		tmpbuf = adr->adr_mapb3_list;
		memcpy(tmpbuf,buf,len);
		break;
	case TISP_PARAM_ADR_MAPB4_LIST:
		len = sizeof(tparams.params_data.TISP_PARAM_ADR_MAPB4_LIST);
		tmpbuf = adr->adr_mapb4_list;
		memcpy(tmpbuf,buf,len);
		break;
	case TISP_PARAM_ADR_BLP2_LIST:
		len = sizeof(tparams.params_data.TISP_PARAM_ADR_BLP2_LIST);
		tmpbuf = adr->adr_blp2_list;
		memcpy(tmpbuf,buf,len);
		tiziano_adr_params_init(adr);
		break;
	default:
		ISP_ERROR("%s,%d: Adr not support param id %d\n",__func__,__LINE__,id);
		return -1;
	}

	return 0;
}
