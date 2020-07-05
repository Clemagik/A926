#include <linux/kernel.h>
#include <linux/irqreturn.h>
#include <linux/dma-mapping.h>

#include "../inc/tiziano_sys.h"
#include "../inc/tiziano_core.h"

#include "tiziano_core.h"
#include "tiziano_awb.h"
#include "tiziano_event.h"
#include "tiziano_map.h"
#include "fix_point_calc.h"


#define ISPAWB_SUBABS(A,B)     ((B>A)?(B-A):(A-B))
#define ISPAWBROUND(fb)        ((uint32_t)1<<(fb-1))
#define ISPAWB_DELTA(A,B,C,D)  ( (unsigned int)((int)A + ((int)B - (int)C) / (int)(D)) )
/***************AWB PARAMETER******************/
//int _awb_parameter[45];
#if 1
unsigned int _awb_parameter[45] = {
	1, 15, 1, 15,      //v_start, zone_v_num, h_start, zone_h_num
	64,64,64,64,64,64,64,64,64,64,64,64,64,64,64, //h_zone pixel number
	36,36,36,36,36,36,36,36,36,36,36,36,36,36,36, //v_zone pixel number
	128, 512,  //rg min, rg max
	128, 512,  //bg min, bg max
	256, 0,    //k1, a1
	256, 4095, //k2, a2
	16, 240,   //Y threshold, y min, y max
	1          //awb frequency
};

unsigned int _pixel_cnt_th = 25;
unsigned int _awb_lowlight_rg_th[2]={65,390}; //{min,max}
unsigned int _AwbPointPos[2] ={10,0};
unsigned int _awb_cof[2] = {1, 4};
unsigned int _awb_mf_para[6] = {0, 0, 256, 256, 256, 256}; ////once
unsigned int _awb_mode[3] = {12, 100, 20000}; //_awb_sky_th _awb_outdoor_th  _awb_lowlight_th  //////ccm

// ce_detcet
unsigned int _awb_ct = {5000};  //once
unsigned int _awb_ct_last = 5000; //once
uint32_t  _wb_static[2] = {1772, 1836}; //{1.73046875, 1.79296875}
unsigned int _light_src[20] = {
	256, 229,
	212, 252,
	447, 154
};
unsigned int _light_src_num = 3;
unsigned int _rg_pos[15] = {168,183,201,218,236,256,271,289,307,325,342,360,378,395,432};
unsigned int _bg_pos[15] = {160,180,199,217,236,256,273,292,310,329,348,366,385,403,436};
unsigned int _awb_ct_th_ot_luxhigh[4] = {5000, 4500, 6500, 7000};
unsigned int _awb_ct_th_ot_luxlow[4] = {4800, 3800, 6500, 7000};
unsigned int _awb_ct_th_in[4] = {1000, 500, 12500, 13000};
unsigned int _awb_ct_para_ot[2] = {1, 8};
unsigned int _awb_ct_para_in[2] = {0, 8};
uint32_t _awb_dis_tw[3] = {0, 2048, 1536}; //_awb_dis_tw_en = 0;  _awb_dis_up_lim = 33554432; //2 _awb_dis_down_lim = 25165842; //1.5

