#include <linux/kernel.h>
#include <linux/irqreturn.h>
#include <linux/dma-mapping.h>

#include "fix_point_calc.h"
#include "../inc/tiziano_sys.h"
#include "../inc/tiziano_isp.h"
#include "tiziano_defog.h"
#include "tiziano_params.h"
#include "tiziano_event.h"
#include "tiziano_core.h"
#include "tiziano_map.h"

#if 0
static uint32_t ev_now;
static uint32_t ev_changed;

uint32_t defog_ev_list[9];
uint32_t defog_trsy0_list[9];
uint32_t defog_trsy1_list[9];
uint32_t defog_trsy2_list[9];
uint32_t defog_trsy3_list[9];
uint32_t defog_trsy4_list[9];
uint32_t defog_rgbra_list[9];
uint32_t defog_meam_block_r[200]={0};
uint32_t defog_meam_block_g[200]={0};
uint32_t defog_meam_block_b[200]={0};
uint32_t defog_meam_block_y[200]={0};
uint32_t defog_variance_block[200]={0};
uint32_t defog_detial_block[200]={0};
uint32_t defog_defog_block_t_x[5];
uint32_t defog_defog_block_t_y[5];
uint32_t defog_detail_x[5] = {255,240,200,170,100};
uint32_t defog_detail_y[5] = {100,100,83,40,0};
uint32_t defog_color_var_x[5] = {0,10,20,30,40};
uint32_t defog_color_var_y[5] = {0,10,20,50,90};
uint32_t defog_fpga_para[8] = {1920,1080,240,160,3,0,0,1024};
uint32_t param_defog_weightlut20[32];
uint32_t param_defog_weightlut02[32];
uint32_t param_defog_weightlut12[32];
uint32_t param_defog_weightlut22[32];
uint32_t param_defog_weightlut21[32];
uint32_t param_defog_ct_par_array[4];
uint32_t param_defog_dark_ct_array[14];
uint32_t param_defog_over_expo_w_array[11];
uint32_t param_defog_dark_spe_w_array[11];
uint32_t param_defog_col_ct_array[14];
uint32_t param_defog_cent3_w_dis_array[24];
uint32_t param_defog_cent5_w_dis_array[31];
uint32_t param_defog_blk_trans_array[10];
uint32_t param_defog_detail_ct_array[10];
uint32_t param_defog_col_var_array[10];
uint32_t param_defog_soft_ct_par_array[6];
#endif

uint32_t defog_block_transmit_t[200] ={
	230, 230, 230, 230, 230, 230, 230, 230, 230, 230, 230, 230, 230, 230, 230, 230, 230, 230, 230, 230,
	230, 230, 230, 230, 230, 230, 230, 230, 230, 230, 230, 230, 230, 230, 230, 230, 230, 230, 230, 230,
	230, 230, 230, 230, 230, 230, 230, 230, 230, 230, 230, 230, 230, 230, 230, 230, 230, 230, 230, 230,
	230, 230, 230, 230, 230, 230, 230, 230, 230, 230, 230, 230, 230, 230, 230, 230, 230, 230, 230, 230,
	230, 230, 230, 230, 230, 230, 230, 230, 230, 230, 230, 230, 230, 230, 230, 230, 230, 230, 230, 230,
	230, 230, 230, 230, 230, 230, 230, 230, 230, 230, 230, 230, 230, 230, 230, 230, 230, 230, 230, 230,
	230, 230, 230, 230, 230, 230, 230, 230, 230, 230, 230, 230, 230, 230, 230, 230, 230, 230, 230, 230,
	230, 230, 230, 230, 230, 230, 230, 230, 230, 230, 230, 230, 230, 230, 230, 230, 230, 230, 230, 230,
	230, 230, 230, 230, 230, 230, 230, 230, 230, 230, 230, 230, 230, 230, 230, 230, 230, 230, 230, 230,
	230, 230, 230, 230, 230, 230, 230, 230, 230, 230, 230, 230, 230, 230, 230, 230, 230, 230, 230, 230
};
uint32_t defog_block_air_light_r[200] ={
	200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200,
	200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200,
	200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200,
	200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200,
	200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200,
	200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200,
	200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200,
	200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200,
	200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200,
	200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200
};
uint32_t defog_block_air_light_g[200] ={
	200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200,
	200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200,
	200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200,
	200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200,
	200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200,
	200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200,
	200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200,
	200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200,
	200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200,
	200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200
};
uint32_t defog_block_air_light_b[200] ={
	200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200,
	200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200,
	200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200,
	200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200,
	200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200,
	200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200,
	200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200,
	200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200,
	200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200,
	200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200
};

#if 0
Isp_Defog_Ct_Detect_Para TizianoDefogStructMe;
#endif

int32_t tisp_defog_ev_update(tisp_defog_t *defog, uint64_t data1)
{
	uint32_t defogPointPos = 10;

	defog->ev_changed = 1;
	defog->ev_now = data1 >> defogPointPos;

	return 0;
}

void tiziano_defog_get_data(tisp_defog_t *defog, uint32_t *img)
{
	int32_t i,k;
	int32_t block_m;
	int32_t block_n;
	uint32_t tmp0,tmp1,tmp2,tmp3,tmp4,tmp5,tmp6,tmp7,tmp8;

	for(i = 0; i < 200; i++){
		tmp0 = (*(img + i * 4 + 0 + 2)) & 0x1fffff;
		tmp1 = (*(img + i * 4 + 0 + 2)) >> 21;
		tmp2 = (*(img + i * 4 + 1 + 2)) & 0x3ff;
		tmp3 =((*(img + i * 4 + 1 + 2)) >> 10) & 0x1fffff;
		tmp4 = (*(img + i * 4 + 1 + 2)) >> 31;
		tmp5 = (*(img + i * 4 + 2 + 2)) & 0x1fffff;
		tmp6 = (*(img + i * 4 + 2 + 2)) >> 21;
		tmp7 = (*(img + i * 4 + 3 + 2)) & 0xfff;
		tmp8 = (*(img + i * 4 + 3 + 2)) >> 12;

		block_m = i / 20;
		block_n = i % 20;
		k = 10 * block_n + block_m;
		defog->defog_meam_block_b[k] = tmp0;
		defog->defog_meam_block_g[k] = tmp1 | (tmp2 << 11);
		defog->defog_meam_block_r[k] = tmp3;
		defog->defog_meam_block_y[k] =(tmp4 << 20) | tmp8;
		defog->defog_detial_block[k] = tmp5;
		defog->defog_variance_block[k] = tmp6 | (tmp7 << 11);
	}
}

