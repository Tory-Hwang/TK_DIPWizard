/*================================================================================
Copyright (c) 2011 Tory Hwang 
Modify : Tory in 2013 .01

Mail : tory45@empal.com
*=================================================================================*/



// MainFrm.cpp : CMainFrame Ŭ������ ����
//

#include "stdafx.h"
#include "TK_ALPRWizard.h"

#include "MainFrm.h"
#include "ALPRWizard_Def.h"
#include "TK_ALPRWizardDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CMainFrame

IMPLEMENT_DYNAMIC(CMainFrame, CMDIFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CMDIFrameWnd)
	ON_COMMAND( WM_USR_INFO_ACTIVE_VIEW, &CMainFrame::OnShowActiveViewInfo)
	ON_WM_CREATE()
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // ���� �� ǥ�ñ�
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};


// CMainFrame ����/�Ҹ�

CMainFrame::CMainFrame()
{
	// TODO: ���⿡ ��� �ʱ�ȭ �ڵ带 �߰��մϴ�.
}

CMainFrame::~CMainFrame()
{

}

void CMainFrame::OnShowActiveViewInfo( void )
{
	CTK_ALPRWizardApp* pApp = (CTK_ALPRWizardApp*)AfxGetApp();	
	CTK_ALPRWizardDoc * ptrActDoc= NULL;
	CTK_ALPRWizardDoc * ptrFirstDoc= NULL;

	CString strMsg;

	ptrActDoc = ( CTK_ALPRWizardDoc * )pApp->GetActiveDoc();
	ptrFirstDoc = ( CTK_ALPRWizardDoc * )pApp->GetFirstDoc();
	
	if ( ptrFirstDoc != NULL  )
		if ( ptrActDoc != NULL )
			ptrFirstDoc->m_pALPRDlg->DisplayActiveView( ptrActDoc->m_tcFileName, &ptrActDoc->m_ImageData );
		else
			ptrFirstDoc->m_pALPRDlg->DisplayActiveView( NULL, NULL );

	//m_wndStatusBar.SetPaneText(0, strMsg);
}


int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CMDIFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	
#if 0
	if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
		| CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_wndToolBar.LoadToolBar(IDR_MAINFRAME))
	{
		TRACE0("���� ������ ������ ���߽��ϴ�.\n");
		return -1;      // ������ ���߽��ϴ�.
	}

	


	// TODO: ���� ������ ��ŷ�� �� ���� �Ϸ��� �� �� ���� �����Ͻʽÿ�.
	m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	EnableDocking(CBRS_ALIGN_ANY);
	DockControlBar(&m_wndToolBar);
#endif

	if (!m_wndStatusBar.Create(this) ||
		!m_wndStatusBar.SetIndicators(indicators,
		sizeof(indicators)/sizeof(UINT)))
	{
		TRACE0("���� ǥ������ ������ ���߽��ϴ�.\n");
		return -1;      // ������ ���߽��ϴ�.
	}

	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	
	if( !CMDIFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	
	cs.style &= ~FWS_ADDTOTITLE;	
	cs.lpszName = _T("TK ALPR Wizard");

	cs.cx = (int )( DLG_WIDTH + IMAGE_WIDTH + 50 );
	cs.cy = ( DLG_HEIGHT + 30 );

	return TRUE;
}


// CMainFrame ����

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CMDIFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CMDIFrameWnd::Dump(dc);
}

#endif //_DEBUG


// CMainFrame �޽��� ó����



