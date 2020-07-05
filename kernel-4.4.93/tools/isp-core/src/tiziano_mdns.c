#include <linux/kernel.h>
#include <linux/irqreturn.h>
#include <linux/dma-mapping.h>

#include "fix_point_calc.h"
#include "../inc/tiziano_sys.h"
#include "tiziano_core.h"
#include "../inc/tiziano_isp.h"
#include "tiziano_mdns.h"
#include "tiziano_params.h"
#include "tiziano_map.h"


int32_t tisp_mdns_top_func_cfg(tisp_mdns_t *mdns, int32_t en)
{
	uint32_t reg_value;

	reg_value = (mdns->mdns_top_func_array[23] << 28) |
		(mdns->mdns_top_func_array[22] << 27) |
		(1 << 26) |
		(mdns->mdns_top_func_array[21] << 25) |
		(mdns->mdns_top_func_array[20] << 24) |
		(mdns->mdns_top_func_array[19] << 23) |
		(mdns->mdns_top_func_array[18] << 22) |
		(mdns->mdns_top_func_array[17] << 21) |
		(mdns->mdns_top_func_array[16] << 20) |
		(mdns->mdns_top_func_array[15] << 19) |
		(mdns->mdns_top_func_array[14] << 18) |
		(mdns->mdns_top_func_array[13] << 17) |
		(mdns->mdns_top_func_array[12] << 16) |
		(mdns->mdns_top_func_array[11] << 12) |
		(mdns->mdns_top_func_array[10] << 11) |
		(1 << 10) |
		(mdns->mdns_top_func_array[9] << 9) |
		(mdns->mdns_top_func_array[8] << 8) |
		(mdns->mdns_top_func_array[7] << 7) |
		(mdns->mdns_top_func_array[6] << 6) |
		(mdns->mdns_top_func_array[5] << 5) |
		(mdns->mdns_top_func_array[4] << 4) |
		(mdns->mdns_top_func_array[3] << 3) |
		(mdns->mdns_top_func_array[2] << 2) |
		(mdns->mdns_top_func_array[1] << 1) |
		(mdns->mdns_top_func_array[0]);

	if (en) {
		system_reg_write(((tisp_core_t *)mdns->core)->priv_data, MDNS_ADDR_T_TOP_FUN, reg_value);
	} else {
		system_reg_write(((tisp_core_t *)mdns->core)->priv_data, MDNS_ADDR_T_TOP_FUN, 0x0);
	}
	system_reg_write(((tisp_core_t *)mdns->core)->priv_data, MDNS_ADDR_Y_CUT_VALUE, 0x00ff0000);
	system_reg_write(((tisp_core_t *)mdns->core)->priv_data, MDNS_ADDR_UV_CUT_VALUE, 0x00ff0000);
	system_reg_write(((tisp_core_t *)mdns->core)->priv_data, MDNS_ADDR_Y_REF_FRM_NUM, 0x1);
	system_reg_write(((tisp_core_t *)mdns->core)->priv_data, MDNS_ADDR_UV_REF_FRM_NUM, 0x1);
	system_reg_write(((tisp_core_t *)mdns->core)->priv_data, MDNS_ADDR_STA_FRM_NUM, 0x10);
	system_reg_write(((tisp_core_t *)mdns->core)->priv_data, MDNS_ADDR_PBT_FRM_NUM, 0x10);
	/* triger */
	system_reg_write(((tisp_core_t *)mdns->core)->priv_data, MDNS_ADDR_REG_CTRL, 0x111);

	return 0;
}

int32_t tisp_mdns_sta_func_cfg(tisp_mdns_t *mdns, uint32_t width,uint32_t height)
{
	uint32_t reg_value;
	uint32_t mdns_pbt_width;
	uint32_t mdns_pbt_height;
	uint32_t mdns_sta_size;
	uint32_t mdns_pbt_size;
	uint32_t mdns_sta_hor_of;
	uint32_t mdns_sta_ver_of;
	uint32_t mdns_pbt_hor_of;
	uint32_t mdns_pbt_ver_of;
	uint32_t mdns_sta_wnum;
	uint32_t mdns_sta_hnum;
	uint32_t mdns_pbt_wnum;
	uint32_t mdns_pbt_hnum;

	if (mdns->mdns_sta_size_intp%4 != 0) {
		mdns_sta_size = (((int)(mdns->mdns_sta_size_intp / 4)) + 1) * 4;
	} else {
		mdns_sta_size = mdns->mdns_sta_size_intp;
	}

	if (mdns_sta_size < 8){
		mdns_sta_size = 8;
	}

	if (width%mdns_sta_size == 0) {
		mdns_sta_hor_of = 0;
	} else {
		mdns_sta_hor_of = 1;
	}

	if (height%mdns_sta_size == 0) {
		mdns_sta_ver_of = 0;
	} else {
		mdns_sta_ver_of = 1;
	}

	if (mdns_sta_ver_of == 1) {
		mdns_sta_hnum  = ((int)(height/mdns_sta_size)) + 1;
	} else {
		mdns_sta_hnum  = ((int)(height/mdns_sta_size));
	}

	if (mdns_sta_hor_of == 1) {
		mdns_sta_wnum  = ((int)(width/mdns_sta_size)) + 1;
	} else {
		mdns_sta_wnum  = ((int)(width/mdns_sta_size));
	}

	reg_value = (mdns_sta_ver_of << 8) |
		(mdns_sta_hor_of);
	system_reg_write(((tisp_core_t *)mdns->core)->priv_data, MDNS_ADDR_STA_OVER_CFG, reg_value);

	reg_value = (mdns_sta_size << 16) |
		(mdns_sta_size);
	system_reg_write(((tisp_core_t *)mdns->core)->priv_data, MDNS_ADDR_STA_BLK_SIZE, reg_value);

	reg_value = (mdns_sta_hnum << 16) |
		(mdns_sta_wnum);
	system_reg_write(((tisp_core_t *)mdns->core)->priv_data, MDNS_ADDR_STA_BLK_NUM, reg_value);

	mdns_pbt_width  = width  - mdns->mdns_pbt_ponit_array[0] - 16;
	mdns_pbt_height = height - mdns->mdns_pbt_ponit_array[1] - 16;

	if (mdns->mdns_pbt_size_intp%4 !=0){
		mdns_pbt_size = (((int)(mdns->mdns_pbt_size_intp/4)) + 1)*4;
	}
	else {
		mdns_pbt_size = mdns->mdns_pbt_size_intp;
	}

	if (mdns_pbt_size < 8){
		mdns_pbt_size = 8;
	}

	if (mdns_pbt_width%mdns_pbt_size == 0) {
		mdns_pbt_hor_of = 0;
	} else {
		mdns_pbt_hor_of = 1;
	}

	if (mdns_pbt_height%mdns_pbt_size == 0) {
		mdns_pbt_ver_of = 0;
	} else {
		mdns_pbt_ver_of = 1;
	}

	if (mdns_pbt_ver_of == 1) {
		mdns_pbt_hnum  = ((int)(mdns_pbt_height/mdns_pbt_size)) + 1;
	} else {
		mdns_pbt_hnum  = ((int)(mdns_pbt_height/mdns_pbt_size));
	}

	if (mdns_pbt_hor_of == 1) {
		mdns_pbt_wnum  = ((int)(mdns_pbt_width/mdns_pbt_size)) + 1;
	} else {
		mdns_pbt_wnum  = ((int)(mdns_pbt_width/mdns_pbt_size));
	}

	reg_value = (mdns_pbt_ver_of << 8) |
		(mdns_pbt_hor_of);
	system_reg_write(((tisp_core_t *)mdns->core)->priv_data, MDNS_ADDR_PBT_OVER_CFG, reg_value);

	reg_value = (mdns_pbt_size << 16) |
		(mdns_pbt_size);
	system_reg_write(((tisp_core_t *)mdns->core)->priv_data, MDNS_ADDR_PBT_BLK_SIZE, reg_value);

	reg_value = (mdns_pbt_hnum << 16) |
		(mdns_pbt_wnum);
	system_reg_write(((tisp_core_t *)mdns->core)->priv_data, MDNS_ADDR_PBT_BLK_NUM, reg_value);

	reg_value = (mdns->mdns_pbt_ponit_array[1] << 16) |
		(mdns->mdns_pbt_ponit_array[0]);
	system_reg_write(((tisp_core_t *)mdns->core)->priv_data, MDNS_ADDR_PBT_START_POINT, reg_value);

	reg_value = (mdns_pbt_height << 16) |
		(mdns_pbt_width);
	system_reg_write(((tisp_core_t *)mdns->core)->priv_data, MDNS_ADDR_PBT_WIN_SIZE, reg_value);

	return 0;
}

int32_t tisp_mdns_y_3d_param_cfg(tisp_mdns_t *mdns)
{
	uint32_t reg_value;

	uint32_t mdns_y_sta_wei0  = mdns->mdns_y_sta_stren_intp*mdns->mdns_y_sta_npv_array[0];
	uint32_t mdns_y_sta_wei1  = mdns->mdns_y_sta_stren_intp*mdns->mdns_y_sta_npv_array[1];
	uint32_t mdns_y_sta_wei2  = mdns->mdns_y_sta_stren_intp*mdns->mdns_y_sta_npv_array[2];
	uint32_t mdns_y_sta_wei3  = mdns->mdns_y_sta_stren_intp*mdns->mdns_y_sta_npv_array[3];
	uint32_t mdns_y_sta_wei4  = mdns->mdns_y_sta_stren_intp*mdns->mdns_y_sta_npv_array[4];
	uint32_t mdns_y_sta_wei5  = mdns->mdns_y_sta_stren_intp*mdns->mdns_y_sta_npv_array[5];
	uint32_t mdns_y_sta_wei6  = mdns->mdns_y_sta_stren_intp*mdns->mdns_y_sta_npv_array[6];
	uint32_t mdns_y_sta_wei7  = mdns->mdns_y_sta_stren_intp*mdns->mdns_y_sta_npv_array[7];
	uint32_t mdns_y_sta_wei8  = mdns->mdns_y_sta_stren_intp*mdns->mdns_y_sta_npv_array[8];
	uint32_t mdns_y_sta_wei9  = mdns->mdns_y_sta_stren_intp*mdns->mdns_y_sta_npv_array[9];
	uint32_t mdns_y_sta_wei10 = mdns->mdns_y_sta_stren_intp*mdns->mdns_y_sta_npv_array[10];
	uint32_t mdns_y_sta_wei11 = mdns->mdns_y_sta_stren_intp*mdns->mdns_y_sta_npv_array[11];
	uint32_t mdns_y_sta_wei12 = mdns->mdns_y_sta_stren_intp*mdns->mdns_y_sta_npv_array[12];
	uint32_t mdns_y_sta_wei13 = mdns->mdns_y_sta_stren_intp*mdns->mdns_y_sta_npv_array[13];
	uint32_t mdns_y_sta_wei14 = mdns->mdns_y_sta_stren_intp*mdns->mdns_y_sta_npv_array[14];
	uint32_t mdns_y_sta_wei15 = mdns->mdns_y_sta_stren_intp*mdns->mdns_y_sta_npv_array[15];
	uint32_t mdns_y_sta_wei16 = mdns->mdns_y_sta_stren_intp*mdns->mdns_y_sta_npv_array[16];
	uint32_t mdns_y_sta_wei17 = mdns->mdns_y_sta_stren_intp*mdns->mdns_y_sta_npv_array[17];
	uint32_t mdns_y_sta_wei18 = mdns->mdns_y_sta_stren_intp*mdns->mdns_y_sta_npv_array[18];
	uint32_t mdns_y_sta_wei19 = mdns->mdns_y_sta_stren_intp*mdns->mdns_y_sta_npv_array[19];
	uint32_t mdns_y_sta_wei20 = mdns->mdns_y_sta_stren_intp*mdns->mdns_y_sta_npv_array[20];
	uint32_t mdns_y_sta_wei21 = mdns->mdns_y_sta_stren_intp*mdns->mdns_y_sta_npv_array[21];
	uint32_t mdns_y_sta_wei22 = mdns->mdns_y_sta_stren_intp*mdns->mdns_y_sta_npv_array[22];
	uint32_t mdns_y_sta_wei23 = mdns->mdns_y_sta_stren_intp*mdns->mdns_y_sta_npv_array[23];
	uint32_t mdns_y_sta_wei24 = mdns->mdns_y_sta_stren_intp*mdns->mdns_y_sta_npv_array[24];
	uint32_t mdns_y_sta_wei25 = mdns->mdns_y_sta_stren_intp*mdns->mdns_y_sta_npv_array[25];
	uint32_t mdns_y_sta_wei26 = mdns->mdns_y_sta_stren_intp*mdns->mdns_y_sta_npv_array[26];
	uint32_t mdns_y_sta_wei27 = mdns->mdns_y_sta_stren_intp*mdns->mdns_y_sta_npv_array[27];
	uint32_t mdns_y_sta_wei28 = mdns->mdns_y_sta_stren_intp*mdns->mdns_y_sta_npv_array[28];
	uint32_t mdns_y_sta_wei29 = mdns->mdns_y_sta_stren_intp*mdns->mdns_y_sta_npv_array[29];
	uint32_t mdns_y_sta_wei30 = mdns->mdns_y_sta_stren_intp*mdns->mdns_y_sta_npv_array[30];
	uint32_t mdns_y_sta_wei31 = mdns->mdns_y_sta_stren_intp*mdns->mdns_y_sta_npv_array[31];

	uint32_t mdns_y_pbt_wei0  = mdns->mdns_y_pbt_stren_intp*mdns->mdns_y_pbt_npv_array[0];
	uint32_t mdns_y_pbt_wei1  = mdns->mdns_y_pbt_stren_intp*mdns->mdns_y_pbt_npv_array[1];
	uint32_t mdns_y_pbt_wei2  = mdns->mdns_y_pbt_stren_intp*mdns->mdns_y_pbt_npv_array[2];
	uint32_t mdns_y_pbt_wei3  = mdns->mdns_y_pbt_stren_intp*mdns->mdns_y_pbt_npv_array[3];
	uint32_t mdns_y_pbt_wei4  = mdns->mdns_y_pbt_stren_intp*mdns->mdns_y_pbt_npv_array[4];
	uint32_t mdns_y_pbt_wei5  = mdns->mdns_y_pbt_stren_intp*mdns->mdns_y_pbt_npv_array[5];
	uint32_t mdns_y_pbt_wei6  = mdns->mdns_y_pbt_stren_intp*mdns->mdns_y_pbt_npv_array[6];
	uint32_t mdns_y_pbt_wei7  = mdns->mdns_y_pbt_stren_intp*mdns->mdns_y_pbt_npv_array[7];
	uint32_t mdns_y_pbt_wei8  = mdns->mdns_y_pbt_stren_intp*mdns->mdns_y_pbt_npv_array[8];
	uint32_t mdns_y_pbt_wei9  = mdns->mdns_y_pbt_stren_intp*mdns->mdns_y_pbt_npv_array[9];
	uint32_t mdns_y_pbt_wei10 = mdns->mdns_y_pbt_stren_intp*mdns->mdns_y_pbt_npv_array[10];
	uint32_t mdns_y_pbt_wei11 = mdns->mdns_y_pbt_stren_intp*mdns->mdns_y_pbt_npv_array[11];
	uint32_t mdns_y_pbt_wei12 = mdns->mdns_y_pbt_stren_intp*mdns->mdns_y_pbt_npv_array[12];
	uint32_t mdns_y_pbt_wei13 = mdns->mdns_y_pbt_stren_intp*mdns->mdns_y_pbt_npv_array[13];
	uint32_t mdns_y_pbt_wei14 = mdns->mdns_y_pbt_stren_intp*mdns->mdns_y_pbt_npv_array[14];
	uint32_t mdns_y_pbt_wei15 = mdns->mdns_y_pbt_stren_intp*mdns->mdns_y_pbt_npv_array[15];
	uint32_t mdns_y_pbt_wei16 = mdns->mdns_y_pbt_stren_intp*mdns->mdns_y_pbt_npv_array[16];
	uint32_t mdns_y_pbt_wei17 = mdns->mdns_y_pbt_stren_intp*mdns->mdns_y_pbt_npv_array[17];
	uint32_t mdns_y_pbt_wei18 = mdns->mdns_y_pbt_stren_intp*mdns->mdns_y_pbt_npv_array[18];
	uint32_t mdns_y_pbt_wei19 = mdns->mdns_y_pbt_stren_intp*mdns->mdns_y_pbt_npv_array[19];
	uint32_t mdns_y_pbt_wei20 = mdns->mdns_y_pbt_stren_intp*mdns->mdns_y_pbt_npv_array[20];
	uint32_t mdns_y_pbt_wei21 = mdns->mdns_y_pbt_stren_intp*mdns->mdns_y_pbt_npv_array[21];
	uint32_t mdns_y_pbt_wei22 = mdns->mdns_y_pbt_stren_intp*mdns->mdns_y_pbt_npv_array[22];
	uint32_t mdns_y_pbt_wei23 = mdns->mdns_y_pbt_stren_intp*mdns->mdns_y_pbt_npv_array[23];
	uint32_t mdns_y_pbt_wei24 = mdns->mdns_y_pbt_stren_intp*mdns->mdns_y_pbt_npv_array[24];
	uint32_t mdns_y_pbt_wei25 = mdns->mdns_y_pbt_stren_intp*mdns->mdns_y_pbt_npv_array[25];
	uint32_t mdns_y_pbt_wei26 = mdns->mdns_y_pbt_stren_intp*mdns->mdns_y_pbt_npv_array[26];
	uint32_t mdns_y_pbt_wei27 = mdns->mdns_y_pbt_stren_intp*mdns->mdns_y_pbt_npv_array[27];
	uint32_t mdns_y_pbt_wei28 = mdns->mdns_y_pbt_stren_intp*mdns->mdns_y_pbt_npv_array[28];
	uint32_t mdns_y_pbt_wei29 = mdns->mdns_y_pbt_stren_intp*mdns->mdns_y_pbt_npv_array[29];
	uint32_t mdns_y_pbt_wei30 = mdns->mdns_y_pbt_stren_intp*mdns->mdns_y_pbt_npv_array[30];
	uint32_t mdns_y_pbt_wei31 = mdns->mdns_y_pbt_stren_intp*mdns->mdns_y_pbt_npv_array[31];

	uint32_t mdns_y_sad_wei0 = mdns->mdns_y_sad_stren_intp*mdns->mdns_y_sad_npv_array[0];
	uint32_t mdns_y_sad_wei1 = mdns->mdns_y_sad_stren_intp*mdns->mdns_y_sad_npv_array[1];
	uint32_t mdns_y_sad_wei2 = mdns->mdns_y_sad_stren_intp*mdns->mdns_y_sad_npv_array[2];
	uint32_t mdns_y_sad_wei3 = mdns->mdns_y_sad_stren_intp*mdns->mdns_y_sad_npv_array[3];

	uint32_t mdns_y_sad_slope1;
	uint32_t mdns_y_sad_slope2;
	uint32_t mdns_y_sad_slope3;
	uint32_t mdns_y_sad_slope4;

	reg_value = (mdns->mdns_y_sad_win_wei_intp << 24) |
		    (mdns->mdns_y_sad_win_opt_intp << 16) |
		    (mdns->mdns_y_sad_win_wei_intp << 8) |
		    (mdns->mdns_y_sad_win_opt_intp);
	system_reg_write(((tisp_core_t *)mdns->core)->priv_data, MDNS_ADDR_Y_SAD_CFG, reg_value);

	reg_value = ((mdns->mdns_y_sad_thres_intp + 10) << 16) |
		( mdns->mdns_y_sad_thres_intp    );
	system_reg_write(((tisp_core_t *)mdns->core)->priv_data, MDNS_ADDR_Y_REF_WEI_SGM_X_PAR0, reg_value);
	system_reg_write(((tisp_core_t *)mdns->core)->priv_data, MDNS_ADDR_Y_REF_WEI_SGM_X_PAR1, (400<<16) | 300);
	system_reg_write(((tisp_core_t *)mdns->core)->priv_data, MDNS_ADDR_Y_REF_WEI_SGM_X_PAR2, 510);

	reg_value = (mdns->mdns_y_sta_win_wei_intp << 24) |
		    (mdns->mdns_y_sta_win_opt_intp << 16) |
		    (mdns->mdns_y_sta_win_wei_intp << 8) |
		    (mdns->mdns_y_sta_win_opt_intp);
	system_reg_write(((tisp_core_t *)mdns->core)->priv_data, MDNS_ADDR_STA_WIN_CFG, reg_value);
	system_reg_write(((tisp_core_t *)mdns->core)->priv_data, MDNS_ADDR_STA_MAX_NUM, mdns->mdns_y_sta_mx_num_intp);
	system_reg_write(((tisp_core_t *)mdns->core)->priv_data, MDNS_ADDR_STA_MV_NUM_THRES00, mdns->mdns_y_sta_mv_num_intp);
	system_reg_write(((tisp_core_t *)mdns->core)->priv_data, MDNS_ADDR_STA_MV_NUM_THRES01, mdns->mdns_y_sta_mv_num_intp);
	system_reg_write(((tisp_core_t *)mdns->core)->priv_data, MDNS_ADDR_STA_MV_NUM_THRES10, mdns->mdns_y_sta_mv_num_intp);
	system_reg_write(((tisp_core_t *)mdns->core)->priv_data, MDNS_ADDR_STA_MV_NUM_THRES11, mdns->mdns_y_sta_mv_num_intp);
	system_reg_write(((tisp_core_t *)mdns->core)->priv_data, MDNS_ADDR_STA_DIFF_THRES, mdns->mdns_y_sta_thres_intp);

	reg_value = 	(mdns->mdns_y_pbt_win_wei_intp << 24) |
			(mdns->mdns_y_pbt_win_opt_intp << 16) |
			(mdns->mdns_y_pbt_win_wei_intp << 8) |
			(mdns->mdns_y_pbt_win_opt_intp);
	system_reg_write(((tisp_core_t *)mdns->core)->priv_data, MDNS_ADDR_PBT_WIN_CFG, reg_value);
	system_reg_write(((tisp_core_t *)mdns->core)->priv_data, MDNS_ADDR_PBT_MAX_NUM, 	mdns->mdns_y_pbt_mx_num_intp);
	system_reg_write(((tisp_core_t *)mdns->core)->priv_data, MDNS_ADDR_PBT_MV_NUM_THRES00, 	mdns->mdns_y_pbt_mv_num_intp);
	system_reg_write(((tisp_core_t *)mdns->core)->priv_data, MDNS_ADDR_PBT_MV_NUM_THRES01, 	mdns->mdns_y_pbt_mv_num_intp);
	system_reg_write(((tisp_core_t *)mdns->core)->priv_data, MDNS_ADDR_PBT_MV_NUM_THRES10, 	mdns->mdns_y_pbt_mv_num_intp);
	system_reg_write(((tisp_core_t *)mdns->core)->priv_data, MDNS_ADDR_PBT_MV_NUM_THRES11, 	mdns->mdns_y_pbt_mv_num_intp);
	system_reg_write(((tisp_core_t *)mdns->core)->priv_data, MDNS_ADDR_PBT_DIFF_THRES, 	mdns->mdns_y_pbt_thres_intp);

	reg_value = (mdns_y_sta_wei1  << 16) |
		(mdns_y_sta_wei0);
	system_reg_write(((tisp_core_t *)mdns->core)->priv_data, MDNS_ADDR_Y_STA_WEI_PAR0,reg_value);
	reg_value = (mdns_y_sta_wei3  << 16) |
		(mdns_y_sta_wei2);
	system_reg_write(((tisp_core_t *)mdns->core)->priv_data, MDNS_ADDR_Y_STA_WEI_PAR1,reg_value);
	reg_value = (mdns_y_sta_wei5  << 16) |
		(mdns_y_sta_wei4);
	system_reg_write(((tisp_core_t *)mdns->core)->priv_data, MDNS_ADDR_Y_STA_WEI_PAR2,reg_value);
	reg_value = (mdns_y_sta_wei7  << 16) |
		(mdns_y_sta_wei6);
	system_reg_write(((tisp_core_t *)mdns->core)->priv_data, MDNS_ADDR_Y_STA_WEI_PAR3,reg_value);
	reg_value = (mdns_y_sta_wei9  << 16) |
		(mdns_y_sta_wei8);
	system_reg_write(((tisp_core_t *)mdns->core)->priv_data, MDNS_ADDR_Y_STA_WEI_PAR4,reg_value);
	reg_value = (mdns_y_sta_wei11 << 16) |
		(mdns_y_sta_wei10);
	system_reg_write(((tisp_core_t *)mdns->core)->priv_data, MDNS_ADDR_Y_STA_WEI_PAR5,reg_value);
	reg_value = (mdns_y_sta_wei13 << 16) |
		(mdns_y_sta_wei12);
	system_reg_write(((tisp_core_t *)mdns->core)->priv_data, MDNS_ADDR_Y_STA_WEI_PAR6,reg_value);
	reg_value = (mdns_y_sta_wei15 << 16) |
		(mdns_y_sta_wei14);
	system_reg_write(((tisp_core_t *)mdns->core)->priv_data, MDNS_ADDR_Y_STA_WEI_PAR7,reg_value);
	reg_value = (mdns_y_sta_wei17 << 16) |
		(mdns_y_sta_wei16);
	system_reg_write(((tisp_core_t *)mdns->core)->priv_data, MDNS_ADDR_Y_STA_WEI_PAR8,reg_value);
	reg_value = (mdns_y_sta_wei19 << 16) |
		(mdns_y_sta_wei18);
	system_reg_write(((tisp_core_t *)mdns->core)->priv_data, MDNS_ADDR_Y_STA_WEI_PAR9,reg_value);
	reg_value = (mdns_y_sta_wei21 << 16) |
		(mdns_y_sta_wei20);
	system_reg_write(((tisp_core_t *)mdns->core)->priv_data, MDNS_ADDR_Y_STA_WEI_PARa,reg_value);
	reg_value = (mdns_y_sta_wei23 << 16) |
		(mdns_y_sta_wei22);
	system_reg_write(((tisp_core_t *)mdns->core)->priv_data, MDNS_ADDR_Y_STA_WEI_PARb,reg_value);
	reg_value = (mdns_y_sta_wei25 << 16) |
		(mdns_y_sta_wei24);
	system_reg_write(((tisp_core_t *)mdns->core)->priv_data, MDNS_ADDR_Y_STA_WEI_PARc,reg_value);
	reg_value = (mdns_y_sta_wei27 << 16) |
		(mdns_y_sta_wei26);
	system_reg_write(((tisp_core_t *)mdns->core)->priv_data, MDNS_ADDR_Y_STA_WEI_PARd,reg_value);
	reg_value = (mdns_y_sta_wei29 << 16) |
		(mdns_y_sta_wei28);
	system_reg_write(((tisp_core_t *)mdns->core)->priv_data, MDNS_ADDR_Y_STA_WEI_PARe,reg_value);
	reg_value = (mdns_y_sta_wei31 << 16) |
		(mdns_y_sta_wei30);
	system_reg_write(((tisp_core_t *)mdns->core)->priv_data, MDNS_ADDR_Y_STA_WEI_PARf,reg_value);

	reg_value = (mdns_y_pbt_wei1  << 16) |
		(mdns_y_pbt_wei0);
	system_reg_write(((tisp_core_t *)mdns->core)->priv_data, MDNS_ADDR_Y_PBT_WEI_PAR0,reg_value);
	reg_value = (mdns_y_pbt_wei3  << 16) |
		(mdns_y_pbt_wei2);
	system_reg_write(((tisp_core_t *)mdns->core)->priv_data, MDNS_ADDR_Y_PBT_WEI_PAR1,reg_value);
	reg_value = (mdns_y_pbt_wei5  << 16) |
		(mdns_y_pbt_wei4);
	system_reg_write(((tisp_core_t *)mdns->core)->priv_data, MDNS_ADDR_Y_PBT_WEI_PAR2,reg_value);
	reg_value = (mdns_y_pbt_wei7  << 16) |
		(mdns_y_pbt_wei6);
	system_reg_write(((tisp_core_t *)mdns->core)->priv_data, MDNS_ADDR_Y_PBT_WEI_PAR3,reg_value);
	reg_value = (mdns_y_pbt_wei9  << 16) |
		(mdns_y_pbt_wei8);
	system_reg_write(((tisp_core_t *)mdns->core)->priv_data, MDNS_ADDR_Y_PBT_WEI_PAR4,reg_value);
	reg_value = (mdns_y_pbt_wei11 << 16) |
		(mdns_y_pbt_wei10);
	system_reg_write(((tisp_core_t *)mdns->core)->priv_data, MDNS_ADDR_Y_PBT_WEI_PAR5,reg_value);
	reg_value = (mdns_y_pbt_wei13 << 16) |
		(mdns_y_pbt_wei12);
	system_reg_write(((tisp_core_t *)mdns->core)->priv_data, MDNS_ADDR_Y_PBT_WEI_PAR6,reg_value);
	reg_value = (mdns_y_pbt_wei15 << 16) |
		(mdns_y_pbt_wei14);
	system_reg_write(((tisp_core_t *)mdns->core)->priv_data, MDNS_ADDR_Y_PBT_WEI_PAR7,reg_value);
	reg_value = (mdns_y_pbt_wei17 << 16) |
		(mdns_y_pbt_wei16);
	system_reg_write(((tisp_core_t *)mdns->core)->priv_data, MDNS_ADDR_Y_PBT_WEI_PAR8,reg_value);
	reg_value = (mdns_y_pbt_wei19 << 16) |
		(mdns_y_pbt_wei18);
	system_reg_write(((tisp_core_t *)mdns->core)->priv_data, MDNS_ADDR_Y_PBT_WEI_PAR9,reg_value);
	reg_value = (mdns_y_pbt_wei21 << 16) |
		(mdns_y_pbt_wei20);
	system_reg_write(((tisp_core_t *)mdns->core)->priv_data, MDNS_ADDR_Y_PBT_WEI_PARa,reg_value);
	reg_value = (mdns_y_pbt_wei23 << 16) |
		(mdns_y_pbt_wei22);
	system_reg_write(((tisp_core_t *)mdns->core)->priv_data, MDNS_ADDR_Y_PBT_WEI_PARb,reg_value);
	reg_value = (mdns_y_pbt_wei25 << 16) |
		(mdns_y_pbt_wei24);
	system_reg_write(((tisp_core_t *)mdns->core)->priv_data, MDNS_ADDR_Y_PBT_WEI_PARc,reg_value);
	reg_value = (mdns_y_pbt_wei27 << 16) |
		(mdns_y_pbt_wei26);
	system_reg_write(((tisp_core_t *)mdns->core)->priv_data, MDNS_ADDR_Y_PBT_WEI_PARd,reg_value);
	reg_value = (mdns_y_pbt_wei29 << 16) |
		(mdns_y_pbt_wei28);
	system_reg_write(((tisp_core_t *)mdns->core)->priv_data, MDNS_ADDR_Y_PBT_WEI_PARe,reg_value);
	reg_value = (mdns_y_pbt_wei31 << 16) |
		(mdns_y_pbt_wei30);
	system_reg_write(((tisp_core_t *)mdns->core)->priv_data, MDNS_ADDR_Y_PBT_WEI_PARf,reg_value);

	reg_value = (mdns_y_sad_wei1  << 16) |
		(mdns_y_sad_wei0);
	system_reg_write(((tisp_core_t *)mdns->core)->priv_data, MDNS_ADDR_Y_REF_WEI_SGM_Y_PAR0,reg_value);
	reg_value = (mdns_y_sad_wei3  << 16) |
		(mdns_y_sad_wei2);
	system_reg_write(((tisp_core_t *)mdns->core)->priv_data, MDNS_ADDR_Y_REF_WEI_SGM_Y_PAR1,reg_value);

	reg_value = (mdns->mdns_y_ref_wei_max_intp << 16) |
		(mdns->mdns_y_ref_wei_min_intp);
	system_reg_write(((tisp_core_t *)mdns->core)->priv_data, MDNS_ADDR_Y_REF_WEI_LMT,reg_value);

	mdns_y_sad_slope1 = (mdns_y_sad_wei0 - mdns_y_sad_wei1) * 8 / 10;
	mdns_y_sad_slope2 = (mdns_y_sad_wei1 - mdns_y_sad_wei2) * 8 / (390 - mdns->mdns_y_sad_thres_intp);
	mdns_y_sad_slope3 = (mdns_y_sad_wei2 - mdns_y_sad_wei3) * 8 / 100;
	mdns_y_sad_slope4 = (mdns_y_sad_wei3 - 0) * 8 / 110;

	reg_value = (mdns_y_sad_slope2 << 16) |
		(mdns_y_sad_slope1);
	system_reg_write(((tisp_core_t *)mdns->core)->priv_data, MDNS_ADDR_Y_REF_WEI_SGM_S_PAR0,reg_value);

	reg_value = (mdns_y_sad_slope4 << 16) |
		(mdns_y_sad_slope3);
	system_reg_write(((tisp_core_t *)mdns->core)->priv_data, MDNS_ADDR_Y_REF_WEI_SGM_S_PAR1,reg_value);

	return 0;
}

uint32_t tisp_mdns_y_2d_param_cfg(tisp_mdns_t *mdns)
{
	uint32_t reg_value;

	uint32_t mdns_y_cs_stren_0 = mdns->mdns_y_cs_stren_intp*mdns->mdns_y_cs_npv_array[0];
	uint32_t mdns_y_cs_stren_1 = mdns->mdns_y_cs_stren_intp*mdns->mdns_y_cs_npv_array[1];
	uint32_t mdns_y_cs_stren_2 = mdns->mdns_y_cs_stren_intp*mdns->mdns_y_cs_npv_array[2];
	uint32_t mdns_y_cs_stren_3 = mdns->mdns_y_cs_stren_intp*mdns->mdns_y_cs_npv_array[3];
	uint32_t mdns_y_cs_stren_4 = mdns->mdns_y_cs_stren_intp*mdns->mdns_y_cs_npv_array[4];
	uint32_t mdns_y_cs_stren_5 = mdns->mdns_y_cs_stren_intp*mdns->mdns_y_cs_npv_array[5];
	uint32_t mdns_y_cs_stren_6 = mdns->mdns_y_cs_stren_intp*mdns->mdns_y_cs_npv_array[6];
	uint32_t mdns_y_cs_stren_7 = mdns->mdns_y_cs_stren_intp*mdns->mdns_y_cs_npv_array[7];
	uint32_t mdns_y_cs_stren_8 = mdns->mdns_y_cs_stren_intp*mdns->mdns_y_cs_npv_array[8];
	uint32_t mdns_y_cs_stren_9 = mdns->mdns_y_cs_stren_intp*mdns->mdns_y_cs_npv_array[9];
	uint32_t mdns_y_cs_stren_10 = mdns->mdns_y_cs_stren_intp*mdns->mdns_y_cs_npv_array[10];
	uint32_t mdns_y_cs_stren_11 = mdns->mdns_y_cs_stren_intp*mdns->mdns_y_cs_npv_array[11];
	uint32_t mdns_y_cs_stren_12 = mdns->mdns_y_cs_stren_intp*mdns->mdns_y_cs_npv_array[12];
	uint32_t mdns_y_cs_stren_13 = mdns->mdns_y_cs_stren_intp*mdns->mdns_y_cs_npv_array[13];
	uint32_t mdns_y_cs_stren_14 = mdns->mdns_y_cs_stren_intp*mdns->mdns_y_cs_npv_array[14];
	uint32_t mdns_y_cs_stren_15 = mdns->mdns_y_cs_stren_intp*mdns->mdns_y_cs_npv_array[15];

	uint32_t mdns_y_rs_stren_0 = mdns->mdns_y_rs_stren_intp*mdns->mdns_y_rs_npv_array[0];
	uint32_t mdns_y_rs_stren_1 = mdns->mdns_y_rs_stren_intp*mdns->mdns_y_rs_npv_array[1];
	uint32_t mdns_y_rs_stren_2 = mdns->mdns_y_rs_stren_intp*mdns->mdns_y_rs_npv_array[2];
	uint32_t mdns_y_rs_stren_3 = mdns->mdns_y_rs_stren_intp*mdns->mdns_y_rs_npv_array[3];
	uint32_t mdns_y_rs_stren_4 = mdns->mdns_y_rs_stren_intp*mdns->mdns_y_rs_npv_array[4];
	uint32_t mdns_y_rs_stren_5 = mdns->mdns_y_rs_stren_intp*mdns->mdns_y_rs_npv_array[5];
	uint32_t mdns_y_rs_stren_6 = mdns->mdns_y_rs_stren_intp*mdns->mdns_y_rs_npv_array[6];
	uint32_t mdns_y_rs_stren_7 = mdns->mdns_y_rs_stren_intp*mdns->mdns_y_rs_npv_array[7];
	uint32_t mdns_y_rs_stren_8 = mdns->mdns_y_rs_stren_intp*mdns->mdns_y_rs_npv_array[8];
	uint32_t mdns_y_rs_stren_9 = mdns->mdns_y_rs_stren_intp*mdns->mdns_y_rs_npv_array[9];
	uint32_t mdns_y_rs_stren_10 = mdns->mdns_y_rs_stren_intp*mdns->mdns_y_rs_npv_array[10];
	uint32_t mdns_y_rs_stren_11 = mdns->mdns_y_rs_stren_intp*mdns->mdns_y_rs_npv_array[11];
	uint32_t mdns_y_rs_stren_12 = mdns->mdns_y_rs_stren_intp*mdns->mdns_y_rs_npv_array[12];
	uint32_t mdns_y_rs_stren_13 = mdns->mdns_y_rs_stren_intp*mdns->mdns_y_rs_npv_array[13];
	uint32_t mdns_y_rs_stren_14 = mdns->mdns_y_rs_stren_intp*mdns->mdns_y_rs_npv_array[14];
	uint32_t mdns_y_rs_stren_15 = mdns->mdns_y_rs_stren_intp*mdns->mdns_y_rs_npv_array[15];


	uint32_t mdns_y_cm_stren_0 = mdns->mdns_y_cm_stren_intp*mdns->mdns_y_cm_npv_array[0];
	uint32_t mdns_y_cm_stren_1 = mdns->mdns_y_cm_stren_intp*mdns->mdns_y_cm_npv_array[1];
	uint32_t mdns_y_cm_stren_2 = mdns->mdns_y_cm_stren_intp*mdns->mdns_y_cm_npv_array[2];
	uint32_t mdns_y_cm_stren_3 = mdns->mdns_y_cm_stren_intp*mdns->mdns_y_cm_npv_array[3];
	uint32_t mdns_y_cm_stren_4 = mdns->mdns_y_cm_stren_intp*mdns->mdns_y_cm_npv_array[4];
	uint32_t mdns_y_cm_stren_5 = mdns->mdns_y_cm_stren_intp*mdns->mdns_y_cm_npv_array[5];
	uint32_t mdns_y_cm_stren_6 = mdns->mdns_y_cm_stren_intp*mdns->mdns_y_cm_npv_array[6];
	uint32_t mdns_y_cm_stren_7 = mdns->mdns_y_cm_stren_intp*mdns->mdns_y_cm_npv_array[7];
	uint32_t mdns_y_cm_stren_8 = mdns->mdns_y_cm_stren_intp*mdns->mdns_y_cm_npv_array[8];
	uint32_t mdns_y_cm_stren_9 = mdns->mdns_y_cm_stren_intp*mdns->mdns_y_cm_npv_array[9];
	uint32_t mdns_y_cm_stren_10 = mdns->mdns_y_cm_stren_intp*mdns->mdns_y_cm_npv_array[10];
	uint32_t mdns_y_cm_stren_11 = mdns->mdns_y_cm_stren_intp*mdns->mdns_y_cm_npv_array[11];
	uint32_t mdns_y_cm_stren_12 = mdns->mdns_y_cm_stren_intp*mdns->mdns_y_cm_npv_array[12];
	uint32_t mdns_y_cm_stren_13 = mdns->mdns_y_cm_stren_intp*mdns->mdns_y_cm_npv_array[13];
	uint32_t mdns_y_cm_stren_14 = mdns->mdns_y_cm_stren_intp*mdns->mdns_y_cm_npv_array[14];
	uint32_t mdns_y_cm_stren_15 = mdns->mdns_y_cm_stren_intp*mdns->mdns_y_cm_npv_array[15];


	uint32_t mdns_y_rm_stren_0 = mdns->mdns_y_rm_stren_intp*mdns->mdns_y_rm_npv_array[0];
	uint32_t mdns_y_rm_stren_1 = mdns->mdns_y_rm_stren_intp*mdns->mdns_y_rm_npv_array[1];
	uint32_t mdns_y_rm_stren_2 = mdns->mdns_y_rm_stren_intp*mdns->mdns_y_rm_npv_array[2];
	uint32_t mdns_y_rm_stren_3 = mdns->mdns_y_rm_stren_intp*mdns->mdns_y_rm_npv_array[3];
	uint32_t mdns_y_rm_stren_4 = mdns->mdns_y_rm_stren_intp*mdns->mdns_y_rm_npv_array[4];
	uint32_t mdns_y_rm_stren_5 = mdns->mdns_y_rm_stren_intp*mdns->mdns_y_rm_npv_array[5];
	uint32_t mdns_y_rm_stren_6 = mdns->mdns_y_rm_stren_intp*mdns->mdns_y_rm_npv_array[6];
	uint32_t mdns_y_rm_stren_7 = mdns->mdns_y_rm_stren_intp*mdns->mdns_y_rm_npv_array[7];
	uint32_t mdns_y_rm_stren_8 = mdns->mdns_y_rm_stren_intp*mdns->mdns_y_rm_npv_array[8];
	uint32_t mdns_y_rm_stren_9 = mdns->mdns_y_rm_stren_intp*mdns->mdns_y_rm_npv_array[9];
	uint32_t mdns_y_rm_stren_10 = mdns->mdns_y_rm_stren_intp*mdns->mdns_y_rm_npv_array[10];
	uint32_t mdns_y_rm_stren_11 = mdns->mdns_y_rm_stren_intp*mdns->mdns_y_rm_npv_array[11];
	uint32_t mdns_y_rm_stren_12 = mdns->mdns_y_rm_stren_intp*mdns->mdns_y_rm_npv_array[12];
	uint32_t mdns_y_rm_stren_13 = mdns->mdns_y_rm_stren_intp*mdns->mdns_y_rm_npv_array[13];
	uint32_t mdns_y_rm_stren_14 = mdns->mdns_y_rm_stren_intp*mdns->mdns_y_rm_npv_array[14];
	uint32_t mdns_y_rm_stren_15 = mdns->mdns_y_rm_stren_intp*mdns->mdns_y_rm_npv_array[15];

	reg_value = 	(mdns->mdns_y_edge_type_array[0] << 16) |
			(mdns->mdns_y_edge_win_intp);
	system_reg_write(((tisp_core_t *)mdns->core)->priv_data, MDNS_ADDR_Y_EDGE_PAR, reg_value);
	system_reg_write(((tisp_core_t *)mdns->core)->priv_data, MDNS_ADDR_Y_BI_PAR0, mdns->mdns_y_bi_thres_intp);
	system_reg_write(((tisp_core_t *)mdns->core)->priv_data, MDNS_ADDR_Y_BI_PAR1, 0);

	system_reg_write(((tisp_core_t *)mdns->core)->priv_data, MDNS_ADDR_Y_FS_MV_JUDGE_THRES, mdns->mdns_y_smj_thres_intp);
	system_reg_write(((tisp_core_t *)mdns->core)->priv_data, MDNS_ADDR_Y_FS_S_SHP_WEI, mdns->mdns_y_shp_swei_intp);
	system_reg_write(((tisp_core_t *)mdns->core)->priv_data, MDNS_ADDR_Y_FS_M_SHP_WEI, mdns->mdns_y_shp_mwei_intp);

	reg_value = 	(mdns->mdns_y_cs_bhold_intp << 28) |
			(mdns->mdns_y_cs_sego_intp << 24) |
			(mdns->mdns_y_cs_ewin_intp << 16) |
			(mdns->mdns_y_cs_iwin_intp << 8) |
			(mdns->mdns_y_cs_bwin_intp);
	system_reg_write(((tisp_core_t *)mdns->core)->priv_data, MDNS_ADDR_Y_FS_S_CUR_SHP_WIN_OPT, reg_value);

	reg_value = 	(mdns->mdns_y_rs_bhold_intp << 28) |
			(mdns->mdns_y_rs_sego_intp << 24) |
			(mdns->mdns_y_rs_ewin_intp << 16) |
			(mdns->mdns_y_rs_iwin_intp << 8) |
			(mdns->mdns_y_rs_bwin_intp);
	system_reg_write(((tisp_core_t *)mdns->core)->priv_data, MDNS_ADDR_Y_FS_S_REF_SHP_WIN_OPT, reg_value);

	reg_value = 	(mdns->mdns_y_cm_bhold_intp << 28) |
			(mdns->mdns_y_cm_sego_intp << 24) |
			(mdns->mdns_y_cm_ewin_intp << 16) |
			(mdns->mdns_y_cm_iwin_intp << 8) |
			(mdns->mdns_y_cm_bwin_intp);
	system_reg_write(((tisp_core_t *)mdns->core)->priv_data, MDNS_ADDR_Y_FS_M_CUR_SHP_WIN_OPT, reg_value);

	reg_value = 	(mdns->mdns_y_rm_bhold_intp << 28) |
			(mdns->mdns_y_rm_sego_intp << 24) |
			(mdns->mdns_y_rm_ewin_intp << 16) |
			(mdns->mdns_y_rm_iwin_intp << 8) |
			(mdns->mdns_y_rm_bwin_intp);
	system_reg_write(((tisp_core_t *)mdns->core)->priv_data, MDNS_ADDR_Y_FS_M_REF_SHP_WIN_OPT, reg_value);

	reg_value = (mdns_y_cs_stren_3 << 24) |
		(mdns_y_cs_stren_2 << 16) |
		(mdns_y_cs_stren_1 << 8) |
		(mdns_y_cs_stren_0);
	system_reg_write(((tisp_core_t *)mdns->core)->priv_data, MDNS_ADDR_Y_FS_S_CUR_SHP_STREN_P0, reg_value);
	reg_value = (mdns_y_cs_stren_7 << 24) |
		(mdns_y_cs_stren_6 << 16) |
		(mdns_y_cs_stren_5 << 8) |
		(mdns_y_cs_stren_4);
	system_reg_write(((tisp_core_t *)mdns->core)->priv_data, MDNS_ADDR_Y_FS_S_CUR_SHP_STREN_P1, reg_value);
	reg_value = (mdns_y_cs_stren_11 << 24) |
		(mdns_y_cs_stren_10 << 16) |
		(mdns_y_cs_stren_9 << 8) |
		(mdns_y_cs_stren_8);
	system_reg_write(((tisp_core_t *)mdns->core)->priv_data, MDNS_ADDR_Y_FS_S_CUR_SHP_STREN_P2, reg_value);
	reg_value = (mdns_y_cs_stren_15 << 24) |
		(mdns_y_cs_stren_14 << 16) |
		(mdns_y_cs_stren_13 << 8) |
		(mdns_y_cs_stren_12);
	system_reg_write(((tisp_core_t *)mdns->core)->priv_data, MDNS_ADDR_Y_FS_S_CUR_SHP_STREN_P3, reg_value);

	reg_value = (mdns_y_rs_stren_3 << 24) |
		(mdns_y_rs_stren_2 << 16) |
		(mdns_y_rs_stren_1 << 8) |
		(mdns_y_rs_stren_0);
	system_reg_write(((tisp_core_t *)mdns->core)->priv_data, MDNS_ADDR_Y_FS_S_REF_SHP_STREN_P0, reg_value);
	reg_value = (mdns_y_rs_stren_7 << 24) |
		(mdns_y_rs_stren_6 << 16) |
		(mdns_y_rs_stren_5 << 8) |
		(mdns_y_rs_stren_4);
	system_reg_write(((tisp_core_t *)mdns->core)->priv_data, MDNS_ADDR_Y_FS_S_REF_SHP_STREN_P1, reg_value);
	reg_value = (mdns_y_rs_stren_11 << 24) |
		(mdns_y_rs_stren_10 << 16) |
		(mdns_y_rs_stren_9 << 8) |
		(mdns_y_rs_stren_8);
	system_reg_write(((tisp_core_t *)mdns->core)->priv_data, MDNS_ADDR_Y_FS_S_REF_SHP_STREN_P2, reg_value);
	reg_value = (mdns_y_rs_stren_15 << 24) |
		(mdns_y_rs_stren_14 << 16) |
		(mdns_y_rs_stren_13 << 8) |
		(mdns_y_rs_stren_12);
	system_reg_write(((tisp_core_t *)mdns->core)->priv_data, MDNS_ADDR_Y_FS_S_REF_SHP_STREN_P3, reg_value);

	reg_value = (mdns_y_cm_stren_3 << 24) |
		(mdns_y_cm_stren_2 << 16) |
		(mdns_y_cm_stren_1 << 8) |
		(mdns_y_cm_stren_0);
	system_reg_write(((tisp_core_t *)mdns->core)->priv_data, MDNS_ADDR_Y_FS_M_CUR_SHP_STREN_P0, reg_value);
	reg_value = (mdns_y_cm_stren_7 << 24) |
		(mdns_y_cm_stren_6 << 16) |
		(mdns_y_cm_stren_5 << 8) |
		(mdns_y_cm_stren_4);
	system_reg_write(((tisp_core_t *)mdns->core)->priv_data, MDNS_ADDR_Y_FS_M_CUR_SHP_STREN_P1, reg_value);
	reg_value = (mdns_y_cm_stren_11 << 24) |
		(mdns_y_cm_stren_10 << 16) |
		(mdns_y_cm_stren_9 << 8) |
		(mdns_y_cm_stren_8);
	system_reg_write(((tisp_core_t *)mdns->core)->priv_data, MDNS_ADDR_Y_FS_M_CUR_SHP_STREN_P2, reg_value);
	reg_value = (mdns_y_cm_stren_15 << 24) |
		(mdns_y_cm_stren_14 << 16) |
		(mdns_y_cm_stren_13 << 8) |
		(mdns_y_cm_stren_12);
	system_reg_write(((tisp_core_t *)mdns->core)->priv_data, MDNS_ADDR_Y_FS_M_CUR_SHP_STREN_P3, reg_value);

	reg_value = (mdns_y_rm_stren_3 << 24) |
		(mdns_y_rm_stren_2 << 16) |
		(mdns_y_rm_stren_1 << 8) |
		(mdns_y_rm_stren_0);
	system_reg_write(((tisp_core_t *)mdns->core)->priv_data, MDNS_ADDR_Y_FS_M_REF_SHP_STREN_P0, reg_value);
	reg_value = (mdns_y_rm_stren_7 << 24) |
		(mdns_y_rm_stren_6 << 16) |
		(mdns_y_rm_stren_5 << 8) |
		(mdns_y_rm_stren_4);
	system_reg_write(((tisp_core_t *)mdns->core)->priv_data, MDNS_ADDR_Y_FS_M_REF_SHP_STREN_P1, reg_value);
	reg_value = (mdns_y_rm_stren_11 << 24) |
		(mdns_y_rm_stren_10 << 16) |
		(mdns_y_rm_stren_9 << 8) |
		(mdns_y_rm_stren_8);
	system_reg_write(((tisp_core_t *)mdns->core)->priv_data, MDNS_ADDR_Y_FS_M_REF_SHP_STREN_P2, reg_value);
	reg_value = (mdns_y_rm_stren_15 << 24) |
		(mdns_y_rm_stren_14 << 16) |
		(mdns_y_rm_stren_13 << 8) |
		(mdns_y_rm_stren_12);
	system_reg_write(((tisp_core_t *)mdns->core)->priv_data, MDNS_ADDR_Y_FS_M_REF_SHP_STREN_P3, reg_value);

	reg_value = 	(mdns->mdns_y_shp_c_7_npv_intp << 28) |
			(mdns->mdns_y_shp_c_6_npv_intp << 24) |
			(mdns->mdns_y_shp_c_5_npv_intp << 20) |
			(mdns->mdns_y_shp_c_4_npv_intp << 16) |
			(mdns->mdns_y_shp_c_3_npv_intp << 12) |
			(mdns->mdns_y_shp_c_2_npv_intp << 8) |
			(mdns->mdns_y_shp_c_1_npv_intp << 4) |
			(mdns->mdns_y_shp_c_0_npv_intp);
	system_reg_write(((tisp_core_t *)mdns->core)->priv_data, MDNS_ADDR_Y_FS_SHP_CUR_STA_WEI_P0,reg_value);

	reg_value = 	(mdns->mdns_y_shp_c_f_npv_intp << 28) |
			(mdns->mdns_y_shp_c_e_npv_intp << 24) |
			(mdns->mdns_y_shp_c_d_npv_intp << 20) |
			(mdns->mdns_y_shp_c_c_npv_intp << 16) |
			(mdns->mdns_y_shp_c_b_npv_intp << 12) |
			(mdns->mdns_y_shp_c_a_npv_intp << 8) |
			(mdns->mdns_y_shp_c_9_npv_intp << 4) |
			(mdns->mdns_y_shp_c_8_npv_intp);
	system_reg_write(((tisp_core_t *)mdns->core)->priv_data, MDNS_ADDR_Y_FS_SHP_CUR_STA_WEI_P1,reg_value);

	reg_value = 	(mdns->mdns_y_shp_r_7_npv_intp << 28) |
			(mdns->mdns_y_shp_r_6_npv_intp << 24) |
			(mdns->mdns_y_shp_r_5_npv_intp << 20) |
			(mdns->mdns_y_shp_r_4_npv_intp << 16) |
			(mdns->mdns_y_shp_r_3_npv_intp << 12) |
			(mdns->mdns_y_shp_r_2_npv_intp << 8) |
			(mdns->mdns_y_shp_r_1_npv_intp << 4) |
			(mdns->mdns_y_shp_r_0_npv_intp);
	system_reg_write(((tisp_core_t *)mdns->core)->priv_data, MDNS_ADDR_Y_FS_SHP_REF_STA_WEI_P0,reg_value);

	reg_value = 	(mdns->mdns_y_shp_r_f_npv_intp << 28) |
			(mdns->mdns_y_shp_r_e_npv_intp << 24) |
			(mdns->mdns_y_shp_r_d_npv_intp << 20) |
			(mdns->mdns_y_shp_r_c_npv_intp << 16) |
			(mdns->mdns_y_shp_r_b_npv_intp << 12) |
			(mdns->mdns_y_shp_r_a_npv_intp << 8) |
			(mdns->mdns_y_shp_r_9_npv_intp << 4) |
			(mdns->mdns_y_shp_r_8_npv_intp);
	system_reg_write(((tisp_core_t *)mdns->core)->priv_data, MDNS_ADDR_Y_FS_SHP_REF_STA_WEI_P1,reg_value);

	return 0;
}

