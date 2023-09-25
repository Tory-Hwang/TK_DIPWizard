// ALPRDlg.cpp : ���� �����Դϴ�.
//

/*================================================================================
Copyright (c) 2011 Tory Hwang 
Modify : Tory in 2013 .01

Mail : tory45@empal.com
*=================================================================================*/

#include "stdafx.h"
#include "TK_ALPRWizard.h"
#include "ALPRDlg.h"

#include "TK_ALPRWizardDoc.h"
#include "ALPRWizard_Def.h"
#include "../include/TK_OCR_TYPE.h"
#include "../include/TK_CONF.h"

#define TIMER_AUTO			1000
#define TIMER_AUTO_INTER	2000

#define TIMER_DLG_MAX_SIZE	2001
#define TIMER_KEYSELECT		2002
// CALPRDlg ��ȭ �����Դϴ�.

IMPLEMENT_DYNAMIC(CALPRDlg, CDialog)


CALPRDlg::CALPRDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CALPRDlg::IDD, pParent)	
{
	m_ptrConfDlg = NULL;
	m_ptrDipDlg = NULL;
}

CALPRDlg::~CALPRDlg()
{
	if ( m_ptrConfDlg != NULL )
	{
		delete m_ptrConfDlg;
		m_ptrConfDlg = NULL;
	}

	if ( m_ptrDipDlg != NULL )
	{
		delete m_ptrDipDlg;
		m_ptrDipDlg = NULL;
	}
}

void CALPRDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_FILE_LIST, m_CtlFileList);
	DDX_Control(pDX, IDC_E_IMAGE_SIZE, m_Ctl_Img_Size);
	DDX_Control(pDX, IDC_E_IMAGE_BITCNT, m_Ctl_Img_BitCnt);
	DDX_Control(pDX, IDC_E_IMAGE_WIDTH, m_Ctl_Img_Width);
	DDX_Control(pDX, IDC_E_IMAGE_HEIGHT, m_Ctl_Img_Height);
	
	DDX_Control(pDX, IDC_E_TIME, m_Ctl_ProcessTime);
	DDX_Control(pDX, IDC_CHK_AUTO_VIEW, m_Ctl_Chek_AutoView);	
	DDX_Control(pDX, IDC_CHECK1, m_CtlAuto);
	DDX_Control(pDX, IDC_BTN_FILE, m_CtlReadFile);
	DDX_Control(pDX, IDC_LOG_LIST, m_CtlLog_List);
	
	DDX_Control(pDX, IDC_CHECK2, m_CtlCheckCreateWindow);

	DDX_Control(pDX, IDC_COM1, m_CtlOCR1);
	DDX_Control(pDX, IDC_COM2, m_CtlOCR2);
	DDX_Control(pDX, IDC_COM3, m_CtlOCR3);
	DDX_Control(pDX, COM4, m_CtlOCR4);
	DDX_Control(pDX, COM5, m_CtlOCR5);
	DDX_Control(pDX, COM6, m_CtlOCR6);
	DDX_Control(pDX, COM7, m_CtlOCR7);
	DDX_Control(pDX, COM8, m_CtlOCR8);
	DDX_Control(pDX, COM9, m_CtlOCR9);
	DDX_Control(pDX, COM10, m_CtlOCR10);
	DDX_Control(pDX, IDC_BUTTON25, m_CtlSave1);
	DDX_Control(pDX, IDC_BUTTON26, m_CtlSave2);
	DDX_Control(pDX, IDC_BUTTON27, m_CtlSave3);
	DDX_Control(pDX, IDC_BUTTON28, m_CtlSave4);
	DDX_Control(pDX, IDC_BUTTON29, m_CtlSave5);
	DDX_Control(pDX, IDC_BUTTON30, m_CtlSave6);
	DDX_Control(pDX, IDC_BUTTON31, m_CtlSave7);
	DDX_Control(pDX, IDC_BUTTON32, m_CtlSave8);
	DDX_Control(pDX, IDC_BUTTON33, m_CtlSave9);
	DDX_Control(pDX, IDC_BUTTON34, m_CtlSave10);
	DDX_Control(pDX, IDC_PLATE_NUMBER, m_CtlPlateNumber);
	DDX_Control(pDX, IDC_STUDY_CNT, m_Ctl_StudyCnt);
	DDX_Control(pDX, IDC_ERR_RATE, m_Ctl_Err_Rate);
	DDX_Control(pDX, IDC_FEATURE_CNT, m_Ctl_FeatureCnt );

	DDX_Control(pDX, IDC_EDIT1, m_Ctrl_SaveName);
	
	DDX_Control(pDX, COM11, m_CtlOCR11);
	DDX_Control(pDX, COM12, m_CtlOCR12);
	DDX_Control(pDX, COM13, m_CtlOCR13);
	DDX_Control(pDX, IDC_BUTTON39, m_CtlSave11);
	DDX_Control(pDX, IDC_BUTTON40, m_CtlSave12);
	DDX_Control(pDX, IDC_BUTTON41, m_CtlSave13);
	DDX_Control(pDX, COM14, m_CtlOCR14);
	DDX_Control(pDX, COM15, m_CtlOCR15);
	DDX_Control(pDX, COM16, m_CtlOCR16);
	DDX_Control(pDX, IDC_BUTTON42, m_CtlSave14);
	DDX_Control(pDX, IDC_BUTTON43, m_CtlSave15);
	DDX_Control(pDX, IDC_BUTTON44, m_CtlSave16);
	DDX_Control(pDX, IDC_CHK_RAW_SAVE, m_chk_raw_save);
	DDX_Control(pDX, IDC_CHECK3, m_chk_all_save);
	DDX_Control(pDX, IDC_CHk_SAME_OCR, m_chk_same_ocr);

	DDX_Control(pDX, IDC_IMG_PATH, m_ctl_img_path);

	DDX_Control(pDX, IDC_APP_VER, m_ctl_APP_VER);
	DDX_Control(pDX, IDC_DIP_VER, m_ctl_DIP_VER);
	DDX_Control(pDX, IDC_OCR_VER, m_ctl_OCR_VER);
	DDX_Control(pDX, IDC_LPR_VER, m_ctl_LPR_VER);	
}


BEGIN_MESSAGE_MAP(CALPRDlg, CDialog)
	ON_BN_CLICKED(IDC_BTN_FILE, &CALPRDlg::OnBnClickedBtnFile)
	ON_NOTIFY(NM_CLICK, IDC_FILE_LIST, &CALPRDlg::OnNMClickFileList)
	ON_BN_CLICKED(IDC_BTN_ALL_Close, &CALPRDlg::OnBnClickedBtnAllClose)
	ON_BN_CLICKED(IDC_CHECK1, &CALPRDlg::OnBnClickedCheck1)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BTN_CLEAR, &CALPRDlg::OnBnClickedBtnClear)
	
	
	ON_BN_CLICKED(IDC_CHECK2, &CALPRDlg::OnBnClickedCheck2)
	ON_WM_PAINT()
	ON_WM_CTLCOLOR()
	ON_BN_CLICKED(IDC_BUTTON25, &CALPRDlg::OnBnClickedButton25)
	ON_BN_CLICKED(IDC_BUTTON26, &CALPRDlg::OnBnClickedButton26)
	ON_BN_CLICKED(IDC_BUTTON27, &CALPRDlg::OnBnClickedButton27)
	ON_BN_CLICKED(IDC_BUTTON28, &CALPRDlg::OnBnClickedButton28)
	ON_BN_CLICKED(IDC_BUTTON29, &CALPRDlg::OnBnClickedButton29)
	ON_BN_CLICKED(IDC_BUTTON30, &CALPRDlg::OnBnClickedButton30)
	ON_BN_CLICKED(IDC_BUTTON31, &CALPRDlg::OnBnClickedButton31)
	ON_BN_CLICKED(IDC_BUTTON32, &CALPRDlg::OnBnClickedButton32)
	ON_BN_CLICKED(IDC_BUTTON33, &CALPRDlg::OnBnClickedButton33)
	ON_BN_CLICKED(IDC_BUTTON34, &CALPRDlg::OnBnClickedButton34)
	ON_BN_CLICKED(IDC_BUTTON24, &CALPRDlg::OnBnClickedButton24)
	ON_BN_CLICKED(IDC_BTNSTUDY, &CALPRDlg::OnBnClickedBtnstudy)
	ON_BN_CLICKED(IDC_BUTTON35, &CALPRDlg::OnBnClickedButton35)
	ON_BN_CLICKED(IDC_BTN_CAMERA, &CALPRDlg::OnBnClickedBtnCamera)	
	ON_NOTIFY(LVN_KEYDOWN, IDC_FILE_LIST, &CALPRDlg::OnLvnKeydownFileList)	
	ON_BN_CLICKED(IDC_BT_IMAGE_SAVE, &CALPRDlg::OnBnClickedBtImageSave)
	ON_BN_CLICKED(IDC_BTN_SHOW_CONF, &CALPRDlg::OnBnClickedBtnShowConf)
	ON_WM_CLOSE()	
	ON_BN_CLICKED(IDC_CHK_AUTO_VIEW, &CALPRDlg::OnBnClickedChkAutoView)
	ON_BN_CLICKED(IDC_BUTTON39, &CALPRDlg::OnBnClickedButton39)
	ON_BN_CLICKED(IDC_BUTTON40, &CALPRDlg::OnBnClickedButton40)
	ON_BN_CLICKED(IDC_BUTTON41, &CALPRDlg::OnBnClickedButton41)
	ON_BN_CLICKED(IDC_BUTTON42, &CALPRDlg::OnBnClickedButton42)
	ON_BN_CLICKED(IDC_BUTTON43, &CALPRDlg::OnBnClickedButton43)
	ON_BN_CLICKED(IDC_BUTTON44, &CALPRDlg::OnBnClickedButton44)	
	ON_BN_CLICKED(IDC_BTN_SHOW_DIP, &CALPRDlg::OnBnClickedBtnShowDip)
	ON_EN_CHANGE(IDC_E_IMAGE_BITCNT, &CALPRDlg::OnEnChangeEImageBitcnt)	
	ON_BN_CLICKED(IDC_BTNFEATURE, &CALPRDlg::OnBnClickedBtnfeature)
	ON_BN_CLICKED(IDC_BTN_BACKUP, &CALPRDlg::OnBnClickedBtnBackup)

	ON_BN_CLICKED(IDC_CHECK3, &CALPRDlg::OnBnClickedCheck3)	
	ON_BN_CLICKED(IDC_CHk_SAME_OCR, &CALPRDlg::OnBnClickedSameOCR)	

	ON_BN_CLICKED(IDC_BTN_NN_SAVE, &CALPRDlg::OnBnClickedBtnNnSave)	
END_MESSAGE_MAP()


/* ================================================================================
User Function
===================================================================================*/
int CALPRDlg::NotifySave()
{
	ReloadConf();
	return ERR_SUCCESS;
}
int CALPRDlg::NotifyClose(  UINT uDLGID  )
{
	if ( uDLGID == IDD_CONF_DIALOG )
	{
		m_bOpenedConfDlg = FALSE;
	}
	else if (uDLGID == IDD_DIP_DIALOG )
	{
		m_bOpenedDipDlg = FALSE;
	}

	return ERR_SUCCESS;
}

int CALPRDlg::Init_Dlg( void )
{	
	
	/* File list setting */
	m_CtlFileList.SetExtendedStyle(LVS_EX_FLATSB | LVS_EX_FULLROWSELECT);
	m_CtlFileList.InsertColumn(0, _T("SEQ")		, LVCFMT_RIGHT, 50);
	m_CtlFileList.InsertColumn(1, _T("Name")	, LVCFMT_LEFT, 250);

	/* log list setting */
	//m_CtlLog_List.SetExtendedStyle(LVS_EX_FLATSB | LVS_EX_FULLROWSELECT);
	//m_CtlLog_List.InsertColumn(0, _T("SEQ")		, LVCFMT_RIGHT, 50);
	//m_CtlLog_List.InsertColumn(1, _T("FileName")	, LVCFMT_LEFT, 250);
	//m_CtlLog_List.InsertColumn(2, _T("PlateNumber")	, LVCFMT_LEFT, 150);
	//m_CtlLog_List.InsertColumn(3, _T("ProcessTime(ms)")	, LVCFMT_LEFT, 150);

	/*ALPR Check */
	m_Ctl_Chek_AutoView.SetCheck( FALSE );

	/* Auto Check */
	m_CtlAuto.SetCheck( FALSE );
	m_bAutoRuning = FALSE;
	
	/* Multi Document Check */
	m_CtlCheckCreateWindow.SetCheck( TRUE );

	/* Show Dlg Normal size */
	SetCreateWindow( TRUE );

	memset(&m_AutoIndex, 0, sizeof( AUTO_INDEX_t ));
	memset(m_tcBaseChar, 0, sizeof(m_tcBaseChar));
	m_bAutoFeatureSave = FALSE;

	ClearImageData();

	Init_OCR();

	/* 200ms �� Dlg �ִ� ������� ���� */
	SetTimer( TIMER_DLG_MAX_SIZE, 200, NULL );


	/* ���� ��ȣ ���� ũ�� */
	LargeFont.CreateFont(60, 0, 0, 0, FW_EXTRABOLD, 0, 0, 0,
		DEFAULT_CHARSET, 
		OUT_CHARACTER_PRECIS,
		CLIP_CHARACTER_PRECIS,
		DEFAULT_QUALITY,DEFAULT_PITCH, 
		_T("Arial"));

	CStatic *sStatic = ((CStatic*)GetDlgItem(IDC_PLATE_NUMBER));
	sStatic->SetFont(&LargeFont,TRUE);

	m_hTh = NULL;
	m_dwTh = 0;
	m_bOpenedConfDlg = FALSE;
	m_bOpenedDipDlg = FALSE;

	GetImagePath();
	SetVersion();

	UpdateData( FALSE );

	return 0;
}

