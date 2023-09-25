
#pragma once
#include "WizardHelper.h"
#include "IBridge.h"
#include "../include/tk_conf.h"
#include "afxwin.h"
#include "WINConf.h"

// CCONFDlg 대화 상자입니다.

class CCONFDlg : public CDialog
{
	DECLARE_DYNAMIC(CCONFDlg)

// User Interface 

public:
	void Set_IALPRDlg( CIBridge * ptrInterface );
private:
	void init_CFG_Dlg(void);
	void Set_CFG_Dlg(void);
	BOOL Save_CFG_Dlg( void );
	void SetSubPrjEnable(int nEnable );

	LPR_CONF_t m_lpr_conf;
	OCR_CONF_t m_ocr_conf;
	DIP_CONF_t m_dip_conf;
	APP_CONF_t m_app_conf;
	CWINConf m_AppClass;
	CWizardHelper m_WizardHelper;

	BOOL m_bSaveFlag ;
	CIBridge * m_ptrALPRDlg;

public:
	CCONFDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CCONFDlg();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_CONF_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButton2();
	afx_msg void OnBnClickedSetPath();

	afx_msg void OnTimer(UINT_PTR nIDEvent);	
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	
	CButton m_chk_run_denoize;
	CButton m_chk_run_labeling;
	CButton m_chk_run_labeling_merge;
	CButton m_chk_run_labeling_extension;
	CButton m_chk_run_projection;
	CButton m_chk_run_priority;
	CButton m_chk_run_resize;
	CButton m_chk_run_pre_dip;	
	CButton m_chk_show_prc_image;
	CButton m_chk_show_uf_area;
	CButton m_chk_show_uf_group;
	CButton m_chk_show_projection;
	CButton m_chk_show_projection_group;
	
	CButton m_chk_show_priority_group;	
	CButton m_chk_plate_choice;
	CButton m_chk_ocr;
	
	int m_ctl_int_resize_w;
	int m_ctl_int_min_char_h;
	int m_ctl_int_min_uf_h;
	int m_ctl_int_min_plate_h;
	int m_ctl_int_min_char_w;
	int m_ctl_int_min_uf_w;
	int m_ctl_int_min_plate_w;
	float m_ctl_float_min_plate_rate;
	float m_ctl_float_max_plate_rate;
	float m_ctl_float_new_plate_rate;
	int m_ctl_int_lpr_type;
	int m_ctl_int_lpr_area_cnt;
	
	CButton m_chk_projection_predip;
	CButton m_ctrl_chk_prj_del_w_line;
	CButton m_ctrl_chk_prj_del_w_btl;
	CButton m_ctrl_chk_prj_del_w_cuve;

	afx_msg void OnBnClickedChkUfProjection();
	CButton m_ctrl_chk_rect_1st;
	
	int m_ctl_int_min_char_cnt;
	int m_ctl_int_max_char_cnt;
	CButton m_ctl_chk_show_last_plate;
	CButton m_chk_segment;
	CButton m_chk_char_sampling;
	CButton m_ctrl_chk_show_last_char;
	int m_ctl_hg_mg_max_w;
	int m_ctl_hg_mg_max_interval;	
	int m_ctl_int_ocr_threshold;
	int m_ctl_int_ocr_h;
	CButton m_chk_thinning;
	CButton m_chk_char_resize;
	CButton m_ctl_chk_show_app_log;
	CButton m_ctl_chk_show_dip_log;
	CButton m_ctl_chk_show_lpr_log;
	CButton m_ctl_chk_show_ocr_log;
	CButton m_chk_run_basic_study;

	CButton m_Ctrl_Radio_NN_1;
	CButton m_Ctrl_Radio_NN_2;	
	CButton m_chk_show_uf_dip;
	int m_ctl_l_angel_pixel;
	int m_ctl_r_angel_pixel;
	int m_ctl_big4_min_h;
	int m_ctl_big4_min_w;

	CButton m_chk_show_big4;
	//CButton m_chk_run_bilinear;
	//CButton m_ctrl_chk_big4_del_w_cuve;
	//CButton m_ctrl_chk_big4_del_w_line;
	//CButton m_ctrl_chk_big4_del_w_blt;
	//CButton m_ctrl_chk_prj_del_h_line;
	CButton m_chk_run_find_uf;
	CButton m_chk_run_find_big4;
	CButton m_chk_run_big4_predip;
	CButton m_ctrl_chk_big4_del_w_line;
	CButton m_ctrl_chk_big4_del_w_cuve;
	CButton m_ctrl_chk_big4_del_w_blt;
	CButton m_ctrl_chk_big4_del_h_line;
	CButton m_chk_run_bilinear;
	CButton m_ctrl_chk_prj_del_h_line;
	CButton m_chk_show_big4_predip;
	CButton m_ctrl_chk_prj_del_noise;
	CButton m_ctrl_chk_show_w_seg_char;
	CButton m_ctrl_chk_show_h_seg_char;
	CButton m_ctrl_chk_show_seg_char;
	CEdit m_ctl_imagePath;	
	int m_ctl_int_learning_cnt;
};
