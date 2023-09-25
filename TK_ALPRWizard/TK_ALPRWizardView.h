// TK_ALPRWizardView.h : CTK_ALPRWizardView Ŭ������ �������̽�
//


#pragma once


class CTK_ALPRWizardView : public CScrollView
{
protected: // serialization������ ��������ϴ�.
	CTK_ALPRWizardView();
	DECLARE_DYNCREATE(CTK_ALPRWizardView)

// Ư���Դϴ�.
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
	virtual void OnDraw(CDC* pDC);  // �� �並 �׸��� ���� �����ǵǾ����ϴ�.
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual void OnInitialUpdate(); // ���� �� ó�� ȣ��Ǿ����ϴ�.
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

// �����Դϴ�.
public:
	virtual ~CTK_ALPRWizardView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// ������ �޽��� �� �Լ�
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnKillFocus(CWnd* pNewWnd);	
};

#ifndef _DEBUG  // TK_ALPRWizardView.cpp�� ����� ����
inline CTK_ALPRWizardDoc* CTK_ALPRWizardView::GetDocument() const
   { return reinterpret_cast<CTK_ALPRWizardDoc*>(m_pDocument); }
#endif

