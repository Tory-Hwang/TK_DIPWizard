// DIPDlg.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "TK_ALPRWizard.h"
#include "TK_ALPRWizardDoc.h"
#include "DIPDlg.h"


#define DIP_INIT_TIMER	3002
// CDIPDlg 대화 상자입니다.

IMPLEMENT_DYNAMIC(CDIPDlg, CDialog)

CDIPDlg::CDIPDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CDIPDlg::IDD, pParent)
	, m_ctl_binary_th(0)
	, m_ctl_bright(0)
	, m_ctl_itg_size(0)
	, m_ctrl_d_rotation(0)
	, m_ctrl_DoGSig1(0)
	, m_ctrl_DoGSig2(0)
	, m_ctl_unsharp_mask(1)
{
	m_ptrALPRDlg = NULL;
}

CDIPDlg::~CDIPDlg()
{
}

void CDIPDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Text(pDX, IDC_EDIT3, m_ctl_binary_th);
	DDX_Text(pDX, IDC_EDIT4, m_ctl_bright);
	DDX_Text(pDX, IDC_EDIT19, m_ctl_itg_size);
	DDX_Text(pDX, IDC_EDIT24,  m_ctrl_d_rotation);
	DDX_Text(pDX, IDC_EDIT25,  m_ctrl_DoGSig1);
	DDX_Text(pDX, IDC_EDIT26,  m_ctrl_DoGSig2);
	DDX_Text(pDX, IDC_EDIT6,  m_ctl_unsharp_mask);

}


BEGIN_MESSAGE_MAP(CDIPDlg, CDialog)
	ON_BN_CLICKED(IDC_BUTTON1, &CDIPDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON7, &CDIPDlg::OnBnClickedButton7)
	ON_BN_CLICKED(IDC_BUTTON49, &CDIPDlg::OnBnClickedButton49)
	ON_BN_CLICKED(IDC_BUTTON11, &CDIPDlg::OnBnClickedButton11)
	ON_BN_CLICKED(IDC_BUTTON6, &CDIPDlg::OnBnClickedButton6)
	ON_BN_CLICKED(IDC_BUTTON15, &CDIPDlg::OnBnClickedButton15)
	ON_BN_CLICKED(IDC_BUTTON36, &CDIPDlg::OnBnClickedButton36)
	ON_BN_CLICKED(IDC_BUTTON3, &CDIPDlg::OnBnClickedButton3)
	ON_BN_CLICKED(IDC_BUTTON5, &CDIPDlg::OnBnClickedButton5)
	ON_BN_CLICKED(IDC_BUTTON9, &CDIPDlg::OnBnClickedButton9)
	ON_BN_CLICKED(IDC_BUTTON18, &CDIPDlg::OnBnClickedButton18)
	ON_BN_CLICKED(IDC_BUTTON12, &CDIPDlg::OnBnClickedButton12)
	ON_BN_CLICKED(IDC_BUTTON20, &CDIPDlg::OnBnClickedButton20)
	ON_BN_CLICKED(IDC_BUTTON2, &CDIPDlg::OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BUTTON22, &CDIPDlg::OnBnClickedButton22)
	ON_BN_CLICKED(IDC_BUTTON23, &CDIPDlg::OnBnClickedButton23)
	ON_BN_CLICKED(IDC_BUTTON16, &CDIPDlg::OnBnClickedButton16)
	ON_BN_CLICKED(IDC_BUTTON10, &CDIPDlg::OnBnClickedButton10)
	ON_BN_CLICKED(IDC_BUTTON19, &CDIPDlg::OnBnClickedButton19)
	ON_BN_CLICKED(IDC_BUTTON37, &CDIPDlg::OnBnClickedButton37)
	ON_BN_CLICKED(IDC_BUTTON21, &CDIPDlg::OnBnClickedButton21)
	ON_BN_CLICKED(IDC_BUTTON17, &CDIPDlg::OnBnClickedButton17)
	ON_BN_CLICKED(IDC_BUTTON45, &CDIPDlg::OnBnClickedButton45)
	ON_BN_CLICKED(IDC_BUTTON48, &CDIPDlg::OnBnClickedButton48)
	ON_BN_CLICKED(IDC_BUTTON46, &CDIPDlg::OnBnClickedButton46)
	ON_BN_CLICKED(IDC_BUTTON4, &CDIPDlg::OnBnClickedButton4)
	ON_BN_CLICKED(IDC_BUTTON8, &CDIPDlg::OnBnClickedButton8)
	ON_BN_CLICKED(IDC_BUTTON38, &CDIPDlg::OnBnClickedButton38)
	ON_BN_CLICKED(IDC_BUTTON47, &CDIPDlg::OnBnClickedButton47)
	ON_WM_CREATE()
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BUTTON50, &CDIPDlg::OnBnClickedButton50)
	ON_BN_CLICKED(IDC_BUTTON13, &CDIPDlg::OnBnClickedButton13)
	ON_BN_CLICKED(IDC_BUTTON51, &CDIPDlg::OnBnClickedButton51)
	ON_BN_CLICKED(IDC_BUTTON52, &CDIPDlg::OnBnClickedButton52)
