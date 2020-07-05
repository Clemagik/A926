#ifndef __JZ_ISP_H_
#define __JZ_ISP_H_

typedef unsigned long long uint64_t; 
typedef int int32_t;
typedef unsigned int uint32_t;

extern int running;
extern int isp_fd;
extern TizianoParameterImpl isp_param_day;

extern struct tiziano_isp_info *isp_info; /* isp_info */
extern struct tiziano_irq_info *irq_info; /* irq_info*/
extern void *mem_ae_base_v[2];
extern void *mem_ae_hist_base_v[2];

extern unsigned int frame_total;


int Jz_Isp_Init(int argc, char*argv[]);
void JZ_Isp_Exit();

/* 3A Algorithm */
int JZ_Isp_3a_Algorithm();
extern int _ae_parameter[38];
extern uint32_t _AePointPos[2];
extern uint64_t _ae_reg[4];  //awb
extern uint64_t analog_gain, analog_gain_last;  //blc
void JZ_Isp_Ae_Reg2par(int *para, unsigned int *reg);
void JZ_Isp_Get_Ae_Statistics(unsigned int *img, unsigned int zone_num_start);
void JZ_Isp_Get_Ae_Hist(unsigned int *img, unsigned int hist_en, unsigned int hist_ir_en);
int JZ_Isp_Ae();

extern int _awb_parameter[45];
extern unsigned int _awb_mode[3];
extern unsigned int _awb_ct;
extern unsigned int _awb_ct_last;
void JZ_Isp_Awb_Reg2par(int *para, unsigned int *reg);
void JZ_Isp_Get_Awb_Statistics(unsigned int *img, unsigned int zone_num_start);
int JZ_Isp_Awb();
// void ct_detect(float zone_rgbg[15][15][2], unsigned int zone_pix_cnt[15][15], unsigned int h_zone, unsigned int v_zone, unsigned short rg_pos[], unsigned short bg_pos[], unsigned short rgbg_weight[], unsigned short light_src[20], unsigned short light_src_num, unsigned short color_temp_mesh[], unsigned short awb_wght[], unsigned int ct_th[4], unsigned int ct_th_para[2], unsigned int dis_tw_en, float dis_up_lim, float dis_down_lim, unsigned int ct[], unsigned int awb_gain[], unsigned int awb_fail[]);
typedef struct Tiziano_Isp_Awb_Ct_Detect_Param
{
  unsigned short *auint16LightSrc;     //
  unsigned short  uint16LightSrcNum;
  unsigned int   *auint32CtTh;         //
  unsigned int   *auint32CtThPara;     // 
  unsigned short *auint16RgBgWeight;   //

  unsigned int    uint32HorZone; 
  unsigned int    uint32VerZone; 
  unsigned short *auint16RgPos;
  unsigned short *auint16BgPos; 
  unsigned short *auint16ColorTempMesh; 
  unsigned short *auint16AwbWght; 
  uint64_t       *auint64AwbDisTw;
  unsigned int   *uint32AwbCt;
  unsigned short *auint32LsWLut;
  unsigned int   *auint32AwbPointPos;
}ISP_AWB_CT_DETECT_PARAM;
void Tiziano_Awb_Ct_Detect(uint64_t *zone_rgbg, unsigned int *zone_pix_cnt, ISP_AWB_CT_DETECT_PARAM IspAwbCtDetectParam, unsigned int *awb_gain, unsigned int *awb_fail);

/* CCM Algorithm */
void JZ_Isp_Ccm_Parameter_Convert(TizianoParameterImpl isp_param);
int JZ_Isp_Cm();
void cm_control(float cm_a[9], float cm_t[9], float cm_d[9], unsigned int ct, float sat, float cm[9]);

/* LSC Algorithm */
int JZ_Isp_Lsc(TizianoParameterImpl isp_param, unsigned int ct, unsigned int ct_last);

/* LSC Algorithm */
int JZ_Isp_Gain_Interpolation(TizianoParameterImpl isp_param);

