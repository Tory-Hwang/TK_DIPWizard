/*================================================================================
Copyright (c) 2011 Tory Hwang 
Modify : Tory in 2013 .01

Mail : tory45@empal.com
*=================================================================================*/


// TK_ALPRWizard.cpp : 응용 프로그램에 대한 클래스 동작을 정의합니다.
//

#include "stdafx.h"
#include "TK_ALPRWizard.h"
#include "MainFrm.h"

#include "ChildFrm.h"
#include "TK_ALPRWizardDoc.h"
#include "TK_ALPRWizardView.h"
#include "ALPRWizard_Def.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CTK_ALPRWizardApp

BEGIN_MESSAGE_MAP(CTK_ALPRWizardApp, CWinApp)
	ON_COMMAND(ID_APP_ABOUT, &CTK_ALPRWizardApp::OnAppAbout)
	// 표준 파일을 기초로 하는 문서 명령입니다.
	ON_COMMAND(ID_FILE_NEW, &CWinApp::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, &CWinApp::OnFileOpen)
	// 표준 인쇄 설정 명령입니다.
	ON_COMMAND(ID_FILE_PRINT_SETUP, &CWinApp::OnFilePrintSetup)
	ON_COMMAND(ID_CLOSE_ALL_VIEW, &CTK_ALPRWizardApp::OnCloseAllView)
END_MESSAGE_MAP()



CTK_ALPRWizardApp::CTK_ALPRWizardApp()
{	
	m_ptrImageData = NULL;
	m_nViewXPos = DLG_WIDTH + 2;
	m_nViewYPos = 0;
	m_ptrActiveDoc = NULL;
	m_ptrFirstDoc= NULL;
	m_ptrtcFileName = NULL;
	m_pImageDocTemplate =NULL;
	m_ptrRect = NULL;
	m_ptrLPRChars = NULL;
	m_DlgDC = NULL;

	m_bCreateWindow = FALSE;
	memset( &m_CompareImage1, 0, sizeof( IMAGE_DATA_t ));
}

CTK_ALPRWizardApp::~CTK_ALPRWizardApp()
{
	CWizardHelper WizardHelper;

	if( m_ptrImageData != NULL )
		WizardHelper.FreeImageAllocData( m_ptrImageData );
	
}

CTK_ALPRWizardApp theApp;

/*=======================================================================================================================*/

/*======================================================================
새로운 Image Document 생성 
========================================================================*/
void CTK_ALPRWizardApp::CreateNewImageDoc( TCHAR * ptrtcFileName, IMAGE_DATA_t * ptrImageData, 
										   CVRECT_t * ptrRect , pLPRCHAR ptrLPRChars )
{	
	m_ptrtcFileName = ptrtcFileName;
	m_ptrImageData = ptrImageData;
	m_ptrRect = ptrRect;
	m_ptrLPRChars = ptrLPRChars;

	AfxGetMainWnd()->SendMessage(WM_COMMAND, ID_FILE_NEW);
}

/*======================================================================
Window 생성 유무 확인 
========================================================================*/
BOOL CTK_ALPRWizardApp::GetCreateWindow( void )
{
	return m_bCreateWindow;
}

/*======================================================================
Child frame 출력 크기 설정  
bCreate : Dlg 에서 Multi Doc 체크 시 1 , 아니면 0
        1 일경우 새 파일을 열때마다 새로운 DOC, Viewr 가 생성됨 
========================================================================*/
void CTK_ALPRWizardApp::SetCreateWindow( BOOL bCreate )
{
	int nFramSize =0;

	if ( bCreate == TRUE )
	{		
		nFramSize = SW_NORMAL;
	}
	else
	{
		nFramSize = SW_MAXIMIZE;		
	}

	CMDIFrameWnd *pFrame = (CMDIFrameWnd*)AfxGetApp()->m_pMainWnd; 
	CMDIChildWnd *pChild = (CMDIChildWnd *) pFrame->GetActiveFrame(); 
	pChild->ShowWindow(nFramSize);

	m_bCreateWindow = bCreate;
}