END_MESSAGE_MAP()

int CDIPDlg::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;

	SetTimer( DIP_INIT_TIMER, 200, NULL);

	return 0;
}



void CDIPDlg::OnTimer(UINT_PTR nIDEvent)
{
	if( nIDEvent == DIP_INIT_TIMER )
	{
		KillTimer(DIP_INIT_TIMER);
		init_DIP_Dlg();
	}

	CDialog::OnTimer(nIDEvent);
}


void CDIPDlg::init_DIP_Dlg( void )
{
	m_ctl_binary_th = DEFAULT_BIN_THRESOLD;
	m_ctl_bright = DEFAULT_BRIGHTNESS;
	m_ctl_itg_size = DEFAULT_ITG_SIZE;
	m_ctrl_d_rotation = 5;

	m_ctrl_DoGSig1 = 1;
	m_ctrl_DoGSig2 = 3;
	m_ctl_unsharp_mask = 1;

	UpdateData( FALSE );

}

// CCONFDlg 메시지 처리기입니다.
void  CDIPDlg::Set_IALPRDlg( CIBridge * ptrInterface )
{
	if( m_ptrALPRDlg == NULL )
		m_ptrALPRDlg = ptrInterface;

}

/*======================================================================
첫번째 Document Interface 요청 
========================================================================*/
CDocument * CDIPDlg::GetFirstDocument( void )
{
	CDocument* pDT = NULL;	
	CTK_ALPRWizardApp* pApp = (CTK_ALPRWizardApp*)AfxGetApp();	

	pDT = pApp->GetFirstDoc();
	return pDT;
}

/*======================================================================
활성화 된 Document Interface 요청 
========================================================================*/
CDocument * CDIPDlg::GetActiveDocument( void )
{	
	CDocument* pDT = NULL;	
	CTK_ALPRWizardApp* pApp = (CTK_ALPRWizardApp*)AfxGetApp();	

	pDT = pApp->GetFirstDoc();

	return pDT;
}

/*======================================================================
Document Interface 정상유무 체크 
========================================================================*/
BOOL CDIPDlg::CheckValidDoc( void * ptrDoc )
{
	if ( ptrDoc == NULL )
	{
		AfxMessageBox(IDS_ERR_NO_IMAGE );
		return FALSE;
	}
	return TRUE;
}

/*======================================================================
gray scale 이미지 변환 
========================================================================*/
void CDIPDlg::ConvertGrayscale( void )
{
	CTK_ALPRWizardDoc * ptrActiveDoc= NULL;
	ptrActiveDoc = (CTK_ALPRWizardDoc * )GetActiveDocument();

	if ( CheckValidDoc( ptrActiveDoc ) == TRUE	)
	{
		ptrActiveDoc->Doc_ConvertGrayscale();
	}
}

/*======================================================================
Histogram 출력 
========================================================================*/
void CDIPDlg::Histogram( void )
{

}

/*======================================================================
Histogram 정규화 변환 
========================================================================*/
void CDIPDlg::Histogram_Equal( void )
{
	CTK_ALPRWizardDoc * ptrActiveDoc= NULL;
	ptrActiveDoc = (CTK_ALPRWizardDoc * )GetActiveDocument();

	if ( CheckValidDoc( ptrActiveDoc ) == TRUE	)
	{
		ptrActiveDoc->Doc_Histogram_Equal();
	}
}

/*======================================================================
영상 이진화 변환 
========================================================================*/
void CDIPDlg::Binarization( void )
{
	UpdateData(TRUE);


	CTK_ALPRWizardDoc * ptrActiveDoc= NULL;
	ptrActiveDoc = (CTK_ALPRWizardDoc * )GetActiveDocument();

	if ( CheckValidDoc( ptrActiveDoc ) == TRUE	)
	{
		ptrActiveDoc->Doc_Binarization( m_ctl_binary_th );
	}
}

/*======================================================================
반복에 의한 영상 이진화 변환 
========================================================================*/
void CDIPDlg::Binarization_Iterate( void )
{
	CTK_ALPRWizardDoc * ptrActiveDoc= NULL;
	ptrActiveDoc = (CTK_ALPRWizardDoc * )GetActiveDocument();

	if ( CheckValidDoc( ptrActiveDoc ) == TRUE	)
	{
		ptrActiveDoc->Doc_Binarization_Iterate();
	}
}

/*======================================================================
Adaptive 영상 이진화 변환 
========================================================================*/
void CDIPDlg::Binarization_Adaptive( void )
{
	CTK_ALPRWizardDoc * ptrActiveDoc= NULL;
	ptrActiveDoc = (CTK_ALPRWizardDoc * )GetActiveDocument();

	if ( CheckValidDoc( ptrActiveDoc ) == TRUE	)
	{
		ptrActiveDoc->Doc_Binarization_Adaptive();
	}
}

/*======================================================================
Integral Adaptive 영상 이진화 변환 
========================================================================*/
void CDIPDlg::Binarization_ITG_Adaptive( void )
{
	UpdateData(TRUE);


	CTK_ALPRWizardDoc * ptrActiveDoc= NULL;
	ptrActiveDoc = (CTK_ALPRWizardDoc * )GetActiveDocument();

	if ( CheckValidDoc( ptrActiveDoc ) == TRUE	)
	{
		ptrActiveDoc->Doc_Binarization_ITG_Adaptive( m_ctl_itg_size );
	}
}


/*======================================================================
OTSU  영상 이진화 변환 
========================================================================*/
void CDIPDlg::Binarization_Otsu( void )
{
	CTK_ALPRWizardDoc * ptrActiveDoc= NULL;
	ptrActiveDoc = (CTK_ALPRWizardDoc * )GetActiveDocument();

	if ( CheckValidDoc( ptrActiveDoc ) == TRUE	)
	{
		ptrActiveDoc->Doc_Binarization_Otsu();
	}
}

/*======================================================================
영상 밝기 변환 
========================================================================*/
void CDIPDlg::Brightness( void )
{
	UpdateData(TRUE);


	CTK_ALPRWizardDoc * ptrActiveDoc= NULL;
	ptrActiveDoc = (CTK_ALPRWizardDoc * )GetActiveDocument();

	if ( CheckValidDoc( ptrActiveDoc ) == TRUE	)
	{
		ptrActiveDoc->Doc_Brightness( m_ctl_bright );
	}
}

/*======================================================================
영상 평활화 
========================================================================*/
void CDIPDlg::BitPlane( void )
{
	CTK_ALPRWizardDoc * ptrActiveDoc= NULL;
	ptrActiveDoc = (CTK_ALPRWizardDoc * )GetActiveDocument();

	if ( CheckValidDoc( ptrActiveDoc ) == TRUE	)
	{
		ptrActiveDoc->Doc_BitPlane();
	}
}

/*======================================================================
Gaussian 변환( 잡음 제거, 영상의 스무딩 기법 )
========================================================================*/
void CDIPDlg::Gaussian( void )
{
	CTK_ALPRWizardDoc * ptrActiveDoc= NULL;
	ptrActiveDoc = (CTK_ALPRWizardDoc * )GetActiveDocument();

	if ( CheckValidDoc( ptrActiveDoc ) == TRUE	)
	{
		ptrActiveDoc->Doc_Gaussian();
	}
}

/*======================================================================
Sobel Edge
========================================================================*/
void CDIPDlg::SobelEdge( void )
{
	CTK_ALPRWizardDoc * ptrActiveDoc= NULL;
	ptrActiveDoc = (CTK_ALPRWizardDoc * )GetActiveDocument();

	if ( CheckValidDoc( ptrActiveDoc ) == TRUE	)
	{
		ptrActiveDoc->Doc_Sobel();
	}
}

