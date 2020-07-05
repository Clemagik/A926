#include <linux/kernel.h>
#include <linux/irqreturn.h>
#include <linux/dma-mapping.h>

#include "fix_point_calc.h"
#include "../inc/tiziano_sys.h"
#include "../inc/tiziano_core.h"
#include "tiziano_core.h"
#include "../inc/tiziano_isp.h"
#include "tiziano_dmsc.h"
#include "tiziano_params.h"
#include "tiziano_map.h"

int32_t tisp_dmsc_out_opt_cfg(tisp_dmsc_t *dmsc)
{
	system_reg_write(((tisp_core_t *)dmsc->core)->priv_data, DMSC_ADDR_OUT_OPT, dmsc->dmsc_out_opt);

	return 0;
}

int32_t tisp_dmsc_uu_np_cfg(tisp_dmsc_t *dmsc)
{
	system_reg_write(((tisp_core_t *)dmsc->core)->priv_data, DMSC_ADDR_UU_NP_ARRAY_RG0,
			 (dmsc->dmsc_uu_np_array[1 ] << 16) |
			 dmsc->dmsc_uu_np_array[0 ]);
	system_reg_write(((tisp_core_t *)dmsc->core)->priv_data, DMSC_ADDR_UU_NP_ARRAY_RG1,
			 (dmsc->dmsc_uu_np_array[3 ] << 16) |
			 dmsc->dmsc_uu_np_array[2 ]);
	system_reg_write(((tisp_core_t *)dmsc->core)->priv_data, DMSC_ADDR_UU_NP_ARRAY_RG2,
			 (dmsc->dmsc_uu_np_array[5 ] << 16) |
			 dmsc->dmsc_uu_np_array[4 ]);
	system_reg_write(((tisp_core_t *)dmsc->core)->priv_data, DMSC_ADDR_UU_NP_ARRAY_RG3,
			 (dmsc->dmsc_uu_np_array[7 ] << 16) |
			 dmsc->dmsc_uu_np_array[6 ]);
	system_reg_write(((tisp_core_t *)dmsc->core)->priv_data, DMSC_ADDR_UU_NP_ARRAY_RG4,
			 (dmsc->dmsc_uu_np_array[9 ] << 16) |
			 dmsc->dmsc_uu_np_array[8 ]);
	system_reg_write(((tisp_core_t *)dmsc->core)->priv_data, DMSC_ADDR_UU_NP_ARRAY_RG5,
			 (dmsc->dmsc_uu_np_array[11] << 16) |
			 dmsc->dmsc_uu_np_array[10]);
	system_reg_write(((tisp_core_t *)dmsc->core)->priv_data, DMSC_ADDR_UU_NP_ARRAY_RG6,
			 (dmsc->dmsc_uu_np_array[13] << 16) |
			 dmsc->dmsc_uu_np_array[12]);
	system_reg_write(((tisp_core_t *)dmsc->core)->priv_data, DMSC_ADDR_UU_NP_ARRAY_RG7,
			 (dmsc->dmsc_uu_np_array[15] << 16) |
			 dmsc->dmsc_uu_np_array[14]);

	return 0;
}

int32_t tisp_dmsc_sp_d_sigma_3_np_cfg(tisp_dmsc_t *dmsc)
{
	uint32_t reg_value;

	reg_value = (dmsc->dmsc_sp_d_sigma_3_np_array[5] << 25) |
		(dmsc->dmsc_sp_d_sigma_3_np_array[4] << 20) |
		(dmsc->dmsc_sp_d_sigma_3_np_array[3] << 15) |
		(dmsc->dmsc_sp_d_sigma_3_np_array[2] << 10) |
		(dmsc->dmsc_sp_d_sigma_3_np_array[1] << 5) |
		(dmsc->dmsc_sp_d_sigma_3_np_array[0]);

	system_reg_write(((tisp_core_t *)dmsc->core)->priv_data, DMSC_ADDR_SP_D_SIGMA_3_NP_ARRAY_RG0, reg_value);

	reg_value = (dmsc->dmsc_sp_d_sigma_3_np_array[11] << 25) |
		(dmsc->dmsc_sp_d_sigma_3_np_array[10] << 20) |
		(dmsc->dmsc_sp_d_sigma_3_np_array[9 ] << 15) |
		(dmsc->dmsc_sp_d_sigma_3_np_array[8 ] << 10) |
		(dmsc->dmsc_sp_d_sigma_3_np_array[7 ] << 5) |
		(dmsc->dmsc_sp_d_sigma_3_np_array[6 ]);

	system_reg_write(((tisp_core_t *)dmsc->core)->priv_data, DMSC_ADDR_SP_D_SIGMA_3_NP_ARRAY_RG1, reg_value);

	reg_value = (dmsc->dmsc_sp_d_sigma_3_np_array[15] << 15) |
		(dmsc->dmsc_sp_d_sigma_3_np_array[14] << 10) |
		(dmsc->dmsc_sp_d_sigma_3_np_array[13] <<  5) |
		(dmsc->dmsc_sp_d_sigma_3_np_array[12]);

	system_reg_write(((tisp_core_t *)dmsc->core)->priv_data, DMSC_ADDR_SP_D_SIGMA_3_NP_ARRAY_RG2, reg_value);

	return 0;
}

int32_t tisp_dmsc_sp_d_w_wei_np_cfg(tisp_dmsc_t *dmsc)
{
	uint32_t reg_value;

	reg_value = (dmsc->dmsc_sp_d_w_wei_np_array[4] << 24) |
		(dmsc->dmsc_sp_d_w_wei_np_array[3] << 18) |
		(dmsc->dmsc_sp_d_w_wei_np_array[2] << 12) |
		(dmsc->dmsc_sp_d_w_wei_np_array[1] << 6) |
		(dmsc->dmsc_sp_d_w_wei_np_array[0]);

	system_reg_write(((tisp_core_t *)dmsc->core)->priv_data, DMSC_ADDR_SP_D_W_WEI_NP_ARRAY_RG0, reg_value);

	reg_value = (dmsc->dmsc_sp_d_w_wei_np_array[9] << 24) |
		(dmsc->dmsc_sp_d_w_wei_np_array[8] << 18) |
		(dmsc->dmsc_sp_d_w_wei_np_array[7] << 12) |
		(dmsc->dmsc_sp_d_w_wei_np_array[6] << 6) |
		(dmsc->dmsc_sp_d_w_wei_np_array[5]);

	system_reg_write(((tisp_core_t *)dmsc->core)->priv_data, DMSC_ADDR_SP_D_W_WEI_NP_ARRAY_RG1, reg_value);

	reg_value = (dmsc->dmsc_sp_d_w_wei_np_array[14] << 24) |
		(dmsc->dmsc_sp_d_w_wei_np_array[13] << 18) |
		(dmsc->dmsc_sp_d_w_wei_np_array[12] << 12) |
		(dmsc->dmsc_sp_d_w_wei_np_array[11] << 6) |
		(dmsc->dmsc_sp_d_w_wei_np_array[10]);

	system_reg_write(((tisp_core_t *)dmsc->core)->priv_data, DMSC_ADDR_SP_D_W_WEI_NP_ARRAY_RG2, reg_value);

	system_reg_write(((tisp_core_t *)dmsc->core)->priv_data, DMSC_ADDR_SP_D_W_WEI_NP_ARRAY_RG3,
			 dmsc->dmsc_sp_d_w_wei_np_array[15]);

	return 0;
}

int32_t tisp_dmsc_sp_d_b_wei_np_cfg(tisp_dmsc_t *dmsc)
{
	uint32_t reg_value;

	reg_value = (dmsc->dmsc_sp_d_b_wei_np_array[4] << 24) |
		(dmsc->dmsc_sp_d_b_wei_np_array[3] << 18) |
		(dmsc->dmsc_sp_d_b_wei_np_array[2] << 12) |
		(dmsc->dmsc_sp_d_b_wei_np_array[1] << 6) |
		(dmsc->dmsc_sp_d_b_wei_np_array[0]);

	system_reg_write(((tisp_core_t *)dmsc->core)->priv_data, DMSC_ADDR_SP_D_B_WEI_NP_ARRAY_RG0, reg_value);

	reg_value = (dmsc->dmsc_sp_d_b_wei_np_array[9] << 24) |
		(dmsc->dmsc_sp_d_b_wei_np_array[8] << 18) |
		(dmsc->dmsc_sp_d_b_wei_np_array[7] << 12) |
		(dmsc->dmsc_sp_d_b_wei_np_array[6] << 6) |
		(dmsc->dmsc_sp_d_b_wei_np_array[5]);

	system_reg_write(((tisp_core_t *)dmsc->core)->priv_data, DMSC_ADDR_SP_D_B_WEI_NP_ARRAY_RG1, reg_value);

	reg_value = (dmsc->dmsc_sp_d_b_wei_np_array[14] << 24) |
		(dmsc->dmsc_sp_d_b_wei_np_array[13] << 18) |
		(dmsc->dmsc_sp_d_b_wei_np_array[12] << 12) |
		(dmsc->dmsc_sp_d_b_wei_np_array[11] << 6) |
		(dmsc->dmsc_sp_d_b_wei_np_array[10]);

	system_reg_write(((tisp_core_t *)dmsc->core)->priv_data, DMSC_ADDR_SP_D_B_WEI_NP_ARRAY_RG2, reg_value);

	system_reg_write(((tisp_core_t *)dmsc->core)->priv_data, DMSC_ADDR_SP_D_B_WEI_NP_ARRAY_RG3,
			 dmsc->dmsc_sp_d_b_wei_np_array[15]);

	return 0;
}

