/*================================================================================
Copyright (c) 2011 Tory Hwang 
Modify : Tory in 2013 .01

Mail : tory45@empal.com
*=================================================================================*/



// TK_ALPRWizardDoc.cpp : CTK_ALPRWizardDoc 클래스의 구현
//
#include<windows.h>
#include "Shobjidl.h"

#include "stdafx.h"
#include "TK_ALPRWizard.h"

#include "ALPRWizard_Def.h"
#include "TK_ALPRWizardDoc.h"

#include "../include/TK_LIB_API.h"

/*==================================================================================
DIP LIB Version
===================================================================================*/
#define APP_VER_1ST		0x00
#define APP_VER_2ND		0x01
/*==================================================================================*/


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CTK_ALPRWizardDoc

IMPLEMENT_DYNCREATE(CTK_ALPRWizardDoc, CDocument)

BEGIN_MESSAGE_MAP(CTK_ALPRWizardDoc, CDocument)
END_MESSAGE_MAP()

static CTK_ALPRWizardApp* m_TmpApp = NULL;
CTK_ALPRWizardDoc::CTK_ALPRWizardDoc()
{		
	m_pALPRDlg = NULL;
	m_pApp = NULL;

	memset( &m_ImageData, 0, sizeof( IMAGE_DATA_t ));
	memset( &m_LPRChars, 0, sizeof( LPR_CHAR_t ));
	memset( m_tcFileName, 0, sizeof( m_tcFileName ));
	memset( &m_Rect, 0, sizeof( m_Rect ));
	memset( &m_Version, 0, sizeof( m_Version ));

	m_pApp = (CTK_ALPRWizardApp*)AfxGetApp();
	m_TmpApp = m_pApp;
}

/*======================================================================
DOC Release
========================================================================*/
CTK_ALPRWizardDoc::~CTK_ALPRWizardDoc()
{	
	CTK_ALPRWizardDoc * ptrActiveDoc = NULL;	

	ptrActiveDoc =  ( CTK_ALPRWizardDoc *  )m_pApp->GetActiveDoc();
	if ( ptrActiveDoc == this )
	{
		m_pApp->SetActiveDoc( NULL );
	}

	if (m_pALPRDlg != NULL )
	{
		delete m_pALPRDlg ;
		m_pALPRDlg = NULL;
	}
	
	/* Image Buffer Release */
	m_WizardHelper.FreeImageAllocData( &m_ImageData );
	for( int i = 0 ; i < MAX_LPR_CHAR_CNT ; i++ )
	{
		m_WizardHelper.FreeImageAllocData( &m_LPRChars.Chars[ i ].ImageData );
	}

}

/*======================================================================
신경망 학습 
========================================================================*/
void CTK_ALPRWizardDoc::OCRNotifyCallBack(int nHitCnt, double dErr )
{
	CTK_ALPRWizardDoc * ptrFirstDoc;

	ptrFirstDoc = ( CTK_ALPRWizardDoc * )m_TmpApp->GetFirstDoc();

	if ( ptrFirstDoc != NULL )
	{	
		ptrFirstDoc->m_pALPRDlg->DisplayStudyState( nHitCnt, dErr );
	}

}

/* ==========================================================================
DLL Load( OCR, DIP, LPR ) 및 초기화

DIP : Image Process 관련 DLL
LPR : 차량 번호판 적출 관련 DLL
OCR : 문자 인식( 신경망 ) 관련 DLL
============================================================================*/
BOOL CTK_ALPRWizardDoc::Doc_Load_TKLIB( void )
{
	BOOL bResult = TRUE;
	LPR_CONF_t LPR_Conf;
	CString strMsg;
	LIB_INIT_DATA_t lib_init_data;

	memset( &LPR_Conf, 0, sizeof( LPR_CONF_t));

	if ( Load_LPR_LIB( TK_LPR_LIB_NAME ) == FALSE)
	{
		bResult = FALSE;
		strMsg.Format(L"%s", TK_LPR_LIB_NAME);
	}

	if ( bResult )
	{
		if (Load_DIP_LIB( TK_DIP_LIB_NAME ) == FALSE)
		{	
			bResult = FALSE;
			strMsg.Format(L"%s", TK_DIP_LIB_NAME);
		}
	}

	if ( bResult )
	{
		if ( Load_OCR_LIB( TK_OCR_LIB_NAME ) == FALSE)
		{
			bResult = FALSE;
			strMsg.Format(L"%s", TK_OCR_LIB_NAME);
		}
	}

	if ( bResult == FALSE )
	{		
		AfxMessageBox(IDS_ERR_ERR_LOAD_LIB);
		AfxGetMainWnd()->PostMessage(WM_CLOSE, 0, 0);
	}
	else
	{		
		memset( &lib_init_data, 0, sizeof(LIB_INIT_DATA_t));
		memset( &m_Version, 0, sizeof( m_Version));

		m_Version.APP_VER[ 0 ] = APP_VER_1ST;
		m_Version.APP_VER[ 1 ] = APP_VER_2ND;

		m_WizardHelper.GetCurrentPathChar( lib_init_data.szwinpath );
		SetWinCurPath( lib_init_data.szwinpath );
		
		OCR_Initialize ( &lib_init_data );
		memcpy( m_Version.OCR_VER, lib_init_data.Version, VER_SIZE);

		DIP_Initialize( &lib_init_data );
		memcpy( m_Version.DIP_VER, lib_init_data.Version, VER_SIZE);

		LPR_Initialize( &lib_init_data );
		memcpy( m_Version.LPR_VER, lib_init_data.Version, VER_SIZE);
	}
	
	Doc_LoadlocalConf();
	return bResult;
}

pVERSION CTK_ALPRWizardDoc::Doc_GetVersion( void )
{
	return &m_Version;
}

int  CTK_ALPRWizardDoc::Doc_IsBaseStudy( void )
{
	return m_ptrLPRConf->LPR_DBG_RUN_BASIC_STUDY ;
}

/* ==========================================================================
DLL Load( OCR, DIP, LPR ) 해제
============================================================================*/
BOOL CTK_ALPRWizardDoc::Doc_Free_TKLIB( void )
{
	Free_DIP_LIB();
	Free_OCR_LIB();
	Free_LPR_LIB();

	return TRUE;
}

/* ==========================================================================
LPR 처리 시간 Display ( 단위 : ms ) 
============================================================================*/
void CTK_ALPRWizardDoc::ShowTickTime( TCHAR * ptrFileName, DWORD wStartTick, DWORD wEndTick )
{
	CTK_ALPRWizardDoc * ptrFirstDoc;

	ptrFirstDoc = ( CTK_ALPRWizardDoc * )m_pApp->GetFirstDoc();

	if ( ptrFirstDoc != NULL )
	{	
		ptrFirstDoc->m_pALPRDlg->DisplayProcessingTime( ptrFileName, NULL, wEndTick - wStartTick );
	}
}

/* ==========================================================================
차량 번호판 Display
============================================================================*/
void CTK_ALPRWizardDoc::ShowCharNumber( void)
{
	CTK_ALPRWizardDoc * ptrFirstDoc;

	ptrFirstDoc = ( CTK_ALPRWizardDoc * )m_pApp->GetFirstDoc();

	if ( ptrFirstDoc != NULL )
	{	
		ptrFirstDoc->m_pALPRDlg->DisplayCharNumber( m_LPRChars.nCharCnt, m_LPRChars.nPlateNumber );
	}
}

/* ==========================================================================
새로운 Image 파일 처리
Multi Doc가 체크 되어 있을 경우 새로운 Doc / Viewr 가 생성되고
그렇지 않을 경우 First DOC에 기존 이미지 정보를 삭제하고 새로운 이미지로 
갱신한다. 
============================================================================*/
void CTK_ALPRWizardDoc::Internal_NewImageDoc( TCHAR * ptrtcFileName, IMAGE_DATA_t * ptrImageData, 
											   CVRECT_t * ptrRect, pLPRCHAR ptrLPRChars )
{
	int i;
	
	if ( m_pApp->GetCreateWindow() == TRUE )
	{
		/* Multi Doc가 체크 되어 있을 경우 새로운 DOC / Viewer 를 생성 한다. */
		m_pApp->CreateNewImageDoc( ptrtcFileName, ptrImageData, ptrRect, ptrLPRChars );
	}
	else
	{
		/* First DOC에 새로운 이미지를 복사 하고 Display 한다. */

		POSITION ptr;

		ptr =GetFirstViewPosition();
		CView * ptrView = GetNextView( ptr );

		/* Copy FileName */
		wcscpy( m_tcFileName, ptrtcFileName);

		/* Copy m_ImageData */
		m_WizardHelper.FreeImageAllocData( &m_ImageData );
		memset( &m_ImageData, 0, sizeof( IMAGE_DATA_t ));		
		m_WizardHelper.CopyImageData( &m_ImageData, ptrImageData, TRUE );
		m_pApp->m_ptrImageData = NULL;

		/* Copy PlateRect */
		if ( ptrRect != NULL )
		{
			memcpy( &m_Rect, ptrRect, sizeof( CVRECT_t));
		}		
		m_pApp->m_ptrRect = NULL;
		

		/* Copy Char  */
		for( i = 0 ; i < MAX_LPR_CHAR_CNT ; i++ )
		{
			m_WizardHelper.FreeImageAllocData( &m_LPRChars.Chars[ i ].ImageData );
		}
		memset( &m_LPRChars, 0, sizeof( LPR_CHAR_t ));
		if ( ptrLPRChars != NULL )
		{	
			m_LPRChars.nCharCnt = ptrLPRChars->nCharCnt;
			m_LPRChars.nPlateNumerLen = ptrLPRChars->nPlateNumerLen;
			
			if ( m_LPRChars.nPlateNumerLen > 0 )
			{
				memcpy( m_LPRChars.nPlateNumber, ptrLPRChars->nPlateNumber, ptrLPRChars->nPlateNumerLen * sizeof( unsigned short));
			}

			if ( m_LPRChars.nCharCnt > 0 )
			{				
				for( i = 0 ; i < MAX_LPR_CHAR_CNT ; i++ )
				{	
					m_WizardHelper.CopyImageData( &m_LPRChars.Chars[ i ].ImageData,  &ptrLPRChars->Chars[ i ].ImageData , TRUE);
					memcpy( &m_LPRChars.Chars[ i ].ChRect , &ptrLPRChars->Chars[ i ].ChRect, sizeof( CVRECT_t) );
				}
			}
			
		}
		m_pApp->m_ptrLPRChars = NULL;

		/* ptrView Char  */
		if ( ptrView != NULL )
		{
			ptrView->Invalidate( TRUE );
		}
	}
}

/* ==========================================================================
이미지 중 차량 번호 정보만을 Dlsplay 한다. 
============================================================================*/
void CTK_ALPRWizardDoc::Doc_DrawCharsImageDlg( HDC hdc  )
{
	int i;
	int nPreCharX = 0;
	int nExtDlgX =0;
	int nExtDlgY =0;

	int nCenterExtx = 0;	

	if ( m_pApp->GetCreateWindow() == FALSE )
	{
		nExtDlgX = DLG_CHAR_X;		
		nExtDlgY = DLG_CHAR_Y;
	}

	if ( m_LPRChars.nCharCnt > 0 )
	{
		for( i = 0 ; i < m_LPRChars.nCharCnt ; i++ )
		{
			m_WizardHelper.DrawWin32( TRUE, hdc, &m_LPRChars.Chars[ i ].ImageData, nCenterExtx + nExtDlgX + nPreCharX  , nExtDlgY );
			nPreCharX += ( m_LPRChars.Chars[ i ].ImageData.nW + DLG_CHAR_INTERVAL -  i/3);
		}
	}	
}

/* ==========================================================================
이미지 전체를 Display 하고 차량 번호판 위치가 존재할 경우 해당 위치에 Line을 그린다. 
============================================================================*/
void CTK_ALPRWizardDoc::Doc_DrawImage( HDC hdc  )
{		
	int nExtX =0;
	int nExtY =0;
	static int nFirst = 1;

	if ( m_pApp->GetCreateWindow() == FALSE )
	{
		nExtX = DLG_WIDTH;		
		nExtY = 3;
	}

	memcpy( &m_hDC, &hdc, sizeof( HDC ));
	m_WizardHelper.DrawWin32( FALSE, hdc, &m_ImageData, nExtX, nExtY);
	m_WizardHelper.DrawRectLine( hdc, &m_Rect[ 0 ],m_ptrLPRConf->LPR_DBG_SHOW_LAST_PLATE_BOX, nExtX, nExtY);	
	m_WizardHelper.DrawCharRectLine( hdc, m_LPRChars.nCharCnt, &m_Rect[ 0 ], &m_LPRChars, m_ptrLPRConf->LPR_DBG_SHOW_LAST_CHAR_BOX, nExtX, nExtY );
	
	if ( nFirst == 0 )
	{
		/* View가 아닌 Dlg에 차량 번호 이미지를 직접 그린다. */
		ShowCharNumber();
	}

	nFirst = 0;
}

/* ==========================================================================
모든 Viewe를 닫는다. 
============================================================================*/
void CTK_ALPRWizardDoc::Doc_AllViewClose( void )
{
	m_pApp->CloseAllView();
}

/* ==========================================================================
Dlg에서 선택한 이미지를 넘겨 받아 DOC와 APP에 저장하고 
Display 한다. 
============================================================================*/
void CTK_ALPRWizardDoc::Doc_DisplaySrcImage( TCHAR * ptrtcFileName, IMAGE_DATA_t * ptrImageData,  int Area )
{
	( void )Area;
	if ( m_pApp != NULL && ptrImageData == NULL || ptrImageData->ptrSrcImage2D == NULL )
		return ;	

	Internal_NewImageDoc( ptrtcFileName, ptrImageData, NULL , NULL );
}