/*======================================================================
Prewitt Edge
========================================================================*/
void CDIPDlg::Prewitt( void )
{
	CTK_ALPRWizardDoc * ptrActiveDoc= NULL;
	ptrActiveDoc = (CTK_ALPRWizardDoc * )GetActiveDocument();

	if ( CheckValidDoc( ptrActiveDoc ) == TRUE	)
	{
		ptrActiveDoc->Doc_Prewitt();
	}
}

/*======================================================================
Roberts Edge
========================================================================*/
void CDIPDlg::Roberts( void )
{
	CTK_ALPRWizardDoc * ptrActiveDoc= NULL;
	ptrActiveDoc = (CTK_ALPRWizardDoc * )GetActiveDocument();

	if ( CheckValidDoc( ptrActiveDoc ) == TRUE	)
	{
		ptrActiveDoc->Doc_Roberts();
	}
}

/*======================================================================
Canny Edge
========================================================================*/
void CDIPDlg::CannyEdge( void )
{
	CTK_ALPRWizardDoc * ptrActiveDoc= NULL;
	ptrActiveDoc = (CTK_ALPRWizardDoc * )GetActiveDocument();

	if ( CheckValidDoc( ptrActiveDoc ) == TRUE	)
	{
		ptrActiveDoc->Doc_Canny();
	}
}

/*======================================================================
Labeling 
========================================================================*/
void CDIPDlg::Labeling( void )
{
	CTK_ALPRWizardDoc * ptrActiveDoc= NULL;
	ptrActiveDoc = (CTK_ALPRWizardDoc * )GetActiveDocument();

	if ( CheckValidDoc( ptrActiveDoc ) == TRUE	)
	{
		ptrActiveDoc->Doc_DeNoiseLabel();
	}
}

/*======================================================================
Erosion : 영상 침식 ??
========================================================================*/
void CDIPDlg::Erosion( void )
{
	CTK_ALPRWizardDoc * ptrActiveDoc= NULL;
	ptrActiveDoc = (CTK_ALPRWizardDoc * )GetActiveDocument();

	if ( CheckValidDoc( ptrActiveDoc ) == TRUE	)
	{
		ptrActiveDoc->Doc_Erosion();
	}
}

/*======================================================================
SubImage : 차 영상 
ptrDOC1 : 첫번째 영상 document
ptrDOC2 : 두번째 영상 documnet
========================================================================*/
void CDIPDlg::SubImage( void * ptrDoc1, void *ptrDoc2  )
{
	CTK_ALPRWizardDoc * ptrActiveDoc= NULL;
	ptrActiveDoc = (CTK_ALPRWizardDoc * )GetActiveDocument();

	if ( CheckValidDoc( ptrActiveDoc ) == TRUE	)
	{
		ptrActiveDoc->Doc_SubImage( ptrDoc1, ptrDoc2 );
	}
}

/*======================================================================
Median( 중앙값) 영상 처리
========================================================================*/
void CDIPDlg::Median( void )
{
	CTK_ALPRWizardDoc * ptrActiveDoc= NULL;
	ptrActiveDoc = (CTK_ALPRWizardDoc * )GetActiveDocument();

	if ( CheckValidDoc( ptrActiveDoc ) == TRUE	)
	{
		ptrActiveDoc->Doc_Median();
	}
}	

/*======================================================================
Mean( 평균값) 영상 처리
========================================================================*/
void CDIPDlg::Mean( void )
{
	CTK_ALPRWizardDoc * ptrActiveDoc= NULL;
	ptrActiveDoc = (CTK_ALPRWizardDoc * )GetActiveDocument();

	if ( CheckValidDoc( ptrActiveDoc ) == TRUE	)
	{
		ptrActiveDoc->Doc_Mean();
	}
}

/*======================================================================
WeightMean( 가중 평균값) 영상 처리
========================================================================*/
void CDIPDlg::WeightMean( void )
{
	CTK_ALPRWizardDoc * ptrActiveDoc= NULL;
	ptrActiveDoc = (CTK_ALPRWizardDoc * )GetActiveDocument();

	if ( CheckValidDoc( ptrActiveDoc ) == TRUE	)
	{
		ptrActiveDoc->Doc_WeightMean();
	}
}

