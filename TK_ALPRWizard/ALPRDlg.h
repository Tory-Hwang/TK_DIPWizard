#pragma once
#include "afxcmn.h"
#include "afxwin.h"
#include "WizardHelper.h"
#include "CONFDlg.h"
#include "DIPDlg.h"
#include "IBridge.h"

// CALPRDlg 대화 상자입니다.

#define	MAX_OCR_CTRL_CNT 16

typedef struct AUTO_INDEX_
{
	int nTotalIndex;
	int nCurIndex;
}AUTO_INDEX_t;

typedef struct OCR_CTRL_
{
	CComboBox * ptrOCRCombo[ MAX_OCR_CTRL_CNT ];
	CButton * ptrOCRButton[ MAX_OCR_CTRL_CNT ];
	CWnd * ptrOCRComboWnd[ MAX_OCR_CTRL_CNT ];
	CWnd * ptrOCRButtonWnd[ MAX_OCR_CTRL_CNT ];
}OCR_CTRL_t;

class CALPRDlg : public CDialog , public CIBridge
{
	DECLARE_DYNAMIC(CALPRDlg)

public:
	CALPRDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CALPRDlg();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_ALPR_DIALOG };

/* ======================================================================
Interface Bridge 
IBridge.h
========================================================================*/
public: 
		virtual int NotifySave();
		virtual int NotifyClose(  UINT uDLGID  );
	
public:
	int Init_Dlg( void );
	int Init_OCR( void );

	void DisplayActiveView( TCHAR * ptrtcFileName, IMAGE_DATA_t * ptrImageData );
	void DisplayProcessingTime( TCHAR * ptrtcFileName, TCHAR * ptrPlateNumber, DWORD nTime );
	void DisplayCharNumber( int nCharCnt,unsigned short * ptrPlateNumber  );
	void DisplayStudyState( int nStudyCnt, double dErr );


