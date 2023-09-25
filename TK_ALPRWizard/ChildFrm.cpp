/*================================================================================
Copyright (c) 2011 Tory Hwang 
Modify : Tory in 2013 .01

Mail : tory45@empal.com
*=================================================================================*/



// ChildFrm.cpp : CChildFrame 클래스의 구현
//
#include "stdafx.h"
#include "TK_ALPRWizard.h"

#include "ChildFrm.h"

#include "ALPRWizard_Def.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CChildFrame

IMPLEMENT_DYNCREATE(CChildFrame, CMDIChildWnd)

BEGIN_MESSAGE_MAP(CChildFrame, CMDIChildWnd)
	
END_MESSAGE_MAP()


// CChildFrame 생성/소멸

CChildFrame::CChildFrame()
{
	// TODO: 여기에 멤버 초기화 코드를 추가합니다.
}

CChildFrame::~CChildFrame()
{
}


BOOL CChildFrame::PreCreateWindow(CREATESTRUCT& cs)
{	
	if( !CMDIChildWnd::PreCreateWindow(cs) )
		return FALSE;	
	int nX, nY;

	CTK_ALPRWizardApp * ptrApp = ( CTK_ALPRWizardApp * )AfxGetApp();

	if ( ptrApp == NULL  )
		return FALSE;

	/* 첫번째 생성된 Viewer는 System 메뉴를 가지지 않는다 */
	if ( ptrApp->GetFirstDoc() == NULL )
	{
		cs.style &= ~ WS_SYSMENU;
		cs.cy = DLG_WIDTH;
		cs.cy = DLG_HEIGHT;
		cs.style &= ~FWS_ADDTOTITLE;	
		cs.lpszName = _T("TK ALPR Dialog");
	}	
	else
	{	
		//cs.style &= ~FWS_ADDTOTITLE;	
		//cs.lpszName = ptrApp->m_ptrtcFileName;
		ptrApp->GetViewXYPos( &nX, &nY );
		cs.x = nX;
		cs.y = nY;
	}
	return TRUE;
}


// CChildFrame 진단

#ifdef _DEBUG
void CChildFrame::AssertValid() const
{
	CMDIChildWnd::AssertValid();
}

void CChildFrame::Dump(CDumpContext& dc) const
{
	CMDIChildWnd::Dump(dc);
}

#endif //_DEBUG
