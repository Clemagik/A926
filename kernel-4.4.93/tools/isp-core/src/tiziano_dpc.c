#include <linux/kernel.h>
#include <linux/irqreturn.h>
#include <linux/dma-mapping.h>

#include "fix_point_calc.h"
#include "../inc/tiziano_sys.h"
#include "tiziano_core.h"
#include "../inc/tiziano_isp.h"
#include "tiziano_dpc.h"
#include "tiziano_params.h"
#include "tiziano_map.h"


/* uu np value config */
int32_t tisp_ctr_md_np_cfg(tisp_dpc_t *dpc)
{
	uint32_t reg_value;

	reg_value = (dpc->ctr_md_np_array[3] << 24) |
		(dpc->ctr_md_np_array[2] << 16) |
		(dpc->ctr_md_np_array[1] << 8) |
		(dpc->ctr_md_np_array[0]);

	system_reg_write(((tisp_core_t *)dpc->core)->priv_data, DPC_ADDR_CTR_MD_NP_ARRAY_RG0, reg_value);

	reg_value = (dpc->ctr_md_np_array[7] << 24) |
		(dpc->ctr_md_np_array[6] << 16) |
		(dpc->ctr_md_np_array[5] << 8) |
		(dpc->ctr_md_np_array[4]);

	system_reg_write(((tisp_core_t *)dpc->core)->priv_data, DPC_ADDR_CTR_MD_NP_ARRAY_RG1, reg_value);

	reg_value = (dpc->ctr_md_np_array[11] << 24) |
		(dpc->ctr_md_np_array[10] << 16) |
		(dpc->ctr_md_np_array[9 ] << 8) |
		(dpc->ctr_md_np_array[8 ]);

	system_reg_write(((tisp_core_t *)dpc->core)->priv_data, DPC_ADDR_CTR_MD_NP_ARRAY_RG2, reg_value);

	reg_value = (dpc->ctr_md_np_array[15] << 24) |
		(dpc->ctr_md_np_array[14] << 16) |
		(dpc->ctr_md_np_array[13] << 8) |
		(dpc->ctr_md_np_array[12]);

	system_reg_write(((tisp_core_t *)dpc->core)->priv_data, DPC_ADDR_CTR_MD_NP_ARRAY_RG3, reg_value);

	return 0;
}

int32_t tisp_rdns_uu_np_cfg(tisp_dpc_t *dpc)
{
	uint32_t reg_value;

	reg_value = (dpc->rdns_uu_np_array[1] << 16) |
		(dpc->rdns_uu_np_array[0]);

	system_reg_write(((tisp_core_t *)dpc->core)->priv_data, DPC_ADDR_RDNS_STD_NP_ARRAY_RG0, reg_value);

	reg_value = (dpc->rdns_uu_np_array[3] << 16) |
		(dpc->rdns_uu_np_array[2]);

	system_reg_write(((tisp_core_t *)dpc->core)->priv_data, DPC_ADDR_RDNS_STD_NP_ARRAY_RG1, reg_value);

	reg_value = (dpc->rdns_uu_np_array[5] << 16) |
		(dpc->rdns_uu_np_array[4]);

	system_reg_write(((tisp_core_t *)dpc->core)->priv_data, DPC_ADDR_RDNS_STD_NP_ARRAY_RG2, reg_value);

	reg_value = (dpc->rdns_uu_np_array[7] << 16) |
		(dpc->rdns_uu_np_array[6]);

	system_reg_write(((tisp_core_t *)dpc->core)->priv_data, DPC_ADDR_RDNS_STD_NP_ARRAY_RG3, reg_value);

	reg_value = (dpc->rdns_uu_np_array[9] << 16) |
		(dpc->rdns_uu_np_array[8]);

	system_reg_write(((tisp_core_t *)dpc->core)->priv_data, DPC_ADDR_RDNS_STD_NP_ARRAY_RG4, reg_value);

	reg_value = (dpc->rdns_uu_np_array[11] << 16) |
		(dpc->rdns_uu_np_array[10]);

	system_reg_write(((tisp_core_t *)dpc->core)->priv_data, DPC_ADDR_RDNS_STD_NP_ARRAY_RG5, reg_value);

	reg_value = (dpc->rdns_uu_np_array[13] << 16) |
		(dpc->rdns_uu_np_array[12]);

	system_reg_write(((tisp_core_t *)dpc->core)->priv_data, DPC_ADDR_RDNS_STD_NP_ARRAY_RG6, reg_value);

	reg_value = (dpc->rdns_uu_np_array[15] << 16) |
		(dpc->rdns_uu_np_array[14]);

	system_reg_write(((tisp_core_t *)dpc->core)->priv_data, DPC_ADDR_RDNS_STD_NP_ARRAY_RG7, reg_value);

	return 0;
}

int32_t tisp_rdns_g_lum_np_cfg(tisp_dpc_t *dpc)
{
	uint32_t reg_value;

	reg_value = (dpc->rdns_g_lum_np_array[1] << 16) |
		(dpc->rdns_g_lum_np_array[0]);
	system_reg_write(((tisp_core_t *)dpc->core)->priv_data, DPC_ADDR_RDNS_G_LUM_NP_ARRAY_RG0, reg_value);

	reg_value = (dpc->rdns_g_lum_np_array[3] << 16) |
		(dpc->rdns_g_lum_np_array[2]);
	system_reg_write(((tisp_core_t *)dpc->core)->priv_data, DPC_ADDR_RDNS_G_LUM_NP_ARRAY_RG1, reg_value);

	reg_value = (dpc->rdns_g_lum_np_array[5] << 16) |
		(dpc->rdns_g_lum_np_array[4]);
	system_reg_write(((tisp_core_t *)dpc->core)->priv_data, DPC_ADDR_RDNS_G_LUM_NP_ARRAY_RG2, reg_value);

	reg_value = (dpc->rdns_g_lum_np_array[7] << 16) |
		(dpc->rdns_g_lum_np_array[6]);
	system_reg_write(((tisp_core_t *)dpc->core)->priv_data, DPC_ADDR_RDNS_G_LUM_NP_ARRAY_RG3, reg_value);

	reg_value = (dpc->rdns_g_lum_np_array[9] << 16) |
		(dpc->rdns_g_lum_np_array[8]);
	system_reg_write(((tisp_core_t *)dpc->core)->priv_data, DPC_ADDR_RDNS_G_LUM_NP_ARRAY_RG4, reg_value);

	reg_value = (dpc->rdns_g_lum_np_array[11] << 16) |
		(dpc->rdns_g_lum_np_array[10]);
	system_reg_write(((tisp_core_t *)dpc->core)->priv_data, DPC_ADDR_RDNS_G_LUM_NP_ARRAY_RG5, reg_value);

	reg_value = (dpc->rdns_g_lum_np_array[13] << 16) |
		(dpc->rdns_g_lum_np_array[12]);
	system_reg_write(((tisp_core_t *)dpc->core)->priv_data, DPC_ADDR_RDNS_G_LUM_NP_ARRAY_RG6, reg_value);

	reg_value = (dpc->rdns_g_lum_np_array[15] << 16) |
		(dpc->rdns_g_lum_np_array[14]);
	system_reg_write(((tisp_core_t *)dpc->core)->priv_data, DPC_ADDR_RDNS_G_LUM_NP_ARRAY_RG7, reg_value);

	return 0;
}

int32_t tisp_rdns_g_std_np_cfg(tisp_dpc_t *dpc)
{
	uint32_t reg_value;

	reg_value = (dpc->rdns_g_std_np_array[1] << 16) |
		(dpc->rdns_g_std_np_array[0]);
	system_reg_write(((tisp_core_t *)dpc->core)->priv_data, DPC_ADDR_RDNS_G_STD_NP_ARRAY_RG0, reg_value);

	reg_value = (dpc->rdns_g_std_np_array[3] << 16) |
		(dpc->rdns_g_std_np_array[2]);
	system_reg_write(((tisp_core_t *)dpc->core)->priv_data, DPC_ADDR_RDNS_G_STD_NP_ARRAY_RG1, reg_value);

	reg_value = (dpc->rdns_g_std_np_array[5] << 16) |
		(dpc->rdns_g_std_np_array[4]);
	system_reg_write(((tisp_core_t *)dpc->core)->priv_data, DPC_ADDR_RDNS_G_STD_NP_ARRAY_RG2, reg_value);

	reg_value = (dpc->rdns_g_std_np_array[7] << 16) |
		(dpc->rdns_g_std_np_array[6]);
	system_reg_write(((tisp_core_t *)dpc->core)->priv_data, DPC_ADDR_RDNS_G_STD_NP_ARRAY_RG3, reg_value);

	reg_value = (dpc->rdns_g_std_np_array[9] << 16) |
		(dpc->rdns_g_std_np_array[8]);
	system_reg_write(((tisp_core_t *)dpc->core)->priv_data, DPC_ADDR_RDNS_G_STD_NP_ARRAY_RG4, reg_value);

	reg_value = (dpc->rdns_g_std_np_array[11] << 16) |
		(dpc->rdns_g_std_np_array[10]);
	system_reg_write(((tisp_core_t *)dpc->core)->priv_data, DPC_ADDR_RDNS_G_STD_NP_ARRAY_RG5, reg_value);

	reg_value = (dpc->rdns_g_std_np_array[13] << 16) |
		(dpc->rdns_g_std_np_array[12]);
	system_reg_write(((tisp_core_t *)dpc->core)->priv_data, DPC_ADDR_RDNS_G_STD_NP_ARRAY_RG6, reg_value);

	reg_value = (dpc->rdns_g_std_np_array[15] << 16) |
		(dpc->rdns_g_std_np_array[14]);
	system_reg_write(((tisp_core_t *)dpc->core)->priv_data, DPC_ADDR_RDNS_G_STD_NP_ARRAY_RG7, reg_value);

	return 0;
}

int32_t tisp_rdns_rb_lum_np_cfg(tisp_dpc_t *dpc)
{
	uint32_t reg_value;

	reg_value = (dpc->rdns_rb_lum_np_array[1] << 16) |
		(dpc->rdns_rb_lum_np_array[0]);
	system_reg_write(((tisp_core_t *)dpc->core)->priv_data, DPC_ADDR_RDNS_RB_LUM_NP_ARRAY_RG0, reg_value);

	reg_value = (dpc->rdns_rb_lum_np_array[3] << 16) |
		(dpc->rdns_rb_lum_np_array[2]);
	system_reg_write(((tisp_core_t *)dpc->core)->priv_data, DPC_ADDR_RDNS_RB_LUM_NP_ARRAY_RG1, reg_value);

	reg_value = (dpc->rdns_rb_lum_np_array[5] << 16) |
		(dpc->rdns_rb_lum_np_array[4]);
	system_reg_write(((tisp_core_t *)dpc->core)->priv_data, DPC_ADDR_RDNS_RB_LUM_NP_ARRAY_RG2, reg_value);

	reg_value = (dpc->rdns_rb_lum_np_array[7] << 16) |
		(dpc->rdns_rb_lum_np_array[6]);
	system_reg_write(((tisp_core_t *)dpc->core)->priv_data, DPC_ADDR_RDNS_RB_LUM_NP_ARRAY_RG3, reg_value);

	reg_value = (dpc->rdns_rb_lum_np_array[9] << 16) |
		(dpc->rdns_rb_lum_np_array[8]);
	system_reg_write(((tisp_core_t *)dpc->core)->priv_data, DPC_ADDR_RDNS_RB_LUM_NP_ARRAY_RG4, reg_value);

	reg_value = (dpc->rdns_rb_lum_np_array[11] << 16) |
		(dpc->rdns_rb_lum_np_array[10]);
	system_reg_write(((tisp_core_t *)dpc->core)->priv_data, DPC_ADDR_RDNS_RB_LUM_NP_ARRAY_RG5, reg_value);

	reg_value = (dpc->rdns_rb_lum_np_array[13] << 16) |
		(dpc->rdns_rb_lum_np_array[12]);
	system_reg_write(((tisp_core_t *)dpc->core)->priv_data, DPC_ADDR_RDNS_RB_LUM_NP_ARRAY_RG6, reg_value);

	reg_value = (dpc->rdns_rb_lum_np_array[15] << 16) |
		(dpc->rdns_rb_lum_np_array[14]);
	system_reg_write(((tisp_core_t *)dpc->core)->priv_data, DPC_ADDR_RDNS_RB_LUM_NP_ARRAY_RG7, reg_value);

	return 0;
}

int32_t tisp_rdns_rb_std_np_cfg(tisp_dpc_t *dpc)
{
	uint32_t reg_value;

	reg_value = (dpc->rdns_rb_std_np_array[1] << 16) |
		(dpc->rdns_rb_std_np_array[0]);
	system_reg_write(((tisp_core_t *)dpc->core)->priv_data, DPC_ADDR_RDNS_RB_STD_NP_ARRAY_RG0, reg_value);

	reg_value = (dpc->rdns_rb_std_np_array[3] << 16) |
		(dpc->rdns_rb_std_np_array[2]);
	system_reg_write(((tisp_core_t *)dpc->core)->priv_data, DPC_ADDR_RDNS_RB_STD_NP_ARRAY_RG1, reg_value);

	reg_value = (dpc->rdns_rb_std_np_array[5] << 16) |
		(dpc->rdns_rb_std_np_array[4]);
	system_reg_write(((tisp_core_t *)dpc->core)->priv_data, DPC_ADDR_RDNS_RB_STD_NP_ARRAY_RG2, reg_value);

	reg_value = (dpc->rdns_rb_std_np_array[7] << 16) |
		(dpc->rdns_rb_std_np_array[6]);
	system_reg_write(((tisp_core_t *)dpc->core)->priv_data, DPC_ADDR_RDNS_RB_STD_NP_ARRAY_RG3, reg_value);

	reg_value = (dpc->rdns_rb_std_np_array[9] << 16) |
		(dpc->rdns_rb_std_np_array[8]);
	system_reg_write(((tisp_core_t *)dpc->core)->priv_data, DPC_ADDR_RDNS_RB_STD_NP_ARRAY_RG4, reg_value);

	reg_value = (dpc->rdns_rb_std_np_array[11] << 16) |
		(dpc->rdns_rb_std_np_array[10]);
	system_reg_write(((tisp_core_t *)dpc->core)->priv_data, DPC_ADDR_RDNS_RB_STD_NP_ARRAY_RG5, reg_value);

	reg_value = (dpc->rdns_rb_std_np_array[13] << 16) |
		(dpc->rdns_rb_std_np_array[12]);
	system_reg_write(((tisp_core_t *)dpc->core)->priv_data, DPC_ADDR_RDNS_RB_STD_NP_ARRAY_RG6, reg_value);

	reg_value = (dpc->rdns_rb_std_np_array[15] << 16) |
		(dpc->rdns_rb_std_np_array[14]);
	system_reg_write(((tisp_core_t *)dpc->core)->priv_data, DPC_ADDR_RDNS_RB_STD_NP_ARRAY_RG7, reg_value);

	return 0;
}


int32_t tisp_dpc_s_par_cfg(tisp_dpc_t *dpc)
{
	int32_t data_tmp1;
	uint32_t reg_value;

	data_tmp1 = dpc->dpc_s_text_thres_intp - dpc->dpc_s_con_par_array[4];
	if(data_tmp1 < 0)
		data_tmp1 = 0;

	reg_value = (system_reg_read(((tisp_core_t *)dpc->core)->priv_data, DPC_ADDR_S_EN) & 0x7ffffff) | (dpc->dpc_s_con_par_array[0] << 27);
	system_reg_write(((tisp_core_t *)dpc->core)->priv_data, DPC_ADDR_S_EN, reg_value);

	reg_value = (dpc->dpc_s_con_par_array[2] << 26) |
		(data_tmp1 << 14) |
		(dpc->dpc_s_text_thres_intp << 2) |
		(dpc->dpc_s_con_par_array[3] << 1) |
		(dpc->dpc_s_con_par_array[3]);
	system_reg_write(((tisp_core_t *)dpc->core)->priv_data, DPC_ADDR_S_OC_BLEND_ALPHA, reg_value);

	reg_value = dpc->dpc_s_con_par_array[1];
	system_reg_write(((tisp_core_t *)dpc->core)->priv_data, DPC_ADDR_SDP_NUM, reg_value);

	return 0;
}