/* ==========================================================================
Dlg에서 선택한 이미지를 넘겨 받아 DOC와 APP에 저장하고 
ALPR 처리를 수행한다.  
============================================================================*/
void CTK_ALPRWizardDoc::Doc_LPR( TCHAR * ptrtcFileName,  IMAGE_DATA_t * ptrImageData , int Area  )
{
	LPR_DATA_t LPRData;
	CAMERA_DATA_t CameraData;
	int nLineByte=0;
	int i, j;
	DWORD wStartTick, wEndTick;
	IMAGE_DATA_t OrgImageData, *ptrCharImage, * ptrTempImageData= NULL;

	memset( &CameraData, 0, sizeof( CAMERA_DATA_t ));
	memset( &LPRData, 0, sizeof( LPR_DATA_t ));
	memset( &OrgImageData, 0, sizeof( IMAGE_DATA_t ));		

	ptrTempImageData = ( pIMAGE_DATA)&LPRData.LPRResult[ 0 ].TempImdateData;

	/* 이미지 데이터 복사 */
	m_WizardHelper.CopyImageData( &OrgImageData, ptrImageData , TRUE);
	m_WizardHelper.CopyImageData( ptrTempImageData, ptrImageData , FALSE);	
	
	/* First DOC 문자 데이터 메모리 해제 */
	for( i = 0 ; i < MAX_LPR_CHAR_CNT ; i++ )
	{
		m_WizardHelper.FreeImageAllocData( &m_LPRChars.Chars[ i ].ImageData );
	}	

	/* First DOC 문자 , 위치 정보 초기화 */
	memset( &m_LPRChars, 0, sizeof( LPR_CHAR_t ));
	memset( &m_Rect[ 0 ], 0, sizeof( CVRECT_t ));

	/* 번호판 개별 문자 Display buffer */
	for( i = 0 ; i < MAX_LPR_CHAR_CNT ; i++ )
	{
		ptrCharImage = &LPRData.LPRResult[ 0 ].LPRChars.Chars[ i ].ImageData;
		m_WizardHelper.CopyImageDataWithEmpty(ptrCharImage,8, m_ptrOCRConf->OCR_CHAR_H, m_ptrOCRConf->OCR_CHAR_W );
	}

	CameraData.nArea = Area;
	CameraData.nBitCount = ptrImageData->nBitCount;
	CameraData.nH = ptrImageData->nH;
	CameraData.nW = ptrImageData->nW;
	CameraData.nImageSize = ptrImageData->nImageSize;
	nLineByte = m_WizardHelper.GetCalBmpWidth(ptrImageData->nW, ptrImageData->nBitCount );

	/* Camera Image Create [ 1차원 메모리 ]*/
	CameraData.ptrSrcImage1D = new BYTE[ ptrImageData->nH * nLineByte ];
	if ( CameraData.ptrSrcImage1D == NULL )
	{
		return;
	}

	/*Camera Image Copy ( Matrix --> Array )*/
	for( i = 0 ; i < ptrImageData->nH ; i++ )
	{
		for( j = 0 ; j < nLineByte ; j++)
		{
			CameraData.ptrSrcImage1D[ ( i * nLineByte ) + j ] = ptrImageData->ptrSrcImage2D[ i ][ j ];
		}
	}

	LPRData.ptrCameraData = &CameraData;

	
	wStartTick= GetTickCount();

	/* LPR 처리( DLL ) */
	if ( LPR_LPRProcess( &LPRData) == ERR_SUCCESS )
	{
		wEndTick= GetTickCount();

#if 1
		Internal_NewImageDoc( ptrtcFileName, ptrTempImageData, &LPRData.LPRResult[ 0 ].PlateRect[ 0 ], &LPRData.LPRResult[ 0 ].LPRChars );
#else
		Internal_NewImageDoc( ptrtcFileName, &OrgImageData, &LPRData.LPRResult[ 0 ].PlateRect[ 0 ] , &LPRData.LPRResult[ 0 ].LPRChars );
#endif

		ShowTickTime(ptrtcFileName, wStartTick, wEndTick );
	}


	/* 메모리 해제 */
	if ( CameraData.ptrSrcImage1D != NULL )
		delete[] CameraData.ptrSrcImage1D;
	
	m_WizardHelper.FreeImageAllocData( &OrgImageData );
	m_WizardHelper.FreeImageAllocData( ptrTempImageData );

	for( i = 0 ; i < MAX_LPR_CHAR_CNT ; i++ )
	{
		ptrCharImage = &LPRData.LPRResult[ 0 ].LPRChars.Chars[ i ].ImageData;
		if (ptrCharImage != NULL )
		{
			m_WizardHelper.FreeImageAllocDataWithSize( ptrCharImage,  m_ptrOCRConf->OCR_CHAR_H,  m_ptrOCRConf->OCR_CHAR_W);
		}
	}

}

/* ==========================================================================
Dlg에서 선택한 이미지의 각종 필터 처리 함수  
============================================================================*/
int CTK_ALPRWizardDoc::Internal_DIP( short uMsg, pIMAGE_DATA ptrImageScr1, pIMAGE_DATA ptrImageScr2, pIMAGE_DATA ptrImageDest, void * ptrParam, int nChkBit )
{
	DIP_DATA_t DIP_Data;
	DWORD wStartTick, wEndTick;
	HISTOGRAM_PARAM_t Histo_Param;
	int nErrCode = ERR_SUCCESS;

	memset(&DIP_Data, 0, sizeof( DIP_DATA_t ));
	memset(&Histo_Param, 0, sizeof( HISTOGRAM_PARAM_t ));

	if ( m_ImageData.nBitCount != nChkBit )
	{
		AfxMessageBox(IDS_ERR_NOT_8BIT);
		return ERR_INVALID_DATA;
	}

	/* DIP ID 등록 */
	DIP_Data.uDIPMsg = uMsg;

	/* 현재 DIP는 최대 3개의 원본 Image Data를 이용하여 
	   이미지 프로세싱 할 수 있다. */

	if ( ptrImageScr1 != NULL )
		DIP_Data.ptrImageDataSrc1 = ptrImageScr1;

	if ( ptrImageScr2 != NULL )
		DIP_Data.ptrImageDataSrc2 = ptrImageScr2;

	if ( ptrImageDest != NULL )
		DIP_Data.ptrImageDataDest = ptrImageDest;

	if ( ptrParam != NULL )
		DIP_Data.ptrInputParam = ptrParam;


	wStartTick= GetTickCount();
	/* DIP 처리( DLL ) */
	nErrCode = DIP_ImageProcess( &DIP_Data );
	wEndTick= GetTickCount();
	
	if  ( nErrCode == ERR_SUCCESS  )
	{		
		if ( DIP_Data.ptrImageDataDest != NULL )
		{
			/* 변환 처리된 이미지 결과 Display */
			Internal_NewImageDoc( m_tcFileName, DIP_Data.ptrImageDataDest, NULL , NULL);
		}
		else
		{
			/* 원본 이미지 Display */
			Internal_NewImageDoc( m_tcFileName, DIP_Data.ptrImageDataSrc1, NULL , NULL);			
		}

		ShowTickTime( m_tcFileName, wStartTick, wEndTick );
	}


	/* Image Buffer 해제 */
	if ( ptrImageScr1 != NULL )
		DIP_FreeAllocData( DIP_Data.ptrImageDataSrc1 );

	if ( ptrImageScr2 != NULL )
		DIP_FreeAllocData( DIP_Data.ptrImageDataSrc2 );

	if ( ptrImageDest != NULL )
		DIP_FreeAllocData( DIP_Data.ptrImageDataDest );

	return nErrCode;
}


/* ==========================================================================
컬러 -> 흑백 처리 
============================================================================*/
void CTK_ALPRWizardDoc::Doc_ConvertGrayscale( void )
{	
	DIP_DATA_t DIP_Data;
	IMAGE_DATA_t ImageDestInfo;
	DWORD wStartTick, wEndTick;

	memset(&DIP_Data, 0, sizeof( DIP_DATA_t ));
	memset( &ImageDestInfo, 0, sizeof( ImageDestInfo ));

	if ( m_ImageData.nBitCount != 24 )
	{
		AfxMessageBox(IDS_ERR_NOT_24BIT);
		return;
	}

	DIP_Data.ptrImageDataSrc1 = &m_ImageData;
	DIP_Data.ptrImageDataDest = &ImageDestInfo;
	DIP_Data.uDIPMsg = TK_DIP_CONVERTGRAY_MSG;

	wStartTick= GetTickCount();

	if  ( DIP_ImageProcess( &DIP_Data ) == ERR_SUCCESS && DIP_Data.ptrImageDataDest != NULL )
	{		
		wEndTick= GetTickCount();		
		Internal_NewImageDoc( m_tcFileName, DIP_Data.ptrImageDataDest, NULL, NULL );
		ShowTickTime( m_tcFileName, wStartTick, wEndTick );
	}

	DIP_FreeAllocData( DIP_Data.ptrImageDataDest );
}

/* ==========================================================================
Histogram ( Grayscale )
typedef struct HISTOGRAM_PARAM_
{
     int bNormalize;
     float fParam[ MAX_GRAY_CNT ];	
}HISTOGRAM_PARAM_t, * pHISTOGRAM_PARAM;
============================================================================*/
void CTK_ALPRWizardDoc::Doc_Histogram( void )
{
	HISTOGRAM_PARAM_t Histo_Param;

	memset(&Histo_Param, 0, sizeof( HISTOGRAM_PARAM_t ));
	Internal_DIP( TK_DIP_HISTOGRAM_MSG, &m_ImageData, NULL, NULL, &Histo_Param, 8 );	
}

/* ==========================================================================
Histogram 평활화
============================================================================*/
void CTK_ALPRWizardDoc::Doc_Histogram_Equal( void )
{
	HISTOGRAM_PARAM_t Histo_Param;
	IMAGE_DATA_t TempImageData;

	memset(&Histo_Param, 0, sizeof( HISTOGRAM_PARAM_t ));
	memset( &TempImageData, 0, sizeof( IMAGE_DATA_t ));

	DIP_CopyImageAllocData( &TempImageData, &m_ImageData );

	Internal_DIP( TK_DIP_HISTOGRAM_EQUAL_MSG, &TempImageData, NULL, NULL, &Histo_Param, 8 );

}

/* ==========================================================================
영상 이진화 
typedef struct BINARIZATION_PARAM_
{
	int nThreshold;
	int nSubThreshold;
	int bOnlyThreshold;
	int nAdaptCnt;
	float fAdaptRatio;
}BINARIZATION_PARAM_t, * pBINARIZATION_PARAM;
============================================================================*/
void CTK_ALPRWizardDoc::Doc_Binarization( int nTh )
{
	BINARIZATION_PARAM_t Binary_Param;
	IMAGE_DATA_t TempImageData;

	memset(&Binary_Param, 0, sizeof( BINARIZATION_PARAM_t ));
	memset( &TempImageData, 0, sizeof( IMAGE_DATA_t ));

	if( nTh == 0 )
	{
		nTh = DEFAULT_BIN_THRESOLD;
	}

	Binary_Param.nThreshold = nTh;	

	DIP_CopyImageAllocData( &TempImageData, &m_ImageData );

	Internal_DIP( TK_DIP_BINARIZATION_MSG, &TempImageData, NULL, NULL, &Binary_Param, 8 );

}

/* ==========================================================================
영상 이진화 ( 반복 기법 )
============================================================================*/
void CTK_ALPRWizardDoc::Doc_Binarization_Iterate( void )
{
	IMAGE_DATA_t TempImageData;
	BINARIZATION_PARAM_t Binary_Param;
	
	memset(&Binary_Param, 0, sizeof( BINARIZATION_PARAM_t ));
	memset( &TempImageData, 0, sizeof( IMAGE_DATA_t ));


	DIP_CopyImageAllocData( &TempImageData, &m_ImageData );
	Internal_DIP( TK_DIP_BINARIZATION_ITER_MSG, &TempImageData, NULL, NULL, &Binary_Param, 8 );
}

/* ==========================================================================
영상 이진화 ( Adaptive 기법 )
============================================================================*/
void CTK_ALPRWizardDoc::Doc_Binarization_Adaptive( void )
{
	IMAGE_DATA_t TempImageData;
	BINARIZATION_PARAM_t Binary_Param;

	memset(&Binary_Param, 0, sizeof( BINARIZATION_PARAM_t ));
	memset( &TempImageData, 0, sizeof( IMAGE_DATA_t ));

	Binary_Param.nAdaptCnt = DEFAULT_ADAPT_CNT;
	Binary_Param.fAdaptRatio = (float)DEFAULT_ADAPT_RATIO;

	DIP_CopyImageAllocData( &TempImageData, &m_ImageData );
	Internal_DIP( TK_DIP_BINARIZATION_ADAPTIVE_MSG, &TempImageData, NULL, NULL, &Binary_Param, 8 );	
}

/* ==========================================================================
영상 이진화 ( Integral Adaptive 기법 )
============================================================================*/
void CTK_ALPRWizardDoc::Doc_Binarization_ITG_Adaptive( int nsize )
{
	IMAGE_DATA_t TempImageData;
	BINARIZATION_PARAM_t Binary_Param;

	memset(&Binary_Param, 0, sizeof( BINARIZATION_PARAM_t ));
	memset( &TempImageData, 0, sizeof( IMAGE_DATA_t ));

	Binary_Param.nITGSize = nsize;	

	DIP_CopyImageAllocData( &TempImageData, &m_ImageData );	
	Internal_DIP( TK_DIP_BINARIZATION_ITG_ADAPTIVE_MSG, &TempImageData, NULL, NULL, &Binary_Param, 8 );	
}


/* ==========================================================================
영상 이진화 ( Otsu 기법 )
============================================================================*/
void CTK_ALPRWizardDoc::Doc_Binarization_Otsu( void )
{
	IMAGE_DATA_t TempImageData;
	BINARIZATION_PARAM_t Binary_Param;

	memset(&Binary_Param, 0, sizeof( BINARIZATION_PARAM_t ));
	memset( &TempImageData, 0, sizeof( IMAGE_DATA_t ));


	DIP_CopyImageAllocData( &TempImageData, &m_ImageData );
	Internal_DIP( TK_DIP_BINARIZATION_OTSU_MSG, &TempImageData, NULL, NULL, &Binary_Param, 8 );
}

/* ==========================================================================
영상 밝게
============================================================================*/
void CTK_ALPRWizardDoc::Doc_Brightness( int nBright )
{
	IMAGE_DATA_t TempImageData;
	BRIGHT_PARAM_t BrightParam;

	memset( &TempImageData, 0, sizeof( IMAGE_DATA_t ));
	memset( &BrightParam, 0, sizeof( BRIGHT_PARAM_t));

	BrightParam.nBright = nBright;
	
	DIP_CopyImageAllocData( &TempImageData, &m_ImageData );

	Internal_DIP( TK_DIP_BRIGHTNESS_MSG, &TempImageData, NULL, NULL, &BrightParam, 8 );
}

/* ==========================================================================
영상 비트평면
Grayscale 영상 1byte를 8개 비트로 나누어 특정 비트만 볼수 있도록 하는 기법
============================================================================*/
void CTK_ALPRWizardDoc::Doc_BitPlane( void )
{
	IMAGE_DATA_t TempImageData;

	memset( &TempImageData, 0, sizeof( IMAGE_DATA_t ));

	DIP_CopyImageAllocData( &TempImageData, &m_ImageData );

	Internal_DIP( TK_DIP_BITPLANE_MSG, &TempImageData, NULL, NULL, NULL, 8 );

}

/* ==========================================================================
영상 가우시안( 영상 스무싱 기법 )
============================================================================*/
void CTK_ALPRWizardDoc::Doc_Gaussian( void )
{
	IMAGE_DATA_t TempImageData;

	memset( &TempImageData, 0, sizeof( IMAGE_DATA_t ));

	DIP_CopyImageAllocData( &TempImageData, &m_ImageData );

	Internal_DIP( TK_DIP_GAUSSIAN_MSG, &TempImageData, NULL, NULL, NULL, 8 );

}

/* ==========================================================================
Sobel 기법( 외각선 검출 기법 )
============================================================================*/
void CTK_ALPRWizardDoc::Doc_Sobel( void )
{
	IMAGE_DATA_t TempImageData;
	LINE_EDGE_PARAM_t LineEdgeParam;

	memset(&LineEdgeParam, 0, sizeof( LineEdgeParam ));
	memset( &TempImageData, 0, sizeof( IMAGE_DATA_t ));

	LineEdgeParam.LineEdgeType = H_LINE_EDGE;

	DIP_CopyImageAllocData( &TempImageData, &m_ImageData );

	Internal_DIP( TK_DIP_SOBEL_MSG, &TempImageData, NULL, NULL, &LineEdgeParam, 8 );
	
}

/* ==========================================================================
Prewitt 기법( 외각선 검출 기법 )
============================================================================*/
void CTK_ALPRWizardDoc::Doc_Prewitt( void )
{
	IMAGE_DATA_t TempImageData;
	LINE_EDGE_PARAM_t LineEdgeParam;

	memset(&LineEdgeParam, 0, sizeof( LineEdgeParam ));
	memset( &TempImageData, 0, sizeof( IMAGE_DATA_t ));

	LineEdgeParam.LineEdgeType =H_LINE_EDGE;

	DIP_CopyImageAllocData( &TempImageData, &m_ImageData );

	Internal_DIP( TK_DIP_PREWITT_MSG, &TempImageData, NULL, NULL, &LineEdgeParam, 8 );
}

/* ==========================================================================
Robert 기법( 외각선 검출 기법 )
============================================================================*/
void CTK_ALPRWizardDoc::Doc_Roberts( void )
{
	IMAGE_DATA_t TempImageData;
	LINE_EDGE_PARAM_t LineEdgeParam;

	memset(&LineEdgeParam, 0, sizeof( LineEdgeParam ));
	memset( &TempImageData, 0, sizeof( IMAGE_DATA_t ));

	LineEdgeParam.LineEdgeType = H_LINE_EDGE;

	DIP_CopyImageAllocData( &TempImageData, &m_ImageData );

	Internal_DIP( TK_DIP_ROBERTS_MSG, &TempImageData, NULL, NULL, &LineEdgeParam, 8 );
}

/* ==========================================================================
 Canny 기법( 외각선 검출 기법 )
============================================================================*/
void CTK_ALPRWizardDoc::Doc_Canny( void )
{
	IMAGE_DATA_t TempImageData;

	memset( &TempImageData, 0, sizeof( IMAGE_DATA_t ));

	DIP_CopyImageAllocData( &TempImageData, &m_ImageData );

	Internal_DIP( TK_DIP_CANNY_MSG, &TempImageData, NULL, NULL, NULL, 8 );
}

/* ==========================================================================
 Labeling 기법( 객체 추출 기법 )
============================================================================*/
void CTK_ALPRWizardDoc::Doc_DeNoiseLabel( void )
{
	IMAGE_DATA_t TempImageData;

	memset( &TempImageData, 0, sizeof( IMAGE_DATA_t ));

	DIP_CopyImageAllocData( &TempImageData, &m_ImageData );

	Internal_DIP( TK_DIP_LABEL_MSG, &TempImageData, NULL, NULL, NULL, 8 );
}

/* ==========================================================================
 영상 침식 기법
============================================================================*/
void CTK_ALPRWizardDoc::Doc_Erosion( void )
{
	IMAGE_DATA_t TempImageData;

	memset( &TempImageData, 0, sizeof( IMAGE_DATA_t ));

	DIP_CopyImageAllocData( &TempImageData, &m_ImageData );

	Internal_DIP( TK_DIP_EROSION_MSG, &TempImageData, NULL, NULL, NULL, 8 );
}

/* ==========================================================================
 영상 빼기 기법
 원본 영상 1
 원본 영상 2
 Dest 영상 : 원본 1 - 원본 2
============================================================================*/
void CTK_ALPRWizardDoc::Doc_SubImage( void * ptrDoc1, void *ptrDoc2  )
{
	CTK_ALPRWizardDoc * ptrCompareDoc1, * ptrCompareDoc2;
	IMAGE_DATA_t ImageDataScr1, ImageDataScr2, ImageDest;
	
	memset( &ImageDest, 0, sizeof( IMAGE_DATA_t ));	
	memset( &ImageDataScr1, 0, sizeof( IMAGE_DATA_t ));	
	memset( &ImageDataScr2, 0, sizeof( IMAGE_DATA_t ));	

	
	ptrCompareDoc1 = (CTK_ALPRWizardDoc *)ptrDoc1;
	ptrCompareDoc2 = (CTK_ALPRWizardDoc *)ptrDoc2;	

	if ( ptrCompareDoc1->m_ImageData.ptrSrcImage2D == NULL || ptrCompareDoc2->m_ImageData.ptrSrcImage2D == NULL)
		return ;
	
	DIP_CopyImageAllocData( &ImageDataScr1, &ptrCompareDoc1->m_ImageData );	
	DIP_CopyImageAllocData( &ImageDataScr2, &ptrCompareDoc2->m_ImageData );	
	DIP_CopyImageAllocData( &ImageDest, &m_ImageData );		

	Internal_DIP( TK_DIP_SUBIMAGE_MSG, &ImageDataScr1, &ImageDataScr2, &ImageDest, NULL, 8 );
}

