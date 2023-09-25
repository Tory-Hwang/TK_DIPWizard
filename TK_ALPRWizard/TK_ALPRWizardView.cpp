/*================================================================================
Copyright (c) 2011 Tory Hwang 
Modify : Tory in 2013 .01

Mail : tory45@empal.com
*=================================================================================*/



// TK_ALPRWizardView.cpp : CTK_ALPRWizardView 클래스의 구현
//

#include "stdafx.h"
#include "TK_ALPRWizard.h"

#include "MainFrm.h"
#include "TK_ALPRWizardDoc.h"
#include "TK_ALPRWizardView.h"

#include "ALPRWizard_Def.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CTK_ALPRWizardView

IMPLEMENT_DYNCREATE(CTK_ALPRWizardView, CScrollView)

BEGIN_MESSAGE_MAP(CTK_ALPRWizardView, CScrollView)
	// 표준 인쇄 명령입니다.
	ON_COMMAND(ID_FILE_PRINT, &CScrollView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CScrollView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CScrollView::OnFilePrintPreview)
	ON_WM_ERASEBKGND()	
	ON_WM_SETFOCUS()	
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_KILLFOCUS()	
	
END_MESSAGE_MAP()

// CTK_ALPRWizardView 생성/소멸

CTK_ALPRWizardView::CTK_ALPRWizardView()
{
	m_bFirst = FALSE;
}

CTK_ALPRWizardView::~CTK_ALPRWizardView()
{
	CTK_ALPRWizardDoc* pDoc = GetDocument();
	if ( m_bFirst )
	{
		pDoc->Doc_Free_TKLIB();
	}
}

/* ==========================================================================
화면 사이즈 계산 
===========================================================================*/
void CTK_ALPRWizardView::FullScrollSize( CTK_ALPRWizardDoc* pDoc )
{
	CScrollView::OnInitialUpdate();
	CSize sizeTotal;	
	CRect ViewRect;	
	int nExtWidth = 0;

	
	GetClientRect(ViewRect);
	if ( pDoc->m_ImageData.ptrSrcImage2D != NULL )
	{		
		nExtWidth = pDoc->m_ImageData.nW;
		
	}

	sizeTotal.cx = DLG_WIDTH ;
	sizeTotal.cy = ViewRect.bottom ;
	SetScrollSizes(MM_TEXT, sizeTotal);
	ResizeParentToFit(TRUE);
	
}

/* ==========================================================================
Scrolsize 계산 
===========================================================================*/
void CTK_ALPRWizardView::OptScrollSize( CTK_ALPRWizardDoc* pDoc )
{
	CScrollView::OnInitialUpdate();
	CSize sizeTotal;	
	
	if ( pDoc->m_ImageData.ptrSrcImage2D != NULL )
	{		
		sizeTotal.cx = pDoc->m_ImageData.nW;
		sizeTotal.cy = pDoc->m_ImageData.nH;
	}
	else
	{
		sizeTotal.cx = sizeTotal.cy = 100;
	}
	
	SetScrollSizes(MM_TEXT, sizeTotal);
	ResizeParentToFit(TRUE);
}

/* ==========================================================================
Dialog Box 위치 선정 
===========================================================================*/
void  CTK_ALPRWizardView::MoveConfigDlg( CTK_ALPRWizardDoc* pDoc )
{
	CRect ViewRect;	

	GetClientRect(ViewRect);
	pDoc->m_pALPRDlg->MoveWindow(0 , 0, DLG_WIDTH, ViewRect.bottom );
}

/* ==========================================================================
현재 화면에 보이고 있는 View와 연결된 DOC 설정 
===========================================================================*/
void CTK_ALPRWizardView::SetActiveDoc( void  )
{
	CTK_ALPRWizardApp * ptrApp = ( CTK_ALPRWizardApp * )AfxGetApp();
	CTK_ALPRWizardDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);	
	ASSERT_VALID( ptrApp );	

	if (!pDoc || !ptrApp )
		return;	
	
	ptrApp->SetActiveDoc( pDoc);

}
/* ==========================================================================
현재 화면에 보이고 있는 View의 위치 설정 
===========================================================================*/
void  CTK_ALPRWizardView::SetActivePos( void )
{
	CTK_ALPRWizardApp * ptrApp = ( CTK_ALPRWizardApp * )AfxGetApp();
	CRect ViewRect;	

	ASSERT_VALID( ptrApp );	

	AfxGetMainWnd()->GetClientRect (ViewRect);


	if (!ptrApp )
		return;	
	ptrApp->SetViewXYPos( ViewRect.left, ViewRect.top );
}