/*======================================================================
typedef struct OCR_CTRL_
{
	CComboBox * ptrOCRCombo[ MAX_OCR_CTRL_CNT ];
	CButton * ptrOCRButton[ MAX_OCR_CTRL_CNT ];
	CWnd * ptrOCRComboWnd[ MAX_OCR_CTRL_CNT ];
	CWnd * ptrOCRButtonWnd[ MAX_OCR_CTRL_CNT ];
}OCR_CTRL_t;
========================================================================*/
int CALPRDlg::Init_OCR( void )
{
	int i, j, nOCRCnt;
	CString strOCR;
	TCHAR szOCR[ 2 ];

	nOCRCnt= sizeof( OCR_UNIDATA ) / sizeof( unsigned short);
	memset( &m_OCR_Ctrl , 0, sizeof( m_OCR_Ctrl ));

	/* OCR Control ����ü ��� */
	m_OCR_Ctrl.ptrOCRButton[ 0 ] = &m_CtlSave1;
	m_OCR_Ctrl.ptrOCRButton[ 1 ] = &m_CtlSave2;
	m_OCR_Ctrl.ptrOCRButton[ 2 ] = &m_CtlSave3;
	m_OCR_Ctrl.ptrOCRButton[ 3 ] = &m_CtlSave4;
	m_OCR_Ctrl.ptrOCRButton[ 4 ] = &m_CtlSave5;
	m_OCR_Ctrl.ptrOCRButton[ 5 ] = &m_CtlSave6;
	m_OCR_Ctrl.ptrOCRButton[ 6 ] = &m_CtlSave7;
	m_OCR_Ctrl.ptrOCRButton[ 7 ] = &m_CtlSave8;
	m_OCR_Ctrl.ptrOCRButton[ 8 ] = &m_CtlSave9;
	m_OCR_Ctrl.ptrOCRButton[ 9 ] = &m_CtlSave10;
	m_OCR_Ctrl.ptrOCRButton[ 10 ] = &m_CtlSave11;
	m_OCR_Ctrl.ptrOCRButton[ 11 ] = &m_CtlSave12;
	m_OCR_Ctrl.ptrOCRButton[ 12 ] = &m_CtlSave13;
	m_OCR_Ctrl.ptrOCRButton[ 13 ] = &m_CtlSave14;
	m_OCR_Ctrl.ptrOCRButton[ 14 ] = &m_CtlSave15;
	m_OCR_Ctrl.ptrOCRButton[ 15 ] = &m_CtlSave16;
	


	m_OCR_Ctrl.ptrOCRButtonWnd[ 0 ] = GetDlgItem( IDC_BUTTON25 );
	m_OCR_Ctrl.ptrOCRButtonWnd[ 1 ] = GetDlgItem( IDC_BUTTON26 );
	m_OCR_Ctrl.ptrOCRButtonWnd[ 2 ] = GetDlgItem( IDC_BUTTON27 );
	m_OCR_Ctrl.ptrOCRButtonWnd[ 3 ] = GetDlgItem( IDC_BUTTON28 );
	m_OCR_Ctrl.ptrOCRButtonWnd[ 4 ] = GetDlgItem( IDC_BUTTON29 );
	m_OCR_Ctrl.ptrOCRButtonWnd[ 5 ] = GetDlgItem( IDC_BUTTON30 );
	m_OCR_Ctrl.ptrOCRButtonWnd[ 6 ] = GetDlgItem( IDC_BUTTON31 );
	m_OCR_Ctrl.ptrOCRButtonWnd[ 7 ] = GetDlgItem( IDC_BUTTON32 );
	m_OCR_Ctrl.ptrOCRButtonWnd[ 8 ] = GetDlgItem( IDC_BUTTON33 );
	m_OCR_Ctrl.ptrOCRButtonWnd[ 9 ] = GetDlgItem( IDC_BUTTON34 );
	m_OCR_Ctrl.ptrOCRButtonWnd[ 10 ] = GetDlgItem( IDC_BUTTON39 );
	m_OCR_Ctrl.ptrOCRButtonWnd[ 11 ] = GetDlgItem( IDC_BUTTON40 );
	m_OCR_Ctrl.ptrOCRButtonWnd[ 12 ] = GetDlgItem( IDC_BUTTON41 );

	m_OCR_Ctrl.ptrOCRButtonWnd[ 13 ] = GetDlgItem( IDC_BUTTON42 );
	m_OCR_Ctrl.ptrOCRButtonWnd[ 14 ] = GetDlgItem( IDC_BUTTON43 );
	m_OCR_Ctrl.ptrOCRButtonWnd[ 15 ] = GetDlgItem( IDC_BUTTON44 );


	m_OCR_Ctrl.ptrOCRCombo[ 0 ] = &m_CtlOCR1;
	m_OCR_Ctrl.ptrOCRCombo[ 1 ] = &m_CtlOCR2;
	m_OCR_Ctrl.ptrOCRCombo[ 2 ] = &m_CtlOCR3;
	m_OCR_Ctrl.ptrOCRCombo[ 3 ] = &m_CtlOCR4;
	m_OCR_Ctrl.ptrOCRCombo[ 4 ] = &m_CtlOCR5;
	m_OCR_Ctrl.ptrOCRCombo[ 5 ] = &m_CtlOCR6;
	m_OCR_Ctrl.ptrOCRCombo[ 6 ] = &m_CtlOCR7;
	m_OCR_Ctrl.ptrOCRCombo[ 7 ] = &m_CtlOCR8;
	m_OCR_Ctrl.ptrOCRCombo[ 8 ] = &m_CtlOCR9;
	m_OCR_Ctrl.ptrOCRCombo[ 9 ] = &m_CtlOCR10;
	m_OCR_Ctrl.ptrOCRCombo[ 10 ] = &m_CtlOCR11;
	m_OCR_Ctrl.ptrOCRCombo[ 11 ] = &m_CtlOCR12;
	m_OCR_Ctrl.ptrOCRCombo[ 12 ] = &m_CtlOCR13;

	m_OCR_Ctrl.ptrOCRCombo[ 13 ] = &m_CtlOCR14;
	m_OCR_Ctrl.ptrOCRCombo[ 14 ] = &m_CtlOCR15;
	m_OCR_Ctrl.ptrOCRCombo[ 15 ] = &m_CtlOCR16;

	m_OCR_Ctrl.ptrOCRComboWnd[ 0 ] = GetDlgItem( IDC_COM1 );
	m_OCR_Ctrl.ptrOCRComboWnd[ 1 ] = GetDlgItem( IDC_COM2 );
	m_OCR_Ctrl.ptrOCRComboWnd[ 2 ] = GetDlgItem( IDC_COM3 );
	m_OCR_Ctrl.ptrOCRComboWnd[ 3 ] = GetDlgItem( COM4 );
	m_OCR_Ctrl.ptrOCRComboWnd[ 4 ] = GetDlgItem( COM5 );
	m_OCR_Ctrl.ptrOCRComboWnd[ 5 ] = GetDlgItem( COM6 );
	m_OCR_Ctrl.ptrOCRComboWnd[ 6 ] = GetDlgItem( COM7 );
	m_OCR_Ctrl.ptrOCRComboWnd[ 7 ] = GetDlgItem( COM8 );
	m_OCR_Ctrl.ptrOCRComboWnd[ 8 ] = GetDlgItem( COM9 );
	m_OCR_Ctrl.ptrOCRComboWnd[ 9 ] = GetDlgItem( COM10 );
	m_OCR_Ctrl.ptrOCRComboWnd[ 10 ] = GetDlgItem( COM11 );
	m_OCR_Ctrl.ptrOCRComboWnd[ 11 ] = GetDlgItem( COM12 );
	m_OCR_Ctrl.ptrOCRComboWnd[ 12 ] = GetDlgItem( COM13 );

	m_OCR_Ctrl.ptrOCRComboWnd[ 13 ] = GetDlgItem( COM14 );
	m_OCR_Ctrl.ptrOCRComboWnd[ 14 ] = GetDlgItem( COM15 );
	m_OCR_Ctrl.ptrOCRComboWnd[ 15 ] = GetDlgItem( COM16 );


#if 0
	for( j = 0 ; j < MAX_OCR_CTRL_CNT ; j++)
	{
		m_OCR_Ctrl.ptrOCRCombo[ j ]->AddString( L"-1" );
	}
#endif

	/* OCR Control ���� ��� */
	for( i = 0; i < nOCRCnt ; i++  )
	{
		memset( szOCR, 0, sizeof( szOCR));
		if ( OCR_UNIDATA[ i ] == 0x0000 )
		{
			strOCR.Format(L"%s", L"-1");
		}
		else if ( OCR_UNIDATA[ i ] == 0x0061 )
		{
			strOCR.Format(L"%s", L"����");
		}
		else if ( OCR_UNIDATA[ i ] == 0x0062 )
		{
			strOCR.Format(L"%s", L"�λ�");
		}
		else if ( OCR_UNIDATA[ i ] == 0x0063 )
		{
			strOCR.Format(L"%s", L"�뱸");
		}
		else if ( OCR_UNIDATA[ i ] == 0x0064 )
		{
			strOCR.Format(L"%s", L"��õ");
		}
		else if ( OCR_UNIDATA[ i ] == 0x0065 )
		{
			strOCR.Format(L"%s", L"����");
		}
		else if ( OCR_UNIDATA[ i ] == 0x0066 )
		{
			strOCR.Format(L"%s", L"����");
		}
		else if ( OCR_UNIDATA[ i ] == 0x0067 )
		{
			strOCR.Format(L"%s", L"���");
		}
		else if ( OCR_UNIDATA[ i ] == 0x0068 )
		{
			strOCR.Format(L"%s", L"���");
		}
		else if ( OCR_UNIDATA[ i ] == 0x0069 )
		{
			strOCR.Format(L"%s", L"����");
		}
		else if ( OCR_UNIDATA[ i ] == 0x006A )
		{
			strOCR.Format(L"%s", L"���");
		}
		else if ( OCR_UNIDATA[ i ] == 0x006B )
		{
			strOCR.Format(L"%s", L"�泲");
		}
		else if ( OCR_UNIDATA[ i ] == 0x006C )
		{
			strOCR.Format(L"%s", L"����");
		}
		else if ( OCR_UNIDATA[ i ] == 0x006D )
		{
			strOCR.Format(L"%s", L"����");
		}
		else if ( OCR_UNIDATA[ i ] == 0x006E )
		{
			strOCR.Format(L"%s", L"���");
		}
		else if ( OCR_UNIDATA[ i ] == 0x006F )
		{
			strOCR.Format(L"%s", L"�泲");
		}
		else if ( OCR_UNIDATA[ i ] == 0x0070 )
		{
			strOCR.Format(L"%s", L"����");
		}
		else
		{
			
			memcpy( &szOCR, &OCR_UNIDATA[ i ], sizeof( unsigned short));		
			strOCR.Format(L"%s", szOCR);
		}
		
		for( j = 0 ; j < MAX_OCR_CTRL_CNT ; j++)
		{
			m_OCR_Ctrl.ptrOCRCombo[ j ]->AddString( strOCR );
		}
	}

	/* Dlg �ʱ� �� OCR ���� �� Combo box �Ⱥ��̰� */
	for( i = 0; i < MAX_OCR_CTRL_CNT ; i++ )
	{			
		m_OCR_Ctrl.ptrOCRButtonWnd[ i ]->ShowWindow( FALSE );
	}

	for( i = 0; i < MAX_OCR_CTRL_CNT ; i++ )
	{			
		m_OCR_Ctrl.ptrOCRComboWnd[ i ]->ShowWindow( FALSE );
	}

	m_chk_all_save.SetCheck( 1 );
	return 0;

}

/*======================================================================
ù��° Document Interface ��û 
========================================================================*/
CDocument * CALPRDlg::GetFirstDocument( void )
{
	CDocument* pDT = NULL;	
	CTK_ALPRWizardApp* pApp = (CTK_ALPRWizardApp*)AfxGetApp();	
	
	pDT = pApp->GetFirstDoc();
	return pDT;
}

/*======================================================================
Ȱ��ȭ �� Document Interface ��û 
========================================================================*/
CDocument * CALPRDlg::GetActiveDocument( void )
{	
	CDocument* pDT = NULL;	
	CTK_ALPRWizardApp* pApp = (CTK_ALPRWizardApp*)AfxGetApp();	

	if ( m_CtlCheckCreateWindow.GetCheck() == FALSE )
	{
		pDT = pApp->GetFirstDoc();
	}
	else
	{
		pDT = pApp->GetActiveDoc();
	}
	
	return pDT;
}

/*======================================================================
Image Information Display
========================================================================*/
void CALPRDlg::DisplayActiveView( TCHAR * ptrtcFileName, IMAGE_DATA_t * ptrImageData )
{
	CString strImageBitCnt;
	CString strImageSize;
	CString strImageH;
	CString strImageW;	
	CString strName;

	if ( ptrImageData == NULL )
	{
		strImageSize.Format(L"0");
		strImageBitCnt.Format(L"0");
		strImageW.Format(L"0");
		strImageH.Format(L"0");
		strName.Format( L"");
	}
	else
	{
		strImageSize.Format(L"%d", ptrImageData->nImageSize );
		strImageBitCnt.Format(L"%d", ptrImageData->nBitCount );
		strImageW.Format(L"%d", ptrImageData->nW );
		strImageH.Format(L"%d", ptrImageData->nH );
		strName.Format( L"%s", ptrtcFileName );
	}
	

	m_Ctl_Img_Size.SetWindowText(strImageSize);
	m_Ctl_Img_BitCnt.SetWindowText(strImageBitCnt);
	m_Ctl_Img_Width.SetWindowText(strImageW);
	m_Ctl_Img_Height.SetWindowText(strImageH);

	m_Ctrl_SaveName.SetWindowText( strName );
}

