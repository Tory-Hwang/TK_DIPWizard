// TK_ALPRWizardView.h : CTK_ALPRWizardView 클래스의 인터페이스
//


#pragma once


class CTK_ALPRWizardView : public CScrollView
{
protected: // serialization에서만 만들어집니다.
	CTK_ALPRWizardView();
	DECLARE_DYNCREATE(CTK_ALPRWizardView)

// 특성입니다.
public:
	CTK_ALPRWizardDoc* GetDocument() const;

private:

	void FullScrollSize( CTK_ALPRWizardDoc* pDoc );
	void OptScrollSize( CTK_ALPRWizardDoc* pDoc );
	void MoveConfigDlg( CTK_ALPRWizardDoc* pDoc );	
	void SetActiveDoc( void  );
	void SetActivePos( void );
public:

private:
	BOOL m_bFirst;
public:
	virtual void OnDraw(CDC* pDC);  // 이 뷰를 그리기 위해 재정의되었습니다.
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual void OnInitialUpdate(); // 생성 후 처음 호출되었습니다.
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

// 구현입니다.
public:
	virtual ~CTK_ALPRWizardView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// 생성된 메시지 맵 함수
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnKillFocus(CWnd* pNewWnd);	
};

#ifndef _DEBUG  // TK_ALPRWizardView.cpp의 디버그 버전
inline CTK_ALPRWizardDoc* CTK_ALPRWizardView::GetDocument() const
   { return reinterpret_cast<CTK_ALPRWizardDoc*>(m_pDocument); }
#endif