private:	
	
	void DisplayFileList( void );

	CDocument * GetFirstDocument( void );
	CDocument * GetActiveDocument( void );

	void DisplayImageData( CString & strName, IMAGE_DATA_t * ptrImageData, int Area );
	void ClearImageData( void );
	
	/* LPR Process */
	void LPRProcess( void );

	/* Image Processing Function*/
	BOOL CheckValidDoc( void * ptrDoc  );
	void SaveCurrentImage( void );
	void FileListClickEvt( int nListIndex );

	void CheckLPR( void );
	void AutoALPR( void );
	void SetTotalIndex( void );
	void NextALPR( void );
	void ReloadConf( void );
	
	void SetCreateWindow( BOOL bCreate );
	int SaveOCRFeature( int nPos, int nOCRCode );
	int BackupOCRFeature( void );

	int RestudyBPNN( void );
	int GetFeatureCnt( void );
	int NNForceSave( void );

	void KeySelectFileList( void );
	void GetImagePath( void );
	void SetVersion( void );
	void SetSameOCR();

	void SetBaseLearningChar( CString &strBaseChar );
	int IsBaseCharStudy( void );

	static DWORD OCRStudy_Thread( LPVOID );
	

	TCHAR m_tcCurPath[ MAX_PATH ];
	TCHAR m_tcBaseChar[ MAX_PATH ];
	FILE_MANAGER_t m_FileManager;
	CWizardHelper m_WizardHelper;
	AUTO_INDEX_t m_AutoIndex;
	BOOL m_bAutoRuning;

	OCR_CTRL_t m_OCR_Ctrl;
	CFont LargeFont;
	HANDLE m_hTh;
	DWORD m_dwTh;
	
	/* Dialog */
	CCONFDlg *m_ptrConfDlg;
	BOOL m_bOpenedConfDlg;

	CDIPDlg *m_ptrDipDlg;
	BOOL m_bOpenedDipDlg;
	BOOL m_bAutoFeatureSave;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	CListCtrl m_CtlFileList;
	CEdit m_Ctl_Img_Size;
	CEdit m_Ctl_Img_BitCnt;
	CEdit m_Ctl_Img_Width;
	CEdit m_Ctl_Img_Height;
	afx_msg void OnBnClickedBtnFile();
	afx_msg void OnNMClickFileList(NMHDR *pNMHDR, LRESULT *pResult);
	
	//CEdit m_Ctl_Active_View;
	CEdit m_Ctl_ProcessTime;
	afx_msg void OnBnClickedBtnAllClose();	
	CButton m_Ctl_Chek_AutoView;	
	afx_msg void OnBnClickedCheck1();
	CButton m_CtlAuto;
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	CButton m_CtlReadFile;
	CListCtrl m_CtlLog_List;
	afx_msg void OnBnClickedBtnClear();	
	
	afx_msg void OnBnClickedCheck2();
	CButton m_CtlCheckCreateWindow;	
	afx_msg void OnPaint();
	CComboBox m_CtlOCR1;
	CComboBox m_CtlOCR2;
	CComboBox m_CtlOCR3;
	CComboBox m_CtlOCR4;
	CComboBox m_CtlOCR5;
	CComboBox m_CtlOCR6;
	CComboBox m_CtlOCR7;
	CComboBox m_CtlOCR8;
	CComboBox m_CtlOCR9;
	CComboBox m_CtlOCR10;
	CButton m_CtlSave1;
	CButton m_CtlSave2;
	CButton m_CtlSave3;
	CButton m_CtlSave4;
	CButton m_CtlSave5;
	CButton m_CtlSave6;
	CButton m_CtlSave7;
	CButton m_CtlSave8;
	CButton m_CtlSave9;
	CButton m_CtlSave10;
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnBnClickedButton25();
	afx_msg void OnBnClickedButton26();
	afx_msg void OnBnClickedButton27();
	afx_msg void OnBnClickedButton28();
	afx_msg void OnBnClickedButton29();
	afx_msg void OnBnClickedButton30();
	afx_msg void OnBnClickedButton31();
	afx_msg void OnBnClickedButton32();
	afx_msg void OnBnClickedButton33();
	afx_msg void OnBnClickedButton34();
	CStatic m_CtlPlateNumber;
	afx_msg void OnBnClickedButton24();
	afx_msg void OnBnClickedBtnstudy();
	afx_msg void OnBnClickedBtnBackup();
	CStatic m_Ctl_StudyCnt;
	CStatic m_Ctl_Err_Rate;
	CStatic m_Ctl_FeatureCnt;
	afx_msg void OnBnClickedButton35();
	afx_msg void OnBnClickedBtnCamera();	
	
	afx_msg void OnLvnKeydownFileList(NMHDR *pNMHDR, LRESULT *pResult);
	CEdit m_Ctrl_SaveName;
	afx_msg void OnBnClickedBtImageSave();
	afx_msg void OnBnClickedBtnShowConf();
	afx_msg void OnClose();
	afx_msg void OnBnClickedChkAutoView();
	CComboBox m_CtlOCR11;
	CComboBox m_CtlOCR12;
	CComboBox m_CtlOCR13;
	CButton m_CtlSave11;
	CButton m_CtlSave12;
	CButton m_CtlSave13;
	afx_msg void OnBnClickedButton39();
	afx_msg void OnBnClickedButton40();
	afx_msg void OnBnClickedButton41();
	CComboBox m_CtlOCR14;
	CComboBox m_CtlOCR15;
	CComboBox m_CtlOCR16;
	CButton m_CtlSave14;
	CButton m_CtlSave15;
	CButton m_CtlSave16;
	afx_msg void OnBnClickedButton42();
	afx_msg void OnBnClickedButton43();
	afx_msg void OnBnClickedButton44();
	CButton m_chk_raw_save;
	CButton m_chk_all_save;
	CButton m_chk_same_ocr;

	CStatic m_ctl_img_path;	
	afx_msg void OnBnClickedBtnShowDip();
	afx_msg void OnEnChangeEImageBitcnt();
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedGetPath();
	afx_msg void OnBnClickedBtnfeature();

	CStatic m_ctl_APP_VER;	
	CStatic m_ctl_LPR_VER;	
	CStatic m_ctl_OCR_VER;	
	CStatic m_ctl_DIP_VER;	

	afx_msg void OnBnClickedCheck3();
	afx_msg void OnBnClickedSameOCR();
	afx_msg void OnBnClickedBtnNnSave();
};