int32_t tisp_dpc_d_par_cfg(tisp_dpc_t *dpc)
{
	int32_t p0_l0_en, p0_l1_en, p0_l2_en, p0_l3_en;
	int32_t p1_l1_en, p1_l2_en, p1_l3_en;
	int32_t p2_l2_en, p2_l3_en;
	int32_t p3_l3_en;
	int32_t data_tmp1, data_tmp2, data_tmp3, data_tmp4, data_tmp5;
	uint32_t reg_value, reg_value_2;

	if (dpc->dpc_d_m1_level_intp == 0) {
		p0_l0_en = 0;
		p0_l1_en = 0;
		p0_l2_en = 0;
		p0_l3_en = 0;
		p1_l1_en = 0;
		p1_l2_en = 0;
		p1_l3_en = 0;
		p2_l2_en = 0;
		p2_l3_en = 0;
		p3_l3_en = 0;
	} else if (dpc->dpc_d_m1_level_intp == 1) {
		p0_l0_en = 1;
		p0_l1_en = 0;
		p0_l2_en = 0;
		p0_l3_en = 0;
		p1_l1_en = 0;
		p1_l2_en = 0;
		p1_l3_en = 0;
		p2_l2_en = 0;
		p2_l3_en = 0;
		p3_l3_en = 0;
	} else if (dpc->dpc_d_m1_level_intp == 2) {
		p0_l0_en = 1;
		p0_l1_en = 1;
		p0_l2_en = 0;
		p0_l3_en = 0;
		p1_l1_en = 1;
		p1_l2_en = 0;
		p1_l3_en = 0;
		p2_l2_en = 0;
		p2_l3_en = 0;
		p3_l3_en = 0;
	} else if (dpc->dpc_d_m1_level_intp == 3) {
		p0_l0_en = 1;
		p0_l1_en = 1;
		p0_l2_en = 1;
		p0_l3_en = 0;
		p1_l1_en = 1;
		p1_l2_en = 1;
		p1_l3_en = 0;
		p2_l2_en = 1;
		p2_l3_en = 0;
		p3_l3_en = 0;
	} else {
		p0_l0_en = 1;
		p0_l1_en = 1;
		p0_l2_en = 1;
		p0_l3_en = 1;
		p1_l1_en = 1;
		p1_l2_en = 1;
		p1_l3_en = 1;
		p2_l2_en = 1;
		p2_l3_en = 1;
		p3_l3_en = 1;
	}

	reg_value_2 = (dpc->dpc_d_m1_con_par_array[0] << 26) |
		(dpc->dpc_d_m1_con_par_array[1] << 25) |
		(dpc->dpc_d_m2_con_par_array[0] << 24) |
		(dpc->dpc_d_m2_con_par_array[1] << 23) |
		(p0_l0_en << 22) |
		(p0_l1_en << 21) |
		(p0_l2_en << 20) |
		(p0_l3_en << 19) |
		(p0_l0_en << 18) |
		(p0_l1_en << 17) |
		(p0_l2_en << 16) |
		(p0_l3_en << 15) |
		(p1_l1_en << 14) |
		(p1_l2_en << 13) |
		(p1_l3_en << 12) |
		(p1_l1_en << 11) |
		(p1_l2_en << 10) |
		(p1_l3_en << 9) |
		(p2_l2_en << 8) |
		(p2_l3_en << 7) |
		(p2_l2_en << 6) |
		(p2_l3_en << 5) |
		(p3_l3_en << 4) |
		(p3_l3_en << 3) |
		(dpc->dpc_d_m1_con_par_array[4] << 1) |
		(dpc->dpc_d_m2_con_par_array[2]);

	reg_value = (system_reg_read(((tisp_core_t *)dpc->core)->priv_data, DPC_ADDR_S_EN) & 0x8000000) | reg_value_2;
	system_reg_write(((tisp_core_t *)dpc->core)->priv_data, DPC_ADDR_S_EN, reg_value);

	if (dpc->dpc_d_m1_con_par_array[12] == 0) {
		data_tmp1 = 16;
		data_tmp2 = 0;
		data_tmp3 = 0;
		data_tmp4 = 0;
		data_tmp5 = 0;
	} else if (dpc->dpc_d_m1_con_par_array[12] == 1) {
		data_tmp1 = 10;
		data_tmp2 = 2;
		data_tmp3 = 1;
		data_tmp4 = 0;
		data_tmp5 = 0;
	} else if (dpc->dpc_d_m1_con_par_array[12] == 2) {
		data_tmp1 = 10;
		data_tmp2 = 1;
		data_tmp3 = 1;
		data_tmp4 = 1;
		data_tmp5 = 0;
	} else {
		data_tmp1 = 6;
		data_tmp2 = 2;
		data_tmp3 = 1;
		data_tmp4 = 1;
		data_tmp5 = 1;
	}

	reg_value = (dpc->dpc_d_m1_con_par_array[2] << 13) |
		(data_tmp5 << 12) |
		(data_tmp4 << 10) |
		(data_tmp3 << 8) |
		(data_tmp2 << 5) |
		(data_tmp1);
	system_reg_write(((tisp_core_t *)dpc->core)->priv_data, DPC_ADDR_OC_BLEND_ALPHA, reg_value);

	data_tmp1 = (255 * dpc->dpc_d_m1_con_par_array[3]) >> 4;
	data_tmp2 = (255 * dpc->dpc_d_m2_con_par_array[3]) >> 4;
	reg_value = (data_tmp1 << 16) | (data_tmp2);
	system_reg_write(((tisp_core_t *)dpc->core)->priv_data, DPC_ADDR_SD_LV1_SLOPE, reg_value);

	data_tmp1 = (255 * dpc->dpc_d_m1_con_par_array[3]) >> 4;
	data_tmp2 = (63 * dpc->dpc_d_m1_con_par_array[3]) >> 4;
	data_tmp3 = (31 * dpc->dpc_d_m2_con_par_array[3]) >> 4;
	reg_value = (data_tmp1 << 16) | (data_tmp2 << 10) |
		(data_tmp3 << 5) | (data_tmp3);
	system_reg_write(((tisp_core_t *)dpc->core)->priv_data, DPC_ADDR_SD_LV2_SLOPE, reg_value);

	data_tmp1 = dpc->dpc_d_m1_l0_fthres_intp - dpc->dpc_d_m1_con_par_array[11];
	if(data_tmp1 < 0)
		data_tmp1 = 0;
	reg_value = (dpc->dpc_d_m2_con_par_array[5] << 22) |
		(dpc->dpc_d_m1_con_par_array[13] << 16) | (data_tmp1);
	system_reg_write(((tisp_core_t *)dpc->core)->priv_data, DPC_ADDR_LS_BLEND_ALPHA_RB, reg_value);

	data_tmp1 = dpc->dpc_d_m1_l0_dthres_intp - dpc->dpc_d_m1_con_par_array[11];
	if(data_tmp1 < 0)
		data_tmp1 = 0;
	data_tmp2 = dpc->dpc_d_m2_l0_thres_intp - dpc->dpc_d_m2_con_par_array[4];
	if(data_tmp2 < 0)
		data_tmp2 = 0;
	reg_value = (data_tmp1 << 16) | (data_tmp2);
	system_reg_write(((tisp_core_t *)dpc->core)->priv_data, DPC_ADDR_LV1_DIFF_THRES_RB, reg_value);

	data_tmp1 = dpc->dpc_d_m1_l1_hthres_intp - dpc->dpc_d_m1_con_par_array[11];
	if(data_tmp1 < 0)
		data_tmp1 = 0;
	data_tmp2 = dpc->dpc_d_m1_l1_lthres_intp - dpc->dpc_d_m1_con_par_array[11];
	if(data_tmp2 < 0)
		data_tmp2 = 0;
	reg_value = (data_tmp1 << 16) | (data_tmp2);
	system_reg_write(((tisp_core_t *)dpc->core)->priv_data, DPC_ADDR_SD_LV2_DIFF_THRES_RB, reg_value);

	data_tmp1 = dpc->dpc_d_m1_l1_d1_thres_intp - dpc->dpc_d_m1_con_par_array[11];
	if(data_tmp1 < 0)
		data_tmp1 = 0;
	data_tmp2 = data_tmp1 + dpc->dpc_d_m1_con_par_array[5];
	reg_value = (data_tmp2 << 16) |	(data_tmp1);
	system_reg_write(((tisp_core_t *)dpc->core)->priv_data, DPC_ADDR_SD_LV2_P0_THRES_RB, reg_value);

	data_tmp2 = data_tmp1 + dpc->dpc_d_m1_con_par_array[6];
	data_tmp3 = data_tmp1 + dpc->dpc_d_m1_con_par_array[7];
	reg_value = (data_tmp3 << 16) |	(data_tmp2);
	system_reg_write(((tisp_core_t *)dpc->core)->priv_data, DPC_ADDR_SD_LV2_P2_THRES_RB, reg_value);

	data_tmp1 = dpc->dpc_d_m1_l1_d2_thres_intp - dpc->dpc_d_m1_con_par_array[11];
	if(data_tmp1 < 0)
		data_tmp1 = 0;
	data_tmp2 = data_tmp1 + dpc->dpc_d_m1_con_par_array[8];
	reg_value = (data_tmp2 << 16) |	(data_tmp1);
	system_reg_write(((tisp_core_t *)dpc->core)->priv_data, DPC_ADDR_SD_LV2_P0_DIST_THRES_RB, reg_value);

	data_tmp2 = data_tmp1 + dpc->dpc_d_m1_con_par_array[9];
	data_tmp3 = data_tmp1 + dpc->dpc_d_m1_con_par_array[10];
	reg_value = (data_tmp3 << 16) |	(data_tmp2);
	system_reg_write(((tisp_core_t *)dpc->core)->priv_data, DPC_ADDR_SD_LV2_P2_DIST_THRES_RB, reg_value);

	data_tmp1 = dpc->dpc_d_m2_l1_pdthres_intp - dpc->dpc_d_m2_con_par_array[4];
	if(data_tmp1 < 0)
		data_tmp1 = 0;

	data_tmp2 = dpc->dpc_d_m2_l1_ldthres_intp - dpc->dpc_d_m2_con_par_array[4];
	if(data_tmp2 < 0)
		data_tmp2 = 0;
	reg_value = (data_tmp2 << 16) | (data_tmp1);
	system_reg_write(((tisp_core_t *)dpc->core)->priv_data, DPC_ADDR_LD_LV2_THRES_RB, reg_value);

	reg_value = (dpc->dpc_d_m2_con_par_array[5] << 22) |
		(dpc->dpc_d_m1_con_par_array[13] << 16) |
		(dpc->dpc_d_m1_l0_fthres_intp);
	system_reg_write(((tisp_core_t *)dpc->core)->priv_data, DPC_ADDR_LS_BLEND_ALPHA_G, reg_value);

	reg_value = (dpc->dpc_d_m1_l0_dthres_intp << 16) |
		(dpc->dpc_d_m2_l0_thres_intp);
	system_reg_write(((tisp_core_t *)dpc->core)->priv_data, DPC_ADDR_LV1_DIFF_THRES_G, reg_value);

	reg_value = (dpc->dpc_d_m1_l1_hthres_intp << 16) |
		(dpc->dpc_d_m1_l1_lthres_intp);
	system_reg_write(((tisp_core_t *)dpc->core)->priv_data, DPC_ADDR_SD_LV2_DIFF_THRES_G, reg_value);

	data_tmp1 = dpc->dpc_d_m1_l1_d1_thres_intp;
	data_tmp2 = data_tmp1 + dpc->dpc_d_m1_con_par_array[5];
	reg_value = (data_tmp2 << 16) |	(data_tmp1);
	system_reg_write(((tisp_core_t *)dpc->core)->priv_data, DPC_ADDR_SD_LV2_P0_THRES_G, reg_value);

	data_tmp2 = data_tmp1 + dpc->dpc_d_m1_con_par_array[6];
	data_tmp3 = data_tmp1 + dpc->dpc_d_m1_con_par_array[7];
	reg_value = (data_tmp3 << 16) | (data_tmp2);
	system_reg_write(((tisp_core_t *)dpc->core)->priv_data, DPC_ADDR_SD_LV2_P2_THRES_G, reg_value);

	data_tmp1 = dpc->dpc_d_m1_l1_d2_thres_intp;
	data_tmp2 = data_tmp1 + dpc->dpc_d_m1_con_par_array[8];
	reg_value = (data_tmp2 << 16) |	(data_tmp1);
	system_reg_write(((tisp_core_t *)dpc->core)->priv_data, DPC_ADDR_SD_LV2_P0_DIST_THRES_G, reg_value);

	data_tmp2 = data_tmp1 + dpc->dpc_d_m1_con_par_array[9];
	data_tmp3 = data_tmp1 + dpc->dpc_d_m1_con_par_array[10];
	reg_value = (data_tmp3 << 16) |	(data_tmp2);
	system_reg_write(((tisp_core_t *)dpc->core)->priv_data, DPC_ADDR_SD_LV2_P2_DIST_THRES_G, reg_value);

	data_tmp1 = dpc->dpc_d_m2_l1_pdthres_intp;
	data_tmp2 = dpc->dpc_d_m2_l1_ldthres_intp;
	if(data_tmp2 < 0)
		data_tmp2 = 0;
	reg_value = (data_tmp2 << 16) | (data_tmp1);
	system_reg_write(((tisp_core_t *)dpc->core)->priv_data, DPC_ADDR_LD_LV2_THRES_G, reg_value);

	return 0;
}

