// CONFDlg.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "TK_ALPRWizard.h"
#include "CONFDlg.h"
#include "TK_ALPRWizardDoc.h"

#define CFG_INIT_TIMER	3001

// CCONFDlg 대화 상자입니다.

IMPLEMENT_DYNAMIC(CCONFDlg, CDialog)

CCONFDlg::CCONFDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CCONFDlg::IDD, pParent)	
	, m_ctl_int_resize_w(0)
	, m_ctl_int_min_char_h( 0)
	, m_ctl_int_min_uf_h(0)
	, m_ctl_int_min_plate_h(0)
	, m_ctl_int_min_char_w(0)
	, m_ctl_int_min_uf_w(0)
	, m_ctl_int_min_plate_w(0)
	, m_ctl_float_min_plate_rate(0)
	, m_ctl_float_max_plate_rate(0)
	, m_ctl_float_new_plate_rate(0)
	, m_ctl_int_lpr_type(0)
	, m_ctl_int_lpr_area_cnt(0)
	, m_ctl_int_min_char_cnt(0)
	, m_ctl_int_max_char_cnt(0)
	, m_ctl_hg_mg_max_w(0)
	, m_ctl_hg_mg_max_interval(0)	
	, m_ctl_int_ocr_threshold(0)
	, m_ctl_int_ocr_h(0)	
	, m_ctl_l_angel_pixel(0)
	, m_ctl_r_angel_pixel(0)
	, m_ctl_big4_min_h(0)
	, m_ctl_big4_min_w(0)
	, m_ctl_int_learning_cnt(0)
{
	m_bSaveFlag = FALSE;
	m_ptrALPRDlg = NULL;
}

CCONFDlg::~CCONFDlg()
{
}

void CCONFDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CHK_UF, m_chk_run_denoize);
	DDX_Control(pDX, IDC_CHK_UF_LABEL, m_chk_run_labeling);
	DDX_Control(pDX, IDC_CHK_UF_LABEL_MERGE, m_chk_run_labeling_merge);
	DDX_Control(pDX, IDC_CHK_UF_LABEL_EXTENSION, m_chk_run_labeling_extension);
	DDX_Control(pDX, IDC_CHK_UF_PROJECTION, m_chk_run_projection);
	DDX_Control(pDX, IDC_CHK_UF_PRIORITY, m_chk_run_priority);
	DDX_Control(pDX, IDC_CHK_IMAGE_RESIZE, m_chk_run_resize);
	DDX_Control(pDX, IDC_CHK_PRE_DIP, m_chk_run_pre_dip);	
	DDX_Control(pDX, IDC_CHK_SHOW_IMAGE_COPY, m_chk_show_prc_image);
	DDX_Control(pDX, IDC_CHK_SHOW_UF_LINE, m_chk_show_uf_area);
	DDX_Control(pDX, IDC_CHK_SHOW_LABEL_BOX, m_chk_show_uf_group);
	DDX_Control(pDX, IDC_CHK_SHOW_PROJECTION, m_chk_show_projection);
	DDX_Control(pDX, IDC_CHK_SHOW_PROJECTION_GROUP, m_chk_show_projection_group);



	DDX_Control(pDX, IDC_CHK_PLATE_CHOICE, m_chk_plate_choice);
	DDX_Control(pDX, IDC_CHK_OCR, m_chk_ocr);

	DDX_Text(pDX, IDC_EDIT1, m_ctl_int_resize_w);
	DDX_Text(pDX, IDC_EDIT2, m_ctl_int_min_char_h);
	DDX_Text(pDX, IDC_EDIT4, m_ctl_int_min_uf_h);
	DDX_Text(pDX, IDC_EDIT6, m_ctl_int_min_plate_h);
	DDX_Text(pDX, IDC_EDIT3, m_ctl_int_min_char_w);
	DDX_Text(pDX, IDC_EDIT5, m_ctl_int_min_uf_w);
	DDX_Text(pDX, IDC_EDIT7, m_ctl_int_min_plate_w);
	DDX_Text(pDX, IDC_EDIT8, m_ctl_float_min_plate_rate);
	DDX_Text(pDX, IDC_EDIT9, m_ctl_float_max_plate_rate);
	DDX_Text(pDX, IDC_EDIT10, m_ctl_float_new_plate_rate);
	DDX_Text(pDX, IDC_EDIT11, m_ctl_int_lpr_type);
	DDX_Text(pDX, IDC_EDIT12, m_ctl_int_lpr_area_cnt);

	DDX_Control(pDX, IDC_CHK_PRJ_PREDIP, m_chk_projection_predip);
	DDX_Control(pDX, IDC_CHK_PRJ_DEL_W_LINE, m_ctrl_chk_prj_del_w_line);
	DDX_Control(pDX, IDC_CHK_PRJ_DEL_W_BLT, m_ctrl_chk_prj_del_w_btl);
	DDX_Control(pDX, IDC_CHK_PRJ_DEL_W_CUVE, m_ctrl_chk_prj_del_w_cuve);	

	DDX_Control(pDX, IDC_CHK_PRJ_RECT_1ST, m_ctrl_chk_rect_1st);

	DDX_Text(pDX, IDC_EDIT20, m_ctl_int_min_char_cnt);
	DDX_Text(pDX, IDC_EDIT21, m_ctl_int_max_char_cnt);
	DDX_Control(pDX, IDC_CHK_SHOW_LAST_PLATE, m_ctl_chk_show_last_plate);
	DDX_Control(pDX, IDC_CHK_CHAR_SEGMENT, m_chk_segment);
	DDX_Control(pDX, IDC_CHK_CHAR_SAMPLING, m_chk_char_sampling);
	DDX_Control(pDX, IDC_CHK_SHOW_LAST_CHAR, m_ctrl_chk_show_last_char);
	DDX_Text(pDX, IDC_EDIT22, m_ctl_hg_mg_max_w);
	DDX_Text(pDX, IDC_EDIT23, m_ctl_hg_mg_max_interval);
	DDX_Control(pDX, IDC_CHK_THINNING, m_chk_thinning);
	DDX_Control(pDX, IDC_CHK_RESIZE, m_chk_char_resize);
	DDX_Text(pDX, IDC_CHAR_RESIZE_THS, m_ctl_int_ocr_threshold);
	DDX_Text(pDX, IDC_CHAR_RESIZE_H, m_ctl_int_ocr_h);
	DDX_Control(pDX, IDC_CHK_SHOW_APP_LOG, m_ctl_chk_show_app_log);
	DDX_Control(pDX, IDC_CHK_SHOW_APP_LOG2, m_ctl_chk_show_dip_log);
	DDX_Control(pDX, IDC_CHK_SHOW_APP_LOG3, m_ctl_chk_show_lpr_log);
	DDX_Control(pDX, IDC_CHK_SHOW_APP_LOG4, m_ctl_chk_show_ocr_log);
	DDX_Control(pDX, IDC_CHK_BASIC_STUDY, m_chk_run_basic_study);

	DDX_Control(pDX, IDC_RADIO_NN_1,  m_Ctrl_Radio_NN_1);
	DDX_Control(pDX, IDC_RADIO_NN_2,  m_Ctrl_Radio_NN_2);	
	DDX_Control(pDX, IDC_CHK_SHOW_UF_DIP, m_chk_show_uf_dip);

	DDX_Text(pDX, IDC_EDIT28, m_ctl_l_angel_pixel);
	DDX_Text(pDX, IDC_EDIT29, m_ctl_r_angel_pixel);
	DDX_Text(pDX, IDC_EDIT30, m_ctl_big4_min_h);
	DDX_Text(pDX, IDC_EDIT31, m_ctl_big4_min_w);
	DDX_Control(pDX, IDC_CHK_SHOW_BIG4, m_chk_show_big4);
	//DDX_Control(pDX, IDC_CHK_BILINEAR, m_chk_run_bilinear);
	//DDX_Control(pDX, IDC_CHK_PRJ_DEL_W_CUVE2, m_ctrl_chk_big4_del_w_cuve);
	//DDX_Control(pDX, IDC_CHK_PRJ_DEL_W_LINE2, m_ctrl_chk_big4_del_w_line);
	//DDX_Control(pDX, IDC_CHK_PRJ_DEL_W_BLT2, m_ctrl_chk_big4_del_w_blt);
	//DDX_Control(pDX, IDC_CHK_PRJ_DEL_H_LINE, m_ctrl_chk_prj_del_h_line);
	DDX_Control(pDX, IDC_CHK_FIND_UF, m_chk_run_find_uf);
	DDX_Control(pDX, IDC_CHK_FIND_BIG4, m_chk_run_find_big4);
	DDX_Control(pDX, IDC_CHK_BIG4_PREDIP, m_chk_run_big4_predip);
	DDX_Control(pDX, IDC_CHK_BIG4_DEL_W_LINE, m_ctrl_chk_big4_del_w_line);
	DDX_Control(pDX, IDC_CHK_BIG4_DEL_W_CUVE, m_ctrl_chk_big4_del_w_cuve);
	DDX_Control(pDX, IDC_CHK_BIG4_DEL_W_BLT, m_ctrl_chk_big4_del_w_blt);
	DDX_Control(pDX, IDC_CHK_BIG4_DEL_H_LINE, m_ctrl_chk_big4_del_h_line);
	DDX_Control(pDX, IDC_CHK_CHG_BILINEAR, m_chk_run_bilinear);
	DDX_Control(pDX, IDC_CHK_PRJ_DEL_H_LINE, m_ctrl_chk_prj_del_h_line);
	DDX_Control(pDX, IDC_CHK_SHOW_BILINEAR, m_chk_show_big4_predip);
	DDX_Control(pDX, IDC_CHK_PRJ_DEL_NOISE, m_ctrl_chk_prj_del_noise);
	DDX_Control(pDX, IDC_CHK_SHOW_W_CHAR_SEG, m_ctrl_chk_show_w_seg_char);
	DDX_Control(pDX, IDC_CHK_SHOW_H_CHAR_SEG, m_ctrl_chk_show_h_seg_char);
	DDX_Control(pDX, IDC_CHK_SHOW_CHAR_SEG, m_ctrl_chk_show_seg_char);
	DDX_Control(pDX, IDC_EDIT14, m_ctl_imagePath);	
	DDX_Text(pDX, IDC_E_LEARNING_CNT, m_ctl_int_learning_cnt);
}

BEGIN_MESSAGE_MAP(CCONFDlg, CDialog)
	ON_BN_CLICKED(IDC_BUTTON1, &CCONFDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &CCONFDlg::OnBnClickedButton2)
	ON_BN_CLICKED(IDC_SET_PATH, &CCONFDlg::OnBnClickedSetPath)

	ON_WM_TIMER()	
	ON_WM_CREATE()	
	ON_BN_CLICKED(IDC_CHK_UF_PROJECTION, &CCONFDlg::OnBnClickedChkUfProjection)
END_MESSAGE_MAP()


// CCONFDlg 메시지 처리기입니다.
void  CCONFDlg::Set_IALPRDlg( CIBridge * ptrInterface )
{
	if( m_ptrALPRDlg == NULL )
		m_ptrALPRDlg = ptrInterface;

}

void CCONFDlg::init_CFG_Dlg( void)
{
	int nResult1 = ERR_SUCCESS;
	int nResult2 = ERR_SUCCESS;
	int nResult3 = ERR_SUCCESS;
	int nResult4 = ERR_SUCCESS;
	
	pLPR_CONF ptrlpr = NULL;
	pOCR_CONF ptrocr = NULL;
	pDIP_CONF ptrdip = NULL;
	pAPP_CONF ptrapp = NULL;
	char szpath[ MAX_PATH];

	memset( szpath, 0, sizeof( szpath ));
	memset( &m_lpr_conf, 0, sizeof( m_lpr_conf));
	memset( &m_ocr_conf, 0, sizeof( m_ocr_conf));
	memset( &m_dip_conf, 0, sizeof( m_dip_conf));
	memset( &m_app_conf, 0, sizeof( m_app_conf));

	nResult1 = loadLPRConf();
	nResult2 = loadOCRConf();
	nResult3 = loadDIPConf();
	nResult4 = loadAPPConf();

	if( nResult1 == ERR_SUCCESS  && nResult2 == ERR_SUCCESS && nResult3 == ERR_SUCCESS  && nResult4 == ERR_SUCCESS)
	{
		ptrlpr = GetLPRConf();
		ptrocr = GetOCRConf();
		ptrdip = GetDIPConf();
		//ptrapp = GetAPPConf();
		ptrapp = m_AppClass.GetWinAPPCONF();

		if ( ptrlpr == NULL || ptrocr == NULL || ptrdip == NULL || ptrapp == NULL)
		{
			nResult1 = ERR_NO_LPR_CONF;			
		}
		else
		{
			memcpy( &m_lpr_conf, ptrlpr, sizeof( LPR_CONF_t));
			memcpy( &m_ocr_conf, ptrocr, sizeof( OCR_CONF_t));
			memcpy( &m_dip_conf, ptrdip, sizeof( DIP_CONF_t));
			memcpy( &m_app_conf, ptrapp, sizeof( APP_CONF_t));

			Set_CFG_Dlg();			
		}
	}
	else
	{
		AfxMessageBox(L"LPR 설정 정보를 읽어 올 수가 없습니다.");
		OnBnClickedButton2();
	}
		
	m_WizardHelper.GetCurrentPathChar( szpath );
	SetWinCurPath( szpath );
}

void CCONFDlg::Set_CFG_Dlg( void)
{
	BYTE bVal;
	CString strData;
	TCHAR tcPath[ MAX_PATH ];
	CString str;

	/* Main process */

	bVal= m_lpr_conf.LPR_DBG_RUN_BASIC_STUDY;
	m_chk_run_basic_study.SetCheck( bVal );	
	
	bVal= m_lpr_conf.LPR_DBG_RUN_RESIZE ;
	m_chk_run_resize.SetCheck(bVal);

	bVal= m_lpr_conf.LPR_DBG_RUN_FIND_UF;
	m_chk_run_find_uf.SetCheck(bVal );

	bVal= m_lpr_conf.LPR_DBG_RUN_FIND_BIG4;
	m_chk_run_find_big4.SetCheck(bVal );

	bVal= m_lpr_conf.LPR_DBG_RUN_PROJECTION;
	m_chk_run_projection.SetCheck(bVal );

	bVal= m_lpr_conf.LPR_DBG_RUN_PRIORITY;
	m_chk_run_priority.SetCheck( bVal );
	
	bVal= m_lpr_conf.LPR_DBG_RUN_CHOICE_PLATE;
	m_chk_plate_choice.SetCheck( bVal );

	bVal= m_lpr_conf.LPR_DBG_RUN_CHAR_SEGMENT;
	m_chk_segment.SetCheck( bVal );
	
	bVal= m_lpr_conf.LPR_DBG_RUN_CHAR_RECOGNITION;
	m_chk_ocr.SetCheck( bVal );

	/* ========================================================
	sub process 
	==========================================================*/	
	bVal= m_lpr_conf.LPR_DBG_RUN_UF_PREDIP;
	m_chk_run_pre_dip.SetCheck( bVal );

	bVal= m_lpr_conf.LPR_DBG_RUN_UF_DEL_THICK;
	m_chk_run_denoize.SetCheck( bVal );	

	bVal= m_lpr_conf.LPR_DBG_RUN_UF_LABELING ;
	m_chk_run_labeling.SetCheck( bVal);

	bVal= m_lpr_conf.LPR_DBG_RUN_UF_MERGE;
	m_chk_run_labeling_merge.SetCheck(bVal );

	bVal= m_lpr_conf.LPR_DBG_RUN_UF_EXTENSION ;
	m_chk_run_labeling_extension.SetCheck(bVal );



	bVal= m_lpr_conf.LPR_DBG_RUN_BIG4_PREDIP;
	m_chk_run_big4_predip.SetCheck( bVal );

	bVal= m_lpr_conf.LPR_DBG_RUN_BIG4_DEL_W_LINE;
	m_ctrl_chk_big4_del_w_line.SetCheck( bVal );

	bVal= m_lpr_conf.LPR_DBG_RUN_BIG4_DEL_W_BLT;
	m_ctrl_chk_big4_del_w_blt.SetCheck( bVal );

	bVal= m_lpr_conf.LPR_DBG_RUN_BIG4_DEL_W_CUVE;
	m_ctrl_chk_big4_del_w_cuve.SetCheck( bVal );

	bVal= m_lpr_conf.LPR_DBG_RUN_BIG4_DEL_H_LINE;
	m_ctrl_chk_big4_del_h_line.SetCheck( bVal );

	bVal= m_lpr_conf.LPR_DBG_RUN_BIG4_BILINEAR ;
	m_chk_run_bilinear.SetCheck(bVal );



	bVal= m_lpr_conf.LPR_DBG_RUN_PRJ_PREDIP;
	m_chk_projection_predip.SetCheck( bVal );

	bVal= m_lpr_conf.LPR_DBG_RUN_PRJ_DEL_W_LINE;
	m_ctrl_chk_prj_del_w_line.SetCheck( bVal );

	bVal= m_lpr_conf.LPR_DBG_RUN_PRJ_DEL_W_BLT;
	m_ctrl_chk_prj_del_w_btl.SetCheck( bVal );
	
	bVal= m_lpr_conf.LPR_DBG_RUN_PRJ_DEL_W_CUVE;
	m_ctrl_chk_prj_del_w_cuve.SetCheck( bVal );
	
	bVal= m_lpr_conf.LPR_DBG_RUN_PRJ_DEL_H_LINE;
	m_ctrl_chk_prj_del_h_line.SetCheck( bVal );

	bVal= m_lpr_conf.LPR_DBG_RUN_PRJ_DEL_NOISE;
	m_ctrl_chk_prj_del_noise.SetCheck( bVal );


	bVal= m_lpr_conf.LPR_DBG_RUN_PRJ_DETAIL_RECT;
	m_ctrl_chk_rect_1st.SetCheck( bVal );
	
	
	bVal= m_lpr_conf.LPR_DBG_RUN_CHAR_SAMPLING;
	m_chk_char_sampling.SetCheck( bVal );
	
	bVal= m_lpr_conf.LPR_DBG_RUN_CHAR_RESIZE;
	m_chk_char_resize.SetCheck( bVal );

	bVal= m_lpr_conf.LPR_DBG_RUN_CHAR_THINNING;
	m_chk_thinning.SetCheck( bVal );



	/* ========================================================
	SHOW 
	===========================================================*/
	bVal = m_lpr_conf.LPR_DBG_SHOW_LOG;
	m_ctl_chk_show_lpr_log.SetCheck(bVal );

	bVal= m_lpr_conf.LPR_DBG_COPY_TRANSFER_IMAGE;
	m_chk_show_prc_image.SetCheck(bVal );

	bVal= m_lpr_conf.LPR_DBG_SHOW_UF_DIP;
	m_chk_show_uf_dip.SetCheck(bVal );

	bVal= m_lpr_conf.LPR_DBG_SHOW_UF_LINE;
	m_chk_show_uf_area.SetCheck(bVal );

	bVal= m_lpr_conf.LPR_DBG_SHOW_UF_VALID_BOX;
	m_chk_show_uf_group.SetCheck(bVal );



	bVal= m_lpr_conf.LPR_DBG_SHOW_BIG4_DIP;
	m_chk_show_big4_predip.SetCheck(bVal );

	bVal= m_lpr_conf.LPR_DBG_SHOW_BIG4_BOX;
	m_chk_show_big4.SetCheck(bVal );


	bVal= m_lpr_conf.LPR_DBG_SHOW_PRJ_DIP;
	m_chk_show_projection.SetCheck(bVal );	

	bVal= m_lpr_conf.LPR_DBG_SHOW_PRJ_VALID_BOX;
	m_chk_show_projection_group.SetCheck(bVal );

	bVal = m_lpr_conf.LPR_DBG_SHOW_LAST_PLATE_BOX;
	m_ctl_chk_show_last_plate.SetCheck( bVal );

	bVal = m_lpr_conf.LPR_DBG_SHOW_LAST_CHAR_BOX;
	m_ctrl_chk_show_last_char.SetCheck( bVal );

	bVal = m_lpr_conf.LPR_DBG_SHOW_CHAR_SEG;
	m_ctrl_chk_show_seg_char.SetCheck( bVal );

	bVal = m_lpr_conf.LPR_DBG_SHOW_W_LINE_SEG;
	m_ctrl_chk_show_w_seg_char.SetCheck( bVal );
	
	bVal = m_lpr_conf.LPR_DBG_SHOW_H_LINE_SEG;
	m_ctrl_chk_show_h_seg_char.SetCheck( bVal );

	/* Value --> Control */
	//UpdateData( FALSE);

	/* 설정 정보 */
	m_ctl_int_lpr_type = m_lpr_conf.LPR_TYPE;
	m_ctl_int_lpr_area_cnt = m_lpr_conf.LPR_AREACNT;
	m_ctl_int_resize_w = m_lpr_conf.LPR_RESIZE_W;
	m_ctl_int_min_char_h = m_lpr_conf.LPR_MIN_CHAR_H;
	m_ctl_int_min_char_w = m_lpr_conf.LPR_MIN_CHAR_W;
	m_ctl_int_min_uf_h = m_lpr_conf.LPR_UF_H;
	m_ctl_int_min_uf_w = m_lpr_conf.LPR_UF_W;
	

	m_ctl_int_min_plate_h = m_lpr_conf.LPR_MIN_PLT_H;
	m_ctl_int_min_plate_w = m_lpr_conf.LPR_MIN_PLT_W;

	m_ctl_float_min_plate_rate = m_lpr_conf.LPR_MIN_PLATE_RATIO;
	m_ctl_float_max_plate_rate = m_lpr_conf.LPR_MAX_PLATE_RATIO;
	m_ctl_float_new_plate_rate = m_lpr_conf.LPR_NEW_PLATE_RATIO;

	m_ctl_int_min_char_cnt = m_lpr_conf.LPR_MIN_CHAR_CNT;
	m_ctl_int_max_char_cnt = m_lpr_conf.LPR_MAX_CHAR_CNT;

	m_ctl_hg_mg_max_w = m_lpr_conf.LPR_MAX_HG_MG_W;
	m_ctl_hg_mg_max_interval = m_lpr_conf.LPR_MAX_HG_MG_INT;	
	
	m_ctl_l_angel_pixel = m_lpr_conf.LPR_L_ANGLE_PIXEL;	
	m_ctl_r_angel_pixel = m_lpr_conf.LPR_R_ANGLE_PIXEL;	

	m_ctl_big4_min_h = m_lpr_conf.LPR_MIN_BIG4_H;
	m_ctl_big4_min_w = m_lpr_conf.LPR_MIN_BIG4_W;

	/* OCR DATA */
	bVal = m_ocr_conf.OCR_DBG_SHOW_LOG;
	m_ctl_chk_show_ocr_log.SetCheck(bVal );
	
	bVal = m_ocr_conf.OCR_NN_TYPE;
	if( bVal == 0 )
		m_Ctrl_Radio_NN_1.SetCheck( 1 );
	else
		m_Ctrl_Radio_NN_2.SetCheck( 1 );

	m_ctl_int_ocr_threshold = m_ocr_conf.OCR_RESIZE_THRESHOLD;
	m_ctl_int_ocr_h = m_ocr_conf.OCR_CHAR_H;
	m_ctl_int_learning_cnt = m_ocr_conf.OCR_LEARNING_CNT;

	/* DIP DATAT */
	bVal = m_dip_conf.DIP_DBG_SHOW_LOG;
	m_ctl_chk_show_dip_log.SetCheck(bVal );

	/* APP DATAT */
	bVal = m_app_conf.APP_DBG_SHOW_LOG;
	m_ctl_chk_show_app_log.SetCheck( bVal);
	
	//Image Path 	
	memset( tcPath, 0, sizeof( tcPath));	
	MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED,m_app_conf.IMG_PATH, strlen(m_app_conf.IMG_PATH), tcPath, 256);
	str.Format(L"%s", tcPath );
	m_ctl_imagePath.SetWindowText( str );

	/* Value --> Control */
	UpdateData( FALSE);
	
}