int32_t tiziano_defog_algorithm(tisp_defog_t *defog)
{
	uint32_t i;
	uint32_t defog_ev;

	defog_ev = defog->ev_now;
	if(defog->ev_changed == 1){
		defog->ev_changed = 0;
		for(i = 0; i < 9; i++){
			if(defog_ev <= (uint32_t)defog->defog_ev_list[i]){
				if(i == 0){
					defog->defog_fpga_para[7] = defog->defog_rgbra_list[0];
					defog->defog_defog_block_t_y[0] = defog->defog_trsy0_list[0];
					defog->defog_defog_block_t_y[1] = defog->defog_trsy1_list[0];
					defog->defog_defog_block_t_y[2] = defog->defog_trsy2_list[0];
					defog->defog_defog_block_t_y[3] = defog->defog_trsy3_list[0];
					defog->defog_defog_block_t_y[4] = defog->defog_trsy4_list[0];
					break;
				}
				if(defog->defog_ev_list[i] == defog->defog_ev_list[i-1]){
					defog->defog_fpga_para[7] = defog->defog_rgbra_list[i];
					defog->defog_defog_block_t_y[0] = defog->defog_trsy0_list[i];
					defog->defog_defog_block_t_y[1] = defog->defog_trsy1_list[i];
					defog->defog_defog_block_t_y[2] = defog->defog_trsy2_list[i];
					defog->defog_defog_block_t_y[3] = defog->defog_trsy3_list[i];
					defog->defog_defog_block_t_y[4] = defog->defog_trsy4_list[i];
					break;
				} else {
					defog->defog_fpga_para[7] = ISPINT(defog_ev,defog->defog_ev_list[i - 1],
								    defog->defog_ev_list[i],
								    defog->defog_rgbra_list[i - 1],
								    defog->defog_rgbra_list[i]);
					defog->defog_defog_block_t_y[0] = ISPINT(defog_ev,
									  defog->defog_ev_list[i - 1],
									  defog->defog_ev_list[i],
									  defog->defog_trsy0_list[i - 1],
									  defog->defog_trsy0_list[i]);
					defog->defog_defog_block_t_y[1] = ISPINT(defog_ev,defog->defog_ev_list[i-1],
									  defog->defog_ev_list[i],
									  defog->defog_trsy1_list[i-1],
									  defog->defog_trsy1_list[i]);
					defog->defog_defog_block_t_y[2] = ISPINT(defog_ev,defog->defog_ev_list[i-1],
									  defog->defog_ev_list[i],
									  defog->defog_trsy2_list[i-1],
									  defog->defog_trsy2_list[i]);
					defog->defog_defog_block_t_y[3] = ISPINT(defog_ev,defog->defog_ev_list[i-1],
									  defog->defog_ev_list[i],
									  defog->defog_trsy3_list[i-1],
									  defog->defog_trsy3_list[i]);
					defog->defog_defog_block_t_y[4] = ISPINT(defog_ev,
									  defog->defog_ev_list[i - 1],
									  defog->defog_ev_list[i],
									  defog->defog_trsy4_list[i - 1],
									  defog->defog_trsy4_list[i]);
					break;
				}
			}
		}
		if(i == 9){
			defog->defog_fpga_para[7] = defog->defog_rgbra_list[8];
			defog->defog_defog_block_t_y[0] = defog->defog_trsy0_list[8];
			defog->defog_defog_block_t_y[1] = defog->defog_trsy1_list[8];
			defog->defog_defog_block_t_y[2] = defog->defog_trsy2_list[8];
			defog->defog_defog_block_t_y[3] = defog->defog_trsy3_list[8];
			defog->defog_defog_block_t_y[4] = defog->defog_trsy4_list[8];
		}

		defog->param_defog_soft_ct_par_array[5] = defog->defog_fpga_para[7];
		defog->param_defog_blk_trans_array[5] = defog->defog_defog_block_t_y[0];
		defog->param_defog_blk_trans_array[6] = defog->defog_defog_block_t_y[1];
		defog->param_defog_blk_trans_array[7] = defog->defog_defog_block_t_y[2];
		defog->param_defog_blk_trans_array[8] = defog->defog_defog_block_t_y[3];
		defog->param_defog_blk_trans_array[9] = defog->defog_defog_block_t_y[4];
	}

	defog->TizianoDefogStructMe.DefogBlockRStat = defog->defog_meam_block_r;
	defog->TizianoDefogStructMe.DefogBlockGStat = defog->defog_meam_block_g;
	defog->TizianoDefogStructMe.DefogBlockBStat = defog->defog_meam_block_b;
	defog->TizianoDefogStructMe.DefogBlockYStat = defog->defog_meam_block_y;
	defog->TizianoDefogStructMe.DefogBlockDetailStat = defog->defog_detial_block;
	defog->TizianoDefogStructMe.DefogBlockVarianceStat = defog->defog_variance_block;
	defog->TizianoDefogStructMe.DefogDefogBlockTX = defog->defog_defog_block_t_x;
	defog->TizianoDefogStructMe.DefogDefogBlockTY = defog->defog_defog_block_t_y;
	defog->TizianoDefogStructMe.DefogDetailX = defog->defog_detail_x;
	defog->TizianoDefogStructMe.DefogDetailY = defog->defog_detail_y;
	defog->TizianoDefogStructMe.DefogColorVarX = defog->defog_color_var_x;
	defog->TizianoDefogStructMe.DefogColorVarY = defog->defog_color_var_y;
	defog->TizianoDefogStructMe.DefogFpgaPara = defog->defog_fpga_para;
	defog->TizianoDefogStructMe.DefogBlockTransmitT = defog->defog_block_transmit_t;
	defog->TizianoDefogStructMe.DefogBlockAirLightR = defog->defog_block_air_light_r;
	defog->TizianoDefogStructMe.DefogBlockAirLightG = defog->defog_block_air_light_g;
	defog->TizianoDefogStructMe.DefogBlockAirLightB = defog->defog_block_air_light_b;

	Tiziano_defog_fpga(defog->TizianoDefogStructMe);
	for (i = 0;i < 200; i++){
		system_reg_write(((tisp_core_t *)defog->core)->priv_data, DEFOG_MEM_BASE + i * 4,
				 defog->defog_block_air_light_r[i] << 24 |
				 defog->defog_block_air_light_g[i] << 16 |
				 defog->defog_block_air_light_b[i] <<  8 |
				 defog->defog_block_transmit_t[i]  <<  0);
	}

	return 0;
}

int32_t tiziano_defog_params_init(tisp_defog_t *defog)
{
	uint32_t i;

	system_reg_write(((tisp_core_t *)defog->core)->priv_data, DEFOG_ADDR_CT3X3_DIS_01_00,
			 defog->param_defog_cent3_w_dis_array[1] << 16 |
			 defog->param_defog_cent3_w_dis_array[0] << 0);
	system_reg_write(((tisp_core_t *)defog->core)->priv_data, DEFOG_ADDR_CT3X3_DIS_03_02,
			 defog->param_defog_cent3_w_dis_array[3] << 16 |
			 defog->param_defog_cent3_w_dis_array[2] << 0);
	system_reg_write(((tisp_core_t *)defog->core)->priv_data, DEFOG_ADDR_CT3X3_DIS_05_04,
			 defog->param_defog_cent3_w_dis_array[5] << 16 |
			 defog->param_defog_cent3_w_dis_array[4] << 0);
	system_reg_write(((tisp_core_t *)defog->core)->priv_data, DEFOG_ADDR_CT3X3_DIS_07_06,
			 defog->param_defog_cent3_w_dis_array[7] << 16 |
			 defog->param_defog_cent3_w_dis_array[6] << 0);
	system_reg_write(((tisp_core_t *)defog->core)->priv_data, DEFOG_ADDR_CT3X3_DIS_09_08,
			 defog->param_defog_cent3_w_dis_array[9] << 16 |
			 defog->param_defog_cent3_w_dis_array[8] << 0);
	system_reg_write(((tisp_core_t *)defog->core)->priv_data, DEFOG_ADDR_CT3X3_DIS_11_10,
			 defog->param_defog_cent3_w_dis_array[11] << 16 |
			 defog->param_defog_cent3_w_dis_array[10] << 0);
	system_reg_write(((tisp_core_t *)defog->core)->priv_data, DEFOG_ADDR_CT3X3_DIS_13_12,
			 defog->param_defog_cent3_w_dis_array[13] << 16 |
			 defog->param_defog_cent3_w_dis_array[12] << 0);
	system_reg_write(((tisp_core_t *)defog->core)->priv_data, DEFOG_ADDR_CT3X3_DIS_15_14,
			 defog->param_defog_cent3_w_dis_array[15] << 16 |
			 defog->param_defog_cent3_w_dis_array[14] << 0);
	system_reg_write(((tisp_core_t *)defog->core)->priv_data, DEFOG_ADDR_CT3X3_DIS_17_16,
			 defog->param_defog_cent3_w_dis_array[17] << 16 |
			 defog->param_defog_cent3_w_dis_array[16] << 0);
	system_reg_write(((tisp_core_t *)defog->core)->priv_data, DEFOG_ADDR_CT3X3_DIS_19_18,
			 defog->param_defog_cent3_w_dis_array[19] << 16 |
			 defog->param_defog_cent3_w_dis_array[18] << 0);
	system_reg_write(((tisp_core_t *)defog->core)->priv_data, DEFOG_ADDR_CT3X3_DIS_21_20,
			 defog->param_defog_cent3_w_dis_array[21] << 16 |
			 defog->param_defog_cent3_w_dis_array[20] << 0);
	system_reg_write(((tisp_core_t *)defog->core)->priv_data, DEFOG_ADDR_CT3X3_DIS_23_22,
			 defog->param_defog_cent3_w_dis_array[23] << 16 |
			 defog->param_defog_cent3_w_dis_array[22] << 0);
	system_reg_write(((tisp_core_t *)defog->core)->priv_data, DEFOG_ADDR_CT5X5_DIS_01_00,
			 defog->param_defog_cent5_w_dis_array[1] << 16 |
			 defog->param_defog_cent5_w_dis_array[0] << 0);
	system_reg_write(((tisp_core_t *)defog->core)->priv_data, DEFOG_ADDR_CT5X5_DIS_03_02,
			 defog->param_defog_cent5_w_dis_array[3] << 16 |
			 defog->param_defog_cent5_w_dis_array[2] << 0);
	system_reg_write(((tisp_core_t *)defog->core)->priv_data, DEFOG_ADDR_CT5X5_DIS_05_04,
			 defog->param_defog_cent5_w_dis_array[5] << 16 |
			 defog->param_defog_cent5_w_dis_array[4] << 0);
	system_reg_write(((tisp_core_t *)defog->core)->priv_data, DEFOG_ADDR_CT5X5_DIS_07_06,
			 defog->param_defog_cent5_w_dis_array[7] << 16 |
			 defog->param_defog_cent5_w_dis_array[6] << 0);
	system_reg_write(((tisp_core_t *)defog->core)->priv_data, DEFOG_ADDR_CT5X5_DIS_09_08,
			 defog->param_defog_cent5_w_dis_array[9] << 16 |
			 defog->param_defog_cent5_w_dis_array[8] << 0);
	system_reg_write(((tisp_core_t *)defog->core)->priv_data, DEFOG_ADDR_CT5X5_DIS_11_10,
			 defog->param_defog_cent5_w_dis_array[11] << 16 |
			 defog->param_defog_cent5_w_dis_array[10] << 0);
	system_reg_write(((tisp_core_t *)defog->core)->priv_data, DEFOG_ADDR_CT5X5_DIS_13_12,
			 defog->param_defog_cent5_w_dis_array[13] << 16 |
			 defog->param_defog_cent5_w_dis_array[12] << 0);
	system_reg_write(((tisp_core_t *)defog->core)->priv_data, DEFOG_ADDR_CT5X5_DIS_15_14,
			 defog->param_defog_cent5_w_dis_array[15] << 16 |
			 defog->param_defog_cent5_w_dis_array[14] << 0);
	system_reg_write(((tisp_core_t *)defog->core)->priv_data, DEFOG_ADDR_CT5X5_DIS_17_16,
			 defog->param_defog_cent5_w_dis_array[17] << 16 |
			 defog->param_defog_cent5_w_dis_array[16] << 0);
	system_reg_write(((tisp_core_t *)defog->core)->priv_data, DEFOG_ADDR_CT5X5_DIS_19_18,
			 defog->param_defog_cent5_w_dis_array[19] << 16 |
			 defog->param_defog_cent5_w_dis_array[18] << 0);
	system_reg_write(((tisp_core_t *)defog->core)->priv_data, DEFOG_ADDR_CT5X5_DIS_21_20,
			 defog->param_defog_cent5_w_dis_array[21] << 16 |
			 defog->param_defog_cent5_w_dis_array[20] << 0);
	system_reg_write(((tisp_core_t *)defog->core)->priv_data, DEFOG_ADDR_CT5X5_DIS_23_22,
			 defog->param_defog_cent5_w_dis_array[23] << 16 |
			 defog->param_defog_cent5_w_dis_array[22] << 0);
	system_reg_write(((tisp_core_t *)defog->core)->priv_data, DEFOG_ADDR_CT5X5_DIS_25_24,
			 defog->param_defog_cent5_w_dis_array[25] << 16 |
			 defog->param_defog_cent5_w_dis_array[24] << 0);
	system_reg_write(((tisp_core_t *)defog->core)->priv_data, DEFOG_ADDR_CT5X5_DIS_27_26,
			 defog->param_defog_cent5_w_dis_array[27] << 16 |
			 defog->param_defog_cent5_w_dis_array[26] << 0);
	system_reg_write(((tisp_core_t *)defog->core)->priv_data, DEFOG_ADDR_CT5X5_DIS_29_28,
			 defog->param_defog_cent5_w_dis_array[29] << 16 |
			 defog->param_defog_cent5_w_dis_array[28] << 0);
	system_reg_write(((tisp_core_t *)defog->core)->priv_data, DEFOG_ADDR_CT5X5_DIS_31_30,
			 defog->param_defog_cent5_w_dis_array[30] << 0);
	system_reg_write(((tisp_core_t *)defog->core)->priv_data, DEFOG_ADDR_WEIGHTLUT02_05_00,
			 defog->param_defog_weightlut02[5] << 25 |
			 defog->param_defog_weightlut02[4] << 20 |
			 defog->param_defog_weightlut02[3] << 15 |
			 defog->param_defog_weightlut02[2] << 10 |
			 defog->param_defog_weightlut02[1] << 5 |
			 defog->param_defog_weightlut02[0] << 0);
	system_reg_write(((tisp_core_t *)defog->core)->priv_data, DEFOG_ADDR_WEIGHTLUT02_11_06,
			 defog->param_defog_weightlut02[11] << 25 |
			 defog->param_defog_weightlut02[10] << 20 |
			 defog->param_defog_weightlut02[9] << 15 |
			 defog->param_defog_weightlut02[8] << 10 |
			 defog->param_defog_weightlut02[7] << 5 |
			 defog->param_defog_weightlut02[6] << 0);
	system_reg_write(((tisp_core_t *)defog->core)->priv_data, DEFOG_ADDR_WEIGHTLUT02_17_12,
			 defog->param_defog_weightlut02[17] << 25 |
			 defog->param_defog_weightlut02[16] << 20 |
			 defog->param_defog_weightlut02[15] << 15 |
			 defog->param_defog_weightlut02[14] << 10 |
			 defog->param_defog_weightlut02[13] << 5 |
			 defog->param_defog_weightlut02[12] << 0);
	system_reg_write(((tisp_core_t *)defog->core)->priv_data, DEFOG_ADDR_WEIGHTLUT02_23_18,
			 defog->param_defog_weightlut02[23] << 25 |
			 defog->param_defog_weightlut02[22] << 20 |
			 defog->param_defog_weightlut02[21] << 15 |
			 defog->param_defog_weightlut02[20] << 10 |
			 defog->param_defog_weightlut02[19] << 5 |
			 defog->param_defog_weightlut02[18] << 0);
	system_reg_write(((tisp_core_t *)defog->core)->priv_data, DEFOG_ADDR_WEIGHTLUT02_29_24,
			 defog->param_defog_weightlut02[29] << 25 |
			 defog->param_defog_weightlut02[28] << 20 |
			 defog->param_defog_weightlut02[27] << 15 |
			 defog->param_defog_weightlut02[26] << 10 |
			 defog->param_defog_weightlut02[25] << 5 |
			 defog->param_defog_weightlut02[24] << 0);
	system_reg_write(((tisp_core_t *)defog->core)->priv_data, DEFOG_ADDR_WEIGHTLUT02_31_30,
			 defog->param_defog_weightlut02[31] << 5 |
			 defog->param_defog_weightlut02[30] << 0);
	system_reg_write(((tisp_core_t *)defog->core)->priv_data, DEFOG_ADDR_WEIGHTLUT20_05_00,
			 defog->param_defog_weightlut20[5] << 25 |
			 defog->param_defog_weightlut20[4] << 20 |
			 defog->param_defog_weightlut20[3] << 15 |
			 defog->param_defog_weightlut20[2] << 10 |
			 defog->param_defog_weightlut20[1] << 5 |
			 defog->param_defog_weightlut20[0] << 0);
	system_reg_write(((tisp_core_t *)defog->core)->priv_data, DEFOG_ADDR_WEIGHTLUT20_11_06,
			 defog->param_defog_weightlut20[11] << 25 |
			 defog->param_defog_weightlut20[10] << 20 |
			 defog->param_defog_weightlut20[9] << 15 |
			 defog->param_defog_weightlut20[8] << 10 |
			 defog->param_defog_weightlut20[7] << 5 |
			 defog->param_defog_weightlut20[6] << 0);
	system_reg_write(((tisp_core_t *)defog->core)->priv_data, DEFOG_ADDR_WEIGHTLUT20_17_12,
			 defog->param_defog_weightlut20[17] << 25 |
			 defog->param_defog_weightlut20[16] << 20 |
			 defog->param_defog_weightlut20[15] << 15 |
			 defog->param_defog_weightlut20[14] << 10 |
			 defog->param_defog_weightlut20[13] << 5 |
			 defog->param_defog_weightlut20[12] << 0);
	system_reg_write(((tisp_core_t *)defog->core)->priv_data, DEFOG_ADDR_WEIGHTLUT20_23_18,
			 defog->param_defog_weightlut20[23] << 25 |
			 defog->param_defog_weightlut20[22] << 20 |
			 defog->param_defog_weightlut20[21] << 15 |
			 defog->param_defog_weightlut20[20] << 10 |
			 defog->param_defog_weightlut20[19] << 5 |
			 defog->param_defog_weightlut20[18] << 0);
	system_reg_write(((tisp_core_t *)defog->core)->priv_data, DEFOG_ADDR_WEIGHTLUT20_29_24,
			 defog->param_defog_weightlut20[29] << 25 |
			 defog->param_defog_weightlut20[28] << 20 |
			 defog->param_defog_weightlut20[27] << 15 |
			 defog->param_defog_weightlut20[26] << 10 |
			 defog->param_defog_weightlut20[25] << 5 |
			 defog->param_defog_weightlut20[24] << 0);
	system_reg_write(((tisp_core_t *)defog->core)->priv_data, DEFOG_ADDR_WEIGHTLUT20_31_30,
			 defog->param_defog_weightlut20[31] << 5 |
			 defog->param_defog_weightlut20[30] << 0);
	system_reg_write(((tisp_core_t *)defog->core)->priv_data, DEFOG_ADDR_WEIGHTLUT21_05_00,
			 defog->param_defog_weightlut21[5] << 25 |
			 defog->param_defog_weightlut21[4] << 20 |
			 defog->param_defog_weightlut21[3] << 15 |
			 defog->param_defog_weightlut21[2] << 10 |
			 defog->param_defog_weightlut21[1] << 5 |
			 defog->param_defog_weightlut21[0] << 0);
	system_reg_write(((tisp_core_t *)defog->core)->priv_data, DEFOG_ADDR_WEIGHTLUT21_11_06,
			 defog->param_defog_weightlut21[11] << 25 |
			 defog->param_defog_weightlut21[10] << 20 |
			 defog->param_defog_weightlut21[9] << 15 |
			 defog->param_defog_weightlut21[8] << 10 |
			 defog->param_defog_weightlut21[7] << 5 |
			 defog->param_defog_weightlut21[6] << 0);
	system_reg_write(((tisp_core_t *)defog->core)->priv_data, DEFOG_ADDR_WEIGHTLUT21_17_12,
			 defog->param_defog_weightlut21[17] << 25 |
			 defog->param_defog_weightlut21[16] << 20 |
			 defog->param_defog_weightlut21[15] << 15 |
			 defog->param_defog_weightlut21[14] << 10 |
			 defog->param_defog_weightlut21[13] << 5 |
			 defog->param_defog_weightlut21[12] << 0);
	system_reg_write(((tisp_core_t *)defog->core)->priv_data, DEFOG_ADDR_WEIGHTLUT21_23_18,
			 defog->param_defog_weightlut21[23] << 25 |
			 defog->param_defog_weightlut21[22] << 20 |
			 defog->param_defog_weightlut21[21] << 15 |
			 defog->param_defog_weightlut21[20] << 10 |
			 defog->param_defog_weightlut21[19] << 5 |
			 defog->param_defog_weightlut21[18] << 0);
	system_reg_write(((tisp_core_t *)defog->core)->priv_data, DEFOG_ADDR_WEIGHTLUT21_29_24,
			 defog->param_defog_weightlut21[29] << 25 |
			 defog->param_defog_weightlut21[28] << 20 |
			 defog->param_defog_weightlut21[27] << 15 |
			 defog->param_defog_weightlut21[26] << 10 |
			 defog->param_defog_weightlut21[25] << 5 |
			 defog->param_defog_weightlut21[24] << 0);
	system_reg_write(((tisp_core_t *)defog->core)->priv_data, DEFOG_ADDR_WEIGHTLUT21_31_30,
			 defog->param_defog_weightlut21[31] << 5 |
			 defog->param_defog_weightlut21[30] << 0);
	system_reg_write(((tisp_core_t *)defog->core)->priv_data, DEFOG_ADDR_WEIGHTLUT12_05_00,
			 defog->param_defog_weightlut12[5] << 25 |
			 defog->param_defog_weightlut12[4] << 20 |
			 defog->param_defog_weightlut12[3] << 15 |
			 defog->param_defog_weightlut12[2] << 10 |
			 defog->param_defog_weightlut12[1] << 5 |
			 defog->param_defog_weightlut12[0] << 0);
	system_reg_write(((tisp_core_t *)defog->core)->priv_data, DEFOG_ADDR_WEIGHTLUT12_11_06,
			 defog->param_defog_weightlut12[11] << 25 |
			 defog->param_defog_weightlut12[10] << 20 |
			 defog->param_defog_weightlut12[9] << 15 |
			 defog->param_defog_weightlut12[8] << 10 |
			 defog->param_defog_weightlut12[7] << 5 |
			 defog->param_defog_weightlut12[6] << 0);
	system_reg_write(((tisp_core_t *)defog->core)->priv_data, DEFOG_ADDR_WEIGHTLUT12_17_12,
			 defog->param_defog_weightlut12[17] << 25 |
			 defog->param_defog_weightlut12[16] << 20 |
			 defog->param_defog_weightlut12[15] << 15 |
			 defog->param_defog_weightlut12[14] << 10 |
			 defog->param_defog_weightlut12[13] << 5 |
			 defog->param_defog_weightlut12[12] << 0);
	system_reg_write(((tisp_core_t *)defog->core)->priv_data, DEFOG_ADDR_WEIGHTLUT12_23_18,
			 defog->param_defog_weightlut12[23] << 25 |
			 defog->param_defog_weightlut12[22] << 20 |
			 defog->param_defog_weightlut12[21] << 15 |
			 defog->param_defog_weightlut12[20] << 10 |
			 defog->param_defog_weightlut12[19] << 5 |
			 defog->param_defog_weightlut12[18] << 0);
	system_reg_write(((tisp_core_t *)defog->core)->priv_data, DEFOG_ADDR_WEIGHTLUT12_29_24,
			 defog->param_defog_weightlut12[29] << 25 |
			 defog->param_defog_weightlut12[28] << 20 |
			 defog->param_defog_weightlut12[27] << 15 |
			 defog->param_defog_weightlut12[26] << 10 |
			 defog->param_defog_weightlut12[25] << 5 |
			 defog->param_defog_weightlut12[24] << 0);
	system_reg_write(((tisp_core_t *)defog->core)->priv_data, DEFOG_ADDR_WEIGHTLUT12_31_30,
			 defog->param_defog_weightlut12[31] << 5 |
			 defog->param_defog_weightlut12[30] << 0);
	system_reg_write(((tisp_core_t *)defog->core)->priv_data, DEFOG_ADDR_WEIGHTLUT22_05_00,
			 defog->param_defog_weightlut22[5] << 25 |
			 defog->param_defog_weightlut22[4] << 20 |
			 defog->param_defog_weightlut22[3] << 15 |
			 defog->param_defog_weightlut22[2] << 10 |
			 defog->param_defog_weightlut22[1] << 5 |
			 defog->param_defog_weightlut22[0] << 0);
	system_reg_write(((tisp_core_t *)defog->core)->priv_data, DEFOG_ADDR_WEIGHTLUT22_11_06,
			 defog->param_defog_weightlut22[11] << 25 |
			 defog->param_defog_weightlut22[10] << 20 |
			 defog->param_defog_weightlut22[9] << 15 |
			 defog->param_defog_weightlut22[8] << 10 |
			 defog->param_defog_weightlut22[7] << 5 |
			 defog->param_defog_weightlut22[6] << 0);
	system_reg_write(((tisp_core_t *)defog->core)->priv_data, DEFOG_ADDR_WEIGHTLUT22_17_12,
			 defog->param_defog_weightlut22[17] << 25 |
			 defog->param_defog_weightlut22[16] << 20 |
			 defog->param_defog_weightlut22[15] << 15 |
			 defog->param_defog_weightlut22[14] << 10 |
			 defog->param_defog_weightlut22[13] << 5 |
			 defog->param_defog_weightlut22[12] << 0);
	system_reg_write(((tisp_core_t *)defog->core)->priv_data, DEFOG_ADDR_WEIGHTLUT22_23_18,
			 defog->param_defog_weightlut22[23] << 25 |
			 defog->param_defog_weightlut22[22] << 20 |
			 defog->param_defog_weightlut22[21] << 15 |
			 defog->param_defog_weightlut22[20] << 10 |
			 defog->param_defog_weightlut22[19] << 5 |
			 defog->param_defog_weightlut22[18] << 0);
	system_reg_write(((tisp_core_t *)defog->core)->priv_data, DEFOG_ADDR_WEIGHTLUT22_29_24,
			 defog->param_defog_weightlut22[29] << 25 |
			 defog->param_defog_weightlut22[28] << 20 |
			 defog->param_defog_weightlut22[27] << 15 |
			 defog->param_defog_weightlut22[26] << 10 |
			 defog->param_defog_weightlut22[25] << 5 |
			 defog->param_defog_weightlut22[24] << 0);
	system_reg_write(((tisp_core_t *)defog->core)->priv_data, DEFOG_ADDR_WEIGHTLUT22_31_30,
			 defog->param_defog_weightlut22[31] << 5 |
			 defog->param_defog_weightlut22[30] << 0);
	system_reg_write(((tisp_core_t *)defog->core)->priv_data, DEFOG_ADDR_CLOR_CTRL_X3210,
			 defog->param_defog_col_ct_array[3] << 24 |
			 defog->param_defog_col_ct_array[2] << 16 |
			 defog->param_defog_col_ct_array[1] <<	8 |
			 defog->param_defog_col_ct_array[0] <<	0);
	system_reg_write(((tisp_core_t *)defog->core)->priv_data, DEFOG_ADDR_CLOR_CTRL_Y3210,
			 defog->param_defog_col_ct_array[8] << 24 |
			 defog->param_defog_col_ct_array[7] << 16 |
			 defog->param_defog_col_ct_array[6] <<	8 |
			 defog->param_defog_col_ct_array[5] <<	0);
	system_reg_write(((tisp_core_t *)defog->core)->priv_data, DEFOG_ADDR_CLOR_CTRL_XY44,
			 defog->param_defog_col_ct_array[9] << 16 |
			 defog->param_defog_col_ct_array[4] <<	0);
	system_reg_write(((tisp_core_t *)defog->core)->priv_data, DEFOG_ADDR_CLOR_CTRL_POW3210,
			 defog->param_defog_col_ct_array[13] << 24 |
			 defog->param_defog_col_ct_array[12] << 16 |
			 defog->param_defog_col_ct_array[11] << 8 |
			 defog->param_defog_col_ct_array[10] << 0);
	system_reg_write(((tisp_core_t *)defog->core)->priv_data, DEFOG_ADDR_TRANS_DARK_CTRL_X3210,
			 defog->param_defog_dark_ct_array[3] << 24 |
			 defog->param_defog_dark_ct_array[2] << 16 |
			 defog->param_defog_dark_ct_array[1] << 8 |
			 defog->param_defog_dark_ct_array[0] << 0);
	system_reg_write(((tisp_core_t *)defog->core)->priv_data, DEFOG_ADDR_TRANS_DARK_CTRL_Y3210,
			 defog->param_defog_dark_ct_array[8] << 24 |
			 defog->param_defog_dark_ct_array[7] << 16 |
			 defog->param_defog_dark_ct_array[6] << 8 |
			 defog->param_defog_dark_ct_array[5] << 0);
	system_reg_write(((tisp_core_t *)defog->core)->priv_data, DEFOG_ADDR_TRANS_DARK_CTRL_XY44,
			 defog->param_defog_dark_ct_array[9] << 16 |
			 defog->param_defog_dark_ct_array[4] << 0);
	system_reg_write(((tisp_core_t *)defog->core)->priv_data, DEFOG_ADDR_TRANS_DARK_CTRL_POW3210,
			 defog->param_defog_dark_ct_array[13] << 24 |
			 defog->param_defog_dark_ct_array[12] << 16 |
			 defog->param_defog_dark_ct_array[11] << 8 |
			 defog->param_defog_dark_ct_array[10] << 0);
	system_reg_write(((tisp_core_t *)defog->core)->priv_data, DEFOG_ADDR_OVEREXPO_WGT_X3210,
			 defog->param_defog_over_expo_w_array[3] << 24 |
			 defog->param_defog_over_expo_w_array[2] << 16 |
			 defog->param_defog_over_expo_w_array[1] << 8 |
			 defog->param_defog_over_expo_w_array[0] << 0);
	system_reg_write(((tisp_core_t *)defog->core)->priv_data, DEFOG_ADDR_OVEREXPO_WGT_Y3210,
			 defog->param_defog_over_expo_w_array[7] << 24 |
			 defog->param_defog_over_expo_w_array[6] << 16 |
			 defog->param_defog_over_expo_w_array[5] << 8 |
			 defog->param_defog_over_expo_w_array[4] << 0);
	system_reg_write(((tisp_core_t *)defog->core)->priv_data, DEFOG_ADDR_OVEREXPO_WGT_POW210,
			 defog->param_defog_over_expo_w_array[10] << 16 |
			 defog->param_defog_over_expo_w_array[9] << 8 |
			 defog->param_defog_over_expo_w_array[8] << 0);
	system_reg_write(((tisp_core_t *)defog->core)->priv_data, DEFOG_ADDR_DARKSPC_WGT_X3210,
			 defog->param_defog_dark_spe_w_array[3] << 24 |
			 defog->param_defog_dark_spe_w_array[2] << 16 |
			 defog->param_defog_dark_spe_w_array[1] << 8 |
			 defog->param_defog_dark_spe_w_array[0] << 0);
	system_reg_write(((tisp_core_t *)defog->core)->priv_data, DEFOG_ADDR_DARKSPC_WGT_Y3210,
			 defog->param_defog_dark_spe_w_array[7] << 24 |
			 defog->param_defog_dark_spe_w_array[6] << 16 |
			 defog->param_defog_dark_spe_w_array[5] << 8 |
			 defog->param_defog_dark_spe_w_array[4] << 0);
	system_reg_write(((tisp_core_t *)defog->core)->priv_data, DEFOG_ADDR_DARKSPC_WGT_POW210,
			 defog->param_defog_dark_spe_w_array[10] << 16 |
			 defog->param_defog_dark_spe_w_array[9] << 8 |
			 defog->param_defog_dark_spe_w_array[8] << 0);
	system_reg_write(((tisp_core_t *)defog->core)->priv_data, DEFOG_ADDR_PARA,
			 defog->param_defog_ct_par_array[3] << 24 |
			 defog->param_defog_ct_par_array[2] << 16 |
			 defog->param_defog_ct_par_array[1] << 8 |
			 defog->param_defog_ct_par_array[0] << 0);
	for (i = 0;i < 200; i++){
		system_reg_write(((tisp_core_t *)defog->core)->priv_data, DEFOG_MEM_BASE + i * 4,
				 defog->defog_block_air_light_r[i] << 24 |
				 defog->defog_block_air_light_g[i] << 16 |
				 defog->defog_block_air_light_b[i] << 8 |
				 defog->defog_block_transmit_t[i] << 0);
	}

	defog->defog_defog_block_t_x[0] = defog->param_defog_blk_trans_array[0];
	defog->defog_defog_block_t_x[1] = defog->param_defog_blk_trans_array[1];
	defog->defog_defog_block_t_x[2] = defog->param_defog_blk_trans_array[2];
	defog->defog_defog_block_t_x[3] = defog->param_defog_blk_trans_array[3];
	defog->defog_defog_block_t_x[4] = defog->param_defog_blk_trans_array[4];

	defog->defog_defog_block_t_y[0] = defog->param_defog_blk_trans_array[5];
	defog->defog_defog_block_t_y[1] = defog->param_defog_blk_trans_array[6];
	defog->defog_defog_block_t_y[2] = defog->param_defog_blk_trans_array[7];
	defog->defog_defog_block_t_y[3] = defog->param_defog_blk_trans_array[8];
	defog->defog_defog_block_t_y[4] = defog->param_defog_blk_trans_array[9];

	defog->defog_detail_x[0] = defog->param_defog_detail_ct_array[0];
	defog->defog_detail_x[1] = defog->param_defog_detail_ct_array[1];
	defog->defog_detail_x[2] = defog->param_defog_detail_ct_array[2];
	defog->defog_detail_x[3] = defog->param_defog_detail_ct_array[3];
	defog->defog_detail_x[4] = defog->param_defog_detail_ct_array[4];

	defog->defog_detail_y[0] = defog->param_defog_detail_ct_array[5];
	defog->defog_detail_y[1] = defog->param_defog_detail_ct_array[6];
	defog->defog_detail_y[2] = defog->param_defog_detail_ct_array[7];
	defog->defog_detail_y[3] = defog->param_defog_detail_ct_array[8];
	defog->defog_detail_y[4] = defog->param_defog_detail_ct_array[9];

	defog->defog_color_var_x[0] = defog->param_defog_col_var_array[0];
	defog->defog_color_var_x[1] = defog->param_defog_col_var_array[1];
	defog->defog_color_var_x[2] = defog->param_defog_col_var_array[2];
	defog->defog_color_var_x[3] = defog->param_defog_col_var_array[3];
	defog->defog_color_var_x[4] = defog->param_defog_col_var_array[4];

	defog->defog_color_var_y[0] = defog->param_defog_col_var_array[5];
	defog->defog_color_var_y[1] = defog->param_defog_col_var_array[6];
	defog->defog_color_var_y[2] = defog->param_defog_col_var_array[7];
	defog->defog_color_var_y[3] = defog->param_defog_col_var_array[8];
	defog->defog_color_var_y[4] = defog->param_defog_col_var_array[9];

	defog->defog_fpga_para[2] = defog->param_defog_soft_ct_par_array[0];
	defog->defog_fpga_para[3] = defog->param_defog_soft_ct_par_array[1];
	defog->defog_fpga_para[4] = defog->param_defog_soft_ct_par_array[2];
	defog->defog_fpga_para[5] = defog->param_defog_soft_ct_par_array[3];
	defog->defog_fpga_para[6] = defog->param_defog_soft_ct_par_array[4];
	defog->defog_fpga_para[7] = defog->param_defog_soft_ct_par_array[5];

	return 0;
}

int tisp_defog_process(void *cb_data, uint64_t data1, uint64_t data2, uint64_t data3, uint64_t data4)
{
	tisp_defog_t *defog = (tisp_defog_t *)cb_data;

	tiziano_defog_algorithm(defog);

	return 0;
}

int tiziano_defog_interrupt_static(void *data)
{
	uint32_t addr = 0;
	tisp_defog_t *defog = (tisp_defog_t *)data;
	tisp_core_t *core = (tisp_core_t *)defog->core;
	tisp_info_t *tispinfo = &core->tispinfo;

	tisp_event_t defog_event;

	addr = system_reg_read(((tisp_core_t *)defog->core)->priv_data, DEFOG_ADDR_FRAME_ADDR );
	if(addr == tispinfo->buf_defog_paddr + 0 * 4 * 1024){
		dma_cache_sync(NULL, (void*)tispinfo->buf_defog_vaddr + 0 * 4 * 1024,
			       4096, DMA_BIDIRECTIONAL);
		tiziano_defog_get_data(defog, (void*)tispinfo->buf_defog_vaddr + 0 * 4 * 1024);
	}
	if(addr == tispinfo->buf_defog_paddr + 1 * 4 * 1024){
		dma_cache_sync(NULL, (void*)tispinfo->buf_defog_vaddr + 1 * 4 * 1024,
			       4096, DMA_BIDIRECTIONAL);
		tiziano_defog_get_data(defog, (void*)tispinfo->buf_defog_vaddr + 1 * 4 * 1024);
	}
	if(addr == tispinfo->buf_defog_paddr + 2 * 4 * 1024){
		dma_cache_sync(NULL, (void*)tispinfo->buf_defog_vaddr + 2 * 4 * 1024,
			       4096, DMA_BIDIRECTIONAL);
		tiziano_defog_get_data(defog, (void*)tispinfo->buf_defog_vaddr + 2 * 4 * 1024);
	}
	if(addr == tispinfo->buf_defog_paddr + 3 * 4 * 1024){
		dma_cache_sync(NULL, (void*)tispinfo->buf_defog_vaddr + 3 * 4 * 1024,
			       4096, DMA_BIDIRECTIONAL);
		tiziano_defog_get_data(defog, (void*)tispinfo->buf_defog_vaddr + 3 * 4 * 1024);
	}
	defog_event.type = TISP_EVENT_TYPE_DEFOG_PROCESS;
	tisp_event_push(&core->tisp_event_mg, &defog_event);

	return IRQ_HANDLED;
}

