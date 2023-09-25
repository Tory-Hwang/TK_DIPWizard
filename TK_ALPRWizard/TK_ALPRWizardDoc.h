// TK_ALPRWizardDoc.h : CTK_ALPRWizardDoc 클래스의 인터페이스
//


#pragma once
#include "ALPRDlg.h"
#include "WizardHelper.h"
#include "../include/TK_TYPE.h"
#include "../include/TK_CONF.h"
#include "WINConf.h"

class CTK_ALPRWizardDoc : public CDocument
{
protected: // serialization에서만 만들어집니다.
	CTK_ALPRWizardDoc();
	DECLARE_DYNCREATE(CTK_ALPRWizardDoc)

public:
	
	void Doc_DrawImage( HDC hdc  );
	void Doc_AllViewClose( void );

	void Doc_LPR( TCHAR * ptrtcFileName,  IMAGE_DATA_t * ptrImageData, int Area );
	void Doc_DisplaySrcImage( TCHAR * ptrtcFileName,  IMAGE_DATA_t * ptrImageData, int Area);
	void Doc_ConvertGrayscale( void );
	void Doc_Histogram( void );
	void Doc_Histogram_Equal( void );
	void Doc_Binarization( int nTh );
	void Doc_Binarization_Iterate( void );
	void Doc_Binarization_Adaptive( void );
	void Doc_Binarization_Otsu( void );
	void Doc_Binarization_ITG_Adaptive( int nsize );

	void Doc_Brightness( int nBright );
	void Doc_BitPlane( void );

	void Doc_Gaussian( void );
	void Doc_Sobel( void );
	void Doc_Prewitt( void );
	void Doc_Roberts( void );

	void Doc_Canny( void );
	void Doc_DeNoiseLabel( void );

	void Doc_Erosion( void );
	void Doc_SubImage( void * ptrDoc1, void *ptrDoc2  );
	void Doc_Median( void );
	void Doc_Mean( void );
	void Doc_WeightMean( void );
	void Doc_Contrast( void );


	void Doc_Beam( void );
	void Doc_Diffusion( void );
	void Doc_MMSE( void );
	void Doc_HoughTransform( void );
	void Doc_Rotation( double dAngle );
	void Doc_DoG( double dSigma1 ,double dSigma2 );
	void Doc_Bilinear_Normal( void );
	void Doc_Unsharp( int nMask );

	void Doc_Dilation( void );
	void Doc_Opening( void );
	void Doc_Closing( void );

	int Doc_SaveCurrentImage( TCHAR * ptrtcFileName, TCHAR * ptrtcCurPath,TCHAR * ptrtcOrgFileName,int nRawSave);
	void Doc_DrawCharsImageDlg( HDC hdc  );
	int Doc_init( void );
	char * Doc_GetImagePath( void );


	BOOL Doc_Load_TKLIB( void );
	BOOL Doc_Free_TKLIB( void );
	
	void Doc_SetCreateNewWindow( BOOL bCreate );	
	int Doc_SaveOCRFeature( int nPos, int nOCRCode );
	

	int Doc_InitOCRData( void );
	int Doc_RestudyBPNN( void );
	void Doc_ReloadConf( void );
	void Doc_LoadlocalConf( void );
	
	int Doc_GetFeatureCnt( void );
	int Doc_BackupOCRFeature( void );
	int Doc_NNForceSave( void );
	
	pVERSION Doc_GetVersion( void );
	int  Doc_IsBaseStudy( void );

	CALPRDlg * m_pALPRDlg;
	IMAGE_DATA_t m_ImageData;
	CVRECT_t m_Rect[ MAX_LPR_AREA_CNT ];
	LPR_CHAR_t m_LPRChars;
	TCHAR m_tcFileName[ MAX_PATH ];
	HDC m_hDC;
	CWINConf m_AppClass;
	VERSION_t m_Version;

private:

	int Internal_DIP( short uMsg, pIMAGE_DATA ptrImageScr1, pIMAGE_DATA ptrImageScr2, pIMAGE_DATA ptrImageDest, void * ptrParam,int nChkBit );
	void ShowTickTime( TCHAR * ptrFileName, DWORD wStartTick, DWORD wEndTick );
	void Internal_NewImageDoc( TCHAR * ptrtcFileName, IMAGE_DATA_t * ptrImageData, CVRECT_t * ptrRect, pLPRCHAR ptrLPRChars );
	void ShowCharNumber( void);

	CWizardHelper m_WizardHelper;
	CTK_ALPRWizardApp* m_pApp;	
	pLPR_CONF m_ptrLPRConf;
	pOCR_CONF m_ptrOCRConf;
	pDIP_CONF m_ptrDIPConf;
	pAPP_CONF m_ptrAPPConf;



	static void OCRNotifyCallBack(int nHitCnt, double dErr );
	
public:
	

public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);

// 구현입니다.
public:
	virtual ~CTK_ALPRWizardDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// 생성된 메시지 맵 함수
protected:
	DECLARE_MESSAGE_MAP()

public:
	
};