BOOL CCONFDlg::Save_CFG_Dlg( void )
{
	BYTE bVal;
	CString strData;
	
	CString strPath;
	wchar_t * wPath;
	char szPath[ MAX_PATH ];
	int path_len =0;	

	int nResult1 = ERR_SUCCESS;
	int nResult2 = ERR_SUCCESS;
	int nResult3 = ERR_SUCCESS;
	int nResult4 = ERR_SUCCESS;

	/* Main process */
	bVal= (BYTE)m_chk_run_basic_study.GetCheck();
	m_lpr_conf.LPR_DBG_RUN_BASIC_STUDY = bVal;

	bVal= (BYTE)m_chk_run_resize.GetCheck();
	m_lpr_conf.LPR_DBG_RUN_RESIZE = bVal;
	
	bVal= (BYTE)m_chk_run_find_uf.GetCheck();
	m_lpr_conf.LPR_DBG_RUN_FIND_UF = bVal;

	bVal= (BYTE)m_chk_run_find_big4.GetCheck();
	m_lpr_conf.LPR_DBG_RUN_FIND_BIG4 = bVal;

	bVal= (BYTE)m_chk_run_projection.GetCheck();
	m_lpr_conf.LPR_DBG_RUN_PROJECTION = bVal;

	bVal= (BYTE)m_chk_run_priority.GetCheck();
	m_lpr_conf.LPR_DBG_RUN_PRIORITY = bVal;
	
	bVal= (BYTE)m_chk_plate_choice.GetCheck();
	m_lpr_conf.LPR_DBG_RUN_CHOICE_PLATE = bVal;

	bVal= (BYTE)m_chk_segment.GetCheck();
	m_lpr_conf.LPR_DBG_RUN_CHAR_SEGMENT = bVal;	

	bVal= (BYTE)m_chk_ocr.GetCheck();
	m_lpr_conf.LPR_DBG_RUN_CHAR_RECOGNITION = bVal;

	/* =========================================================
	sub process 
	==========================================================*/
	bVal= (BYTE)m_chk_run_pre_dip.GetCheck();
	m_lpr_conf.LPR_DBG_RUN_UF_PREDIP = bVal;

	bVal= (BYTE)m_chk_run_denoize.GetCheck();
	m_lpr_conf.LPR_DBG_RUN_UF_DEL_THICK = bVal;

	bVal= (BYTE)m_chk_run_labeling.GetCheck();
	m_lpr_conf.LPR_DBG_RUN_UF_LABELING = bVal;

	bVal= (BYTE)m_chk_run_labeling_merge.GetCheck();
	m_lpr_conf.LPR_DBG_RUN_UF_MERGE = bVal;

	bVal= (BYTE)m_chk_run_labeling_extension.GetCheck();
	m_lpr_conf.LPR_DBG_RUN_UF_EXTENSION = bVal;

	
	
	bVal= (BYTE)m_chk_run_big4_predip.GetCheck();
	m_lpr_conf.LPR_DBG_RUN_BIG4_PREDIP = bVal;

	bVal= (BYTE)m_ctrl_chk_big4_del_w_line.GetCheck();
	m_lpr_conf.LPR_DBG_RUN_BIG4_DEL_W_LINE = bVal;

	bVal= (BYTE)m_ctrl_chk_big4_del_w_blt.GetCheck();
	m_lpr_conf.LPR_DBG_RUN_BIG4_DEL_W_BLT = bVal;

	bVal= (BYTE)m_ctrl_chk_big4_del_w_cuve.GetCheck();
	m_lpr_conf.LPR_DBG_RUN_BIG4_DEL_W_CUVE= bVal;

	bVal= (BYTE)m_ctrl_chk_big4_del_h_line.GetCheck();
	m_lpr_conf.LPR_DBG_RUN_BIG4_DEL_H_LINE = bVal;

	bVal= (BYTE)m_chk_run_bilinear.GetCheck();
	m_lpr_conf.LPR_DBG_RUN_BIG4_BILINEAR = bVal;

	bVal= (BYTE)m_chk_projection_predip.GetCheck();
	m_lpr_conf.LPR_DBG_RUN_PRJ_PREDIP = bVal;

	bVal= (BYTE)m_ctrl_chk_prj_del_w_line.GetCheck();
	m_lpr_conf.LPR_DBG_RUN_PRJ_DEL_W_LINE = bVal;

	bVal= (BYTE)m_ctrl_chk_prj_del_w_btl.GetCheck();
	m_lpr_conf.LPR_DBG_RUN_PRJ_DEL_W_BLT = bVal;

	bVal= (BYTE)m_ctrl_chk_prj_del_w_cuve.GetCheck();
	m_lpr_conf.LPR_DBG_RUN_PRJ_DEL_W_CUVE= bVal;
	
	bVal= (BYTE)m_ctrl_chk_prj_del_h_line.GetCheck();
	m_lpr_conf.LPR_DBG_RUN_PRJ_DEL_H_LINE = bVal;

	bVal= (BYTE)m_ctrl_chk_prj_del_noise.GetCheck();
	m_lpr_conf.LPR_DBG_RUN_PRJ_DEL_NOISE = bVal;
	
	bVal= (BYTE)m_ctrl_chk_rect_1st.GetCheck();
	m_lpr_conf.LPR_DBG_RUN_PRJ_DETAIL_RECT= bVal;

	bVal= (BYTE)m_chk_char_sampling.GetCheck();
	m_lpr_conf.LPR_DBG_RUN_CHAR_SAMPLING = bVal;

	bVal= (BYTE)m_chk_char_resize.GetCheck();
	m_lpr_conf.LPR_DBG_RUN_CHAR_RESIZE = bVal;

	bVal= (BYTE)m_chk_thinning.GetCheck();
	m_lpr_conf.LPR_DBG_RUN_CHAR_THINNING = bVal;

	
	/* =========================================================
	SHOW 
	============================================================*/
	bVal= (BYTE)m_ctl_chk_show_lpr_log.GetCheck();;
	m_lpr_conf.LPR_DBG_SHOW_LOG = bVal;

	bVal= (BYTE)m_chk_show_prc_image.GetCheck();;
	m_lpr_conf.LPR_DBG_COPY_TRANSFER_IMAGE = bVal;

	bVal= (BYTE)m_chk_show_uf_dip.GetCheck();
	m_lpr_conf.LPR_DBG_SHOW_UF_DIP = bVal;	
	
	bVal= (BYTE)m_chk_show_uf_area.GetCheck();
	m_lpr_conf.LPR_DBG_SHOW_UF_LINE = bVal;	

	bVal= (BYTE)m_chk_show_uf_group.GetCheck();
	m_lpr_conf.LPR_DBG_SHOW_UF_VALID_BOX = bVal;


	bVal= (BYTE)m_chk_show_big4_predip.GetCheck();
	m_lpr_conf.LPR_DBG_SHOW_BIG4_DIP = bVal;	

	bVal= (BYTE)m_chk_show_big4.GetCheck();
	m_lpr_conf.LPR_DBG_SHOW_BIG4_BOX = bVal;	


	bVal= (BYTE)m_chk_show_projection.GetCheck();
	m_lpr_conf.LPR_DBG_SHOW_PRJ_DIP = bVal;		

	bVal= (BYTE)m_chk_show_projection_group.GetCheck( );
	m_lpr_conf.LPR_DBG_SHOW_PRJ_VALID_BOX = bVal;
	
	
	bVal=(BYTE) m_ctl_chk_show_last_plate.GetCheck( );
	m_lpr_conf.LPR_DBG_SHOW_LAST_PLATE_BOX= bVal;

	bVal=(BYTE) m_ctrl_chk_show_last_char.GetCheck( );
	m_lpr_conf.LPR_DBG_SHOW_LAST_CHAR_BOX= bVal;

	bVal=(BYTE) m_ctrl_chk_show_seg_char.GetCheck( );
	m_lpr_conf.LPR_DBG_SHOW_CHAR_SEG= bVal;

	bVal=(BYTE) m_ctrl_chk_show_w_seg_char.GetCheck( );
	m_lpr_conf.LPR_DBG_SHOW_W_LINE_SEG= bVal;

	bVal=(BYTE) m_ctrl_chk_show_h_seg_char.GetCheck( );
	m_lpr_conf.LPR_DBG_SHOW_H_LINE_SEG= bVal;


	/* Ctrol --> Value */
	UpdateData( TRUE );

	/* 설정 정보 */	
	m_lpr_conf.LPR_TYPE = (BYTE)m_ctl_int_lpr_type;
	m_lpr_conf.LPR_AREACNT = (BYTE)m_ctl_int_lpr_area_cnt;	
	

	m_lpr_conf.LPR_RESIZE_W = (short)m_ctl_int_resize_w;
	m_lpr_conf.LPR_MIN_CHAR_H = (short)m_ctl_int_min_char_h;
	m_lpr_conf.LPR_MIN_CHAR_W = (short)m_ctl_int_min_char_w;
	m_lpr_conf.LPR_UF_H = (short)m_ctl_int_min_uf_h;
	m_lpr_conf.LPR_UF_W = (short)m_ctl_int_min_uf_w;
	

	m_lpr_conf.LPR_MIN_PLT_H = (short)m_ctl_int_min_plate_h;
	m_lpr_conf.LPR_MIN_PLT_W = (short)m_ctl_int_min_plate_w;

	m_lpr_conf.LPR_MIN_PLATE_RATIO = m_ctl_float_min_plate_rate;
	m_lpr_conf.LPR_MAX_PLATE_RATIO = m_ctl_float_max_plate_rate;
	m_lpr_conf.LPR_NEW_PLATE_RATIO = m_ctl_float_new_plate_rate;

	m_lpr_conf.LPR_MIN_CHAR_CNT = (BYTE)m_ctl_int_min_char_cnt;
	m_lpr_conf.LPR_MAX_CHAR_CNT = (BYTE)m_ctl_int_max_char_cnt;

	m_lpr_conf.LPR_MAX_HG_MG_W		= (BYTE)m_ctl_hg_mg_max_w;
	m_lpr_conf.LPR_MAX_HG_MG_INT	= (BYTE)m_ctl_hg_mg_max_interval;

	m_lpr_conf.LPR_L_ANGLE_PIXEL = m_ctl_l_angel_pixel;
	m_lpr_conf.LPR_R_ANGLE_PIXEL = m_ctl_r_angel_pixel;	

	m_lpr_conf.LPR_MIN_BIG4_H = m_ctl_big4_min_h;
	m_lpr_conf.LPR_MIN_BIG4_W = m_ctl_big4_min_w;

	/* OCR */
	bVal= (BYTE)m_ctl_chk_show_ocr_log.GetCheck();;
	m_ocr_conf.OCR_DBG_SHOW_LOG = bVal;

	bVal= (BYTE)m_Ctrl_Radio_NN_1.GetCheck();
	if ( bVal == 1 )
		m_ocr_conf.OCR_NN_TYPE = 0;
	else
		m_ocr_conf.OCR_NN_TYPE = 1;

	m_ocr_conf.OCR_LEARNING_CNT = m_ctl_int_learning_cnt;
	m_ocr_conf.OCR_RESIZE_THRESHOLD	= (BYTE)m_ctl_int_ocr_threshold;
	m_ocr_conf.OCR_CHAR_H = (short)m_ctl_int_ocr_h;

	/* DIP */
	bVal= (BYTE)m_ctl_chk_show_dip_log.GetCheck();;
	m_dip_conf.DIP_DBG_SHOW_LOG = bVal;

	/* APP */
	bVal= (BYTE)m_ctl_chk_show_app_log.GetCheck();;
	m_app_conf.APP_DBG_SHOW_LOG = bVal;

	//Image Path 
	m_ctl_imagePath.GetWindowText( strPath );
	
	memset( szPath,0, sizeof( szPath));
	wPath =  strPath.GetBuffer(strPath.GetLength());
	path_len = WideCharToMultiByte(CP_ACP, 0, wPath, -1, NULL, 0, NULL, NULL);
	WideCharToMultiByte(CP_ACP, 0, wPath, -1, szPath, path_len, 0,0);     	

	int nlen ;
	if ( szPath != NULL && strlen( szPath) > 0 )
	{
		nlen =  strlen( szPath);
		memset( m_app_conf.IMG_PATH, 0, MAX_PATH);
		memcpy( m_app_conf.IMG_PATH, szPath, nlen);
	}
	
	nResult1 = SetLPRConf( &m_lpr_conf );
	nResult2 = SetOCRConf( &m_ocr_conf );
	nResult3 = SetDIPConf( &m_dip_conf );
	//nResult4 = SetAPPConf( &m_app_conf );
	nResult4 = m_AppClass.SetWinAPPCONF( &m_app_conf );

	if ( nResult1 == ERR_SUCCESS && nResult2 == ERR_SUCCESS && nResult3 == ERR_SUCCESS && nResult4 == ERR_SUCCESS)
	{
		strData.Format(L"정상적으로 저장 되었습니다");
		m_bSaveFlag = TRUE;
	}
	else
	{
		strData.Format(L"정상적으로 저장 되지 못했습니다.");
		m_bSaveFlag = FALSE;
	}
	
	AfxMessageBox( strData );
	return m_bSaveFlag;
}


void CCONFDlg::SetSubPrjEnable(int nEnable )
{
	if (nEnable == 1 )
	{
		m_ctrl_chk_prj_del_w_line.EnableWindow( TRUE);
		m_ctrl_chk_prj_del_w_btl.EnableWindow( TRUE);
		m_ctrl_chk_prj_del_w_cuve.EnableWindow( TRUE);
		m_ctrl_chk_rect_1st.EnableWindow( TRUE );


	}
	else
	{
		m_ctrl_chk_prj_del_w_line.EnableWindow( FALSE);
		m_ctrl_chk_prj_del_w_btl.EnableWindow( FALSE);
		m_ctrl_chk_prj_del_w_cuve.EnableWindow( FALSE);
		m_ctrl_chk_rect_1st.EnableWindow( FALSE );


	}
}

/*======================================================================================================
Wizard Function
========================================================================================================*/
void CCONFDlg::OnBnClickedButton1( )
{	
	if( Save_CFG_Dlg( ) )
	{
		if( m_ptrALPRDlg != NULL )
		{
			m_ptrALPRDlg->NotifySave();
		}
	}
}

void CCONFDlg::OnBnClickedButton2()
{
	if( m_ptrALPRDlg != NULL )
	{
		m_ptrALPRDlg->NotifyClose( IDD_CONF_DIALOG );
	}

	DestroyWindow();
}