unsigned int _rgbg_weight[225] = {
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 8, 57,
	0, 0, 0, 0, 0, 1, 3, 7, 9, 12, 19, 47, 114, 256, 236,
	0, 0, 0, 0, 2, 11, 30, 69, 98, 140, 256, 249, 236, 142, 13,
	0, 0, 0, 4, 21, 67, 126, 256, 241, 255, 210, 123, 41, 7, 0,
	0, 0, 2, 23, 107, 220, 256, 256, 152, 71, 25, 6, 1, 0, 0,
	0, 1, 9, 61, 216, 256, 249, 79, 11, 1, 0, 0, 0, 0, 0,
	0, 2, 24, 118, 256, 256, 68, 5, 0, 0, 0, 0, 0, 0, 0,
	1, 8, 61, 206, 256, 143, 15, 0, 0, 0, 0, 0, 0, 0, 0,
	2, 23, 123, 256, 256, 46, 2, 0, 0, 0, 0, 0, 0, 0, 0,
	3, 38, 198, 256, 182, 9, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	5, 70, 256, 256, 55, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	13, 256, 256, 132, 9, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	19, 195, 209, 31, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	20, 256, 99, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	59, 234, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

unsigned int _color_temp_mesh[225] = {
	220, 220, 229, 244, 260, 274, 274, 296, 319, 342, 364, 384, 405, 425, 435,
	220, 220, 220, 232, 248, 265, 274, 286, 309, 332, 353, 375, 395, 415, 435,
	220, 220, 220, 221, 236, 254, 267, 276, 299, 322, 344, 366, 386, 406, 435,
	218, 220, 220, 220, 226, 243, 256, 272, 290, 313, 334, 357, 378, 397, 435,
	205, 210, 216, 220, 220, 232, 245, 260, 280, 303, 325, 347, 368, 388, 435,
	192, 197, 202, 208, 213, 220, 233, 248, 264, 293, 314, 337, 359, 378, 421,
	181, 185, 191, 196, 202, 208, 213, 238, 254, 284, 306, 329, 351, 370, 413,
	168, 173, 178, 184, 189, 196, 200, 206, 242, 258, 296, 319, 342, 361, 403,
	150, 161, 166, 172, 177, 184, 188, 194, 200, 247, 262, 310, 332, 354, 395,
	131, 139, 149, 159, 165, 171, 176, 181, 187, 193, 250, 300, 323, 344, 385,
	112, 120, 130, 140, 150, 160, 163, 169, 175, 180, 185, 191, 313, 335, 376,
	97, 102, 112, 122, 132, 143, 152, 160, 163, 168, 174, 179, 185, 325, 368,
	90, 92, 94, 103, 113, 124, 133, 143, 153, 160, 161, 167, 172, 178, 358,
	81, 83, 85, 87, 90, 92, 115, 125, 135, 145, 155, 222, 160, 166, 353,
	75, 75, 75, 75, 75, 76, 78, 80, 83, 85, 122, 132, 142, 151, 336
};

unsigned int _awb_wght[225] = {
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1
};

unsigned int _rgbg_weight_ot[225] = {
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 45, 40, 25, 4, 2, 0, 0, 0, 0,
	0, 1, 2, 18, 39, 53, 193, 179, 150, 70, 1, 0, 0, 0, 0,
	5, 2, 81, 145, 179, 193, 232, 206, 90, 24, 1, 0, 0, 0, 0,
	13, 94, 172, 243, 255, 256, 240, 103, 26, 4, 0, 0, 0, 0, 0,
	20, 137, 232, 255, 256, 256, 83, 20, 3, 0, 0, 0, 0, 0, 0,
	24, 149, 238, 256, 256, 139, 17, 2, 0, 0, 0, 0, 0, 0, 0,
	25, 150, 232, 255, 167, 38, 2, 0, 0, 0, 0, 0, 0, 0, 0,
	13, 145, 214, 213, 56, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 157, 188, 107, 17, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 131, 130, 55, 7, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
};


unsigned int _ls_w_lut[514] = {256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256,
			       256, 256, 256, 256, 256, 256, 256, 256, 254, 252, 250, 248, 246, 244, 242, 240, 238, 236, 234, 233, 231, 229, 227, 225, 224, 222,
			       220, 218, 217, 215, 213, 212, 210, 208, 207, 205, 203, 202, 200, 199, 197, 195, 194, 192, 191, 189, 188, 186, 185, 183, 182, 181,
			       179, 178, 176, 175, 174, 172, 171, 170, 168, 167, 166, 164, 163, 162, 160, 159, 158, 157, 155, 154, 153, 152, 151, 149, 148, 147,
			       146, 145, 144, 142, 141, 140, 139, 138, 137, 136, 135, 134, 133, 132, 131, 130, 129, 128, 127, 126, 125, 124, 123, 122, 121, 120,
			       119, 118, 117, 116, 115, 114, 113, 112, 112, 111, 110, 109, 108, 107, 106, 106, 105, 104, 103, 102, 101, 101, 100, 99, 98, 98, 97,
			       96, 95, 94, 94, 93, 92, 92, 91, 90, 89, 89, 88, 87, 87, 86, 85, 85, 84, 83, 83, 82, 81, 81, 80, 79, 79, 78, 78, 77, 76, 76, 75, 75,
			       74, 73, 73, 72, 72, 71, 71, 70, 69, 69, 68, 68, 67, 67, 66, 66, 65, 65, 64, 64, 63, 63, 62, 62, 61, 61, 60, 60, 59, 59, 58, 58, 57,
			       57, 57, 56, 56, 55, 55, 54, 54, 53, 53, 53, 52, 52, 51, 51, 51, 50, 50, 49, 49, 49, 48, 48, 48, 47, 47, 46, 46, 46, 45, 45, 45, 44,
			       44, 44, 43, 43, 43, 42, 42, 42, 41, 41, 41, 40, 40, 40, 39, 39, 39, 38, 38, 38, 37, 37, 37, 37, 36, 36, 36, 35, 35, 35, 35, 34, 34,
			       34, 34, 33, 33, 33, 33, 32, 32, 32, 32, 31, 31, 31, 31, 30, 30, 30, 30, 29, 29, 29, 29, 28, 28, 28, 28, 28, 27, 27, 27, 27, 26, 26,
			       26, 26, 26, 25, 25, 25, 25, 25, 24, 24, 24, 24, 24, 24, 23, 23, 23, 23, 23, 22, 22, 22, 22, 22, 22, 21, 21, 21, 21, 21, 21, 20, 20,
			       20, 20, 20, 20, 19, 19, 19, 19, 19, 19, 19, 18, 18, 18, 18, 18, 18, 18, 17, 17, 17, 17, 17, 17, 17, 16, 16, 16, 16, 16, 16, 16, 16,
			       15, 15, 15, 15, 15, 15, 15, 15, 15, 14, 14, 14, 14, 14, 14, 14, 14, 14, 13, 13, 13, 13, 13, 13, 13, 13, 13, 12, 12, 12, 12, 12, 12,
			       12, 12, 12, 12, 12, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 9, 9, 9, 9, 9,
			       9, 9, 9, 9, 9, 9, 9, 9, 9, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 6,
			       6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6};
#endif

void JZ_Isp_Awb_Reg2par(int *para, unsigned int *reg)
{
	unsigned int awb_zone_v_start, awb_zone_h_start;
	unsigned int awb_zone_v_num, awb_zone_h_num;
	unsigned int awb_zone_h[16], awb_zone_v[16];
	unsigned int awb_rg_min, awb_rg_max;
	unsigned int awb_bg_min, awb_bg_max;
	unsigned int awb_k1, awb_a1;
	unsigned int awb_k2, awb_a2;
	unsigned int awb_y_min, awb_y_max;
	unsigned int awb_freq;

	int i, j;

	awb_zone_v_start = *reg & 0x7ff;
	awb_zone_v_num = (*reg & 0xf000) >> 12;
	awb_zone_h_start = (*reg & 0x7ff0000) >> 16;
	awb_zone_h_num = (*reg & 0xf0000000) >> 28;

	for (i = 0; i < 4; i++) {
		for (j = 0; j < 4; j++) {
			awb_zone_h[i * 4 + j] = (*(reg + 1 + i) & (0x7f << (j * 8))) >> (j * 8);
			awb_zone_v[i * 4 + j] = (*(reg + 5 + i) & (0x7f << (j * 8))) >> (j * 8);
		}
	}

	awb_rg_min = *(reg + 9) & 0xfff;
	awb_rg_max = (*(reg + 9) & 0xfff0000) >> 16;
	awb_bg_min = *(reg + 10) & 0xfff;
	awb_bg_max = (*(reg + 10) & 0xfff0000) >> 16;

	awb_k1 = *(reg + 11) & 0xffff;
	awb_a1 = (*(reg + 11) & 0xffff0000) >> 16;
	awb_k2 = *(reg + 12) & 0xffff;
	awb_a2 = (*(reg + 12) & 0xffff0000) >> 16;

	awb_y_min = *(reg + 13) & 0xff;
	awb_y_max = (*(reg + 13) & 0xff00) >> 8;
	awb_freq  = (*(reg + 13) & 0xf0000) >> 16;

	*(para) = awb_zone_v_start;
	*(para + 1) = awb_zone_v_num;
	*(para + 2) = awb_zone_h_start;
	*(para + 3) = awb_zone_h_num;

	for (i = 0; i < 15; i++) {
		*(para + 4 + i) = awb_zone_h[i];
		*(para + 19 + i) = awb_zone_v[i];
	}

	*(para + 34) = awb_rg_min;
	*(para + 35) = awb_rg_max;
	*(para + 36) = awb_bg_min;
	*(para + 37) = awb_bg_max;
	*(para + 38) = awb_k1;
	*(para + 39) = awb_a1;
	*(para + 40) = awb_k2;
	*(para + 41) = awb_a2;
	*(para + 42) = awb_y_min;
	*(para + 43) = awb_y_max;
	*(para + 44) = awb_freq;
}

void JZ_Isp_Awb_Awbg2reg(unsigned int final_gain[2], unsigned int awb_gain[2])
{
	uint32_t rgain = final_gain[0] * 4;
	uint32_t bgain = final_gain[1] * 4;

	if(rgain>4095)
		rgain = 4095;
	if(bgain>4095)
		bgain = 4095;

	*(awb_gain) = 0x4000000 | rgain;
	*(awb_gain + 1) = 0x4000000 | bgain;
}

void JZ_Isp_Get_Awb_Statistics(tisp_awb_t *awb, unsigned int *img, unsigned int zone_num_start)
{
	unsigned int h_zone = zone_num_start >> 28;
	unsigned int v_zone = (zone_num_start & 0xf000) >> 12;
	int i, j;

	for (i = 0; i < v_zone; i++) {
		for (j = 0; j < h_zone; j++){
			awb->awb_array_r[i * 15 + j] = (*(img + i * h_zone * 4 + j * 4)) & 0x1fffff;
			awb->awb_array_g[i * 15 + j] = (((*(img + i * h_zone * 4 + j * 4)) & 0xffe00000) >> 21) | (((*(img + i * h_zone * 4 + j * 4 + 1)) & 0x3ff) << 11);
			awb->awb_array_b[i * 15 + j] = (((*(img + i * h_zone * 4 + j * 4 + 1)) & 0x7ffffc00) >> 10);
			awb->awb_array_p[i * 15 + j] = (((*(img + i * h_zone * 4 + j * 4 + 1)) & 0x80000000) >> 31) | (((*(img + i * h_zone * 4 + j * 4 + 2)) & 0xfff) << 1);
		}
	}

#ifdef PARAMETER_DUMP
	if (running == 4) {
		running = 0;
		FILE *fp_r = NULL;
		FILE *fp_g = NULL;
		FILE *fp_b = NULL;
		FILE *fp_p = NULL;
		if((fp_r = fopen("fpga_awb_r_sum.dat","wb")) == NULL){
			printf("open save file error! \n");
		}
		if((fp_g = fopen("fpga_awb_g_sum.dat","wb")) == NULL){
			printf("open save file error! \n");
		}
		if((fp_b = fopen("fpga_awb_b_sum.dat","wb")) == NULL){
			printf("open save file error! \n");
		}
		if((fp_p = fopen("fpga_awb_pix_cnt.dat","wb")) == NULL){
			printf("open save file error! \n");
		}
		char ch[9];
		for(i = 0; i < 225; i++){
			snprintf(ch, sizeof(ch), "%06x,\n", awb->awb_array_r[i]);
			fwrite(ch, sizeof(ch), 1, fp_r);
			snprintf(ch, sizeof(ch), "%06x,\n", awb->awb_array_g[i]);
			fwrite(ch, sizeof(ch), 1, fp_g);
			snprintf(ch, sizeof(ch), "%06x,\n", awb->awb_array_b[i]);
			fwrite(ch, sizeof(ch), 1, fp_b);
			snprintf(ch, sizeof(ch), "%06x,\n", awb->awb_array_p[i]);
			fwrite(ch, sizeof(ch), 1, fp_p);
		}
		fclose(fp_r);
		fclose(fp_g);
		fclose(fp_b);
		fclose(fp_p);
	}
#endif
}

void Tiziano_awb_fpga(tisp_awb_t *awb)
{
	unsigned int  i,j;
	unsigned int *r_sum = awb->IspAwbFpgaParam.a_r_sum;
	unsigned int *g_sum = awb->IspAwbFpgaParam.a_g_sum;
	unsigned int *b_sum = awb->IspAwbFpgaParam.a_b_sum;
	unsigned int *pix_cnt = awb->IspAwbFpgaParam.a_pix_cnt;
	unsigned int *awb_cof = awb->IspAwbFpgaParam.a_awb_cof;
	unsigned int *awb_mf_para= awb->IspAwbFpgaParam.a_awb_mf_para;
	unsigned int *awb_parameter= awb->IspAwbFpgaParam.a_awb_parameter;
	unsigned int pixel_cnt_th = awb->IspAwbFpgaParam.pixel_cnt_th;
	uint32_t *wb_static = awb->IspAwbFpgaParam.a_awb_static;
	uint32_t AwbPointPos = awb->IspAwbFpgaParam.a_AwbPointPos[0];
	uint32_t wb_static_256[2];
	unsigned int awb_gain[2];
	unsigned int final_gain[2] = {256, 256};
	unsigned int awb_fail[1];
	unsigned int awb_reg[2];
	uint32_t rgbg_offset[2] = {0, 0};
	uint32_t ave_offset = 0;
	unsigned int h_zone = awb_parameter[3];
	unsigned int v_zone = awb_parameter[1];
	unsigned int awb_state = *awb_mf_para;
	unsigned int awb_do_frame = *(awb_mf_para + 1);
	unsigned int awb_p_gain[2];
	unsigned int awb_last_gain[2];
	unsigned int awb_frame = *awb_cof;
	unsigned int awb_tolerance = *(awb_cof + 1);
	unsigned int c_dis;

	awb_p_gain[0] = *(awb_mf_para + 2);
	awb_p_gain[1] = *(awb_mf_para + 3);
	awb_last_gain[0] = *(awb_mf_para + 4);
	awb_last_gain[1] = *(awb_mf_para + 5);

	/*--------------------- AWB Statistic -------------------------*/
	/* unsigned int frame_gain[2]; */
	wb_static_256[0] = fix_point_mult2_32(AwbPointPos, (uint32_t)256 << AwbPointPos, wb_static[0]);
	wb_static_256[1] = fix_point_mult2_32(AwbPointPos, (uint32_t)256 << AwbPointPos, wb_static[1]);

	for (i = 0; i < v_zone; i++) {
		for (j = 0; j < h_zone; j++) {
			awb->zone_rgbg[i * h_zone + j] = fix_point_mult2_32(AwbPointPos, ((uint32_t)r_sum[i * h_zone + j] << AwbPointPos) / g_sum[i * h_zone + j], wb_static_256[0]);
			/* round((float)(r_sum[i * 15 + j]) / (float)(g_sum[i * 15 + j]) * 256 * wb_static[0]); */
			awb->zone_rgbg[i * h_zone + j + h_zone * v_zone] = fix_point_mult2_32(AwbPointPos, ((uint32_t)b_sum[i * h_zone + j] << AwbPointPos) / g_sum[i * h_zone + j], wb_static_256[1]);
			/* round((float)(b_sum[i * 15 + j]) / (float)(g_sum[i * 15 + j]) * 256 * wb_static[1]); */
			awb->zone_pix_cnt[i * h_zone + j]  = pix_cnt[i * h_zone + j];
		}
	}

	/*------------------- Soft Photosensitive ----------------------*/
	awb->awb_rg_global = 0;
	awb->awb_bg_global = 0;
	awb->awb_pix_cnt[0] = v_zone * h_zone;
	awb->awb_pix_cnt[1] = v_zone * h_zone;
	for (i = 0; i < v_zone; i++) {
		for (j = 0; j < h_zone; j++) {
			awb->awb_rg_global += awb->zone_rgbg[i * h_zone + j];
			awb->awb_bg_global += awb->zone_rgbg[i * h_zone+j + h_zone * v_zone];
			if (0 == awb->zone_rgbg[i * h_zone + j])
				awb->awb_pix_cnt[0] = awb->awb_pix_cnt[0] - 1;
			if (0 == awb->zone_rgbg[i * h_zone + j + h_zone * v_zone])
				awb->awb_pix_cnt[1] = awb->awb_pix_cnt[1] - 1;
		}
	}
	awb->awb_rg_global = (fix_point_div(AwbPointPos, awb->awb_rg_global, wb_static[0])) >> AwbPointPos;
	awb->awb_bg_global = (fix_point_div(AwbPointPos, awb->awb_bg_global, wb_static[1])) >> AwbPointPos;

	if (0 == awb->awb_pix_cnt[0])
		awb->tisp_wb_attr.tisp_wb_rg_sta_global = 256;
	else
		awb->tisp_wb_attr.tisp_wb_rg_sta_global = awb->awb_rg_global / awb->awb_pix_cnt[0];

	if (0 == awb->awb_pix_cnt[1])
		awb->tisp_wb_attr.tisp_wb_bg_sta_global = 256;
	else
		awb->tisp_wb_attr.tisp_wb_bg_sta_global = awb->awb_bg_global / awb->awb_pix_cnt[1];
	/* ISP_ERROR("rg0: %d  bg0: %d\n", tisp_wb_attr.tisp_wb_rg_sta_global,
	   tisp_wb_attr.tisp_wb_bg_sta_global); */

/***************AWB Statistic*******************/
	for (i = 0; i < v_zone; i++) {
		for (j = 0; j < h_zone; j++) {
			if (pix_cnt[i * h_zone + j] <= pixel_cnt_th) {
				awb->zone_rgbg[i * h_zone + j]     = 0;
				awb->zone_rgbg[i * h_zone + j + h_zone * v_zone] = 0;
				awb->zone_pix_cnt[i * h_zone + j]  = 0;
			}
		}
	}
#if 0
	for (i = 0; i < v_zone; i++) {
		for (j = 0; j < h_zone; j++) {
			ISP_ERROR("%d,",zone_rgbg[i * h_zone + j]);
		}
		ISP_ERROR("\n");
	}
	ISP_ERROR("\n");

	for (i = 0; i < v_zone; i++) {
		for (j = 0; j < h_zone; j++) {
			ISP_ERROR("%d,",zone_rgbg[i * h_zone + j + h_zone * v_zone]);
		}
		ISP_ERROR("\n");
	}
	ISP_ERROR("\n");
#endif
	/*------------ abs(rg_eve - rg_last_eve) + abs(bg_eve - bg_last_eve) < offet_thres---------------*/
	if (awb->first_frame == 0) {
		for (i = 0; i < v_zone; i++) {
			for (j = 0; j < h_zone; j++) {
				rgbg_offset[0] = rgbg_offset[0] + ISPAWB_SUBABS(awb->zone_rgbg[i * h_zone + j], awb->zone_rgbg_last[i * h_zone + j]);
				rgbg_offset[1] = rgbg_offset[1] + ISPAWB_SUBABS(awb->zone_rgbg[i * h_zone + j + h_zone * v_zone], awb->zone_rgbg_last[i * h_zone + j + h_zone * v_zone]);
			}
		}
		ave_offset = ((rgbg_offset[0] + rgbg_offset[1]) >> AwbPointPos) / (v_zone * h_zone);
		awb->offet_thres = awb->_AwbPointPos[1];
		if ((ave_offset < awb->offet_thres) && (awb->tisp_wb_attr.tisp_wb_manual == 0) && (awb->awb_moa == 0)) {
			return;
		}
		/* ISP_ERROR("ave_offset %d\n", ave_offset); */
	}
	for (i = 0; i < v_zone; i++) {
		for (j = 0; j < h_zone; j++) {
			awb->zone_rgbg_last[i * h_zone + j] = awb->zone_rgbg[i * h_zone + j];
			awb->zone_rgbg_last[i * h_zone + j + h_zone * v_zone] = awb->zone_rgbg[i * h_zone + j + h_zone * v_zone];
		}
	}

/***************Ct Calculate*****************/
	Tiziano_Awb_Ct_Detect(awb->zone_rgbg, awb->zone_pix_cnt, awb->IspAwbCtDetectParam, awb_gain, awb_fail);
	/* ISP_ERROR("%d %d %d\n", awb_gain[0], awb_gain[1], awb_fail[0]); */

/*******rg == rg_original && bg == bg_original*******/
	if (awb->first_frame == 0) {
		if ((awb_gain[0] == awb->awb_gain_original[0]) && (awb_gain[1] == awb->awb_gain_original[1]) && (awb->tisp_wb_attr.tisp_wb_manual == 0) && (awb->awb_moa == 0) && (*(awb_mf_para + 2) == *(awb_mf_para + 4)) && (*(awb_mf_para + 3) == *(awb_mf_para + 5)))
			return;
	} else {
		awb->first_frame = 0;
		awb_frame = 1;
	}
	awb->awb_gain_original[0] = awb_gain[0];
	awb->awb_gain_original[1] = awb_gain[1];

/**************** Convergence *****************/
	/* final_gain[0] = (unsigned int)round(wb_static[0] * 256 * 256 / (float)awb_gain[0]); */
	/* final_gain[1] = (unsigned int)round(wb_static[1] * 256 * 256 / (float)awb_gain[1]); */

	final_gain[0] = (((uint32_t)1 << (AwbPointPos + 16)) / awb_gain[0] +
			 ISPAWBROUND(AwbPointPos)) >> AwbPointPos;
	/* (unsigned int)round(256 * 256 / (float)awb_gain[0]); */
	final_gain[1] = (((uint32_t)1 << (AwbPointPos + 16)) / awb_gain[1] +
			 ISPAWBROUND(AwbPointPos)) >> AwbPointPos;
	/* (unsigned int)round(256 * 256 / (float)awb_gain[1]); */
	/* ISP_ERROR("final_gain %d %d\n", final_gain[0], final_gain[1]); */

	c_dis = ISPAWB_SUBABS(final_gain[0], awb_p_gain[0]) +
		ISPAWB_SUBABS(final_gain[1], awb_p_gain[1]);
	/* c_dis = abs(((int)final_gain[0] - (int)awb_p_gain[0])) + abs(((int)final_gain[1] - (int)awb_p_gain[1])); */

	if (awb_state == 0) {
		if (c_dis <= awb_tolerance || awb_fail[0] == 1) {
			*awb_mf_para = 0;
			*(awb_mf_para + 1) = 0;
			*(awb_mf_para + 2) = *(awb_mf_para + 2);
			*(awb_mf_para + 3) = *(awb_mf_para + 3);
			*(awb_mf_para + 4) = *(awb_mf_para + 4);
			*(awb_mf_para + 5) = *(awb_mf_para + 5);
		} else {
			*awb_mf_para = 1;
			*(awb_mf_para + 1) = 1;
			*(awb_mf_para + 2) = final_gain[0];
			*(awb_mf_para + 3) = final_gain[1];
			*(awb_mf_para + 4) = ISPAWB_DELTA(awb_last_gain[0],
							  final_gain[0],
							  awb_p_gain[0],
							  awb_frame);
			/* (unsigned int)(  (float)awb_last_gain[0] + (float)((int)final_gain[0] - (int)awb_p_gain[0]) / (float)awb_frame  ); */
			*(awb_mf_para + 5) = ISPAWB_DELTA(awb_last_gain[1],
							  final_gain[1],
							  awb_p_gain[1],
							  awb_frame);
			/* (unsigned int)((float)awb_last_gain[1] + (float)((int)final_gain[1] - (int)awb_p_gain[1]) / (float)awb_frame); */
		}
	} else {
		if (c_dis <= awb_tolerance || awb_fail[0] == 1) {
			if (awb_do_frame + 1 == awb_frame || awb_frame == 1) {
				*awb_mf_para = 0;
				*(awb_mf_para + 4) = *(awb_mf_para + 2);
				*(awb_mf_para + 5) = *(awb_mf_para + 3);
			} else {
				*awb_mf_para = 1;
				*(awb_mf_para + 4) = ISPAWB_DELTA(awb_last_gain[0],
								  final_gain[0],
								  awb_last_gain[0],
								  (awb_frame - awb_do_frame));
				/* (unsigned int)((float)awb_last_gain[0] + (float)((int)final_gain[0] - (int)awb_last_gain[0]) / (float)(awb_frame - awb_do_frame)); */
				*(awb_mf_para + 5) = ISPAWB_DELTA(awb_last_gain[1],
								  final_gain[1],
								  awb_last_gain[1],
								  (awb_frame - awb_do_frame));
				/* (unsigned int)((float)awb_last_gain[1] + (float)((int)final_gain[1] - (int)awb_last_gain[1]) / (float)(awb_frame - awb_do_frame)); */
			}
			*(awb_mf_para + 1) = *(awb_mf_para + 1) + 1;
			*(awb_mf_para + 2) = *(awb_mf_para + 2);
			*(awb_mf_para + 3) = *(awb_mf_para + 3);
		} else {
			*awb_mf_para = 1;
			*(awb_mf_para + 1) = 1;
			*(awb_mf_para + 2) = final_gain[0];
			*(awb_mf_para + 3) = final_gain[1];
			*(awb_mf_para + 4) = ISPAWB_DELTA(awb_last_gain[0],
							  final_gain[0],
							  awb_last_gain[0], awb_frame);
			/* (unsigned int)((float)awb_last_gain[0] + (float)((int)final_gain[0] - (int)awb_p_gain[0]) / (float)awb_frame); //p-->last */
			*(awb_mf_para + 5) = ISPAWB_DELTA(awb_last_gain[1],
							  final_gain[1],
							  awb_last_gain[1], awb_frame);
			/* (unsigned int)((float)awb_last_gain[1] + (float)((int)final_gain[1] - (int)awb_p_gain[1]) / (float)awb_frame);  //p-->last */
		}
	}
	/* ISP_ERROR("%d %d %d %d\n", final_gain[0], *(awb_mf_para + 4), final_gain[1], *(awb_mf_para + 5)); */

	/* final_gain[0] = *(awb_mf_para + 4) * 16; */
	/* final_gain[1] = *(awb_mf_para + 5) * 16; */
	/* printf("%d %d\n", final_gain[0], final_gain[0]); */

	final_gain[0] = *(awb_mf_para + 4) ;
	final_gain[1] = *(awb_mf_para + 5) ;
	/* ISP_ERROR("%d %d %d %d\n", _awb_ct, final_gain[0], final_gain[1], awb_fail[0]); */

	final_gain[0] = (fix_point_mult2_32(AwbPointPos,
					    (uint32_t)final_gain[0]<<AwbPointPos, wb_static[0]) +
			 ISPAWBROUND(AwbPointPos))>>AwbPointPos;
	/* (unsigned int)((float)final_gain[0] * wb_static[0] + 0.5); */
	final_gain[1] = (fix_point_mult2_32(AwbPointPos,
					    (uint32_t)final_gain[1]<<AwbPointPos, wb_static[1]) +
			 ISPAWBROUND(AwbPointPos))>>AwbPointPos;
	/* (unsigned int)((float)final_gain[1] * wb_static[1] + 0.5); */

	/*-------------- Soft Photosensitive --------------------*/
	awb->tisp_wb_attr.tisp_wb_rg_sta_weight = 65536 / final_gain[0];
	awb->tisp_wb_attr.tisp_wb_bg_sta_weight = 65536 / final_gain[1];
	/* printf("rg0: %d  bg0: %d\n", awb->tisp_wb_attr.tisp_wb_rg_sta_global, awb->tisp_wb_attr.tisp_wb_bg_sta_global); */
	/* printf("rg1: %d  bg1: %d\n", awb->tisp_wb_attr.tisp_wb_rg_sta_weight, awb->tisp_wb_attr.tisp_wb_bg_sta_weight); */

	/*--------------------- AWB Mode ------------------------*/
	if ((awb->tisp_wb_attr.tisp_wb_manual >= 1) && (awb->tisp_wb_attr.tisp_wb_manual <= 8)) {
		final_gain[0] = awb->tisp_wb_attr.tisp_wb_rg;
		final_gain[1] = awb->tisp_wb_attr.tisp_wb_bg;
	} else if (awb->tisp_wb_attr.tisp_wb_manual == 9) {
		final_gain[0] = final_gain[0] * (64 + awb->tisp_wb_attr.tisp_wb_rg) / 64;
		final_gain[1] = final_gain[1] * (64 + awb->tisp_wb_attr.tisp_wb_bg) / 64;
	}

	/*-------------------- set rg & bg ---------------------*/
	awb->awb_moa = 0;
	JZ_Isp_Awb_Awbg2reg(final_gain, awb_reg);
	if (awb->awb_frz == 0) {
		system_reg_write(((tisp_core_t *)awb->core)->priv_data, AWB_GAIN_ADDR_R_G, awb_reg[0]);
		system_reg_write(((tisp_core_t *)awb->core)->priv_data, AWB_GAIN_ADDR_B_IR, awb_reg[1]);
	}
}

static uint32_t AePointPos_Awb = 10;
static uint32_t _ev = (uint32_t)400<<10;
int tisp_awb_ev_update(tisp_awb_t *awb, uint64_t data1)
{
	_ev = data1;
	/* ISP_ERROR("%s,%d: _ev = 0x%llx\n", __func__, __LINE__, _ev); */
	return 0;
}



int JZ_Isp_Awb(void *cb_data, uint64_t data1, uint64_t data2, uint64_t data3, uint64_t data4)
{
	uint32_t ev = _ev;
	static uint32_t ModeFlag = 0;
	tisp_awb_t *awb = (tisp_awb_t *)cb_data;
	tisp_core_t *core = awb->core;

	awb->_awb_ct_last = awb->_awb_ct;

	if (ev < (uint32_t)awb->_awb_mode[0] << AePointPos_Awb) {
		awb->IspAwbCtDetectParam.auint16RgBgWeight = awb->_rgbg_weight_ot;
		awb->IspAwbCtDetectParam.auint16LightSrc = awb->_light_src;
		awb->IspAwbCtDetectParam.uint16LightSrcNum = 0;
		awb->IspAwbCtDetectParam.auint32CtTh  = awb->_awb_ct_th_ot_luxhigh;
		awb->IspAwbCtDetectParam.auint32CtThPara = awb->_awb_ct_para_ot;
	} else if (ev < (uint32_t)awb->_awb_mode[1]<<AePointPos_Awb) {
		awb->IspAwbCtDetectParam.auint16RgBgWeight = awb->_rgbg_weight_ot;
		awb->IspAwbCtDetectParam.auint16LightSrc = awb->_light_src;
		awb->IspAwbCtDetectParam.uint16LightSrcNum = 0;
		awb->IspAwbCtDetectParam.auint32CtTh = awb->_awb_ct_th_ot_luxlow;
		awb->IspAwbCtDetectParam.auint32CtThPara = awb->_awb_ct_para_ot;
	} else {
		awb->IspAwbCtDetectParam.auint16RgBgWeight = awb->_rgbg_weight;
		awb->IspAwbCtDetectParam.auint16LightSrc = awb->_light_src;
		awb->IspAwbCtDetectParam.uint16LightSrcNum = awb->_light_src_num;
		awb->IspAwbCtDetectParam.auint32CtTh = awb->_awb_ct_th_in;
		awb->IspAwbCtDetectParam.auint32CtThPara = awb->_awb_ct_para_in;
	}

	awb->IspAwbCtDetectParam.uint32HorZone = awb->_awb_parameter[3];
	awb->IspAwbCtDetectParam.uint32VerZone = awb->_awb_parameter[1];
	awb->IspAwbCtDetectParam.auint16RgPos = awb->_rg_pos;
	awb->IspAwbCtDetectParam.auint16BgPos = awb->_bg_pos;
	awb->IspAwbCtDetectParam.auint16ColorTempMesh = awb->_color_temp_mesh;
	awb->IspAwbCtDetectParam.auint16AwbWght = awb->_awb_wght;
	awb->IspAwbCtDetectParam.auint64AwbDisTw = awb->_awb_dis_tw;
	awb->IspAwbCtDetectParam.uint32AwbCt = &awb->_awb_ct;
	awb->IspAwbCtDetectParam.auint32LsWLut = awb->_ls_w_lut;
	awb->IspAwbCtDetectParam.auint32AwbPointPos = awb->_AwbPointPos;

	/* fpga para */
	awb->IspAwbFpgaParam.a_r_sum = awb->awb_array_r;
	awb->IspAwbFpgaParam.a_g_sum = awb->awb_array_g;
	awb->IspAwbFpgaParam.a_b_sum = awb->awb_array_b;
	awb->IspAwbFpgaParam.a_pix_cnt = awb->awb_array_p;
	awb->IspAwbFpgaParam.a_awb_cof = awb->_awb_cof;
	awb->IspAwbFpgaParam.a_awb_mf_para = awb->_awb_mf_para;
	awb->IspAwbFpgaParam.a_awb_parameter= awb->_awb_parameter;
	awb->IspAwbFpgaParam.pixel_cnt_th = awb->_pixel_cnt_th;
	awb->IspAwbFpgaParam.a_awb_static = awb->_wb_static;
	awb->IspAwbFpgaParam.a_AwbPointPos = awb->_AwbPointPos;

	Tiziano_awb_fpga(awb);

	if (ev > ((uint32_t)awb->_awb_mode[2] << AePointPos_Awb) && ModeFlag == 0) {
		ModeFlag = 1;
		if (awb->awb_frz == 0){
			system_reg_write(((tisp_core_t *)awb->core)->priv_data, AWB_STAT_ADDR_RG_TH,
					 awb->_awb_lowlight_rg_th[1] << 16 | awb->_awb_lowlight_rg_th[0]);
		}
	} else if (ev <= ((uint32_t)awb->_awb_mode[2] << AePointPos_Awb) && ModeFlag == 1) {
		ModeFlag = 0;
		if (awb->awb_frz == 0) {
			system_reg_write(((tisp_core_t *)awb->core)->priv_data, AWB_STAT_ADDR_RG_TH,
					 awb->_awb_parameter[35] << 16 | awb->_awb_parameter[34]);
		}
	}

	{
		tisp_event_t awb_event;
		awb_event.type = TISP_EVENT_TYPE_AWB_CT;
		awb_event.data1 = awb->_awb_ct;
		tisp_event_push(&core->tisp_event_mg, &awb_event);
	}

	return 0;
}

int tiziano_awb_params_refresh(tisp_awb_t *awb)
{
	memcpy(awb->_awb_parameter,
	       tparams.params_data.TISP_PARAM_AWB_AWB_PARAMETER,
	       sizeof(tparams.params_data.TISP_PARAM_AWB_AWB_PARAMETER));
	memcpy(&awb->_pixel_cnt_th,
	       tparams.params_data.TISP_PARAM_AWB_PIXEL_CNT_TH,
	       sizeof(tparams.params_data.TISP_PARAM_AWB_PIXEL_CNT_TH));
	memcpy(awb->_awb_lowlight_rg_th,
	       tparams.params_data.TISP_PARAM_AWB_LOWLIGHT_RG_TH,
	       sizeof(tparams.params_data.TISP_PARAM_AWB_LOWLIGHT_RG_TH));
	memcpy(awb->_AwbPointPos,
	       tparams.params_data.TISP_PARAM_AWB_POINTPOS,
	       sizeof(tparams.params_data.TISP_PARAM_AWB_POINTPOS));
	memcpy(awb->_awb_cof,
	       tparams.params_data.TISP_PARAM_AWB_COF,
	       sizeof(tparams.params_data.TISP_PARAM_AWB_COF));
	memcpy(awb->_awb_mode,
	       tparams.params_data.TISP_PARAM_AWB_MODE,
	       sizeof(tparams.params_data.TISP_PARAM_AWB_MODE));
	memcpy(awb->_wb_static,
	       tparams.params_data.TISP_PARAM_AWB_STATIC,
	       sizeof(tparams.params_data.TISP_PARAM_AWB_STATIC));
	memcpy(awb->_light_src,
	       tparams.params_data.TISP_PARAM_AWB_LIGHT_SRC,
	       sizeof(tparams.params_data.TISP_PARAM_AWB_LIGHT_SRC));
	memcpy(&awb->_light_src_num,
	       tparams.params_data.TISP_PARAM_AWB_LIGHT_SRC_NUM,
	       sizeof(tparams.params_data.TISP_PARAM_AWB_LIGHT_SRC_NUM));
	memcpy(awb->_rg_pos,
	       tparams.params_data.TISP_PARAM_AWB_RG_POS,
	       sizeof(tparams.params_data.TISP_PARAM_AWB_RG_POS));
	memcpy(awb->_bg_pos,
	       tparams.params_data.TISP_PARAM_AWB_BG_POS,
	       sizeof(tparams.params_data.TISP_PARAM_AWB_BG_POS));
	memcpy(awb->_awb_ct_th_ot_luxhigh,
	       tparams.params_data.TISP_PARAM_AWB_CT_TH_OT_LUXHIGH,
	       sizeof(tparams.params_data.TISP_PARAM_AWB_CT_TH_OT_LUXHIGH));
	memcpy(awb->_awb_ct_th_ot_luxlow,
	       tparams.params_data.TISP_PARAM_AWB_CT_TH_OT_LUXLOW,
	       sizeof(tparams.params_data.TISP_PARAM_AWB_CT_TH_OT_LUXLOW));
	memcpy(awb->_awb_ct_th_in,
	       tparams.params_data.TISP_PARAM_AWB_CT_TH_IN,
	       sizeof(tparams.params_data.TISP_PARAM_AWB_CT_TH_IN));
	memcpy(awb->_awb_ct_para_ot,
	       tparams.params_data.TISP_PARAM_AWB_CT_PARA_OT,
	       sizeof(tparams.params_data.TISP_PARAM_AWB_CT_PARA_OT));
	memcpy(awb->_awb_ct_para_in,
	       tparams.params_data.TISP_PARAM_AWB_CT_PARA_IN,
	       sizeof(tparams.params_data.TISP_PARAM_AWB_CT_PARA_IN));
	memcpy(awb->_awb_dis_tw,
	       tparams.params_data.TISP_PARAM_AWB_DIS_TW,
	       sizeof(tparams.params_data.TISP_PARAM_AWB_DIS_TW));
	memcpy(awb->_rgbg_weight,
	       tparams.params_data.TISP_PARAM_AWB_RGBG_WEIGHT,
	       sizeof(tparams.params_data.TISP_PARAM_AWB_RGBG_WEIGHT));
	memcpy(awb->_color_temp_mesh,
	       tparams.params_data.TISP_PARAM_AWB_COLOR_TEMP_MESH,
	       sizeof(tparams.params_data.TISP_PARAM_AWB_COLOR_TEMP_MESH));
	memcpy(awb->_awb_wght,
	       tparams.params_data.TISP_PARAM_AWB_WGHT,
	       sizeof(tparams.params_data.TISP_PARAM_AWB_WGHT));
	memcpy(awb->_rgbg_weight_ot,
	       tparams.params_data.TISP_PARAM_AWB_RGBG_WEIGHT_OT,
	       sizeof(tparams.params_data.TISP_PARAM_AWB_RGBG_WEIGHT_OT));
	memcpy(awb->_ls_w_lut,
	       tparams.params_data.TISP_PARAM_AWB_LS_W_LUT,
	       sizeof(tparams.params_data.TISP_PARAM_AWB_LS_W_LUT));
	if(awb->awb_dn_refresh_flag == 0){
		memcpy(awb->_awb_mf_para,
		       tparams.params_data.TISP_PARAM_AWB_MF_PARA,
		       sizeof(tparams.params_data.TISP_PARAM_AWB_MF_PARA));
		memcpy(&awb->_awb_ct,
		       tparams.params_data.TISP_PARAM_AWB_CT,
		       sizeof(tparams.params_data.TISP_PARAM_AWB_CT));
		memcpy(&awb->_awb_ct_last,
		       tparams.params_data.TISP_PARAM_AWB_CT_LAST,
		       sizeof(tparams.params_data.TISP_PARAM_AWB_CT_LAST));
	}
	awb->awb_dn_refresh_flag = 0;

	return 0;
}

int awb_interrupt_static(void *data)
{
	int num = 0;

	tisp_awb_t *awb = (tisp_awb_t *)data;
	tisp_core_t *core = (tisp_core_t *)awb->core;
	tisp_info_t *tispinfo = &core->tispinfo;

	tisp_event_t awb_event;
	/* ISP_ERROR("%s,%d: --------- \n", __func__, __LINE__); */

	num = system_reg_read(((tisp_core_t *)awb->core)->priv_data, AWB_STAT_ADDR_DMA_INFO);
	dma_cache_sync(NULL, (void*)tispinfo->buf_awb_vaddr + num * 4 * 1024, 4096, DMA_BIDIRECTIONAL);
	JZ_Isp_Get_Awb_Statistics(awb, (void*)tispinfo->buf_awb_vaddr+num * 4 * 1024, 0xf001f001);

#if 0
	int i,j;
	for (i = 0; i < 15; i++) {
		for (j = 0; j < 15; j++) {
			ISP_ERROR("%d ", awb->awb_array_b[j + i * 15]);
		}
		ISP_ERROR("\n");
	}
	ISP_ERROR("\n");
#endif

	/* ISP_ERROR("%s,%d: \n", __func__, __LINE__); */
	awb_event.type = TISP_EVENT_TYPE_WB_STAITCS;
	tisp_event_push(&core->tisp_event_mg, &awb_event);

	return IRQ_HANDLED;
}

#define DUMP_AWB_REG(awb, name) ISP_ERROR("0x%x: 0x%xx\n", name, system_reg_read(((tisp_core_t *)awb->core)->priv_data, name));

void tiziano_awb_dump(tisp_awb_t *awb)
{
	ISP_ERROR("-----awb regs dump-----\n");
	DUMP_AWB_REG(awb, AWB_STAT_ADDR_REG_CTRL);
	DUMP_AWB_REG(awb, AWB_STAT_ADDR_ZONE_NUM_START);
	DUMP_AWB_REG(awb, AWB_STAT_ADDR_HOR_ZONE_SIZE_14);
	DUMP_AWB_REG(awb, AWB_STAT_ADDR_HOR_ZONE_SIZE_58);
	DUMP_AWB_REG(awb, AWB_STAT_ADDR_HOR_ZONE_SIZE_912);
	DUMP_AWB_REG(awb, AWB_STAT_ADDR_HOR_ZONE_SIZE_1315);
	DUMP_AWB_REG(awb, AWB_STAT_ADDR_VER_ZONE_SIZE_14);
	DUMP_AWB_REG(awb, AWB_STAT_ADDR_VER_ZONE_SIZE_58);
	DUMP_AWB_REG(awb, AWB_STAT_ADDR_VER_ZONE_SIZE_912);
	DUMP_AWB_REG(awb, AWB_STAT_ADDR_VER_ZONE_SIZE_1315);
	DUMP_AWB_REG(awb, AWB_STAT_ADDR_RG_TH);
	DUMP_AWB_REG(awb, AWB_STAT_ADDR_BG_TH);
	DUMP_AWB_REG(awb, AWB_STAT_ADDR_K1_A1);
	DUMP_AWB_REG(awb, AWB_STAT_ADDR_K2_A2);
	DUMP_AWB_REG(awb, AWB_STAT_ADDR_LUM_TH_FREQ);
	DUMP_AWB_REG(awb, AWB_STAT_ADDR_AWB_DMA_BASE_1);
	DUMP_AWB_REG(awb, AWB_STAT_ADDR_AWB_DMA_BASE_2);
	DUMP_AWB_REG(awb, AWB_STAT_ADDR_AWB_DMA_BASE_3);
	DUMP_AWB_REG(awb, AWB_STAT_ADDR_AWB_DMA_BASE_4);
	DUMP_AWB_REG(awb, AWB_STAT_ADDR_DMA_BASE_NUM);
	DUMP_AWB_REG(awb, AWB_STAT_ADDR_DMA_INFO);
	DUMP_AWB_REG(awb, AWB_STAT_ADDR_DEBUG0);
	DUMP_AWB_REG(awb, AWB_STAT_ADDR_DEBUG1);
}

int tiziano_awb_dn_params_refresh(tisp_awb_t *awb)
{
	awb->awb_dn_refresh_flag = 1;
	tiziano_awb_params_refresh(awb);
	awb->first_frame = 1;

	return 0;
}

int tiziano_awb_set_hardware_param(tisp_awb_t *awb)
{
	if (awb->awb_first == 0) {
		awb->awb_first = 1;
		system_reg_write(((tisp_core_t *)awb->core)->priv_data, AWB_STAT_ADDR_ZONE_NUM_START,
				 awb->_awb_parameter[3] << 28 | awb->_awb_parameter[2] << 16 |
				 awb->_awb_parameter[1] << 12 | awb->_awb_parameter[0]);
		system_reg_write(((tisp_core_t *)awb->core)->priv_data, AWB_STAT_ADDR_HOR_ZONE_SIZE_14,
				 awb->_awb_parameter[7] << 24 | awb->_awb_parameter[6] << 16 |
				 awb->_awb_parameter[5] << 8	| awb->_awb_parameter[4]);
		system_reg_write(((tisp_core_t *)awb->core)->priv_data, AWB_STAT_ADDR_HOR_ZONE_SIZE_58,
				 awb->_awb_parameter[11] << 24 | awb->_awb_parameter[10] << 16 |
				 awb->_awb_parameter[9] << 8 | awb->_awb_parameter[8]);
		system_reg_write(((tisp_core_t *)awb->core)->priv_data, AWB_STAT_ADDR_HOR_ZONE_SIZE_912,
				 awb->_awb_parameter[15] << 24 | awb->_awb_parameter[14] << 16 |
				 awb->_awb_parameter[13] << 8 | awb->_awb_parameter[12]);
		system_reg_write(((tisp_core_t *)awb->core)->priv_data, AWB_STAT_ADDR_HOR_ZONE_SIZE_1315,
				 awb->_awb_parameter[18] << 16 | awb->_awb_parameter[17] << 8 |
				 awb->_awb_parameter[16]);
		system_reg_write(((tisp_core_t *)awb->core)->priv_data, AWB_STAT_ADDR_VER_ZONE_SIZE_14,
				awb-> _awb_parameter[22] << 24 | awb->_awb_parameter[21] << 16 |
				 awb->_awb_parameter[20] << 8 | awb->_awb_parameter[19]);
		system_reg_write(((tisp_core_t *)awb->core)->priv_data, AWB_STAT_ADDR_VER_ZONE_SIZE_58,
				 awb->_awb_parameter[26] << 24 | awb->_awb_parameter[25] << 16 |
				 awb->_awb_parameter[24] << 8 | awb->_awb_parameter[23]);
		system_reg_write(((tisp_core_t *)awb->core)->priv_data, AWB_STAT_ADDR_VER_ZONE_SIZE_912,
				 awb->_awb_parameter[30] << 24 | awb->_awb_parameter[29] << 16 |
				 awb->_awb_parameter[28] << 8 | awb->_awb_parameter[27]);
		system_reg_write(((tisp_core_t *)awb->core)->priv_data, AWB_STAT_ADDR_VER_ZONE_SIZE_1315,
				 awb->_awb_parameter[33] << 16 | awb->_awb_parameter[32] << 8 |
				 awb->_awb_parameter[31]);
	}
	system_reg_write(((tisp_core_t *)awb->core)->priv_data, AWB_STAT_ADDR_RG_TH,
			 awb->_awb_parameter[35] << 16 | awb->_awb_parameter[34]);
	system_reg_write(((tisp_core_t *)awb->core)->priv_data, AWB_STAT_ADDR_BG_TH,
			 awb->_awb_parameter[37] << 16 | awb->_awb_parameter[36]);
	system_reg_write(((tisp_core_t *)awb->core)->priv_data, AWB_STAT_ADDR_K1_A1,
			 awb->_awb_parameter[39] << 16 | awb->_awb_parameter[38]);
	system_reg_write(((tisp_core_t *)awb->core)->priv_data, AWB_STAT_ADDR_K2_A2,
			 awb->_awb_parameter[41] << 16 | awb->_awb_parameter[40]);
	system_reg_write(((tisp_core_t *)awb->core)->priv_data, AWB_STAT_ADDR_LUM_TH_FREQ,
			 awb->_awb_parameter[44] << 16 | awb->_awb_parameter[43] << 8 |
			 awb->_awb_parameter[42]);

	return 0;
}

void awb_parameter_init(tisp_awb_t *awb)
{
	memcpy(awb->_awb_parameter, _awb_parameter, sizeof(_awb_parameter));
	awb->_pixel_cnt_th = 25;
	memcpy(awb->_awb_lowlight_rg_th, _awb_lowlight_rg_th, sizeof(_awb_lowlight_rg_th));
	memcpy(awb->_AwbPointPos, _AwbPointPos, sizeof(_AwbPointPos));
	memcpy(awb->_awb_cof, _awb_cof, sizeof(_awb_cof));
	memcpy(awb->_awb_mf_para, _awb_mf_para, sizeof(_awb_mf_para));
	memcpy(awb->_awb_mode, _awb_mode, sizeof(_awb_mode));
	awb->_awb_ct = 5000;
	awb->_awb_ct_last = 5000;
	memcpy(awb->_wb_static, _wb_static, sizeof(_wb_static));
	memcpy(awb->_light_src, _light_src, sizeof(_light_src));
	awb->_light_src_num = 3;
	memcpy(awb->_rg_pos, _rg_pos, sizeof(_rg_pos));
	memcpy(awb->_bg_pos, _bg_pos, sizeof(_bg_pos));
	memcpy(awb->_awb_ct_th_ot_luxhigh, _awb_ct_th_ot_luxhigh, sizeof(_awb_ct_th_ot_luxhigh));
	memcpy(awb->_awb_ct_th_ot_luxlow, _awb_ct_th_ot_luxlow, sizeof(_awb_ct_th_ot_luxlow));
	memcpy(awb->_awb_ct_th_in, _awb_ct_th_in, sizeof(_awb_ct_th_in));
	memcpy(awb->_awb_ct_para_ot, _awb_ct_para_ot, sizeof(_awb_ct_para_ot));
	memcpy(awb->_awb_ct_para_in, _awb_ct_para_in, sizeof(_awb_ct_para_in));
	memcpy(awb->_awb_dis_tw, _awb_dis_tw, sizeof(_awb_dis_tw));
	memcpy(awb->_rgbg_weight, _rgbg_weight, sizeof(_rgbg_weight));
	memcpy(awb->_color_temp_mesh, _color_temp_mesh, sizeof(_color_temp_mesh));
	memcpy(awb->_awb_wght, _awb_wght, sizeof(_awb_wght));
	memcpy(awb->_rgbg_weight_ot, _rgbg_weight_ot, sizeof(_rgbg_weight_ot));
	memcpy(awb->_ls_w_lut, _ls_w_lut, sizeof(_ls_w_lut));
}

int tiziano_awb_init(tisp_awb_t *awb, uint32_t height, uint32_t width)
{
 	int32_t i;

	tisp_core_t *core = awb->core;


	memset(&awb->tisp_wb_attr, 0, sizeof(awb->tisp_wb_attr));
	awb->awb_moa = 0;
	awb->awb_frz = 0;
	awb->awb_first = 0;
	awb->awb_dn_refresh_flag = 0;
	memset(&awb->IspAwbFpgaParam, 0, sizeof(awb->IspAwbFpgaParam));
	memset(&awb->IspAwbCtDetectParam, 0, sizeof(awb->IspAwbCtDetectParam));
	memset(awb->awb_array_r, 0, sizeof(awb->awb_array_r));
	memset(awb->awb_array_g, 0, sizeof(awb->awb_array_r));
	memset(awb->awb_array_b, 0, sizeof(awb->awb_array_r));
	memset(awb->awb_array_p, 0, sizeof(awb->awb_array_r));
	awb->awb_rg_global = 0;
	awb->awb_bg_global = 0;
	memset(awb->awb_pix_cnt, 0, sizeof(awb->awb_pix_cnt));


	memset(awb->zone_rgbg, 0, sizeof(awb->zone_rgbg));
	memset(awb->zone_rgbg_last, 0, sizeof(awb->zone_rgbg_last));
	memset(awb->zone_pix_cnt, 0, sizeof(awb->zone_pix_cnt));
	memset(awb->awb_gain_original, 0, sizeof(awb->awb_gain_original));
	awb->first_frame = 1;
	awb->offet_thres = 10;


	awb_parameter_init(awb);
	tiziano_awb_params_refresh(awb);

	for(i = 0; i<awb->_awb_parameter[3]; i++){
		awb->_awb_parameter[4+i] = (width / 2) / awb->_awb_parameter[3];
	}
	for(i = 0; i < awb->_awb_parameter[1]; i++){
		awb->_awb_parameter[19+i] = (height / 2) / awb->_awb_parameter[1];
	}

	if(awb->awb_frz == 0) {
		tiziano_awb_set_hardware_param(awb);
	}

	tisp_event_set_cb(&core->tisp_event_mg, TISP_EVENT_TYPE_WB_STAITCS, JZ_Isp_Awb, awb);
	system_irq_func_set(((tisp_core_t *)awb->core)->priv_data, TIZIANO_ISP_IRQ_AWB_STATIC, awb_interrupt_static, awb);
//	tiziano_awb_dump();

	return 0;
}

int tisp_g_wb_mode(tisp_awb_t *awb, tisp_wb_attr_t *wb_attr)
{
	memcpy(wb_attr, &awb->tisp_wb_attr, sizeof(tisp_wb_attr_t));

	return 0;
}

void tisp_awb_set_frz(tisp_awb_t *awb, unsigned char frz)
{
	awb->awb_frz = frz;
}

void tisp_awb_get_frz(tisp_awb_t *awb, unsigned char *frz)
{
	*frz = awb->awb_frz;
}

int tisp_s_wb_mode(tisp_awb_t *awb, tisp_wb_attr_t wb_attr)
{
	switch (wb_attr.tisp_wb_manual) {
	case 0:
		awb->tisp_wb_attr.tisp_wb_manual = 0;
		break;
	case 1:
		awb->tisp_wb_attr.tisp_wb_manual = 1;
		awb->tisp_wb_attr.tisp_wb_rg = wb_attr.tisp_wb_rg;
		awb->tisp_wb_attr.tisp_wb_bg = wb_attr.tisp_wb_bg;
		break;
	case 2:
		awb->tisp_wb_attr.tisp_wb_manual = 2;
		awb->tisp_wb_attr.tisp_wb_rg = 384;
		awb->tisp_wb_attr.tisp_wb_bg = 384;
		break;
	case 3:
		awb->tisp_wb_attr.tisp_wb_manual = 3;
		awb->tisp_wb_attr.tisp_wb_rg = 438;
		awb->tisp_wb_attr.tisp_wb_bg = 303;
		break;
	case 4:
		awb->tisp_wb_attr.tisp_wb_manual = 4;
		awb->tisp_wb_attr.tisp_wb_rg = 219;
		awb->tisp_wb_attr.tisp_wb_bg = 690;
		break;
	case 5:
		awb->tisp_wb_attr.tisp_wb_manual = 5;
		awb->tisp_wb_attr.tisp_wb_rg = 240;
		awb->tisp_wb_attr.tisp_wb_bg = 564;
		break;
	case 6:
		awb->tisp_wb_attr.tisp_wb_manual = 6;
		awb->tisp_wb_attr.tisp_wb_rg = 315;
		awb->tisp_wb_attr.tisp_wb_bg = 459;
		break;
	case 7:
		awb->tisp_wb_attr.tisp_wb_manual = 7;
		awb->tisp_wb_attr.tisp_wb_rg = 468;
		awb->tisp_wb_attr.tisp_wb_bg = 279;
		break;
	case 8:
		awb->tisp_wb_attr.tisp_wb_manual = 8;
		awb->tisp_wb_attr.tisp_wb_rg = 240;
		awb->tisp_wb_attr.tisp_wb_bg = 376;
		break;
	case 9:
		awb->tisp_wb_attr.tisp_wb_manual = 9;
		awb->tisp_wb_attr.tisp_wb_rg = wb_attr.tisp_wb_rg;
		awb->tisp_wb_attr.tisp_wb_bg = wb_attr.tisp_wb_bg;
		break;
	default:
		ISP_ERROR("%s:%d::Can not support this mode!!!\n",__func__,__LINE__);
	}

	awb->awb_moa = 1;

	return 0;
}

int32_t tisp_awb_param_array_get(tisp_awb_t *awb, int32_t id, void *buf, uint32_t *plen)
{
	uint32_t len;
	void *tmpbuf;

	switch (id) {
	case TISP_PARAM_AWB_AWB_PARAMETER:
		len = sizeof(tparams.params_data.TISP_PARAM_AWB_AWB_PARAMETER);
		tmpbuf=awb->_awb_parameter;
		break;
	case TISP_PARAM_AWB_PIXEL_CNT_TH:
		len = sizeof(tparams.params_data.TISP_PARAM_AWB_PIXEL_CNT_TH);
		tmpbuf=&awb->_pixel_cnt_th;
		break;
	case TISP_PARAM_AWB_LOWLIGHT_RG_TH:
		len = sizeof(tparams.params_data.TISP_PARAM_AWB_LOWLIGHT_RG_TH);
		tmpbuf=awb->_awb_lowlight_rg_th;
		break;
	case TISP_PARAM_AWB_POINTPOS:
		len = sizeof(tparams.params_data.TISP_PARAM_AWB_POINTPOS);
		tmpbuf=awb->_AwbPointPos;
		break;
	case TISP_PARAM_AWB_COF:
		len = sizeof(tparams.params_data.TISP_PARAM_AWB_COF);
		tmpbuf=awb->_awb_cof;
		break;
	case TISP_PARAM_AWB_MF_PARA:
		len = sizeof(tparams.params_data.TISP_PARAM_AWB_MF_PARA);
		tmpbuf=awb->_awb_mf_para;
		break;
	case TISP_PARAM_AWB_MODE:
		len = sizeof(tparams.params_data.TISP_PARAM_AWB_MODE);
		tmpbuf=awb->_awb_mode;
		break;
	case TISP_PARAM_AWB_CT:
		len = sizeof(tparams.params_data.TISP_PARAM_AWB_CT);
		tmpbuf=&awb->_awb_ct;
		break;
	case TISP_PARAM_AWB_CT_LAST:
		len = sizeof(tparams.params_data.TISP_PARAM_AWB_CT_LAST);
		tmpbuf=&awb->_awb_ct_last;
		break;
	case TISP_PARAM_AWB_STATIC:
		len = sizeof(tparams.params_data.TISP_PARAM_AWB_STATIC);
		tmpbuf=awb->_wb_static;
		break;
	case TISP_PARAM_AWB_LIGHT_SRC:
		len = sizeof(tparams.params_data.TISP_PARAM_AWB_LIGHT_SRC);
		tmpbuf=awb->_light_src;
		break;
	case TISP_PARAM_AWB_LIGHT_SRC_NUM:
		len = sizeof(tparams.params_data.TISP_PARAM_AWB_LIGHT_SRC_NUM);
		tmpbuf=&awb->_light_src_num;
		break;
	case TISP_PARAM_AWB_RG_POS:
		len = sizeof(tparams.params_data.TISP_PARAM_AWB_RG_POS);
		tmpbuf=awb->_rg_pos;
		break;
	case TISP_PARAM_AWB_BG_POS:
		len = sizeof(tparams.params_data.TISP_PARAM_AWB_BG_POS);
		tmpbuf=awb->_bg_pos;
		break;
	case TISP_PARAM_AWB_CT_TH_OT_LUXHIGH:
		len = sizeof(tparams.params_data.TISP_PARAM_AWB_CT_TH_OT_LUXHIGH);
		tmpbuf=awb->_awb_ct_th_ot_luxhigh;
		break;
	case TISP_PARAM_AWB_CT_TH_OT_LUXLOW:
		len = sizeof(tparams.params_data.TISP_PARAM_AWB_CT_TH_OT_LUXLOW);
		tmpbuf=awb->_awb_ct_th_ot_luxlow;
		break;
	case TISP_PARAM_AWB_CT_TH_IN:
		len = sizeof(tparams.params_data.TISP_PARAM_AWB_CT_TH_IN);
		tmpbuf=awb->_awb_ct_th_in;
		break;
	case TISP_PARAM_AWB_CT_PARA_OT:
		len = sizeof(tparams.params_data.TISP_PARAM_AWB_CT_PARA_OT);
		tmpbuf=awb->_awb_ct_para_ot;
		break;
	case TISP_PARAM_AWB_CT_PARA_IN:
		len = sizeof(tparams.params_data.TISP_PARAM_AWB_CT_PARA_IN);
		tmpbuf=awb->_awb_ct_para_in;
		break;
	case TISP_PARAM_AWB_DIS_TW:
		len = sizeof(tparams.params_data.TISP_PARAM_AWB_DIS_TW);
		tmpbuf=awb->_awb_dis_tw;
		break;
	case TISP_PARAM_AWB_RGBG_WEIGHT:
		len = sizeof(tparams.params_data.TISP_PARAM_AWB_RGBG_WEIGHT);
		tmpbuf=awb->_rgbg_weight;
		break;
	case TISP_PARAM_AWB_COLOR_TEMP_MESH:
		len = sizeof(tparams.params_data.TISP_PARAM_AWB_COLOR_TEMP_MESH);
		tmpbuf=awb->_color_temp_mesh;
		break;
	case TISP_PARAM_AWB_WGHT:
		len = sizeof(tparams.params_data.TISP_PARAM_AWB_WGHT);
		tmpbuf=awb->_awb_wght;
		break;
	case TISP_PARAM_AWB_RGBG_WEIGHT_OT:
		len = sizeof(tparams.params_data.TISP_PARAM_AWB_RGBG_WEIGHT_OT);
		tmpbuf=awb->_rgbg_weight_ot;
		break;
	case TISP_PARAM_AWB_LS_W_LUT:
		len = sizeof(tparams.params_data.TISP_PARAM_AWB_LS_W_LUT);
		tmpbuf=awb->_ls_w_lut;
		break;
	default:
		ISP_ERROR("%s,%d: awb not support param id %d\n", __func__, __LINE__, id);
		return -1;
	}
	memcpy(buf, tmpbuf, len);
	*plen = len;

	return 0;
}

int32_t tisp_awb_param_array_set(tisp_awb_t *awb, int32_t id, void *buf, uint32_t *plen)
{
	uint32_t len;
	void *tmpbuf;

	switch (id) {
	case TISP_PARAM_AWB_AWB_PARAMETER:
		len = sizeof(tparams.params_data.TISP_PARAM_AWB_AWB_PARAMETER);
		tmpbuf=awb->_awb_parameter;
		memcpy(tmpbuf, buf, len);
		break;
	case TISP_PARAM_AWB_PIXEL_CNT_TH:
		len = sizeof(tparams.params_data.TISP_PARAM_AWB_PIXEL_CNT_TH);
		tmpbuf=&awb->_pixel_cnt_th;
		memcpy(tmpbuf, buf, len);
		break;
	case TISP_PARAM_AWB_LOWLIGHT_RG_TH:
		len = sizeof(tparams.params_data.TISP_PARAM_AWB_LOWLIGHT_RG_TH);
		tmpbuf=awb->_awb_lowlight_rg_th;
		memcpy(tmpbuf, buf, len);
		break;
	case TISP_PARAM_AWB_POINTPOS:
		len = sizeof(tparams.params_data.TISP_PARAM_AWB_POINTPOS);
		tmpbuf=awb->_AwbPointPos;
		memcpy(tmpbuf, buf, len);
		break;
	case TISP_PARAM_AWB_COF:
		len = sizeof(tparams.params_data.TISP_PARAM_AWB_COF);
		tmpbuf=awb->_awb_cof;
		memcpy(tmpbuf, buf, len);
		break;
	case TISP_PARAM_AWB_MF_PARA:
		len = sizeof(tparams.params_data.TISP_PARAM_AWB_MF_PARA);
		tmpbuf=awb->_awb_mf_para;
		break;
	case TISP_PARAM_AWB_MODE:
		len = sizeof(tparams.params_data.TISP_PARAM_AWB_MODE);
		tmpbuf=awb->_awb_mode;
		memcpy(tmpbuf, buf, len);
		break;
	case TISP_PARAM_AWB_CT:
		len = sizeof(tparams.params_data.TISP_PARAM_AWB_CT);
		tmpbuf=&awb->_awb_ct;
		break;
	case TISP_PARAM_AWB_CT_LAST:
		len = sizeof(tparams.params_data.TISP_PARAM_AWB_CT_LAST);
		tmpbuf=&awb->_awb_ct_last;
		break;
	case TISP_PARAM_AWB_STATIC:
		len = sizeof(tparams.params_data.TISP_PARAM_AWB_STATIC);
		tmpbuf=awb->_wb_static;
		memcpy(tmpbuf, buf, len);
		break;
	case TISP_PARAM_AWB_LIGHT_SRC:
		len = sizeof(tparams.params_data.TISP_PARAM_AWB_LIGHT_SRC);
		tmpbuf=awb->_light_src;
		memcpy(tmpbuf, buf, len);
		break;
	case TISP_PARAM_AWB_LIGHT_SRC_NUM:
		len = sizeof(tparams.params_data.TISP_PARAM_AWB_LIGHT_SRC_NUM);
		tmpbuf=&awb->_light_src_num;
		memcpy(tmpbuf, buf, len);
		break;
	case TISP_PARAM_AWB_RG_POS:
		len = sizeof(tparams.params_data.TISP_PARAM_AWB_RG_POS);
		tmpbuf=awb->_rg_pos;
		memcpy(tmpbuf, buf, len);
		break;
	case TISP_PARAM_AWB_BG_POS:
		len = sizeof(tparams.params_data.TISP_PARAM_AWB_BG_POS);
		tmpbuf=awb->_bg_pos;
		memcpy(tmpbuf, buf, len);
		break;
	case TISP_PARAM_AWB_CT_TH_OT_LUXHIGH:
		len = sizeof(tparams.params_data.TISP_PARAM_AWB_CT_TH_OT_LUXHIGH);
		tmpbuf=awb->_awb_ct_th_ot_luxhigh;
		memcpy(tmpbuf, buf, len);
		break;
	case TISP_PARAM_AWB_CT_TH_OT_LUXLOW:
		len = sizeof(tparams.params_data.TISP_PARAM_AWB_CT_TH_OT_LUXLOW);
		tmpbuf=awb->_awb_ct_th_ot_luxlow;
		memcpy(tmpbuf, buf, len);
		break;
	case TISP_PARAM_AWB_CT_TH_IN:
		len = sizeof(tparams.params_data.TISP_PARAM_AWB_CT_TH_IN);
		tmpbuf=awb->_awb_ct_th_in;
		memcpy(tmpbuf, buf, len);
		break;
	case TISP_PARAM_AWB_CT_PARA_OT:
		len = sizeof(tparams.params_data.TISP_PARAM_AWB_CT_PARA_OT);
		tmpbuf=awb->_awb_ct_para_ot;
		memcpy(tmpbuf, buf, len);
		break;
	case TISP_PARAM_AWB_CT_PARA_IN:
		len = sizeof(tparams.params_data.TISP_PARAM_AWB_CT_PARA_IN);
		tmpbuf=awb->_awb_ct_para_in;
		memcpy(tmpbuf, buf, len);
		break;
	case TISP_PARAM_AWB_DIS_TW:
		len = sizeof(tparams.params_data.TISP_PARAM_AWB_DIS_TW);
		tmpbuf=awb->_awb_dis_tw;
		memcpy(tmpbuf, buf, len);
		break;
	case TISP_PARAM_AWB_RGBG_WEIGHT:
		len = sizeof(tparams.params_data.TISP_PARAM_AWB_RGBG_WEIGHT);
		tmpbuf=awb->_rgbg_weight;
		memcpy(tmpbuf, buf, len);
		break;
	case TISP_PARAM_AWB_COLOR_TEMP_MESH:
		len = sizeof(tparams.params_data.TISP_PARAM_AWB_COLOR_TEMP_MESH);
		tmpbuf=awb->_color_temp_mesh;
		memcpy(tmpbuf, buf, len);
		break;
	case TISP_PARAM_AWB_WGHT:
		len = sizeof(tparams.params_data.TISP_PARAM_AWB_WGHT);
		tmpbuf=awb->_awb_wght;
		memcpy(tmpbuf, buf, len);
		break;
	case TISP_PARAM_AWB_RGBG_WEIGHT_OT:
		len = sizeof(tparams.params_data.TISP_PARAM_AWB_RGBG_WEIGHT_OT);
		tmpbuf=awb->_rgbg_weight_ot;
		memcpy(tmpbuf, buf, len);
		break;
	case TISP_PARAM_AWB_LS_W_LUT:
		len = sizeof(tparams.params_data.TISP_PARAM_AWB_LS_W_LUT);
		tmpbuf=awb->_ls_w_lut;
		memcpy(tmpbuf, buf, len);
		break;
	default:
		ISP_ERROR("%s,%d: awb not support param id %d\n", __func__, __LINE__, id);
		return -1;
	}
	if (*plen != len) {
		ISP_ERROR("%s,%d: param size error %d, want len = %d, recv len = %d\n",
		       __func__, __LINE__, id, len, *plen);
		return -1;
	}
	tiziano_awb_set_hardware_param(awb);

	return 0;
}
