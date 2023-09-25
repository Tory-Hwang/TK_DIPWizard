
#pragma once

#include "WizardHelper.h"
#include "IBridge.h"
#include "../include/tk_conf.h"
#include "afxwin.h"

// CDIPDlg 대화 상자입니다.

class CDIPDlg : public CDialog
{
	DECLARE_DYNAMIC(CDIPDlg)

public:
	void Set_IALPRDlg( CIBridge * ptrInterface );

public:
	CDIPDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CDIPDlg();

private:
	
	CDocument * GetFirstDocument( void );
	CDocument * GetActiveDocument( void );


	/* Image Processing Function*/
	void init_DIP_Dlg( void );

	BOOL CheckValidDoc( void * ptrDoc  );
	void ConvertGrayscale( void );
	void Histogram_Equal( void );
	void Histogram( void );
	void Binarization( void );
	void Binarization_Iterate( void );
	void Binarization_Adaptive( void );
	void Binarization_Otsu( void );
	void Binarization_ITG_Adaptive( void );

	void Brightness( void );
	void BitPlane( void );
	void Gaussian( void );

	void SobelEdge( void );
	void Prewitt( void );
	void Roberts( void );

	void CannyEdge( void );

	void Labeling( void );

	void Erosion( void );
	void SubImage( void * ptrDoc1, void *ptrDoc2 );
	void Median( void );
	void Beam( void );

	void Mean( void );
	void WeightMean( void );

	void Contrast( void );
	void Diffusion( void );
	void MMSE( void );
	void HoughTransform( void );
	void Rotation( void );
	void DoG( void );
	void Bilinear_Normal( void );
	void UnSharp( void );

	void Dilation( void );
	void Opening( void );
	void Closing( void );

	CIBridge * m_ptrALPRDlg;


// 대화 상자 데이터입니다.
	enum { IDD = IDD_DIP_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButton7();
	afx_msg void OnBnClickedButton49();
	afx_msg void OnBnClickedButton11();
	afx_msg void OnBnClickedButton6();
	afx_msg void OnBnClickedButton15();
	afx_msg void OnBnClickedButton36();
	afx_msg void OnBnClickedButton3();
	afx_msg void OnBnClickedButton5();
	afx_msg void OnBnClickedButton9();
	afx_msg void OnBnClickedButton18();
	afx_msg void OnBnClickedButton12();
	afx_msg void OnBnClickedButton20();
	afx_msg void OnBnClickedButton2();
	afx_msg void OnBnClickedButton22();
	afx_msg void OnBnClickedButton23();
	afx_msg void OnBnClickedButton16();
	afx_msg void OnBnClickedButton10();
	afx_msg void OnBnClickedButton19();
	afx_msg void OnBnClickedButton37();
	afx_msg void OnBnClickedButton21();
	afx_msg void OnBnClickedButton17();
	afx_msg void OnBnClickedButton45();
	afx_msg void OnBnClickedButton48();
	afx_msg void OnBnClickedButton46();
	afx_msg void OnBnClickedButton4();
	afx_msg void OnBnClickedButton8();
	afx_msg void OnBnClickedButton38();
	afx_msg void OnBnClickedButton47();
	int m_ctl_binary_th;
	int m_ctl_bright;
	int m_ctl_itg_size;
	double m_ctrl_d_rotation;
	double m_ctrl_DoGSig1;
	double m_ctrl_DoGSig2;
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	int m_ctl_unsharp_mask;
	afx_msg void OnBnClickedButton50();
	afx_msg void OnBnClickedButton13();
	afx_msg void OnBnClickedButton51();
	afx_msg void OnBnClickedButton52();
};
