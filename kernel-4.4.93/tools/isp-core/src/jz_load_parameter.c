/****************************************************************/
/*    Ingenic Tiziano Firmware                                  */
/****************************************************************/
/* Function  :  jz_load_parameter.c                             */
/* Author    :  Abner.sheng                                     */
/* Data      :  2018/4/19/                                      */
/* Version   :  0.0.0                                           */
/* Note      :  load parameter                                  */
/****************************************************************/
#include <stdio.h>
#include <sys/ioctl.h>


#include "jz_load_parameter.h"
#include "jz_isp.h"
#include "jz_control.h"
#include "tiziano_reg_def.h"
#include "clm_lut.h"


int isp_fd;

int JZ_Isp_Set_Parameter(struct ispval_list ispval)
{

	if (ioctl(isp_fd, IOCTL_ISP_SET_REG, &ispval)) {
		printf("F: isp set JZ_Isp_Set_Parameter failed\n");
		return -1;
	}

	return 0;
}

int JZ_Isp_Get_Parameter(struct ispval_list *ispval)
{
	if (ioctl(isp_fd, IOCTL_ISP_GET_REG, ispval)) {
		printf("F: isp set JZ_Isp_Get_Parameter failed\n");
		return -1;
	}
	return 0;
}



void Tiziano_Cpu_Write(unsigned int ADDR,unsigned int DATA){
	struct ispval_list isp_regval;
	isp_regval.reg_num = ADDR;
	isp_regval.value = DATA;
	// printf("ADDR = %08x,DATA = %08x\n", isp_regval.reg_num,isp_regval.value);
	if (JZ_Isp_Set_Parameter(isp_regval)) {
		printf("F: isp set reg failed\n");
	}
}



int JZ_Set_Parameter_Gamma(TizianoParameterImpl isp_param)
{
	//printf("^^^ Function  = ^_^  %s  ^_^  LINE = ^_^ [ %d]   ^_^ ^^^ \n", __func__,__LINE__);
	int i;
	struct ispval_list isp_regval;

	for(i = 0;i<isp_param.calibrations[_TIZIANO_FR_RGB_GAMMA_LUT_SADDR]->cols-1;i++)
	{
		isp_regval.reg_num = R_GAMMA_MEM_BASE+i*4;
		isp_regval.value = ((*(isp_param.calibrations[_TIZIANO_FR_RGB_GAMMA_LUT_SADDR]->ptr + i + 1))<<12) | (*(isp_param.calibrations[_TIZIANO_FR_RGB_GAMMA_LUT_SADDR]->ptr+i));

		if (JZ_Isp_Set_Parameter(isp_regval)) {
			printf("F: isp set reg failed    function%s,%d\n", __func__,__LINE__);
			return -1;
		}

		isp_regval.reg_num = G_GAMMA_MEM_BASE+i*4;
		isp_regval.value = ((*(isp_param.calibrations[_TIZIANO_FR_RGB_GAMMA_LUT_SADDR]->ptr + i + 1))<<12) | (*(isp_param.calibrations[_TIZIANO_FR_RGB_GAMMA_LUT_SADDR]->ptr+i));
		if (JZ_Isp_Set_Parameter(isp_regval)) {
			printf("F: isp set reg failed\n");
			return -1;
		}
		isp_regval.reg_num = B_GAMMA_MEM_BASE+i*4;
		isp_regval.value = ((*(isp_param.calibrations[_TIZIANO_FR_RGB_GAMMA_LUT_SADDR]->ptr + i + 1))<<12) | (*(isp_param.calibrations[_TIZIANO_FR_RGB_GAMMA_LUT_SADDR]->ptr+i));
		if (JZ_Isp_Set_Parameter(isp_regval)) {
			printf("F: isp set reg failed\n");
			return -1;
		}
		//printf("num = %08x,value = 0x%08x\n",isp_regval.reg_num,isp_regval.value);
	}
	return 0;
}


int JZ_Set_Parameter_Lsc(TizianoParameterImpl isp_param, int ct)
{
	//printf("^^^ Function  = ^_^  %s  ^_^  LINE = ^_^ [ %d]   ^_^ ^^^ \n", __func__,__LINE__);
	struct ispval_list isp_regval;
	int value0;
	int value1;
	int value2;
	int wdata_l;
	int wdata_h;
	int i;


	Tiziano_Cpu_Write(LSC_ADDR_MESH_SCALE                  ,  *(isp_param.calibrations[_TIZIANO_LSC_MESH_SCALE]->ptr));
	Tiziano_Cpu_Write(LSC_ADDR_LUT_STRIDE                  ,  *(isp_param.calibrations[_TIZIANO_LSC_LUT_STRIDE]->ptr));
	Tiziano_Cpu_Write(LSC_ADDR_MESH_SIZE                   ,  (*(isp_param.calibrations[_TIZIANO_LSC_MESH_SIZE ]->ptr + 1) <<16) | *(isp_param.calibrations[_TIZIANO_LSC_MESH_SIZE ]->ptr));


	int lsc_lut_size = isp_param.calibrations[_TIZIANO_LSC_A_LINEAR]->cols;
	//printf("size  = %d\n",  lsc_lut_size);
	for(i = 0; i < lsc_lut_size; i=i+3){
		value0 = *(isp_param.calibrations[_TIZIANO_LSC_D_LINEAR]->ptr + i    );
		value1 = *(isp_param.calibrations[_TIZIANO_LSC_D_LINEAR]->ptr + i + 1);
		value2 = *(isp_param.calibrations[_TIZIANO_LSC_D_LINEAR]->ptr + i + 2);

		wdata_l = ((value1 & 0xff) << 16) | value0;
		wdata_h = (value2 << 8) | ((value1 & 0xff00) >> 8);

		isp_regval.reg_num = LSC_MEM_BASE + (i/3)*8;
		isp_regval.value = wdata_l;
		//  printf("isp_regval.reg_num1 = %08x,isp_regval.value1 = %08x\n",isp_regval.reg_num,isp_regval.value);
		if (JZ_Isp_Set_Parameter(isp_regval)) {
			printf("F: isp set reg failed\n");
			return -1;
		}
		isp_regval.reg_num = LSC_MEM_BASE + (i/3)*8 + 4;
		isp_regval.value = wdata_h;

		if (JZ_Isp_Set_Parameter(isp_regval)) {
			printf("F: isp set reg failed\n");
			return -1;
		}
	}
	return 0;
}


int JZ_Set_Dmsc_Init_Parameter(TizianoParameterImpl isp_param)
{
	//printf("^^^ Function  = ^_^  %s  ^_^  LINE = ^_^ [ %d]   ^_^ ^^^ \n", __func__,__LINE__);
	struct ispval_list isp_regval;
	int i;
	for(i=0;i<isp_param.calibrations[_TIZIANO_DMSC_INIT_PARAMETER]->cols;i++)
	{
		isp_regval.reg_num = DMSC_ADDR_G_STD_STREN+i*4;
		isp_regval.value = (*(isp_param.calibrations[_TIZIANO_DMSC_INIT_PARAMETER]->ptr+i));
		//printf("isp_regval.reg_num1 = %08x,isp_regval.value1 = %d\n",isp_regval.reg_num,isp_regval.value);
		if (JZ_Isp_Set_Parameter(isp_regval)) {
			printf("F: isp set reg failed\n");
			return -1;
		}
	}
	isp_regval.reg_num = DMSC_ADDR_SYNC;
	isp_regval.value = 1;
	if (JZ_Isp_Set_Parameter(isp_regval)) {
		printf("F: isp set reg failed\n");
		return -1;
	}
	//printf("isp_regval.reg_num1 = %08x,isp_regval.value1 = %d\n",isp_regval.reg_num,isp_regval.value);
	return 0;
}

int JZ_Set_Gib_Init_Parameter(TizianoParameterImpl isp_param)
{
	//printf("^^^ Function  = ^_^  %s  ^_^  LINE = ^_^ [ %d]   ^_^ ^^^ \n", __func__,__LINE__);
	struct ispval_list isp_regval;
	isp_regval.reg_num = _GIB_BEGIN_CONFIG_BASE;
	isp_regval.value = (*(isp_param.calibrations[_TIZIANO_GIB_CONFIG_LINEAR]->ptr));
	if (JZ_Isp_Set_Parameter(isp_regval)) {
		printf("F: isp set reg failed\n");
		return -1;
	}
	isp_regval.reg_num = _GIB_DEIRM_BLC_BASE;
	isp_regval.value = (*(isp_param.calibrations[_TIZIANO_GIB_DEIRM_BLC_LINEAR]->ptr)|(*(isp_param.calibrations[_TIZIANO_GIB_DEIRM_BLC_LINEAR]->ptr+1) << 16));
	if (JZ_Isp_Set_Parameter(isp_regval)) {
		printf("F: isp set reg failed\n");
		return -1;
	}
	//printf("isp_regval.reg_num1 = %08x,isp_regval.value1 = %d\n",isp_regval.reg_num,isp_regval.value);
	isp_regval.reg_num = _GIB_DEIRM_BLC_BASE+4;
	isp_regval.value = (*(isp_param.calibrations[_TIZIANO_GIB_DEIRM_BLC_LINEAR]->ptr+2)|(*(isp_param.calibrations[_TIZIANO_GIB_DEIRM_BLC_LINEAR]->ptr+3) << 16));
	if (JZ_Isp_Set_Parameter(isp_regval)) {
		printf("F: isp set reg failed\n");
		return -1;
	}
	//printf("isp_regval.reg_num1 = %08x,isp_regval.value1 = %d\n",isp_regval.reg_num,isp_regval.value);
	isp_regval.reg_num = _GIB_BEGIN_R_G_BASE;
	isp_regval.value = (*(isp_param.calibrations[_TIZIANO_GIB_R_G_LINEAR]->ptr)|(*(isp_param.calibrations[_TIZIANO_GIB_R_G_LINEAR]->ptr+1) << 16));
	if (JZ_Isp_Set_Parameter(isp_regval)) {
		printf("F: isp set reg failed\n");
		return -1;
	}
	// printf("isp_regval.reg_num1 = %08x,isp_regval.value1 = %d\n",isp_regval.reg_num,isp_regval.value);
	isp_regval.reg_num = _GIB_BEGIN_B_IR_BASE;
	isp_regval.value = (*(isp_param.calibrations[_TIZIANO_GIB_B_IR_LINEAR]->ptr)|(*(isp_param.calibrations[_TIZIANO_GIB_B_IR_LINEAR]->ptr+1) << 16));
	if (JZ_Isp_Set_Parameter(isp_regval)) {
		printf("F: isp set reg failed\n");
		return -1;
	}
	//printf("isp_regval.reg_num1 = %08x,isp_regval.value1 = %d\n",isp_regval.reg_num,isp_regval.value);
	return 0;
}


void JZ_Set_Ae_Init_Parameter(TizianoParameterImpl isp_param)
{
	Tiziano_Cpu_Write(AE_ADDR_ZONE_NUM_START_LINEAR                  ,  *(isp_param.calibrations[_TIZIANO_AE_ZONE_NUM_START_LINEAR]->ptr));
	Tiziano_Cpu_Write(AE_ADDR_HOR_ZONE_SIZE_1_4_LINEAR               ,  *(isp_param.calibrations[_TIZIANO_AE_HOR_ZONE_SIZE_1_4_LINEAR]->ptr));
	Tiziano_Cpu_Write(AE_ADDR_HOR_ZONE_SIZE_5_8_LINEAR               ,  *(isp_param.calibrations[_TIZIANO_AE_HOR_ZONE_SIZE_5_8_LINEAR]->ptr));
	Tiziano_Cpu_Write(AE_ADDR_HOR_ZONE_SIZE_9_12_LINEAR              ,  *(isp_param.calibrations[_TIZIANO_AE_HOR_ZONE_SIZE_9_12_LINEAR]->ptr));
	Tiziano_Cpu_Write(AE_ADDR_HOR_ZONE_SIZE_13_15_LINEAR             ,  *(isp_param.calibrations[_TIZIANO_AE_HOR_ZONE_SIZE_13_15_LINEAR]->ptr));
	Tiziano_Cpu_Write(AE_ADDR_VER_ZONE_SIZE_1_4_LINEAR               ,  *(isp_param.calibrations[_TIZIANO_AE_VER_ZONE_SIZE_1_4_LINEAR]->ptr));
	Tiziano_Cpu_Write(AE_ADDR_VER_ZONE_SIZE_5_8_LINEAR               ,  *(isp_param.calibrations[_TIZIANO_AE_VER_ZONE_SIZE_5_8_LINEAR]->ptr));
	Tiziano_Cpu_Write(AE_ADDR_VER_ZONE_SIZE_9_12_LINEAR              ,  *(isp_param.calibrations[_TIZIANO_AE_VER_ZONE_SIZE_9_12_LINEAR]->ptr));
	Tiziano_Cpu_Write(AE_ADDR_VER_ZONE_SIZE_13_15_LINEAR             ,  *(isp_param.calibrations[_TIZIANO_AE_VER_ZONE_SIZE_13_15_LINEAR]->ptr));
	Tiziano_Cpu_Write(AE_ADDR_LUM_TH_FREQ_LINEAR                     ,  *(isp_param.calibrations[_TIZIANO_AE_LUM_TH_FREQ_LINEAR]->ptr));
}

int JZ_Set_Parameter_CCM(TizianoParameterImpl isp_param, unsigned int *_ccm_reg){

	//printf("^^^ Function  = ^_^  %s  ^_^  LINE = ^_^ [ %d]   ^_^ ^^^ \n", __func__,__LINE__);
	static int dp_cfg_init=0;
	if(dp_cfg_init ==0){
		Tiziano_Cpu_Write(CCM_ADDR_DP_CFG_BASE                  ,  *(isp_param.calibrations[_TIZIANO_CCM_DP_CFG]->ptr));
		dp_cfg_init=1;
	}

	int i;
	struct ispval_list isp_regval;
	int ccm_lut_size = isp_param.calibrations[_TIZIANO_CCM_A_LINEAR]->cols;

	for(i = 0; i < ccm_lut_size; i+=2){
		isp_regval.reg_num = CCM_ADDR_A_BASE+i*2;
		if(i == (ccm_lut_size - 1)){
			isp_regval.value = *(_ccm_reg+i);
		}
		else{
			isp_regval.value = ((*(_ccm_reg + i + 1))<<16) | (*(_ccm_reg+i));
		}

		//printf("reg = 0x%08x, value = 0x%08x\n", isp_regval.reg_num, isp_regval.value);
		if (JZ_Isp_Set_Parameter(isp_regval)) {
			printf("F: isp set ccm reg failed\n");
			return -1;
		}
	}

	return 0;
}

void clm_lut2reg(int clm_s_lut[30][7][5], int clm_h_lut[30][7][5], int clm_s_reg[420], int clm_h_reg[420]){
	int i, j;

	for(i=0; i<30; i++){
		for(j=0; j<7; j++){
			clm_s_reg[i*7*2+j*2]   = ((clm_s_lut[i][j][3] & 0x1f) << 27) | ((clm_s_lut[i][j][2] & 0x1ff) << 18) | ((clm_s_lut[i][j][1] & 0x1ff) << 9) | ((clm_s_lut[i][j][0] & 0x1ff) << 0);
			clm_s_reg[i*7*2+j*2+1] = ((clm_s_lut[i][j][4] & 0x1ff) << 4) | ((clm_s_lut[i][j][3] & 0x1e0) >> 5);
			clm_h_reg[i*7*2+j*2]   = ((clm_h_lut[i][j][4] & 0xf) << 28)  | ((clm_h_lut[i][j][3] & 0x7f) << 21)  | ((clm_h_lut[i][j][2] & 0x7f) << 14) | ((clm_h_lut[i][j][1] & 0x7f) << 7) | ((clm_h_lut[i][j][0] & 0x7f) << 0);
			clm_h_reg[i*7*2+j*2+1] = ((clm_h_lut[i][j][4] & 0x70) >> 4) ;
			//printf("trans %d %d\n",i,j);
		}
	}

}

int JZ_Set_Parameter_CLM()
{
	int i;
	int clm_s_reg[420];
	int clm_h_reg[420];
	clm_lut2reg(clm_lut_s,clm_lut_h,clm_s_reg,clm_h_reg);
	struct ispval_list isp_clm_lut_reg;
	struct ispval_list isp_clm_reg;
	isp_clm_reg.reg_num = CLM_ADDR_LUT_SHIFT;
	isp_clm_reg.value   = 0x580 ;
	if (JZ_Isp_Set_Parameter(isp_clm_reg)) {
		printf("F: isp set clm lut start failed\n");
		return -1;
	}

	for(i=0; i<420;i++){
		isp_clm_lut_reg.reg_num = CLM_H_0_MEM_BASE + i*4;
		isp_clm_lut_reg.value = clm_h_reg[i];
		if (JZ_Isp_Set_Parameter(isp_clm_lut_reg)) {
			printf("F: isp set clm lut h0 %d failed\n", i);
			return -1;
		}
		else{
			//printf("Isp set clm lut h0 %d: %x\n", i, clm_h_reg[i]);
		}
	}

	for(i=0; i<420;i++){
		isp_clm_lut_reg.reg_num = CLM_H_1_MEM_BASE + i*4;
		isp_clm_lut_reg.value = clm_h_reg[i];
		if (JZ_Isp_Set_Parameter(isp_clm_lut_reg)) {
			printf("F: isp set clm lut h1 %d failed\n", i);
			return -1;
		}
		else{
			//printf("Isp set clm lut h1 %d: %x\n", i, clm_h_reg[i]);
		}
	}

	for(i=0; i<420;i++){
		isp_clm_lut_reg.reg_num = CLM_S_0_MEM_BASE + i*4;
		isp_clm_lut_reg.value = clm_s_reg[i];
		if (JZ_Isp_Set_Parameter(isp_clm_lut_reg)) {
			printf("F: isp set clm lut s0 %d failed\n", i);
			return -1;
		}
		else{
			//printf("Isp set clm lut s0 %d: %x\n", i, clm_s_reg[i]);
		}
	}

	for(i=0; i<420;i++){
		isp_clm_lut_reg.reg_num = CLM_S_1_MEM_BASE + i*4;
		isp_clm_lut_reg.value = clm_s_reg[i];
		if (JZ_Isp_Set_Parameter(isp_clm_lut_reg)) {
			printf("F: isp set clm lut s1 %d failed\n", i);
			return -1;
		}
		else{
			//printf("Isp set clm lut s1 %d: %x\n", i, clm_s_reg[i]);
		}
	}

	return 0;

}