uint32_t tisp_mdns_y_adj_param_cfg(tisp_mdns_t *mdns)
{
	uint32_t reg_value;

	uint32_t mdns_y_adj_tedge_0 = mdns->mdns_y_adj_tedg_s_intp*mdns->mdns_y_adj_tedg_n_array[0];
	uint32_t mdns_y_adj_tedge_1 = mdns->mdns_y_adj_tedg_s_intp*mdns->mdns_y_adj_tedg_n_array[1];
	uint32_t mdns_y_adj_tedge_2 = mdns->mdns_y_adj_tedg_s_intp*mdns->mdns_y_adj_tedg_n_array[2];
	uint32_t mdns_y_adj_tedge_3 = mdns->mdns_y_adj_tedg_s_intp*mdns->mdns_y_adj_tedg_n_array[3];
	uint32_t mdns_y_adj_tedge_4 = mdns->mdns_y_adj_tedg_s_intp*mdns->mdns_y_adj_tedg_n_array[4];
	uint32_t mdns_y_adj_tedge_5 = mdns->mdns_y_adj_tedg_s_intp*mdns->mdns_y_adj_tedg_n_array[5];
	uint32_t mdns_y_adj_tedge_6 = mdns->mdns_y_adj_tedg_s_intp*mdns->mdns_y_adj_tedg_n_array[6];
	uint32_t mdns_y_adj_tedge_7 = mdns->mdns_y_adj_tedg_s_intp*mdns->mdns_y_adj_tedg_n_array[7];

	uint32_t mdns_y_adj_wedge_0 = mdns->mdns_y_adj_wedg_s_intp*mdns->mdns_y_adj_wedg_n_array[0];
	uint32_t mdns_y_adj_wedge_1 = mdns->mdns_y_adj_wedg_s_intp*mdns->mdns_y_adj_wedg_n_array[1];
	uint32_t mdns_y_adj_wedge_2 = mdns->mdns_y_adj_wedg_s_intp*mdns->mdns_y_adj_wedg_n_array[2];
	uint32_t mdns_y_adj_wedge_3 = mdns->mdns_y_adj_wedg_s_intp*mdns->mdns_y_adj_wedg_n_array[3];
	uint32_t mdns_y_adj_wedge_4 = mdns->mdns_y_adj_wedg_s_intp*mdns->mdns_y_adj_wedg_n_array[4];
	uint32_t mdns_y_adj_wedge_5 = mdns->mdns_y_adj_wedg_s_intp*mdns->mdns_y_adj_wedg_n_array[5];
	uint32_t mdns_y_adj_wedge_6 = mdns->mdns_y_adj_wedg_s_intp*mdns->mdns_y_adj_wedg_n_array[6];
	uint32_t mdns_y_adj_wedge_7 = mdns->mdns_y_adj_wedg_s_intp*mdns->mdns_y_adj_wedg_n_array[7];

	uint32_t mdns_y_adj_tluma_0 = mdns->mdns_y_adj_tlum_s_intp*mdns->mdns_y_adj_tlum_n_array[0];
	uint32_t mdns_y_adj_tluma_1 = mdns->mdns_y_adj_tlum_s_intp*mdns->mdns_y_adj_tlum_n_array[1];
	uint32_t mdns_y_adj_tluma_2 = mdns->mdns_y_adj_tlum_s_intp*mdns->mdns_y_adj_tlum_n_array[2];
	uint32_t mdns_y_adj_tluma_3 = mdns->mdns_y_adj_tlum_s_intp*mdns->mdns_y_adj_tlum_n_array[3];
	uint32_t mdns_y_adj_tluma_4 = mdns->mdns_y_adj_tlum_s_intp*mdns->mdns_y_adj_tlum_n_array[4];
	uint32_t mdns_y_adj_tluma_5 = mdns->mdns_y_adj_tlum_s_intp*mdns->mdns_y_adj_tlum_n_array[5];
	uint32_t mdns_y_adj_tluma_6 = mdns->mdns_y_adj_tlum_s_intp*mdns->mdns_y_adj_tlum_n_array[6];
	uint32_t mdns_y_adj_tluma_7 = mdns->mdns_y_adj_tlum_s_intp*mdns->mdns_y_adj_tlum_n_array[7];
	uint32_t mdns_y_adj_tluma_8 = mdns->mdns_y_adj_tlum_s_intp*mdns->mdns_y_adj_tlum_n_array[8];
	uint32_t mdns_y_adj_tluma_9 = mdns->mdns_y_adj_tlum_s_intp*mdns->mdns_y_adj_tlum_n_array[9];
	uint32_t mdns_y_adj_tluma_10 = mdns->mdns_y_adj_tlum_s_intp*mdns->mdns_y_adj_tlum_n_array[10];
	uint32_t mdns_y_adj_tluma_11 = mdns->mdns_y_adj_tlum_s_intp*mdns->mdns_y_adj_tlum_n_array[11];
	uint32_t mdns_y_adj_tluma_12 = mdns->mdns_y_adj_tlum_s_intp*mdns->mdns_y_adj_tlum_n_array[12];
	uint32_t mdns_y_adj_tluma_13 = mdns->mdns_y_adj_tlum_s_intp*mdns->mdns_y_adj_tlum_n_array[13];
	uint32_t mdns_y_adj_tluma_14 = mdns->mdns_y_adj_tlum_s_intp*mdns->mdns_y_adj_tlum_n_array[14];
	uint32_t mdns_y_adj_tluma_15 = mdns->mdns_y_adj_tlum_s_intp*mdns->mdns_y_adj_tlum_n_array[15];

	uint32_t mdns_y_adj_wluma_0 = mdns->mdns_y_adj_wlum_s_intp*mdns->mdns_y_adj_wlum_n_array[0];
	uint32_t mdns_y_adj_wluma_1 = mdns->mdns_y_adj_wlum_s_intp*mdns->mdns_y_adj_wlum_n_array[1];
	uint32_t mdns_y_adj_wluma_2 = mdns->mdns_y_adj_wlum_s_intp*mdns->mdns_y_adj_wlum_n_array[2];
	uint32_t mdns_y_adj_wluma_3 = mdns->mdns_y_adj_wlum_s_intp*mdns->mdns_y_adj_wlum_n_array[3];
	uint32_t mdns_y_adj_wluma_4 = mdns->mdns_y_adj_wlum_s_intp*mdns->mdns_y_adj_wlum_n_array[4];
	uint32_t mdns_y_adj_wluma_5 = mdns->mdns_y_adj_wlum_s_intp*mdns->mdns_y_adj_wlum_n_array[5];
	uint32_t mdns_y_adj_wluma_6 = mdns->mdns_y_adj_wlum_s_intp*mdns->mdns_y_adj_wlum_n_array[6];
	uint32_t mdns_y_adj_wluma_7 = mdns->mdns_y_adj_wlum_s_intp*mdns->mdns_y_adj_wlum_n_array[7];
	uint32_t mdns_y_adj_wluma_8 = mdns->mdns_y_adj_wlum_s_intp*mdns->mdns_y_adj_wlum_n_array[8];
	uint32_t mdns_y_adj_wluma_9 = mdns->mdns_y_adj_wlum_s_intp*mdns->mdns_y_adj_wlum_n_array[9];
	uint32_t mdns_y_adj_wluma_10 = mdns->mdns_y_adj_wlum_s_intp*mdns->mdns_y_adj_wlum_n_array[10];
	uint32_t mdns_y_adj_wluma_11 = mdns->mdns_y_adj_wlum_s_intp*mdns->mdns_y_adj_wlum_n_array[11];
	uint32_t mdns_y_adj_wluma_12 = mdns->mdns_y_adj_wlum_s_intp*mdns->mdns_y_adj_wlum_n_array[12];
	uint32_t mdns_y_adj_wluma_13 = mdns->mdns_y_adj_wlum_s_intp*mdns->mdns_y_adj_wlum_n_array[13];
	uint32_t mdns_y_adj_wluma_14 = mdns->mdns_y_adj_wlum_s_intp*mdns->mdns_y_adj_wlum_n_array[14];
	uint32_t mdns_y_adj_wluma_15 = mdns->mdns_y_adj_wlum_s_intp*mdns->mdns_y_adj_wlum_n_array[15];

	reg_value = (mdns->mdns_y_adj_cnr_array[3] << 24) |
		(mdns->mdns_y_adj_cnr_array[2] << 16) |
		(mdns->mdns_y_adj_cnr_array[1] << 8) |
		(mdns->mdns_y_adj_cnr_array[0]);
	system_reg_write(((tisp_core_t *)mdns->core)->priv_data, MDNS_ADDR_Y_TCA_PAR,reg_value);

	reg_value = (mdns->mdns_y_fluct_lmt_array[3] << 24) |
		(mdns->mdns_y_fluct_lmt_array[2] << 16) |
		(mdns->mdns_y_fluct_lmt_array[1] << 8) |
		(mdns->mdns_y_fluct_lmt_array[0]);
	system_reg_write(((tisp_core_t *)mdns->core)->priv_data, MDNS_ADDR_Y_FS_SHP_CUR_FLUCT_PAR,reg_value);

	reg_value = (mdns->mdns_y_fluct_lmt_array[7] << 24) |
		(mdns->mdns_y_fluct_lmt_array[6] << 16) |
		(mdns->mdns_y_fluct_lmt_array[5] << 8) |
		(mdns->mdns_y_fluct_lmt_array[4]);
	system_reg_write(((tisp_core_t *)mdns->core)->priv_data, MDNS_ADDR_Y_FS_SHP_REF_FLUCT_PAR,reg_value);

	reg_value = (mdns->mdns_y_adj_sta_array[2] << 24) |
		(mdns->mdns_y_adj_sta_array[1] << 16) |
		(mdns->mdns_y_adj_sta_array[0]);
	system_reg_write(((tisp_core_t *)mdns->core)->priv_data, MDNS_ADDR_Y_WSA_S_PAR,reg_value);

	reg_value = (mdns->mdns_y_adj_sta_array[5] << 24) |
		(mdns->mdns_y_adj_sta_array[4] << 16) |
		(mdns->mdns_y_adj_sta_array[3]);
	system_reg_write(((tisp_core_t *)mdns->core)->priv_data, MDNS_ADDR_Y_WSA_M_PAR,reg_value);

	reg_value = (mdns->mdns_y_adj_sta_array[8] << 24) |
		(mdns->mdns_y_adj_sta_array[7] << 16) |
		(mdns->mdns_y_adj_sta_array[6]);
	system_reg_write(((tisp_core_t *)mdns->core)->priv_data, MDNS_ADDR_Y_WFA_PAR,reg_value);

	system_reg_write(((tisp_core_t *)mdns->core)->priv_data, MDNS_ADDR_Y_TEA_T_PAR0,0x20181004);
	system_reg_write(((tisp_core_t *)mdns->core)->priv_data, MDNS_ADDR_Y_TEA_T_PAR1,0x383028);

	reg_value = (mdns_y_adj_tedge_3 << 24) |
		(mdns_y_adj_tedge_2 << 16) |
		(mdns_y_adj_tedge_1 << 8) |
		(mdns_y_adj_tedge_0);
	system_reg_write(((tisp_core_t *)mdns->core)->priv_data, MDNS_ADDR_Y_TEA_V_PAR0,reg_value);

	reg_value = (mdns_y_adj_tedge_7 << 24) |
		(mdns_y_adj_tedge_6 << 16) |
		(mdns_y_adj_tedge_5 << 8) |
		(mdns_y_adj_tedge_4);
	system_reg_write(((tisp_core_t *)mdns->core)->priv_data, MDNS_ADDR_Y_TEA_V_PAR1,reg_value);

	reg_value = (mdns_y_adj_wedge_3 << 24) |
		(mdns_y_adj_wedge_2 << 16) |
		(mdns_y_adj_wedge_1 << 8) |
		(mdns_y_adj_wedge_0);
	system_reg_write(((tisp_core_t *)mdns->core)->priv_data, MDNS_ADDR_Y_TEA_W_PAR0,reg_value);

	reg_value = (mdns_y_adj_wedge_7 << 24) |
		(mdns_y_adj_wedge_6 << 16) |
		(mdns_y_adj_wedge_5 << 8) |
		(mdns_y_adj_wedge_4);
	system_reg_write(((tisp_core_t *)mdns->core)->priv_data, MDNS_ADDR_Y_TEA_W_PAR1, reg_value);

	system_reg_write(((tisp_core_t *)mdns->core)->priv_data, MDNS_ADDR_Y_TLA_T_PAR0, 0x241e140a);
	system_reg_write(((tisp_core_t *)mdns->core)->priv_data, MDNS_ADDR_Y_TLA_T_PAR1, 0x55443a30);
	system_reg_write(((tisp_core_t *)mdns->core)->priv_data, MDNS_ADDR_Y_TLA_T_PAR2, 0x8f7a685f);
	system_reg_write(((tisp_core_t *)mdns->core)->priv_data, MDNS_ADDR_Y_TLA_T_PAR3, 0xb0a098);

	reg_value = (mdns_y_adj_tluma_3 << 24) |
		(mdns_y_adj_tluma_2 << 16) |
		(mdns_y_adj_tluma_1 << 8) |
		(mdns_y_adj_tluma_0);
	system_reg_write(((tisp_core_t *)mdns->core)->priv_data, MDNS_ADDR_Y_TLA_V_PAR0, reg_value);

	reg_value = (mdns_y_adj_tluma_7 << 24) |
		(mdns_y_adj_tluma_6 << 16) |
		(mdns_y_adj_tluma_5 << 8) |
		(mdns_y_adj_tluma_4);
	system_reg_write(((tisp_core_t *)mdns->core)->priv_data, MDNS_ADDR_Y_TLA_V_PAR1,reg_value);

	reg_value = (mdns_y_adj_tluma_11<< 24) |
		(mdns_y_adj_tluma_10<< 16) |
		(mdns_y_adj_tluma_9 << 8) |
		(mdns_y_adj_tluma_8);
	system_reg_write(((tisp_core_t *)mdns->core)->priv_data, MDNS_ADDR_Y_TLA_V_PAR2,reg_value);

	reg_value = (mdns_y_adj_tluma_15<< 24) |
		(mdns_y_adj_tluma_14<< 16) |
		(mdns_y_adj_tluma_13<< 8) |
		(mdns_y_adj_tluma_12);
	system_reg_write(((tisp_core_t *)mdns->core)->priv_data, MDNS_ADDR_Y_TLA_V_PAR3,reg_value);

	reg_value = (mdns_y_adj_wluma_3 << 24) |
		(mdns_y_adj_wluma_2 << 16) |
		(mdns_y_adj_wluma_1 << 8) |
		(mdns_y_adj_wluma_0);
	system_reg_write(((tisp_core_t *)mdns->core)->priv_data, MDNS_ADDR_Y_TLA_W_PAR0,reg_value);

	reg_value = (mdns_y_adj_wluma_7 << 24) |
		(mdns_y_adj_wluma_6 << 16) |
		(mdns_y_adj_wluma_5 << 8) |
		(mdns_y_adj_wluma_4);
	system_reg_write(((tisp_core_t *)mdns->core)->priv_data, MDNS_ADDR_Y_TLA_W_PAR1,reg_value);

	reg_value = (mdns_y_adj_wluma_11<< 24) |
		(mdns_y_adj_wluma_10<< 16) |
		(mdns_y_adj_wluma_9 << 8) |
		(mdns_y_adj_wluma_8);
	system_reg_write(((tisp_core_t *)mdns->core)->priv_data, MDNS_ADDR_Y_TLA_W_PAR2,reg_value);

	reg_value = (mdns_y_adj_wluma_15<< 24) |
		(mdns_y_adj_wluma_14<< 16) |
		(mdns_y_adj_wluma_13<< 8) |
		(mdns_y_adj_wluma_12);
	system_reg_write(((tisp_core_t *)mdns->core)->priv_data, MDNS_ADDR_Y_TLA_W_PAR3,reg_value);
	system_reg_write(((tisp_core_t *)mdns->core)->priv_data, MDNS_ADDR_Y_TLA_WIN_OPT, mdns->mdns_y_adj_lum_win_array[0]);

	return 0;
}

int32_t tisp_mdns_c_3d_param_cfg(tisp_mdns_t *mdns)
{
	uint32_t reg_value;

	uint32_t mdns_c_sta_wei0 =  mdns->mdns_c_sta_stren_intp*mdns->mdns_c_sta_npv_array[0];
	uint32_t mdns_c_sta_wei1 =  mdns->mdns_c_sta_stren_intp*mdns->mdns_c_sta_npv_array[1];
	uint32_t mdns_c_sta_wei2 =  mdns->mdns_c_sta_stren_intp*mdns->mdns_c_sta_npv_array[2];
	uint32_t mdns_c_sta_wei3 =  mdns->mdns_c_sta_stren_intp*mdns->mdns_c_sta_npv_array[3];
	uint32_t mdns_c_sta_wei4 =  mdns->mdns_c_sta_stren_intp*mdns->mdns_c_sta_npv_array[4];
	uint32_t mdns_c_sta_wei5 =  mdns->mdns_c_sta_stren_intp*mdns->mdns_c_sta_npv_array[5];
	uint32_t mdns_c_sta_wei6 =  mdns->mdns_c_sta_stren_intp*mdns->mdns_c_sta_npv_array[6];
	uint32_t mdns_c_sta_wei7 =  mdns->mdns_c_sta_stren_intp*mdns->mdns_c_sta_npv_array[7];
	uint32_t mdns_c_sta_wei8 =  mdns->mdns_c_sta_stren_intp*mdns->mdns_c_sta_npv_array[8];
	uint32_t mdns_c_sta_wei9 =  mdns->mdns_c_sta_stren_intp*mdns->mdns_c_sta_npv_array[9];
	uint32_t mdns_c_sta_wei10 = mdns->mdns_c_sta_stren_intp*mdns->mdns_c_sta_npv_array[10];
	uint32_t mdns_c_sta_wei11 = mdns->mdns_c_sta_stren_intp*mdns->mdns_c_sta_npv_array[11];
	uint32_t mdns_c_sta_wei12 = mdns->mdns_c_sta_stren_intp*mdns->mdns_c_sta_npv_array[12];
	uint32_t mdns_c_sta_wei13 = mdns->mdns_c_sta_stren_intp*mdns->mdns_c_sta_npv_array[13];
	uint32_t mdns_c_sta_wei14 = mdns->mdns_c_sta_stren_intp*mdns->mdns_c_sta_npv_array[14];
	uint32_t mdns_c_sta_wei15 = mdns->mdns_c_sta_stren_intp*mdns->mdns_c_sta_npv_array[15];
	uint32_t mdns_c_sta_wei16 = mdns->mdns_c_sta_stren_intp*mdns->mdns_c_sta_npv_array[16];
	uint32_t mdns_c_sta_wei17 = mdns->mdns_c_sta_stren_intp*mdns->mdns_c_sta_npv_array[17];
	uint32_t mdns_c_sta_wei18 = mdns->mdns_c_sta_stren_intp*mdns->mdns_c_sta_npv_array[18];
	uint32_t mdns_c_sta_wei19 = mdns->mdns_c_sta_stren_intp*mdns->mdns_c_sta_npv_array[19];
	uint32_t mdns_c_sta_wei20 = mdns->mdns_c_sta_stren_intp*mdns->mdns_c_sta_npv_array[20];
	uint32_t mdns_c_sta_wei21 = mdns->mdns_c_sta_stren_intp*mdns->mdns_c_sta_npv_array[21];
	uint32_t mdns_c_sta_wei22 = mdns->mdns_c_sta_stren_intp*mdns->mdns_c_sta_npv_array[22];
	uint32_t mdns_c_sta_wei23 = mdns->mdns_c_sta_stren_intp*mdns->mdns_c_sta_npv_array[23];
	uint32_t mdns_c_sta_wei24 = mdns->mdns_c_sta_stren_intp*mdns->mdns_c_sta_npv_array[24];
	uint32_t mdns_c_sta_wei25 = mdns->mdns_c_sta_stren_intp*mdns->mdns_c_sta_npv_array[25];
	uint32_t mdns_c_sta_wei26 = mdns->mdns_c_sta_stren_intp*mdns->mdns_c_sta_npv_array[26];
	uint32_t mdns_c_sta_wei27 = mdns->mdns_c_sta_stren_intp*mdns->mdns_c_sta_npv_array[27];
	uint32_t mdns_c_sta_wei28 = mdns->mdns_c_sta_stren_intp*mdns->mdns_c_sta_npv_array[28];
	uint32_t mdns_c_sta_wei29 = mdns->mdns_c_sta_stren_intp*mdns->mdns_c_sta_npv_array[29];
	uint32_t mdns_c_sta_wei30 = mdns->mdns_c_sta_stren_intp*mdns->mdns_c_sta_npv_array[30];
	uint32_t mdns_c_sta_wei31 = mdns->mdns_c_sta_stren_intp*mdns->mdns_c_sta_npv_array[31];

	uint32_t mdns_c_pbt_wei0  = mdns->mdns_c_pbt_stren_intp*mdns->mdns_c_pbt_npv_array[0];
	uint32_t mdns_c_pbt_wei1  = mdns->mdns_c_pbt_stren_intp*mdns->mdns_c_pbt_npv_array[1];
	uint32_t mdns_c_pbt_wei2  = mdns->mdns_c_pbt_stren_intp*mdns->mdns_c_pbt_npv_array[2];
	uint32_t mdns_c_pbt_wei3  = mdns->mdns_c_pbt_stren_intp*mdns->mdns_c_pbt_npv_array[3];
	uint32_t mdns_c_pbt_wei4  = mdns->mdns_c_pbt_stren_intp*mdns->mdns_c_pbt_npv_array[4];
	uint32_t mdns_c_pbt_wei5  = mdns->mdns_c_pbt_stren_intp*mdns->mdns_c_pbt_npv_array[5];
	uint32_t mdns_c_pbt_wei6  = mdns->mdns_c_pbt_stren_intp*mdns->mdns_c_pbt_npv_array[6];
	uint32_t mdns_c_pbt_wei7  = mdns->mdns_c_pbt_stren_intp*mdns->mdns_c_pbt_npv_array[7];
	uint32_t mdns_c_pbt_wei8  = mdns->mdns_c_pbt_stren_intp*mdns->mdns_c_pbt_npv_array[8];
	uint32_t mdns_c_pbt_wei9  = mdns->mdns_c_pbt_stren_intp*mdns->mdns_c_pbt_npv_array[9];
	uint32_t mdns_c_pbt_wei10 = mdns->mdns_c_pbt_stren_intp*mdns->mdns_c_pbt_npv_array[10];
	uint32_t mdns_c_pbt_wei11 = mdns->mdns_c_pbt_stren_intp*mdns->mdns_c_pbt_npv_array[11];
	uint32_t mdns_c_pbt_wei12 = mdns->mdns_c_pbt_stren_intp*mdns->mdns_c_pbt_npv_array[12];
	uint32_t mdns_c_pbt_wei13 = mdns->mdns_c_pbt_stren_intp*mdns->mdns_c_pbt_npv_array[13];
	uint32_t mdns_c_pbt_wei14 = mdns->mdns_c_pbt_stren_intp*mdns->mdns_c_pbt_npv_array[14];
	uint32_t mdns_c_pbt_wei15 = mdns->mdns_c_pbt_stren_intp*mdns->mdns_c_pbt_npv_array[15];
	uint32_t mdns_c_pbt_wei16 = mdns->mdns_c_pbt_stren_intp*mdns->mdns_c_pbt_npv_array[16];
	uint32_t mdns_c_pbt_wei17 = mdns->mdns_c_pbt_stren_intp*mdns->mdns_c_pbt_npv_array[17];
	uint32_t mdns_c_pbt_wei18 = mdns->mdns_c_pbt_stren_intp*mdns->mdns_c_pbt_npv_array[18];
	uint32_t mdns_c_pbt_wei19 = mdns->mdns_c_pbt_stren_intp*mdns->mdns_c_pbt_npv_array[19];
	uint32_t mdns_c_pbt_wei20 = mdns->mdns_c_pbt_stren_intp*mdns->mdns_c_pbt_npv_array[20];
	uint32_t mdns_c_pbt_wei21 = mdns->mdns_c_pbt_stren_intp*mdns->mdns_c_pbt_npv_array[21];
	uint32_t mdns_c_pbt_wei22 = mdns->mdns_c_pbt_stren_intp*mdns->mdns_c_pbt_npv_array[22];
	uint32_t mdns_c_pbt_wei23 = mdns->mdns_c_pbt_stren_intp*mdns->mdns_c_pbt_npv_array[23];
	uint32_t mdns_c_pbt_wei24 = mdns->mdns_c_pbt_stren_intp*mdns->mdns_c_pbt_npv_array[24];
	uint32_t mdns_c_pbt_wei25 = mdns->mdns_c_pbt_stren_intp*mdns->mdns_c_pbt_npv_array[25];
	uint32_t mdns_c_pbt_wei26 = mdns->mdns_c_pbt_stren_intp*mdns->mdns_c_pbt_npv_array[26];
	uint32_t mdns_c_pbt_wei27 = mdns->mdns_c_pbt_stren_intp*mdns->mdns_c_pbt_npv_array[27];
	uint32_t mdns_c_pbt_wei28 = mdns->mdns_c_pbt_stren_intp*mdns->mdns_c_pbt_npv_array[28];
	uint32_t mdns_c_pbt_wei29 = mdns->mdns_c_pbt_stren_intp*mdns->mdns_c_pbt_npv_array[29];
	uint32_t mdns_c_pbt_wei30 = mdns->mdns_c_pbt_stren_intp*mdns->mdns_c_pbt_npv_array[30];
	uint32_t mdns_c_pbt_wei31 = mdns->mdns_c_pbt_stren_intp*mdns->mdns_c_pbt_npv_array[31];

	uint32_t mdns_c_sad_wei0 =  mdns->mdns_c_sad_stren_intp*mdns->mdns_c_sad_npv_array[0];
	uint32_t mdns_c_sad_wei1 =  mdns->mdns_c_sad_stren_intp*mdns->mdns_c_sad_npv_array[1];
	uint32_t mdns_c_sad_wei2 =  mdns->mdns_c_sad_stren_intp*mdns->mdns_c_sad_npv_array[2];
	uint32_t mdns_c_sad_wei3 =  mdns->mdns_c_sad_stren_intp*mdns->mdns_c_sad_npv_array[3];

	uint32_t mdns_c_sad_slope1;
	uint32_t mdns_c_sad_slope2;
	uint32_t mdns_c_sad_slope3;
	uint32_t mdns_c_sad_slope4;

	system_reg_write(((tisp_core_t *)mdns->core)->priv_data, MDNS_ADDR_UV_SAD_CFG, mdns->mdns_c_sad_win_opt_intp);

	reg_value = ((mdns->mdns_c_sad_thres_intp + 10) << 16) |
		( mdns->mdns_c_sad_thres_intp    );
	system_reg_write(((tisp_core_t *)mdns->core)->priv_data, MDNS_ADDR_UV_REF_WEI_SGM_X_PAR0, reg_value);
	system_reg_write(((tisp_core_t *)mdns->core)->priv_data, MDNS_ADDR_UV_REF_WEI_SGM_X_PAR1, (400<<16) | 300);
	system_reg_write(((tisp_core_t *)mdns->core)->priv_data, MDNS_ADDR_UV_REF_WEI_SGM_X_PAR2, 510);

	reg_value = (mdns_c_sta_wei1 << 16) |
		(mdns_c_sta_wei0);
	system_reg_write(((tisp_core_t *)mdns->core)->priv_data, MDNS_ADDR_UV_STA_WEI_PAR0,reg_value);
	reg_value = (mdns_c_sta_wei3 << 16) |
		(mdns_c_sta_wei2);
	system_reg_write(((tisp_core_t *)mdns->core)->priv_data, MDNS_ADDR_UV_STA_WEI_PAR1,reg_value);
	reg_value = (mdns_c_sta_wei5 << 16) |
		(mdns_c_sta_wei4);
	system_reg_write(((tisp_core_t *)mdns->core)->priv_data, MDNS_ADDR_UV_STA_WEI_PAR2,reg_value);
	reg_value = (mdns_c_sta_wei7 << 16) |
		(mdns_c_sta_wei6);
	system_reg_write(((tisp_core_t *)mdns->core)->priv_data, MDNS_ADDR_UV_STA_WEI_PAR3,reg_value);
	reg_value = (mdns_c_sta_wei9 << 16) |
		(mdns_c_sta_wei8);
	system_reg_write(((tisp_core_t *)mdns->core)->priv_data, MDNS_ADDR_UV_STA_WEI_PAR4,reg_value);
	reg_value = (mdns_c_sta_wei11 << 16) |
		(mdns_c_sta_wei10);
	system_reg_write(((tisp_core_t *)mdns->core)->priv_data, MDNS_ADDR_UV_STA_WEI_PAR5,reg_value);
	reg_value = (mdns_c_sta_wei13 << 16) |
		(mdns_c_sta_wei12);
	system_reg_write(((tisp_core_t *)mdns->core)->priv_data, MDNS_ADDR_UV_STA_WEI_PAR6,reg_value);
	reg_value = (mdns_c_sta_wei15 << 16) |
		(mdns_c_sta_wei14);
	system_reg_write(((tisp_core_t *)mdns->core)->priv_data, MDNS_ADDR_UV_STA_WEI_PAR7,reg_value);
	reg_value = (mdns_c_sta_wei17 << 16) |
		(mdns_c_sta_wei16);
	system_reg_write(((tisp_core_t *)mdns->core)->priv_data, MDNS_ADDR_UV_STA_WEI_PAR8,reg_value);
	reg_value = (mdns_c_sta_wei19 << 16) |
		(mdns_c_sta_wei18);
	system_reg_write(((tisp_core_t *)mdns->core)->priv_data, MDNS_ADDR_UV_STA_WEI_PAR9,reg_value);
	reg_value = (mdns_c_sta_wei21 << 16) |
		(mdns_c_sta_wei20);
	system_reg_write(((tisp_core_t *)mdns->core)->priv_data, MDNS_ADDR_UV_STA_WEI_PARa,reg_value);
	reg_value = (mdns_c_sta_wei23 << 16) |
		(mdns_c_sta_wei22);
	system_reg_write(((tisp_core_t *)mdns->core)->priv_data, MDNS_ADDR_UV_STA_WEI_PARb,reg_value);
	reg_value = (mdns_c_sta_wei25 << 16) |
		(mdns_c_sta_wei24);
	system_reg_write(((tisp_core_t *)mdns->core)->priv_data, MDNS_ADDR_UV_STA_WEI_PARc,reg_value);
	reg_value = (mdns_c_sta_wei27 << 16) |
		(mdns_c_sta_wei26);
	system_reg_write(((tisp_core_t *)mdns->core)->priv_data, MDNS_ADDR_UV_STA_WEI_PARd,reg_value);
	reg_value = (mdns_c_sta_wei29 << 16) |
		(mdns_c_sta_wei28);
	system_reg_write(((tisp_core_t *)mdns->core)->priv_data, MDNS_ADDR_UV_STA_WEI_PARe,reg_value);
	reg_value = (mdns_c_sta_wei31 << 16) |
		(mdns_c_sta_wei30);
	system_reg_write(((tisp_core_t *)mdns->core)->priv_data, MDNS_ADDR_UV_STA_WEI_PARf,reg_value);

	reg_value = (mdns_c_pbt_wei1 << 16) |
		(mdns_c_pbt_wei0);
	system_reg_write(((tisp_core_t *)mdns->core)->priv_data, MDNS_ADDR_UV_PBT_WEI_PAR0,reg_value);
	reg_value = (mdns_c_pbt_wei3  << 16) |
		(mdns_c_pbt_wei2);
	system_reg_write(((tisp_core_t *)mdns->core)->priv_data, MDNS_ADDR_UV_PBT_WEI_PAR1,reg_value);
	reg_value = (mdns_c_pbt_wei5  << 16) |
		(mdns_c_pbt_wei4);
	system_reg_write(((tisp_core_t *)mdns->core)->priv_data, MDNS_ADDR_UV_PBT_WEI_PAR2,reg_value);
	reg_value = (mdns_c_pbt_wei7  << 16) |
		(mdns_c_pbt_wei6);
	system_reg_write(((tisp_core_t *)mdns->core)->priv_data, MDNS_ADDR_UV_PBT_WEI_PAR3,reg_value);
	reg_value = (mdns_c_pbt_wei9  << 16) |
		(mdns_c_pbt_wei8);
	system_reg_write(((tisp_core_t *)mdns->core)->priv_data, MDNS_ADDR_UV_PBT_WEI_PAR4,reg_value);
	reg_value = (mdns_c_pbt_wei11 << 16) |
		(mdns_c_pbt_wei10);
	system_reg_write(((tisp_core_t *)mdns->core)->priv_data, MDNS_ADDR_UV_PBT_WEI_PAR5,reg_value);
	reg_value = (mdns_c_pbt_wei13 << 16) |
		(mdns_c_pbt_wei12);
	system_reg_write(((tisp_core_t *)mdns->core)->priv_data, MDNS_ADDR_UV_PBT_WEI_PAR6,reg_value);
	reg_value = (mdns_c_pbt_wei15 << 16) |
		(mdns_c_pbt_wei14);
	system_reg_write(((tisp_core_t *)mdns->core)->priv_data, MDNS_ADDR_UV_PBT_WEI_PAR7,reg_value);
	reg_value = (mdns_c_pbt_wei17 << 16) |
		(mdns_c_pbt_wei16);
	system_reg_write(((tisp_core_t *)mdns->core)->priv_data, MDNS_ADDR_UV_PBT_WEI_PAR8,reg_value);
	reg_value = (mdns_c_pbt_wei19 << 16) |
		(mdns_c_pbt_wei18);
	system_reg_write(((tisp_core_t *)mdns->core)->priv_data, MDNS_ADDR_UV_PBT_WEI_PAR9,reg_value);
	reg_value = (mdns_c_pbt_wei21 << 16) |
		(mdns_c_pbt_wei20);
	system_reg_write(((tisp_core_t *)mdns->core)->priv_data, MDNS_ADDR_UV_PBT_WEI_PARa,reg_value);
	reg_value = (mdns_c_pbt_wei23 << 16) |
		(mdns_c_pbt_wei22);
	system_reg_write(((tisp_core_t *)mdns->core)->priv_data, MDNS_ADDR_UV_PBT_WEI_PARb,reg_value);
	reg_value = (mdns_c_pbt_wei25 << 16) |
		(mdns_c_pbt_wei24);
	system_reg_write(((tisp_core_t *)mdns->core)->priv_data, MDNS_ADDR_UV_PBT_WEI_PARc,reg_value);
	reg_value = (mdns_c_pbt_wei27 << 16) |
		(mdns_c_pbt_wei26);
	system_reg_write(((tisp_core_t *)mdns->core)->priv_data, MDNS_ADDR_UV_PBT_WEI_PARd,reg_value);
	reg_value = (mdns_c_pbt_wei29 << 16) |
		(mdns_c_pbt_wei28);
	system_reg_write(((tisp_core_t *)mdns->core)->priv_data, MDNS_ADDR_UV_PBT_WEI_PARe,reg_value);
	reg_value = (mdns_c_pbt_wei31 << 16) |
		(mdns_c_pbt_wei30);
	system_reg_write(((tisp_core_t *)mdns->core)->priv_data, MDNS_ADDR_UV_PBT_WEI_PARf,reg_value);

	reg_value = (mdns_c_sad_wei1  << 16) |
		(mdns_c_sad_wei0);
	system_reg_write(((tisp_core_t *)mdns->core)->priv_data, MDNS_ADDR_UV_REF_WEI_SGM_Y_PAR0,reg_value);
	reg_value = (mdns_c_sad_wei3  << 16) |
		(mdns_c_sad_wei2);
	system_reg_write(((tisp_core_t *)mdns->core)->priv_data, MDNS_ADDR_UV_REF_WEI_SGM_Y_PAR1,reg_value);

	reg_value = (mdns->mdns_c_ref_wei_max_intp << 16) |
		(mdns->mdns_c_ref_wei_min_intp);
	system_reg_write(((tisp_core_t *)mdns->core)->priv_data, MDNS_ADDR_UV_REF_WEI_LMT,reg_value);

	mdns_c_sad_slope1 = (mdns_c_sad_wei0 - mdns_c_sad_wei1) * 8 / 10;
	mdns_c_sad_slope2 = (mdns_c_sad_wei1 - mdns_c_sad_wei2) * 8 / (390 - mdns->mdns_c_sad_thres_intp);
	mdns_c_sad_slope3 = (mdns_c_sad_wei2 - mdns_c_sad_wei3) * 8 / 100;
	mdns_c_sad_slope4 = (mdns_c_sad_wei3 - 0) * 8 / 110;

	reg_value = (mdns_c_sad_slope2 << 16) |
		(mdns_c_sad_slope1);
	system_reg_write(((tisp_core_t *)mdns->core)->priv_data, MDNS_ADDR_UV_REF_WEI_SGM_S_PAR0,reg_value);

	reg_value = (mdns_c_sad_slope4 << 16) |
		(mdns_c_sad_slope3);
	system_reg_write(((tisp_core_t *)mdns->core)->priv_data, MDNS_ADDR_UV_REF_WEI_SGM_S_PAR1,reg_value);

	return 0;
}

