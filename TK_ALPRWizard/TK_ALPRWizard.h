// TK_ALPRWizard.h : TK_ALPRWizard ���� ���α׷��� ���� �� ��� ����
//
#pragma once
#include "WizardHelper.h"

#ifndef __AFXWIN_H__
	#error "PCH�� ���� �� ������ �����ϱ� ���� 'stdafx.h'�� �����մϴ�."
#endif

#include "resource.h"       // �� ��ȣ�Դϴ�.


// CTK_ALPRWizardApp:
// �� Ŭ������ ������ ���ؼ��� TK_ALPRWizard.cpp�� �����Ͻʽÿ�.
//

class CTK_ALPRWizardApp : public CWinApp
{
public:
	CTK_ALPRWizardApp();
	~CTK_ALPRWizardApp();

public:
	
	int GetViewXYPos( int *XPos, int * YPos );
	int SetViewXYPos( int nXPos, int  nYPos );

	void CreateNewImageDoc( TCHAR * ptrtcFileName, IMAGE_DATA_t * ptrImageData, CVRECT_t * ptrRect ,pLPRCHAR ptrLPRChars );

	void SetFirstDoc( CDocument * ptrDoc );
	CDocument * GetFirstDoc( void );	
	
	void SetActiveDoc( CDocument * ptrDoc );	
	CDocument * GetActiveDoc( void );	
	void CloseAllView( void );

	void SetDlgHDC( HDC DlgDC );
	HDC GetDlgHDC( void );

	IMAGE_DATA_t * GetCompareImage1( void );
	IMAGE_DATA_t * GetCompareImage2( void );
	
	BOOL GetCreateWindow( void );
	void SetCreateWindow( BOOL bCreate );

	IMAGE_DATA_t * m_ptrImageData;
	IMAGE_DATA_t m_CompareImage1;
	IMAGE_DATA_t m_CompareImage2;
	CVRECT_t * m_ptrRect;
	LPR_CHAR_t * m_ptrLPRChars;

	CMultiDocTemplate* m_pImageDocTemplate;

	TCHAR * m_ptrtcFileName;
	BOOL m_bCreateWindow;
private:

	CDocument * m_ptrActiveDoc;
	CDocument *  m_ptrFirstDoc;
	HDC m_DlgDC;


	int m_nViewXPos;
	int m_nViewYPos;
	
public:
	virtual BOOL InitInstance();

// �����Դϴ�.
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
	afx_msg void OnCloseAllView();
};

extern CTK_ALPRWizardApp theApp;

/////////////////////////////////////////////////////////////////////////////
// ���� �Լ� ����

