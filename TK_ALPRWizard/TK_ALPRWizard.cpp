/*================================================================================
Copyright (c) 2011 Tory Hwang 
Modify : Tory in 2013 .01

Mail : tory45@empal.com
*=================================================================================*/


// TK_ALPRWizard.cpp : ���� ���α׷��� ���� Ŭ���� ������ �����մϴ�.
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
	// ǥ�� ������ ���ʷ� �ϴ� ���� ����Դϴ�.
	ON_COMMAND(ID_FILE_NEW, &CWinApp::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, &CWinApp::OnFileOpen)
	// ǥ�� �μ� ���� ����Դϴ�.
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
���ο� Image Document ���� 
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
Window ���� ���� Ȯ�� 
========================================================================*/
BOOL CTK_ALPRWizardApp::GetCreateWindow( void )
{
	return m_bCreateWindow;
}

/*======================================================================
Child frame ��� ũ�� ����  
bCreate : Dlg ���� Multi Doc üũ �� 1 , �ƴϸ� 0
        1 �ϰ�� �� ������ �������� ���ο� DOC, Viewr �� ������ 
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
���� Active Document ���� 
========================================================================*/
void CTK_ALPRWizardApp::SetActiveDoc( CDocument *  ptrDoc )
{
	m_ptrActiveDoc = ptrDoc;	

	/* MainFrame ���� ���� Active�� Document�� �̹����� Display �϶�� �޼��� ���� */
	AfxGetMainWnd()->PostMessage(WM_COMMAND, WM_USR_INFO_ACTIVE_VIEW );
}

/*======================================================================
���� Active Document ��û
========================================================================*/
CDocument * CTK_ALPRWizardApp::GetActiveDoc( void )
{	
	return m_ptrActiveDoc;
}

/*======================================================================
ù��° Document ����
========================================================================*/
void CTK_ALPRWizardApp::SetFirstDoc( CDocument *  ptrDoc )
{
	m_ptrFirstDoc= ptrDoc;
}

/*======================================================================
ù��° Document ��û
========================================================================*/
CDocument * CTK_ALPRWizardApp::GetFirstDoc( void )
{	
	return m_ptrFirstDoc;
}

/*======================================================================
View�� X, Y ��ġ ��û
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
View�� X, Y ��ġ ����
========================================================================*/
int CTK_ALPRWizardApp::SetViewXYPos( int nXPos, int  nYPos )
{
	m_nViewXPos += ( nXPos + 5 );
	m_nViewYPos += ( nYPos + 5 );
	return 0;
}

/*======================================================================
��� View ����
========================================================================*/
void CTK_ALPRWizardApp::CloseAllView( void )
{
	OnCloseAllView();
}

/*======================================================================
HDC ����
========================================================================*/
void CTK_ALPRWizardApp::SetDlgHDC( HDC DlgDC )
{
	m_DlgDC = DlgDC;
}

/*======================================================================
HDC ��û
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
	// ���� ���α׷� �Ŵ��佺Ʈ�� ComCtl32.dll ���� 6 �̻��� ����Ͽ� ���־� ��Ÿ����
	// ����ϵ��� �����ϴ� ���, Windows XP �󿡼� �ݵ�� InitCommonControlsEx()�� �ʿ��մϴ�. 
	// InitCommonControlsEx()�� ������� ������ â�� ���� �� �����ϴ�.
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// ���� ���α׷����� ����� ��� ���� ��Ʈ�� Ŭ������ �����ϵ���
	// �� �׸��� �����Ͻʽÿ�.
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();

	// OLE ���̺귯���� �ʱ�ȭ�մϴ�.
	if (!AfxOleInit())
	{
		AfxMessageBox(IDP_OLE_INIT_FAILED);
		return FALSE;
	}
	AfxEnableControlContainer();
	// ǥ�� �ʱ�ȭ
	// �̵� ����� ������� �ʰ� ���� ���� ������ ũ�⸦ ���̷���
	// �Ʒ����� �ʿ� ���� Ư�� �ʱ�ȭ
	// ��ƾ�� �����ؾ� �մϴ�.
	// �ش� ������ ����� ������Ʈ�� Ű�� �����Ͻʽÿ�.
	// TODO: �� ���ڿ��� ȸ�� �Ǵ� ������ �̸��� ����
	// ������ �������� �����ؾ� �մϴ�.
	SetRegistryKey(_T("���� ���� ���α׷� �����翡�� ������ ���� ���α׷�"));
	LoadStdProfileSettings(4);  // MRU�� �����Ͽ� ǥ�� INI ���� �ɼ��� �ε��մϴ�.
	// ���� ���α׷��� ���� ���ø��� ����մϴ�. ���� ���ø���
	//  ����, ������ â �� �� ������ ���� ������ �մϴ�.
	
	//CMultiDocTemplate* pDocTemplate;

	m_pImageDocTemplate = new CMultiDocTemplate(IDR_TK_ALPRWizardTYPE,
		RUNTIME_CLASS(CTK_ALPRWizardDoc),
		RUNTIME_CLASS(CChildFrame), // ����� ���� MDI �ڽ� �������Դϴ�.
		RUNTIME_CLASS(CTK_ALPRWizardView));
	if (!m_pImageDocTemplate)
		return FALSE;
	AddDocTemplate(m_pImageDocTemplate);

	// �� MDI ������ â�� ����ϴ�.
	CMainFrame* pMainFrame = new CMainFrame;
	if (!pMainFrame || !pMainFrame->LoadFrame(IDR_MAINFRAME))
	{
		delete pMainFrame;
		return FALSE;
	}
	m_pMainWnd = pMainFrame;
	// ���̻簡 ���� ��쿡�� DragAcceptFiles�� ȣ���մϴ�.
	//  MDI ���� ���α׷������� m_pMainWnd�� ������ �� �ٷ� �̷��� ȣ���� �߻��ؾ� �մϴ�.


	// ǥ�� �� ���, DDE, ���� ���⿡ ���� ������� ���� �м��մϴ�.
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);


	// ����ٿ� ������ ����� ����ġ�մϴ�.
	// ���� ���α׷��� /RegServer, /Register, /Unregserver �Ǵ� /Unregister�� ���۵� ��� FALSE�� ��ȯ�մϴ�.
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;
	// �� â�� �ʱ�ȭ�Ǿ����Ƿ� �̸� ǥ���ϰ� ������Ʈ�մϴ�.
	pMainFrame->ShowWindow(m_nCmdShow);
	pMainFrame->UpdateWindow();

	return TRUE;
}



// ���� ���α׷� ������ ���Ǵ� CAboutDlg ��ȭ �����Դϴ�.

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

// �����Դϴ�.
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

// ��ȭ ���ڸ� �����ϱ� ���� ���� ���α׷� ����Դϴ�.
void CTK_ALPRWizardApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}


// CTK_ALPRWizardApp �޽��� ó����


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