/*======================================================================
현재 Active Document 설정 
========================================================================*/
void CTK_ALPRWizardApp::SetActiveDoc( CDocument *  ptrDoc )
{
	m_ptrActiveDoc = ptrDoc;	

	/* MainFrame 에게 현재 Active된 Document의 이미지를 Display 하라는 메세지 보냄 */
	AfxGetMainWnd()->PostMessage(WM_COMMAND, WM_USR_INFO_ACTIVE_VIEW );
}

/*======================================================================
현재 Active Document 요청
========================================================================*/
CDocument * CTK_ALPRWizardApp::GetActiveDoc( void )
{	
	return m_ptrActiveDoc;
}

/*======================================================================
첫번째 Document 설정
========================================================================*/
void CTK_ALPRWizardApp::SetFirstDoc( CDocument *  ptrDoc )
{
	m_ptrFirstDoc= ptrDoc;
}

/*======================================================================
첫번째 Document 요청
========================================================================*/
CDocument * CTK_ALPRWizardApp::GetFirstDoc( void )
{	
	return m_ptrFirstDoc;
}

/*======================================================================
View의 X, Y 위치 요청
========================================================================*/
int CTK_ALPRWizardApp::GetViewXYPos( int *XPos, int * YPos )
{
	m_nViewXPos += 5;
	m_nViewYPos += 5;

	if ( m_nViewXPos >= IMAGE_WIDTH )
		m_nViewXPos = DLG_WIDTH + 5;

	if ( m_nViewYPos >= DLG_HEIGHT / 2 )
		m_nViewYPos=0;

	*XPos = m_nViewXPos;
	*YPos = m_nViewYPos;

	return 0;
}

/*======================================================================
View의 X, Y 위치 설정
========================================================================*/
int CTK_ALPRWizardApp::SetViewXYPos( int nXPos, int  nYPos )
{
	m_nViewXPos += ( nXPos + 5 );
	m_nViewYPos += ( nYPos + 5 );
	return 0;
}

/*======================================================================
모든 View 종료
========================================================================*/
void CTK_ALPRWizardApp::CloseAllView( void )
{
	OnCloseAllView();
}

/*======================================================================
HDC 설정
========================================================================*/
void CTK_ALPRWizardApp::SetDlgHDC( HDC DlgDC )
{
	m_DlgDC = DlgDC;
}

/*======================================================================
HDC 요청
========================================================================*/
HDC CTK_ALPRWizardApp::GetDlgHDC( void )
{
	return m_DlgDC;
}

/*
int CTK_ALPRWizardApp::SetCompareImage( IMAGE_DATA_t * ptrImageData )
{
	CWizardHelper WizardHelper;

	if ( ptrImageData != NULL  && ptrImageData->ptrSrcImage2D != NULL )
	{
		if ( m_CompareImage1.ptrSrcImage2D != NULL )
		{
			WizardHelper.FreeImageAllocData( &m_CompareImage1 );
		}
		WizardHelper.CopyImageData( &m_CompareImage1, ptrImageData );
	}

	return ERR_SUCCESS;
}

IMAGE_DATA_t * CTK_ALPRWizardApp::GetCompareImage( void )
{
	return &m_CompareImage;
}

int CTK_ALPRWizardApp::RealseCompareImage( void )
{
	CWizardHelper WizardHelper;

	if ( m_CompareImage.ptrSrcImage2D != NULL )
	{
		WizardHelper.FreeImageAllocData( &m_CompareImage );
	}
	m_CompareImage.nBitCount = 0;
	m_CompareImage.nH = 0;
	m_CompareImage.nImageSize = 0;
	m_CompareImage.nW = 0;
	return ERR_SUCCESS;
}
*/
BOOL CTK_ALPRWizardApp::InitInstance()
{
	// 응용 프로그램 매니페스트가 ComCtl32.dll 버전 6 이상을 사용하여 비주얼 스타일을
	// 사용하도록 지정하는 경우, Windows XP 상에서 반드시 InitCommonControlsEx()가 필요합니다. 
	// InitCommonControlsEx()를 사용하지 않으면 창을 만들 수 없습니다.
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// 응용 프로그램에서 사용할 모든 공용 컨트롤 클래스를 포함하도록
	// 이 항목을 설정하십시오.
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();

	// OLE 라이브러리를 초기화합니다.
	if (!AfxOleInit())
	{
		AfxMessageBox(IDP_OLE_INIT_FAILED);
		return FALSE;
	}
	AfxEnableControlContainer();
	// 표준 초기화
	// 이들 기능을 사용하지 않고 최종 실행 파일의 크기를 줄이려면
	// 아래에서 필요 없는 특정 초기화
	// 루틴을 제거해야 합니다.
	// 해당 설정이 저장된 레지스트리 키를 변경하십시오.
	// TODO: 이 문자열을 회사 또는 조직의 이름과 같은
	// 적절한 내용으로 수정해야 합니다.
	SetRegistryKey(_T("로컬 응용 프로그램 마법사에서 생성된 응용 프로그램"));
	LoadStdProfileSettings(4);  // MRU를 포함하여 표준 INI 파일 옵션을 로드합니다.
	// 응용 프로그램의 문서 템플릿을 등록합니다. 문서 템플릿은
	//  문서, 프레임 창 및 뷰 사이의 연결 역할을 합니다.
	
	//CMultiDocTemplate* pDocTemplate;

	m_pImageDocTemplate = new CMultiDocTemplate(IDR_TK_ALPRWizardTYPE,
		RUNTIME_CLASS(CTK_ALPRWizardDoc),
		RUNTIME_CLASS(CChildFrame), // 사용자 지정 MDI 자식 프레임입니다.
		RUNTIME_CLASS(CTK_ALPRWizardView));
	if (!m_pImageDocTemplate)
		return FALSE;
	AddDocTemplate(m_pImageDocTemplate);

	// 주 MDI 프레임 창을 만듭니다.
	CMainFrame* pMainFrame = new CMainFrame;
	if (!pMainFrame || !pMainFrame->LoadFrame(IDR_MAINFRAME))
	{
		delete pMainFrame;
		return FALSE;
	}
	m_pMainWnd = pMainFrame;
	// 접미사가 있을 경우에만 DragAcceptFiles를 호출합니다.
	//  MDI 응용 프로그램에서는 m_pMainWnd를 설정한 후 바로 이러한 호출이 발생해야 합니다.


	// 표준 셸 명령, DDE, 파일 열기에 대한 명령줄을 구문 분석합니다.
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);


	// 명령줄에 지정된 명령을 디스패치합니다.
	// 응용 프로그램이 /RegServer, /Register, /Unregserver 또는 /Unregister로 시작된 경우 FALSE를 반환합니다.
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;
	// 주 창이 초기화되었으므로 이를 표시하고 업데이트합니다.
	pMainFrame->ShowWindow(m_nCmdShow);
	pMainFrame->UpdateWindow();

	return TRUE;
}



// 응용 프로그램 정보에 사용되는 CAboutDlg 대화 상자입니다.

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

// 구현입니다.
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()

// 대화 상자를 실행하기 위한 응용 프로그램 명령입니다.
void CTK_ALPRWizardApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}


// CTK_ALPRWizardApp 메시지 처리기


void CTK_ALPRWizardApp::OnCloseAllView()
{
	CWizardHelper WizardHelper;

	if( m_ptrImageData != NULL )
		WizardHelper.FreeImageAllocData( m_ptrImageData );	

	m_ptrImageData = NULL;
	m_nViewXPos = DLG_WIDTH + 2;
	m_nViewYPos = 0;
	m_ptrActiveDoc = NULL;
	m_ptrFirstDoc= NULL;
	m_ptrtcFileName = NULL;

	memset( &m_CompareImage1, 0, sizeof( IMAGE_DATA_t ));


	m_pImageDocTemplate->CloseAllDocuments( FALSE );
	AfxGetMainWnd()->SendMessage(WM_COMMAND, ID_FILE_NEW);
}