/*======================================================================
Image Information Display
========================================================================*/
void CALPRDlg::DisplayImageData( CString & strName, IMAGE_DATA_t * ptrImageData , int Area)
{
	
	CTK_ALPRWizardDoc * ptrDoc= NULL;	
	TCHAR tcFileName[ MAX_PATH ];
	
	ptrDoc = (CTK_ALPRWizardDoc * )GetFirstDocument();

	if ( ptrDoc == NULL )
	{
		return ;
	}
	
	memset( tcFileName, 0, sizeof( tcFileName ));
	wsprintf( tcFileName,L"%s", strName);

	/* ALPR�� üũ �Ǿ� ���� ���� ��� ���� ���� Display �� 
	   ALPR üũ �� LPR ������� Display ��  */	

	if ( m_Ctl_Chek_AutoView.GetCheck() == 0 )
		ptrDoc->Doc_DisplaySrcImage( tcFileName, ptrImageData, Area );
	else
		ptrDoc->Doc_LPR( tcFileName, ptrImageData, Area );
	
	/* ���� �̹��� ���� Display */
	DisplayActiveView(tcFileName, ptrImageData );
	
}

/*======================================================================
Image Information �ʱ�ȭ
========================================================================*/
void CALPRDlg::ClearImageData( void )
{
	m_Ctl_Img_Size.SetWindowText(L"0");
	m_Ctl_Img_BitCnt.SetWindowText(L"0");
	m_Ctl_Img_Width.SetWindowText(L"0");
	m_Ctl_Img_Height.SetWindowText(L"0");	
	m_Ctl_ProcessTime.SetWindowText(L"0" );

	m_CtlFileList.DeleteAllItems();
}

/*======================================================================
File list ��� 
========================================================================*/
void CALPRDlg::DisplayFileList( void )
{
	TCHAR strIndex[ 50 ];
	int iIdx;	
	LV_ITEM lvitem;	
	int i;

	for( i = 0 ; i < m_FileManager.Cnt ; i++ )
	{
		memset( strIndex, 0, sizeof( strIndex ) );
		iIdx = m_CtlFileList.GetItemCount();
		lvitem.iItem = iIdx;

		wsprintf( strIndex,L"%d", iIdx+1 );

		lvitem.mask = LVIF_TEXT ;
		lvitem.iSubItem = 0;
		lvitem.pszText = (LPWSTR)&strIndex;
		m_CtlFileList.InsertItem(&lvitem);	
		m_CtlFileList.SetItemText(iIdx, 1, m_FileManager.Files[ i ].tcName);
		//m_CtlFileList.EnsureVisible( iIdx, TRUE);
	}
}

/*======================================================================
���� ��ȣ ���
========================================================================*/
void CALPRDlg::DisplayCharNumber( int nCharCnt , unsigned short * ptrPlateNumber )
{	
	int i, j;
	CTK_ALPRWizardDoc * ptrActiveDoc= NULL;
	CPaintDC dc( this );
	CString strPlateNumber;
	CString strChar;
	TCHAR tcChar[ MAX_PATH ];

	ptrActiveDoc = (CTK_ALPRWizardDoc * )GetActiveDocument();

	if( ptrActiveDoc == NULL )
		return;

	/*Dlg�� ���� ��ȣ�� ���� Display(Draw) */
	ptrActiveDoc->Doc_DrawCharsImageDlg(dc.m_hDC );
	
	/* ������ ������ŭ OCR ���� ���� ��ư ǥ�� */
	for( i = 0; i < MAX_OCR_CTRL_CNT ; i++ )
	{	
		if ( i < nCharCnt )
			m_OCR_Ctrl.ptrOCRButtonWnd[ i ]->ShowWindow( TRUE );
		else
			m_OCR_Ctrl.ptrOCRButtonWnd[ i ]->ShowWindow( FALSE );
	}

	/* ������ ������ŭ OCR ���� �޺� �ڽ� ǥ�� */
	for( i = 0; i < MAX_OCR_CTRL_CNT ; i++ )
	{			
		if ( i < nCharCnt )
			m_OCR_Ctrl.ptrOCRComboWnd[ i ]->ShowWindow( TRUE );
		else
			m_OCR_Ctrl.ptrOCRComboWnd[ i ]->ShowWindow( FALSE );


		m_OCR_Ctrl.ptrOCRCombo[ i ]->SetCurSel( 0 );
	}

	if( wcslen( m_tcBaseChar ) > 0 )
	{
		/* ������ ������ŭ OCR ���� �޺� �ڽ� ǥ�� */
		for( i = 0 ; i < OCR_MAX_CODE; i++)
		{
			m_OCR_Ctrl.ptrOCRCombo[ 0 ]->GetLBText(i, strChar );		
			memset(tcChar, 0, sizeof(tcChar));
			wcscpy_s( tcChar, strChar.GetLength() + 1 , strChar );

			if( wcscmp(tcChar, m_tcBaseChar ) == 0 )
			{
				for( j = 0; j < MAX_OCR_CTRL_CNT ; j++ )
				{
					m_OCR_Ctrl.ptrOCRCombo[ j ]->SetCurSel( i );					
				}
				break;
			}
		}
	}

	

	/* ��ȣ�� ǥ�� */
	strPlateNumber.Format(L"%s", ptrPlateNumber );
	m_CtlPlateNumber.SetWindowText( strPlateNumber );

	OnBnClickedCheck3();

	/* Feature �ڵ� ���� */
	if( m_bAutoFeatureSave )
	{
		OnBnClickedButton24();
	}

}

/*======================================================================
���� ��ȣ�� ���� �ð� ��� �� log list ��� 
========================================================================*/
void CALPRDlg::DisplayProcessingTime( TCHAR * ptrtcFileName, TCHAR * ptrPlateNumber, DWORD nTime )
{
	CString strMsg;

	strMsg.Format(L"%u", nTime);
	m_Ctl_ProcessTime.SetWindowText( strMsg );
#if 0
	
	TCHAR strIndex[ 50 ];
	int iIdx;	
	LV_ITEM lvitem;	

	( void )ptrPlateNumber;
	
	/* process �ð� ��� */
	
	/* log list ��� */
	memset( strIndex, 0, sizeof( strIndex ) );
	iIdx = m_CtlLog_List.GetItemCount();
	lvitem.iItem = iIdx;

	wsprintf( strIndex,L"%d", iIdx+1 );
	lvitem.mask = LVIF_TEXT ;
	lvitem.iSubItem = 0;
	lvitem.pszText = (LPWSTR)&strIndex;
	m_CtlLog_List.InsertItem(&lvitem);	
	m_CtlLog_List.SetItemText(iIdx, 1, ptrtcFileName );
	m_CtlLog_List.SetItemText(iIdx, 3, strMsg );
	m_CtlLog_List.EnsureVisible( iIdx, TRUE);	
#endif
}

/*======================================================================
�Ű�� �н� �ݺ� Ƚ�� �� ���� ���� ��� 
========================================================================*/
void CALPRDlg::DisplayStudyState( int nStudyCnt, double dErr )
{
	CString strMsg;
	CString strMsg2;
	CString strMsg3;

	strMsg.Format(L"%d", nStudyCnt );
	strMsg2.Format(L"%lf", dErr );	
	
	strMsg3.Format(L"�Ű�� �н� �� �Դϴ�.[ %d ]", nStudyCnt );

	m_Ctl_StudyCnt.SetWindowText( strMsg );
	m_Ctl_Err_Rate.SetWindowText( strMsg2 );
	m_CtlPlateNumber.SetWindowText( strMsg3 );
	
}

/*======================================================================
LPR ó��( ��� ���� )
========================================================================*/
void CALPRDlg::LPRProcess( void )
{
#if 0
	CTK_ALPRWizardDoc * ptrActiveDoc= NULL;
	ptrActiveDoc = (CTK_ALPRWizardDoc * )GetActiveDocument();

	if ( ptrActiveDoc == NULL )
	{
		AfxMessageBox(IDS_ERR_NO_IMAGE );
		return ;
	}

	ptrActiveDoc->Doc_LPR();
#endif

}

/*======================================================================
Document Interface �������� üũ 
========================================================================*/
BOOL CALPRDlg::CheckValidDoc( void * ptrDoc )
{
	if ( ptrDoc == NULL )
	{
		AfxMessageBox(IDS_ERR_NO_IMAGE );
		return FALSE;
	}
	return TRUE;
}

/*======================================================================
Dialog ȭ�� ��� 
========================================================================*/
void CALPRDlg::SetCreateWindow( BOOL bCreate )
{
	CTK_ALPRWizardApp* pApp = (CTK_ALPRWizardApp*)AfxGetApp();
	pApp->SetCreateWindow( bCreate );
}