void JZ_Set_Awb_Init_Parameter(TizianoParameterImpl isp_param)
{
	//printf("^^^ Function  = ^_^  %s  ^_^  LINE = ^_^ [ %d]   ^_^ ^^^ \n", __func__,__LINE__);
	Tiziano_Cpu_Write( AWB_ADDR_ZONE_NUM_START_LINEAR                 ,  *(isp_param.calibrations[_TIZIANO_AWB_ZONE_NUM_START_LINEAR]->ptr));
	Tiziano_Cpu_Write( AWB_ADDR_HOR_ZONE_SIZE_1_4_LINEAR              ,  *(isp_param.calibrations[_TIZIANO_AWB_HOR_ZONE_SIZE_1_4_LINEAR]->ptr));
	Tiziano_Cpu_Write( AWB_ADDR_HOR_ZONE_SIZE_5_8_LINEAR              ,  *(isp_param.calibrations[_TIZIANO_AWB_HOR_ZONE_SIZE_5_8_LINEAR]->ptr));
	Tiziano_Cpu_Write( AWB_ADDR_HOR_ZONE_SIZE_9_12_LINEAR             ,  *(isp_param.calibrations[_TIZIANO_AWB_HOR_ZONE_SIZE_9_12_LINEAR]->ptr));
	Tiziano_Cpu_Write( AWB_ADDR_HOR_ZONE_SIZE_13_15_LINEAR            ,  *(isp_param.calibrations[_TIZIANO_AWB_HOR_ZONE_SIZE_13_15_LINEAR]->ptr));
	Tiziano_Cpu_Write( AWB_ADDR_VER_ZONE_SIZE_1_4_LINEAR              ,  *(isp_param.calibrations[_TIZIANO_AWB_VER_ZONE_SIZE_1_4_LINEAR]->ptr));
	Tiziano_Cpu_Write( AWB_ADDR_VER_ZONE_SIZE_5_8_LINEAR              ,  *(isp_param.calibrations[_TIZIANO_AWB_VER_ZONE_SIZE_5_8_LINEAR]->ptr));
	Tiziano_Cpu_Write( AWB_ADDR_VER_ZONE_SIZE_9_12_LINEAR             ,  *(isp_param.calibrations[_TIZIANO_AWB_VER_ZONE_SIZE_9_12_LINEAR]->ptr));
	Tiziano_Cpu_Write( AWB_ADDR_VER_ZONE_SIZE_13_15_LINEAR            ,  *(isp_param.calibrations[_TIZIANO_AWB_VER_ZONE_SIZE_13_15_LINEAR]->ptr));
	Tiziano_Cpu_Write( AWB_ADDR_RG_TH_LINEAR                          ,  *(isp_param.calibrations[_TIZIANO_AWB_RG_TH_LINEAR]->ptr));
	Tiziano_Cpu_Write( AWB_ADDR_BG_TH_LINEAR                          ,  *(isp_param.calibrations[_TIZIANO_AWB_BG_TH_LINEAR]->ptr));
	Tiziano_Cpu_Write( AWB_ADDR_K1_A1_LINEAR                          ,  *(isp_param.calibrations[_TIZIANO_AWB_K1_A1_LINEAR]->ptr));
	Tiziano_Cpu_Write( AWB_ADDR_K2_A2_LINEAR                          ,  *(isp_param.calibrations[_TIZIANO_AWB_K2_A2_LINEAR]->ptr));
	Tiziano_Cpu_Write( AWB_ADDR_LUM_TH_FREQ_LINEAR                    ,  *(isp_param.calibrations[_TIZIANO_AWB_LUM_TH_FREQ_LINEAR]->ptr));
	Tiziano_Cpu_Write( AWBG_ADDR_G_R_LINEAR                           ,  *(isp_param.calibrations[_TIZIANO_AWBG_G_R_LINEAR]->ptr));
	Tiziano_Cpu_Write( AWBG_ADDR_IR_B_LINEAR                          ,  *(isp_param.calibrations[_TIZIANO_AWBG_IR_B_LINEAR]->ptr));

}