/* ADR Algorithm */
typedef struct Tiziano_Isp_Adr_Ct_Detect_Para
{
  int *CtcKneepointX      ;      
  int *CtcKneepointMux    ;    
  int *MinKneepointX      ;     
  int *MinKneepointY      ;
  int *MapKneepointX      ;
  int *MapKneepointY      ;
  int *ContrastWDistance  ;  
  int *AdrHist            ;           
  int *AdrBlockY          ;           
  int *AdrBlockHist       ;
  int *TmBaseLut          ;
  int *AdrGamX            ;
  int *AdrGamY            ;
  int *AdrMapMode         ;
  int *AdrLightEnd        ;
  int *AdrBlockLight      ;
}Isp_Adr_Ct_Detect_Para;

void JZ_Isp_Get_Adr_Statistics(unsigned int *img);
int JZ_Isp_adr_Algorithm();
void Tiziano_adr_fpga(Isp_Adr_Ct_Detect_Para TizianoAdrFpgaStructMe);

/* DEFOG Algorithm */
typedef struct Tiziano_Isp_Defog_Ct_Detect_Para
{

  int *DefogBlockRStat        ; 
  int *DefogBlockGStat        ; 
  int *DefogBlockBStat        ; 
  int *DefogBlockYStat        ; 
  int *DefogBlockDetailStat   ; 
  int *DefogBlockVarianceStat ; 
  int *DefogDefogBlockTX      ; 
  int *DefogDefogBlockTY      ; 
  int *DefogDetailX           ; 
  int *DefogDetailY           ; 
  int *DefogColorVarX         ; 
  int *DefogColorVarY         ; 
  int *DefogFpgaPara          ; 
  int *DefogBlockTransmitT    ; 
  int *DefogBlockAirLightR    ;          
  int *DefogBlockAirLightG    ;          
  int *DefogBlockAirLightB    ;          
                                        

}Isp_Defog_Ct_Detect_Para;

void defog_get(unsigned int *img);
int JZ_Isp_Defog_Algorithm();
void Tiziano_defog_fpga(Isp_Defog_Ct_Detect_Para TizianoDefogStructMe);

struct mem_fr_list {
    unsigned int mem_fr_p;
    struct mem_fr_list *next;
};

enum ispPixFormat
{
    PIX_FMT_BGGR8 = 0,
    PIX_FMT_RGGB8,
    PIX_FMT_GBRG8,
    PIX_FMT_GRBG8,
    PIX_FMT_BGGR10,
    PIX_FMT_RGGB10,
    PIX_FMT_GBRG10,
    PIX_FMT_GRBG10,
    PIX_FMT_BGGR12,
    PIX_FMT_RGGB12,
    PIX_FMT_GBRG12,
    PIX_FMT_GRBG12,
};

struct tiziano_irq_info {
    unsigned int irq_value;
    unsigned int irq_isp_save_frame;
    unsigned int irq_isp_done_cnt;
    unsigned int irq_ae_index;
    unsigned int irq_ae_hist_index;
    unsigned int irq_awb_index;
    unsigned int irq_ctm_index;
    unsigned int irq_adr_index;
    unsigned int irq_defog_index;
    unsigned int irq_dmaout_save_frame;
    unsigned int irq_dmaout_done_cnt;
    unsigned int irq_dmaout_index;
};

struct tiziano_isp_info {
    int width;
    int height;
    int fps;
    enum ispPixFormat format;
    unsigned int mem_fr_dump_p;
    unsigned int mem_fr_dump_s;
    unsigned int mem_ae_base_p[2];
    unsigned int mem_ae_hist_base_p[2];
    unsigned int mem_awb_base_p[2];
    unsigned int mem_adr_base_p[4];
    unsigned int mem_defog_base_p[4];
    unsigned int mem_af_base_p[2];
    unsigned int mem_ctm_base_p[2];
    unsigned int mem_dmaout_p[2];
    unsigned int mem_dmaout_dump_p;
    unsigned int mem_dmaout_dump_s;
    unsigned int mem_ipu_p[2];
    unsigned int mem_ipu_dump_p;
    unsigned int mem_ipu_dump_s;
    struct mem_fr_list mem_fr_p_list[3];
    unsigned int mem_mdns_base_p[4];  
    unsigned int mem_ch1_base_p[2];
    unsigned int mem_ch2_base_p[2];
};

#endif 