/*======================================================================
���� �̹��� ����
========================================================================*/
void CALPRDlg::SaveCurrentImage( void )
{
	CString strFileName;
	TCHAR tcFileName[ MAX_PATH ];
	TCHAR tcOrgName[ MAX_PATH];

	CTK_ALPRWizardDoc * ptrActiveDoc= NULL;
	int nErrCode = ERR_SUCCESS;
	int nRawSave = 0;

	ptrActiveDoc = (CTK_ALPRWizardDoc * )GetActiveDocument();

	memset( tcFileName, 0, sizeof( tcFileName ));
	memset( tcOrgName, 0, sizeof( tcOrgName ));

	m_Ctrl_SaveName.GetWindowText( strFileName );
	strFileName.MakeUpper();

	nRawSave= m_chk_raw_save.GetCheck();

	if ( nRawSave != 1 && strFileName.Right( 4 ) != L".BMP")
	{
		AfxMessageBox(L"BMP ���Ϸθ� ������ �� �ֽ��ϴ�.");
		return;
	}
	wsprintf( tcFileName, L"%s\\%s", m_tcCurPath, strFileName);
	wsprintf( tcOrgName, L"%s", strFileName);


	if ( CheckValidDoc( ptrActiveDoc ) == TRUE	)
	{
		nErrCode = ptrActiveDoc->Doc_SaveCurrentImage( tcFileName, m_tcCurPath, tcOrgName , nRawSave );
		if ( nErrCode != ERR_SUCCESS)
			AfxMessageBox(L"���������� ���� ���� ���߽��ϴ�.");
		else
			AfxMessageBox(L"���������� ����Ǿ����ϴ�");
	}
}

/*======================================================================
ALPR ó�� ���� ��
========================================================================*/
void CALPRDlg::CheckLPR( void )
{
	CTK_ALPRWizardDoc * ptrActiveDoc= NULL;
	ptrActiveDoc = (CTK_ALPRWizardDoc * )GetActiveDocument();

	if ( CheckValidDoc( ptrActiveDoc ) == TRUE	)
	{
		ptrActiveDoc->Doc_init();
	}
}

/*======================================================================
���� ALPR ó�� 
========================================================================*/
void CALPRDlg::AutoALPR( void )
{
	if ( m_AutoIndex.nTotalIndex == 0 )
	{
		SetTotalIndex( );
		NextALPR();
	}
	else
	{
		NextALPR();
	}
}

/*======================================================================
��ü ���� ���� ���� 
========================================================================*/
void CALPRDlg::SetTotalIndex( void )
{
	m_AutoIndex.nTotalIndex = m_CtlFileList.GetItemCount();	
}

/*======================================================================
���õ� ���� LPR ó�� 
========================================================================*/
void CALPRDlg::NextALPR( void )
{
	if ( m_AutoIndex.nTotalIndex > 0 )
	{
		if ( m_AutoIndex.nTotalIndex == m_AutoIndex.nCurIndex )
		{
			KillTimer( TIMER_AUTO );
			m_CtlReadFile.EnableWindow( TRUE );			
			m_CtlAuto.SetCheck( 0 );
			m_bAutoRuning = FALSE;

			AfxMessageBox(L"��� ó���� �������ϴ�.");
			return;
		}

		/* ���õ� ���� ó�� */
		FileListClickEvt( m_AutoIndex.nCurIndex );
		m_AutoIndex.nCurIndex++;

	}
}

/*======================================================================
OCR ���� ����
nPos		: OCR ��ġ
nOCRCode	: OCR �ڵ�
========================================================================*/
int CALPRDlg::SaveOCRFeature( int nPos, int nOCRCode )
{
	CTK_ALPRWizardDoc * ptrActiveDoc= NULL;
	ptrActiveDoc = (CTK_ALPRWizardDoc * )GetActiveDocument();
	
	if ( CheckValidDoc( ptrActiveDoc ) == TRUE	)
	{
		return ptrActiveDoc->Doc_SaveOCRFeature( nPos, nOCRCode );
	}

	return ERR_INVALID_DATA;
}

int CALPRDlg::BackupOCRFeature( void )
{
	CTK_ALPRWizardDoc * ptrActiveDoc= NULL;
	ptrActiveDoc = (CTK_ALPRWizardDoc * )GetActiveDocument();

	if ( CheckValidDoc( ptrActiveDoc ) == TRUE	)
	{
		return ptrActiveDoc->Doc_BackupOCRFeature( );
	}

	return ERR_INVALID_DATA;
}

/*======================================================================
�Ű�� �н�
========================================================================*/
int CALPRDlg::RestudyBPNN( void )
{
	int nResult;
	CString strMsg;

	CTK_ALPRWizardDoc * ptrActiveDoc= NULL;
	ptrActiveDoc = (CTK_ALPRWizardDoc * )GetActiveDocument();

	
	if ( CheckValidDoc( ptrActiveDoc ) == TRUE	)
	{
		nResult= ptrActiveDoc->Doc_RestudyBPNN();
		if ( nResult == ERR_SUCCESS )
		{
			strMsg.Format(L"�Ű�� �н��� ��� �̷�� �����ϴ�");
		}
		else
		{
			strMsg.Format(L"�Ű�� �н��� ���������� ó�� ���� ���߽��ϴ�");
		}
	}
	
	AfxMessageBox( strMsg );
	m_CtlPlateNumber.SetWindowText(L"");

	return ERR_INVALID_DATA;
}

/*======================================================================
Ư¡�� ��ȸ
========================================================================*/
int CALPRDlg::GetFeatureCnt( void )
{
	CTK_ALPRWizardDoc * ptrActiveDoc= NULL;
	ptrActiveDoc = (CTK_ALPRWizardDoc * )GetActiveDocument();

	if ( CheckValidDoc( ptrActiveDoc ) == TRUE	)
	{
		return ptrActiveDoc->Doc_GetFeatureCnt();
	}

	return  -1;
}

/*======================================================================
Ư¡�� ��ȸ
========================================================================*/
int CALPRDlg::NNForceSave( void )
{
	CTK_ALPRWizardDoc * ptrActiveDoc= NULL;
	ptrActiveDoc = (CTK_ALPRWizardDoc * )GetActiveDocument();

	if ( CheckValidDoc( ptrActiveDoc ) == TRUE	)
	{
		return ptrActiveDoc->Doc_NNForceSave();
	}

	return  -1;
}

/*======================================================================
�Ű�� �н� ������
========================================================================*/
DWORD CALPRDlg::OCRStudy_Thread( LPVOID Param )
{
	CALPRDlg *pThis = ( CALPRDlg * )Param;

	pThis->RestudyBPNN();

	if (pThis->m_hTh != NULL )
	{	
		CloseHandle(pThis->m_hTh);
		pThis->m_hTh = NULL;
		pThis->m_dwTh = 0;
	}

	return 0L;
}


/*======================================================================
���� ���� �б�
========================================================================*/
void CALPRDlg::ReloadConf( void)
{
	CTK_ALPRWizardDoc * ptrActiveDoc= NULL;
	ptrActiveDoc = (CTK_ALPRWizardDoc * )GetActiveDocument();

	if ( CheckValidDoc( ptrActiveDoc ) == TRUE	)
	{
		ptrActiveDoc->Doc_ReloadConf();
		GetImagePath();
	}
}

/* ================================================================================
Wizard Function
===================================================================================*/
void CALPRDlg::OnTimer(UINT_PTR nIDEvent)
{
	static BOOL bRun = FALSE;

	if ( nIDEvent == TIMER_AUTO )
	{	
		if ( bRun == FALSE )
		{
			bRun = TRUE;
			/* ALPR ���� ó�� */
			AutoALPR();
			bRun = FALSE;
		}		
	}
	else if( nIDEvent == TIMER_DLG_MAX_SIZE)
	{
		KillTimer( TIMER_DLG_MAX_SIZE );
		m_CtlCheckCreateWindow.SetCheck( FALSE );
		/* Dlg �ִ�ũ�� */
		SetCreateWindow( FALSE );
	}
	else if ( nIDEvent == TIMER_KEYSELECT )
	{
		KillTimer( TIMER_KEYSELECT );
		/* ���õ� ���� ����Ʈ ó�� */
		KeySelectFileList();
	}

	CDialog::OnTimer(nIDEvent);
}

/* ================================================================================
��� Ȱ��ȭ View ����
===================================================================================*/
void CALPRDlg::OnBnClickedBtnAllClose()
{
	CTK_ALPRWizardDoc * ptrFirstDoc= NULL;
	ptrFirstDoc = (CTK_ALPRWizardDoc * )GetFirstDocument();

	if ( ptrFirstDoc == NULL )
	{
		AfxMessageBox(IDS_ERR_NO_IMAGE );
		return ;
	}
	ptrFirstDoc->Doc_AllViewClose();
}
/* ================================================================================
Image Directory Display
===================================================================================*/
void CALPRDlg::GetImagePath()
{
	CTK_ALPRWizardDoc * ptrFirstDoc= NULL;
	ptrFirstDoc = (CTK_ALPRWizardDoc * )GetFirstDocument();
	char * ptrPath= NULL;
	//TCHAR tcPath[ MAX_PATH ];
	CString str;

	if ( ptrFirstDoc == NULL )
	{
		AfxMessageBox(IDS_ERR_NO_IMAGE );
		return ;
	}

	ptrPath = ptrFirstDoc->Doc_GetImagePath();
	if( ptrPath != NULL )
	{
		memset( m_tcCurPath, 0, sizeof( m_tcCurPath ));		
		MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED,ptrPath, strlen(ptrPath), m_tcCurPath, 256);
		str.Format(L"%s", m_tcCurPath );
		m_ctl_img_path.SetWindowText( str );
	}
}