int32_t tisp_dmsc_sp_ud_w_wei_np_cfg(tisp_dmsc_t *dmsc)
{
	uint32_t reg_value;

	reg_value = (dmsc->dmsc_sp_ud_w_wei_np_array[4] << 24) |
		(dmsc->dmsc_sp_ud_w_wei_np_array[3] << 18) |
		(dmsc->dmsc_sp_ud_w_wei_np_array[2] << 12) |
		(dmsc->dmsc_sp_ud_w_wei_np_array[1] << 6) |
		(dmsc->dmsc_sp_ud_w_wei_np_array[0]);

	system_reg_write(((tisp_core_t *)dmsc->core)->priv_data, DMSC_ADDR_SP_UD_W_WEI_NP_ARRAY_RG0, reg_value);

	reg_value = (dmsc->dmsc_sp_ud_w_wei_np_array[9] << 24) |
		(dmsc->dmsc_sp_ud_w_wei_np_array[8] << 18) |
		(dmsc->dmsc_sp_ud_w_wei_np_array[7] << 12) |
		(dmsc->dmsc_sp_ud_w_wei_np_array[6] << 6) |
		(dmsc->dmsc_sp_ud_w_wei_np_array[5]);

	system_reg_write(((tisp_core_t *)dmsc->core)->priv_data, DMSC_ADDR_SP_UD_W_WEI_NP_ARRAY_RG1, reg_value);

	reg_value = (dmsc->dmsc_sp_ud_w_wei_np_array[14] << 24) |
		(dmsc->dmsc_sp_ud_w_wei_np_array[13] << 18) |
		(dmsc->dmsc_sp_ud_w_wei_np_array[12] << 12) |
		(dmsc->dmsc_sp_ud_w_wei_np_array[11] << 6) |
		(dmsc->dmsc_sp_ud_w_wei_np_array[10]);

	system_reg_write(((tisp_core_t *)dmsc->core)->priv_data, DMSC_ADDR_SP_UD_W_WEI_NP_ARRAY_RG2, reg_value);


	system_reg_write(((tisp_core_t *)dmsc->core)->priv_data, DMSC_ADDR_SP_UD_W_WEI_NP_ARRAY_RG3,
			 dmsc->dmsc_sp_ud_w_wei_np_array[15]);

	return 0;
}

int32_t tisp_dmsc_sp_ud_b_wei_np_cfg(tisp_dmsc_t *dmsc)
{
	uint32_t reg_value;

	reg_value = (dmsc->dmsc_sp_ud_b_wei_np_array[4] << 24) |
		(dmsc->dmsc_sp_ud_b_wei_np_array[3] << 18) |
		(dmsc->dmsc_sp_ud_b_wei_np_array[2] << 12) |
		(dmsc->dmsc_sp_ud_b_wei_np_array[1] << 6) |
		(dmsc->dmsc_sp_ud_b_wei_np_array[0]);

	system_reg_write(((tisp_core_t *)dmsc->core)->priv_data, DMSC_ADDR_SP_UD_B_WEI_NP_ARRAY_RG0, reg_value);

	reg_value = (dmsc->dmsc_sp_ud_b_wei_np_array[9] << 24) |
		(dmsc->dmsc_sp_ud_b_wei_np_array[8] << 18) |
		(dmsc->dmsc_sp_ud_b_wei_np_array[7] << 12) |
		(dmsc->dmsc_sp_ud_b_wei_np_array[6] << 6) |
		(dmsc->dmsc_sp_ud_b_wei_np_array[5]);

	system_reg_write(((tisp_core_t *)dmsc->core)->priv_data, DMSC_ADDR_SP_UD_B_WEI_NP_ARRAY_RG1, reg_value);

	reg_value = (dmsc->dmsc_sp_ud_b_wei_np_array[14] << 24) |
		(dmsc->dmsc_sp_ud_b_wei_np_array[13] << 18) |
		(dmsc->dmsc_sp_ud_b_wei_np_array[12] << 12) |
		(dmsc->dmsc_sp_ud_b_wei_np_array[11] << 6) |
		(dmsc->dmsc_sp_ud_b_wei_np_array[10]);

	system_reg_write(((tisp_core_t *)dmsc->core)->priv_data, DMSC_ADDR_SP_UD_B_WEI_NP_ARRAY_RG2, reg_value);


	system_reg_write(((tisp_core_t *)dmsc->core)->priv_data, DMSC_ADDR_SP_UD_B_WEI_NP_ARRAY_RG3,
			 dmsc->dmsc_sp_ud_b_wei_np_array[15]);

	return 0;
}


int32_t tisp_dmsc_dir_par_cfg(tisp_dmsc_t *dmsc)
{
	int32_t data_tmp1, data_tmp2;
	uint32_t reg_value;

	data_tmp1 = dmsc->dmsc_dir_par_array[0] - (dmsc->dmsc_dir_par_array[0] >> 3);
	data_tmp2 = dmsc->dmsc_hv_thres_1_intp - (dmsc->dmsc_hv_thres_1_intp >> 3);
	reg_value = (data_tmp1 << 16) |	(data_tmp2);
	system_reg_write(((tisp_core_t *)dmsc->core)->priv_data, DMSC_ADDR_WIN5_HV_EDGE_THRES, reg_value);

	reg_value = (dmsc->dmsc_dir_par_array[0] << 16) |
		(dmsc->dmsc_hv_thres_1_intp);
	system_reg_write(((tisp_core_t *)dmsc->core)->priv_data, DMSC_ADDR_HV_EDGE_THRES, reg_value);

	reg_value = (dmsc->dmsc_dir_par_array[1] << 24) |
		(dmsc->dmsc_dir_par_array[2] << 16) |
		(dmsc->dmsc_hv_stren_intp);
	system_reg_write(((tisp_core_t *)dmsc->core)->priv_data, DMSC_ADDR_HV_EDGE, reg_value);

	reg_value = (dmsc->dmsc_dir_par_array[3] << 16) |
		(dmsc->dmsc_aa_thres_1_intp);
	system_reg_write(((tisp_core_t *)dmsc->core)->priv_data, DMSC_ADDR_AA_EDGE_THRES, reg_value);

	reg_value = (dmsc->dmsc_dir_par_array[4] << 24) |
		(dmsc->dmsc_dir_par_array[5] << 16) |
		(dmsc->dmsc_aa_stren_intp);
	system_reg_write(((tisp_core_t *)dmsc->core)->priv_data, DMSC_ADDR_AA_EDGE, reg_value);

	reg_value = (dmsc->dmsc_dir_par_array[6] << 16) |
		(dmsc->dmsc_hvaa_thres_1_intp);
	system_reg_write(((tisp_core_t *)dmsc->core)->priv_data, DMSC_ADDR_HVAA_EDGE_THRES, reg_value);

	reg_value = (dmsc->dmsc_dir_par_array[7] << 24) |
		(dmsc->dmsc_dir_par_array[8] << 16) |
		(dmsc->dmsc_hvaa_stren_intp);
	system_reg_write(((tisp_core_t *)dmsc->core)->priv_data, DMSC_ADDR_HVAA_EDGE, reg_value);

	return 0;
}

int32_t tisp_dmsc_uu_par_cfg(tisp_dmsc_t *dmsc)
{
	uint32_t reg_value;

	reg_value = (dmsc->dmsc_uu_thres_intp << 16) |
		(dmsc->dmsc_uu_stren_intp);
	system_reg_write(((tisp_core_t *)dmsc->core)->priv_data, DMSC_ADDR_UU_THRES, reg_value);

	reg_value = (dmsc->dmsc_uu_par_array[0] << 10) |
		(dmsc->dmsc_uu_par_array[1] << 8) |
		(dmsc->dmsc_uu_par_array[2]);
	system_reg_write(((tisp_core_t *)dmsc->core)->priv_data, DMSC_ADDR_G_STD_STREN, reg_value);

	return 0;
}

int32_t tisp_dmsc_alias_par_cfg(tisp_dmsc_t *dmsc)
{
	int32_t data_tmp1;
	uint32_t reg_value;

	data_tmp1 = dmsc->dmsc_alias_dir_thres_intp - dmsc->dmsc_alias_par_array[3];
	reg_value = (data_tmp1 << 16) |	(dmsc->dmsc_alias_dir_thres_intp);
	system_reg_write(((tisp_core_t *)dmsc->core)->priv_data, DMSC_ADDR_ALIAS_DIR_DIFF, reg_value);

	reg_value = (dmsc->dmsc_alias_stren_intp   << 18) |
		(dmsc->dmsc_alias_par_array[0] << 10) |
		(dmsc->dmsc_alias_par_array[2] << 6) |
		(dmsc->dmsc_alias_par_array[1]);
	system_reg_write(((tisp_core_t *)dmsc->core)->priv_data, DMSC_ADDR_ALIAS_STREN, reg_value);

	reg_value = (dmsc->dmsc_alias_thres_2_intp << 16) |
		(dmsc->dmsc_alias_thres_1_intp);
	system_reg_write(((tisp_core_t *)dmsc->core)->priv_data, DMSC_ADDR_ALIAS_THRES, reg_value);

	return 0;
}