void JZ_Set_Defog_Init_Parameter(TizianoParameterImpl isp_param)
{
	//printf("^^^ Function  = ^_^  %s  ^_^  LINE = ^_^ [ %d]   ^_^ ^^^ \n", __func__,__LINE__);
	struct ispval_list isp_regval;
	int i;
	//printf("valude is = %d\n",isp_param.calibrations[_TIZIANO_DEFOG_BLOCK_COORD_M_01_00]->cols);
	for(i = 0;i < isp_param.calibrations[_TIZIANO_DEFOG_BLOCK_COORD_M_01_00]->cols; i++)
	{
		isp_regval.reg_num = DEFOG_ADDR_BLOCK_COORD_M_01_00 + i*4;
		isp_regval.value = (*(isp_param.calibrations[_TIZIANO_DEFOG_BLOCK_COORD_M_01_00]->ptr+i));
		Tiziano_Cpu_Write(isp_regval.reg_num, isp_regval.value);
	}

	for(i = 0;i < isp_param.calibrations[_TIZIANO_DEFOG_BLOCK_COORD_N_01_00]->cols; i++)
	{
		isp_regval.reg_num = DEFOG_ADDR_BLOCK_COORD_N_01_00 + i*4;
		isp_regval.value = (*(isp_param.calibrations[_TIZIANO_DEFOG_BLOCK_COORD_N_01_00]->ptr+i));
		Tiziano_Cpu_Write(isp_regval.reg_num, isp_regval.value);
	}

	for(i = 0;i < isp_param.calibrations[_TIZIANO_DEFOG_CT3x3_dis_01_00]->cols; i++)
	{
		isp_regval.reg_num = DEFOG_ADDR_CT3X3_DIS_01_00 + i*4;
		isp_regval.value = (*(isp_param.calibrations[_TIZIANO_DEFOG_CT3x3_dis_01_00]->ptr+i));
		Tiziano_Cpu_Write(isp_regval.reg_num, isp_regval.value);
	}

	for(i = 0;i < isp_param.calibrations[_TIZIANO_DEFOG_CT5x5_dis_01_00]->cols; i++)
	{
		isp_regval.reg_num = DEFOG_ADDR_CT5X5_DIS_01_00 + i*4;
		isp_regval.value = (*(isp_param.calibrations[_TIZIANO_DEFOG_CT5x5_dis_01_00]->ptr+i));
		Tiziano_Cpu_Write(isp_regval.reg_num, isp_regval.value);
	}

	for(i = 0;i < isp_param.calibrations[_TIZIANO_DEFOG_WEIGHTLUT02_05_00]->cols; i++)
	{
		isp_regval.reg_num = DEFOG_ADDR_WEIGHTLUT02_05_00 + i*4;
		isp_regval.value = (*(isp_param.calibrations[_TIZIANO_DEFOG_WEIGHTLUT02_05_00]->ptr+i));
		Tiziano_Cpu_Write(isp_regval.reg_num, isp_regval.value);
	}

	for(i = 0;i < isp_param.calibrations[_TIZIANO_DEFOG_WEIGHTLUT20_05_00]->cols; i++)
	{
		isp_regval.reg_num = DEFOG_ADDR_WEIGHTLUT20_05_00 + i*4;
		isp_regval.value = (*(isp_param.calibrations[_TIZIANO_DEFOG_WEIGHTLUT20_05_00]->ptr+i));
		Tiziano_Cpu_Write(isp_regval.reg_num, isp_regval.value);
	}

	for(i = 0;i < isp_param.calibrations[_TIZIANO_DEFOG_WEIGHTLUT21_05_00]->cols; i++)
	{
		isp_regval.reg_num = DEFOG_ADDR_WEIGHTLUT21_05_00 + i*4;
		isp_regval.value = (*(isp_param.calibrations[_TIZIANO_DEFOG_WEIGHTLUT21_05_00]->ptr+i));
		Tiziano_Cpu_Write(isp_regval.reg_num, isp_regval.value);
	}

	for(i = 0;i < isp_param.calibrations[_TIZIANO_DEFOG_WEIGHTLUT12_05_00]->cols; i++)
	{
		isp_regval.reg_num = DEFOG_ADDR_WEIGHTLUT12_05_00 + i*4;
		isp_regval.value = (*(isp_param.calibrations[_TIZIANO_DEFOG_WEIGHTLUT12_05_00]->ptr+i));
		Tiziano_Cpu_Write(isp_regval.reg_num, isp_regval.value);
	}

	for(i = 0;i < isp_param.calibrations[_TIZIANO_DEFOG_WEIGHTLUT22_05_00]->cols; i++)
	{
		isp_regval.reg_num = DEFOG_ADDR_WEIGHTLUT22_05_00 + i*4;
		isp_regval.value = (*(isp_param.calibrations[_TIZIANO_DEFOG_WEIGHTLUT22_05_00]->ptr+i));
		Tiziano_Cpu_Write(isp_regval.reg_num, isp_regval.value);
		//printf("defog isp_regval.reg_num1 = %08x,isp_regval.value1 = %08x\n",isp_regval.reg_num,isp_regval.value);
	}

	Tiziano_Cpu_Write(DEFOG_ADDR_BLOCK_COORD_M____10        , isp_regval.value = (*(isp_param.calibrations[_TIZIANO_DEFOG_BLOCK_COORD_M____10]->ptr))           );
	Tiziano_Cpu_Write(DEFOG_ADDR_BLOCK_COORD_N____20        , isp_regval.value = (*(isp_param.calibrations[_TIZIANO_DEFOG_BLOCK_COORD_N____20]->ptr))           );
	Tiziano_Cpu_Write(DEFOG_ADDR_CT3X3_DIS_23_22            , isp_regval.value = (*(isp_param.calibrations[_TIZIANO_DEFOG_CT3x3_dis_23_22]->ptr))               );
	Tiziano_Cpu_Write(DEFOG_ADDR_CT5X5_DIS_31_30            , isp_regval.value = (*(isp_param.calibrations[_TIZIANO_DEFOG_CT5x5_dis_31_30]->ptr))               );
	Tiziano_Cpu_Write(DEFOG_ADDR_WEIGHTLUT02_31_30          , isp_regval.value = (*(isp_param.calibrations[_TIZIANO_DEFOG_WEIGHTLUT02_31_30]->ptr))             );
	Tiziano_Cpu_Write(DEFOG_ADDR_WEIGHTLUT20_31_30          , isp_regval.value = (*(isp_param.calibrations[_TIZIANO_DEFOG_WEIGHTLUT20_31_30]->ptr))             );
	Tiziano_Cpu_Write(DEFOG_ADDR_WEIGHTLUT22_31_30          , isp_regval.value = (*(isp_param.calibrations[_TIZIANO_DEFOG_WEIGHTLUT22_31_30]->ptr))             );

	Tiziano_Cpu_Write(DEFOG_ADDR_PARA                       , *(isp_param.calibrations[_TIZAINO_DEFOG_PARA]->ptr)                          );
	Tiziano_Cpu_Write(DEFOG_ADDR_CLOR_CTRL_X3210            , *(isp_param.calibrations[_TIZAINO_DEFOG_CLOR_CTRL_X3210]->ptr)               );
	Tiziano_Cpu_Write(DEFOG_ADDR_CLOR_CTRL_Y3210            , *(isp_param.calibrations[_TIZAINO_DEFOG_CLOR_CTRL_Y3210]->ptr)               );
	Tiziano_Cpu_Write(DEFOG_ADDR_CLOR_CTRL_XY44             , *(isp_param.calibrations[_TIZAINO_DEFOG_CLOR_CTRL_XY44]->ptr)                );
	Tiziano_Cpu_Write(DEFOG_ADDR_CLOR_CTRL_POW3210          , *(isp_param.calibrations[_TIZAINO_DEFOG_CLOR_CTRL_POW3210]->ptr)             );
	Tiziano_Cpu_Write(DEFOG_ADDR_TRANS_DARK_CTRL_X3210      , *(isp_param.calibrations[_TIZAINO_DEFOG_TRANS_DARK_CTRL_X3210]->ptr)         );
	Tiziano_Cpu_Write(DEFOG_ADDR_TRANS_DARK_CTRL_Y3210      , *(isp_param.calibrations[_TIZAINO_DEFOG_TRANS_DARK_CTRL_Y3210]->ptr)         );
	Tiziano_Cpu_Write(DEFOG_ADDR_TRANS_DARK_CTRL_XY44       , *(isp_param.calibrations[_TIZAINO_DEFOG_TRANS_DARK_CTRL_XY44]->ptr)          );
	Tiziano_Cpu_Write(DEFOG_ADDR_TRANS_DARK_CTRL_POW3210    , *(isp_param.calibrations[_TIZAINO_DEFOG_TRANS_DARK_CTRL_POW3210]->ptr)       );
	Tiziano_Cpu_Write(DEFOG_ADDR_OVEREXPO_WGT_X3210         , *(isp_param.calibrations[_TIZAINO_DEFOG_OVEREXPO_WGT_X3210]->ptr)            );
	Tiziano_Cpu_Write(DEFOG_ADDR_OVEREXPO_WGT_Y3210         , *(isp_param.calibrations[_TIZAINO_DEFOG_OVEREXPO_WGT_Y3210]->ptr)            );
	Tiziano_Cpu_Write(DEFOG_ADDR_OVEREXPO_WGT_POW210        , *(isp_param.calibrations[_TIZAINO_DEFOG_OVEREXPO_WGT_POW210]->ptr)           );
	Tiziano_Cpu_Write(DEFOG_ADDR_DARKSPC_WGT_X3210          , *(isp_param.calibrations[_TIZAINO_DEFOG_DARKSPC_WGT_X3210]->ptr)             );
	Tiziano_Cpu_Write(DEFOG_ADDR_DARKSPC_WGT_Y3210          , *(isp_param.calibrations[_TIZAINO_DEFOG_DARKSPC_WGT_Y3210]->ptr)             );
	Tiziano_Cpu_Write(DEFOG_ADDR_DARKSPC_WGT_POW210         , *(isp_param.calibrations[_TIZAINO_DEFOG_DARKSPC_WGT_POW210]->ptr)            );
	Tiziano_Cpu_Write(DEFOG_ADDR_CTRL                       , *(isp_param.calibrations[_TIZAINO_DEFOG_CTRL]->ptr)                          );
	Tiziano_Cpu_Write(DEFOG_ADDR_FRAME_RATE                 , *(isp_param.calibrations[_TIZAINO_DEFOG_FRAME_RATE]->ptr)                    );
	Tiziano_Cpu_Write(DEFOG_ADDR_FRAME_ADDR                 , *(isp_param.calibrations[_TIZAINO_DEFOG_FRAME_ADDR]->ptr)                    );
	Tiziano_Cpu_Write(DEFOG_ADDR_FRAME_ID                   , *(isp_param.calibrations[_TIZAINO_DEFOG_FRAME_ID]->ptr)                      );
	for(i =0; i<200;i++)
	{
		Tiziano_Cpu_Write(0X9000+i*4,160<<24|160<<16|160<<8|204);
	}
}