/* ==========================================================================
 영상Median( 중간값 ) 기법 
============================================================================*/

void CTK_ALPRWizardDoc::Doc_Median( void )
{
	IMAGE_DATA_t TempImageData;

	memset( &TempImageData, 0, sizeof( IMAGE_DATA_t ));

	DIP_CopyImageAllocData( &TempImageData, &m_ImageData );

	Internal_DIP( TK_DIP_MEDIAN_MSG, &TempImageData, NULL, NULL, NULL, 8 );
}

/* ==========================================================================
 영상 Mean( 평균 ) 기법 
============================================================================*/
void CTK_ALPRWizardDoc::Doc_Mean( void )
{
	IMAGE_DATA_t TempImageData;

	memset( &TempImageData, 0, sizeof( IMAGE_DATA_t ));

	DIP_CopyImageAllocData( &TempImageData, &m_ImageData );

	Internal_DIP( TK_DIP_MEAN_MSG, &TempImageData, NULL, NULL, NULL, 8 );
}

/* ==========================================================================
 영상 WeightMean( 가중 평균) 기법 
============================================================================*/
void CTK_ALPRWizardDoc::Doc_WeightMean( void )
{
	IMAGE_DATA_t TempImageData;

	memset( &TempImageData, 0, sizeof( IMAGE_DATA_t ));

	DIP_CopyImageAllocData( &TempImageData, &m_ImageData );

	Internal_DIP( TK_DIP_WEIGHTEDMEAN_MSG, &TempImageData, NULL, NULL, NULL, 8 );
	

}

/* ==========================================================================
 영상 Contrast 기법 
============================================================================*/
void CTK_ALPRWizardDoc::Doc_Contrast( void )
{
	IMAGE_DATA_t TempImageData;
	CONTRAST_PARAM_t ContrastParam;

	memset( &ContrastParam, 0, sizeof ( CONTRAST_PARAM_t ));
	ContrastParam.nRatio = DEFAULT_CONTRAST_RATIO;

	memset( &TempImageData, 0, sizeof( IMAGE_DATA_t ));

	DIP_CopyImageAllocData( &TempImageData, &m_ImageData );

	Internal_DIP( TK_DIP_CONTRAST_MSG, &TempImageData, NULL, NULL, &ContrastParam, 8 );
}

/* ==========================================================================
 차량 이미지에서 해드라이트 부분( 가장 밝은 부분 )을 찾아 
 영상의 평균값으로 변환하는 기법 
============================================================================*/
void CTK_ALPRWizardDoc::Doc_Beam( void )
{
	IMAGE_DATA_t TempImageData;
	BINARIZATION_PARAM_t Binary_Param;

	memset( &TempImageData, 0, sizeof( IMAGE_DATA_t ));
	memset( &Binary_Param, 0, sizeof( BINARIZATION_PARAM_t ));
	Binary_Param.nThreshold = DEFAULT_BEAM_TH;

	DIP_CopyImageAllocData( &TempImageData, &m_ImageData );

	Internal_DIP( TK_DIP_BEAM_MSG, &TempImageData, NULL, NULL, &Binary_Param, 8 );
}

/* ==========================================================================
  비등방성 확산 필터( 저주파 필터) : 잡음 제거 필터 
============================================================================*/
void CTK_ALPRWizardDoc::Doc_Diffusion( void )
{
	IMAGE_DATA_t TempImageData;

	memset( &TempImageData, 0, sizeof( IMAGE_DATA_t ));

	DIP_CopyImageAllocData( &TempImageData, &m_ImageData );

	Internal_DIP( TK_DIP_DIFFUSION_MSG, &TempImageData, NULL, NULL, NULL, 8 );
}
/* ==========================================================================
 MMSE : Minimum Mean Squared Error
   경계선을 보존하면서 곙계선 주변의 잡음제거 필터 
============================================================================*/
void CTK_ALPRWizardDoc::Doc_MMSE( void )
{
	IMAGE_DATA_t TempImageData;

	memset( &TempImageData, 0, sizeof( IMAGE_DATA_t ));

	DIP_CopyImageAllocData( &TempImageData, &m_ImageData );

	Internal_DIP( TK_DIP_MMSE_MSG, &TempImageData, NULL, NULL, NULL, 8 );
}

/* ==========================================================================
 HOUGH TRANSFORM : 영상의 직선 왜곡을 바로 잡는다.    
============================================================================*/
void CTK_ALPRWizardDoc::Doc_HoughTransform( void )
{
	IMAGE_DATA_t TempImageData;
	HOUGH_PARAM_t Hough_Param;
	CString strMsg;

	memset( &TempImageData, 0, sizeof( IMAGE_DATA_t ));	
	memset( &Hough_Param, 0, sizeof( HOUGH_PARAM_t ));

	DIP_CopyImageAllocData( &TempImageData, &m_ImageData );
	Internal_DIP( TK_DIP_HOUGH_LINE_TRANS_MSG, &TempImageData, NULL, NULL, &Hough_Param, 8 );
	
	strMsg.Format(L"허프 변환 결과:\n\nRho = %lf, Angle = %lf", Hough_Param.Rho, Hough_Param.Angle);
	AfxMessageBox(strMsg);
}

/* ==========================================================================
 Rotation TRANSFORM : 영상 회전 처리 .    
============================================================================*/
void CTK_ALPRWizardDoc::Doc_Rotation( double dAngle )
{
	IMAGE_DATA_t TempImageData;
	ROTATE_PARAM_t Rotate_Param;

	memset( &TempImageData, 0, sizeof( IMAGE_DATA_t ));	
	memset( &Rotate_Param, 0, sizeof( ROTATE_PARAM_t ));

	DIP_CopyImageAllocData( &TempImageData, &m_ImageData );
	Rotate_Param.Angle =dAngle;

	Internal_DIP( TK_DIP_ROTATE_MSG, &TempImageData, NULL, NULL, &Rotate_Param, 8 );
}

/* ==========================================================================
 DoG Filter : 영상 회전 처리 .     
============================================================================*/
void CTK_ALPRWizardDoc::Doc_DoG( double dSigma1 ,double dSigma2 )
{
	IMAGE_DATA_t TempImageData;
	GAUSSIAN_PARAM_t Gauss_Param;

	memset( &TempImageData, 0, sizeof( IMAGE_DATA_t ));	
	memset( &Gauss_Param, 0, sizeof( GAUSSIAN_PARAM_t ));

	DIP_CopyImageAllocData( &TempImageData, &m_ImageData );
	Gauss_Param.dDoGSigma1 =dSigma1;
	Gauss_Param.dDoGSigma2 =dSigma2;

	Internal_DIP( TK_DIP_DOG_MSG, &TempImageData, NULL, NULL, &Gauss_Param, 8 );
}