int32_t tisp_dmsc_nor_par_cfg(tisp_dmsc_t *dmsc)
{
	//int32_t data_tmp1, data_tmp2;
	uint32_t reg_value;

	reg_value = (dmsc->dmsc_nor_par_array[0] << 16) |
		(dmsc->dmsc_nor_alias_thres_intp);
	system_reg_write(((tisp_core_t *)dmsc->core)->priv_data, DMSC_ADDR_NOR_ALIAS_BLUR, reg_value);

	reg_value = (dmsc->dmsc_nor_par_array[1] << 16) |
		(dmsc->dmsc_nor_par_array[2] << 6) |
		(dmsc->dmsc_nor_par_array[3]);
	system_reg_write(((tisp_core_t *)dmsc->core)->priv_data, DMSC_ADDR_NOR_UU_WEI_STREN, reg_value);

	return 0;
}

int32_t tisp_dmsc_sp_d_par_cfg(tisp_dmsc_t *dmsc)
{
	int32_t data_tmp1, data_tmp2;
	uint32_t reg_value;

	data_tmp1 = 16 - dmsc->dmsc_sp_d_par_array[0];
	data_tmp2 = 8 - dmsc->dmsc_sp_d_par_array[1];

	reg_value = (data_tmp2 << 21) |
		(dmsc->dmsc_sp_d_par_array[1] << 17) |
		(data_tmp1 << 12) |
		(dmsc->dmsc_sp_d_par_array[0] << 7) |
		(dmsc->dmsc_sp_d_par_array[2] << 6) |
		(dmsc->dmsc_sp_d_par_array[3] << 4) |
		(dmsc->dmsc_sp_d_par_array[3] << 2) |
		(dmsc->dmsc_sp_d_par_array[3]);
	system_reg_write(((tisp_core_t *)dmsc->core)->priv_data, DMSC_ADDR_SP_D_V2_SIGMA, reg_value);

	reg_value = (dmsc->dmsc_sp_d_w_stren_intp << 16) |
		(dmsc->dmsc_sp_d_b_stren_intp);
	system_reg_write(((tisp_core_t *)dmsc->core)->priv_data, DMSC_ADDR_SP_D_W_SP_STREN, reg_value);

	reg_value = (dmsc->dmsc_sp_d_par_array[7] << 16) |
		(dmsc->dmsc_sp_d_par_array[6]);
	system_reg_write(((tisp_core_t *)dmsc->core)->priv_data, DMSC_ADDR_SP_D_LL_STREN, reg_value);

	reg_value = (dmsc->dmsc_sp_d_brig_thres_intp << 16) |
		(dmsc->dmsc_sp_d_dark_thres_intp);
	system_reg_write(((tisp_core_t *)dmsc->core)->priv_data, DMSC_ADDR_SP_D_LL_THRES, reg_value);

	reg_value = (dmsc->dmsc_sp_d_par_array[4]);
	system_reg_write(((tisp_core_t *)dmsc->core)->priv_data, DMSC_ADDR_SP_D_W_LIMIT, reg_value);

	reg_value = (dmsc->dmsc_sp_d_par_array[5]);
	system_reg_write(((tisp_core_t *)dmsc->core)->priv_data, DMSC_ADDR_SP_D_B_LIMIT, reg_value);

	return 0;
}

int32_t tisp_dmsc_sp_ud_par_cfg(tisp_dmsc_t *dmsc)
{
	int32_t data_tmp1, data_tmp2;
	uint32_t reg_value;

	data_tmp1 = 8 - dmsc->dmsc_sp_ud_par_array[2];
	data_tmp2 = 8 - dmsc->dmsc_sp_ud_par_array[0] - dmsc->dmsc_sp_ud_par_array[1];

	reg_value = (data_tmp1 << 20) |
		(dmsc->dmsc_sp_ud_par_array[2] << 16) |
		(data_tmp2 << 12) |
		(dmsc->dmsc_sp_ud_par_array[1] << 8) |
		(dmsc->dmsc_sp_ud_par_array[0] << 4) |
		(dmsc->dmsc_sp_ud_par_array[3] << 2) |
		(dmsc->dmsc_sp_ud_par_array[3]);
	system_reg_write(((tisp_core_t *)dmsc->core)->priv_data, DMSC_ADDR_SP_UD_V2_SIGMA, reg_value);

	reg_value = (dmsc->dmsc_sp_ud_w_stren_intp << 16) |
		(dmsc->dmsc_sp_ud_b_stren_intp);
	system_reg_write(((tisp_core_t *)dmsc->core)->priv_data, DMSC_ADDR_SP_UD_W_SP_STREN, reg_value);

	reg_value = (dmsc->dmsc_sp_ud_par_array[7] << 16) |
		(dmsc->dmsc_sp_ud_par_array[6]);
	system_reg_write(((tisp_core_t *)dmsc->core)->priv_data, DMSC_ADDR_SP_UD_LL_STREN, reg_value);

	reg_value = (dmsc->dmsc_sp_ud_brig_thres_intp << 16) |
		(dmsc->dmsc_sp_ud_dark_thres_intp);
	system_reg_write(((tisp_core_t *)dmsc->core)->priv_data, DMSC_ADDR_SP_UD_LL_THRES, reg_value);

	reg_value = (dmsc->dmsc_sp_ud_par_array[4]);
	system_reg_write(((tisp_core_t *)dmsc->core)->priv_data, DMSC_ADDR_SP_UD_W_LIMIT, reg_value);

	reg_value = (dmsc->dmsc_sp_ud_par_array[5]);
	system_reg_write(((tisp_core_t *)dmsc->core)->priv_data, DMSC_ADDR_SP_UD_B_LIMIT, reg_value);

	return 0;
}


int32_t tisp_dmsc_sp_alias_par_cfg(tisp_dmsc_t *dmsc)
{
	uint32_t reg_value;

	reg_value = (dmsc->dmsc_sp_alias_thres_intp   << 16) |
		(dmsc->dmsc_sp_alias_par_array[0]);
	system_reg_write(((tisp_core_t *)dmsc->core)->priv_data, DMSC_ADDR_SP_NOR_ALIAS_THRES, reg_value);

	return 0;
}

int32_t tisp_dmsc_rgb_alias_par_cfg(tisp_dmsc_t *dmsc)
{
	uint32_t reg_value;

	reg_value = (dmsc->dmsc_rgb_alias_par_array[0] << 16) |
		(dmsc->dmsc_rgb_dir_thres_intp);
	system_reg_write(((tisp_core_t *)dmsc->core)->priv_data, DMSC_ADDR_ALIAS_FUSION_THRES, reg_value);

	reg_value = (dmsc->dmsc_sp_alias_par_array[1]  << 16) |
		(dmsc->dmsc_rgb_alias_stren_intp   << 5) |
		(dmsc->dmsc_rgb_alias_par_array[1]);
	system_reg_write(((tisp_core_t *)dmsc->core)->priv_data, DMSC_ADDR_ALIAS_DIR_INTP_SP_STREN, reg_value);

	return 0;
}


int32_t tisp_dmsc_fc_par_cfg(tisp_dmsc_t *dmsc)
{
	int32_t data_tmp1, data_tmp2;
	uint32_t reg_value;

	reg_value = (dmsc->dmsc_fc_alias_stren_intp << 21) |
		(dmsc->dmsc_fc_t1_stren_intp << 14) |
		(dmsc->dmsc_fc_t2_stren_intp << 7) |
		(dmsc->dmsc_fc_par_array[8]);
	system_reg_write(((tisp_core_t *)dmsc->core)->priv_data, DMSC_ADDR_FC_ALIAS_STREN, reg_value);

	reg_value = (dmsc->dmsc_fc_par_array[1] << 16) | (dmsc->dmsc_fc_t1_thres_intp);
	system_reg_write(((tisp_core_t *)dmsc->core)->priv_data, DMSC_ADDR_FC_ALIAS_THRES, reg_value);

	reg_value = (dmsc->dmsc_fc_par_array[2] << 16) | (dmsc->dmsc_fc_par_array[3]);
	system_reg_write(((tisp_core_t *)dmsc->core)->priv_data, DMSC_ADDR_FC_NOR_SAT_THRES, reg_value);

	data_tmp1 = dmsc->dmsc_fc_t1_thres_intp + dmsc->dmsc_fc_par_array[4];
	reg_value = (data_tmp1 << 16) |	(dmsc->dmsc_fc_par_array[7]);
	system_reg_write(((tisp_core_t *)dmsc->core)->priv_data, DMSC_ADDR_FC_TEXT_THRES, reg_value);

	reg_value = (dmsc->dmsc_fc_par_array[0] << 16) |
		(dmsc->dmsc_fc_par_array[0] << 6) |
		(dmsc->dmsc_fc_par_array[0]);
	system_reg_write(((tisp_core_t *)dmsc->core)->priv_data, DMSC_ADDR_FC_NOR_EDGE_SLOPE, reg_value);

	reg_value = (dmsc->dmsc_fc_t3_stren_intp);
	system_reg_write(((tisp_core_t *)dmsc->core)->priv_data, DMSC_ADDR_FC_TEXT_2_STREN, reg_value);

	data_tmp1 = dmsc->dmsc_fc_t1_thres_intp + dmsc->dmsc_fc_par_array[4] + dmsc->dmsc_fc_par_array[5];
	reg_value = (dmsc->dmsc_fc_par_array[0] << 16) | (data_tmp1);
	system_reg_write(((tisp_core_t *)dmsc->core)->priv_data, DMSC_ADDR_FC_TEXT_2_THRES, reg_value);

	data_tmp1 = dmsc->dmsc_fc_t1_thres_intp + dmsc->dmsc_fc_par_array[4];
	data_tmp2 = dmsc->dmsc_fc_t1_thres_intp + dmsc->dmsc_fc_par_array[4] + dmsc->dmsc_fc_par_array[5];
	reg_value = (data_tmp2 << 16) |	(data_tmp1);
	system_reg_write(((tisp_core_t *)dmsc->core)->priv_data, DMSC_ADDR_FC_THRES_H_1, reg_value);

	data_tmp1 = dmsc->dmsc_fc_t1_thres_intp + dmsc->dmsc_fc_par_array[4] +
		dmsc->dmsc_fc_par_array[5] + dmsc->dmsc_fc_par_array[6];
	if(data_tmp1 > 4095)
		data_tmp1 = 4095;
	reg_value = (data_tmp1);
	system_reg_write(((tisp_core_t *)dmsc->core)->priv_data, DMSC_ADDR_FC_THRES_H_2, reg_value);

	return 0;
}