void JZ_Set_Adr_Init_Parameter(TizianoParameterImpl isp_param)
{
	//printf("^^^ Function  = ^_^  %s  ^_^  LINE = ^_^ [ %d]   ^_^ ^^^ \n", __func__,__LINE__);
	struct ispval_list isp_regval;
	int i;
	for(i = 0;i < isp_param.calibrations[_TIZIANO_ADR_CEN_DIS_P_0]->cols; i++)
	{
		isp_regval.reg_num = ADR_ADDR_CEN_DIS_P_0 + i*4;
		isp_regval.value = (*(isp_param.calibrations[_TIZIANO_ADR_CEN_DIS_P_0]->ptr+i));
		Tiziano_Cpu_Write(isp_regval.reg_num, isp_regval.value);
	}

	for(i = 0;i < isp_param.calibrations[_TIZIANO_ADR_MAP_KPOINT_X_P0]->cols; i++)
	{
		isp_regval.reg_num = ADR_ADDR_MAP_KPOINT_X_P0 + i*4;
		isp_regval.value = (*(isp_param.calibrations[_TIZIANO_ADR_MAP_KPOINT_X_P0]->ptr+i));
		Tiziano_Cpu_Write(isp_regval.reg_num, isp_regval.value);
	}
	for(i = 0;i < isp_param.calibrations[_TIZIANO_ADR_MAP_KPOINT_Y_P00_0]->cols; i++)
	{
		isp_regval.reg_num = ADR_ADDR_MAP_KPOINT_Y_P00_0 + i*4;
		isp_regval.value = (*(isp_param.calibrations[_TIZIANO_ADR_MAP_KPOINT_Y_P00_0]->ptr+i));
		Tiziano_Cpu_Write(isp_regval.reg_num, isp_regval.value);
	}
	for(i = 0;i < isp_param.calibrations[_TIZIANO_ADR_MAP_KPOINT_Y_P00_5]->cols; i++)
	{
		isp_regval.reg_num = ADR_ADDR_MAP_KPOINT_Y_P00_5 + i*4;
		isp_regval.value = (*(isp_param.calibrations[_TIZIANO_ADR_MAP_KPOINT_Y_P00_5]->ptr+i));
		Tiziano_Cpu_Write(isp_regval.reg_num, isp_regval.value);
	}
	for(i = 0;i < isp_param.calibrations[_TIZIANO_ADR_MAP_KPOINT_POW_P0]->cols; i++)
	{
		isp_regval.reg_num = ADR_ADDR_MAP_KPOINT_POW_P0 + i*4;
		isp_regval.value = (*(isp_param.calibrations[_TIZIANO_ADR_MAP_KPOINT_POW_P0]->ptr+i));
		Tiziano_Cpu_Write(isp_regval.reg_num, isp_regval.value);
	}
	for(i = 0;i < isp_param.calibrations[_TIZIANO_ADR_WEILUT20_P0]->cols; i++)
	{
		isp_regval.reg_num = ADR_ADDR_WEILUT20_P0 + i*4;
		isp_regval.value = (*(isp_param.calibrations[_TIZIANO_ADR_WEILUT20_P0]->ptr+i));
		Tiziano_Cpu_Write(isp_regval.reg_num, isp_regval.value);
	}
	for(i = 0;i < isp_param.calibrations[_TIZIANO_ADR_WEILUT02_P0]->cols; i++)
	{
		isp_regval.reg_num = ADR_ADDR_WEILUT02_P0 + i*4;
		isp_regval.value = (*(isp_param.calibrations[_TIZIANO_ADR_WEILUT02_P0]->ptr+i));
		Tiziano_Cpu_Write(isp_regval.reg_num, isp_regval.value);
	}
	for(i = 0;i < isp_param.calibrations[_TIZIANO_ADR_WEILUT21_P0]->cols; i++)
	{
		isp_regval.reg_num = ADR_ADDR_WEILUT21_P0 + i*4;
		isp_regval.value = (*(isp_param.calibrations[_TIZIANO_ADR_WEILUT21_P0]->ptr+i));
		Tiziano_Cpu_Write(isp_regval.reg_num, isp_regval.value);
	}
	for(i = 0;i < isp_param.calibrations[_TIZIANO_ADR_WEILUT12_P0]->cols; i++)
	{
		isp_regval.reg_num = ADR_ADDR_WEILUT12_P0 + i*4;
		isp_regval.value = (*(isp_param.calibrations[_TIZIANO_ADR_WEILUT12_P0]->ptr+i));
		Tiziano_Cpu_Write(isp_regval.reg_num, isp_regval.value);
	}
	for(i = 0;i < isp_param.calibrations[_TIZIANO_ADR_CON_W_DIS_P00]->cols; i++)
	{
		isp_regval.reg_num = ADR_ADDR_CON_W_DIS_P00 + i*4;
		isp_regval.value = (*(isp_param.calibrations[_TIZIANO_ADR_CON_W_DIS_P00]->ptr+i));
		Tiziano_Cpu_Write(isp_regval.reg_num, isp_regval.value);
	}

	for(i = 0;i < isp_param.calibrations[_TIZIANO_ADR_MIN_KPOINT_X_P0]->cols; i++)
	{
		isp_regval.reg_num = ADR_ADDR_MIN_KPOINT_X_P0 + i*4;
		isp_regval.value = (*(isp_param.calibrations[_TIZIANO_ADR_MIN_KPOINT_X_P0]->ptr+i));
		Tiziano_Cpu_Write(isp_regval.reg_num, isp_regval.value);
	}

	for(i = 0;i < isp_param.calibrations[_TIZIANO_ADR_MIN_KPOINT_Y_P0]->cols; i++)
	{
		isp_regval.reg_num =  ADR_ADDR_MIN_KPOINT_Y_P0+ i*4;
		isp_regval.value = (*(isp_param.calibrations[_TIZIANO_ADR_MIN_KPOINT_Y_P0]->ptr+i));
		Tiziano_Cpu_Write(isp_regval.reg_num, isp_regval.value);
	}

	for(i = 0;i < isp_param.calibrations[_TIZIANO_ADR_MIN_KPOINT_POW_P0]->cols; i++)
	{
		isp_regval.reg_num = ADR_ADDR_MIN_KPOINT_POW_P0 + i*4;
		isp_regval.value = (*(isp_param.calibrations[_TIZIANO_ADR_MIN_KPOINT_POW_P0]->ptr+i));
		Tiziano_Cpu_Write(isp_regval.reg_num, isp_regval.value);
	}

	for(i = 0;i < isp_param.calibrations[_TIZIANO_ADR_COC_KPOINT_X_P0]->cols; i++)
	{
		isp_regval.reg_num = ADR_ADDR_COC_KPOINT_X_P0 + i*4;
		isp_regval.value = (*(isp_param.calibrations[_TIZIANO_ADR_COC_KPOINT_X_P0]->ptr+i));
		Tiziano_Cpu_Write(isp_regval.reg_num, isp_regval.value);
	}

	Tiziano_Cpu_Write(ADR_ADDR_T_VERSION                    , *(isp_param.calibrations[_TIZIANO_ADR_T_VERSION]->ptr)                        );
	Tiziano_Cpu_Write(ADR_ADDR_T_DBG_SEL                    , *(isp_param.calibrations[_TIZIANO_ADR_T_DBG_SEL]->ptr)                        );
	Tiziano_Cpu_Write(ADR_ADDR_T_DEG_VALUE                  , *(isp_param.calibrations[_TIZIANO_ADR_T_DEG_VALUE]->ptr)                      );
	Tiziano_Cpu_Write(ADR_ADDR_STEP_PAR                     , *(isp_param.calibrations[_TIZIANO_ADR_STEP_PAR]->ptr)                         );
	Tiziano_Cpu_Write(ADR_ADDR_BLK_COORD_M_P_0              , *(isp_param.calibrations[_TIZIANO_ADR_BLK_COORD_M_P_0]->ptr)                  );
	Tiziano_Cpu_Write(ADR_ADDR_BLK_COORD_M_P_1              , *(isp_param.calibrations[_TIZIANO_ADR_BLK_COORD_M_P_1]->ptr)                  );
	Tiziano_Cpu_Write(ADR_ADDR_BLK_COORD_M_P_2              , *(isp_param.calibrations[_TIZIANO_ADR_BLK_COORD_M_P_2]->ptr)                  );
	Tiziano_Cpu_Write(ADR_ADDR_BLK_COORD_N_P_0              , *(isp_param.calibrations[_TIZIANO_ADR_BLK_COORD_N_P_0]->ptr)                  );
	Tiziano_Cpu_Write(ADR_ADDR_BLK_COORD_N_P_1              , *(isp_param.calibrations[_TIZIANO_ADR_BLK_COORD_N_P_1]->ptr)                  );
	Tiziano_Cpu_Write(ADR_ADDR_BLK_COORD_N_P_2              , *(isp_param.calibrations[_TIZIANO_ADR_BLK_COORD_N_P_2]->ptr)                  );
	Tiziano_Cpu_Write(ADR_ADDR_CEN_DIS_P_F                  , *(isp_param.calibrations[_TIZIANO_ADR_CEN_DIS_P_F]->ptr)                      );
	Tiziano_Cpu_Write(ADR_ADDR_MAP_KPOINT_X_P5              , *(isp_param.calibrations[_TIZIANO_ADR_MAP_KPOINT_X_P5]->ptr)                  );

	Tiziano_Cpu_Write(ADR_ADDR_CTC_KPOINT_X_P0              , *(isp_param.calibrations[_TIZIANO_ADR_CTL_KPOINT_X_P0]->ptr)                       );
	Tiziano_Cpu_Write(ADR_ADDR_CTC_KPOINT_X_P1              , *(isp_param.calibrations[_TIZIANO_ADR_CTL_KPOINT_X_P1]->ptr)                       );
	Tiziano_Cpu_Write(ADR_ADDR_CTC_KPOINT_MUX_P0            , *(isp_param.calibrations[_TIZIANO_ADR_CTL_KPOINT_MUX_P0]->ptr)                     );
	Tiziano_Cpu_Write(ADR_ADDR_CTC_KPOINT_MUX_P1            , *(isp_param.calibrations[_TIZIANO_ADR_CTL_KPOINT_MUX_P1]->ptr)                     );
	Tiziano_Cpu_Write(ADR_ADDR_CTC_RATIO                    , *(isp_param.calibrations[_TIZIANO_ADR_CTL_RATIO]->ptr)                             );
	Tiziano_Cpu_Write(ADR_ADDR_MIN_KPOINT_X_P5              , *(isp_param.calibrations[_TIZIANO_ADR_MIN_KPOINT_X_P5]->ptr)                             );
	Tiziano_Cpu_Write(ADR_ADDR_MIN_KPOINT_Y_P5              , *(isp_param.calibrations[_TIZIANO_ADR_MIN_KPOINT_Y_P5]->ptr)                             );
	Tiziano_Cpu_Write(ADR_ADDR_COC_KPOINT_X_P5              , *(isp_param.calibrations[_TIZIANO_ADR_COC_KPOINT_X_P5]->ptr)                             );

	Tiziano_Cpu_Write(ADR_ADDR_COC_THRES                    , *(isp_param.calibrations[_TIZIANO_ADR_COC_THRES]->ptr)                        );
	Tiziano_Cpu_Write(ADR_ADDR_COC_STREN                    , *(isp_param.calibrations[_TIZIANO_ADR_COC_STREN]->ptr)                        );
	Tiziano_Cpu_Write(ADR_ADDR_COC_MODE                     , *(isp_param.calibrations[_TIZIANO_ADR_COC_MODE]->ptr)                        );
	Tiziano_Cpu_Write(ADR_ADDR_COC_KPOINT_POW_P0            , *(isp_param.calibrations[_TIZIANO_ADR_COC_KPOINT_POW_P0]->ptr)                        );
	Tiziano_Cpu_Write(ADR_ADDR_COC_KPOINT_POW_P1            , *(isp_param.calibrations[_TIZIANO_ADR_COC_KPOINT_POW_P1]->ptr)                        );
	Tiziano_Cpu_Write(ADR_ADDR_COC_KPOINT_POW_P2            , *(isp_param.calibrations[_TIZIANO_ADR_COC_KPOINT_POW_P2]->ptr)                        );
	Tiziano_Cpu_Write(ADR_ADDR_STAT_BLOCK_DIFF_P_0          , *(isp_param.calibrations[_TIZIANO_ADR_STAT_BLOCK_DIFF_P_0]->ptr)                        );
	Tiziano_Cpu_Write(ADR_ADDR_STAT_BLOCK_DIFF_P_1          , *(isp_param.calibrations[_TIZIANO_ADR_STAT_BLOCK_DIFF_P_1]->ptr)                        );
	Tiziano_Cpu_Write(ADR_ADDR_SHADOW_CTRL                  , *(isp_param.calibrations[_TIZIANO_ADR_SHADOW_CTRL]->ptr)                        );
	Tiziano_Cpu_Write(ADR_ADDR_BANK_NUMBER                  , *(isp_param.calibrations[_TIZIANO_ADR_BANK_NUMBER]->ptr)                        );
	Tiziano_Cpu_Write(ADR_ADDR_FRAME_ADDR                   , *(isp_param.calibrations[_TIZIANO_ADR_FRAME_ADDR]->ptr)                        );
	Tiziano_Cpu_Write(ADR_ADDR_FRAME_ID                     , *(isp_param.calibrations[_TIZIANO_ADR_FRAME_ID]->ptr)                        );
	Tiziano_Cpu_Write(ADR_ADDR_FRAME_RATE                   , *(isp_param.calibrations[_TIZIANO_ADR_FRAME_RATE]->ptr)                        );
}



