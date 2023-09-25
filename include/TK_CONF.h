#ifndef __TK_CONF_H__
#define __TK_CONF_H__

#include <stdlib.h>

#include "../include/TK_UT_LIST.h"
#include "../include/TK_TYPE.h"

#define STR_DELIM		"="


#define SESSION_APP		"APP"
#define SESSION_DIP		"DIP"
#define SESSION_LPR		"LPR"
#define SESSION_OCR		"OCR"

/* =====================================================
APP
=======================================================*/
#define APP_KEY_SHOW_LOG				"APP_SHOW_LOG"
#define APP_KEY_IMG_PATH				"APP_IMG_PATH"

/* =====================================================
DIP 
=======================================================*/
#define DIP_KEY_SHOW_LOG				"dip_show_log"

/* =====================================================
OCR 
=======================================================*/
#define OCR_KEY_SHOW_LOG			"ocr_show_log"
#define OCR_KEY_NN_TYPE				"ocr_nn_type"
#define OCR_KEY_CHAR_H				"ocr_stt_char_h"
#define OCR_KEY_RESIZE_THS			"ocr_stt_resize_ths"
#define OCR_KEY_LEARNING_CNT		"ocr_stt_learning_cnt"

/* =====================================================
LPR
=======================================================*/
#define LPR_KEY_SHOW_LOG			"lpr_show_log"
/* setting */
#define LPR_KEY_TYPE				"lpr_stt_type"
#define LPR_KEY_AREA_CNT			"lpr_stt_area_cnt"


#define LPR_KEY_RESIZE_W			"lpr_stt_resize_w"
#define LPR_KEY_UF_H				"lpr_stt_uf_h"
#define LPR_KEY_UF_W				"lpr_stt_uf_w"

#define LPR_KEY_MIN_CHAR_H			"lpr_stt_min_char_h"
#define LPR_KEY_MIN_CHAR_W			"lpr_stt_min_char_w"
#define LPR_KEY_MIN_PLATE_H			"lpr_stt_min_plate_h"
#define LPR_KEY_MIN_PLATE_W			"lpr_stt_min_plate_w"

#define LPR_KEY_MIN_CHAR_CNT		"lpr_stt_min_chars_cnt"
#define LPR_KEY_MAX_CHAR_CNT		"lpr_stt_max_chars_cnt"

#define LPR_KEY_MAX_HG_MG_W			"lpr_stt_max_hg_mg_w"
#define LPR_KEY_MAX_HG_MG_INT		"lpr_stt_max_hg_mg_int"

#define LPR_KEY_MIN_PLATE_RATIO		"lpr_stt_min_plate_ratio"
#define LPR_KEY_MAX_PLATE_RATIO		"lpr_stt_max_plate_ratio"
#define LPR_KEY_NEW_PLATE_RATIO		"lpr_stt_new_plate_ratio"

/* sub process */
#define LPR_KEY_RUN_UF_PREDIP				"lpr_run_uf_predip"
#define LPR_KEY_RUN_UF_DEL_THICK			"lpr_run_uf_del_tick"
#define LPR_KEY_RUN_UF_GROUP_LABEL			"lpr_run_uf_group_label"
#define LPR_KEY_RUN_UF_GROUP_MERGE			"lpr_run_uf_group_merge"
#define LPR_KEY_RUN_UF_GROUP_EXTEN			"lpr_run_uf_group_exten"

#define LPR_KEY_RUN_BIG4_PREDIP				"lpr_run_big4_predip"
#define LPR_KEY_RUN_BIG4_DEL_W_LINE			"lpr_run_big4_del_w_line"
#define LPR_KEY_RUN_BIG4_DEL_W_BLT			"lpr_run_big4_del_w_blt"
#define LPR_KEY_RUN_BIG4_DEL_W_CUVE			"lpr_run_big4_del_w_cuve"
#define LPR_KEY_RUN_BIG4_DEL_H_LINE			"lpr_run_big4_del_h_line"
#define LPR_KEY_RUN_BIG4_BILINEAR			"lpr_run_big4_bilinear"