int tiziano_defog_params_refresh(tisp_defog_t *defog)
{
	memcpy(defog->param_defog_weightlut20,
	       tparams.params_data.TISP_PARAM_DEFOG_WEIGHTLUT20,
	       sizeof(tparams.params_data.TISP_PARAM_DEFOG_WEIGHTLUT20));
	memcpy(defog->param_defog_weightlut02,
	       tparams.params_data.TISP_PARAM_DEFOG_WEIGHTLUT02,
	       sizeof(tparams.params_data.TISP_PARAM_DEFOG_WEIGHTLUT02));
	memcpy(defog->param_defog_weightlut12,
	       tparams.params_data.TISP_PARAM_DEFOG_WEIGHTLUT12,
	       sizeof(tparams.params_data.TISP_PARAM_DEFOG_WEIGHTLUT12));
	memcpy(defog->param_defog_weightlut22,
	       tparams.params_data.TISP_PARAM_DEFOG_WEIGHTLUT22,
	       sizeof(tparams.params_data.TISP_PARAM_DEFOG_WEIGHTLUT22));
	memcpy(defog->param_defog_weightlut21,
	       tparams.params_data.TISP_PARAM_DEFOG_WEIGHTLUT21,
	       sizeof(tparams.params_data.TISP_PARAM_DEFOG_WEIGHTLUT21));
	memcpy(defog->param_defog_ct_par_array,
	       tparams.params_data.TISP_PARAM_DEFOG_CT_PAR_ARRAY,
	       sizeof(tparams.params_data.TISP_PARAM_DEFOG_CT_PAR_ARRAY));
	memcpy(defog->param_defog_dark_ct_array,
	       tparams.params_data.TISP_PARAM_DEFOG_DARK_CT_ARRAY,
	       sizeof(tparams.params_data.TISP_PARAM_DEFOG_DARK_CT_ARRAY));
	memcpy(defog->param_defog_over_expo_w_array,
	       tparams.params_data.TISP_PARAM_DEFOG_OVER_EXPO_W_ARRAY,
	       sizeof(tparams.params_data.TISP_PARAM_DEFOG_OVER_EXPO_W_ARRAY));
	memcpy(defog->param_defog_dark_spe_w_array,
	       tparams.params_data.TISP_PARAM_DEFOG_DARK_SPE_W_ARRAY,
	       sizeof(tparams.params_data.TISP_PARAM_DEFOG_DARK_SPE_W_ARRAY));
	memcpy(defog->param_defog_col_ct_array,
	       tparams.params_data.TISP_PARAM_DEFOG_COL_CT_ARRAY,
	       sizeof(tparams.params_data.TISP_PARAM_DEFOG_COL_CT_ARRAY));
	memcpy(defog->param_defog_cent3_w_dis_array,
	       tparams.params_data.TISP_PARAM_DEFOG_CENT3_W_DIS_ARRAY,
	       sizeof(tparams.params_data.TISP_PARAM_DEFOG_CENT3_W_DIS_ARRAY));
	memcpy(defog->param_defog_cent5_w_dis_array,
	       tparams.params_data.TISP_PARAM_DEFOG_CENT5_W_DIS_ARRAY,
	       sizeof(tparams.params_data.TISP_PARAM_DEFOG_CENT5_W_DIS_ARRAY));
	memcpy(defog->param_defog_blk_trans_array,
	       tparams.params_data.TISP_PARAM_DEFOG_BLK_TRANS_ARRAY,
	       sizeof(tparams.params_data.TISP_PARAM_DEFOG_BLK_TRANS_ARRAY));
	memcpy(defog->param_defog_detail_ct_array,
	       tparams.params_data.TISP_PARAM_DEFOG_DETAIL_CT_ARRAY,
	       sizeof(tparams.params_data.TISP_PARAM_DEFOG_DETAIL_CT_ARRAY));
	memcpy(defog->param_defog_col_var_array,
	       tparams.params_data.TISP_PARAM_DEFOG_COL_VAR_ARRAY,
	       sizeof(tparams.params_data.TISP_PARAM_DEFOG_COL_VAR_ARRAY));
	memcpy(defog->param_defog_soft_ct_par_array,
	       tparams.params_data.TISP_PARAM_DEFOG_SOFT_CT_PAR_ARRAY,
	       sizeof(tparams.params_data.TISP_PARAM_DEFOG_SOFT_CT_PAR_ARRAY));
	memcpy(defog->defog_ev_list,
	       tparams.params_data.TISP_PARAM_DEFOG_EV_LIST,
	       sizeof(tparams.params_data.TISP_PARAM_DEFOG_EV_LIST));
	memcpy(defog->defog_trsy0_list,
	       tparams.params_data.TISP_PARAM_DEFOG_TRSY0_LIST,
	       sizeof(tparams.params_data.TISP_PARAM_DEFOG_TRSY0_LIST));
	memcpy(defog->defog_trsy1_list,
	       tparams.params_data.TISP_PARAM_DEFOG_TRSY1_LIST,
	       sizeof(tparams.params_data.TISP_PARAM_DEFOG_TRSY1_LIST));
	memcpy(defog->defog_trsy2_list,
	       tparams.params_data.TISP_PARAM_DEFOG_TRSY2_LIST,
	       sizeof(tparams.params_data.TISP_PARAM_DEFOG_TRSY2_LIST));
	memcpy(defog->defog_trsy3_list,
	       tparams.params_data.TISP_PARAM_DEFOG_TRSY3_LIST,
	       sizeof(tparams.params_data.TISP_PARAM_DEFOG_TRSY3_LIST));
	memcpy(defog->defog_trsy4_list,
	       tparams.params_data.TISP_PARAM_DEFOG_TRSY4_LIST,
	       sizeof(tparams.params_data.TISP_PARAM_DEFOG_TRSY4_LIST));
	memcpy(defog->defog_rgbra_list,
	       tparams.params_data.TISP_PARAM_DEFOG_RGBRA_LIST,
	       sizeof(tparams.params_data.TISP_PARAM_DEFOG_RGBRA_LIST));

	return 0;
}

int tiziano_defog_dn_params_refresh(tisp_defog_t *defog)
{
	tiziano_defog_params_refresh(defog);
	tiziano_defog_params_init(defog);

	return 0;
}

int tiziano_defog_init(tisp_defog_t *defog, uint32_t width, uint32_t height)
{
	uint32_t width_step;
	uint32_t height_step;
	uint32_t defog_detail_x[5] = {255,240,200,170,100};
	uint32_t defog_detail_y[5] = {100,100,83,40,0};
	uint32_t defog_color_var_x[5] = {0,10,20,30,40};
	uint32_t defog_color_var_y[5] = {0,10,20,50,90};
	uint32_t defog_fpga_para[8] = {1920,1080,240,160,3,0,0,1024};
	tisp_core_t *core = defog->core;


	memcpy(defog->defog_detail_x, defog_detail_x, sizeof(defog->defog_detail_x));
	memcpy(defog->defog_detail_y, defog_detail_y, sizeof(defog->defog_detail_y));
	memcpy(defog->defog_color_var_x, defog_color_var_x, sizeof(defog->defog_color_var_x));
	memcpy(defog->defog_color_var_y, defog_color_var_y, sizeof(defog->defog_color_var_y));
	memcpy(defog->defog_fpga_para, defog_fpga_para, sizeof(defog->defog_fpga_para));

	defog->defog_fpga_para[0] = width;
	defog->defog_fpga_para[1] = height;

	width_step = width/20;
	height_step = height/10;

	memcpy(defog->defog_block_transmit_t, defog_block_transmit_t, sizeof(defog_block_transmit_t));
	memcpy(defog->defog_block_air_light_r, defog_block_air_light_r, sizeof(defog_block_air_light_r));
	memcpy(defog->defog_block_air_light_g, defog_block_air_light_g, sizeof(defog_block_air_light_g));
	memcpy(defog->defog_block_air_light_b, defog_block_air_light_b, sizeof(defog_block_air_light_b));


	tiziano_defog_params_refresh(defog);

	system_reg_write(((tisp_core_t *)defog->core)->priv_data, DEFOG_ADDR_BLOCK_COORD_M_01_00,
			 1 * height_step << 16 | 0 * height_step << 0);
	system_reg_write(((tisp_core_t *)defog->core)->priv_data, DEFOG_ADDR_BLOCK_COORD_M_03_02,
			 3 * height_step << 16 | 2 * height_step << 0);
	system_reg_write(((tisp_core_t *)defog->core)->priv_data, DEFOG_ADDR_BLOCK_COORD_M_05_04,
			 5 * height_step << 16 | 4 * height_step << 0);
	system_reg_write(((tisp_core_t *)defog->core)->priv_data, DEFOG_ADDR_BLOCK_COORD_M_07_06,
			 7 * height_step << 16 | 6 * height_step << 0);
	system_reg_write(((tisp_core_t *)defog->core)->priv_data, DEFOG_ADDR_BLOCK_COORD_M_09_08,
			 9 * height_step << 16 | 8 * height_step << 0);
	system_reg_write(((tisp_core_t *)defog->core)->priv_data, DEFOG_ADDR_BLOCK_COORD_M____10,
			 height << 0);
	system_reg_write(((tisp_core_t *)defog->core)->priv_data, DEFOG_ADDR_BLOCK_COORD_N_01_00,
			 1 * width_step << 16 |  0 * width_step << 0);
	system_reg_write(((tisp_core_t *)defog->core)->priv_data, DEFOG_ADDR_BLOCK_COORD_N_03_02,
			 3 * width_step << 16 |  2 * width_step << 0);
	system_reg_write(((tisp_core_t *)defog->core)->priv_data, DEFOG_ADDR_BLOCK_COORD_N_05_04,
			 5 * width_step << 16 |  4 * width_step << 0);
	system_reg_write(((tisp_core_t *)defog->core)->priv_data, DEFOG_ADDR_BLOCK_COORD_N_07_06,
			 7 * width_step << 16 |  6 * width_step << 0);
	system_reg_write(((tisp_core_t *)defog->core)->priv_data, DEFOG_ADDR_BLOCK_COORD_N_09_08,
			 9 * width_step << 16 |  8 * width_step << 0);
	system_reg_write(((tisp_core_t *)defog->core)->priv_data, DEFOG_ADDR_BLOCK_COORD_N_11_10,
			 11 * width_step << 16 | 10 * width_step << 0);
	system_reg_write(((tisp_core_t *)defog->core)->priv_data, DEFOG_ADDR_BLOCK_COORD_N_13_12,
			 13 * width_step << 16 | 12 * width_step << 0);
	system_reg_write(((tisp_core_t *)defog->core)->priv_data, DEFOG_ADDR_BLOCK_COORD_N_15_14,
			 15 * width_step << 16 | 14 * width_step << 0);
	system_reg_write(((tisp_core_t *)defog->core)->priv_data, DEFOG_ADDR_BLOCK_COORD_N_17_16,
			 17 * width_step << 16 | 16 * width_step << 0);
	system_reg_write(((tisp_core_t *)defog->core)->priv_data, DEFOG_ADDR_BLOCK_COORD_N_19_18,
			 19 * width_step << 16 | 18 * width_step << 0);
	system_reg_write(((tisp_core_t *)defog->core)->priv_data, DEFOG_ADDR_BLOCK_COORD_N____20,
			 width << 0);

	tiziano_defog_params_init(defog);
	system_irq_func_set(((tisp_core_t *)defog->core)->priv_data, TIZIANO_ISP_IRQ_DEFOG_STATIC,
			    tiziano_defog_interrupt_static, defog);
	tisp_event_set_cb(&core->tisp_event_mg, TISP_EVENT_TYPE_DEFOG_PROCESS, tisp_defog_process, defog);

	return 0;
}

int32_t tisp_defog_param_array_get(tisp_defog_t *defog, int32_t id, void *buf, uint32_t *plen)
{
	uint32_t len;
	void *tmpbuf;

	switch (id) {
	case TISP_PARAM_DEFOG_WEIGHTLUT20:
		len = sizeof(tparams.params_data.TISP_PARAM_DEFOG_WEIGHTLUT20);
		tmpbuf = defog->param_defog_weightlut20;
		break;
	case TISP_PARAM_DEFOG_WEIGHTLUT02		:
		len = sizeof(tparams.params_data.TISP_PARAM_DEFOG_WEIGHTLUT02);
		tmpbuf = defog->param_defog_weightlut02		;
		break;
	case TISP_PARAM_DEFOG_WEIGHTLUT12:
		len = sizeof(tparams.params_data.TISP_PARAM_DEFOG_WEIGHTLUT12);
		tmpbuf = defog->param_defog_weightlut12;
		break;
	case TISP_PARAM_DEFOG_WEIGHTLUT22:
		len = sizeof(tparams.params_data.TISP_PARAM_DEFOG_WEIGHTLUT22);
		tmpbuf = defog->param_defog_weightlut22;
		break;
	case TISP_PARAM_DEFOG_WEIGHTLUT21:
		len = sizeof(tparams.params_data.TISP_PARAM_DEFOG_WEIGHTLUT21);
		tmpbuf = defog->param_defog_weightlut21;
		break;
	case TISP_PARAM_DEFOG_CT_PAR_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_DEFOG_CT_PAR_ARRAY);
		tmpbuf = defog->param_defog_ct_par_array;
		break;
	case TISP_PARAM_DEFOG_DARK_CT_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_DEFOG_DARK_CT_ARRAY);
		tmpbuf = defog->param_defog_dark_ct_array;
		break;
	case TISP_PARAM_DEFOG_OVER_EXPO_W_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_DEFOG_OVER_EXPO_W_ARRAY);
		tmpbuf = defog->param_defog_over_expo_w_array;
		break;
	case TISP_PARAM_DEFOG_DARK_SPE_W_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_DEFOG_DARK_SPE_W_ARRAY);
		tmpbuf = defog->param_defog_dark_spe_w_array;
		break;
	case TISP_PARAM_DEFOG_COL_CT_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_DEFOG_COL_CT_ARRAY);
		tmpbuf = defog->param_defog_col_ct_array;
		break;
	case TISP_PARAM_DEFOG_CENT3_W_DIS_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_DEFOG_CENT3_W_DIS_ARRAY);
		tmpbuf = defog->param_defog_cent3_w_dis_array;
		break;
	case TISP_PARAM_DEFOG_CENT5_W_DIS_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_DEFOG_CENT5_W_DIS_ARRAY);
		tmpbuf = defog->param_defog_cent5_w_dis_array;
		break;
	case TISP_PARAM_DEFOG_BLK_TRANS_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_DEFOG_BLK_TRANS_ARRAY);
		tmpbuf = defog->param_defog_blk_trans_array;
		break;
	case TISP_PARAM_DEFOG_DETAIL_CT_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_DEFOG_DETAIL_CT_ARRAY);
		tmpbuf = defog->param_defog_detail_ct_array;
		break;
	case TISP_PARAM_DEFOG_COL_VAR_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_DEFOG_COL_VAR_ARRAY);
		tmpbuf = defog->param_defog_col_var_array;
		break;
	case TISP_PARAM_DEFOG_SOFT_CT_PAR_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_DEFOG_SOFT_CT_PAR_ARRAY);
		tmpbuf = defog->param_defog_soft_ct_par_array;
		break;
	case TISP_PARAM_DEFOG_EV_LIST:
		len = sizeof(tparams.params_data.TISP_PARAM_DEFOG_EV_LIST);
		tmpbuf = defog->defog_ev_list;
		break;
	case TISP_PARAM_DEFOG_TRSY0_LIST:
		len = sizeof(tparams.params_data.TISP_PARAM_DEFOG_TRSY0_LIST);
		tmpbuf = defog->defog_trsy0_list;
		break;
	case TISP_PARAM_DEFOG_TRSY1_LIST:
		len = sizeof(tparams.params_data.TISP_PARAM_DEFOG_TRSY1_LIST);
		tmpbuf = defog->defog_trsy1_list;
		break;
	case TISP_PARAM_DEFOG_TRSY2_LIST:
		len = sizeof(tparams.params_data.TISP_PARAM_DEFOG_TRSY2_LIST);
		tmpbuf = defog->defog_trsy2_list;
		break;
	case TISP_PARAM_DEFOG_TRSY3_LIST:
		len = sizeof(tparams.params_data.TISP_PARAM_DEFOG_TRSY3_LIST);
		tmpbuf = defog->defog_trsy3_list;
		break;
	case TISP_PARAM_DEFOG_TRSY4_LIST:
		len = sizeof(tparams.params_data.TISP_PARAM_DEFOG_TRSY4_LIST);
		tmpbuf = defog->defog_trsy4_list;
		break;
	case TISP_PARAM_DEFOG_RGBRA_LIST:
		len = sizeof(tparams.params_data.TISP_PARAM_DEFOG_RGBRA_LIST);
		tmpbuf = defog->defog_rgbra_list;
		break;
	default:
		ISP_ERROR("%s,%d: lsc not support param id %d\n", __func__, __LINE__, id);
	}

	memcpy(buf, tmpbuf, len);
	*plen = len;

	return 0;
}