int32_t tisp_dmsc_intp(tisp_dmsc_t *dmsc, int32_t gain_value)
{
	int32_t x_int, x_fra;

	x_int = gain_value >> 16;
	x_fra = gain_value & 0xffff;

	//hv_thres_1 intp value
	dmsc->dmsc_hv_thres_1_intp = tisp_simple_intp(x_int, x_fra,
						dmsc->dmsc_hv_thres_1_array);
	//hv_stren intp value
	dmsc->dmsc_hv_stren_intp = tisp_simple_intp(x_int, x_fra,
					      dmsc->dmsc_hv_stren_array);
	//aa_thres_1 intp value
	dmsc->dmsc_aa_thres_1_intp = tisp_simple_intp(x_int, x_fra,
						dmsc->dmsc_aa_thres_1_array);
	//aa_stren intp value
	dmsc->dmsc_aa_stren_intp = tisp_simple_intp(x_int, x_fra,
					      dmsc->dmsc_aa_stren_array);
	//hvaa_thres_1 intp value
	dmsc->dmsc_hvaa_thres_1_intp = tisp_simple_intp(x_int, x_fra,
						  dmsc->dmsc_hvaa_thres_1_array);
	//hvaa_stren intp value
	dmsc->dmsc_hvaa_stren_intp = tisp_simple_intp(x_int, x_fra,
						dmsc->dmsc_hvaa_stren_array);
	//uu_thres intp value
	dmsc->dmsc_uu_thres_intp = tisp_simple_intp(x_int, x_fra,
					      dmsc->dmsc_uu_thres_array);
	//uu_stren intp value
	dmsc->dmsc_uu_stren_intp = tisp_simple_intp(x_int, x_fra,
					      dmsc->dmsc_uu_stren_array);
	//alias_stren intp value
	dmsc->dmsc_alias_stren_intp = tisp_simple_intp(x_int, x_fra,
						 dmsc->dmsc_alias_stren_array);
	//alias_thres_1 intp value
	dmsc->dmsc_alias_thres_1_intp = tisp_simple_intp(x_int, x_fra,
						   dmsc->dmsc_alias_thres_1_array);
	//alias_thres_2 intp value
	dmsc->dmsc_alias_thres_2_intp = tisp_simple_intp(x_int, x_fra,
						   dmsc->dmsc_alias_thres_2_array);
	//alias_dir_thres intp value
	dmsc->dmsc_alias_dir_thres_intp = tisp_simple_intp(x_int, x_fra,
						     dmsc->dmsc_alias_dir_thres_array);
	//nor_alias_thres intp
	dmsc->dmsc_nor_alias_thres_intp = tisp_simple_intp(x_int, x_fra,
						     dmsc->dmsc_nor_alias_thres_array);
	//sp_d_w_stren intp value
	dmsc->dmsc_sp_d_w_stren_intp = tisp_simple_intp(x_int, x_fra,
						  dmsc->dmsc_sp_d_w_stren_array);
	//sp_d_b_stren intp value
	dmsc->dmsc_sp_d_b_stren_intp = tisp_simple_intp(x_int, x_fra,
						  dmsc->dmsc_sp_d_b_stren_array);
	//sp_d_brig_thres intp value
	dmsc->dmsc_sp_d_brig_thres_intp = tisp_simple_intp(x_int, x_fra,
						     dmsc->dmsc_sp_d_brig_thres_array);
	//sp_d_dark_thres intp value
	dmsc->dmsc_sp_d_dark_thres_intp = tisp_simple_intp(x_int, x_fra,
						     dmsc->dmsc_sp_d_dark_thres_array);
	//sp_ud_w_stren intp value
	dmsc->dmsc_sp_ud_w_stren_intp = tisp_simple_intp(x_int, x_fra,
						   dmsc->dmsc_sp_ud_w_stren_array);
	//sp_ud_b_stren intp value
	dmsc->dmsc_sp_ud_b_stren_intp = tisp_simple_intp(x_int, x_fra,
						   dmsc->dmsc_sp_ud_b_stren_array);
	//sp_ud_brig_thres intp value
	dmsc->dmsc_sp_ud_brig_thres_intp = tisp_simple_intp(x_int, x_fra,
						      dmsc->dmsc_sp_ud_brig_thres_array);
	//sp_ud_dark_thres intp value
	dmsc->dmsc_sp_ud_dark_thres_intp = tisp_simple_intp(x_int, x_fra,
						      dmsc->dmsc_sp_ud_dark_thres_array);
	//sp_alias_thres intp value
	dmsc->dmsc_sp_alias_thres_intp = tisp_simple_intp(x_int, x_fra,
						    dmsc->dmsc_sp_alias_thres_array);
	//rgb_dir_thres intp value
	dmsc->dmsc_rgb_dir_thres_intp = tisp_simple_intp(x_int, x_fra,
						   dmsc->dmsc_rgb_dir_thres_array);
	//rgb_alias_stren intp value
	dmsc->dmsc_rgb_alias_stren_intp = tisp_simple_intp(x_int, x_fra,
						     dmsc->dmsc_rgb_alias_stren_array);
	//fc_alias_stren intp value
	dmsc->dmsc_fc_alias_stren_intp = tisp_simple_intp(x_int, x_fra,
						    dmsc->dmsc_fc_alias_stren_array);
	//fc_t1_thres intp value
	dmsc->dmsc_fc_t1_thres_intp = tisp_simple_intp(x_int, x_fra,
						 dmsc->dmsc_fc_t1_thres_array);
	//fc_t1_stren intp value
	dmsc->dmsc_fc_t1_stren_intp = tisp_simple_intp(x_int, x_fra,
						 dmsc->dmsc_fc_t1_stren_array);
	//fc_t2_stren intp value
	dmsc->dmsc_fc_t2_stren_intp = tisp_simple_intp(x_int, x_fra,
						 dmsc->dmsc_fc_t2_stren_array);
	//fc_t3_stren intp value
	dmsc->dmsc_fc_t3_stren_intp = tisp_simple_intp(x_int, x_fra,
						 dmsc->dmsc_fc_t3_stren_array);

	return 0;
}

int32_t tisp_dmsc_all_reg_refresh(tisp_dmsc_t *dmsc, int gain_value)
{
	tisp_dmsc_intp(dmsc, gain_value);
	tisp_dmsc_out_opt_cfg(dmsc);
	tisp_dmsc_uu_np_cfg(dmsc);
	tisp_dmsc_sp_d_sigma_3_np_cfg(dmsc);
	tisp_dmsc_sp_d_w_wei_np_cfg(dmsc);
	tisp_dmsc_sp_d_b_wei_np_cfg(dmsc);
	tisp_dmsc_sp_ud_w_wei_np_cfg(dmsc);
	tisp_dmsc_sp_ud_b_wei_np_cfg(dmsc);
	tisp_dmsc_dir_par_cfg(dmsc);
	tisp_dmsc_uu_par_cfg(dmsc);
	tisp_dmsc_alias_par_cfg(dmsc);
	tisp_dmsc_nor_par_cfg(dmsc);
	tisp_dmsc_sp_d_par_cfg(dmsc);
	tisp_dmsc_sp_ud_par_cfg(dmsc);
	tisp_dmsc_sp_alias_par_cfg(dmsc);
	tisp_dmsc_rgb_alias_par_cfg(dmsc);
	tisp_dmsc_fc_par_cfg(dmsc);

	return 0;
}

int32_t tisp_dmsc_intp_reg_refresh(tisp_dmsc_t *dmsc, int gain_value)
{
	tisp_dmsc_intp(dmsc, gain_value);
	tisp_dmsc_dir_par_cfg(dmsc);
	tisp_dmsc_uu_par_cfg(dmsc);
	tisp_dmsc_alias_par_cfg(dmsc);
	tisp_dmsc_nor_par_cfg(dmsc);
	tisp_dmsc_sp_d_par_cfg(dmsc);
	tisp_dmsc_sp_ud_par_cfg(dmsc);
	tisp_dmsc_sp_alias_par_cfg(dmsc);
	tisp_dmsc_rgb_alias_par_cfg(dmsc);
	tisp_dmsc_fc_par_cfg(dmsc);

	return 0;
}