#define LPR_KEY_RUN_PROJECTION_PREDIP		"lpr_run_prj_predip"
#define LPR_KEY_RUN_PROJECTION_DEL_W_LINE	"lpr_run_prj_del_w_line"
#define LPR_KEY_RUN_PROJECTION_DEL_W_BLT	"lpr_run_prj_del_w_blt"
#define LPR_KEY_RUN_PROJECTION_DEL_W_CUVE	"lpr_run_prj_del_w_cuve"
#define LPR_KEY_RUN_PROJECTION_DEL_H_LINE	"lpr_run_prj_del_h_line"
#define LPR_KEY_RUN_PROJECTION_DEL_NOISE	"lpr_run_prj_del_noize"
#define LPR_KEY_RUN_PROJECTION_DETAIL_RECT	"lpr_run_prj_detail_rect"

#define LPR_KEY_RUN_CHO_PLATE				"lpr_run_choice_plate"
#define LPR_KEY_RUN_CHAR_SAMPLING			"lpr_run_char_sampling"
#define LPR_KEY_RUN_CHAR_RESIZE				"lpr_run_char_resize"
#define LPR_KEY_RUN_CHAR_THINNING			"lpr_run_char_thinning"

/*main process */
#define LPR_KEY_RUN_BASIC_STUDY				"lpr_run_basic_study"
#define LPR_KEY_RUN_RESIZE					"lpr_run_resize"

#define LPR_KEY_RUN_FIND_UF					"lpr_run_find_uf"
#define LPR_KEY_RUN_FIND_BIG4				"lpr_run_find_big4"
#define LPR_KEY_RUN_PROJECTION				"lpr_run_prj"
#define LPR_KEY_RUN_PRIORITY				"lpr_run_pro"
#define LPR_KEY_RUN_CHAR_RECOG				"lpr_run_char_recog"
#define LPR_KEY_RUN_CHAR_SEGMENT			"lpr_run_char_seg"

/* debug */
#define LPR_KEY_SHOW_CPY_PRC_IMG			"lpr_show_cpy_prc_img"
#define LPR_KEY_SHOW_UF_DIP					"lpr_show_uf_dip"
#define LPR_KEY_SHOW_UF_LINE				"lpr_show_uf_line"
#define LPR_KEY_SHOW_UF_LABEL_BOX			"lpr_show_uf_label_box"

#define LPR_KEY_SHOW_BIG4_DIP				"lpr_show_big4_dip"
#define LPR_KEY_SHOW_BIG4_BOX				"lpr_show_big4_box"

#define LPR_KEY_SHOW_PROJECTION_DIP			"lpr_show_prj_dip"
#define LPR_KEY_SHOW_PROJECTION_BOX			"lpr_show_prj_box"

#define LPR_KEY_SHOW_CHAR_IMG				"lpr_show_char_img"
#define LPR_KEY_SHOW_LAST_PLATE_BOX			"lpr_show_last_plate"
#define LPR_KEY_SHOW_LAST_CHAR_BOX			"lpr_show_last_char"

#define LPR_KEY_SHOW_CHAR_SEG				"lpr_show_char_seg"
#define LPR_KEY_SHOW_W_LINE_SEG				"lpr_show_w_line_seg"
#define LPR_KEY_SHOW_H_LINE_SEG				"lpr_show_h_line_seg"

int SetWinCurPath( char * ptrpath );
int GetWinCurPath( char * ptrpath, char * ptrfilename );

int loadAPPConf( void);
pAPP_CONF GetAPPConf( void );
int SetAPPConf( pAPP_CONF ptrAPPConf );

int loadLPRConf( void);
pLPR_CONF GetLPRConf( void );
int SetLPRConf( pLPR_CONF ptrLPRConf );

int loadOCRConf( void);
pOCR_CONF GetOCRConf( void );
int SetOCRConf( pOCR_CONF ptrOCRConf );

int loadDIPConf( void);
pDIP_CONF GetDIPConf( void );
int SetDIPConf( pDIP_CONF ptrDIPConf );

#endif