int32_t tisp_ctr_par_cfg(tisp_dpc_t *dpc)
{
	uint32_t reg_value, data_tmp1, data_tmp2;

	reg_value = (dpc->ctr_con_par_array[0] << 25) |
		(dpc->ctr_stren_intp << 16) |
		(dpc->ctr_con_par_array[2] << 13) |
		(dpc->ctr_con_par_array[1] << 12) |
		(dpc->ctr_md_thres_intp);
	system_reg_write(((tisp_core_t *)dpc->core)->priv_data, DPC_ADDR_CTR_FUNC_EN, reg_value);

	data_tmp1 = (255 * dpc->ctr_con_par_array[4]) >> 4;
	data_tmp2 = (63 * dpc->ctr_con_par_array[4]) >> 4;
	reg_value = (dpc->ctr_con_par_array[3] << 16) | (data_tmp1 << 6) | (data_tmp2);
	system_reg_write(((tisp_core_t *)dpc->core)->priv_data, DPC_ADDR_CTR_MD_SEG_OPT, reg_value);

	reg_value = (dpc->ctr_eh_thres_intp << 16) |	(dpc->ctr_el_thres_intp);
	system_reg_write(((tisp_core_t *)dpc->core)->priv_data, DPC_ADDR_CTR_BASE_THRES, reg_value);

	return 0;
}

int32_t tisp_rdns_par_cfg(tisp_dpc_t *dpc)
{
	int32_t data_tmp1, data_tmp2;
	int32_t x1_tmp, x2_tmp, y1_tmp, y2_tmp, s1_tmp, s2_tmp, s3_tmp;
	uint32_t reg_value;

	reg_value = (dpc->rdns_con_par_array[0] << 16) |
		(dpc->rdns_stren_intp);
	system_reg_write(((tisp_core_t *)dpc->core)->priv_data, DPC_ADDR_RDNS_FUNC_EN, reg_value);

	reg_value = (dpc->rdns_y_fthres_intp << 16) |
		(dpc->rdns_uv_fthres_intp);
	system_reg_write(((tisp_core_t *)dpc->core)->priv_data, DPC_ADDR_RDNS_FLAT_THRES, reg_value);

	reg_value = (dpc->rdns_y_tthres_intp << 16) |
		(dpc->rdns_uv_tthres_intp);
	system_reg_write(((tisp_core_t *)dpc->core)->priv_data, DPC_ADDR_RDNS_TEXT_THRES, reg_value);

	data_tmp1 = (dpc->rdns_con_par_array[9] * 31) >> 4;
	if (dpc->rdns_con_par_array[9] == 0) {
		x1_tmp = 0;
		x2_tmp = 0;
		y1_tmp = 0;
		y2_tmp = 0;
		s1_tmp = 0;
		s2_tmp = 0;
		s3_tmp = 0;
	} else {
		data_tmp2 = 128 / data_tmp1;
		x1_tmp = data_tmp2 / 4;
		x2_tmp = 2 * x1_tmp;
		y1_tmp = 24;
		y2_tmp = 8;
		s1_tmp = data_tmp1;
		s2_tmp = data_tmp1;
		s3_tmp = data_tmp1;
	}
	reg_value = (x1_tmp << 16) | (x1_tmp);
	system_reg_write(((tisp_core_t *)dpc->core)->priv_data, DPC_ADDR_RDNS_SIGMA_X1, reg_value);

	data_tmp1 = (63 * dpc->rdns_con_par_array[5]) >> 4;
	reg_value = (dpc->rdns_con_par_array[1] << 28) |
		(dpc->rdns_std_thres_intp << 16) |
		(dpc->rdns_con_par_array[3] << 6) |
		(data_tmp1);
	system_reg_write(((tisp_core_t *)dpc->core)->priv_data, DPC_ADDR_RDNS_STD, reg_value);

	reg_value = (dpc->rdns_con_par_array[8] << 12) |
		(dpc->rdns_con_par_array[7] << 10) |
		(dpc->rdns_con_par_array[6] << 8) |
		(dpc->rdns_con_par_array[4] << 6) |
		(dpc->rdns_con_par_array[3] << 4) |
		(dpc->rdns_con_par_array[4] << 2) |
		(dpc->rdns_con_par_array[3]);
	system_reg_write(((tisp_core_t *)dpc->core)->priv_data, DPC_ADDR_RDNS_RB_GAUS_SIGMA, reg_value);

	reg_value = (x2_tmp << 16) | (y2_tmp << 6) | (y1_tmp);
	system_reg_write(((tisp_core_t *)dpc->core)->priv_data, DPC_ADDR_RDNS_G_SIGMA_X, reg_value);

	reg_value = (s3_tmp << 10) | (s2_tmp << 5) | (s1_tmp);
	system_reg_write(((tisp_core_t *)dpc->core)->priv_data, DPC_ADDR_RDNS_G_SIGMA_S, reg_value);

	reg_value = (x2_tmp << 16) |(y2_tmp << 6) | (y1_tmp);
	system_reg_write(((tisp_core_t *)dpc->core)->priv_data, DPC_ADDR_RDNS_RB_SIGMA_X, reg_value);

	reg_value = (s3_tmp << 10) | (s2_tmp << 5) | (s1_tmp);
	system_reg_write(((tisp_core_t *)dpc->core)->priv_data, DPC_ADDR_RDNS_RB_SIGMA_S, reg_value);

	return 0;
}

int32_t tisp_dpc_intp(tisp_dpc_t *dpc, int32_t gain_value)
{

	int32_t x_int, x_fra;

	x_int = gain_value >> 16;
	x_fra = gain_value & 0xffff;

	dpc->dpc_s_text_thres_intp = tisp_simple_intp(x_int, x_fra, dpc->dpc_s_text_thres_array);
	dpc->dpc_d_m1_level_intp = tisp_simple_intp(x_int, x_fra, dpc->dpc_d_m1_level_array);
	dpc->dpc_d_m1_l0_fthres_intp = tisp_simple_intp(x_int, x_fra, dpc->dpc_d_m1_l0_fthres_array);
	dpc->dpc_d_m1_l0_dthres_intp = tisp_simple_intp(x_int, x_fra, dpc->dpc_d_m1_l0_dthres_array);
	dpc->dpc_d_m1_l1_hthres_intp = tisp_simple_intp(x_int, x_fra, dpc->dpc_d_m1_l1_hthres_array);
	dpc->dpc_d_m1_l1_lthres_intp = tisp_simple_intp(x_int, x_fra, dpc->dpc_d_m1_l1_lthres_array);
	dpc->dpc_d_m1_l1_d1_thres_intp = tisp_simple_intp(x_int, x_fra, dpc->dpc_d_m1_l1_d1_thres_array);
	dpc->dpc_d_m1_l1_d2_thres_intp = tisp_simple_intp(x_int, x_fra, dpc->dpc_d_m1_l1_d2_thres_array);
	dpc->dpc_d_m2_level_intp = tisp_simple_intp(x_int, x_fra, dpc->dpc_d_m2_level_array);
	dpc->dpc_d_m2_l0_thres_intp = tisp_simple_intp(x_int, x_fra, dpc->dpc_d_m2_l0_thres_array);
	dpc->dpc_d_m2_l1_ldthres_intp = tisp_simple_intp(x_int, x_fra, dpc->dpc_d_m2_l1_ldthres_array);
	dpc->dpc_d_m2_l1_pdthres_intp = tisp_simple_intp(x_int, x_fra, dpc->dpc_d_m2_l1_pdthres_array);
	dpc->ctr_stren_intp = tisp_simple_intp(x_int, x_fra, dpc->ctr_stren_array);
	dpc->ctr_md_thres_intp = tisp_simple_intp(x_int, x_fra, dpc->ctr_md_thres_array);
	dpc->ctr_el_thres_intp = tisp_simple_intp(x_int, x_fra, dpc->ctr_el_thres_array);
	dpc->ctr_eh_thres_intp = tisp_simple_intp(x_int, x_fra, dpc->ctr_eh_thres_array);
	dpc->rdns_stren_intp = tisp_simple_intp(x_int, x_fra, dpc->rdns_stren_array);
	dpc->rdns_std_thres_intp = tisp_simple_intp(x_int, x_fra, dpc->rdns_std_thres_array);
	dpc->rdns_y_fthres_intp = tisp_simple_intp(x_int, x_fra, dpc->rdns_y_fthres_array);
	dpc->rdns_y_tthres_intp = tisp_simple_intp(x_int, x_fra, dpc->rdns_y_tthres_array);
	dpc->rdns_uv_fthres_intp = tisp_simple_intp(x_int, x_fra, dpc->rdns_uv_fthres_array);
	dpc->rdns_uv_tthres_intp = tisp_simple_intp(x_int, x_fra, dpc->rdns_uv_tthres_array);

	return 0;
}

int32_t tisp_dpc_all_reg_refresh(tisp_dpc_t *dpc, int gain_value)
{
	tisp_dpc_intp(dpc, gain_value);
	tisp_ctr_md_np_cfg(dpc);
	tisp_rdns_uu_np_cfg(dpc);
	tisp_rdns_g_lum_np_cfg(dpc);
	tisp_rdns_g_std_np_cfg(dpc);
	tisp_rdns_rb_lum_np_cfg(dpc);
	tisp_rdns_rb_std_np_cfg(dpc);
	tisp_dpc_s_par_cfg(dpc);
	tisp_dpc_d_par_cfg(dpc);
	tisp_ctr_par_cfg(dpc);
	tisp_rdns_par_cfg(dpc);

	return 0;
}

int32_t tisp_dpc_intp_reg_refresh(tisp_dpc_t *dpc, int gain_value)
{
	tisp_dpc_intp(dpc, gain_value);
	tisp_dpc_s_par_cfg(dpc);
	tisp_dpc_d_par_cfg(dpc);
	tisp_ctr_par_cfg(dpc);
	tisp_rdns_par_cfg(dpc);

	return 0;
}

static uint32_t gain_old = 0xffffffff;
static uint32_t gain_thres = 0x100;
static uint32_t shadow_en = 0;

uint32_t tisp_dpc_par_refresh(tisp_dpc_t *dpc, uint32_t gain_value, uint32_t gain_thres, uint32_t shadow_en)
{
	/* shadow_en: */
	/* 0: shadow disable */
	/* 1: shadow enable */

	uint32_t gain_diff;

	if (gain_old == 0xffffffff) {
		gain_old = gain_value;
		tisp_dpc_all_reg_refresh(dpc, gain_value);
	} else {
		if (gain_value >= gain_old)
			gain_diff = gain_value-gain_old;
		else
			gain_diff = gain_old-gain_value;

		if (gain_diff >= gain_thres) {
			gain_old = gain_value;
			tisp_dpc_intp_reg_refresh(dpc, gain_value);
		}
	}

	if (shadow_en == 1) {
		system_reg_write(((tisp_core_t *)dpc->core)->priv_data, DPC_ADDR_SYNC, 1);
	}

	return 0;
}