/*======================================================================
Contrast 영상 처리
========================================================================*/
void CDIPDlg::Contrast( void )
{
	CTK_ALPRWizardDoc * ptrActiveDoc= NULL;
	ptrActiveDoc = (CTK_ALPRWizardDoc * )GetActiveDocument();

	if ( CheckValidDoc( ptrActiveDoc ) == TRUE	)
	{
		ptrActiveDoc->Doc_Contrast();
	}
}

/*======================================================================
Beam : 차량 해드라이트 제거 필터
========================================================================*/
void CDIPDlg::Beam( void )
{
	CTK_ALPRWizardDoc * ptrActiveDoc= NULL;
	ptrActiveDoc = (CTK_ALPRWizardDoc * )GetActiveDocument();

	if ( CheckValidDoc( ptrActiveDoc ) == TRUE	)
	{
		ptrActiveDoc->Doc_Beam();
	}
}	

/*======================================================================
Diffusin : ??
========================================================================*/
void CDIPDlg::Diffusion( void )
{
	CTK_ALPRWizardDoc * ptrActiveDoc= NULL;
	ptrActiveDoc = (CTK_ALPRWizardDoc * )GetActiveDocument();

	if ( CheckValidDoc( ptrActiveDoc ) == TRUE	)
	{
		ptrActiveDoc->Doc_Diffusion();
	}
}

/*======================================================================
MMSE : ??
========================================================================*/
void CDIPDlg::MMSE( void )
{
	CTK_ALPRWizardDoc * ptrActiveDoc= NULL;
	ptrActiveDoc = (CTK_ALPRWizardDoc * )GetActiveDocument();

	if ( CheckValidDoc( ptrActiveDoc ) == TRUE	)
	{
		ptrActiveDoc->Doc_MMSE();
	}
}


/*======================================================================
HoughTransform
========================================================================*/
void CDIPDlg::HoughTransform( void )
{
	CTK_ALPRWizardDoc * ptrActiveDoc= NULL;
	ptrActiveDoc = (CTK_ALPRWizardDoc * )GetActiveDocument();

	if ( CheckValidDoc( ptrActiveDoc ) == TRUE	)
	{
		ptrActiveDoc->Doc_HoughTransform();
	}
}
/*======================================================================
Rotation
========================================================================*/
void CDIPDlg::Rotation( void )
{

	UpdateData(TRUE);

	CTK_ALPRWizardDoc * ptrActiveDoc= NULL;
	ptrActiveDoc = (CTK_ALPRWizardDoc * )GetActiveDocument();

	if ( CheckValidDoc( ptrActiveDoc ) == TRUE	)
	{
		ptrActiveDoc->Doc_Rotation( m_ctrl_d_rotation );
	}
}

/*======================================================================
Differance Of Gaussian ( DoG )
========================================================================*/
void CDIPDlg::DoG( void )
{
	UpdateData(TRUE);

	CTK_ALPRWizardDoc * ptrActiveDoc= NULL;
	ptrActiveDoc = (CTK_ALPRWizardDoc * )GetActiveDocument();

	if ( CheckValidDoc( ptrActiveDoc ) == TRUE	)
	{
		ptrActiveDoc->Doc_DoG( m_ctrl_DoGSig1, m_ctrl_DoGSig2 );
	}
}

/*======================================================================
Bilinear + Normalization Transform
========================================================================*/
void CDIPDlg::Bilinear_Normal( void )
{

	CTK_ALPRWizardDoc * ptrActiveDoc= NULL;
	ptrActiveDoc = (CTK_ALPRWizardDoc * )GetActiveDocument();

	if ( CheckValidDoc( ptrActiveDoc ) == TRUE	)
	{
		ptrActiveDoc->Doc_Bilinear_Normal( );
	}
}

void CDIPDlg::UnSharp( void )
{
	CTK_ALPRWizardDoc * ptrActiveDoc= NULL;
	ptrActiveDoc = (CTK_ALPRWizardDoc * )GetActiveDocument();

	if ( CheckValidDoc( ptrActiveDoc ) == TRUE	)
	{
		ptrActiveDoc->Doc_Unsharp( m_ctl_unsharp_mask );
	}
}