/* ==========================================================================
Bilinear Noramlization : 양선형 보간     
============================================================================*/
void CTK_ALPRWizardDoc::Doc_Bilinear_Normal( void )
{
	IMAGE_DATA_t TempImageData, ImageDest, ImageNew;
	BILINEAR_NOR_PARAM_t Bilinear_Param;
	int nNewW, nNewH, nLineByte;

	memset( &ImageDest, 0, sizeof( IMAGE_DATA_t ));
	memset( &ImageNew, 0, sizeof(IMAGE_DATA_t ));

	memset( &TempImageData, 0, sizeof( IMAGE_DATA_t ));	
	memset( &Bilinear_Param, 0, sizeof( BILINEAR_NOR_PARAM_t ));

	DIP_CopyImageAllocData( &TempImageData, &m_ImageData );
	
	nNewW = 250;
	nNewH = nNewW / 2 ;
	
	nLineByte = m_WizardHelper.GetCalBmpWidth(nNewW, TempImageData.nBitCount );

	ImageNew.nBitCount = TempImageData.nBitCount;
	ImageNew.nH = nNewH;
	ImageNew.nW = nNewW;
	ImageNew.nImageSize =  ( nLineByte * nNewH );	
	ImageNew.ptrPalette = NULL;



	DIP_CopyImageAllocData( &ImageDest, &ImageNew );		

	Bilinear_Param.LT_POINT.nX = 11;
	Bilinear_Param.LT_POINT.nY = 91;

	Bilinear_Param.RT_POINT.nX = 136;
	Bilinear_Param.RT_POINT.nY = 91;

	Bilinear_Param.RB_POINT.nX = 154;
	Bilinear_Param.RB_POINT.nY = 139;

	Bilinear_Param.LB_POINT.nX = 30;
	Bilinear_Param.LB_POINT.nY = 139;

	Internal_DIP( TK_DIP_BILINEAR_NOR_MSG, &TempImageData, NULL, &ImageDest, &Bilinear_Param, 8 );
}

void CTK_ALPRWizardDoc::Doc_Unsharp( int nMask )
{
	IMAGE_DATA_t TempImageData;
	UNSHARP_PARAM_t Unsharp_Param;

	memset( &TempImageData, 0, sizeof( IMAGE_DATA_t ));	
	memset( &Unsharp_Param, 0, sizeof( UNSHARP_PARAM_t ));

	DIP_CopyImageAllocData( &TempImageData, &m_ImageData );
	Unsharp_Param.Mask = nMask;	

	Internal_DIP( TK_DIP_UNSHARP_MSG, &TempImageData, NULL, NULL, &Unsharp_Param, 8 );
}

void CTK_ALPRWizardDoc::Doc_Dilation( void )
{
	IMAGE_DATA_t TempImageData;

	memset( &TempImageData, 0, sizeof( IMAGE_DATA_t ));	
	DIP_CopyImageAllocData( &TempImageData, &m_ImageData );	

	Internal_DIP( TK_DIP_DILATION_MSG, &TempImageData, NULL, NULL, NULL, 8 );
}

void CTK_ALPRWizardDoc::Doc_Opening( void )
{
	IMAGE_DATA_t TempImageData;

	memset( &TempImageData, 0, sizeof( IMAGE_DATA_t ));	
	DIP_CopyImageAllocData( &TempImageData, &m_ImageData );	
	Internal_DIP( TK_DIP_OPENING_MSG, &TempImageData, NULL, NULL, NULL, 8 );
}

void CTK_ALPRWizardDoc::Doc_Closing( void )
{
	IMAGE_DATA_t TempImageData;

	memset( &TempImageData, 0, sizeof( IMAGE_DATA_t ));	
	DIP_CopyImageAllocData( &TempImageData, &m_ImageData );	
	Internal_DIP( TK_DIP_CLOSING_MSG, &TempImageData, NULL, NULL, NULL, 8 );
}

/* ==========================================================================
 DIP 처리된 현재 이미지 저장
============================================================================*/
int CTK_ALPRWizardDoc::Doc_SaveCurrentImage( TCHAR * ptrtcFileName, TCHAR * ptrtcCurPath,TCHAR * ptrtcOrgFileName,int nRawSave)
{
	int nErrCode = ERR_SUCCESS;
	
	if( ptrtcFileName == NULL )
		return ERR_NO_PARAM;
	
	if ( nRawSave )
	{
		if( m_ImageData.nBitCount != DEFAULT_BIT_CNT )
			return ERR_INVALID_DATA;

		nErrCode = m_WizardHelper.SaveRaw( ptrtcCurPath, ptrtcOrgFileName, &m_ImageData );
	}
	else
	{
		nErrCode =  m_WizardHelper.SaveBmp( ptrtcFileName, &m_ImageData );
	}
	return nErrCode;
}


/* ==========================================================================
 Clild Window 생성 
============================================================================*/
void CTK_ALPRWizardDoc::Doc_SetCreateNewWindow( BOOL bCreate )
{
	m_pApp->SetCreateWindow( bCreate );
}

/* ==========================================================================
 OCR 데이터 저장 
============================================================================*/
int CTK_ALPRWizardDoc::Doc_SaveOCRFeature( int nPos, int nOCRCode )
{
	OCR_DATA_t OCR_Data;
	int nErrCode = ERR_SUCCESS;
	pIMAGE_DATA ptrOCRImage= NULL;

	ptrOCRImage = &m_LPRChars.Chars[ nPos ].ImageData;
	if ( ptrOCRImage == NULL || nPos >= m_LPRChars.nCharCnt )
		return ERR_INVALID_DATA;

	memset( &OCR_Data, 0, sizeof( OCR_DATA_t));
	OCR_Data.uOCRMsg = TK_OCR_SAVE_MSG;
	OCR_Data.ptrImageDataSrc = ptrOCRImage;
	OCR_Data.OCR_CODE = ( BYTE )nOCRCode;

	nErrCode = OCR_OCRProcess( &OCR_Data );
	
	return nErrCode;
}

/* ==========================================================================
 OCR 데이터 초기화
============================================================================*/
int CTK_ALPRWizardDoc::Doc_InitOCRData( void )
{
	int i;
	POSITION ptr;

	ptr =GetFirstViewPosition();
	CView * ptrView = GetNextView( ptr );

	for( i = 0 ; i < MAX_LPR_CHAR_CNT ; i++ )
	{
		m_WizardHelper.FreeImageAllocData( &m_LPRChars.Chars[ i ].ImageData );
	}	

	memset( &m_LPRChars, 0, sizeof( LPR_CHAR_t ));
	/* ptrView Char  */
	if ( ptrView != NULL )
	{
		ptrView->Invalidate( TRUE );
	}

	return ERR_SUCCESS;
}

/* ==========================================================================
 신경망 재 교육
============================================================================*/
int CTK_ALPRWizardDoc::Doc_RestudyBPNN( void )
{
	int nErrCode = ERR_SUCCESS;	
	nErrCode = OCR_Restudy( OCRNotifyCallBack );

	return nErrCode;
}

/* ==========================================================================
 신경망 재 교육
============================================================================*/
int CTK_ALPRWizardDoc::Doc_GetFeatureCnt( void )
{
	int nCnt;	

	IOCTL_DATA_t lpr_ioctl;

	nCnt =0;
	memset( &lpr_ioctl, 0, sizeof( IOCTL_DATA_t));

	lpr_ioctl.ptrOutput = &nCnt;
	lpr_ioctl.uIOCTLMsg = TK_IOCTL_FEATURECNT_MSG;

	OCR_IOCTL( &lpr_ioctl );
	
	return nCnt;
}