uint32_t tisp_dmsc_par_refresh(tisp_dmsc_t *dmsc, uint32_t gain_value, uint32_t gain_thres, uint32_t shadow_en)
{
	/* shadow_en: */
	/* 0: shadow disable */
	/* 1: shadow enable */

	uint32_t gain_diff;

	if(dmsc->gain_old == 0xffffffff){
		dmsc->gain_old = gain_value;
		tisp_dmsc_all_reg_refresh(dmsc, gain_value);
	} else {
		if(gain_value >= dmsc->gain_old)
			gain_diff = gain_value-dmsc->gain_old;
		else
			gain_diff = dmsc->gain_old-gain_value;

		if(gain_diff >= gain_thres) {
			dmsc->gain_old = gain_value;
			tisp_dmsc_intp_reg_refresh(dmsc, gain_value);
		}
	}

	if(shadow_en == 1){
		system_reg_write(((tisp_core_t *)dmsc->core)->priv_data, DMSC_ADDR_SYNC, 1);
	}

	return 0;
}

int32_t tiziano_dmsc_params_refresh(tisp_dmsc_t *dmsc)
{
	memcpy(dmsc->dmsc_uu_np_array,
	       tparams.params_data.TISP_PARAM_DMSC_UU_NP_ARRAY,
	       sizeof(tparams.params_data.TISP_PARAM_DMSC_UU_NP_ARRAY));
	memcpy(dmsc->dmsc_sp_d_sigma_3_np_array,
	       tparams.params_data.TISP_PARAM_DMSC_SP_D_SIGMA_3_NP_ARRAY,
	       sizeof(tparams.params_data.TISP_PARAM_DMSC_SP_D_SIGMA_3_NP_ARRAY));
	memcpy(dmsc->dmsc_sp_d_w_wei_np_array,
	       tparams.params_data.TISP_PARAM_DMSC_SP_D_W_WEI_NP_ARRAY,
	       sizeof(tparams.params_data.TISP_PARAM_DMSC_SP_D_W_WEI_NP_ARRAY));
	memcpy(dmsc->dmsc_sp_d_b_wei_np_array,
	       tparams.params_data.TISP_PARAM_DMSC_SP_D_B_WEI_NP_ARRAY,
	       sizeof(tparams.params_data.TISP_PARAM_DMSC_SP_D_B_WEI_NP_ARRAY));
	memcpy(dmsc->dmsc_sp_ud_w_wei_np_array,
	       tparams.params_data.TISP_PARAM_DMSC_SP_UD_W_WEI_NP_ARRAY,
	       sizeof(tparams.params_data.TISP_PARAM_DMSC_SP_UD_W_WEI_NP_ARRAY));
	memcpy(dmsc->dmsc_sp_ud_b_wei_np_array,
	       tparams.params_data.TISP_PARAM_DMSC_SP_UD_B_WEI_NP_ARRAY,
	       sizeof(tparams.params_data.TISP_PARAM_DMSC_SP_UD_B_WEI_NP_ARRAY));
	memcpy(&dmsc->dmsc_out_opt,
	       tparams.params_data.TISP_PARAM_DMSC_OUT_OPT,
	       sizeof(tparams.params_data.TISP_PARAM_DMSC_OUT_OPT));
	memcpy(dmsc->dmsc_hv_thres_1_array,
	       tparams.params_data.TISP_PARAM_DMSC_HV_THRES_1_ARRAY,
	       sizeof(tparams.params_data.TISP_PARAM_DMSC_HV_THRES_1_ARRAY));
	memcpy(dmsc->dmsc_hv_stren_array,
	       tparams.params_data.TISP_PARAM_DMSC_HV_STREN_ARRAY,
	       sizeof(tparams.params_data.TISP_PARAM_DMSC_HV_STREN_ARRAY));
	memcpy(dmsc->dmsc_aa_thres_1_array,
	       tparams.params_data.TISP_PARAM_DMSC_AA_THRES_1_ARRAY,
	       sizeof(tparams.params_data.TISP_PARAM_DMSC_AA_THRES_1_ARRAY));
	memcpy(dmsc->dmsc_aa_stren_array,
	       tparams.params_data.TISP_PARAM_DMSC_AA_STREN_ARRAY,
	       sizeof(tparams.params_data.TISP_PARAM_DMSC_AA_STREN_ARRAY));
	memcpy(dmsc->dmsc_hvaa_thres_1_array,
	       tparams.params_data.TISP_PARAM_DMSC_HVAA_THRES_1_ARRAY,
	       sizeof(tparams.params_data.TISP_PARAM_DMSC_HVAA_THRES_1_ARRAY));
	memcpy(dmsc->dmsc_hvaa_stren_array,
	       tparams.params_data.TISP_PARAM_DMSC_HVAA_STREN_ARRAY,
	       sizeof(tparams.params_data.TISP_PARAM_DMSC_HVAA_STREN_ARRAY));
	memcpy(dmsc->dmsc_dir_par_array,
	       tparams.params_data.TISP_PARAM_DMSC_DIR_PAR_ARRAY,
	       sizeof(tparams.params_data.TISP_PARAM_DMSC_DIR_PAR_ARRAY));
	memcpy(dmsc->dmsc_uu_thres_array,
	       tparams.params_data.TISP_PARAM_DMSC_UU_THRES_ARRAY,
	       sizeof(tparams.params_data.TISP_PARAM_DMSC_UU_THRES_ARRAY));
	memcpy(dmsc->dmsc_uu_stren_array,
	       tparams.params_data.TISP_PARAM_DMSC_UU_STREN_ARRAY,
	       sizeof(tparams.params_data.TISP_PARAM_DMSC_UU_STREN_ARRAY));
	memcpy(dmsc->dmsc_uu_par_array,
	       tparams.params_data.TISP_PARAM_DMSC_UU_PAR_ARRAY,
	       sizeof(tparams.params_data.TISP_PARAM_DMSC_UU_PAR_ARRAY));
	memcpy(dmsc->dmsc_alias_stren_array,
	       tparams.params_data.TISP_PARAM_DMSC_ALIAS_STREN_ARRAY,
	       sizeof(tparams.params_data.TISP_PARAM_DMSC_ALIAS_STREN_ARRAY));
	memcpy(dmsc->dmsc_alias_thres_1_array,
	       tparams.params_data.TISP_PARAM_DMSC_ALIAS_THRES_1_ARRAY,
	       sizeof(tparams.params_data.TISP_PARAM_DMSC_ALIAS_THRES_1_ARRAY));
	memcpy(dmsc->dmsc_alias_thres_2_array,
	       tparams.params_data.TISP_PARAM_DMSC_ALIAS_THRES_2_ARRAY,
	       sizeof(tparams.params_data.TISP_PARAM_DMSC_ALIAS_THRES_2_ARRAY));
	memcpy(dmsc->dmsc_alias_dir_thres_array,
	       tparams.params_data.TISP_PARAM_DMSC_ALIAS_DIR_THRES_ARRAY,
	       sizeof(tparams.params_data.TISP_PARAM_DMSC_ALIAS_DIR_THRES_ARRAY));
	memcpy(dmsc->dmsc_alias_par_array,
	       tparams.params_data.TISP_PARAM_DMSC_ALIAS_PAR_ARRAY,
	       sizeof(tparams.params_data.TISP_PARAM_DMSC_ALIAS_PAR_ARRAY));
	memcpy(dmsc->dmsc_nor_alias_thres_array,
	       tparams.params_data.TISP_PARAM_DMSC_NOR_ALIAS_THRES_ARRAY,
	       sizeof(tparams.params_data.TISP_PARAM_DMSC_NOR_ALIAS_THRES_ARRAY));
	memcpy(dmsc->dmsc_nor_par_array,
	       tparams.params_data.TISP_PARAM_DMSC_NOR_PAR_ARRAY,
	       sizeof(tparams.params_data.TISP_PARAM_DMSC_NOR_PAR_ARRAY));
	memcpy(dmsc->dmsc_sp_d_w_stren_array,
	       tparams.params_data.TISP_PARAM_DMSC_SP_D_W_STREN_ARRAY,
	       sizeof(tparams.params_data.TISP_PARAM_DMSC_SP_D_W_STREN_ARRAY));
	memcpy(dmsc->dmsc_sp_d_b_stren_array,
	       tparams.params_data.TISP_PARAM_DMSC_SP_D_B_STREN_ARRAY,
	       sizeof(tparams.params_data.TISP_PARAM_DMSC_SP_D_B_STREN_ARRAY));
	memcpy(dmsc->dmsc_sp_d_brig_thres_array,
	       tparams.params_data.TISP_PARAM_DMSC_SP_D_BRIG_THRES_ARRAY,
	       sizeof(tparams.params_data.TISP_PARAM_DMSC_SP_D_BRIG_THRES_ARRAY));
	memcpy(dmsc->dmsc_sp_d_dark_thres_array,
	       tparams.params_data.TISP_PARAM_DMSC_SP_D_DARK_THRES_ARRAY,
	       sizeof(tparams.params_data.TISP_PARAM_DMSC_SP_D_DARK_THRES_ARRAY));
	memcpy(dmsc->dmsc_sp_d_par_array,
	       tparams.params_data.TISP_PARAM_DMSC_SP_D_PAR_ARRAY,
	       sizeof(tparams.params_data.TISP_PARAM_DMSC_SP_D_PAR_ARRAY));
	memcpy(dmsc->dmsc_sp_ud_w_stren_array,
	       tparams.params_data.TISP_PARAM_DMSC_SP_UD_W_STREN_ARRAY,
	       sizeof(tparams.params_data.TISP_PARAM_DMSC_SP_UD_W_STREN_ARRAY));
	memcpy(dmsc->dmsc_sp_ud_b_stren_array,
	       tparams.params_data.TISP_PARAM_DMSC_SP_UD_B_STREN_ARRAY,
	       sizeof(tparams.params_data.TISP_PARAM_DMSC_SP_UD_B_STREN_ARRAY));
	memcpy(dmsc->dmsc_sp_ud_brig_thres_array,
	       tparams.params_data.TISP_PARAM_DMSC_SP_UD_BRIG_THRES_ARRAY,
	       sizeof(tparams.params_data.TISP_PARAM_DMSC_SP_UD_BRIG_THRES_ARRAY));
	memcpy(dmsc->dmsc_sp_ud_dark_thres_array,
	       tparams.params_data.TISP_PARAM_DMSC_SP_UD_DARK_THRES_ARRAY,
	       sizeof(tparams.params_data.TISP_PARAM_DMSC_SP_UD_DARK_THRES_ARRAY));
	memcpy(dmsc->dmsc_sp_ud_par_array,
	       tparams.params_data.TISP_PARAM_DMSC_SP_UD_PAR_ARRAY,
	       sizeof(tparams.params_data.TISP_PARAM_DMSC_SP_UD_PAR_ARRAY));
	memcpy(dmsc->dmsc_sp_alias_thres_array,
	       tparams.params_data.TISP_PARAM_DMSC_SP_ALIAS_THRES_ARRAY,
	       sizeof(tparams.params_data.TISP_PARAM_DMSC_SP_ALIAS_THRES_ARRAY));
	memcpy(dmsc->dmsc_sp_alias_par_array,
	       tparams.params_data.TISP_PARAM_DMSC_SP_ALIAS_PAR_ARRAY,
	       sizeof(tparams.params_data.TISP_PARAM_DMSC_SP_ALIAS_PAR_ARRAY));
	memcpy(dmsc->dmsc_rgb_dir_thres_array,
	       tparams.params_data.TISP_PARAM_DMSC_RGB_DIR_THRES_ARRAY,
	       sizeof(tparams.params_data.TISP_PARAM_DMSC_RGB_DIR_THRES_ARRAY));
	memcpy(dmsc->dmsc_rgb_alias_stren_array,
	       tparams.params_data.TISP_PARAM_DMSC_RGB_ALIAS_STREN_ARRAY,
	       sizeof(tparams.params_data.TISP_PARAM_DMSC_RGB_ALIAS_STREN_ARRAY));
	memcpy(dmsc->dmsc_rgb_alias_par_array,
	       tparams.params_data.TISP_PARAM_DMSC_RGB_ALIAS_PAR_ARRAY,
	       sizeof(tparams.params_data.TISP_PARAM_DMSC_RGB_ALIAS_PAR_ARRAY));
	memcpy(dmsc->dmsc_fc_alias_stren_array,
	       tparams.params_data.TISP_PARAM_DMSC_FC_ALIAS_STREN_ARRAY,
	       sizeof(tparams.params_data.TISP_PARAM_DMSC_FC_ALIAS_STREN_ARRAY));
	memcpy(dmsc->dmsc_fc_t1_thres_array,
	       tparams.params_data.TISP_PARAM_DMSC_FC_T1_THRES_ARRAY,
	       sizeof(tparams.params_data.TISP_PARAM_DMSC_FC_T1_THRES_ARRAY));
	memcpy(dmsc->dmsc_fc_t1_stren_array,
	       tparams.params_data.TISP_PARAM_DMSC_FC_T1_STREN_ARRAY,
	       sizeof(tparams.params_data.TISP_PARAM_DMSC_FC_T1_STREN_ARRAY));
	memcpy(dmsc->dmsc_fc_t2_stren_array,
	       tparams.params_data.TISP_PARAM_DMSC_FC_T2_STREN_ARRAY,
	       sizeof(tparams.params_data.TISP_PARAM_DMSC_FC_T2_STREN_ARRAY));
	memcpy(dmsc->dmsc_fc_t3_stren_array,
	       tparams.params_data.TISP_PARAM_DMSC_FC_T3_STREN_ARRAY,
	       sizeof(tparams.params_data.TISP_PARAM_DMSC_FC_T3_STREN_ARRAY));
	memcpy(dmsc->dmsc_fc_par_array,
	       tparams.params_data.TISP_PARAM_DMSC_FC_PAR_ARRAY,
	       sizeof(tparams.params_data.TISP_PARAM_DMSC_FC_PAR_ARRAY));

	return 0;
}