/*====================================================================
Wizard Function
=====================================================================*/
BOOL CTK_ALPRWizardView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: CREATESTRUCT cs를 수정하여 여기에서
	//  Window 클래스 또는 스타일을 수정합니다.
	

	return CScrollView::PreCreateWindow(cs);
}


/* ==========================================================================
 Dlg를 그리고 난뒤 DOC에 등록된 Image Data를 그린다. 
===========================================================================*/
void CTK_ALPRWizardView::OnDraw(CDC* pDC)
{
	CTK_ALPRWizardDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;
	
	if ( pDoc->m_pALPRDlg != NULL )
	{
		//FullScrollSize( pDoc );
		MoveConfigDlg( pDoc );	
	}

	pDoc->Doc_DrawImage( pDC->m_hDC );
}

/* ==========================================================================
 View 갱신 
 처음 생성되었을 경우 
 1. DLL을 초기화
 2. 설정 Dialog 생성 
 3. Scrollsize 초기화 
===========================================================================*/
void CTK_ALPRWizardView::OnInitialUpdate()
{
	CTK_ALPRWizardDoc* pDoc = GetDocument();
	CTK_ALPRWizardApp * ptrApp = ( CTK_ALPRWizardApp * )AfxGetApp();
	ASSERT_VALID(pDoc);	
	ASSERT_VALID(ptrApp);	

	if (!pDoc || !ptrApp )
		return;	
	
	/* 첫번째 생성된 View는 ALPR 다이얼을 하나 생성하고 CFirstApp에 첫번째 View가 생성됨으로 설정 한다 */
	if ( ptrApp->GetFirstDoc() == NULL )
	{
		m_bFirst = TRUE;
		ptrApp->SetFirstDoc( pDoc );
		pDoc->Doc_Load_TKLIB();

		pDoc->m_pALPRDlg = new CALPRDlg();
		pDoc->m_pALPRDlg->Create( IDD_ALPR_DIALOG, this );
		pDoc->m_pALPRDlg->ShowWindow( SW_SHOW );
		pDoc->m_pALPRDlg->Init_Dlg();
		FullScrollSize( pDoc );
		MoveConfigDlg( pDoc );
	}
	else
	{		
		OptScrollSize( pDoc );
		ptrApp->SetActiveDoc( pDoc );
	}	
}


// CTK_ALPRWizardView 인쇄

BOOL CTK_ALPRWizardView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// 기본적인 준비
	return DoPreparePrinting(pInfo);
}

void CTK_ALPRWizardView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 인쇄하기 전에 추가 초기화 작업을 추가합니다.
}

void CTK_ALPRWizardView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	
}


// CTK_ALPRWizardView 진단

#ifdef _DEBUG
void CTK_ALPRWizardView::AssertValid() const
{
	CScrollView::AssertValid();
}

void CTK_ALPRWizardView::Dump(CDumpContext& dc) const
{
	CScrollView::Dump(dc);
}

CTK_ALPRWizardDoc* CTK_ALPRWizardView::GetDocument() const // 디버그되지 않은 버전은 인라인으로 지정됩니다.
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CTK_ALPRWizardDoc)));
	return (CTK_ALPRWizardDoc*)m_pDocument;
}
#endif //_DEBUG



BOOL CTK_ALPRWizardView::OnEraseBkgnd(CDC* pDC)
{
#if 1
	/* 격자모양의 배경 처리 */
	CBrush br;
	br.CreateHatchBrush(HS_DIAGCROSS, RGB(0xFF, 0xCC, 0));
	//br.CreateSolidBrush( RGB(0x00, 0x33, 0));
	FillOutsideRect(pDC, &br);
	
	return TRUE;
#else
	return CScrollView::OnEraseBkgnd(pDC);
#endif

}

void CTK_ALPRWizardView::OnSetFocus(CWnd* pOldWnd)
{
	if ( !m_bFirst)
	{
		SetActiveDoc( );
	}

	CScrollView::OnSetFocus(pOldWnd);
}

void CTK_ALPRWizardView::OnLButtonDown(UINT nFlags, CPoint point)
{		
	if ( !m_bFirst)
	{
		SetActiveDoc( );
	}

	CScrollView::OnLButtonDown(nFlags, point);
}

void CTK_ALPRWizardView::OnLButtonUp(UINT nFlags, CPoint point)
{

	CScrollView::OnLButtonUp(nFlags, point);
}

void CTK_ALPRWizardView::OnKillFocus(CWnd* pNewWnd)
{
	if ( !m_bFirst)
	{
		SetActivePos( );
	}
	CScrollView::OnKillFocus(pNewWnd);
}