int32_t tisp_mdns_c_2d_param_cfg(tisp_mdns_t *mdns)
{
	uint32_t reg_value;

	system_reg_write(((tisp_core_t *)mdns->core)->priv_data, MDNS_ADDR_UV_FS_MV_JUDGE_THRES, mdns->mdns_c_smj_thres_intp);

	reg_value = (mdns->mdns_c_ref_blur_intp << 16) |
		(mdns->mdns_c_cur_blur_intp);
	system_reg_write(((tisp_core_t *)mdns->core)->priv_data, MDNS_ADDR_UV_BLUR_WEI,reg_value);
	system_reg_write(((tisp_core_t *)mdns->core)->priv_data, MDNS_ADDR_UV_FC_TYPE,0);

	reg_value = (mdns->mdns_c_sfla_boh_s_intp << 24) |
		(mdns->mdns_c_sfla_cut_t_intp << 16) |
		(mdns->mdns_c_sfla_boh_t_intp << 8) |
		(128                 );
	system_reg_write(((tisp_core_t *)mdns->core)->priv_data, MDNS_ADDR_UV_FC_S_PAR0,reg_value);
	system_reg_write(((tisp_core_t *)mdns->core)->priv_data, MDNS_ADDR_UV_FC_S_PAR1,mdns->mdns_c_sfla_cut_s_intp);

	reg_value = (mdns->mdns_c_mfla_boh_s_intp << 24) |
		(mdns->mdns_c_mfla_cut_t_intp << 16) |
		(mdns->mdns_c_mfla_boh_t_intp << 8) |
		(128                 );
	system_reg_write(((tisp_core_t *)mdns->core)->priv_data, MDNS_ADDR_UV_FC_M_PAR0,reg_value);
	system_reg_write(((tisp_core_t *)mdns->core)->priv_data, MDNS_ADDR_UV_FC_M_PAR1,mdns->mdns_c_mfla_cut_s_intp);

	return 0;
}

int32_t tisp_mdns_c_adj_param_cfg(tisp_mdns_t *mdns)
{
	uint32_t reg_value;

	uint32_t mdns_c_adj_tchrm_0  = mdns->mdns_c_adj_tcrm_s_intp*mdns->mdns_c_adj_tcrm_n_array[0];
	uint32_t mdns_c_adj_tchrm_1  = mdns->mdns_c_adj_tcrm_s_intp*mdns->mdns_c_adj_tcrm_n_array[1];
	uint32_t mdns_c_adj_tchrm_2  = mdns->mdns_c_adj_tcrm_s_intp*mdns->mdns_c_adj_tcrm_n_array[2];
	uint32_t mdns_c_adj_tchrm_3  = mdns->mdns_c_adj_tcrm_s_intp*mdns->mdns_c_adj_tcrm_n_array[3];
	uint32_t mdns_c_adj_tchrm_4  = mdns->mdns_c_adj_tcrm_s_intp*mdns->mdns_c_adj_tcrm_n_array[4];
	uint32_t mdns_c_adj_tchrm_5  = mdns->mdns_c_adj_tcrm_s_intp*mdns->mdns_c_adj_tcrm_n_array[5];
	uint32_t mdns_c_adj_tchrm_6  = mdns->mdns_c_adj_tcrm_s_intp*mdns->mdns_c_adj_tcrm_n_array[6];
	uint32_t mdns_c_adj_tchrm_7  = mdns->mdns_c_adj_tcrm_s_intp*mdns->mdns_c_adj_tcrm_n_array[7];

	uint32_t mdns_c_adj_wchrm_0 =  mdns->mdns_c_adj_wcrm_s_intp*mdns->mdns_c_adj_wcrm_n_array[0];
	uint32_t mdns_c_adj_wchrm_1 =  mdns->mdns_c_adj_wcrm_s_intp*mdns->mdns_c_adj_wcrm_n_array[1];
	uint32_t mdns_c_adj_wchrm_2 =  mdns->mdns_c_adj_wcrm_s_intp*mdns->mdns_c_adj_wcrm_n_array[2];
	uint32_t mdns_c_adj_wchrm_3 =  mdns->mdns_c_adj_wcrm_s_intp*mdns->mdns_c_adj_wcrm_n_array[3];
	uint32_t mdns_c_adj_wchrm_4 =  mdns->mdns_c_adj_wcrm_s_intp*mdns->mdns_c_adj_wcrm_n_array[4];
	uint32_t mdns_c_adj_wchrm_5 =  mdns->mdns_c_adj_wcrm_s_intp*mdns->mdns_c_adj_wcrm_n_array[5];
	uint32_t mdns_c_adj_wchrm_6 =  mdns->mdns_c_adj_wcrm_s_intp*mdns->mdns_c_adj_wcrm_n_array[6];
	uint32_t mdns_c_adj_wchrm_7 =  mdns->mdns_c_adj_wcrm_s_intp*mdns->mdns_c_adj_wcrm_n_array[7];

	reg_value = (mdns->mdns_c_adj_cnr_array[3] << 24) |
		(mdns->mdns_c_adj_cnr_array[2] << 16) |
		(mdns->mdns_c_adj_cnr_array[1] << 8) |
		(mdns->mdns_c_adj_cnr_array[0]);
	system_reg_write(((tisp_core_t *)mdns->core)->priv_data, MDNS_ADDR_UV_TCA_PAR,reg_value);

	reg_value = (mdns->mdns_c_adj_sta_array[2] << 24) |
		(mdns->mdns_c_adj_sta_array[1] << 16) |
		(mdns->mdns_c_adj_sta_array[0]);
	system_reg_write(((tisp_core_t *)mdns->core)->priv_data, MDNS_ADDR_UV_WSA_S_PAR,reg_value);

	reg_value = (mdns->mdns_c_adj_sta_array[5] << 24) |
		(mdns->mdns_c_adj_sta_array[4] << 16) |
		(mdns->mdns_c_adj_sta_array[3]);
	system_reg_write(((tisp_core_t *)mdns->core)->priv_data, MDNS_ADDR_UV_WSA_M_PAR,reg_value);

	system_reg_write(((tisp_core_t *)mdns->core)->priv_data, MDNS_ADDR_UV_TMA_T_PAR0,0x80604020);
	system_reg_write(((tisp_core_t *)mdns->core)->priv_data, MDNS_ADDR_UV_TMA_T_PAR1,0xd0c0a0);

	reg_value = (mdns_c_adj_tchrm_3 << 24) |
		(mdns_c_adj_tchrm_2 << 16) |
		(mdns_c_adj_tchrm_1 << 8) |
		(mdns_c_adj_tchrm_0);
	system_reg_write(((tisp_core_t *)mdns->core)->priv_data, MDNS_ADDR_UV_TMA_V_PAR0,reg_value);

	reg_value = (mdns_c_adj_tchrm_7 << 24) |
		(mdns_c_adj_tchrm_6 << 16) |
		(mdns_c_adj_tchrm_5 << 8) |
		(mdns_c_adj_tchrm_4);
	system_reg_write(((tisp_core_t *)mdns->core)->priv_data, MDNS_ADDR_UV_TMA_V_PAR1,reg_value);

	reg_value = (mdns_c_adj_wchrm_3 << 24) |
		(mdns_c_adj_wchrm_2 << 16) |
		(mdns_c_adj_wchrm_1 << 8) |
		(mdns_c_adj_wchrm_0);
	system_reg_write(((tisp_core_t *)mdns->core)->priv_data, MDNS_ADDR_UV_TMA_W_PAR0,reg_value);

	reg_value = (mdns_c_adj_wchrm_7 << 24) |
		(mdns_c_adj_wchrm_6 << 16) |
		(mdns_c_adj_wchrm_5 << 8) |
		(mdns_c_adj_wchrm_4);
	system_reg_write(((tisp_core_t *)mdns->core)->priv_data, MDNS_ADDR_UV_TMA_W_PAR1,reg_value);

	return 0;
}

int32_t tisp_mdns_intp(tisp_mdns_t *mdns, int32_t gain_value)
{
	int32_t x_int, x_fra;

	x_int = gain_value >> 16;
	x_fra = gain_value & 0xffff;

	mdns->mdns_sta_size_intp       = tisp_simple_intp(x_int,x_fra,mdns->mdns_sta_size_array);
	mdns->mdns_pbt_size_intp       = tisp_simple_intp(x_int,x_fra,mdns->mdns_pbt_size_array);
	mdns->mdns_y_sad_thres_intp    = tisp_simple_intp(x_int,x_fra,mdns->mdns_y_sad_thres_array);
	mdns->mdns_y_sta_thres_intp    = tisp_simple_intp(x_int,x_fra,mdns->mdns_y_sta_thres_array);
	mdns->mdns_y_pbt_thres_intp    = tisp_simple_intp(x_int,x_fra,mdns->mdns_y_pbt_thres_array);
	mdns->mdns_y_sad_stren_intp    = tisp_simple_intp(x_int,x_fra,mdns->mdns_y_sad_stren_array);
	mdns->mdns_y_sta_stren_intp    = tisp_simple_intp(x_int,x_fra,mdns->mdns_y_sta_stren_array);
	mdns->mdns_y_pbt_stren_intp    = tisp_simple_intp(x_int,x_fra,mdns->mdns_y_pbt_stren_array);
	mdns->mdns_y_sad_win_opt_intp  = tisp_simple_intp(x_int,x_fra,mdns->mdns_y_sad_win_opt_array);
	mdns->mdns_y_sta_win_opt_intp  = tisp_simple_intp(x_int,x_fra,mdns->mdns_y_sta_win_opt_array);
	mdns->mdns_y_pbt_win_opt_intp  = tisp_simple_intp(x_int,x_fra,mdns->mdns_y_pbt_win_opt_array);
	mdns->mdns_y_sad_win_wei_intp  = tisp_simple_intp(x_int,x_fra,mdns->mdns_y_sad_win_wei_array);
	mdns->mdns_y_sta_win_wei_intp  = tisp_simple_intp(x_int,x_fra,mdns->mdns_y_sta_win_wei_array);
	mdns->mdns_y_pbt_win_wei_intp  = tisp_simple_intp(x_int,x_fra,mdns->mdns_y_pbt_win_wei_array);
	mdns->mdns_y_sta_mv_num_intp   = tisp_simple_intp(x_int,x_fra,mdns->mdns_y_sta_mv_num_array);
	mdns->mdns_y_pbt_mv_num_intp   = tisp_simple_intp(x_int,x_fra,mdns->mdns_y_pbt_mv_num_array);
	mdns->mdns_y_sta_mx_num_intp   = tisp_simple_intp(x_int,x_fra,mdns->mdns_y_sta_mx_num_array);
	mdns->mdns_y_pbt_mx_num_intp   = tisp_simple_intp(x_int,x_fra,mdns->mdns_y_pbt_mx_num_array);
	mdns->mdns_y_ref_wei_min_intp  = tisp_simple_intp(x_int,x_fra,mdns->mdns_y_ref_wei_min_array);
	mdns->mdns_y_ref_wei_max_intp  = tisp_simple_intp(x_int,x_fra,mdns->mdns_y_ref_wei_max_array);
	mdns->mdns_y_edge_win_intp     = tisp_simple_intp(x_int,x_fra,mdns->mdns_y_edge_win_array);
	mdns->mdns_y_bi_thres_intp     = tisp_simple_intp(x_int,x_fra,mdns->mdns_y_bi_thres_array);
	mdns->mdns_y_smj_thres_intp    = tisp_simple_intp(x_int,x_fra,mdns->mdns_y_smj_thres_array);
	mdns->mdns_y_shp_swei_intp     = tisp_simple_intp(x_int,x_fra,mdns->mdns_y_shp_swei_array);
	mdns->mdns_y_shp_mwei_intp     = tisp_simple_intp(x_int,x_fra,mdns->mdns_y_shp_mwei_array);
	mdns->mdns_y_cs_bhold_intp     = tisp_simple_intp(x_int,x_fra,mdns->mdns_y_cs_bhold_array);
	mdns->mdns_y_cm_bhold_intp     = tisp_simple_intp(x_int,x_fra,mdns->mdns_y_cm_bhold_array);
	mdns->mdns_y_rs_bhold_intp     = tisp_simple_intp(x_int,x_fra,mdns->mdns_y_rs_bhold_array);
	mdns->mdns_y_rm_bhold_intp     = tisp_simple_intp(x_int,x_fra,mdns->mdns_y_rm_bhold_array);
	mdns->mdns_y_cs_bwin_intp      = tisp_simple_intp(x_int,x_fra,mdns->mdns_y_cs_bwin_array);
	mdns->mdns_y_cm_bwin_intp      = tisp_simple_intp(x_int,x_fra,mdns->mdns_y_cm_bwin_array);
	mdns->mdns_y_rs_bwin_intp      = tisp_simple_intp(x_int,x_fra,mdns->mdns_y_rs_bwin_array);
	mdns->mdns_y_rm_bwin_intp      = tisp_simple_intp(x_int,x_fra,mdns->mdns_y_rm_bwin_array);
	mdns->mdns_y_cs_iwin_intp      = tisp_simple_intp(x_int,x_fra,mdns->mdns_y_cs_iwin_array);
	mdns->mdns_y_cm_iwin_intp      = tisp_simple_intp(x_int,x_fra,mdns->mdns_y_cm_iwin_array);
	mdns->mdns_y_rs_iwin_intp      = tisp_simple_intp(x_int,x_fra,mdns->mdns_y_rs_iwin_array);
	mdns->mdns_y_rm_iwin_intp      = tisp_simple_intp(x_int,x_fra,mdns->mdns_y_rm_iwin_array);
	mdns->mdns_y_cs_ewin_intp      = tisp_simple_intp(x_int,x_fra,mdns->mdns_y_cs_ewin_array);
	mdns->mdns_y_cm_ewin_intp      = tisp_simple_intp(x_int,x_fra,mdns->mdns_y_cm_ewin_array);
	mdns->mdns_y_rs_ewin_intp      = tisp_simple_intp(x_int,x_fra,mdns->mdns_y_rs_ewin_array);
	mdns->mdns_y_rm_ewin_intp      = tisp_simple_intp(x_int,x_fra,mdns->mdns_y_rm_ewin_array);
	mdns->mdns_y_cs_sego_intp      = tisp_simple_intp(x_int,x_fra,mdns->mdns_y_cs_sego_array);
	mdns->mdns_y_cm_sego_intp      = tisp_simple_intp(x_int,x_fra,mdns->mdns_y_cm_sego_array);
	mdns->mdns_y_rs_sego_intp      = tisp_simple_intp(x_int,x_fra,mdns->mdns_y_rs_sego_array);
	mdns->mdns_y_rm_sego_intp      = tisp_simple_intp(x_int,x_fra,mdns->mdns_y_rm_sego_array);
	mdns->mdns_y_cs_stren_intp     = tisp_simple_intp(x_int,x_fra,mdns->mdns_y_cs_stren_array);
	mdns->mdns_y_cm_stren_intp     = tisp_simple_intp(x_int,x_fra,mdns->mdns_y_cm_stren_array);
	mdns->mdns_y_rs_stren_intp     = tisp_simple_intp(x_int,x_fra,mdns->mdns_y_rs_stren_array);
	mdns->mdns_y_rm_stren_intp     = tisp_simple_intp(x_int,x_fra,mdns->mdns_y_rm_stren_array);
	mdns->mdns_y_shp_c_0_npv_intp  = tisp_simple_intp(x_int,x_fra,mdns->mdns_y_shp_c_0_npv_array);
	mdns->mdns_y_shp_c_1_npv_intp  = tisp_simple_intp(x_int,x_fra,mdns->mdns_y_shp_c_1_npv_array);
	mdns->mdns_y_shp_c_2_npv_intp  = tisp_simple_intp(x_int,x_fra,mdns->mdns_y_shp_c_2_npv_array);
	mdns->mdns_y_shp_c_3_npv_intp  = tisp_simple_intp(x_int,x_fra,mdns->mdns_y_shp_c_3_npv_array);
	mdns->mdns_y_shp_c_4_npv_intp  = tisp_simple_intp(x_int,x_fra,mdns->mdns_y_shp_c_4_npv_array);
	mdns->mdns_y_shp_c_5_npv_intp  = tisp_simple_intp(x_int,x_fra,mdns->mdns_y_shp_c_5_npv_array);
	mdns->mdns_y_shp_c_6_npv_intp  = tisp_simple_intp(x_int,x_fra,mdns->mdns_y_shp_c_6_npv_array);
	mdns->mdns_y_shp_c_7_npv_intp  = tisp_simple_intp(x_int,x_fra,mdns->mdns_y_shp_c_7_npv_array);
	mdns->mdns_y_shp_c_8_npv_intp  = tisp_simple_intp(x_int,x_fra,mdns->mdns_y_shp_c_8_npv_array);
	mdns->mdns_y_shp_c_9_npv_intp  = tisp_simple_intp(x_int,x_fra,mdns->mdns_y_shp_c_9_npv_array);
	mdns->mdns_y_shp_c_a_npv_intp  = tisp_simple_intp(x_int,x_fra,mdns->mdns_y_shp_c_a_npv_array);
	mdns->mdns_y_shp_c_b_npv_intp  = tisp_simple_intp(x_int,x_fra,mdns->mdns_y_shp_c_b_npv_array);
	mdns->mdns_y_shp_c_c_npv_intp  = tisp_simple_intp(x_int,x_fra,mdns->mdns_y_shp_c_c_npv_array);
	mdns->mdns_y_shp_c_d_npv_intp  = tisp_simple_intp(x_int,x_fra,mdns->mdns_y_shp_c_d_npv_array);
	mdns->mdns_y_shp_c_e_npv_intp  = tisp_simple_intp(x_int,x_fra,mdns->mdns_y_shp_c_e_npv_array);
	mdns->mdns_y_shp_c_f_npv_intp  = tisp_simple_intp(x_int,x_fra,mdns->mdns_y_shp_c_f_npv_array);
	mdns->mdns_y_shp_r_0_npv_intp  = tisp_simple_intp(x_int,x_fra,mdns->mdns_y_shp_r_0_npv_array);
	mdns->mdns_y_shp_r_1_npv_intp  = tisp_simple_intp(x_int,x_fra,mdns->mdns_y_shp_r_1_npv_array);
	mdns->mdns_y_shp_r_2_npv_intp  = tisp_simple_intp(x_int,x_fra,mdns->mdns_y_shp_r_2_npv_array);
	mdns->mdns_y_shp_r_3_npv_intp  = tisp_simple_intp(x_int,x_fra,mdns->mdns_y_shp_r_3_npv_array);
	mdns->mdns_y_shp_r_4_npv_intp  = tisp_simple_intp(x_int,x_fra,mdns->mdns_y_shp_r_4_npv_array);
	mdns->mdns_y_shp_r_5_npv_intp  = tisp_simple_intp(x_int,x_fra,mdns->mdns_y_shp_r_5_npv_array);
	mdns->mdns_y_shp_r_6_npv_intp  = tisp_simple_intp(x_int,x_fra,mdns->mdns_y_shp_r_6_npv_array);
	mdns->mdns_y_shp_r_7_npv_intp  = tisp_simple_intp(x_int,x_fra,mdns->mdns_y_shp_r_7_npv_array);
	mdns->mdns_y_shp_r_8_npv_intp  = tisp_simple_intp(x_int,x_fra,mdns->mdns_y_shp_r_8_npv_array);
	mdns->mdns_y_shp_r_9_npv_intp  = tisp_simple_intp(x_int,x_fra,mdns->mdns_y_shp_r_9_npv_array);
	mdns->mdns_y_shp_r_a_npv_intp  = tisp_simple_intp(x_int,x_fra,mdns->mdns_y_shp_r_a_npv_array);
	mdns->mdns_y_shp_r_b_npv_intp  = tisp_simple_intp(x_int,x_fra,mdns->mdns_y_shp_r_b_npv_array);
	mdns->mdns_y_shp_r_c_npv_intp  = tisp_simple_intp(x_int,x_fra,mdns->mdns_y_shp_r_c_npv_array);
	mdns->mdns_y_shp_r_d_npv_intp  = tisp_simple_intp(x_int,x_fra,mdns->mdns_y_shp_r_d_npv_array);
	mdns->mdns_y_shp_r_e_npv_intp  = tisp_simple_intp(x_int,x_fra,mdns->mdns_y_shp_r_e_npv_array);
	mdns->mdns_y_shp_r_f_npv_intp  = tisp_simple_intp(x_int,x_fra,mdns->mdns_y_shp_r_f_npv_array);
	mdns->mdns_y_adj_tedg_s_intp   = tisp_simple_intp(x_int,x_fra,mdns->mdns_y_adj_tedg_s_array);
	mdns->mdns_y_adj_wedg_s_intp   = tisp_simple_intp(x_int,x_fra,mdns->mdns_y_adj_wedg_s_array);
	mdns->mdns_y_adj_tlum_s_intp   = tisp_simple_intp(x_int,x_fra,mdns->mdns_y_adj_tlum_s_array);
	mdns->mdns_y_adj_wlum_s_intp   = tisp_simple_intp(x_int,x_fra,mdns->mdns_y_adj_wlum_s_array);
	mdns->mdns_c_sad_thres_intp    = tisp_simple_intp(x_int,x_fra,mdns->mdns_c_sad_thres_array);
	mdns->mdns_c_sad_win_opt_intp  = tisp_simple_intp(x_int,x_fra,mdns->mdns_c_sad_win_opt_array);
	mdns->mdns_c_sad_stren_intp    = tisp_simple_intp(x_int,x_fra,mdns->mdns_c_sad_stren_array);
	mdns->mdns_c_sta_stren_intp    = tisp_simple_intp(x_int,x_fra,mdns->mdns_c_sta_stren_array);
	mdns->mdns_c_pbt_stren_intp    = tisp_simple_intp(x_int,x_fra,mdns->mdns_c_pbt_stren_array);
	mdns->mdns_c_ref_wei_min_intp  = tisp_simple_intp(x_int,x_fra,mdns->mdns_c_ref_wei_min_array);
	mdns->mdns_c_ref_wei_max_intp  = tisp_simple_intp(x_int,x_fra,mdns->mdns_c_ref_wei_max_array);
	mdns->mdns_c_cur_blur_intp     = tisp_simple_intp(x_int,x_fra,mdns->mdns_c_cur_blur_array);
	mdns->mdns_c_ref_blur_intp     = tisp_simple_intp(x_int,x_fra,mdns->mdns_c_ref_blur_array);
	mdns->mdns_c_smj_thres_intp    = tisp_simple_intp(x_int,x_fra,mdns->mdns_c_smj_thres_array);
	mdns->mdns_c_sfla_boh_t_intp   = tisp_simple_intp(x_int,x_fra,mdns->mdns_c_sfla_boh_t_array);
	mdns->mdns_c_sfla_cut_t_intp   = tisp_simple_intp(x_int,x_fra,mdns->mdns_c_sfla_cut_t_array);
	mdns->mdns_c_sfla_boh_s_intp   = tisp_simple_intp(x_int,x_fra,mdns->mdns_c_sfla_boh_s_array);
	mdns->mdns_c_sfla_cut_s_intp   = tisp_simple_intp(x_int,x_fra,mdns->mdns_c_sfla_cut_s_array);
	mdns->mdns_c_mfla_boh_t_intp   = tisp_simple_intp(x_int,x_fra,mdns->mdns_c_mfla_boh_t_array);
	mdns->mdns_c_mfla_cut_t_intp   = tisp_simple_intp(x_int,x_fra,mdns->mdns_c_mfla_cut_t_array);
	mdns->mdns_c_mfla_boh_s_intp   = tisp_simple_intp(x_int,x_fra,mdns->mdns_c_mfla_boh_s_array);
	mdns->mdns_c_mfla_cut_s_intp   = tisp_simple_intp(x_int,x_fra,mdns->mdns_c_mfla_cut_s_array);
	mdns->mdns_c_adj_tcrm_s_intp   = tisp_simple_intp(x_int,x_fra,mdns->mdns_c_adj_tcrm_s_array);
	mdns->mdns_c_adj_wcrm_s_intp   = tisp_simple_intp(x_int,x_fra,mdns->mdns_c_adj_wcrm_s_array);

	return 0;
}