int32_t tiziano_dmsc_dn_params_refresh(tisp_dmsc_t *dmsc)
{
	dmsc->gain_old += 0x200;
	tiziano_dmsc_params_refresh(dmsc);
	tisp_dmsc_all_reg_refresh(dmsc, dmsc->gain_old);

	return 0;
}

int32_t tiziano_dmsc_init(tisp_dmsc_t *dmsc)
{
	dmsc->gain_old = 0xffffffff;
	dmsc->gain_thres = 0x100;
	tiziano_dmsc_params_refresh(dmsc);
	tisp_dmsc_par_refresh(dmsc, 0x10000, 0x10000, 0);

	return 0;
}

int32_t tisp_dmsc_refresh(tisp_dmsc_t *dmsc, uint32_t gain_value)
{
	tisp_dmsc_par_refresh(dmsc, gain_value, dmsc->gain_thres, dmsc->shadow_en);

	return 0;
}

int32_t tisp_dmsc_param_array_get(tisp_dmsc_t *dmsc, int32_t id, void *buf, uint32_t *plen)
{
	uint32_t len;
	void *tmpbuf;

	switch (id) {
	case TISP_PARAM_DMSC_UU_NP_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_DMSC_UU_NP_ARRAY);
		tmpbuf = dmsc->dmsc_uu_np_array;
		break;
	case TISP_PARAM_DMSC_SP_D_SIGMA_3_NP_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_DMSC_SP_D_SIGMA_3_NP_ARRAY);
		tmpbuf = dmsc->dmsc_sp_d_sigma_3_np_array;
		break;
	case TISP_PARAM_DMSC_SP_D_W_WEI_NP_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_DMSC_SP_D_W_WEI_NP_ARRAY);
		tmpbuf = dmsc->dmsc_sp_d_w_wei_np_array;
		break;
	case TISP_PARAM_DMSC_SP_D_B_WEI_NP_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_DMSC_SP_D_B_WEI_NP_ARRAY);
		tmpbuf = dmsc->dmsc_sp_d_b_wei_np_array;
		break;
	case TISP_PARAM_DMSC_SP_UD_W_WEI_NP_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_DMSC_SP_UD_W_WEI_NP_ARRAY);
		tmpbuf = dmsc->dmsc_sp_ud_w_wei_np_array;
		break;
	case TISP_PARAM_DMSC_SP_UD_B_WEI_NP_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_DMSC_SP_UD_B_WEI_NP_ARRAY);
		tmpbuf = dmsc->dmsc_sp_ud_b_wei_np_array;
		break;
	case TISP_PARAM_DMSC_OUT_OPT:
		len = sizeof(tparams.params_data.TISP_PARAM_DMSC_OUT_OPT);
		tmpbuf = &dmsc->dmsc_out_opt;
		break;
	case TISP_PARAM_DMSC_HV_THRES_1_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_DMSC_HV_THRES_1_ARRAY);
		tmpbuf = dmsc->dmsc_hv_thres_1_array;
		break;
	case TISP_PARAM_DMSC_HV_STREN_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_DMSC_HV_STREN_ARRAY);
		tmpbuf = dmsc->dmsc_hv_stren_array;
		break;
	case TISP_PARAM_DMSC_AA_THRES_1_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_DMSC_AA_THRES_1_ARRAY);
		tmpbuf = dmsc->dmsc_aa_thres_1_array;
		break;
	case TISP_PARAM_DMSC_AA_STREN_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_DMSC_AA_STREN_ARRAY);
		tmpbuf = dmsc->dmsc_aa_stren_array;
		break;
	case TISP_PARAM_DMSC_HVAA_THRES_1_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_DMSC_HVAA_THRES_1_ARRAY);
		tmpbuf = dmsc->dmsc_hvaa_thres_1_array;
		break;
	case TISP_PARAM_DMSC_HVAA_STREN_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_DMSC_HVAA_STREN_ARRAY);
		tmpbuf = dmsc->dmsc_hvaa_stren_array;
		break;
	case TISP_PARAM_DMSC_DIR_PAR_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_DMSC_DIR_PAR_ARRAY);
		tmpbuf = dmsc->dmsc_dir_par_array;
		break;
	case TISP_PARAM_DMSC_UU_THRES_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_DMSC_UU_THRES_ARRAY);
		tmpbuf = dmsc->dmsc_uu_thres_array;
		break;
	case TISP_PARAM_DMSC_UU_STREN_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_DMSC_UU_STREN_ARRAY);
		tmpbuf = dmsc->dmsc_uu_stren_array;
		break;
	case TISP_PARAM_DMSC_UU_PAR_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_DMSC_UU_PAR_ARRAY);
		tmpbuf = dmsc->dmsc_uu_par_array;
		break;
	case TISP_PARAM_DMSC_ALIAS_STREN_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_DMSC_ALIAS_STREN_ARRAY);
		tmpbuf = dmsc->dmsc_alias_stren_array;
		break;
	case TISP_PARAM_DMSC_ALIAS_THRES_1_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_DMSC_ALIAS_THRES_1_ARRAY);
		tmpbuf = dmsc->dmsc_alias_thres_1_array;
		break;
	case TISP_PARAM_DMSC_ALIAS_THRES_2_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_DMSC_ALIAS_THRES_2_ARRAY);
		tmpbuf = dmsc->dmsc_alias_thres_2_array;
		break;
	case TISP_PARAM_DMSC_ALIAS_DIR_THRES_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_DMSC_ALIAS_DIR_THRES_ARRAY);
		tmpbuf = dmsc->dmsc_alias_dir_thres_array;
		break;
	case TISP_PARAM_DMSC_ALIAS_PAR_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_DMSC_ALIAS_PAR_ARRAY);
		tmpbuf = dmsc->dmsc_alias_par_array;
		break;
	case TISP_PARAM_DMSC_NOR_ALIAS_THRES_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_DMSC_NOR_ALIAS_THRES_ARRAY);
		tmpbuf = dmsc->dmsc_nor_alias_thres_array;
		break;
	case TISP_PARAM_DMSC_NOR_PAR_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_DMSC_NOR_PAR_ARRAY);
		tmpbuf = dmsc->dmsc_nor_par_array;
		break;
	case TISP_PARAM_DMSC_SP_D_W_STREN_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_DMSC_SP_D_W_STREN_ARRAY);
		tmpbuf = dmsc->dmsc_sp_d_w_stren_array;
		break;
	case TISP_PARAM_DMSC_SP_D_B_STREN_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_DMSC_SP_D_B_STREN_ARRAY);
		tmpbuf = dmsc->dmsc_sp_d_b_stren_array;
		break;
	case TISP_PARAM_DMSC_SP_D_BRIG_THRES_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_DMSC_SP_D_BRIG_THRES_ARRAY);
		tmpbuf = dmsc->dmsc_sp_d_brig_thres_array;
		break;
	case TISP_PARAM_DMSC_SP_D_DARK_THRES_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_DMSC_SP_D_DARK_THRES_ARRAY);
		tmpbuf = dmsc->dmsc_sp_d_dark_thres_array;
		break;
	case TISP_PARAM_DMSC_SP_D_PAR_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_DMSC_SP_D_PAR_ARRAY);
		tmpbuf = dmsc->dmsc_sp_d_par_array;
		break;
	case TISP_PARAM_DMSC_SP_UD_W_STREN_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_DMSC_SP_UD_W_STREN_ARRAY);
		tmpbuf = dmsc->dmsc_sp_ud_w_stren_array;
		break;
	case TISP_PARAM_DMSC_SP_UD_B_STREN_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_DMSC_SP_UD_B_STREN_ARRAY);
		tmpbuf = dmsc->dmsc_sp_ud_b_stren_array;
		break;
	case TISP_PARAM_DMSC_SP_UD_BRIG_THRES_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_DMSC_SP_UD_BRIG_THRES_ARRAY);
		tmpbuf = dmsc->dmsc_sp_ud_brig_thres_array;
		break;
	case TISP_PARAM_DMSC_SP_UD_DARK_THRES_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_DMSC_SP_UD_DARK_THRES_ARRAY);
		tmpbuf = dmsc->dmsc_sp_ud_dark_thres_array;
		break;
	case TISP_PARAM_DMSC_SP_UD_PAR_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_DMSC_SP_UD_PAR_ARRAY);
		tmpbuf = dmsc->dmsc_sp_ud_par_array;
		break;
	case TISP_PARAM_DMSC_SP_ALIAS_THRES_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_DMSC_SP_ALIAS_THRES_ARRAY);
		tmpbuf = dmsc->dmsc_sp_alias_thres_array;
		break;
	case TISP_PARAM_DMSC_SP_ALIAS_PAR_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_DMSC_SP_ALIAS_PAR_ARRAY);
		tmpbuf = dmsc->dmsc_sp_alias_par_array;
		break;
	case TISP_PARAM_DMSC_RGB_DIR_THRES_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_DMSC_RGB_DIR_THRES_ARRAY);
		tmpbuf = dmsc->dmsc_rgb_dir_thres_array;
		break;
	case TISP_PARAM_DMSC_RGB_ALIAS_STREN_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_DMSC_RGB_ALIAS_STREN_ARRAY);
		tmpbuf = dmsc->dmsc_rgb_alias_stren_array;
		break;
	case TISP_PARAM_DMSC_RGB_ALIAS_PAR_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_DMSC_RGB_ALIAS_PAR_ARRAY);
		tmpbuf = dmsc->dmsc_rgb_alias_par_array;
		break;
	case TISP_PARAM_DMSC_FC_ALIAS_STREN_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_DMSC_FC_ALIAS_STREN_ARRAY);
		tmpbuf = dmsc->dmsc_fc_alias_stren_array;
		break;
	case TISP_PARAM_DMSC_FC_T1_THRES_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_DMSC_FC_T1_THRES_ARRAY);
		tmpbuf = dmsc->dmsc_fc_t1_thres_array;
		break;
	case TISP_PARAM_DMSC_FC_T1_STREN_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_DMSC_FC_T1_STREN_ARRAY);
		tmpbuf = dmsc->dmsc_fc_t1_stren_array;
		break;
	case TISP_PARAM_DMSC_FC_T2_STREN_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_DMSC_FC_T2_STREN_ARRAY);
		tmpbuf = dmsc->dmsc_fc_t2_stren_array;
		break;
	case TISP_PARAM_DMSC_FC_T3_STREN_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_DMSC_FC_T3_STREN_ARRAY);
		tmpbuf = dmsc->dmsc_fc_t3_stren_array;
		break;
	case TISP_PARAM_DMSC_FC_PAR_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_DMSC_FC_PAR_ARRAY);
		tmpbuf = dmsc->dmsc_fc_par_array;
		break;
	default:
		ISP_ERROR("%s,%d: dmsc not support param id %d\n", __func__, __LINE__, id);
		return -1;
	}

	memcpy(buf, tmpbuf, len);
	*plen = len;

	return 0;
}

int32_t tisp_dmsc_param_array_set(tisp_dmsc_t *dmsc, int32_t id, void *buf, uint32_t *plen)
{
	uint32_t len;
	void *tmpbuf;

	switch (id) {
	case TISP_PARAM_DMSC_UU_NP_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_DMSC_UU_NP_ARRAY);
		tmpbuf = dmsc->dmsc_uu_np_array;
		memcpy(tmpbuf, buf, len);
		break;
	case TISP_PARAM_DMSC_SP_D_SIGMA_3_NP_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_DMSC_SP_D_SIGMA_3_NP_ARRAY);
		tmpbuf = dmsc->dmsc_sp_d_sigma_3_np_array;
		memcpy(tmpbuf, buf, len);
		break;
	case TISP_PARAM_DMSC_SP_D_W_WEI_NP_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_DMSC_SP_D_W_WEI_NP_ARRAY);
		tmpbuf = dmsc->dmsc_sp_d_w_wei_np_array;
		memcpy(tmpbuf, buf, len);
		break;
	case TISP_PARAM_DMSC_SP_D_B_WEI_NP_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_DMSC_SP_D_B_WEI_NP_ARRAY);
		tmpbuf = dmsc->dmsc_sp_d_b_wei_np_array;
		memcpy(tmpbuf, buf, len);
		break;
	case TISP_PARAM_DMSC_SP_UD_W_WEI_NP_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_DMSC_SP_UD_W_WEI_NP_ARRAY);
		tmpbuf = dmsc->dmsc_sp_ud_w_wei_np_array;
		memcpy(tmpbuf, buf, len);
		break;
	case TISP_PARAM_DMSC_SP_UD_B_WEI_NP_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_DMSC_SP_UD_B_WEI_NP_ARRAY);
		tmpbuf = dmsc->dmsc_sp_ud_b_wei_np_array;
		memcpy(tmpbuf, buf, len);
		break;
	case TISP_PARAM_DMSC_OUT_OPT:
		len = sizeof(tparams.params_data.TISP_PARAM_DMSC_OUT_OPT);
		tmpbuf = &dmsc->dmsc_out_opt;
		memcpy(tmpbuf, buf, len);
		break;
	case TISP_PARAM_DMSC_HV_THRES_1_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_DMSC_HV_THRES_1_ARRAY);
		tmpbuf = dmsc->dmsc_hv_thres_1_array;
		memcpy(tmpbuf, buf, len);
		break;
	case TISP_PARAM_DMSC_HV_STREN_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_DMSC_HV_STREN_ARRAY);
		tmpbuf = dmsc->dmsc_hv_stren_array;
		memcpy(tmpbuf, buf, len);
		break;
	case TISP_PARAM_DMSC_AA_THRES_1_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_DMSC_AA_THRES_1_ARRAY);
		tmpbuf = dmsc->dmsc_aa_thres_1_array;
		memcpy(tmpbuf, buf, len);
		break;
	case TISP_PARAM_DMSC_AA_STREN_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_DMSC_AA_STREN_ARRAY);
		tmpbuf = dmsc->dmsc_aa_stren_array;
		memcpy(tmpbuf, buf, len);
		break;
	case TISP_PARAM_DMSC_HVAA_THRES_1_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_DMSC_HVAA_THRES_1_ARRAY);
		tmpbuf = dmsc->dmsc_hvaa_thres_1_array;
		memcpy(tmpbuf, buf, len);
		break;
	case TISP_PARAM_DMSC_HVAA_STREN_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_DMSC_HVAA_STREN_ARRAY);
		tmpbuf = dmsc->dmsc_hvaa_stren_array;
		memcpy(tmpbuf, buf, len);
		break;
	case TISP_PARAM_DMSC_DIR_PAR_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_DMSC_DIR_PAR_ARRAY);
		tmpbuf = dmsc->dmsc_dir_par_array;
		memcpy(tmpbuf, buf, len);
		break;
	case TISP_PARAM_DMSC_UU_THRES_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_DMSC_UU_THRES_ARRAY);
		tmpbuf = dmsc->dmsc_uu_thres_array;
		memcpy(tmpbuf, buf, len);
		break;
	case TISP_PARAM_DMSC_UU_STREN_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_DMSC_UU_STREN_ARRAY);
		tmpbuf = dmsc->dmsc_uu_stren_array;
		memcpy(tmpbuf, buf, len);
		break;
	case TISP_PARAM_DMSC_UU_PAR_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_DMSC_UU_PAR_ARRAY);
		tmpbuf = dmsc->dmsc_uu_par_array;
		memcpy(tmpbuf, buf, len);
		break;
	case TISP_PARAM_DMSC_ALIAS_STREN_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_DMSC_ALIAS_STREN_ARRAY);
		tmpbuf = dmsc->dmsc_alias_stren_array;
		memcpy(tmpbuf, buf, len);
		break;
	case TISP_PARAM_DMSC_ALIAS_THRES_1_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_DMSC_ALIAS_THRES_1_ARRAY);
		tmpbuf = dmsc->dmsc_alias_thres_1_array;
		memcpy(tmpbuf, buf, len);
		break;
	case TISP_PARAM_DMSC_ALIAS_THRES_2_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_DMSC_ALIAS_THRES_2_ARRAY);
		tmpbuf = dmsc->dmsc_alias_thres_2_array;
		memcpy(tmpbuf, buf, len);
		break;
	case TISP_PARAM_DMSC_ALIAS_DIR_THRES_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_DMSC_ALIAS_DIR_THRES_ARRAY);
		tmpbuf = dmsc->dmsc_alias_dir_thres_array;
		memcpy(tmpbuf, buf, len);
		break;
	case TISP_PARAM_DMSC_ALIAS_PAR_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_DMSC_ALIAS_PAR_ARRAY);
		tmpbuf = dmsc->dmsc_alias_par_array;
		memcpy(tmpbuf, buf, len);
		break;
	case TISP_PARAM_DMSC_NOR_ALIAS_THRES_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_DMSC_NOR_ALIAS_THRES_ARRAY);
		tmpbuf = dmsc->dmsc_nor_alias_thres_array;
		memcpy(tmpbuf, buf, len);
		break;
	case TISP_PARAM_DMSC_NOR_PAR_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_DMSC_NOR_PAR_ARRAY);
		tmpbuf = dmsc->dmsc_nor_par_array;
		memcpy(tmpbuf, buf, len);
		break;
	case TISP_PARAM_DMSC_SP_D_W_STREN_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_DMSC_SP_D_W_STREN_ARRAY);
		tmpbuf = dmsc->dmsc_sp_d_w_stren_array;
		memcpy(tmpbuf, buf, len);
		break;
	case TISP_PARAM_DMSC_SP_D_B_STREN_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_DMSC_SP_D_B_STREN_ARRAY);
		tmpbuf = dmsc->dmsc_sp_d_b_stren_array;
		memcpy(tmpbuf, buf, len);
		break;
	case TISP_PARAM_DMSC_SP_D_BRIG_THRES_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_DMSC_SP_D_BRIG_THRES_ARRAY);
		tmpbuf = dmsc->dmsc_sp_d_brig_thres_array;
		memcpy(tmpbuf, buf, len);
		break;
	case TISP_PARAM_DMSC_SP_D_DARK_THRES_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_DMSC_SP_D_DARK_THRES_ARRAY);
		tmpbuf = dmsc->dmsc_sp_d_dark_thres_array;
		memcpy(tmpbuf, buf, len);
		break;
	case TISP_PARAM_DMSC_SP_D_PAR_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_DMSC_SP_D_PAR_ARRAY);
		tmpbuf = dmsc->dmsc_sp_d_par_array;
		memcpy(tmpbuf, buf, len);
		break;
	case TISP_PARAM_DMSC_SP_UD_W_STREN_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_DMSC_SP_UD_W_STREN_ARRAY);
		tmpbuf = dmsc->dmsc_sp_ud_w_stren_array;
		memcpy(tmpbuf, buf, len);
		break;
	case TISP_PARAM_DMSC_SP_UD_B_STREN_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_DMSC_SP_UD_B_STREN_ARRAY);
		tmpbuf = dmsc->dmsc_sp_ud_b_stren_array;
		memcpy(tmpbuf, buf, len);
		break;
	case TISP_PARAM_DMSC_SP_UD_BRIG_THRES_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_DMSC_SP_UD_BRIG_THRES_ARRAY);
		tmpbuf = dmsc->dmsc_sp_ud_brig_thres_array;
		memcpy(tmpbuf, buf, len);
		break;
	case TISP_PARAM_DMSC_SP_UD_DARK_THRES_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_DMSC_SP_UD_DARK_THRES_ARRAY);
		tmpbuf = dmsc->dmsc_sp_ud_dark_thres_array;
		memcpy(tmpbuf, buf, len);
		break;
	case TISP_PARAM_DMSC_SP_UD_PAR_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_DMSC_SP_UD_PAR_ARRAY);
		tmpbuf = dmsc->dmsc_sp_ud_par_array;
		memcpy(tmpbuf, buf, len);
		break;
	case TISP_PARAM_DMSC_SP_ALIAS_THRES_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_DMSC_SP_ALIAS_THRES_ARRAY);
		tmpbuf = dmsc->dmsc_sp_alias_thres_array;
		memcpy(tmpbuf, buf, len);
		break;
	case TISP_PARAM_DMSC_SP_ALIAS_PAR_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_DMSC_SP_ALIAS_PAR_ARRAY);
		tmpbuf = dmsc->dmsc_sp_alias_par_array;
		memcpy(tmpbuf, buf, len);
		break;
	case TISP_PARAM_DMSC_RGB_DIR_THRES_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_DMSC_RGB_DIR_THRES_ARRAY);
		tmpbuf = dmsc->dmsc_rgb_dir_thres_array;
		memcpy(tmpbuf, buf, len);
		break;
	case TISP_PARAM_DMSC_RGB_ALIAS_STREN_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_DMSC_RGB_ALIAS_STREN_ARRAY);
		tmpbuf = dmsc->dmsc_rgb_alias_stren_array;
		memcpy(tmpbuf, buf, len);
		break;
	case TISP_PARAM_DMSC_RGB_ALIAS_PAR_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_DMSC_RGB_ALIAS_PAR_ARRAY);
		tmpbuf = dmsc->dmsc_rgb_alias_par_array;
		memcpy(tmpbuf, buf, len);
		break;
	case TISP_PARAM_DMSC_FC_ALIAS_STREN_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_DMSC_FC_ALIAS_STREN_ARRAY);
		tmpbuf = dmsc->dmsc_fc_alias_stren_array;
		memcpy(tmpbuf, buf, len);
		break;
	case TISP_PARAM_DMSC_FC_T1_THRES_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_DMSC_FC_T1_THRES_ARRAY);
		tmpbuf = dmsc->dmsc_fc_t1_thres_array;
		memcpy(tmpbuf, buf, len);
		break;
	case TISP_PARAM_DMSC_FC_T1_STREN_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_DMSC_FC_T1_STREN_ARRAY);
		tmpbuf = dmsc->dmsc_fc_t1_stren_array;
		memcpy(tmpbuf, buf, len);
		break;
	case TISP_PARAM_DMSC_FC_T2_STREN_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_DMSC_FC_T2_STREN_ARRAY);
		tmpbuf = dmsc->dmsc_fc_t2_stren_array;
		memcpy(tmpbuf, buf, len);
		break;
	case TISP_PARAM_DMSC_FC_T3_STREN_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_DMSC_FC_T3_STREN_ARRAY);
		tmpbuf = dmsc->dmsc_fc_t3_stren_array;
		memcpy(tmpbuf, buf, len);
		break;
	case TISP_PARAM_DMSC_FC_PAR_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_DMSC_FC_PAR_ARRAY);
		tmpbuf = dmsc->dmsc_fc_par_array;
		memcpy(tmpbuf, buf, len);
		break;
	default:
		ISP_ERROR("%s,%d: dmsc not support param id %d\n", __func__, __LINE__, id);
		return -1;
	}

	tisp_dmsc_all_reg_refresh(dmsc, dmsc->gain_old + 0x200);

	return 0;
}