void CDIPDlg::Dilation( void )
{
	CTK_ALPRWizardDoc * ptrActiveDoc= NULL;
	ptrActiveDoc = (CTK_ALPRWizardDoc * )GetActiveDocument();

	if ( CheckValidDoc( ptrActiveDoc ) == TRUE	)
	{
		ptrActiveDoc->Doc_Dilation( );
	}
}

void CDIPDlg::Opening( void )
{
	CTK_ALPRWizardDoc * ptrActiveDoc= NULL;
	ptrActiveDoc = (CTK_ALPRWizardDoc * )GetActiveDocument();

	if ( CheckValidDoc( ptrActiveDoc ) == TRUE	)
	{
		ptrActiveDoc->Doc_Opening( );
	}
}

void CDIPDlg::Closing( void )
{
	CTK_ALPRWizardDoc * ptrActiveDoc= NULL;
	ptrActiveDoc = (CTK_ALPRWizardDoc * )GetActiveDocument();

	if ( CheckValidDoc( ptrActiveDoc ) == TRUE	)
	{
		ptrActiveDoc->Doc_Closing( );
	}
}

/*======================================================================================================
Wizard Function
========================================================================================================*/
void CDIPDlg::OnBnClickedButton1()
{

	if( m_ptrALPRDlg != NULL )
	{
		m_ptrALPRDlg->NotifyClose( IDD_DIP_DIALOG );
	}

	DestroyWindow();
}

void CDIPDlg::OnBnClickedButton7()
{
	SobelEdge();
}

void CDIPDlg::OnBnClickedButton49()
{
	ConvertGrayscale();
}

void CDIPDlg::OnBnClickedButton11()
{
	CannyEdge();
}

void CDIPDlg::OnBnClickedButton6()
{
	Erosion();
}

void CDIPDlg::OnBnClickedButton15()
{
	Median();
}

void CDIPDlg::OnBnClickedButton36()
{
	Diffusion();
}

void CDIPDlg::OnBnClickedButton3()
{
	Histogram( );
}

void CDIPDlg::OnBnClickedButton5()
{
	Binarization_Iterate( );
}

void CDIPDlg::OnBnClickedButton9()
{
	BitPlane();
}

void CDIPDlg::OnBnClickedButton18()
{
	Prewitt();
}

void CDIPDlg::OnBnClickedButton12()
{
	Labeling();
}

void CDIPDlg::OnBnClickedButton20()
{
	Mean();
}

void CDIPDlg::OnBnClickedButton2()
{
	Histogram_Equal();
}

void CDIPDlg::OnBnClickedButton22()
{
	Binarization_Otsu();
}

void CDIPDlg::OnBnClickedButton23()
{
	Binarization_Adaptive();
}

void CDIPDlg::OnBnClickedButton16()
{
	Beam();
}

void CDIPDlg::OnBnClickedButton10()
{
	Gaussian();
}

void CDIPDlg::OnBnClickedButton19()
{
	Roberts();
}

void CDIPDlg::OnBnClickedButton37()
{
	MMSE();
}

void CDIPDlg::OnBnClickedButton21()
{
	WeightMean();
}

void CDIPDlg::OnBnClickedButton17()
{
	Contrast();
}

void CDIPDlg::OnBnClickedButton45()
{
	HoughTransform();
}

void CDIPDlg::OnBnClickedButton48()
{
	Bilinear_Normal();
}

void CDIPDlg::OnBnClickedButton46()
{
	Rotation();	
}

void CDIPDlg::OnBnClickedButton4()
{
	Binarization( );
}

void CDIPDlg::OnBnClickedButton8()
{
	Brightness();
}

void CDIPDlg::OnBnClickedButton38()
{
	Binarization_ITG_Adaptive();
}

void CDIPDlg::OnBnClickedButton47()
{
	DoG();
}

void CDIPDlg::OnBnClickedButton50()
{
	UnSharp();	
}

void CDIPDlg::OnBnClickedButton13()
{
	Opening();
}

void CDIPDlg::OnBnClickedButton51()
{
	Closing();
}

void CDIPDlg::OnBnClickedButton52()
{
	Dilation();
}