/* ================================================================================
Version ���� Display
===================================================================================*/
void CALPRDlg::SetVersion()
{
	CTK_ALPRWizardDoc * ptrFirstDoc= NULL;
	ptrFirstDoc = (CTK_ALPRWizardDoc * )GetFirstDocument();

	CString strAPP, strDIP, strLPR, strOCR;
	pVERSION ptrVer= NULL;

	if ( ptrFirstDoc == NULL )
	{
		AfxMessageBox(IDS_ERR_NO_IMAGE );
		return ;
	}

	ptrVer = ptrFirstDoc->Doc_GetVersion();

	if( ptrVer != NULL )
	{
		strAPP.Format(L"%d.%d", ptrVer->APP_VER[ 0 ] ,ptrVer->APP_VER[ 1 ] );
		strOCR.Format(L"%d.%d", ptrVer->OCR_VER[ 0 ] ,ptrVer->OCR_VER[ 1 ] );
		strDIP.Format(L"%d.%d", ptrVer->DIP_VER[ 0 ] ,ptrVer->DIP_VER[ 1 ] );
		strLPR.Format(L"%d.%d", ptrVer->LPR_VER[ 0 ] ,ptrVer->LPR_VER[ 1 ] );

		m_ctl_APP_VER.SetWindowText( strAPP );
		m_ctl_OCR_VER.SetWindowText( strOCR );
		m_ctl_DIP_VER.SetWindowText( strDIP );
		m_ctl_LPR_VER.SetWindowText( strLPR );
	}
}

int CALPRDlg::IsBaseCharStudy( void )
{
	CTK_ALPRWizardDoc * ptrFirstDoc= NULL;
	ptrFirstDoc = (CTK_ALPRWizardDoc * )GetFirstDocument();
	int isBaseStudy= 0;

	if ( ptrFirstDoc == NULL )
	{
		AfxMessageBox(IDS_ERR_NO_IMAGE );
		return 0;
	}

	memset(m_tcBaseChar, 0, sizeof( m_tcBaseChar ));
	isBaseStudy = ptrFirstDoc->Doc_IsBaseStudy();

	return isBaseStudy;
}
/* ================================================================================
�⺻ �н������� �������� ����
===================================================================================*/
void CALPRDlg::SetBaseLearningChar( CString &strBaseChar )
{
	
	CString strChar;
	int nCharPos,nCharPos2 = 0;	
	int isBaseStudy= 0;

	isBaseStudy = IsBaseCharStudy();

	if( isBaseStudy )
	{
		if ( m_Ctl_Chek_AutoView.GetCheck() )
		{
			nCharPos = strBaseChar.Find(L"_");
			if ( nCharPos > 0 )
			{
				nCharPos2 = strBaseChar.Find(L"_", nCharPos+1);
				strChar = strBaseChar.Mid(nCharPos+1, ( nCharPos2 - nCharPos -1 ));				
				wcscpy_s( m_tcBaseChar, strChar.GetLength() + 1 , strChar );
			}
		}		
	}

}

/* ================================================================================
Camera ����
===================================================================================*/
void CALPRDlg::OnBnClickedBtnCamera()
{
	
}

/* ================================================================================
���� ���� ��û
===================================================================================*/
void CALPRDlg::OnBnClickedButton35()
{
	ReloadConf();
}

/* ================================================================================
���� ���� Dialog Box ���� 
===================================================================================*/
void CALPRDlg::OnBnClickedBtnShowConf()
{
		
	if ( m_bOpenedConfDlg )
		return;

	if( m_ptrConfDlg == NULL)
	{
		m_ptrConfDlg = new CCONFDlg;
		if ( m_ptrConfDlg != NULL )
		{
			m_ptrConfDlg->Create( IDD_CONF_DIALOG );
			m_ptrConfDlg->Set_IALPRDlg( this );
			m_ptrConfDlg->ShowWindow( SW_SHOW );
		}
	}
	else
	{
		m_ptrConfDlg->Create( IDD_CONF_DIALOG );
		m_ptrConfDlg->Set_IALPRDlg( this );
		m_ptrConfDlg->ShowWindow( SW_SHOW );
	}
	
	m_bOpenedConfDlg = TRUE;	
}

/* ================================================================================
DIP Dialog Box ���� 
===================================================================================*/
void CALPRDlg::OnBnClickedBtnShowDip()
{
	if ( m_bOpenedDipDlg )
		return;

	if( m_ptrDipDlg == NULL)
	{
		m_ptrDipDlg = new CDIPDlg;
		if ( m_ptrDipDlg != NULL )
		{
			m_ptrDipDlg->Create( IDD_DIP_DIALOG );
			m_ptrDipDlg->Set_IALPRDlg( this );
			m_ptrDipDlg->ShowWindow( SW_SHOW );
		}
	}
	else
	{
		m_ptrDipDlg->Create( IDD_DIP_DIALOG );
		m_ptrDipDlg->Set_IALPRDlg( this );
		m_ptrDipDlg->ShowWindow( SW_SHOW );
	}

	m_bOpenedDipDlg = TRUE;	

}

/* ================================================================================
���� ���� ó��
===================================================================================*/
void CALPRDlg::KeySelectFileList( void )
{
	int nCnt2;

	nCnt2 = m_CtlFileList.GetItemCount();		
	for ( int i = 0 ; i < nCnt2 ; i++)
	{
		if ( m_CtlFileList.GetItemState(i, LVIS_SELECTED) !=0 )
		{
			FileListClickEvt( i );
		}
	}
}

/* ================================================================================
�̹��� ���� �б�
===================================================================================*/
void CALPRDlg::OnBnClickedBtnFile()
{		
	ClearImageData();	
	memset( &m_FileManager, 0, sizeof( FILE_MANAGER_t));	
	m_WizardHelper.GetFileInfo( m_tcCurPath, &m_FileManager );
	m_ctl_img_path.SetWindowText( m_tcCurPath );

	/* ���� ���� ��� */
	DisplayFileList();
}

/* ================================================================================
���õ� ���� ����Ʈ ó�� 
RAW, BMP, JPEG ������ �а� ȭ�鿡 ��� 
===================================================================================*/
void CALPRDlg::FileListClickEvt( int nListIndex )
{
	CString strFileName;	
	IMAGE_DATA_t ImageData;
	TCHAR tcPath[ MAX_PATH ];
	int nArea = CENTER_AREA;
	
	memset( &ImageData, 0, sizeof( IMAGE_DATA_t ));
	memset( tcPath, 0, sizeof( tcPath ));

	/* File Name ���� */
	strFileName = m_CtlFileList.GetItemText( nListIndex, 1 );
	wsprintf( tcPath, L"%s\\%s", m_tcCurPath, strFileName);	

	strFileName.MakeUpper();


	if (strFileName.Find(L"LC") > 0 )
	{
		nArea = LEFT_AREA;
	}
	else if (strFileName.Find(L"RC") > 0 )
	{
		nArea = RIGHT_AREA;
	}	
	
	SetBaseLearningChar( strFileName );

	
	/* RAW �̹��� ��� */
	if ( strFileName.Right( 4 ) == L".RAW" )
	{
		/* RAW Image�� H, W, bitCnt ������ ���� ������ */
		ImageData.nBitCount = RAW_8_BITCNT;
		//ImageData.nH = RAW_H_120;
		//ImageData.nW = RAW_W_720;

		if ( m_WizardHelper.ReadRaw( tcPath, &ImageData ) == ERR_SUCCESS )
			DisplayImageData(strFileName, &ImageData, nArea );
		else
			AfxMessageBox(L"TK�� RAW FILE ������ �ƴҼ� �ֽ��ϴ�.");
	}
	/* BMP �̹��� ��� */
	else if ( strFileName.Right( 4 ) == L".BMP")
	{		
		m_WizardHelper.ReadBmp( tcPath , &ImageData);
		DisplayImageData( strFileName, &ImageData, nArea );
	}
	/* JPG �̹��� ��� */
	else if ( strFileName.Right( 4 ) == L".JPG")
	{		
		m_WizardHelper.ReadJpeg( tcPath, &ImageData );
		DisplayImageData( strFileName, &ImageData, nArea );
	}
	else
	{
		AfxMessageBox(IDS_ERR_INVALID_FILE);
	}
	
	/*Read�� ���� ������ ���� �޸𸮸� �ݵ�� ���� �� */
	m_WizardHelper.FreeImageAllocData( &ImageData );
}

/* ================================================================================
���� ����Ʈ Ŭ�� �� ( ���콺 Ŭ�� )
===================================================================================*/
void CALPRDlg::OnNMClickFileList(NMHDR *pNMHDR, LRESULT *pResult)
{	
	int nCnt2, nCnt =0;
	
	NM_LISTVIEW* pNMListView =(NM_LISTVIEW*)pNMHDR;
	nCnt = pNMListView->iItem;
	
	if (nCnt >= 0)
	{
		nCnt2 = m_CtlFileList.GetItemCount();		
		for ( int i = 0 ; i < nCnt2 ; i++)
		{
			if ( m_CtlFileList.GetItemState(i, LVIS_SELECTED) !=0 )
			{
				FileListClickEvt( i );
			}
		}
	}	
	*pResult = 0;
}

/* ================================================================================
���� ����Ʈ ���� �� ( Ű���� Ŭ�� )
===================================================================================*/
void CALPRDlg::OnLvnKeydownFileList(NMHDR *pNMHDR, LRESULT *pResult)
{
	
	LPNMLVKEYDOWN pLVKeyDow = reinterpret_cast<LPNMLVKEYDOWN>(pNMHDR);	
	
	SetTimer( TIMER_KEYSELECT, 50, NULL );
	*pResult = 0;
}

void CALPRDlg::OnBnClickedBtImageSave()
{
	SaveCurrentImage();
}

/* ================================================================================
ALPR ���� / ���� 

���� �� : ���õ� ���Ͽ� ���� LPR ó���� �����Ѵ�. 
���� �� : ���õ� ���Ͽ� ���� �̹����� �����ش�. 
===================================================================================*/
void CALPRDlg::OnBnClickedChkAutoView()
{
	CheckLPR();
}


/* ================================================================================
Auto Check ���� / ���� 

���� �� : ���ϸ���Ʈ�� �ִ� �׸� ������ �ڵ����� LPR ó����
���� �� : �ڵ� LPR ó�� ���� 
===================================================================================*/
void CALPRDlg::OnBnClickedCheck1()
{
	if ( m_CtlAuto.GetCheck() == 1 )
	{
		m_CtlReadFile.EnableWindow( FALSE );		
		memset(&m_AutoIndex, 0, sizeof( AUTO_INDEX_t ));
		if ( IsBaseCharStudy() )
		{
			SetTimer( TIMER_AUTO, 1000 , NULL );
		}
		else
		{
			SetTimer( TIMER_AUTO, TIMER_AUTO_INTER, NULL );
		}
		
		m_bAutoRuning = TRUE;
	}
	else
	{
		m_CtlReadFile.EnableWindow( TRUE );		
		KillTimer( TIMER_AUTO );
		m_bAutoRuning = FALSE;
	}
}

/* ================================================================================
LOG list clear 
OCR ���� �ʱ�ȭ 
===================================================================================*/
void CALPRDlg::OnBnClickedBtnClear()
{
	//m_CtlLog_List.DeleteAllItems();
	m_CtlPlateNumber.SetWindowText(L"");
	m_Ctl_StudyCnt.SetWindowText( L"0" );
	m_Ctl_Err_Rate.SetWindowText( L"0.0" );

	Init_OCR();

	CTK_ALPRWizardDoc * ptrActiveDoc= NULL;
	ptrActiveDoc = (CTK_ALPRWizardDoc * )GetActiveDocument();

	if ( CheckValidDoc( ptrActiveDoc ) == TRUE	)
	{
		ptrActiveDoc->Doc_InitOCRData();
	}

}
/* ================================================================================
Multi Document Ȱ��ȭ ���� ó�� 
===================================================================================*/
void CALPRDlg::OnBnClickedCheck2()
{

#if 1
	if (m_CtlCheckCreateWindow.GetCheck() == 1 )
		SetCreateWindow( TRUE );
	else
	{		
		SetCreateWindow( FALSE);
	}
#endif

}

void CALPRDlg::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	
}


HBRUSH CALPRDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

	/* ���� ��ȣ�� Control�� ��� �Ķ��� */
	if ( pWnd->GetDlgCtrlID() == IDC_PLATE_NUMBER )
	{
		pDC->SetTextColor(RGB(0, 0, 0xff));
	}

	return hbr;
}

/* ================================================================================
OCR 1��° ��ġ ������ ���� 
===================================================================================*/
void CALPRDlg::OnBnClickedButton25()
{
	int nOCRCode = 0 ;
	int nPos = 0;

	nOCRCode = m_OCR_Ctrl.ptrOCRCombo[ nPos ]->GetCurSel();
	SaveOCRFeature( nPos, nOCRCode  );
	AfxMessageBox(L"�н������� ������ �Ϸ� �Ǿ����ϴ�.");

}

/* ================================================================================
OCR 2��° ��ġ ������ ���� 
===================================================================================*/
void CALPRDlg::OnBnClickedButton26()
{
	int nOCRCode = 0 ;
	int nPos = 1;

	nOCRCode = m_OCR_Ctrl.ptrOCRCombo[ nPos ]->GetCurSel();
	SaveOCRFeature( nPos, nOCRCode );
	AfxMessageBox(L"�н������� ������ �Ϸ� �Ǿ����ϴ�.");
}

/* ================================================================================
OCR 3��° ��ġ ������ ���� 
===================================================================================*/
void CALPRDlg::OnBnClickedButton27()
{
	int nOCRCode = 0 ;
	int nPos = 2;

	nOCRCode = m_OCR_Ctrl.ptrOCRCombo[ nPos ]->GetCurSel();
	SaveOCRFeature( nPos, nOCRCode );
	AfxMessageBox(L"�н������� ������ �Ϸ� �Ǿ����ϴ�.");
}

/* ================================================================================
OCR 4��° ��ġ ������ ���� 
===================================================================================*/
void CALPRDlg::OnBnClickedButton28()
{
	int nOCRCode = 0 ;
	int nPos = 3;

	nOCRCode = m_OCR_Ctrl.ptrOCRCombo[ nPos ]->GetCurSel();
	SaveOCRFeature( nPos, nOCRCode );
	AfxMessageBox(L"�н������� ������ �Ϸ� �Ǿ����ϴ�.");
}

/* ================================================================================
OCR 5��° ��ġ ������ ���� 
===================================================================================*/
void CALPRDlg::OnBnClickedButton29()
{
	int nOCRCode = 0 ;
	int nPos = 4;

	nOCRCode = m_OCR_Ctrl.ptrOCRCombo[ nPos ]->GetCurSel();
	SaveOCRFeature( nPos, nOCRCode );
	AfxMessageBox(L"�н������� ������ �Ϸ� �Ǿ����ϴ�.");
}

/* ================================================================================
OCR 6��° ��ġ ������ ���� 
===================================================================================*/
void CALPRDlg::OnBnClickedButton30()
{
	int nOCRCode = 0 ;
	int nPos = 5;

	nOCRCode = m_OCR_Ctrl.ptrOCRCombo[ nPos ]->GetCurSel();
	SaveOCRFeature( nPos, nOCRCode );
	AfxMessageBox(L"�н������� ������ �Ϸ� �Ǿ����ϴ�.");
}

/* ================================================================================
OCR 7��° ��ġ ������ ���� 
===================================================================================*/
void CALPRDlg::OnBnClickedButton31()
{
	int nOCRCode = 0 ;
	int nPos = 6;

	nOCRCode = m_OCR_Ctrl.ptrOCRCombo[ nPos ]->GetCurSel();
	SaveOCRFeature( nPos, nOCRCode );
	AfxMessageBox(L"�н������� ������ �Ϸ� �Ǿ����ϴ�.");
}

/* ================================================================================
OCR 8��° ��ġ ������ ���� 
===================================================================================*/
void CALPRDlg::OnBnClickedButton32()
{
	int nOCRCode = 0 ;
	int nPos = 7;

	nOCRCode = m_OCR_Ctrl.ptrOCRCombo[ nPos ]->GetCurSel();
	SaveOCRFeature( nPos, nOCRCode );
	AfxMessageBox(L"�н������� ������ �Ϸ� �Ǿ����ϴ�.");
}

/* ================================================================================
OCR 9��° ��ġ ������ ���� 
===================================================================================*/
void CALPRDlg::OnBnClickedButton33()
{
	int nOCRCode = 0 ;
	int nPos = 8;

	nOCRCode = m_OCR_Ctrl.ptrOCRCombo[ nPos ]->GetCurSel();
	SaveOCRFeature( nPos, nOCRCode );
	AfxMessageBox(L"�н������� ������ �Ϸ� �Ǿ����ϴ�.");
}