int32_t tisp_mdns_all_reg_refresh(tisp_mdns_t *mdns, int gain_value)
{
	tisp_mdns_intp(mdns, gain_value);

	tisp_mdns_y_3d_param_cfg(mdns);
	tisp_mdns_y_2d_param_cfg(mdns);
	tisp_mdns_y_adj_param_cfg(mdns);
	tisp_mdns_c_3d_param_cfg(mdns);
	tisp_mdns_c_2d_param_cfg(mdns);
	tisp_mdns_c_adj_param_cfg(mdns);
	tisp_mdns_sta_func_cfg(mdns, mdns->vin_width, mdns->vin_height);
	tisp_mdns_top_func_cfg(mdns, 1);
	system_reg_write(((tisp_core_t *)mdns->core)->priv_data, MDNS_ADDR_REG_CTRL, 0x111);

	return 0;
}

int32_t tisp_mdns_intp_reg_refresh(tisp_mdns_t *mdns, int gain_value)
{
	tisp_mdns_intp(mdns, gain_value);
	tisp_mdns_y_3d_param_cfg(mdns);
	tisp_mdns_y_2d_param_cfg(mdns);
	tisp_mdns_y_adj_param_cfg(mdns);
	tisp_mdns_c_3d_param_cfg(mdns);
	tisp_mdns_c_2d_param_cfg(mdns);
	tisp_mdns_c_adj_param_cfg(mdns);
	tisp_mdns_sta_func_cfg(mdns, mdns->vin_width, mdns->vin_height);
	tisp_mdns_top_func_cfg(mdns, 1);
	system_reg_write(((tisp_core_t *)mdns->core)->priv_data, MDNS_ADDR_REG_CTRL, 0x111);

	return 0;
}

static uint32_t gain_old = 0xffffffff;
static uint32_t gain_thres = 0x100;

uint32_t tisp_mdns_par_refresh(tisp_mdns_t *mdns, uint32_t gain_value, uint32_t gain_thres, uint32_t width,uint32_t height)
{
	uint32_t gain_diff;

	if (gain_old == 0xffffffff) {
		gain_old = gain_value;
		tisp_mdns_all_reg_refresh(mdns, gain_value);
		system_reg_write(((tisp_core_t *)mdns->core)->priv_data, MDNS_ADDR_T_FRM_NUM, 0x101);
	} else {
		if (gain_value >= gain_old)
			gain_diff = gain_value-gain_old;
		else
			gain_diff = gain_old-gain_value;

		if (gain_diff >= gain_thres) {
			gain_old = gain_value;
			tisp_mdns_intp_reg_refresh(mdns, gain_value);
		}
	}

	return 0;
}

int32_t tiziano_mdns_params_refresh(tisp_mdns_t *mdns)
{
	memcpy(mdns->mdns_top_func_array, tparams.params_data.TISP_PARAM_MDNS_TOP_FUNC_ARRAY,
	       sizeof(tparams.params_data.TISP_PARAM_MDNS_TOP_FUNC_ARRAY));
	memcpy(mdns->mdns_sta_size_array, tparams.params_data.TISP_PARAM_MDSN_STA_SIZE_ARRAY,
	       sizeof(tparams.params_data.TISP_PARAM_MDSN_STA_SIZE_ARRAY));
	memcpy(mdns->mdns_pbt_size_array, tparams.params_data.TISP_PARAM_MDSN_PBT_SIZE_ARRAY,
	       sizeof(tparams.params_data.TISP_PARAM_MDSN_PBT_SIZE_ARRAY));
	memcpy(mdns->mdns_pbt_ponit_array, tparams.params_data.TISP_PARAM_MDSN_PBT_PONIT_ARRAY,
	       sizeof(tparams.params_data.TISP_PARAM_MDSN_PBT_PONIT_ARRAY));
	memcpy(mdns->mdns_y_sad_thres_array, tparams.params_data.TISP_PARAM_MDSN_Y_SAD_THRES_ARRAY,
	       sizeof(tparams.params_data.TISP_PARAM_MDSN_Y_SAD_THRES_ARRAY));
	memcpy(mdns->mdns_y_sta_thres_array, tparams.params_data.TISP_PARAM_MDSN_Y_STA_THRES_ARRAY,
	       sizeof(tparams.params_data.TISP_PARAM_MDSN_Y_STA_THRES_ARRAY));
	memcpy(mdns->mdns_y_pbt_thres_array, tparams.params_data.TISP_PARAM_MDSN_Y_PBT_THRES_ARRAY,
	       sizeof(tparams.params_data.TISP_PARAM_MDSN_Y_PBT_THRES_ARRAY));
	memcpy(mdns->mdns_y_sad_stren_array, tparams.params_data.TISP_PARAM_MDSN_Y_SAD_STREN_ARRAY,
	       sizeof(tparams.params_data.TISP_PARAM_MDSN_Y_SAD_STREN_ARRAY));
	memcpy(mdns->mdns_y_sta_stren_array, tparams.params_data.TISP_PARAM_MDSN_Y_STA_STREN_ARRAY,
	       sizeof(tparams.params_data.TISP_PARAM_MDSN_Y_STA_STREN_ARRAY));
	memcpy(mdns->mdns_y_pbt_stren_array, tparams.params_data.TISP_PARAM_MDSN_Y_PBT_STREN_ARRAY,
	       sizeof(tparams.params_data.TISP_PARAM_MDSN_Y_PBT_STREN_ARRAY));
	memcpy(mdns->mdns_y_sad_win_opt_array, tparams.params_data.TISP_PARAM_MDSN_Y_SAD_WIN_OPT_ARRAY,
	       sizeof(tparams.params_data.TISP_PARAM_MDSN_Y_SAD_WIN_OPT_ARRAY));
	memcpy(mdns->mdns_y_sta_win_opt_array, tparams.params_data.TISP_PARAM_MDSN_Y_STA_WIN_OPT_ARRAY,
	       sizeof(tparams.params_data.TISP_PARAM_MDSN_Y_STA_WIN_OPT_ARRAY));
	memcpy(mdns->mdns_y_pbt_win_opt_array, tparams.params_data.TISP_PARAM_MDSN_Y_PBT_WIN_OPT_ARRAY,
	       sizeof(tparams.params_data.TISP_PARAM_MDSN_Y_PBT_WIN_OPT_ARRAY));
	memcpy(mdns->mdns_y_sad_win_wei_array, tparams.params_data.TISP_PARAM_MDSN_Y_SAD_WIN_WEI_ARRAY,
	       sizeof(tparams.params_data.TISP_PARAM_MDSN_Y_SAD_WIN_WEI_ARRAY));
	memcpy(mdns->mdns_y_sta_win_wei_array, tparams.params_data.TISP_PARAM_MDSN_Y_STA_WIN_WEI_ARRAY,
	       sizeof(tparams.params_data.TISP_PARAM_MDSN_Y_STA_WIN_WEI_ARRAY));
	memcpy(mdns->mdns_y_pbt_win_wei_array, tparams.params_data.TISP_PARAM_MDSN_Y_PBT_WIn_WEI_ARRAY,
	       sizeof(tparams.params_data.TISP_PARAM_MDSN_Y_PBT_WIn_WEI_ARRAY));
	memcpy(mdns->mdns_y_sta_mv_num_array, tparams.params_data.TISP_PARAM_MDSN_Y_STA_MV_NUM_ARRAY,
	       sizeof(tparams.params_data.TISP_PARAM_MDSN_Y_STA_MV_NUM_ARRAY));
	memcpy(mdns->mdns_y_pbt_mv_num_array, tparams.params_data.TISP_PARAM_MDSN_Y_PBT_MV_NUM_ARRAY,
	       sizeof(tparams.params_data.TISP_PARAM_MDSN_Y_PBT_MV_NUM_ARRAY));
	memcpy(mdns->mdns_y_sta_mx_num_array, tparams.params_data.TISP_PARAM_MDSN_Y_STA_MX_NUM_ARRAY,
	       sizeof(tparams.params_data.TISP_PARAM_MDSN_Y_STA_MX_NUM_ARRAY));
	memcpy(mdns->mdns_y_pbt_mx_num_array, tparams.params_data.TISP_PARAM_MDSN_Y_PBT_MX_NUM_ARRAY,
	       sizeof(tparams.params_data.TISP_PARAM_MDSN_Y_PBT_MX_NUM_ARRAY));
	memcpy(mdns->mdns_y_sad_npv_array, tparams.params_data.TISP_PARAM_MDSN_Y_SAD_NPV_ARRAY,
	       sizeof(tparams.params_data.TISP_PARAM_MDSN_Y_SAD_NPV_ARRAY));
	memcpy(mdns->mdns_y_sta_npv_array, tparams.params_data.TISP_PARAM_MDSN_Y_STA_NPV_ARRAY,
	       sizeof(tparams.params_data.TISP_PARAM_MDSN_Y_STA_NPV_ARRAY));
	memcpy(mdns->mdns_y_pbt_npv_array, tparams.params_data.TISP_PARAM_MDSN_Y_PBT_NPV_ARRAY,
	       sizeof(tparams.params_data.TISP_PARAM_MDSN_Y_PBT_NPV_ARRAY));
	memcpy(mdns->mdns_y_ref_wei_min_array, tparams.params_data.TISP_PARAM_MDSN_Y_REF_WEI_MIN_ARRAY,
	       sizeof(tparams.params_data.TISP_PARAM_MDSN_Y_REF_WEI_MIN_ARRAY));
	memcpy(mdns->mdns_y_ref_wei_max_array, tparams.params_data.TISP_PARAM_MDSN_Y_REF_WEI_MAX_ARRAY,
	       sizeof(tparams.params_data.TISP_PARAM_MDSN_Y_REF_WEI_MAX_ARRAY));
	memcpy(mdns->mdns_y_edge_type_array, tparams.params_data.TISP_PARAM_MDSN_Y_EDGE_TYPE_ARRAY,
	       sizeof(tparams.params_data.TISP_PARAM_MDSN_Y_EDGE_TYPE_ARRAY));
	memcpy(mdns->mdns_y_edge_win_array, tparams.params_data.TISP_PARAM_MDSN_Y_EDGE_WIN_ARRAY,
	       sizeof(tparams.params_data.TISP_PARAM_MDSN_Y_EDGE_WIN_ARRAY));
	memcpy(mdns->mdns_y_bi_thres_array, tparams.params_data.TISP_PARAM_MDSN_Y_BI_THRES_ARRAY,
	       sizeof(tparams.params_data.TISP_PARAM_MDSN_Y_BI_THRES_ARRAY));
	memcpy(mdns->mdns_y_smj_thres_array, tparams.params_data.TISP_PARAM_MDSN_Y_SMJ_THRES_ARRAY,
	       sizeof(tparams.params_data.TISP_PARAM_MDSN_Y_SMJ_THRES_ARRAY));
	memcpy(mdns->mdns_y_shp_swei_array, tparams.params_data.TISP_PARAM_MDSN_Y_SHP_SWEI_ARRAY,
	       sizeof(tparams.params_data.TISP_PARAM_MDSN_Y_SHP_SWEI_ARRAY));
	memcpy(mdns->mdns_y_shp_mwei_array, tparams.params_data.TISP_PARAM_MDSN_Y_SHP_MWEI_ARRAY,
	       sizeof(tparams.params_data.TISP_PARAM_MDSN_Y_SHP_MWEI_ARRAY));
	memcpy(mdns->mdns_y_cs_bhold_array, tparams.params_data.TISP_PARAM_MDSN_Y_CS_BHOLD_ARRAY,
	       sizeof(tparams.params_data.TISP_PARAM_MDSN_Y_CS_BHOLD_ARRAY));
	memcpy(mdns->mdns_y_cm_bhold_array, tparams.params_data.TISP_PARAM_MDSN_Y_CM_BHOLD_ARRAY,
	       sizeof(tparams.params_data.TISP_PARAM_MDSN_Y_CM_BHOLD_ARRAY));
	memcpy(mdns->mdns_y_rs_bhold_array, tparams.params_data.TISP_PARAM_MDSN_Y_RS_BHOLD_ARRAY,
	       sizeof(tparams.params_data.TISP_PARAM_MDSN_Y_RS_BHOLD_ARRAY));
	memcpy(mdns->mdns_y_rm_bhold_array, tparams.params_data.TISP_PARAM_MDSN_Y_RM_BHOLD_ARRAY,
	       sizeof(tparams.params_data.TISP_PARAM_MDSN_Y_RM_BHOLD_ARRAY));
	memcpy(mdns->mdns_y_cs_bwin_array, tparams.params_data.TISP_PARAM_MDSN_Y_CS_BWIN_ARRAY,
	       sizeof(tparams.params_data.TISP_PARAM_MDSN_Y_CS_BWIN_ARRAY));
	memcpy(mdns->mdns_y_cm_bwin_array, tparams.params_data.TISP_PARAM_MDSN_Y_CM_BWIN_ARRAY,
	       sizeof(tparams.params_data.TISP_PARAM_MDSN_Y_CM_BWIN_ARRAY));
	memcpy(mdns->mdns_y_rs_bwin_array, tparams.params_data.TISP_PARAM_MDSN_Y_RS_BWIN_ARRAY,
	       sizeof(tparams.params_data.TISP_PARAM_MDSN_Y_RS_BWIN_ARRAY));
	memcpy(mdns->mdns_y_rm_bwin_array, tparams.params_data.TISP_PARAM_MDSN_Y_RM_BWIN_ARRAY,
	       sizeof(tparams.params_data.TISP_PARAM_MDSN_Y_RM_BWIN_ARRAY));
	memcpy(mdns->mdns_y_cs_iwin_array, tparams.params_data.TISP_PARAM_MDSN_Y_CS_IWIN_ARRAY,
	       sizeof(tparams.params_data.TISP_PARAM_MDSN_Y_CS_IWIN_ARRAY));
	memcpy(mdns->mdns_y_cm_iwin_array, tparams.params_data.TISP_PARAM_MDSN_Y_CM_IWIN_ARRAY,
	       sizeof(tparams.params_data.TISP_PARAM_MDSN_Y_CM_IWIN_ARRAY));
	memcpy(mdns->mdns_y_rs_iwin_array, tparams.params_data.TISP_PARAM_MDSN_Y_RS_IWIN_ARRAY,
	       sizeof(tparams.params_data.TISP_PARAM_MDSN_Y_RS_IWIN_ARRAY));
	memcpy(mdns->mdns_y_rm_iwin_array, tparams.params_data.TISP_PARAM_MDSN_Y_RM_IWIN_ARRAY,
	       sizeof(tparams.params_data.TISP_PARAM_MDSN_Y_RM_IWIN_ARRAY));
	memcpy(mdns->mdns_y_cs_ewin_array, tparams.params_data.TISP_PARAM_MDSN_Y_CS_EWIN_ARRAY,
	       sizeof(tparams.params_data.TISP_PARAM_MDSN_Y_CS_EWIN_ARRAY));
	memcpy(mdns->mdns_y_cm_ewin_array, tparams.params_data.TISP_PARAM_MDSN_Y_CM_EWIN_ARRAY,
	       sizeof(tparams.params_data.TISP_PARAM_MDSN_Y_CM_EWIN_ARRAY));
	memcpy(mdns->mdns_y_rs_ewin_array, tparams.params_data.TISP_PARAM_MDSN_Y_RS_EWIN_ARRAY,
	       sizeof(tparams.params_data.TISP_PARAM_MDSN_Y_RS_EWIN_ARRAY));
	memcpy(mdns->mdns_y_rm_ewin_array, tparams.params_data.TISP_PARAM_MDSN_Y_RM_EWIN_ARRAY,
	       sizeof(tparams.params_data.TISP_PARAM_MDSN_Y_RM_EWIN_ARRAY));
	memcpy(mdns->mdns_y_cs_sego_array, tparams.params_data.TISP_PARAM_MDSN_Y_CS_SEGO_ARRAY,
	       sizeof(tparams.params_data.TISP_PARAM_MDSN_Y_CS_SEGO_ARRAY));
	memcpy(mdns->mdns_y_cm_sego_array, tparams.params_data.TISP_PARAM_MDSN_Y_CM_SEGO_ARRAY,
	       sizeof(tparams.params_data.TISP_PARAM_MDSN_Y_CM_SEGO_ARRAY));
	memcpy(mdns->mdns_y_rs_sego_array, tparams.params_data.TISP_PARAM_MDSN_Y_RS_SEGO_ARRAY,
	       sizeof(tparams.params_data.TISP_PARAM_MDSN_Y_RS_SEGO_ARRAY));
	memcpy(mdns->mdns_y_rm_sego_array, tparams.params_data.TISP_PARAM_MDSN_Y_RM_SEGO_ARRAY,
	       sizeof(tparams.params_data.TISP_PARAM_MDSN_Y_RM_SEGO_ARRAY));
	memcpy(mdns->mdns_y_cs_stren_array, tparams.params_data.TISP_PARAM_MDSN_Y_CS_STREN_ARRAY,
	       sizeof(tparams.params_data.TISP_PARAM_MDSN_Y_CS_STREN_ARRAY));
	memcpy(mdns->mdns_y_cm_stren_array, tparams.params_data.TISP_PARAM_MDSN_Y_CM_STREN_ARRAY,
	       sizeof(tparams.params_data.TISP_PARAM_MDSN_Y_CM_STREN_ARRAY));
	memcpy(mdns->mdns_y_rs_stren_array, tparams.params_data.TISP_PARAM_MDSN_Y_RS_STREN_ARRAY,
	       sizeof(tparams.params_data.TISP_PARAM_MDSN_Y_RS_STREN_ARRAY));
	memcpy(mdns->mdns_y_rm_stren_array, tparams.params_data.TISP_PARAM_MDSN_Y_RM_STREN_ARRAY,
	       sizeof(tparams.params_data.TISP_PARAM_MDSN_Y_RM_STREN_ARRAY));
	memcpy(mdns->mdns_y_cs_npv_array, tparams.params_data.TISP_PARAM_MDSN_Y_CS_NPV_ARRAY,
	       sizeof(tparams.params_data.TISP_PARAM_MDSN_Y_CS_NPV_ARRAY));
	memcpy(mdns->mdns_y_cm_npv_array, tparams.params_data.TISP_PARAM_MDSN_Y_CM_NPV_ARRAY,
	       sizeof(tparams.params_data.TISP_PARAM_MDSN_Y_CM_NPV_ARRAY));
	memcpy(mdns->mdns_y_rs_npv_array, tparams.params_data.TISP_PARAM_MDSN_Y_RS_NPV_ARRAY,
	       sizeof(tparams.params_data.TISP_PARAM_MDSN_Y_RS_NPV_ARRAY));
	memcpy(mdns->mdns_y_rm_npv_array, tparams.params_data.TISP_PARAM_MDSN_Y_RM_NPV_ARRAY,
	       sizeof(tparams.params_data.TISP_PARAM_MDSN_Y_RM_NPV_ARRAY));
	memcpy(mdns->mdns_y_shp_c_0_npv_array, tparams.params_data.TISP_PARAM_MDSN_Y_SHP_C_0_NPV_ARRAY,
	       sizeof(tparams.params_data.TISP_PARAM_MDSN_Y_SHP_C_0_NPV_ARRAY));
	memcpy(mdns->mdns_y_shp_c_1_npv_array, tparams.params_data.TISP_PARAM_MDSN_Y_SHP_C_1_NPV_ARRAY,
	       sizeof(tparams.params_data.TISP_PARAM_MDSN_Y_SHP_C_1_NPV_ARRAY));
	memcpy(mdns->mdns_y_shp_c_2_npv_array, tparams.params_data.TISP_PARAM_MDSN_Y_SHP_C_2_NPV_ARRAY,
	       sizeof(tparams.params_data.TISP_PARAM_MDSN_Y_SHP_C_2_NPV_ARRAY));
	memcpy(mdns->mdns_y_shp_c_3_npv_array, tparams.params_data.TISP_PARAM_MDSN_Y_SHP_C_3_NPV_ARRAY,
	       sizeof(tparams.params_data.TISP_PARAM_MDSN_Y_SHP_C_3_NPV_ARRAY));
	memcpy(mdns->mdns_y_shp_c_4_npv_array, tparams.params_data.TISP_PARAM_MDSN_Y_SHP_C_4_NPV_ARRAY,
	       sizeof(tparams.params_data.TISP_PARAM_MDSN_Y_SHP_C_4_NPV_ARRAY));
	memcpy(mdns->mdns_y_shp_c_5_npv_array, tparams.params_data.TISP_PARAM_MDSN_Y_SHP_C_5_NPV_ARRAY,
	       sizeof(tparams.params_data.TISP_PARAM_MDSN_Y_SHP_C_5_NPV_ARRAY));
	memcpy(mdns->mdns_y_shp_c_6_npv_array, tparams.params_data.TISP_PARAM_MDSN_Y_SHP_C_6_NPV_ARRAY,
	       sizeof(tparams.params_data.TISP_PARAM_MDSN_Y_SHP_C_6_NPV_ARRAY));
	memcpy(mdns->mdns_y_shp_c_7_npv_array, tparams.params_data.TISP_PARAM_MDSN_Y_SHP_C_7_NPV_ARRAY,
	       sizeof(tparams.params_data.TISP_PARAM_MDSN_Y_SHP_C_7_NPV_ARRAY));
	memcpy(mdns->mdns_y_shp_c_8_npv_array, tparams.params_data.TISP_PARAM_MDSN_Y_SHP_C_8_NPV_ARRAY,
	       sizeof(tparams.params_data.TISP_PARAM_MDSN_Y_SHP_C_8_NPV_ARRAY));
	memcpy(mdns->mdns_y_shp_c_9_npv_array, tparams.params_data.TISP_PARAM_MDSN_Y_SHP_C_9_NPV_ARRAY,
	       sizeof(tparams.params_data.TISP_PARAM_MDSN_Y_SHP_C_9_NPV_ARRAY));
	memcpy(mdns->mdns_y_shp_c_a_npv_array, tparams.params_data.TISP_PARAM_MDSN_Y_SHP_C_A_NPV_ARRAY,
	       sizeof(tparams.params_data.TISP_PARAM_MDSN_Y_SHP_C_A_NPV_ARRAY));
	memcpy(mdns->mdns_y_shp_c_b_npv_array, tparams.params_data.TISP_PARAM_MDSN_Y_SHP_C_B_NPV_ARRAY,
	       sizeof(tparams.params_data.TISP_PARAM_MDSN_Y_SHP_C_B_NPV_ARRAY));
	memcpy(mdns->mdns_y_shp_c_c_npv_array, tparams.params_data.TISP_PARAM_MDSN_Y_SHP_C_C_NPV_ARRAY,
	       sizeof(tparams.params_data.TISP_PARAM_MDSN_Y_SHP_C_C_NPV_ARRAY));
	memcpy(mdns->mdns_y_shp_c_d_npv_array, tparams.params_data.TISP_PARAM_MDSN_Y_SHP_C_D_NPV_ARRAY,
	       sizeof(tparams.params_data.TISP_PARAM_MDSN_Y_SHP_C_D_NPV_ARRAY));
	memcpy(mdns->mdns_y_shp_c_e_npv_array, tparams.params_data.TISP_PARAM_MDSN_Y_SHP_C_E_NPV_ARRAY,
	       sizeof(tparams.params_data.TISP_PARAM_MDSN_Y_SHP_C_E_NPV_ARRAY));
	memcpy(mdns->mdns_y_shp_c_f_npv_array, tparams.params_data.TISP_PARAM_MDSN_Y_SHP_C_F_NPV_ARRAY,
	       sizeof(tparams.params_data.TISP_PARAM_MDSN_Y_SHP_C_F_NPV_ARRAY));
	memcpy(mdns->mdns_y_shp_r_0_npv_array, tparams.params_data.TISP_PARAM_MDSN_Y_SHP_R_0_NPV_ARRAY,
	       sizeof(tparams.params_data.TISP_PARAM_MDSN_Y_SHP_R_0_NPV_ARRAY));
	memcpy(mdns->mdns_y_shp_r_1_npv_array, tparams.params_data.TISP_PARAM_MDSN_Y_SHP_R_1_NPV_ARRAY,
	       sizeof(tparams.params_data.TISP_PARAM_MDSN_Y_SHP_R_1_NPV_ARRAY));
	memcpy(mdns->mdns_y_shp_r_2_npv_array, tparams.params_data.TISP_PARAM_MDSN_Y_SHP_R_2_NPV_ARRAY,
	       sizeof(tparams.params_data.TISP_PARAM_MDSN_Y_SHP_R_2_NPV_ARRAY));
	memcpy(mdns->mdns_y_shp_r_3_npv_array, tparams.params_data.TISP_PARAM_MDSN_Y_SHP_R_3_NPV_ARRAY,
	       sizeof(tparams.params_data.TISP_PARAM_MDSN_Y_SHP_R_3_NPV_ARRAY));
	memcpy(mdns->mdns_y_shp_r_4_npv_array, tparams.params_data.TISP_PARAM_MDSN_Y_SHP_R_4_NPV_ARRAY,
	       sizeof(tparams.params_data.TISP_PARAM_MDSN_Y_SHP_R_4_NPV_ARRAY));
	memcpy(mdns->mdns_y_shp_r_5_npv_array, tparams.params_data.TISP_PARAM_MDSN_Y_SHP_R_5_NPV_ARRAY,
	       sizeof(tparams.params_data.TISP_PARAM_MDSN_Y_SHP_R_5_NPV_ARRAY));
	memcpy(mdns->mdns_y_shp_r_6_npv_array, tparams.params_data.TISP_PARAM_MDSN_Y_SHP_R_6_NPV_ARRAY,
	       sizeof(tparams.params_data.TISP_PARAM_MDSN_Y_SHP_R_6_NPV_ARRAY));
	memcpy(mdns->mdns_y_shp_r_7_npv_array, tparams.params_data.TISP_PARAM_MDSN_Y_SHP_R_7_NPV_ARRAY,
	       sizeof(tparams.params_data.TISP_PARAM_MDSN_Y_SHP_R_7_NPV_ARRAY));
	memcpy(mdns->mdns_y_shp_r_8_npv_array, tparams.params_data.TISP_PARAM_MDSN_Y_SHP_R_8_NPV_ARRAY,
	       sizeof(tparams.params_data.TISP_PARAM_MDSN_Y_SHP_R_8_NPV_ARRAY));
	memcpy(mdns->mdns_y_shp_r_9_npv_array, tparams.params_data.TISP_PARAM_MDSN_Y_SHP_R_9_NPV_ARRAY,
	       sizeof(tparams.params_data.TISP_PARAM_MDSN_Y_SHP_R_9_NPV_ARRAY));
	memcpy(mdns->mdns_y_shp_r_a_npv_array, tparams.params_data.TISP_PARAM_MDSN_Y_SHP_R_A_NPV_ARRAY,
	       sizeof(tparams.params_data.TISP_PARAM_MDSN_Y_SHP_R_A_NPV_ARRAY));
	memcpy(mdns->mdns_y_shp_r_b_npv_array, tparams.params_data.TISP_PARAM_MDSN_Y_SHP_R_B_NPV_ARRAY,
	       sizeof(tparams.params_data.TISP_PARAM_MDSN_Y_SHP_R_B_NPV_ARRAY));
	memcpy(mdns->mdns_y_shp_r_c_npv_array, tparams.params_data.TISP_PARAM_MDSN_Y_SHP_R_C_NPV_ARRAY,
	       sizeof(tparams.params_data.TISP_PARAM_MDSN_Y_SHP_R_C_NPV_ARRAY));
	memcpy(mdns->mdns_y_shp_r_d_npv_array, tparams.params_data.TISP_PARAM_MDSN_Y_SHP_R_D_NPV_ARRAY,
	       sizeof(tparams.params_data.TISP_PARAM_MDSN_Y_SHP_R_D_NPV_ARRAY));
	memcpy(mdns->mdns_y_shp_r_e_npv_array, tparams.params_data.TISP_PARAM_MDSN_Y_SHP_R_E_NPV_ARRAY,
	       sizeof(tparams.params_data.TISP_PARAM_MDSN_Y_SHP_R_E_NPV_ARRAY));
	memcpy(mdns->mdns_y_shp_r_f_npv_array, tparams.params_data.TISP_PARAM_MDSN_Y_SHP_R_F_NPV_ARRAY,
	       sizeof(tparams.params_data.TISP_PARAM_MDSN_Y_SHP_R_F_NPV_ARRAY));
	memcpy(mdns->mdns_y_adj_cnr_array, tparams.params_data.TISP_PARAM_MDSN_Y_ADJ_CNR_ARRAY,
	       sizeof(tparams.params_data.TISP_PARAM_MDSN_Y_ADJ_CNR_ARRAY));
	memcpy(mdns->mdns_y_fluct_lmt_array, tparams.params_data.TISP_PARAM_MDSN_Y_FLUCT_LMT_ARRAY,
	       sizeof(tparams.params_data.TISP_PARAM_MDSN_Y_FLUCT_LMT_ARRAY));
	memcpy(mdns->mdns_y_adj_sta_array, tparams.params_data.TISP_PARAM_MDSN_Y_ADJ_STA_ARRAY,
	       sizeof(tparams.params_data.TISP_PARAM_MDSN_Y_ADJ_STA_ARRAY));
	memcpy(mdns->mdns_y_adj_tedg_s_array, tparams.params_data.TISP_PARAM_MDSN_Y_ADJ_TEDG_S_ARRAY,
	       sizeof(tparams.params_data.TISP_PARAM_MDSN_Y_ADJ_TEDG_S_ARRAY));
	memcpy(mdns->mdns_y_adj_wedg_s_array, tparams.params_data.TISP_PARAM_MDSN_Y_ADJ_WEDG_S_ARRAY,
	       sizeof(tparams.params_data.TISP_PARAM_MDSN_Y_ADJ_WEDG_S_ARRAY));
	memcpy(mdns->mdns_y_adj_tedg_n_array, tparams.params_data.TISP_PARAM_MDSN_Y_ADJ_TEDG_N_ARRAY,
	       sizeof(tparams.params_data.TISP_PARAM_MDSN_Y_ADJ_TEDG_N_ARRAY));
	memcpy(mdns->mdns_y_adj_wedg_n_array, tparams.params_data.TISP_PARAM_MDSN_Y_ADJ_WEDG_N_ARRAY,
	       sizeof(tparams.params_data.TISP_PARAM_MDSN_Y_ADJ_WEDG_N_ARRAY));
	memcpy(mdns->mdns_y_adj_lum_win_array, tparams.params_data.TISP_PARAM_MDSN_Y_ADJ_LUM_WIN_ARRAY,
	       sizeof(tparams.params_data.TISP_PARAM_MDSN_Y_ADJ_LUM_WIN_ARRAY));
	memcpy(mdns->mdns_y_adj_tlum_s_array, tparams.params_data.TISP_PARAM_MDSN_Y_ADJ_TLUM_S_ARRAY,
	       sizeof(tparams.params_data.TISP_PARAM_MDSN_Y_ADJ_TLUM_S_ARRAY));
	memcpy(mdns->mdns_y_adj_wlum_s_array, tparams.params_data.TISP_PARAM_MDSN_Y_ADJ_WLUM_S_ARRAY,
	       sizeof(tparams.params_data.TISP_PARAM_MDSN_Y_ADJ_WLUM_S_ARRAY));
	memcpy(mdns->mdns_y_adj_tlum_n_array, tparams.params_data.TISP_PARAM_MDSN_Y_ADJ_TLUM_N_ARRAY,
	       sizeof(tparams.params_data.TISP_PARAM_MDSN_Y_ADJ_TLUM_N_ARRAY));
	memcpy(mdns->mdns_y_adj_wlum_n_array, tparams.params_data.TISP_PARAM_MDSN_Y_ADJ_WLUM_N_ARRAY,
	       sizeof(tparams.params_data.TISP_PARAM_MDSN_Y_ADJ_WLUM_N_ARRAY));
	memcpy(mdns->mdns_c_sad_thres_array, tparams.params_data.TISP_PARAM_MDSN_C_SAD_THRES_ARRAY,
	       sizeof(tparams.params_data.TISP_PARAM_MDSN_C_SAD_THRES_ARRAY));
	memcpy(mdns->mdns_c_sad_win_opt_array, tparams.params_data.TISP_PARAM_MDSN_C_SAD_WIN_OPT_ARRAY,
	       sizeof(tparams.params_data.TISP_PARAM_MDSN_C_SAD_WIN_OPT_ARRAY));
	memcpy(mdns->mdns_c_sad_stren_array, tparams.params_data.TISP_PARAM_MDSN_C_SAD_STREN_ARRAY,
	       sizeof(tparams.params_data.TISP_PARAM_MDSN_C_SAD_STREN_ARRAY));
	memcpy(mdns->mdns_c_sta_stren_array, tparams.params_data.TISP_PARAM_MDSN_C_STA_STREN_ARRAY,
	       sizeof(tparams.params_data.TISP_PARAM_MDSN_C_STA_STREN_ARRAY));
	memcpy(mdns->mdns_c_pbt_stren_array, tparams.params_data.TISP_PARAM_MDSN_C_PBT_STREN_ARRAY,
	       sizeof(tparams.params_data.TISP_PARAM_MDSN_C_PBT_STREN_ARRAY));
	memcpy(mdns->mdns_c_sad_npv_array, tparams.params_data.TISP_PARAM_MDSN_C_SAD_NPV_ARRAY,
	       sizeof(tparams.params_data.TISP_PARAM_MDSN_C_SAD_NPV_ARRAY));
	memcpy(mdns->mdns_c_sta_npv_array, tparams.params_data.TISP_PARAM_MDSN_C_STA_NPV_ARRAY,
	       sizeof(tparams.params_data.TISP_PARAM_MDSN_C_STA_NPV_ARRAY));
	memcpy(mdns->mdns_c_pbt_npv_array, tparams.params_data.TISP_PARAM_MDSN_C_PBT_NPV_ARRAY,
	       sizeof(tparams.params_data.TISP_PARAM_MDSN_C_PBT_NPV_ARRAY));
	memcpy(mdns->mdns_c_ref_wei_min_array, tparams.params_data.TISP_PARAM_MDSN_C_REF_WEI_MIN_ARRAY,
	       sizeof(tparams.params_data.TISP_PARAM_MDSN_C_REF_WEI_MIN_ARRAY));
	memcpy(mdns->mdns_c_ref_wei_max_array, tparams.params_data.TISP_PARAM_MDSN_C_REF_WEI_MAX_ARRAY,
	       sizeof(tparams.params_data.TISP_PARAM_MDSN_C_REF_WEI_MAX_ARRAY));
	memcpy(mdns->mdns_c_cur_blur_array, tparams.params_data.TISP_PARAM_MDSN_C_CUR_BLUR_ARRAY,
	       sizeof(tparams.params_data.TISP_PARAM_MDSN_C_CUR_BLUR_ARRAY));
	memcpy(mdns->mdns_c_ref_blur_array, tparams.params_data.TISP_PARAM_MDSN_C_REF_BLUR_ARRAY,
	       sizeof(tparams.params_data.TISP_PARAM_MDSN_C_REF_BLUR_ARRAY));
	memcpy(mdns->mdns_c_smj_thres_array, tparams.params_data.TISP_PARAM_MDSN_C_SMJ_THRES_ARRAY,
	       sizeof(tparams.params_data.TISP_PARAM_MDSN_C_SMJ_THRES_ARRAY));
	memcpy(mdns->mdns_c_sfla_boh_t_array, tparams.params_data.TISP_PARAM_MDSN_C_SFLA_BOH_T_ARRAY,
	       sizeof(tparams.params_data.TISP_PARAM_MDSN_C_SFLA_BOH_T_ARRAY));
	memcpy(mdns->mdns_c_sfla_cut_t_array, tparams.params_data.TISP_PARAM_MDSN_C_SFLA_CUT_T_ARRAY,
	       sizeof(tparams.params_data.TISP_PARAM_MDSN_C_SFLA_CUT_T_ARRAY));
	memcpy(mdns->mdns_c_sfla_boh_s_array, tparams.params_data.TISP_PARAM_MDSN_C_SFLA_BOH_S_ARRAY,
	       sizeof(tparams.params_data.TISP_PARAM_MDSN_C_SFLA_BOH_S_ARRAY));
	memcpy(mdns->mdns_c_sfla_cut_s_array, tparams.params_data.TISP_PARAM_MDSN_C_SFLA_CUT_S_ARRAY,
	       sizeof(tparams.params_data.TISP_PARAM_MDSN_C_SFLA_CUT_S_ARRAY));
	memcpy(mdns->mdns_c_mfla_boh_t_array, tparams.params_data.TISP_PARAM_MDSN_C_MFLA_BOH_T_ARRAY,
	       sizeof(tparams.params_data.TISP_PARAM_MDSN_C_MFLA_BOH_T_ARRAY));
	memcpy(mdns->mdns_c_mfla_cut_t_array, tparams.params_data.TISP_PARAM_MDSN_C_MFLA_CUT_T_ARRAY,
	       sizeof(tparams.params_data.TISP_PARAM_MDSN_C_MFLA_CUT_T_ARRAY));
	memcpy(mdns->mdns_c_mfla_boh_s_array, tparams.params_data.TISP_PARAM_MDSN_C_MFLA_BOH_S_ARRAY,
	       sizeof(tparams.params_data.TISP_PARAM_MDSN_C_MFLA_BOH_S_ARRAY));
	memcpy(mdns->mdns_c_mfla_cut_s_array, tparams.params_data.TISP_PARAM_MDSN_C_MFLA_CUT_S_ARRAY,
	       sizeof(tparams.params_data.TISP_PARAM_MDSN_C_MFLA_CUT_S_ARRAY));
	memcpy(mdns->mdns_c_adj_cnr_array, tparams.params_data.TISP_PARAM_MDSN_C_ADJ_CNR_ARRAY,
	       sizeof(tparams.params_data.TISP_PARAM_MDSN_C_ADJ_CNR_ARRAY));
	memcpy(mdns->mdns_c_adj_sta_array, tparams.params_data.TISP_PARAM_MDSN_C_ADJ_STA_ARRAY,
	       sizeof(tparams.params_data.TISP_PARAM_MDSN_C_ADJ_STA_ARRAY));
	memcpy(mdns->mdns_c_adj_tcrm_s_array, tparams.params_data.TISP_PARAM_MDSN_C_ADJ_TCRM_S_ARRAY,
	       sizeof(tparams.params_data.TISP_PARAM_MDSN_C_ADJ_TCRM_S_ARRAY));
	memcpy(mdns->mdns_c_adj_wcrm_s_array, tparams.params_data.TISP_PARAM_MDSN_C_ADJ_WCRM_S_ARRAY,
	       sizeof(tparams.params_data.TISP_PARAM_MDSN_C_ADJ_WCRM_S_ARRAY));
	memcpy(mdns->mdns_c_adj_tcrm_n_array, tparams.params_data.TISP_PARAM_MDSN_C_ADJ_TCRM_N_ARRAY,
	       sizeof(tparams.params_data.TISP_PARAM_MDSN_C_ADJ_TCRM_N_ARRAY));
	memcpy(mdns->mdns_c_adj_wcrm_n_array, tparams.params_data.TISP_PARAM_MDSN_C_ADJ_WCRM_N_ARRAY,
	       sizeof(tparams.params_data.TISP_PARAM_MDSN_C_ADJ_WCRM_N_ARRAY));

	return 0;
}

int32_t tisp_mdns_bypass(tisp_mdns_t *mdns, int32_t en)
{
	tisp_mdns_top_func_cfg(mdns, !en);

	return 0;
}

int32_t tiziano_mdns_dn_params_refresh(tisp_mdns_t *mdns)
{
	gain_old += 0x200;
	tiziano_mdns_params_refresh(mdns);
	tisp_mdns_all_reg_refresh(mdns, gain_old);

	return 0;
}

int32_t tiziano_mdns_init(tisp_mdns_t *mdns, uint32_t width, uint32_t height)
{
	mdns->vin_width = width;
	mdns->vin_height = height;

	gain_old = 0xffffffff;
	tiziano_mdns_params_refresh(mdns);
	tisp_mdns_par_refresh(mdns, 0x10000, 0x10000, mdns->vin_width, mdns->vin_height);
	tisp_mdns_bypass(mdns, 0);

	return 0;
}

int32_t tisp_mdns_refresh(tisp_mdns_t *mdns, uint32_t gain_value)
{
	tisp_mdns_par_refresh(mdns, gain_value, gain_thres,mdns->vin_width,mdns->vin_height);

	return 0;
}

int32_t tisp_mdns_param_array_get(tisp_mdns_t *mdns, int32_t id, void *buf, uint32_t *plen)
{
	uint32_t len;
	void *tmpbuf;

	switch (id) {
	case TISP_PARAM_MDNS_TOP_FUNC_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_MDNS_TOP_FUNC_ARRAY);
		tmpbuf = mdns->mdns_top_func_array;
		break;
	case TISP_PARAM_MDSN_STA_SIZE_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_MDSN_STA_SIZE_ARRAY);
		tmpbuf = mdns->mdns_sta_size_array;
		break;
	case TISP_PARAM_MDSN_PBT_SIZE_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_MDSN_PBT_SIZE_ARRAY);
		tmpbuf = mdns->mdns_pbt_size_array;
		break;
	case TISP_PARAM_MDSN_PBT_PONIT_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_MDSN_PBT_PONIT_ARRAY);
		tmpbuf = mdns->mdns_pbt_ponit_array;
		break;
	case TISP_PARAM_MDSN_Y_SAD_THRES_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_MDSN_Y_SAD_THRES_ARRAY);
		tmpbuf = mdns->mdns_y_sad_thres_array;
		break;
	case TISP_PARAM_MDSN_Y_STA_THRES_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_MDSN_Y_STA_THRES_ARRAY);
		tmpbuf = mdns->mdns_y_sta_thres_array;
		break;
	case TISP_PARAM_MDSN_Y_PBT_THRES_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_MDSN_Y_PBT_THRES_ARRAY);
		tmpbuf = mdns->mdns_y_pbt_thres_array;
		break;
	case TISP_PARAM_MDSN_Y_SAD_STREN_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_MDSN_Y_SAD_STREN_ARRAY);
		tmpbuf = mdns->mdns_y_sad_stren_array;
		break;
	case TISP_PARAM_MDSN_Y_STA_STREN_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_MDSN_Y_STA_STREN_ARRAY);
		tmpbuf = mdns->mdns_y_sta_stren_array;
		break;
	case TISP_PARAM_MDSN_Y_PBT_STREN_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_MDSN_Y_PBT_STREN_ARRAY);
		tmpbuf = mdns->mdns_y_pbt_stren_array;
		break;
	case TISP_PARAM_MDSN_Y_SAD_WIN_OPT_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_MDSN_Y_SAD_WIN_OPT_ARRAY);
		tmpbuf = mdns->mdns_y_sad_win_opt_array;
		break;
	case TISP_PARAM_MDSN_Y_STA_WIN_OPT_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_MDSN_Y_STA_WIN_OPT_ARRAY);
		tmpbuf = mdns->mdns_y_sta_win_opt_array;
		break;
	case TISP_PARAM_MDSN_Y_PBT_WIN_OPT_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_MDSN_Y_PBT_WIN_OPT_ARRAY);
		tmpbuf = mdns->mdns_y_pbt_win_opt_array;
		break;
	case TISP_PARAM_MDSN_Y_SAD_WIN_WEI_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_MDSN_Y_SAD_WIN_WEI_ARRAY);
		tmpbuf = mdns->mdns_y_sad_win_wei_array;
		break;
	case TISP_PARAM_MDSN_Y_STA_WIN_WEI_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_MDSN_Y_STA_WIN_WEI_ARRAY);
		tmpbuf = mdns->mdns_y_sta_win_wei_array;
		break;
	case TISP_PARAM_MDSN_Y_PBT_WIn_WEI_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_MDSN_Y_PBT_WIn_WEI_ARRAY);
		tmpbuf = mdns->mdns_y_pbt_win_wei_array;
		break;
	case TISP_PARAM_MDSN_Y_STA_MV_NUM_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_MDSN_Y_STA_MV_NUM_ARRAY);
		tmpbuf = mdns->mdns_y_sta_mv_num_array;
		break;
	case TISP_PARAM_MDSN_Y_PBT_MV_NUM_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_MDSN_Y_PBT_MV_NUM_ARRAY);
		tmpbuf = mdns->mdns_y_pbt_mv_num_array;
		break;
	case TISP_PARAM_MDSN_Y_STA_MX_NUM_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_MDSN_Y_STA_MX_NUM_ARRAY);
		tmpbuf = mdns->mdns_y_sta_mx_num_array;
		break;
	case TISP_PARAM_MDSN_Y_PBT_MX_NUM_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_MDSN_Y_PBT_MX_NUM_ARRAY);
		tmpbuf = mdns->mdns_y_pbt_mx_num_array;
		break;
	case TISP_PARAM_MDSN_Y_SAD_NPV_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_MDSN_Y_SAD_NPV_ARRAY);
		tmpbuf = mdns->mdns_y_sad_npv_array;
		break;
	case TISP_PARAM_MDSN_Y_STA_NPV_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_MDSN_Y_STA_NPV_ARRAY);
		tmpbuf = mdns->mdns_y_sta_npv_array;
		break;
	case TISP_PARAM_MDSN_Y_PBT_NPV_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_MDSN_Y_PBT_NPV_ARRAY);
		tmpbuf = mdns->mdns_y_pbt_npv_array;
		break;
	case TISP_PARAM_MDSN_Y_REF_WEI_MIN_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_MDSN_Y_REF_WEI_MIN_ARRAY);
		tmpbuf = mdns->mdns_y_ref_wei_min_array;
		break;
	case TISP_PARAM_MDSN_Y_REF_WEI_MAX_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_MDSN_Y_REF_WEI_MAX_ARRAY);
		tmpbuf = mdns->mdns_y_ref_wei_max_array;
		break;
	case TISP_PARAM_MDSN_Y_EDGE_TYPE_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_MDSN_Y_EDGE_TYPE_ARRAY);
		tmpbuf = mdns->mdns_y_edge_type_array;
		break;
	case TISP_PARAM_MDSN_Y_EDGE_WIN_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_MDSN_Y_EDGE_WIN_ARRAY);
		tmpbuf = mdns->mdns_y_edge_win_array;
		break;
	case TISP_PARAM_MDSN_Y_BI_THRES_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_MDSN_Y_BI_THRES_ARRAY);
		tmpbuf = mdns->mdns_y_bi_thres_array;
		break;
	case TISP_PARAM_MDSN_Y_SMJ_THRES_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_MDSN_Y_SMJ_THRES_ARRAY);
		tmpbuf = mdns->mdns_y_smj_thres_array;
		break;
	case TISP_PARAM_MDSN_Y_SHP_SWEI_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_MDSN_Y_SHP_SWEI_ARRAY);
		tmpbuf = mdns->mdns_y_shp_swei_array;
		break;
	case TISP_PARAM_MDSN_Y_SHP_MWEI_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_MDSN_Y_SHP_MWEI_ARRAY);
		tmpbuf = mdns->mdns_y_shp_mwei_array;
		break;
	case TISP_PARAM_MDSN_Y_CS_BHOLD_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_MDSN_Y_CS_BHOLD_ARRAY);
		tmpbuf = mdns->mdns_y_cs_bhold_array;
		break;
	case TISP_PARAM_MDSN_Y_CM_BHOLD_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_MDSN_Y_CM_BHOLD_ARRAY);
		tmpbuf = mdns->mdns_y_cm_bhold_array;
		break;
	case TISP_PARAM_MDSN_Y_RS_BHOLD_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_MDSN_Y_RS_BHOLD_ARRAY);
		tmpbuf = mdns->mdns_y_rs_bhold_array;
		break;
	case TISP_PARAM_MDSN_Y_RM_BHOLD_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_MDSN_Y_RM_BHOLD_ARRAY);
		tmpbuf = mdns->mdns_y_rm_bhold_array;
		break;
	case TISP_PARAM_MDSN_Y_CS_BWIN_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_MDSN_Y_CS_BWIN_ARRAY);
		tmpbuf = mdns->mdns_y_cs_bwin_array;
		break;
	case TISP_PARAM_MDSN_Y_CM_BWIN_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_MDSN_Y_CM_BWIN_ARRAY);
		tmpbuf = mdns->mdns_y_cm_bwin_array;
		break;
	case TISP_PARAM_MDSN_Y_RS_BWIN_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_MDSN_Y_RS_BWIN_ARRAY);
		tmpbuf = mdns->mdns_y_rs_bwin_array;
		break;
	case TISP_PARAM_MDSN_Y_RM_BWIN_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_MDSN_Y_RM_BWIN_ARRAY);
		tmpbuf = mdns->mdns_y_rm_bwin_array;
		break;
	case TISP_PARAM_MDSN_Y_CS_IWIN_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_MDSN_Y_CS_IWIN_ARRAY);
		tmpbuf = mdns->mdns_y_cs_iwin_array;
		break;
	case TISP_PARAM_MDSN_Y_CM_IWIN_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_MDSN_Y_CM_IWIN_ARRAY);
		tmpbuf = mdns->mdns_y_cm_iwin_array;
		break;
	case TISP_PARAM_MDSN_Y_RS_IWIN_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_MDSN_Y_RS_IWIN_ARRAY);
		tmpbuf = mdns->mdns_y_rs_iwin_array;
		break;
	case TISP_PARAM_MDSN_Y_RM_IWIN_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_MDSN_Y_RM_IWIN_ARRAY);
		tmpbuf = mdns->mdns_y_rm_iwin_array;
		break;
	case TISP_PARAM_MDSN_Y_CS_EWIN_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_MDSN_Y_CS_EWIN_ARRAY);
		tmpbuf = mdns->mdns_y_cs_ewin_array;
		break;
	case TISP_PARAM_MDSN_Y_CM_EWIN_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_MDSN_Y_CM_EWIN_ARRAY);
		tmpbuf = mdns->mdns_y_cm_ewin_array;
		break;
	case TISP_PARAM_MDSN_Y_RS_EWIN_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_MDSN_Y_RS_EWIN_ARRAY);
		tmpbuf = mdns->mdns_y_rs_ewin_array;
		break;
	case TISP_PARAM_MDSN_Y_RM_EWIN_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_MDSN_Y_RM_EWIN_ARRAY);
		tmpbuf = mdns->mdns_y_rm_ewin_array;
		break;
	case TISP_PARAM_MDSN_Y_CS_SEGO_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_MDSN_Y_CS_SEGO_ARRAY);
		tmpbuf = mdns->mdns_y_cs_sego_array;
		break;
	case TISP_PARAM_MDSN_Y_CM_SEGO_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_MDSN_Y_CM_SEGO_ARRAY);
		tmpbuf = mdns->mdns_y_cm_sego_array;
		break;
	case TISP_PARAM_MDSN_Y_RS_SEGO_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_MDSN_Y_RS_SEGO_ARRAY);
		tmpbuf = mdns->mdns_y_rs_sego_array;
		break;
	case TISP_PARAM_MDSN_Y_RM_SEGO_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_MDSN_Y_RM_SEGO_ARRAY);
		tmpbuf = mdns->mdns_y_rm_sego_array;
		break;
	case TISP_PARAM_MDSN_Y_CS_STREN_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_MDSN_Y_CS_STREN_ARRAY);
		tmpbuf = mdns->mdns_y_cs_stren_array;
		break;
	case TISP_PARAM_MDSN_Y_CM_STREN_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_MDSN_Y_CM_STREN_ARRAY);
		tmpbuf = mdns->mdns_y_cm_stren_array;
		break;
	case TISP_PARAM_MDSN_Y_RS_STREN_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_MDSN_Y_RS_STREN_ARRAY);
		tmpbuf = mdns->mdns_y_rs_stren_array;
		break;
	case TISP_PARAM_MDSN_Y_RM_STREN_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_MDSN_Y_RM_STREN_ARRAY);
		tmpbuf = mdns->mdns_y_rm_stren_array;
		break;
	case TISP_PARAM_MDSN_Y_CS_NPV_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_MDSN_Y_CS_NPV_ARRAY);
		tmpbuf = mdns->mdns_y_cs_npv_array;
		break;
	case TISP_PARAM_MDSN_Y_CM_NPV_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_MDSN_Y_CM_NPV_ARRAY);
		tmpbuf = mdns->mdns_y_cm_npv_array;
		break;
	case TISP_PARAM_MDSN_Y_RS_NPV_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_MDSN_Y_RS_NPV_ARRAY);
		tmpbuf = mdns->mdns_y_rs_npv_array;
		break;
	case TISP_PARAM_MDSN_Y_RM_NPV_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_MDSN_Y_RM_NPV_ARRAY);
		tmpbuf = mdns->mdns_y_rm_npv_array;
		break;
	case TISP_PARAM_MDSN_Y_SHP_C_0_NPV_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_MDSN_Y_SHP_C_0_NPV_ARRAY);
		tmpbuf = mdns->mdns_y_shp_c_0_npv_array;
		break;
	case TISP_PARAM_MDSN_Y_SHP_C_1_NPV_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_MDSN_Y_SHP_C_1_NPV_ARRAY);
		tmpbuf = mdns->mdns_y_shp_c_1_npv_array;
		break;
	case TISP_PARAM_MDSN_Y_SHP_C_2_NPV_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_MDSN_Y_SHP_C_2_NPV_ARRAY);
		tmpbuf = mdns->mdns_y_shp_c_2_npv_array;
		break;
	case TISP_PARAM_MDSN_Y_SHP_C_3_NPV_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_MDSN_Y_SHP_C_3_NPV_ARRAY);
		tmpbuf = mdns->mdns_y_shp_c_3_npv_array;
		break;
	case TISP_PARAM_MDSN_Y_SHP_C_4_NPV_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_MDSN_Y_SHP_C_4_NPV_ARRAY);
		tmpbuf = mdns->mdns_y_shp_c_4_npv_array;
		break;
	case TISP_PARAM_MDSN_Y_SHP_C_5_NPV_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_MDSN_Y_SHP_C_5_NPV_ARRAY);
		tmpbuf = mdns->mdns_y_shp_c_5_npv_array;
		break;
	case TISP_PARAM_MDSN_Y_SHP_C_6_NPV_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_MDSN_Y_SHP_C_6_NPV_ARRAY);
		tmpbuf = mdns->mdns_y_shp_c_6_npv_array;
		break;
	case TISP_PARAM_MDSN_Y_SHP_C_7_NPV_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_MDSN_Y_SHP_C_7_NPV_ARRAY);
		tmpbuf = mdns->mdns_y_shp_c_7_npv_array;
		break;
	case TISP_PARAM_MDSN_Y_SHP_C_8_NPV_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_MDSN_Y_SHP_C_8_NPV_ARRAY);
		tmpbuf = mdns->mdns_y_shp_c_8_npv_array;
		break;
	case TISP_PARAM_MDSN_Y_SHP_C_9_NPV_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_MDSN_Y_SHP_C_9_NPV_ARRAY);
		tmpbuf = mdns->mdns_y_shp_c_9_npv_array;
		break;
	case TISP_PARAM_MDSN_Y_SHP_C_A_NPV_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_MDSN_Y_SHP_C_A_NPV_ARRAY);
		tmpbuf = mdns->mdns_y_shp_c_a_npv_array;
		break;
	case TISP_PARAM_MDSN_Y_SHP_C_B_NPV_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_MDSN_Y_SHP_C_B_NPV_ARRAY);
		tmpbuf = mdns->mdns_y_shp_c_b_npv_array;
		break;
	case TISP_PARAM_MDSN_Y_SHP_C_C_NPV_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_MDSN_Y_SHP_C_C_NPV_ARRAY);
		tmpbuf = mdns->mdns_y_shp_c_c_npv_array;
		break;
	case TISP_PARAM_MDSN_Y_SHP_C_D_NPV_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_MDSN_Y_SHP_C_D_NPV_ARRAY);
		tmpbuf = mdns->mdns_y_shp_c_d_npv_array;
		break;
	case TISP_PARAM_MDSN_Y_SHP_C_E_NPV_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_MDSN_Y_SHP_C_E_NPV_ARRAY);
		tmpbuf = mdns->mdns_y_shp_c_e_npv_array;
		break;
	case TISP_PARAM_MDSN_Y_SHP_C_F_NPV_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_MDSN_Y_SHP_C_F_NPV_ARRAY);
		tmpbuf = mdns->mdns_y_shp_c_f_npv_array;
		break;
	case TISP_PARAM_MDSN_Y_SHP_R_0_NPV_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_MDSN_Y_SHP_R_0_NPV_ARRAY);
		tmpbuf = mdns->mdns_y_shp_r_0_npv_array;
		break;
	case TISP_PARAM_MDSN_Y_SHP_R_1_NPV_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_MDSN_Y_SHP_R_1_NPV_ARRAY);
		tmpbuf = mdns->mdns_y_shp_r_1_npv_array;
		break;
	case TISP_PARAM_MDSN_Y_SHP_R_2_NPV_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_MDSN_Y_SHP_R_2_NPV_ARRAY);
		tmpbuf = mdns->mdns_y_shp_r_2_npv_array;
		break;
	case TISP_PARAM_MDSN_Y_SHP_R_3_NPV_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_MDSN_Y_SHP_R_3_NPV_ARRAY);
		tmpbuf = mdns->mdns_y_shp_r_3_npv_array;
		break;
	case TISP_PARAM_MDSN_Y_SHP_R_4_NPV_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_MDSN_Y_SHP_R_4_NPV_ARRAY);
		tmpbuf = mdns->mdns_y_shp_r_4_npv_array;
		break;
	case TISP_PARAM_MDSN_Y_SHP_R_5_NPV_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_MDSN_Y_SHP_R_5_NPV_ARRAY);
		tmpbuf = mdns->mdns_y_shp_r_5_npv_array;
		break;
	case TISP_PARAM_MDSN_Y_SHP_R_6_NPV_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_MDSN_Y_SHP_R_6_NPV_ARRAY);
		tmpbuf = mdns->mdns_y_shp_r_6_npv_array;
		break;
	case TISP_PARAM_MDSN_Y_SHP_R_7_NPV_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_MDSN_Y_SHP_R_7_NPV_ARRAY);
		tmpbuf = mdns->mdns_y_shp_r_7_npv_array;
		break;
	case TISP_PARAM_MDSN_Y_SHP_R_8_NPV_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_MDSN_Y_SHP_R_8_NPV_ARRAY);
		tmpbuf = mdns->mdns_y_shp_r_8_npv_array;
		break;
	case TISP_PARAM_MDSN_Y_SHP_R_9_NPV_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_MDSN_Y_SHP_R_9_NPV_ARRAY);
		tmpbuf = mdns->mdns_y_shp_r_9_npv_array;
		break;
	case TISP_PARAM_MDSN_Y_SHP_R_A_NPV_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_MDSN_Y_SHP_R_A_NPV_ARRAY);
		tmpbuf = mdns->mdns_y_shp_r_a_npv_array;
		break;
	case TISP_PARAM_MDSN_Y_SHP_R_B_NPV_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_MDSN_Y_SHP_R_B_NPV_ARRAY);
		tmpbuf = mdns->mdns_y_shp_r_b_npv_array;
		break;
	case TISP_PARAM_MDSN_Y_SHP_R_C_NPV_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_MDSN_Y_SHP_R_C_NPV_ARRAY);
		tmpbuf = mdns->mdns_y_shp_r_c_npv_array;
		break;
	case TISP_PARAM_MDSN_Y_SHP_R_D_NPV_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_MDSN_Y_SHP_R_D_NPV_ARRAY);
		tmpbuf = mdns->mdns_y_shp_r_d_npv_array;
		break;
	case TISP_PARAM_MDSN_Y_SHP_R_E_NPV_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_MDSN_Y_SHP_R_E_NPV_ARRAY);
		tmpbuf = mdns->mdns_y_shp_r_e_npv_array;
		break;
	case TISP_PARAM_MDSN_Y_SHP_R_F_NPV_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_MDSN_Y_SHP_R_F_NPV_ARRAY);
		tmpbuf = mdns->mdns_y_shp_r_f_npv_array;
		break;
	case TISP_PARAM_MDSN_Y_ADJ_CNR_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_MDSN_Y_ADJ_CNR_ARRAY);
		tmpbuf = mdns->mdns_y_adj_cnr_array;
		break;
	case TISP_PARAM_MDSN_Y_FLUCT_LMT_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_MDSN_Y_FLUCT_LMT_ARRAY);
		tmpbuf = mdns->mdns_y_fluct_lmt_array;
		break;
	case TISP_PARAM_MDSN_Y_ADJ_STA_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_MDSN_Y_ADJ_STA_ARRAY);
		tmpbuf = mdns->mdns_y_adj_sta_array;
		break;
	case TISP_PARAM_MDSN_Y_ADJ_TEDG_S_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_MDSN_Y_ADJ_TEDG_S_ARRAY);
		tmpbuf = mdns->mdns_y_adj_tedg_s_array;
		break;
	case TISP_PARAM_MDSN_Y_ADJ_WEDG_S_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_MDSN_Y_ADJ_WEDG_S_ARRAY);
		tmpbuf = mdns->mdns_y_adj_wedg_s_array;
		break;
	case TISP_PARAM_MDSN_Y_ADJ_TEDG_N_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_MDSN_Y_ADJ_TEDG_N_ARRAY);
		tmpbuf = mdns->mdns_y_adj_tedg_n_array;
		break;
	case TISP_PARAM_MDSN_Y_ADJ_WEDG_N_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_MDSN_Y_ADJ_WEDG_N_ARRAY);
		tmpbuf = mdns->mdns_y_adj_wedg_n_array;
		break;
	case TISP_PARAM_MDSN_Y_ADJ_LUM_WIN_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_MDSN_Y_ADJ_LUM_WIN_ARRAY);
		tmpbuf = mdns->mdns_y_adj_lum_win_array;
		break;
	case TISP_PARAM_MDSN_Y_ADJ_TLUM_S_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_MDSN_Y_ADJ_TLUM_S_ARRAY);
		tmpbuf = mdns->mdns_y_adj_tlum_s_array;
		break;
	case TISP_PARAM_MDSN_Y_ADJ_WLUM_S_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_MDSN_Y_ADJ_WLUM_S_ARRAY);
		tmpbuf = mdns->mdns_y_adj_wlum_s_array;
		break;
	case TISP_PARAM_MDSN_Y_ADJ_TLUM_N_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_MDSN_Y_ADJ_TLUM_N_ARRAY);
		tmpbuf = mdns->mdns_y_adj_tlum_n_array;
		break;
	case TISP_PARAM_MDSN_Y_ADJ_WLUM_N_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_MDSN_Y_ADJ_WLUM_N_ARRAY);
		tmpbuf = mdns->mdns_y_adj_wlum_n_array;
		break;
	case TISP_PARAM_MDSN_C_SAD_THRES_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_MDSN_C_SAD_THRES_ARRAY);
		tmpbuf = mdns->mdns_c_sad_thres_array;
		break;
	case TISP_PARAM_MDSN_C_SAD_WIN_OPT_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_MDSN_C_SAD_WIN_OPT_ARRAY);
		tmpbuf = mdns->mdns_c_sad_win_opt_array;
		break;
	case TISP_PARAM_MDSN_C_SAD_STREN_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_MDSN_C_SAD_STREN_ARRAY);
		tmpbuf = mdns->mdns_c_sad_stren_array;
		break;
	case TISP_PARAM_MDSN_C_STA_STREN_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_MDSN_C_STA_STREN_ARRAY);
		tmpbuf = mdns->mdns_c_sta_stren_array;
		break;
	case TISP_PARAM_MDSN_C_PBT_STREN_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_MDSN_C_PBT_STREN_ARRAY);
		tmpbuf = mdns->mdns_c_pbt_stren_array;
		break;
	case TISP_PARAM_MDSN_C_SAD_NPV_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_MDSN_C_SAD_NPV_ARRAY);
		tmpbuf = mdns->mdns_c_sad_npv_array;
		break;
	case TISP_PARAM_MDSN_C_STA_NPV_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_MDSN_C_STA_NPV_ARRAY);
		tmpbuf = mdns->mdns_c_sta_npv_array;
		break;
	case TISP_PARAM_MDSN_C_PBT_NPV_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_MDSN_C_PBT_NPV_ARRAY);
		tmpbuf = mdns->mdns_c_pbt_npv_array;
		break;
	case TISP_PARAM_MDSN_C_REF_WEI_MIN_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_MDSN_C_REF_WEI_MIN_ARRAY);
		tmpbuf = mdns->mdns_c_ref_wei_min_array;
		break;
	case TISP_PARAM_MDSN_C_REF_WEI_MAX_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_MDSN_C_REF_WEI_MAX_ARRAY);
		tmpbuf = mdns->mdns_c_ref_wei_max_array;
		break;
	case TISP_PARAM_MDSN_C_CUR_BLUR_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_MDSN_C_CUR_BLUR_ARRAY);
		tmpbuf = mdns->mdns_c_cur_blur_array;
		break;
	case TISP_PARAM_MDSN_C_REF_BLUR_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_MDSN_C_REF_BLUR_ARRAY);
		tmpbuf = mdns->mdns_c_ref_blur_array;
		break;
	case TISP_PARAM_MDSN_C_SMJ_THRES_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_MDSN_C_SMJ_THRES_ARRAY);
		tmpbuf = mdns->mdns_c_smj_thres_array;
		break;
	case TISP_PARAM_MDSN_C_SFLA_BOH_T_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_MDSN_C_SFLA_BOH_T_ARRAY);
		tmpbuf = mdns->mdns_c_sfla_boh_t_array;
		break;
	case TISP_PARAM_MDSN_C_SFLA_CUT_T_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_MDSN_C_SFLA_CUT_T_ARRAY);
		tmpbuf = mdns->mdns_c_sfla_cut_t_array;
		break;
	case TISP_PARAM_MDSN_C_SFLA_BOH_S_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_MDSN_C_SFLA_BOH_S_ARRAY);
		tmpbuf = mdns->mdns_c_sfla_boh_s_array;
		break;
	case TISP_PARAM_MDSN_C_SFLA_CUT_S_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_MDSN_C_SFLA_CUT_S_ARRAY);
		tmpbuf = mdns->mdns_c_sfla_cut_s_array;
		break;
	case TISP_PARAM_MDSN_C_MFLA_BOH_T_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_MDSN_C_MFLA_BOH_T_ARRAY);
		tmpbuf = mdns->mdns_c_mfla_boh_t_array;
		break;
	case TISP_PARAM_MDSN_C_MFLA_CUT_T_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_MDSN_C_MFLA_CUT_T_ARRAY);
		tmpbuf = mdns->mdns_c_mfla_cut_t_array;
		break;
	case TISP_PARAM_MDSN_C_MFLA_BOH_S_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_MDSN_C_MFLA_BOH_S_ARRAY);
		tmpbuf = mdns->mdns_c_mfla_boh_s_array;
		break;
	case TISP_PARAM_MDSN_C_MFLA_CUT_S_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_MDSN_C_MFLA_CUT_S_ARRAY);
		tmpbuf = mdns->mdns_c_mfla_cut_s_array;
		break;
	case TISP_PARAM_MDSN_C_ADJ_CNR_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_MDSN_C_ADJ_CNR_ARRAY);
		tmpbuf = mdns->mdns_c_adj_cnr_array;
		break;
	case TISP_PARAM_MDSN_C_ADJ_STA_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_MDSN_C_ADJ_STA_ARRAY);
		tmpbuf = mdns->mdns_c_adj_sta_array;
		break;
	case TISP_PARAM_MDSN_C_ADJ_TCRM_S_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_MDSN_C_ADJ_TCRM_S_ARRAY);
		tmpbuf = mdns->mdns_c_adj_tcrm_s_array;
		break;
	case TISP_PARAM_MDSN_C_ADJ_WCRM_S_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_MDSN_C_ADJ_WCRM_S_ARRAY);
		tmpbuf = mdns->mdns_c_adj_wcrm_s_array;
		break;
	case TISP_PARAM_MDSN_C_ADJ_TCRM_N_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_MDSN_C_ADJ_TCRM_N_ARRAY);
		tmpbuf = mdns->mdns_c_adj_tcrm_n_array;
		break;
	case TISP_PARAM_MDSN_C_ADJ_WCRM_N_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_MDSN_C_ADJ_WCRM_N_ARRAY);
		tmpbuf = mdns->mdns_c_adj_wcrm_n_array;
		break;
	default:
		ISP_ERROR("%s,%d: mdns not support param id %d\n", __func__, __LINE__, id);
		return -1;
	}

	memcpy(buf, tmpbuf, len);
	*plen = len;

	return 0;
}