int32_t tisp_defog_param_array_set(tisp_defog_t *defog, int32_t id, void *buf, uint32_t *plen)
{
	uint32_t len;
	void *tmpbuf;

	switch (id) {
	case TISP_PARAM_DEFOG_WEIGHTLUT20:
		len = sizeof(tparams.params_data.TISP_PARAM_DEFOG_WEIGHTLUT20);
		tmpbuf = defog->param_defog_weightlut20;
		memcpy(tmpbuf, buf, len);
		break;
	case TISP_PARAM_DEFOG_WEIGHTLUT02		:
		len = sizeof(tparams.params_data.TISP_PARAM_DEFOG_WEIGHTLUT02);
		tmpbuf = defog->param_defog_weightlut02		;
		memcpy(tmpbuf, buf, len);
		break;
	case TISP_PARAM_DEFOG_WEIGHTLUT12:
		len = sizeof(tparams.params_data.TISP_PARAM_DEFOG_WEIGHTLUT12);
		tmpbuf = defog->param_defog_weightlut12;
		memcpy(tmpbuf, buf, len);
		break;
	case TISP_PARAM_DEFOG_WEIGHTLUT22:
		len = sizeof(tparams.params_data.TISP_PARAM_DEFOG_WEIGHTLUT22);
		tmpbuf = defog->param_defog_weightlut22;
		memcpy(tmpbuf, buf, len);
		break;
	case TISP_PARAM_DEFOG_WEIGHTLUT21:
		len = sizeof(tparams.params_data.TISP_PARAM_DEFOG_WEIGHTLUT21);
		tmpbuf = defog->param_defog_weightlut21;
		memcpy(tmpbuf, buf, len);
		break;
	case TISP_PARAM_DEFOG_CT_PAR_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_DEFOG_CT_PAR_ARRAY);
		tmpbuf = defog->param_defog_ct_par_array;
		memcpy(tmpbuf, buf, len);
		break;
	case TISP_PARAM_DEFOG_DARK_CT_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_DEFOG_DARK_CT_ARRAY);
		tmpbuf = defog->param_defog_dark_ct_array;
		memcpy(tmpbuf, buf, len);
		break;
	case TISP_PARAM_DEFOG_OVER_EXPO_W_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_DEFOG_OVER_EXPO_W_ARRAY);
		tmpbuf = defog->param_defog_over_expo_w_array;
		memcpy(tmpbuf, buf, len);
		break;
	case TISP_PARAM_DEFOG_DARK_SPE_W_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_DEFOG_DARK_SPE_W_ARRAY);
		tmpbuf = defog->param_defog_dark_spe_w_array;
		memcpy(tmpbuf, buf, len);
		break;
	case TISP_PARAM_DEFOG_COL_CT_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_DEFOG_COL_CT_ARRAY);
		tmpbuf = defog->param_defog_col_ct_array;
		memcpy(tmpbuf, buf, len);
		break;
	case TISP_PARAM_DEFOG_CENT3_W_DIS_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_DEFOG_CENT3_W_DIS_ARRAY);
		tmpbuf = defog->param_defog_cent3_w_dis_array;
		memcpy(tmpbuf, buf, len);
		break;
	case TISP_PARAM_DEFOG_CENT5_W_DIS_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_DEFOG_CENT5_W_DIS_ARRAY);
		tmpbuf = defog->param_defog_cent5_w_dis_array;
		memcpy(tmpbuf, buf, len);
		break;
	case TISP_PARAM_DEFOG_BLK_TRANS_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_DEFOG_BLK_TRANS_ARRAY);
		tmpbuf = defog->param_defog_blk_trans_array;
		memcpy(tmpbuf, buf, len);
		break;
	case TISP_PARAM_DEFOG_DETAIL_CT_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_DEFOG_DETAIL_CT_ARRAY);
		tmpbuf = defog->param_defog_detail_ct_array;
		memcpy(tmpbuf, buf, len);
		break;
	case TISP_PARAM_DEFOG_COL_VAR_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_DEFOG_COL_VAR_ARRAY);
		tmpbuf = defog->param_defog_col_var_array;
		memcpy(tmpbuf, buf, len);
		break;
	case TISP_PARAM_DEFOG_SOFT_CT_PAR_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_DEFOG_SOFT_CT_PAR_ARRAY);
		tmpbuf = defog->param_defog_soft_ct_par_array;
		memcpy(tmpbuf, buf, len);
		break;
	case TISP_PARAM_DEFOG_EV_LIST:
		len = sizeof(tparams.params_data.TISP_PARAM_DEFOG_EV_LIST);
		tmpbuf = defog->defog_ev_list;
		memcpy(tmpbuf, buf, len);
		break;
	case TISP_PARAM_DEFOG_TRSY0_LIST:
		len = sizeof(tparams.params_data.TISP_PARAM_DEFOG_TRSY0_LIST);
		tmpbuf = defog->defog_trsy0_list;
		memcpy(tmpbuf, buf, len);
		break;
	case TISP_PARAM_DEFOG_TRSY1_LIST:
		len = sizeof(tparams.params_data.TISP_PARAM_DEFOG_TRSY1_LIST);
		tmpbuf = defog->defog_trsy1_list;
		memcpy(tmpbuf, buf, len);
		break;
	case TISP_PARAM_DEFOG_TRSY2_LIST:
		len = sizeof(tparams.params_data.TISP_PARAM_DEFOG_TRSY2_LIST);
		tmpbuf = defog->defog_trsy2_list;
		memcpy(tmpbuf, buf, len);
		break;
	case TISP_PARAM_DEFOG_TRSY3_LIST:
		len = sizeof(tparams.params_data.TISP_PARAM_DEFOG_TRSY3_LIST);
		tmpbuf = defog->defog_trsy3_list;
		memcpy(tmpbuf, buf, len);
		break;
	case TISP_PARAM_DEFOG_TRSY4_LIST:
		len = sizeof(tparams.params_data.TISP_PARAM_DEFOG_TRSY4_LIST);
		tmpbuf = defog->defog_trsy4_list;
		memcpy(tmpbuf, buf, len);
		break;
	case TISP_PARAM_DEFOG_RGBRA_LIST:
		len = sizeof(tparams.params_data.TISP_PARAM_DEFOG_RGBRA_LIST);
		tmpbuf = defog->defog_rgbra_list;
		memcpy(tmpbuf, buf, len);
		tiziano_defog_params_init(defog);
		break;
	default:
		ISP_ERROR("%s,%d: lsc not support param id %d\n", __func__, __LINE__, id);
		return -1;
	}

	return 0;
}