/* ==========================================================================
 신경망 강제 저장
============================================================================*/
int CTK_ALPRWizardDoc::Doc_NNForceSave( void )
{
	int nResult;	

	IOCTL_DATA_t lpr_ioctl;

	memset( &lpr_ioctl, 0, sizeof( IOCTL_DATA_t));

	lpr_ioctl.uIOCTLMsg = TK_IOCTL_NN_FORCE_SAVE_MSG;
	nResult= OCR_IOCTL( &lpr_ioctl );
	
	return nResult;
}


int CTK_ALPRWizardDoc::Doc_BackupOCRFeature( void )
{	
	TCHAR tcsourcePath[ MAX_PATH ];
	TCHAR tcdestPath[ MAX_PATH * 2];
	TCHAR tcTime[ MAX_PATH ];
	TCHAR tcpath[ MAX_PATH ];
	time_t Cur_Time;
	struct tm today ;
	SHFILEOPSTRUCT fo;

	memset( tcsourcePath, 0, sizeof(tcsourcePath));
	memset( tcdestPath, 0, sizeof( tcdestPath ));
	memset( tcpath, 0, sizeof( tcpath ));

	m_WizardHelper.GetCurrentPath( tcpath );
	Cur_Time = time( NULL );
	localtime_s(&today, &Cur_Time ) ;
	wsprintf( tcTime,L"%d-%02d-%02d[%02d%02d]", today.tm_year + 1900,today.tm_mon + 1,today.tm_mday, today.tm_hour, today.tm_min );
	wsprintf( tcsourcePath,L"%s\\%s", tcpath, _T( TK_OCR_FEATURE_DATA));
	wsprintf( tcdestPath,L"%s\\%s_%s",tcpath, tcTime, _T(TK_OCR_FEATURE_DATA));	
	
	CWnd *pWnd = AfxGetMainWnd();
	HWND hWnd = pWnd->m_hWnd;	

	fo.hwnd = hWnd;
	fo.wFunc = FO_COPY;
	fo.pFrom = tcsourcePath;
	fo.pTo = tcdestPath;
	fo.fFlags =  FOF_NOCONFIRMATION | FOF_NOERRORUI;
	SHFileOperation(&fo);
	return 1;

}
/* ==========================================================================
 DOC 설정 정보 
============================================================================*/
void CTK_ALPRWizardDoc::Doc_LoadlocalConf( void )
{
	loadLPRConf();
	loadOCRConf();
	loadDIPConf();
	loadAPPConf();

	m_ptrLPRConf = GetLPRConf();
	m_ptrOCRConf = GetOCRConf();

	m_ptrDIPConf = GetDIPConf();
	//m_ptrAPPConf = GetAPPConf();
	m_ptrAPPConf = m_AppClass.GetWinAPPCONF();
}

/* ==========================================================================
 설정 정보 갱신
============================================================================*/
void CTK_ALPRWizardDoc::Doc_ReloadConf( void )
{
	IOCTL_DATA_t lpr_ioctl;

	memset( &lpr_ioctl, 0, sizeof( IOCTL_DATA_t));
	lpr_ioctl.uIOCTLMsg = TK_IOCTL_RECONF_MSG;

	LPR_IOCTL( &lpr_ioctl );
	DIP_IOCTL( &lpr_ioctl );
	OCR_IOCTL( &lpr_ioctl );

	Doc_LoadlocalConf();
	
}

/* ==========================================================================
 DOC 메모리 초기화 
============================================================================*/
int CTK_ALPRWizardDoc::Doc_init( void )
{
	int i;

	/* 번호판 문자 복사 */
	for( i = 0 ; i < MAX_LPR_CHAR_CNT ; i++ )
	{
		m_WizardHelper.FreeImageAllocData( &m_LPRChars.Chars[ i ].ImageData );
	}
	
	memset( &m_LPRChars, 0, sizeof( LPR_CHAR_t ));
	memset( &m_Rect[ 0 ], 0, sizeof( CVRECT_t ));

	m_pApp->m_ptrImageData = NULL;
	m_pApp->m_ptrRect = NULL;

	return TRUE;
}

char * CTK_ALPRWizardDoc::Doc_GetImagePath( void )
{	
	//sprintf( m_ptrAPPConf->IMG_PATH, "%s", "C\sas\\sss");	
	return m_ptrAPPConf->IMG_PATH;
}	

/* ==========================================================================
Wizard Function
Main Menu 또는 Clid Menu에서 새 파일을 클릭 할 경우 
===========================================================================*/
BOOL CTK_ALPRWizardDoc::OnNewDocument()
{	
	int i;
	if (!CDocument::OnNewDocument())
		return FALSE;
	
	m_WizardHelper.FreeImageAllocData( &m_ImageData );

	/* 이미지 데이터 복사 */
	if ( m_pApp->m_ptrImageData != NULL )
	{
		memset( &m_ImageData, 0, sizeof( IMAGE_DATA_t ));
		wcscpy( m_tcFileName, m_pApp->m_ptrtcFileName);
		m_WizardHelper.CopyImageData( &m_ImageData, m_pApp->m_ptrImageData, TRUE );
		m_pApp->m_ptrImageData = NULL;
	}

	/* 번호판 영역 복사 */
	if ( m_pApp->m_ptrRect != NULL )
	{
		memcpy( &m_Rect, m_pApp->m_ptrRect, sizeof( m_Rect ));
		m_pApp->m_ptrRect = NULL;
	}

	/* 번호판 문자 복사 */
	for( i = 0 ; i < MAX_LPR_CHAR_CNT ; i++ )
	{
		m_WizardHelper.FreeImageAllocData( &m_LPRChars.Chars[ i ].ImageData );
	}
	memset( &m_LPRChars, 0, sizeof( LPR_CHAR_t ));

	if ( m_pApp->m_ptrLPRChars != NULL )
	{		
		m_LPRChars.nCharCnt = m_pApp->m_ptrLPRChars->nCharCnt;
		for( i = 0 ; i < MAX_LPR_CHAR_CNT ; i++ )
		{
			m_WizardHelper.CopyImageData( &m_LPRChars.Chars[ i ].ImageData,  &m_pApp->m_ptrLPRChars->Chars[ i ].ImageData, TRUE );
			memcpy( &m_LPRChars.Chars[ i ].ChRect , &m_pApp->m_ptrLPRChars->Chars[ i ].ChRect, sizeof( CVRECT_t) );
		}
		m_pApp->m_ptrLPRChars = NULL;

	}

	return TRUE;
}


void CTK_ALPRWizardDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: 여기에 저장 코드를 추가합니다.
	}
	else
	{
		// TODO: 여기에 로딩 코드를 추가합니다.
	}
}


// CTK_ALPRWizardDoc 진단

#ifdef _DEBUG
void CTK_ALPRWizardDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CTK_ALPRWizardDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// CTK_ALPRWizardDoc 명령