int32_t tisp_mdns_param_array_set(tisp_mdns_t *mdns, int32_t id, void *buf, uint32_t *plen)
{
	uint32_t len;
	void *tmpbuf;

	switch (id) {
	case TISP_PARAM_MDNS_TOP_FUNC_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_MDNS_TOP_FUNC_ARRAY);
		tmpbuf = mdns->mdns_top_func_array;
		memcpy(tmpbuf, buf, len);
		break;
	case TISP_PARAM_MDSN_STA_SIZE_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_MDSN_STA_SIZE_ARRAY);
		tmpbuf = mdns->mdns_sta_size_array;
		memcpy(tmpbuf, buf, len);
		break;
	case TISP_PARAM_MDSN_PBT_SIZE_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_MDSN_PBT_SIZE_ARRAY);
		tmpbuf = mdns->mdns_pbt_size_array;
		memcpy(tmpbuf, buf, len);
		break;
	case TISP_PARAM_MDSN_PBT_PONIT_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_MDSN_PBT_PONIT_ARRAY);
		tmpbuf = mdns->mdns_pbt_ponit_array;
		memcpy(tmpbuf, buf, len);
		break;
	case TISP_PARAM_MDSN_Y_SAD_THRES_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_MDSN_Y_SAD_THRES_ARRAY);
		tmpbuf = mdns->mdns_y_sad_thres_array;
		memcpy(tmpbuf, buf, len);
		break;
	case TISP_PARAM_MDSN_Y_STA_THRES_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_MDSN_Y_STA_THRES_ARRAY);
		tmpbuf = mdns->mdns_y_sta_thres_array;
		memcpy(tmpbuf, buf, len);
		break;
	case TISP_PARAM_MDSN_Y_PBT_THRES_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_MDSN_Y_PBT_THRES_ARRAY);
		tmpbuf = mdns->mdns_y_pbt_thres_array;
		memcpy(tmpbuf, buf, len);
		break;
	case TISP_PARAM_MDSN_Y_SAD_STREN_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_MDSN_Y_SAD_STREN_ARRAY);
		tmpbuf = mdns->mdns_y_sad_stren_array;
		memcpy(tmpbuf, buf, len);
		break;
	case TISP_PARAM_MDSN_Y_STA_STREN_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_MDSN_Y_STA_STREN_ARRAY);
		tmpbuf = mdns->mdns_y_sta_stren_array;
		memcpy(tmpbuf, buf, len);
		break;
	case TISP_PARAM_MDSN_Y_PBT_STREN_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_MDSN_Y_PBT_STREN_ARRAY);
		tmpbuf = mdns->mdns_y_pbt_stren_array;
		memcpy(tmpbuf, buf, len);
		break;
	case TISP_PARAM_MDSN_Y_SAD_WIN_OPT_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_MDSN_Y_SAD_WIN_OPT_ARRAY);
		tmpbuf = mdns->mdns_y_sad_win_opt_array;
		memcpy(tmpbuf, buf, len);
		break;
	case TISP_PARAM_MDSN_Y_STA_WIN_OPT_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_MDSN_Y_STA_WIN_OPT_ARRAY);
		tmpbuf = mdns->mdns_y_sta_win_opt_array;
		memcpy(tmpbuf, buf, len);
		break;
	case TISP_PARAM_MDSN_Y_PBT_WIN_OPT_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_MDSN_Y_PBT_WIN_OPT_ARRAY);
		tmpbuf = mdns->mdns_y_pbt_win_opt_array;
		memcpy(tmpbuf, buf, len);
		break;
	case TISP_PARAM_MDSN_Y_SAD_WIN_WEI_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_MDSN_Y_SAD_WIN_WEI_ARRAY);
		tmpbuf = mdns->mdns_y_sad_win_wei_array;
		memcpy(tmpbuf, buf, len);
		break;
	case TISP_PARAM_MDSN_Y_STA_WIN_WEI_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_MDSN_Y_STA_WIN_WEI_ARRAY);
		tmpbuf = mdns->mdns_y_sta_win_wei_array;
		memcpy(tmpbuf, buf, len);
		break;
	case TISP_PARAM_MDSN_Y_PBT_WIn_WEI_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_MDSN_Y_PBT_WIn_WEI_ARRAY);
		tmpbuf = mdns->mdns_y_pbt_win_wei_array;
		memcpy(tmpbuf, buf, len);
		break;
	case TISP_PARAM_MDSN_Y_STA_MV_NUM_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_MDSN_Y_STA_MV_NUM_ARRAY);
		tmpbuf = mdns->mdns_y_sta_mv_num_array;
		memcpy(tmpbuf, buf, len);
		break;
	case TISP_PARAM_MDSN_Y_PBT_MV_NUM_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_MDSN_Y_PBT_MV_NUM_ARRAY);
		tmpbuf = mdns->mdns_y_pbt_mv_num_array;
		memcpy(tmpbuf, buf, len);
		break;
	case TISP_PARAM_MDSN_Y_STA_MX_NUM_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_MDSN_Y_STA_MX_NUM_ARRAY);
		tmpbuf = mdns->mdns_y_sta_mx_num_array;
		memcpy(tmpbuf, buf, len);
		break;
	case TISP_PARAM_MDSN_Y_PBT_MX_NUM_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_MDSN_Y_PBT_MX_NUM_ARRAY);
		tmpbuf = mdns->mdns_y_pbt_mx_num_array;
		memcpy(tmpbuf, buf, len);
		break;
	case TISP_PARAM_MDSN_Y_SAD_NPV_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_MDSN_Y_SAD_NPV_ARRAY);
		tmpbuf = mdns->mdns_y_sad_npv_array;
		memcpy(tmpbuf, buf, len);
		break;
	case TISP_PARAM_MDSN_Y_STA_NPV_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_MDSN_Y_STA_NPV_ARRAY);
		tmpbuf = mdns->mdns_y_sta_npv_array;
		memcpy(tmpbuf, buf, len);
		break;
	case TISP_PARAM_MDSN_Y_PBT_NPV_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_MDSN_Y_PBT_NPV_ARRAY);
		tmpbuf = mdns->mdns_y_pbt_npv_array;
		memcpy(tmpbuf, buf, len);
		break;
	case TISP_PARAM_MDSN_Y_REF_WEI_MIN_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_MDSN_Y_REF_WEI_MIN_ARRAY);
		tmpbuf = mdns->mdns_y_ref_wei_min_array;
		memcpy(tmpbuf, buf, len);
		tisp_mdns_all_reg_refresh(mdns, gain_old + 0x200);
		break;
	case TISP_PARAM_MDSN_Y_REF_WEI_MAX_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_MDSN_Y_REF_WEI_MAX_ARRAY);
		tmpbuf = mdns->mdns_y_ref_wei_max_array;
		memcpy(tmpbuf, buf, len);
		break;
	case TISP_PARAM_MDSN_Y_EDGE_TYPE_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_MDSN_Y_EDGE_TYPE_ARRAY);
		tmpbuf = mdns->mdns_y_edge_type_array;
		memcpy(tmpbuf, buf, len);
		break;
	case TISP_PARAM_MDSN_Y_EDGE_WIN_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_MDSN_Y_EDGE_WIN_ARRAY);
		tmpbuf = mdns->mdns_y_edge_win_array;
		memcpy(tmpbuf, buf, len);
		break;
	case TISP_PARAM_MDSN_Y_BI_THRES_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_MDSN_Y_BI_THRES_ARRAY);
		tmpbuf = mdns->mdns_y_bi_thres_array;
		memcpy(tmpbuf, buf, len);
		break;
	case TISP_PARAM_MDSN_Y_SMJ_THRES_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_MDSN_Y_SMJ_THRES_ARRAY);
		tmpbuf = mdns->mdns_y_smj_thres_array;
		memcpy(tmpbuf, buf, len);
		break;
	case TISP_PARAM_MDSN_Y_SHP_SWEI_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_MDSN_Y_SHP_SWEI_ARRAY);
		tmpbuf = mdns->mdns_y_shp_swei_array;
		memcpy(tmpbuf, buf, len);
		break;
	case TISP_PARAM_MDSN_Y_SHP_MWEI_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_MDSN_Y_SHP_MWEI_ARRAY);
		tmpbuf = mdns->mdns_y_shp_mwei_array;
		memcpy(tmpbuf, buf, len);
		break;
	case TISP_PARAM_MDSN_Y_CS_BHOLD_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_MDSN_Y_CS_BHOLD_ARRAY);
		tmpbuf = mdns->mdns_y_cs_bhold_array;
		memcpy(tmpbuf, buf, len);
		break;
	case TISP_PARAM_MDSN_Y_CM_BHOLD_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_MDSN_Y_CM_BHOLD_ARRAY);
		tmpbuf = mdns->mdns_y_cm_bhold_array;
		memcpy(tmpbuf, buf, len);
		break;
	case TISP_PARAM_MDSN_Y_RS_BHOLD_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_MDSN_Y_RS_BHOLD_ARRAY);
		tmpbuf = mdns->mdns_y_rs_bhold_array;
		memcpy(tmpbuf, buf, len);
		break;
	case TISP_PARAM_MDSN_Y_RM_BHOLD_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_MDSN_Y_RM_BHOLD_ARRAY);
		tmpbuf = mdns->mdns_y_rm_bhold_array;
		memcpy(tmpbuf, buf, len);
		break;
	case TISP_PARAM_MDSN_Y_CS_BWIN_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_MDSN_Y_CS_BWIN_ARRAY);
		tmpbuf = mdns->mdns_y_cs_bwin_array;
		memcpy(tmpbuf, buf, len);
		break;
	case TISP_PARAM_MDSN_Y_CM_BWIN_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_MDSN_Y_CM_BWIN_ARRAY);
		tmpbuf = mdns->mdns_y_cm_bwin_array;
		memcpy(tmpbuf, buf, len);
		break;
	case TISP_PARAM_MDSN_Y_RS_BWIN_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_MDSN_Y_RS_BWIN_ARRAY);
		tmpbuf = mdns->mdns_y_rs_bwin_array;
		memcpy(tmpbuf, buf, len);
		break;
	case TISP_PARAM_MDSN_Y_RM_BWIN_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_MDSN_Y_RM_BWIN_ARRAY);
		tmpbuf = mdns->mdns_y_rm_bwin_array;
		memcpy(tmpbuf, buf, len);
		break;
	case TISP_PARAM_MDSN_Y_CS_IWIN_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_MDSN_Y_CS_IWIN_ARRAY);
		tmpbuf = mdns->mdns_y_cs_iwin_array;
		memcpy(tmpbuf, buf, len);
		break;
	case TISP_PARAM_MDSN_Y_CM_IWIN_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_MDSN_Y_CM_IWIN_ARRAY);
		tmpbuf = mdns->mdns_y_cm_iwin_array;
		memcpy(tmpbuf, buf, len);
		break;
	case TISP_PARAM_MDSN_Y_RS_IWIN_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_MDSN_Y_RS_IWIN_ARRAY);
		tmpbuf = mdns->mdns_y_rs_iwin_array;
		memcpy(tmpbuf, buf, len);
		break;
	case TISP_PARAM_MDSN_Y_RM_IWIN_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_MDSN_Y_RM_IWIN_ARRAY);
		tmpbuf = mdns->mdns_y_rm_iwin_array;
		memcpy(tmpbuf, buf, len);
		break;
	case TISP_PARAM_MDSN_Y_CS_EWIN_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_MDSN_Y_CS_EWIN_ARRAY);
		tmpbuf = mdns->mdns_y_cs_ewin_array;
		memcpy(tmpbuf, buf, len);
		break;
	case TISP_PARAM_MDSN_Y_CM_EWIN_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_MDSN_Y_CM_EWIN_ARRAY);
		tmpbuf = mdns->mdns_y_cm_ewin_array;
		memcpy(tmpbuf, buf, len);
		break;
	case TISP_PARAM_MDSN_Y_RS_EWIN_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_MDSN_Y_RS_EWIN_ARRAY);
		tmpbuf = mdns->mdns_y_rs_ewin_array;
		memcpy(tmpbuf, buf, len);
		break;
	case TISP_PARAM_MDSN_Y_RM_EWIN_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_MDSN_Y_RM_EWIN_ARRAY);
		tmpbuf = mdns->mdns_y_rm_ewin_array;
		memcpy(tmpbuf, buf, len);
		break;
	case TISP_PARAM_MDSN_Y_CS_SEGO_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_MDSN_Y_CS_SEGO_ARRAY);
		tmpbuf = mdns->mdns_y_cs_sego_array;
		memcpy(tmpbuf, buf, len);
		break;
	case TISP_PARAM_MDSN_Y_CM_SEGO_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_MDSN_Y_CM_SEGO_ARRAY);
		tmpbuf = mdns->mdns_y_cm_sego_array;
		memcpy(tmpbuf, buf, len);
		break;
	case TISP_PARAM_MDSN_Y_RS_SEGO_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_MDSN_Y_RS_SEGO_ARRAY);
		tmpbuf = mdns->mdns_y_rs_sego_array;
		memcpy(tmpbuf, buf, len);
		break;
	case TISP_PARAM_MDSN_Y_RM_SEGO_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_MDSN_Y_RM_SEGO_ARRAY);
		tmpbuf = mdns->mdns_y_rm_sego_array;
		memcpy(tmpbuf, buf, len);
		break;
	case TISP_PARAM_MDSN_Y_CS_STREN_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_MDSN_Y_CS_STREN_ARRAY);
		tmpbuf = mdns->mdns_y_cs_stren_array;
		memcpy(tmpbuf, buf, len);
		break;
	case TISP_PARAM_MDSN_Y_CM_STREN_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_MDSN_Y_CM_STREN_ARRAY);
		tmpbuf = mdns->mdns_y_cm_stren_array;
		memcpy(tmpbuf, buf, len);
		break;
	case TISP_PARAM_MDSN_Y_RS_STREN_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_MDSN_Y_RS_STREN_ARRAY);
		tmpbuf = mdns->mdns_y_rs_stren_array;
		memcpy(tmpbuf, buf, len);
		break;
	case TISP_PARAM_MDSN_Y_RM_STREN_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_MDSN_Y_RM_STREN_ARRAY);
		tmpbuf = mdns->mdns_y_rm_stren_array;
		memcpy(tmpbuf, buf, len);
		break;
	case TISP_PARAM_MDSN_Y_CS_NPV_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_MDSN_Y_CS_NPV_ARRAY);
		tmpbuf = mdns->mdns_y_cs_npv_array;
		memcpy(tmpbuf, buf, len);
		break;
	case TISP_PARAM_MDSN_Y_CM_NPV_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_MDSN_Y_CM_NPV_ARRAY);
		tmpbuf = mdns->mdns_y_cm_npv_array;
		memcpy(tmpbuf, buf, len);
		break;
	case TISP_PARAM_MDSN_Y_RS_NPV_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_MDSN_Y_RS_NPV_ARRAY);
		tmpbuf = mdns->mdns_y_rs_npv_array;
		memcpy(tmpbuf, buf, len);
		tisp_mdns_all_reg_refresh(mdns, gain_old + 0x200);
		break;
	case TISP_PARAM_MDSN_Y_RM_NPV_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_MDSN_Y_RM_NPV_ARRAY);
		tmpbuf = mdns->mdns_y_rm_npv_array;
		memcpy(tmpbuf, buf, len);
		tisp_mdns_all_reg_refresh(mdns, gain_old + 0x200);
		break;
	case TISP_PARAM_MDSN_Y_SHP_C_0_NPV_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_MDSN_Y_SHP_C_0_NPV_ARRAY);
		tmpbuf = mdns->mdns_y_shp_c_0_npv_array;
		memcpy(tmpbuf, buf, len);
		tisp_mdns_all_reg_refresh(mdns, gain_old + 0x200);
		break;
	case TISP_PARAM_MDSN_Y_SHP_C_1_NPV_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_MDSN_Y_SHP_C_1_NPV_ARRAY);
		tmpbuf = mdns->mdns_y_shp_c_1_npv_array;
		memcpy(tmpbuf, buf, len);
		break;
	case TISP_PARAM_MDSN_Y_SHP_C_2_NPV_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_MDSN_Y_SHP_C_2_NPV_ARRAY);
		tmpbuf = mdns->mdns_y_shp_c_2_npv_array;
		memcpy(tmpbuf, buf, len);
		break;
	case TISP_PARAM_MDSN_Y_SHP_C_3_NPV_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_MDSN_Y_SHP_C_3_NPV_ARRAY);
		tmpbuf = mdns->mdns_y_shp_c_3_npv_array;
		memcpy(tmpbuf, buf, len);
		break;
	case TISP_PARAM_MDSN_Y_SHP_C_4_NPV_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_MDSN_Y_SHP_C_4_NPV_ARRAY);
		tmpbuf = mdns->mdns_y_shp_c_4_npv_array;
		memcpy(tmpbuf, buf, len);
		break;
	case TISP_PARAM_MDSN_Y_SHP_C_5_NPV_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_MDSN_Y_SHP_C_5_NPV_ARRAY);
		tmpbuf = mdns->mdns_y_shp_c_5_npv_array;
		memcpy(tmpbuf, buf, len);
		break;
	case TISP_PARAM_MDSN_Y_SHP_C_6_NPV_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_MDSN_Y_SHP_C_6_NPV_ARRAY);
		tmpbuf = mdns->mdns_y_shp_c_6_npv_array;
		memcpy(tmpbuf, buf, len);
		break;
	case TISP_PARAM_MDSN_Y_SHP_C_7_NPV_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_MDSN_Y_SHP_C_7_NPV_ARRAY);
		tmpbuf = mdns->mdns_y_shp_c_7_npv_array;
		memcpy(tmpbuf, buf, len);
		break;
	case TISP_PARAM_MDSN_Y_SHP_C_8_NPV_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_MDSN_Y_SHP_C_8_NPV_ARRAY);
		tmpbuf = mdns->mdns_y_shp_c_8_npv_array;
		memcpy(tmpbuf, buf, len);
		break;
	case TISP_PARAM_MDSN_Y_SHP_C_9_NPV_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_MDSN_Y_SHP_C_9_NPV_ARRAY);
		tmpbuf = mdns->mdns_y_shp_c_9_npv_array;
		memcpy(tmpbuf, buf, len);
		break;
	case TISP_PARAM_MDSN_Y_SHP_C_A_NPV_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_MDSN_Y_SHP_C_A_NPV_ARRAY);
		tmpbuf = mdns->mdns_y_shp_c_a_npv_array;
		memcpy(tmpbuf, buf, len);
		break;
	case TISP_PARAM_MDSN_Y_SHP_C_B_NPV_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_MDSN_Y_SHP_C_B_NPV_ARRAY);
		tmpbuf = mdns->mdns_y_shp_c_b_npv_array;
		memcpy(tmpbuf, buf, len);
		break;
	case TISP_PARAM_MDSN_Y_SHP_C_C_NPV_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_MDSN_Y_SHP_C_C_NPV_ARRAY);
		tmpbuf = mdns->mdns_y_shp_c_c_npv_array;
		memcpy(tmpbuf, buf, len);
		break;
	case TISP_PARAM_MDSN_Y_SHP_C_D_NPV_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_MDSN_Y_SHP_C_D_NPV_ARRAY);
		tmpbuf = mdns->mdns_y_shp_c_d_npv_array;
		memcpy(tmpbuf, buf, len);
		break;
	case TISP_PARAM_MDSN_Y_SHP_C_E_NPV_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_MDSN_Y_SHP_C_E_NPV_ARRAY);
		tmpbuf = mdns->mdns_y_shp_c_e_npv_array;
		memcpy(tmpbuf, buf, len);
		break;
	case TISP_PARAM_MDSN_Y_SHP_C_F_NPV_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_MDSN_Y_SHP_C_F_NPV_ARRAY);
		tmpbuf = mdns->mdns_y_shp_c_f_npv_array;
		memcpy(tmpbuf, buf, len);
		break;
	case TISP_PARAM_MDSN_Y_SHP_R_0_NPV_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_MDSN_Y_SHP_R_0_NPV_ARRAY);
		tmpbuf = mdns->mdns_y_shp_r_0_npv_array;
		memcpy(tmpbuf, buf, len);
		break;
	case TISP_PARAM_MDSN_Y_SHP_R_1_NPV_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_MDSN_Y_SHP_R_1_NPV_ARRAY);
		tmpbuf = mdns->mdns_y_shp_r_1_npv_array;
		memcpy(tmpbuf, buf, len);
		break;
	case TISP_PARAM_MDSN_Y_SHP_R_2_NPV_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_MDSN_Y_SHP_R_2_NPV_ARRAY);
		tmpbuf = mdns->mdns_y_shp_r_2_npv_array;
		memcpy(tmpbuf, buf, len);
		break;
	case TISP_PARAM_MDSN_Y_SHP_R_3_NPV_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_MDSN_Y_SHP_R_3_NPV_ARRAY);
		tmpbuf = mdns->mdns_y_shp_r_3_npv_array;
		memcpy(tmpbuf, buf, len);
		break;
	case TISP_PARAM_MDSN_Y_SHP_R_4_NPV_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_MDSN_Y_SHP_R_4_NPV_ARRAY);
		tmpbuf = mdns->mdns_y_shp_r_4_npv_array;
		memcpy(tmpbuf, buf, len);
		break;
	case TISP_PARAM_MDSN_Y_SHP_R_5_NPV_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_MDSN_Y_SHP_R_5_NPV_ARRAY);
		tmpbuf = mdns->mdns_y_shp_r_5_npv_array;
		memcpy(tmpbuf, buf, len);
		break;
	case TISP_PARAM_MDSN_Y_SHP_R_6_NPV_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_MDSN_Y_SHP_R_6_NPV_ARRAY);
		tmpbuf = mdns->mdns_y_shp_r_6_npv_array;
		memcpy(tmpbuf, buf, len);
		break;
	case TISP_PARAM_MDSN_Y_SHP_R_7_NPV_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_MDSN_Y_SHP_R_7_NPV_ARRAY);
		tmpbuf = mdns->mdns_y_shp_r_7_npv_array;
		memcpy(tmpbuf, buf, len);
		break;
	case TISP_PARAM_MDSN_Y_SHP_R_8_NPV_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_MDSN_Y_SHP_R_8_NPV_ARRAY);
		tmpbuf = mdns->mdns_y_shp_r_8_npv_array;
		memcpy(tmpbuf, buf, len);
		break;
	case TISP_PARAM_MDSN_Y_SHP_R_9_NPV_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_MDSN_Y_SHP_R_9_NPV_ARRAY);
		tmpbuf = mdns->mdns_y_shp_r_9_npv_array;
		memcpy(tmpbuf, buf, len);
		break;
	case TISP_PARAM_MDSN_Y_SHP_R_A_NPV_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_MDSN_Y_SHP_R_A_NPV_ARRAY);
		tmpbuf = mdns->mdns_y_shp_r_a_npv_array;
		memcpy(tmpbuf, buf, len);
		break;
	case TISP_PARAM_MDSN_Y_SHP_R_B_NPV_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_MDSN_Y_SHP_R_B_NPV_ARRAY);
		tmpbuf = mdns->mdns_y_shp_r_b_npv_array;
		memcpy(tmpbuf, buf, len);
		break;
	case TISP_PARAM_MDSN_Y_SHP_R_C_NPV_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_MDSN_Y_SHP_R_C_NPV_ARRAY);
		tmpbuf = mdns->mdns_y_shp_r_c_npv_array;
		memcpy(tmpbuf, buf, len);
		break;
	case TISP_PARAM_MDSN_Y_SHP_R_D_NPV_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_MDSN_Y_SHP_R_D_NPV_ARRAY);
		tmpbuf = mdns->mdns_y_shp_r_d_npv_array;
		memcpy(tmpbuf, buf, len);
		break;
	case TISP_PARAM_MDSN_Y_SHP_R_E_NPV_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_MDSN_Y_SHP_R_E_NPV_ARRAY);
		tmpbuf = mdns->mdns_y_shp_r_e_npv_array;
		memcpy(tmpbuf, buf, len);
		break;
	case TISP_PARAM_MDSN_Y_SHP_R_F_NPV_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_MDSN_Y_SHP_R_F_NPV_ARRAY);
		tmpbuf = mdns->mdns_y_shp_r_f_npv_array;
		memcpy(tmpbuf, buf, len);
		break;
	case TISP_PARAM_MDSN_Y_ADJ_CNR_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_MDSN_Y_ADJ_CNR_ARRAY);
		tmpbuf = mdns->mdns_y_adj_cnr_array;
		memcpy(tmpbuf, buf, len);
		break;
	case TISP_PARAM_MDSN_Y_FLUCT_LMT_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_MDSN_Y_FLUCT_LMT_ARRAY);
		tmpbuf = mdns->mdns_y_fluct_lmt_array;
		memcpy(tmpbuf, buf, len);
		break;
	case TISP_PARAM_MDSN_Y_ADJ_STA_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_MDSN_Y_ADJ_STA_ARRAY);
		tmpbuf = mdns->mdns_y_adj_sta_array;
		memcpy(tmpbuf, buf, len);
		break;
	case TISP_PARAM_MDSN_Y_ADJ_TEDG_S_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_MDSN_Y_ADJ_TEDG_S_ARRAY);
		tmpbuf = mdns->mdns_y_adj_tedg_s_array;
		memcpy(tmpbuf, buf, len);
		break;
	case TISP_PARAM_MDSN_Y_ADJ_WEDG_S_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_MDSN_Y_ADJ_WEDG_S_ARRAY);
		tmpbuf = mdns->mdns_y_adj_wedg_s_array;
		memcpy(tmpbuf, buf, len);
		break;
	case TISP_PARAM_MDSN_Y_ADJ_TEDG_N_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_MDSN_Y_ADJ_TEDG_N_ARRAY);
		tmpbuf = mdns->mdns_y_adj_tedg_n_array;
		memcpy(tmpbuf, buf, len);
		break;
	case TISP_PARAM_MDSN_Y_ADJ_WEDG_N_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_MDSN_Y_ADJ_WEDG_N_ARRAY);
		tmpbuf = mdns->mdns_y_adj_wedg_n_array;
		memcpy(tmpbuf, buf, len);
		break;
	case TISP_PARAM_MDSN_Y_ADJ_LUM_WIN_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_MDSN_Y_ADJ_LUM_WIN_ARRAY);
		tmpbuf = mdns->mdns_y_adj_lum_win_array;
		memcpy(tmpbuf, buf, len);
		break;
	case TISP_PARAM_MDSN_Y_ADJ_TLUM_S_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_MDSN_Y_ADJ_TLUM_S_ARRAY);
		tmpbuf = mdns->mdns_y_adj_tlum_s_array;
		memcpy(tmpbuf, buf, len);
		break;
	case TISP_PARAM_MDSN_Y_ADJ_WLUM_S_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_MDSN_Y_ADJ_WLUM_S_ARRAY);
		tmpbuf = mdns->mdns_y_adj_wlum_s_array;
		memcpy(tmpbuf, buf, len);
		break;
	case TISP_PARAM_MDSN_Y_ADJ_TLUM_N_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_MDSN_Y_ADJ_TLUM_N_ARRAY);
		tmpbuf = mdns->mdns_y_adj_tlum_n_array;
		memcpy(tmpbuf, buf, len);
		break;
	case TISP_PARAM_MDSN_Y_ADJ_WLUM_N_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_MDSN_Y_ADJ_WLUM_N_ARRAY);
		tmpbuf = mdns->mdns_y_adj_wlum_n_array;
		memcpy(tmpbuf, buf, len);
		break;
	case TISP_PARAM_MDSN_C_SAD_THRES_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_MDSN_C_SAD_THRES_ARRAY);
		tmpbuf = mdns->mdns_c_sad_thres_array;
		memcpy(tmpbuf, buf, len);
		break;
	case TISP_PARAM_MDSN_C_SAD_WIN_OPT_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_MDSN_C_SAD_WIN_OPT_ARRAY);
		tmpbuf = mdns->mdns_c_sad_win_opt_array;
		memcpy(tmpbuf, buf, len);
		break;
	case TISP_PARAM_MDSN_C_SAD_STREN_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_MDSN_C_SAD_STREN_ARRAY);
		tmpbuf = mdns->mdns_c_sad_stren_array;
		memcpy(tmpbuf, buf, len);
		break;
	case TISP_PARAM_MDSN_C_STA_STREN_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_MDSN_C_STA_STREN_ARRAY);
		tmpbuf = mdns->mdns_c_sta_stren_array;
		memcpy(tmpbuf, buf, len);
		break;
	case TISP_PARAM_MDSN_C_PBT_STREN_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_MDSN_C_PBT_STREN_ARRAY);
		tmpbuf = mdns->mdns_c_pbt_stren_array;
		memcpy(tmpbuf, buf, len);
		break;
	case TISP_PARAM_MDSN_C_SAD_NPV_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_MDSN_C_SAD_NPV_ARRAY);
		tmpbuf = mdns->mdns_c_sad_npv_array;
		memcpy(tmpbuf, buf, len);
		break;
	case TISP_PARAM_MDSN_C_STA_NPV_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_MDSN_C_STA_NPV_ARRAY);
		tmpbuf = mdns->mdns_c_sta_npv_array;
		memcpy(tmpbuf, buf, len);
		break;
	case TISP_PARAM_MDSN_C_PBT_NPV_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_MDSN_C_PBT_NPV_ARRAY);
		tmpbuf = mdns->mdns_c_pbt_npv_array;
		memcpy(tmpbuf, buf, len);
		break;
	case TISP_PARAM_MDSN_C_REF_WEI_MIN_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_MDSN_C_REF_WEI_MIN_ARRAY);
		tmpbuf = mdns->mdns_c_ref_wei_min_array;
		memcpy(tmpbuf, buf, len);
		break;
	case TISP_PARAM_MDSN_C_REF_WEI_MAX_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_MDSN_C_REF_WEI_MAX_ARRAY);
		tmpbuf = mdns->mdns_c_ref_wei_max_array;
		memcpy(tmpbuf, buf, len);
		break;
	case TISP_PARAM_MDSN_C_CUR_BLUR_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_MDSN_C_CUR_BLUR_ARRAY);
		tmpbuf = mdns->mdns_c_cur_blur_array;
		memcpy(tmpbuf, buf, len);
		break;
	case TISP_PARAM_MDSN_C_REF_BLUR_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_MDSN_C_REF_BLUR_ARRAY);
		tmpbuf = mdns->mdns_c_ref_blur_array;
		memcpy(tmpbuf, buf, len);
		break;
	case TISP_PARAM_MDSN_C_SMJ_THRES_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_MDSN_C_SMJ_THRES_ARRAY);
		tmpbuf = mdns->mdns_c_smj_thres_array;
		memcpy(tmpbuf, buf, len);
		break;
	case TISP_PARAM_MDSN_C_SFLA_BOH_T_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_MDSN_C_SFLA_BOH_T_ARRAY);
		tmpbuf = mdns->mdns_c_sfla_boh_t_array;
		memcpy(tmpbuf, buf, len);
		break;
	case TISP_PARAM_MDSN_C_SFLA_CUT_T_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_MDSN_C_SFLA_CUT_T_ARRAY);
		tmpbuf = mdns->mdns_c_sfla_cut_t_array;
		memcpy(tmpbuf, buf, len);
		break;
	case TISP_PARAM_MDSN_C_SFLA_BOH_S_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_MDSN_C_SFLA_BOH_S_ARRAY);
		tmpbuf = mdns->mdns_c_sfla_boh_s_array;
		memcpy(tmpbuf, buf, len);
		break;
	case TISP_PARAM_MDSN_C_SFLA_CUT_S_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_MDSN_C_SFLA_CUT_S_ARRAY);
		tmpbuf = mdns->mdns_c_sfla_cut_s_array;
		memcpy(tmpbuf, buf, len);
		break;
	case TISP_PARAM_MDSN_C_MFLA_BOH_T_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_MDSN_C_MFLA_BOH_T_ARRAY);
		tmpbuf = mdns->mdns_c_mfla_boh_t_array;
		memcpy(tmpbuf, buf, len);
		break;
	case TISP_PARAM_MDSN_C_MFLA_CUT_T_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_MDSN_C_MFLA_CUT_T_ARRAY);
		tmpbuf = mdns->mdns_c_mfla_cut_t_array;
		memcpy(tmpbuf, buf, len);
		break;
	case TISP_PARAM_MDSN_C_MFLA_BOH_S_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_MDSN_C_MFLA_BOH_S_ARRAY);
		tmpbuf = mdns->mdns_c_mfla_boh_s_array;
		memcpy(tmpbuf, buf, len);
		break;
	case TISP_PARAM_MDSN_C_MFLA_CUT_S_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_MDSN_C_MFLA_CUT_S_ARRAY);
		tmpbuf = mdns->mdns_c_mfla_cut_s_array;
		memcpy(tmpbuf, buf, len);
		break;
	case TISP_PARAM_MDSN_C_ADJ_CNR_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_MDSN_C_ADJ_CNR_ARRAY);
		tmpbuf = mdns->mdns_c_adj_cnr_array;
		memcpy(tmpbuf, buf, len);
		break;
	case TISP_PARAM_MDSN_C_ADJ_STA_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_MDSN_C_ADJ_STA_ARRAY);
		tmpbuf = mdns->mdns_c_adj_sta_array;
		memcpy(tmpbuf, buf, len);
		break;
	case TISP_PARAM_MDSN_C_ADJ_TCRM_S_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_MDSN_C_ADJ_TCRM_S_ARRAY);
		tmpbuf = mdns->mdns_c_adj_tcrm_s_array;
		memcpy(tmpbuf, buf, len);
		break;
	case TISP_PARAM_MDSN_C_ADJ_WCRM_S_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_MDSN_C_ADJ_WCRM_S_ARRAY);
		tmpbuf = mdns->mdns_c_adj_wcrm_s_array;
		memcpy(tmpbuf, buf, len);
		break;
	case TISP_PARAM_MDSN_C_ADJ_TCRM_N_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_MDSN_C_ADJ_TCRM_N_ARRAY);
		tmpbuf = mdns->mdns_c_adj_tcrm_n_array;
		memcpy(tmpbuf, buf, len);
		break;
	case TISP_PARAM_MDSN_C_ADJ_WCRM_N_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_MDSN_C_ADJ_WCRM_N_ARRAY);
		tmpbuf = mdns->mdns_c_adj_wcrm_n_array;
		memcpy(tmpbuf, buf, len);
		tisp_mdns_all_reg_refresh(mdns, gain_old + 0x200);
		break;
	default:
		ISP_ERROR("%s,%d: mdns not support param id %d\n", __func__, __LINE__, id);
		return -1;
	}

	return 0;
}