/* ================================================================================
OCR 10��° ��ġ ������ ���� 
===================================================================================*/
void CALPRDlg::OnBnClickedButton34()
{
	int nOCRCode = 0 ;
	int nPos = 9;

	nOCRCode = m_OCR_Ctrl.ptrOCRCombo[ nPos ]->GetCurSel();
	SaveOCRFeature( nPos, nOCRCode );
	AfxMessageBox(L"�н������� ������ �Ϸ� �Ǿ����ϴ�.");
}
/* ================================================================================
OCR 11��° ��ġ ������ ���� 
===================================================================================*/
void CALPRDlg::OnBnClickedButton39()
{
	int nOCRCode = 0 ;
	int nPos = 10;

	nOCRCode = m_OCR_Ctrl.ptrOCRCombo[ nPos ]->GetCurSel();
	SaveOCRFeature( nPos, nOCRCode );
	AfxMessageBox(L"�н������� ������ �Ϸ� �Ǿ����ϴ�.");
}
/* ================================================================================
OCR 12��° ��ġ ������ ���� 
===================================================================================*/
void CALPRDlg::OnBnClickedButton40()
{
	int nOCRCode = 0 ;
	int nPos = 11;

	nOCRCode = m_OCR_Ctrl.ptrOCRCombo[ nPos ]->GetCurSel();
	SaveOCRFeature( nPos, nOCRCode );
	AfxMessageBox(L"�н������� ������ �Ϸ� �Ǿ����ϴ�.");
}

/* ================================================================================
OCR 13��° ��ġ ������ ���� 
===================================================================================*/
void CALPRDlg::OnBnClickedButton41()
{
	int nOCRCode = 0 ;
	int nPos = 12;

	nOCRCode = m_OCR_Ctrl.ptrOCRCombo[ nPos ]->GetCurSel();
	SaveOCRFeature( nPos, nOCRCode );
	AfxMessageBox(L"�н������� ������ �Ϸ� �Ǿ����ϴ�.");
}
/* ================================================================================
OCR 14��° ��ġ ������ ���� 
===================================================================================*/
void CALPRDlg::OnBnClickedButton42()
{
	int nOCRCode = 0 ;
	int nPos = 13;

	nOCRCode = m_OCR_Ctrl.ptrOCRCombo[ nPos ]->GetCurSel();
	SaveOCRFeature( nPos, nOCRCode );
	AfxMessageBox(L"�н������� ������ �Ϸ� �Ǿ����ϴ�.");
}

/* ================================================================================
OCR 15��° ��ġ ������ ���� 
===================================================================================*/
void CALPRDlg::OnBnClickedButton43()
{
	int nOCRCode = 0 ;
	int nPos = 14;

	nOCRCode = m_OCR_Ctrl.ptrOCRCombo[ nPos ]->GetCurSel();
	SaveOCRFeature( nPos, nOCRCode );
	AfxMessageBox(L"�н������� ������ �Ϸ� �Ǿ����ϴ�.");
}
/* ================================================================================
OCR 16��° ��ġ ������ ���� 
===================================================================================*/
void CALPRDlg::OnBnClickedButton44()
{
	int nOCRCode = 0 ;
	int nPos = 15;

	nOCRCode = m_OCR_Ctrl.ptrOCRCombo[ nPos ]->GetCurSel();
	SaveOCRFeature( nPos, nOCRCode );
	AfxMessageBox(L"�н������� ������ �Ϸ� �Ǿ����ϴ�.");
}



/* ================================================================================
OCR 1~10��° ��ġ �� ��ȿ�� ������ ���� 
===================================================================================*/
void CALPRDlg::OnBnClickedButton24()
{
	int i=0;
	int nOCRCode = 0 ;
	int nResult = ERR_SUCCESS;
	int nValid = 0;
	CString strMsg;

#if 0
	if (m_chk_same_ocr.GetCheck() == 1 )
	{
		//SetSameOCR();
		m_bAutoFeatureSave =1;
	}
	else
	{
		m_bAutoFeatureSave =0;
	}
#endif

	for( i = 0 ; i < MAX_OCR_CTRL_CNT ; i++ )
	{
		if ( m_OCR_Ctrl.ptrOCRButton[ i ]->IsWindowVisible() && m_OCR_Ctrl.ptrOCRCombo[ i ]->GetCurSel() != 0  )
		{
			nOCRCode = m_OCR_Ctrl.ptrOCRCombo[ i ]->GetCurSel();
			nResult = SaveOCRFeature( i, nOCRCode );
			if ( nResult != ERR_SUCCESS )
			{
				strMsg.Format(L"ERR::�н������͸� ���������� ���� ���� ���߽��ϴ�.");
				if (m_bAutoFeatureSave ==0 )
				{
					AfxMessageBox(strMsg );
				}
				
				else
				{
					m_CtlPlateNumber.SetWindowText( strMsg );
				}
				break;
			}
			nValid = 1;
		}
	}
	
	if (  nValid == 1 && nResult == ERR_SUCCESS )
	{
		strMsg.Format(L"�н������� ������ �Ϸ� �Ǿ����ϴ�.");
		if (m_bAutoFeatureSave ==0 )
		{
			AfxMessageBox(strMsg);
		}
		else
		{
			m_CtlPlateNumber.SetWindowText( strMsg );
		}		
	}

	if (m_bAutoFeatureSave )
	{
		//Sleep( 10 );
		//m_CtlPlateNumber.SetWindowText( L"" );
	}
}

/* ================================================================================
���� Ư¡�� ������ ����Ѵ�.  
===================================================================================*/
void CALPRDlg::OnBnClickedBtnBackup()
{
	int nResult ;
	CString strMsg;

	nResult = BackupOCRFeature();

	if ( nResult == 1 )
	{
		strMsg.Format(L"���������� ����Ͽ����ϴ�.");
	}
	else
	{
		strMsg.Format(L"ERR::���������� ������� ���߽��ϴ�.");
	}

	AfxMessageBox( strMsg );
}

/* ================================================================================
�Ű�� �н� ���� 
===================================================================================*/
void CALPRDlg::OnBnClickedBtnstudy()
{
	/* �Ű�� �н��� �ð��� ���� �ɸ��⶧���� Thread�� ����Ѵ� */
	m_Ctl_StudyCnt.SetWindowText( L"0" );
	m_Ctl_Err_Rate.SetWindowText( L"0.0" );
	
	if ( m_hTh == NULL )
	{
		m_hTh = CreateThread(0, 0, (LPTHREAD_START_ROUTINE)OCRStudy_Thread, this,	0,&m_dwTh);
	}	
}

/* ================================================================================
Ư¡�� ���� ��ȸ
===================================================================================*/
void CALPRDlg::OnBnClickedBtnfeature()
{
	int nCnt;
	CString strCnt;

	m_Ctl_FeatureCnt.SetWindowText( L"0" );
	
	nCnt = GetFeatureCnt();
	if( nCnt < 0)
	{
		AfxMessageBox(L"Ư¡�� ������ ���� �� �����ϴ�.");
	}
	else
	{
		strCnt.Format(L"%d", nCnt);
		m_Ctl_FeatureCnt.SetWindowText( strCnt );
	}
}

/* ================================================================================
�н� ������ ���� ����
===================================================================================*/
void CALPRDlg::OnBnClickedBtnNnSave()
{
	int nResult = ERR_SUCCESS;
	nResult= NNForceSave();

	if( nResult != ERR_SUCCESS)
	{
		AfxMessageBox(L"������ �Ű�� �н� ���°� �ƴմϴ�.");
	}	
}

/* ================================================================================
Ư¡�� ��ü ���� 
===================================================================================*/
void CALPRDlg::OnBnClickedCheck3()
{
	int i;
	BOOL bEnable;
	if ( m_chk_all_save.GetCheck()==1 )
	{
		bEnable = FALSE;
	}
	else
	{
		bEnable = TRUE;
	}

	for( i = 0; i < MAX_OCR_CTRL_CNT ; i++ )
	{			
		m_OCR_Ctrl.ptrOCRButtonWnd[ i ]->EnableWindow( bEnable );
	}
}
/* ================================================================================
Ư¡�� �ڵ� ���� 
===================================================================================*/
void CALPRDlg::OnBnClickedSameOCR()
{
	
	if( m_chk_same_ocr.GetCheck() )
	{
		m_bAutoFeatureSave = 1;
	}
	else
	{
		m_bAutoFeatureSave = 0;
	}
}

/* ================================================================================
Ư¡�� �����ϰ� 
===================================================================================*/
void CALPRDlg::SetSameOCR()
{
	int nOCRCode, i,nResult;

	nOCRCode = m_OCR_Ctrl.ptrOCRCombo[ 0 ]->GetCurSel();

	for( i = 0 ; i < MAX_OCR_CTRL_CNT ; i++ )
	{
		if ( m_OCR_Ctrl.ptrOCRButton[ i ]->IsWindowVisible() )
		{
			//if( i == 1 || i == 2 )
			{
				m_OCR_Ctrl.ptrOCRCombo[ i ]->SetCurSel( nOCRCode );			
			}			
		}
	}
}


void CALPRDlg::OnClose()
{
	__super::OnClose();
}

void CALPRDlg::OnEnChangeEImageBitcnt()
{
	// TODO:  RICHEDIT ��Ʈ���� ���, �� ��Ʈ����
	// __super::OnInitDialog() �Լ��� �������ϰ�  ����ũ�� OR �����Ͽ� ������
	// ENM_CHANGE �÷��׸� �����Ͽ� CRichEditCtrl().SetEventMask()�� ȣ���ؾ߸�
	// �ش� �˸� �޽����� �����ϴ�.

	// TODO:  ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
}