int32_t tiziano_dpc_params_refresh(tisp_dpc_t *dpc)
{
	memcpy(dpc->ctr_md_np_array,
	       tparams.params_data.TISP_PARAM_CTR_MD_NP_ARRAY,
	       sizeof(tparams.params_data.TISP_PARAM_CTR_MD_NP_ARRAY));
	memcpy(dpc->rdns_uu_np_array,
	       tparams.params_data.TISP_PARAM_RDNS_UU_NP_ARRAY,
	       sizeof(tparams.params_data.TISP_PARAM_RDNS_UU_NP_ARRAY));
	memcpy(dpc->rdns_g_lum_np_array,
	       tparams.params_data.TISP_PARAM_RDNS_G_LUM_NP_ARRAY,
	       sizeof(tparams.params_data.TISP_PARAM_RDNS_G_LUM_NP_ARRAY));
	memcpy(dpc->rdns_g_std_np_array,
	       tparams.params_data.TISP_PARAM_RDNS_G_STD_NP_ARRAY,
	       sizeof(tparams.params_data.TISP_PARAM_RDNS_G_STD_NP_ARRAY));
	memcpy(dpc->rdns_rb_lum_np_array,
	       tparams.params_data.TISP_PARAM_RDNS_RB_LUM_NP_ARRAY,
	       sizeof(tparams.params_data.TISP_PARAM_RDNS_RB_LUM_NP_ARRAY));
	memcpy(dpc->rdns_rb_std_np_array,
	       tparams.params_data.TISP_PARAM_RDNS_RB_STD_NP_ARRAY,
	       sizeof(tparams.params_data.TISP_PARAM_RDNS_RB_STD_NP_ARRAY));
	memcpy(dpc->dpc_s_text_thres_array,
	       tparams.params_data.TISP_PARAM_DPC_S_TEXT_THRES_ARRAY,
	       sizeof(tparams.params_data.TISP_PARAM_DPC_S_TEXT_THRES_ARRAY));
	memcpy(dpc->dpc_s_con_par_array,
	       tparams.params_data.TISP_PARAM_DPC_S_CON_PAR_ARRAY,
	       sizeof(tparams.params_data.TISP_PARAM_DPC_S_CON_PAR_ARRAY));
	memcpy(dpc->dpc_d_m1_level_array,
	       tparams.params_data.TISP_PARAM_DPC_D_M1_LEVEL_ARRAY,
	       sizeof(tparams.params_data.TISP_PARAM_DPC_D_M1_LEVEL_ARRAY));
	memcpy(dpc->dpc_d_m1_l0_fthres_array,
	       tparams.params_data.TISP_PARAM_DPC_D_M1_L0_FTHRES_ARRAY,
	       sizeof(tparams.params_data.TISP_PARAM_DPC_D_M1_L0_FTHRES_ARRAY));
	memcpy(dpc->dpc_d_m1_l0_dthres_array,
	       tparams.params_data.TISP_PARAM_DPC_D_M1_L0_DTHRES_ARRAY,
	       sizeof(tparams.params_data.TISP_PARAM_DPC_D_M1_L0_DTHRES_ARRAY));
	memcpy(dpc->dpc_d_m1_l1_hthres_array,
	       tparams.params_data.TISP_PARAM_DPC_D_M1_L1_HTHRES_ARRAY,
	       sizeof(tparams.params_data.TISP_PARAM_DPC_D_M1_L1_HTHRES_ARRAY));
	memcpy(dpc->dpc_d_m1_l1_lthres_array,
	       tparams.params_data.TISP_PARAM_DPC_D_M1_L1_LTHRES_ARRAY,
	       sizeof(tparams.params_data.TISP_PARAM_DPC_D_M1_L1_LTHRES_ARRAY));
	memcpy(dpc->dpc_d_m1_l1_d1_thres_array,
	       tparams.params_data.TISP_PARAM_DPC_D_M1_L1_D1_THRES_ARRAY,
	       sizeof(tparams.params_data.TISP_PARAM_DPC_D_M1_L1_D1_THRES_ARRAY));
	memcpy(dpc->dpc_d_m1_l1_d2_thres_array,
	       tparams.params_data.TISP_PARAM_DPC_D_M1_L1_D2_THRES_ARRAY,
	       sizeof(tparams.params_data.TISP_PARAM_DPC_D_M1_L1_D2_THRES_ARRAY));
	memcpy(dpc->dpc_d_m1_con_par_array,
	       tparams.params_data.TISP_PARAM_DPC_D_M1_CON_PAR_ARRAY,
	       sizeof(tparams.params_data.TISP_PARAM_DPC_D_M1_CON_PAR_ARRAY));
	memcpy(dpc->dpc_d_m2_level_array,
	       tparams.params_data.TISP_PARAM_DPC_D_M2_LEVEL_ARRAY,
	       sizeof(tparams.params_data.TISP_PARAM_DPC_D_M2_LEVEL_ARRAY));
	memcpy(dpc->dpc_d_m2_l0_thres_array,
	       tparams.params_data.TISP_PARAM_DPC_D_M2_L0_THRES_ARRAY,
	       sizeof(tparams.params_data.TISP_PARAM_DPC_D_M2_L0_THRES_ARRAY));
	memcpy(dpc->dpc_d_m2_l1_ldthres_array,
	       tparams.params_data.TISP_PARAM_DPC_D_M2_L1_LDTHRES_ARRAY,
	       sizeof(tparams.params_data.TISP_PARAM_DPC_D_M2_L1_LDTHRES_ARRAY));
	memcpy(dpc->dpc_d_m2_l1_pdthres_array,
	       tparams.params_data.TISP_PARAM_DPC_D_M2_L1_PDTHRES_ARRAY,
	       sizeof(tparams.params_data.TISP_PARAM_DPC_D_M2_L1_PDTHRES_ARRAY));
	memcpy(dpc->dpc_d_m2_con_par_array,
	       tparams.params_data.TISP_PARAM_DPC_D_M2_CON_PAR_ARRAY,
	       sizeof(tparams.params_data.TISP_PARAM_DPC_D_M2_CON_PAR_ARRAY));
	memcpy(dpc->ctr_stren_array,
	       tparams.params_data.TISP_PARAM_CTR_STREN_ARRAY,
	       sizeof(tparams.params_data.TISP_PARAM_CTR_STREN_ARRAY));
	memcpy(dpc->ctr_md_thres_array,
	       tparams.params_data.TISP_PARAM_CTR_MD_THRES_ARRAY,
	       sizeof(tparams.params_data.TISP_PARAM_CTR_MD_THRES_ARRAY));
	memcpy(dpc->ctr_el_thres_array,
	       tparams.params_data.TISP_PARAM_CTR_EL_THRES_ARRAY,
	       sizeof(tparams.params_data.TISP_PARAM_CTR_EL_THRES_ARRAY));
	memcpy(dpc->ctr_eh_thres_array,
	       tparams.params_data.TISP_PARAM_CTR_EH_THRES_ARRAY,
	       sizeof(tparams.params_data.TISP_PARAM_CTR_EH_THRES_ARRAY));
	memcpy(dpc->ctr_con_par_array,
	       tparams.params_data.TISP_PARAM_CTR_CON_PAR_ARRAY,
	       sizeof(tparams.params_data.TISP_PARAM_CTR_CON_PAR_ARRAY));
	memcpy(dpc->rdns_stren_array,
	       tparams.params_data.TISP_PARAM_RDNS_STREN_ARRAY,
	       sizeof(tparams.params_data.TISP_PARAM_RDNS_STREN_ARRAY));
	memcpy(dpc->rdns_std_thres_array,
	       tparams.params_data.TISP_PARAM_RDNS_STD_THRES_ARRAY,
	       sizeof(tparams.params_data.TISP_PARAM_RDNS_STD_THRES_ARRAY));
	memcpy(dpc->rdns_y_fthres_array,
	       tparams.params_data.TISP_PARAM_RDNS_Y_FTHRES_ARRAY,
	       sizeof(tparams.params_data.TISP_PARAM_RDNS_Y_FTHRES_ARRAY));
	memcpy(dpc->rdns_y_tthres_array,
	       tparams.params_data.TISP_PARAM_RDNS_Y_TTHRES_ARRAY,
	       sizeof(tparams.params_data.TISP_PARAM_RDNS_Y_TTHRES_ARRAY));
	memcpy(dpc->rdns_uv_fthres_array,
	       tparams.params_data.TISP_PARAM_RDNS_UV_FTHRES_ARRAY,
	       sizeof(tparams.params_data.TISP_PARAM_RDNS_UV_FTHRES_ARRAY));
	memcpy(dpc->rdns_uv_tthres_array,
	       tparams.params_data.TISP_PARAM_RDNS_UV_TTHRES_ARRAY,
	       sizeof(tparams.params_data.TISP_PARAM_RDNS_UV_TTHRES_ARRAY));
	memcpy(dpc->rdns_con_par_array,
	       tparams.params_data.TISP_PARAM_RDNS_CON_PAR_ARRAY,
	       sizeof(tparams.params_data.TISP_PARAM_RDNS_CON_PAR_ARRAY));

	return 0;
}

int32_t tiziano_dpc_dn_params_refresh(tisp_dpc_t *dpc)
{
	gain_old += 0x200;
	tiziano_dpc_params_refresh(dpc);
	tisp_dpc_all_reg_refresh(dpc, gain_old);

	return 0;
}

int32_t tiziano_dpc_init(tisp_dpc_t *dpc)
{
	gain_old = 0xffffffff;
	tiziano_dpc_params_refresh(dpc);
	tisp_dpc_par_refresh(dpc, 0x10000, 0x10000, 0);

	return 0;
}

int32_t tisp_dpc_refresh(tisp_dpc_t *dpc, uint32_t gain_value)
{
	tisp_dpc_par_refresh(dpc, gain_value, gain_thres, shadow_en);

	return 0;
}

int32_t tisp_dpc_param_array_get(tisp_dpc_t *dpc, int32_t id, void *buf, uint32_t *plen)
{
	uint32_t len;
	void *tmpbuf;

	switch (id) {
	case TISP_PARAM_CTR_MD_NP_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_CTR_MD_NP_ARRAY);
		tmpbuf =dpc->ctr_md_np_array;
		break;
	case TISP_PARAM_RDNS_UU_NP_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_RDNS_UU_NP_ARRAY);
		tmpbuf =dpc->rdns_uu_np_array;
		break;
	case TISP_PARAM_RDNS_G_LUM_NP_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_RDNS_G_LUM_NP_ARRAY);
		tmpbuf =dpc->rdns_g_lum_np_array;
		break;
	case TISP_PARAM_RDNS_G_STD_NP_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_RDNS_G_STD_NP_ARRAY);
		tmpbuf =dpc->rdns_g_std_np_array;
		break;
	case TISP_PARAM_RDNS_RB_LUM_NP_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_RDNS_RB_LUM_NP_ARRAY);
		tmpbuf =dpc->rdns_rb_lum_np_array;
		break;
	case TISP_PARAM_RDNS_RB_STD_NP_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_RDNS_RB_STD_NP_ARRAY);
		tmpbuf =dpc->rdns_rb_std_np_array;
		break;
	case TISP_PARAM_DPC_S_TEXT_THRES_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_DPC_S_TEXT_THRES_ARRAY);
		tmpbuf =dpc->dpc_s_text_thres_array;
		break;
	case TISP_PARAM_DPC_S_CON_PAR_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_DPC_S_CON_PAR_ARRAY);
		tmpbuf =dpc->dpc_s_con_par_array;
		break;
	case TISP_PARAM_DPC_D_M1_LEVEL_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_DPC_D_M1_LEVEL_ARRAY);
		tmpbuf =dpc->dpc_d_m1_level_array;
		break;
	case TISP_PARAM_DPC_D_M1_L0_FTHRES_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_DPC_D_M1_L0_FTHRES_ARRAY);
		tmpbuf =dpc->dpc_d_m1_l0_fthres_array;
		break;
	case TISP_PARAM_DPC_D_M1_L0_DTHRES_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_DPC_D_M1_L0_DTHRES_ARRAY);
		tmpbuf =dpc->dpc_d_m1_l0_dthres_array;
		break;
	case TISP_PARAM_DPC_D_M1_L1_HTHRES_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_DPC_D_M1_L1_HTHRES_ARRAY);
		tmpbuf =dpc->dpc_d_m1_l1_hthres_array;
		break;
	case TISP_PARAM_DPC_D_M1_L1_LTHRES_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_DPC_D_M1_L1_LTHRES_ARRAY);
		tmpbuf =dpc->dpc_d_m1_l1_lthres_array;
		break;
	case TISP_PARAM_DPC_D_M1_L1_D1_THRES_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_DPC_D_M1_L1_D1_THRES_ARRAY);
		tmpbuf =dpc->dpc_d_m1_l1_d1_thres_array;
		break;
	case TISP_PARAM_DPC_D_M1_L1_D2_THRES_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_DPC_D_M1_L1_D2_THRES_ARRAY);
		tmpbuf =dpc->dpc_d_m1_l1_d2_thres_array;
		break;
	case TISP_PARAM_DPC_D_M1_CON_PAR_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_DPC_D_M1_CON_PAR_ARRAY);
		tmpbuf =dpc->dpc_d_m1_con_par_array;
		break;
	case TISP_PARAM_DPC_D_M2_LEVEL_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_DPC_D_M2_LEVEL_ARRAY);
		tmpbuf =dpc->dpc_d_m2_level_array;
		break;
	case TISP_PARAM_DPC_D_M2_L0_THRES_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_DPC_D_M2_L0_THRES_ARRAY);
		tmpbuf =dpc->dpc_d_m2_l0_thres_array;
		break;
	case TISP_PARAM_DPC_D_M2_L1_LDTHRES_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_DPC_D_M2_L1_LDTHRES_ARRAY);
		tmpbuf =dpc->dpc_d_m2_l1_ldthres_array;
		break;
	case TISP_PARAM_DPC_D_M2_L1_PDTHRES_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_DPC_D_M2_L1_PDTHRES_ARRAY);
		tmpbuf =dpc->dpc_d_m2_l1_pdthres_array;
		break;
	case TISP_PARAM_DPC_D_M2_CON_PAR_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_DPC_D_M2_CON_PAR_ARRAY);
		tmpbuf =dpc->dpc_d_m2_con_par_array;
		break;
	case TISP_PARAM_CTR_STREN_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_CTR_STREN_ARRAY);
		tmpbuf =dpc->ctr_stren_array;
		break;
	case TISP_PARAM_CTR_MD_THRES_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_CTR_MD_THRES_ARRAY);
		tmpbuf =dpc->ctr_md_thres_array;
		break;
	case TISP_PARAM_CTR_EL_THRES_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_CTR_EL_THRES_ARRAY);
		tmpbuf =dpc->ctr_el_thres_array;
		break;
	case TISP_PARAM_CTR_EH_THRES_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_CTR_EH_THRES_ARRAY);
		tmpbuf =dpc->ctr_eh_thres_array;
		break;
	case TISP_PARAM_CTR_CON_PAR_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_CTR_CON_PAR_ARRAY);
		tmpbuf =dpc->ctr_con_par_array;
		break;
	case TISP_PARAM_RDNS_STREN_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_RDNS_STREN_ARRAY);
		tmpbuf =dpc->rdns_stren_array;
		break;
	case TISP_PARAM_RDNS_STD_THRES_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_RDNS_STD_THRES_ARRAY);
		tmpbuf =dpc->rdns_std_thres_array;
		break;
	case TISP_PARAM_RDNS_Y_FTHRES_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_RDNS_Y_FTHRES_ARRAY);
		tmpbuf =dpc->rdns_y_fthres_array;
		break;
	case TISP_PARAM_RDNS_Y_TTHRES_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_RDNS_Y_TTHRES_ARRAY);
		tmpbuf =dpc->rdns_y_tthres_array;
		break;
	case TISP_PARAM_RDNS_UV_FTHRES_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_RDNS_UV_FTHRES_ARRAY);
		tmpbuf =dpc->rdns_uv_fthres_array;
		break;
	case TISP_PARAM_RDNS_UV_TTHRES_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_RDNS_UV_TTHRES_ARRAY);
		tmpbuf =dpc->rdns_uv_tthres_array;
		break;
	case TISP_PARAM_RDNS_CON_PAR_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_RDNS_CON_PAR_ARRAY);
		tmpbuf =dpc->rdns_con_par_array;
		break;
	default:
		ISP_ERROR("%s,%d: dpc not support param id %d\n", __func__, __LINE__, id);
		return -1;
	}
	/* ISP_ERROR("### buf= %p tmpbuf = %p, len = %d tmpbuf[0] = 0x%x\n", buf, tmpbuf, len, ((uint32_t*)tmpbuf)[0]); */

	memcpy(buf, tmpbuf, len);
	*plen = len;

	return 0;
}

int32_t tisp_dpc_param_array_set(tisp_dpc_t *dpc, int32_t id, void *buf, uint32_t *plen)
{
	uint32_t len;
	void *tmpbuf;

	switch (id) {
	case TISP_PARAM_CTR_MD_NP_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_CTR_MD_NP_ARRAY);
		tmpbuf =dpc->ctr_md_np_array;
		memcpy(tmpbuf, buf, len);
		break;
	case TISP_PARAM_RDNS_UU_NP_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_RDNS_UU_NP_ARRAY);
		tmpbuf =dpc->rdns_uu_np_array;
		memcpy(tmpbuf, buf, len);
		break;
	case TISP_PARAM_RDNS_G_LUM_NP_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_RDNS_G_LUM_NP_ARRAY);
		tmpbuf =dpc->rdns_g_lum_np_array;
		memcpy(tmpbuf, buf, len);
		break;
	case TISP_PARAM_RDNS_G_STD_NP_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_RDNS_G_STD_NP_ARRAY);
		tmpbuf =dpc->rdns_g_std_np_array;
		memcpy(tmpbuf, buf, len);
		break;
	case TISP_PARAM_RDNS_RB_LUM_NP_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_RDNS_RB_LUM_NP_ARRAY);
		tmpbuf =dpc->rdns_rb_lum_np_array;
		memcpy(tmpbuf, buf, len);
		break;
	case TISP_PARAM_RDNS_RB_STD_NP_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_RDNS_RB_STD_NP_ARRAY);
		tmpbuf =dpc->rdns_rb_std_np_array;
		memcpy(tmpbuf, buf, len);
		break;
	case TISP_PARAM_DPC_S_TEXT_THRES_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_DPC_S_TEXT_THRES_ARRAY);
		tmpbuf =dpc->dpc_s_text_thres_array;
		memcpy(tmpbuf, buf, len);
		break;
	case TISP_PARAM_DPC_S_CON_PAR_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_DPC_S_CON_PAR_ARRAY);
		tmpbuf =dpc->dpc_s_con_par_array;
		memcpy(tmpbuf, buf, len);
		break;
	case TISP_PARAM_DPC_D_M1_LEVEL_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_DPC_D_M1_LEVEL_ARRAY);
		tmpbuf =dpc->dpc_d_m1_level_array;
		memcpy(tmpbuf, buf, len);
		break;
	case TISP_PARAM_DPC_D_M1_L0_FTHRES_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_DPC_D_M1_L0_FTHRES_ARRAY);
		tmpbuf =dpc->dpc_d_m1_l0_fthres_array;
		memcpy(tmpbuf, buf, len);
		break;
	case TISP_PARAM_DPC_D_M1_L0_DTHRES_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_DPC_D_M1_L0_DTHRES_ARRAY);
		tmpbuf =dpc->dpc_d_m1_l0_dthres_array;
		memcpy(tmpbuf, buf, len);
		break;
	case TISP_PARAM_DPC_D_M1_L1_HTHRES_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_DPC_D_M1_L1_HTHRES_ARRAY);
		tmpbuf =dpc->dpc_d_m1_l1_hthres_array;
		memcpy(tmpbuf, buf, len);
		break;
	case TISP_PARAM_DPC_D_M1_L1_LTHRES_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_DPC_D_M1_L1_LTHRES_ARRAY);
		tmpbuf =dpc->dpc_d_m1_l1_lthres_array;
		memcpy(tmpbuf, buf, len);
		break;
	case TISP_PARAM_DPC_D_M1_L1_D1_THRES_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_DPC_D_M1_L1_D1_THRES_ARRAY);
		tmpbuf =dpc->dpc_d_m1_l1_d1_thres_array;
		memcpy(tmpbuf, buf, len);
		break;
	case TISP_PARAM_DPC_D_M1_L1_D2_THRES_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_DPC_D_M1_L1_D2_THRES_ARRAY);
		tmpbuf =dpc->dpc_d_m1_l1_d2_thres_array;
		memcpy(tmpbuf, buf, len);
		break;
	case TISP_PARAM_DPC_D_M1_CON_PAR_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_DPC_D_M1_CON_PAR_ARRAY);
		tmpbuf =dpc->dpc_d_m1_con_par_array;
		memcpy(tmpbuf, buf, len);
		break;
	case TISP_PARAM_DPC_D_M2_LEVEL_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_DPC_D_M2_LEVEL_ARRAY);
		tmpbuf =dpc->dpc_d_m2_level_array;
		memcpy(tmpbuf, buf, len);
		break;
	case TISP_PARAM_DPC_D_M2_L0_THRES_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_DPC_D_M2_L0_THRES_ARRAY);
		tmpbuf =dpc->dpc_d_m2_l0_thres_array;
		memcpy(tmpbuf, buf, len);
		break;
	case TISP_PARAM_DPC_D_M2_L1_LDTHRES_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_DPC_D_M2_L1_LDTHRES_ARRAY);
		tmpbuf =dpc->dpc_d_m2_l1_ldthres_array;
		memcpy(tmpbuf, buf, len);
		break;
	case TISP_PARAM_DPC_D_M2_L1_PDTHRES_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_DPC_D_M2_L1_PDTHRES_ARRAY);
		tmpbuf =dpc->dpc_d_m2_l1_pdthres_array;
		memcpy(tmpbuf, buf, len);
		break;
	case TISP_PARAM_DPC_D_M2_CON_PAR_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_DPC_D_M2_CON_PAR_ARRAY);
		tmpbuf =dpc->dpc_d_m2_con_par_array;
		memcpy(tmpbuf, buf, len);
		break;
	case TISP_PARAM_CTR_STREN_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_CTR_STREN_ARRAY);
		tmpbuf =dpc->ctr_stren_array;
		memcpy(tmpbuf, buf, len);
		break;
	case TISP_PARAM_CTR_MD_THRES_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_CTR_MD_THRES_ARRAY);
		tmpbuf =dpc->ctr_md_thres_array;
		memcpy(tmpbuf, buf, len);
		break;
	case TISP_PARAM_CTR_EL_THRES_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_CTR_EL_THRES_ARRAY);
		tmpbuf =dpc->ctr_el_thres_array;
		memcpy(tmpbuf, buf, len);
		break;
	case TISP_PARAM_CTR_EH_THRES_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_CTR_EH_THRES_ARRAY);
		tmpbuf =dpc->ctr_eh_thres_array;
		memcpy(tmpbuf, buf, len);
		break;
	case TISP_PARAM_CTR_CON_PAR_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_CTR_CON_PAR_ARRAY);
		tmpbuf =dpc->ctr_con_par_array;
		memcpy(tmpbuf, buf, len);
		break;
	case TISP_PARAM_RDNS_STREN_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_RDNS_STREN_ARRAY);
		tmpbuf =dpc->rdns_stren_array;
		memcpy(tmpbuf, buf, len);
		break;
	case TISP_PARAM_RDNS_STD_THRES_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_RDNS_STD_THRES_ARRAY);
		tmpbuf =dpc->rdns_std_thres_array;
		memcpy(tmpbuf, buf, len);
		break;
	case TISP_PARAM_RDNS_Y_FTHRES_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_RDNS_Y_FTHRES_ARRAY);
		tmpbuf =dpc->rdns_y_fthres_array;
		memcpy(tmpbuf, buf, len);
		break;
	case TISP_PARAM_RDNS_Y_TTHRES_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_RDNS_Y_TTHRES_ARRAY);
		tmpbuf =dpc->rdns_y_tthres_array;
		memcpy(tmpbuf, buf, len);
		break;
	case TISP_PARAM_RDNS_UV_FTHRES_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_RDNS_UV_FTHRES_ARRAY);
		tmpbuf =dpc->rdns_uv_fthres_array;
		memcpy(tmpbuf, buf, len);
		break;
	case TISP_PARAM_RDNS_UV_TTHRES_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_RDNS_UV_TTHRES_ARRAY);
		tmpbuf =dpc->rdns_uv_tthres_array;
		memcpy(tmpbuf, buf, len);
		break;
	case TISP_PARAM_RDNS_CON_PAR_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_RDNS_CON_PAR_ARRAY);
		tmpbuf =dpc->rdns_con_par_array;
		memcpy(tmpbuf, buf, len);
		tisp_dpc_all_reg_refresh(dpc, gain_old+0x200);
		break;
	default:
		ISP_ERROR("%s,%d: dpc not support param id %d\n", __func__, __LINE__, id);
		return -1;
	}

	return 0;
}