void CCONFDlg::OnBnClickedSetPath()
{
	CString strPathName;
	TCHAR Filter[] = L"Image (*.BMP, *.JPG, *.RAW) | *.BMP;*.JPG;*.RAW | All Files(*.*)|*.*||";
	

	CFileDialog dlg(TRUE, NULL, NULL, OFN_HIDEREADONLY, Filter);

	if(IDOK == dlg.DoModal()) 
	{
		strPathName = dlg.GetPathName(); 
		strPathName = strPathName.Left(strPathName.ReverseFind('\\'));
		m_ctl_imagePath.SetWindowText( strPathName );
	}
	
}


void CCONFDlg::OnTimer(UINT_PTR nIDEvent)
{
	if( nIDEvent == CFG_INIT_TIMER )
	{
		KillTimer(CFG_INIT_TIMER);
		init_CFG_Dlg();
	}
	CDialog::OnTimer(nIDEvent);
}

int CCONFDlg::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;

	SetTimer( CFG_INIT_TIMER, 200, NULL);

	return 0;
}

void CCONFDlg::OnBnClickedChkUfProjection()
{
	if (m_chk_run_projection.GetCheck() == 1 )
	{
		SetSubPrjEnable( 1 );
	}
	else
	{
		SetSubPrjEnable( 0 );
	}
}