void JZ_Set_DPC_Init_Parameter(TizianoParameterImpl isp_param)
{
	//printf("^^^ Function  = ^_^  %s  ^_^  LINE = ^_^ [ %d]   ^_^ ^^^ \n", __func__,__LINE__);
	struct ispval_list isp_regval;
	int i;
	for(i = 0;i < isp_param.calibrations[_TIZIANO_DPC_INIT_PARAMETER]->cols; i++)
	{
		isp_regval.reg_num = DPC_ADDR_S_EN + i*4;
		isp_regval.value = (*(isp_param.calibrations[_TIZIANO_DPC_INIT_PARAMETER]->ptr+i));
		//printf("DPC isp_regval.reg_num1 = %08x,isp_regval.value1 = %d\n",isp_regval.reg_num,isp_regval.value);
		Tiziano_Cpu_Write(isp_regval.reg_num, isp_regval.value);
	}
	Tiziano_Cpu_Write(DPC_ADDR_SYNC, 1);
}

void JZ_Set_Sharpen_Init_Parameter(TizianoParameterImpl isp_param)
{
	//printf("^^^ Function  = ^_^  %s  ^_^  LINE = ^_^ [ %d]   ^_^ ^^^ \n", __func__,__LINE__);
	struct ispval_list isp_regval;
	int i;
	for(i = 0;i < isp_param.calibrations[_TIZIANO_SHARPEN_INIT_PARAMETER]->cols; i++)
	{
		isp_regval.reg_num = SHARPEN_ADDR_STD_DIV_OPT + i*4;
		isp_regval.value = (*(isp_param.calibrations[_TIZIANO_SHARPEN_INIT_PARAMETER]->ptr+i));
		//printf("DPC isp_regval.reg_num1 = %08x,isp_regval.value1 = %d\n",isp_regval.reg_num,isp_regval.value);
		Tiziano_Cpu_Write(isp_regval.reg_num, isp_regval.value);
	}
	Tiziano_Cpu_Write(SHARPEN_ADDR_SYNC, 1);
}


int Jz_Paramter_Init(TizianoParameterImpl isp_param)
{

	//printf("^^^ Function  = ^_^  %s  ^_^  LINE = ^_^ [ %d]   ^_^ ^^^ \n", __func__,__LINE__);
	JZ_Set_Parameter_Gamma(isp_param);
	JZ_Set_Parameter_Lsc(isp_param,5600);
	JZ_Set_Gib_Init_Parameter(isp_param);
	JZ_Set_Ae_Init_Parameter(isp_param);
	JZ_Set_Parameter_CCM(isp_param, isp_param.calibrations[_TIZIANO_CCM_D_LINEAR]->ptr);
	JZ_Set_Parameter_CLM();
	JZ_Set_Awb_Init_Parameter(isp_param);
	JZ_Set_Dmsc_Init_Parameter(isp_param);
	JZ_Set_Defog_Init_Parameter(isp_param);
	JZ_Set_Adr_Init_Parameter(isp_param);
	JZ_Set_DPC_Init_Parameter(isp_param);
	JZ_Set_Sharpen_Init_Parameter(isp_param);

	return 0;

}
