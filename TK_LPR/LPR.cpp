#include "stdafx.h"
#include <malloc.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "../include/TK_TYPE.h"
#include "../include/TK_LIB_API.h"
#include "../include/TK_LIB_Helper.h"
#include "../include/TK_CONF.h"

#include "LPR_DIP.h"
#include "LPR.h"
#include "LPR_COMMON.h"
#include "LPR_PLATE.h"
#include "LPR_CHAR.h"
#include "LPR_BASIC.h"

/*==================================================================================
DIP LIB Version
===================================================================================*/
#define LPR_VER_1ST		0x00
#define LPR_VER_2ND		0x01
/*==================================================================================*/

static int Load_LIB( void );
static int Free_LIB( void );

static int Internal_DIFFProcess( pDIFF_DATA ptrDiffData );
static int Internal_LPRProcess( pLPR_DATA ptrLPRData );

static int Load_Conf_Data( void )
{
	int nErrCode = ERR_SUCCESS;

	nErrCode = loadLPRConf();
	if ( nErrCode != ERR_SUCCESS )
		return nErrCode;

	nErrCode = loadOCRConf();
	if ( nErrCode != ERR_SUCCESS )
		return nErrCode;

	return nErrCode;
}

/*======================================================================
LPR 시작 시 DIP DLL과 OCR DLL을 사용하기때문에 
LOAD 해 놓는다. 
========================================================================*/
static int Load_LIB(  pLIB_INIT_DATA ptrInitData )
{
	BOOL bResult = TRUE;	

	if (Load_DIP_LIB( TK_DIP_LIB_NAME ) == FALSE)
	{	
		bResult = FALSE;
	}

	if ( bResult )
	{
		if ( Load_OCR_LIB( TK_OCR_LIB_NAME ) == FALSE)
		{
			bResult = FALSE;
		}
	}

	if ( bResult == TRUE )
	{		

		DIP_Initialize( ptrInitData );
		OCR_Initialize (ptrInitData );

		
		return ERR_SUCCESS;
	}	
	
	return ERR_INITIALIZE;
}

static int Free_LIB( void )
{
	Free_DIP_LIB();
	Free_OCR_LIB();
	return 0;
}

/*======================================================================
BMP 원본 1차원 DATA를 쉽게 처리하기 위해 2차원 구조로 재 생성한다. 
필요할 경우 TEMP 2차원 메모리를 할당 한다. 
========================================================================*/
static int MakeMatrix( pCAMERA_DATA ptrCameraData, pIMAGE_DATA ptrImageData , pIMAGE_DATA ptrImageData2 )
{
	int i, j, nLineByte=0;
	BYTE Data;

	/* Camera Image 정보 복사 */
	ptrImageData->nBitCount = ptrCameraData->nBitCount;
	ptrImageData->nH = ptrCameraData->nH;
	ptrImageData->nW = ptrCameraData->nW;
	ptrImageData->nImageSize = ptrCameraData->nImageSize;	

	if ( ptrCameraData->nBitCount != DEFAULT_BIT_CNT )
		return ERR_INVALID_DATA;

	/* ArrayData --> Matrix 을위한 buf 생성*/
	nLineByte = HLP_GetCalBmpWidth( ptrImageData->nW , ptrImageData->nBitCount );
	ptrImageData->ptrSrcImage2D = HLP_AllocMatrix( ptrImageData->nH, nLineByte );

	if ( ptrImageData->ptrSrcImage2D == NULL )
		return ERR_LIB_PROCESS;

	if ( ptrImageData2 != NULL )
	{
		ptrImageData2->nBitCount = ptrCameraData->nBitCount;
		ptrImageData2->nH = ptrCameraData->nH;
		ptrImageData2->nW = ptrCameraData->nW;
		ptrImageData2->nImageSize = ptrCameraData->nImageSize;

		ptrImageData2->ptrSrcImage2D = HLP_AllocMatrix( ptrImageData2->nH, nLineByte );
		if ( ptrImageData2->ptrSrcImage2D == NULL )
			return ERR_LIB_PROCESS;
	}

	/* ArrayData --> Matrix 복사*/
	for( i = 0 ; i < ptrCameraData->nH ; i++ )
	{
		for( j= 0 ; j< nLineByte ; j++ )
		{
			Data = *( ptrCameraData->ptrSrcImage1D +  ( i * nLineByte ) + j ) ;			
			ptrImageData->ptrSrcImage2D[ i ][ j ] =  Data ;
		}
	}

	return ERR_SUCCESS;
}


/*======================================================================
이미지 변환( 축소/확대 )
원본 이미지를 이용하여 특정 크기( 가로 세로 비율 고정 )로 변환

큰 이미지를 작은 이미지로 축소하여 LPR처리를 할 경우 처리 시간이 줄어
드는 장점이 있음. 
========================================================================*/
static int ResizeImage(pLPR_CONF ptrLPRConf, pDIP_DATA ptrDIPData, pIMAGE_DATA ptrSrcImageData, pIMAGE_DATA ptrDestImageData )
{

	int nNewH, nNewW, nLineByte;
	float fHRatio = 0.0;	 

	fHRatio = (float)(ptrSrcImageData->nW / ptrSrcImageData->nH);
	nNewW = ptrLPRConf->LPR_RESIZE_W;
	nNewH = (int)(nNewW / fHRatio);

	nLineByte = HLP_GetCalBmpWidth( nNewW , ptrSrcImageData->nBitCount );
	ptrDestImageData->nBitCount = ptrSrcImageData->nBitCount;
	ptrDestImageData->nH = nNewH;
	ptrDestImageData->nW = nNewW;
	ptrDestImageData->nImageSize =  ( nLineByte * nNewH );	
	ptrDestImageData->ptrPalette = NULL;

	/* ArrayData --> Matrix 을위한 buf 생성*/	
	ptrDestImageData->ptrSrcImage2D = HLP_AllocMatrix( ptrDestImageData->nH, nLineByte );
	if ( ptrDestImageData->ptrSrcImage2D == NULL )
		return ERR_LIB_PROCESS;

	for ( int i = 0 ; i < ptrDestImageData->nH ; i++ )
	{
		memset( *( ptrDestImageData->ptrSrcImage2D + i ) , 0 , nLineByte );
	}

	ReSizeCubic( ptrDIPData, ptrSrcImageData, ptrDestImageData );

	return ERR_SUCCESS;
}


/*======================================================================
LPR Process 
1.넘겨 받은 BMP 데이터 복사
2.Hybrid subregion feature 방식에 의한 번호판 영역 추적

========================================================================*/
static int Internal_LPRProcess( pLPR_DATA ptrLPRData)
{
	int i ;
	IMAGE_DATA_t ImageData;	
	pCAMERA_DATA ptrCameraData=NULL;
	pLPR_RESULT ptrLPRResult = NULL;
	DIP_DATA_t DIPData;

	static int nFirst = 0;
	
	pIMAGE_DATA ptrDestImageData = NULL;
	pIMAGE_DATA ptrTargetImageData = NULL;	
	pIMAGE_DATA ptrChangeImageData = NULL;	

	pCVRECT ptrPlateRect = NULL;
	pLPRCHAR ptrLPRChars = NULL;
	pCOMMONINFO ptrComInfo = NULL;
	IMAGE_DATA_t ResizeImageData;	

	int nErrCode= ERR_SUCCESS;

	pLPR_CONF ptrLPRConf = NULL;
	pOCR_CONF ptrOCRConf = NULL;
	
	int nValidPlateCnt = 0;

	memset( &ImageData, 0, sizeof(IMAGE_DATA_t ));	
	memset( &DIPData, 0, sizeof( DIP_DATA_t));
	memset( &ResizeImageData, 0, sizeof( IMAGE_DATA_t));

	ptrCameraData = ptrLPRData->ptrCameraData;
	ptrLPRResult = ( pLPR_RESULT )&ptrLPRData->LPRResult[ 0 ];

	ptrLPRConf = GetLPRConf();
	ptrOCRConf = GetOCRConf();

	if ( ptrLPRResult == NULL || ptrCameraData == NULL || 
		 ptrCameraData->ptrSrcImage1D == NULL || 
		 ptrLPRConf == NULL || ptrOCRConf == NULL )
	{
		return ERR_INVALID_DATA;
	}

	/*Camera의 Array Data를 Matrix데이터로 저장 */
	nErrCode = MakeMatrix( ptrCameraData, &ImageData, NULL );
	if ( nErrCode != ERR_SUCCESS )
		return nErrCode;	
	
	ptrDestImageData	= (pIMAGE_DATA )&ptrLPRResult->TempImdateData;
	ptrPlateRect		= (pCVRECT)&ptrLPRResult->PlateRect[ 0 ];
	ptrLPRChars			= ( pLPRCHAR)&ptrLPRResult->LPRChars;
	
	/*영상 좌,우 방향성 설정 */
	COM_SetArea( ptrCameraData->nArea );
	/*===========================================================================
	[ 사이즈 변경 단계 ]
	원본 이미지를 절반 크기로 줄여 처리 속도를 향상 한다. 
	============================================================================*/
	if( ptrLPRConf->LPR_DBG_RUN_RESIZE == 0)
	{
		ptrTargetImageData = &ImageData;
	}
	else
	{
		ResizeImage( ptrLPRConf, &DIPData, &ImageData , &ResizeImageData );
		ptrTargetImageData = (pIMAGE_DATA)&ResizeImageData;
	}	
	
	/*===========================================================================
	[ 공용 메모리 생성 및 재 생성 단계 ]
	LPR 처리 중 사용하게 될 공유 메모리를 미리 생성 함으로 처리 속도를 향상 시킨다.
	============================================================================*/
	if ( nFirst == 0 )
	{
		nFirst = 1;
		/*공용 메모리 초기화 */
		if ( COM_Initialize( ptrLPRConf, ptrOCRConf, ptrTargetImageData ) != ERR_SUCCESS )
		{
			nFirst = 0;
		}
	}
	else
	{
		/*이전 이미지와 현재 이미지의 사이즈가 다를 경우 공용 메모리 재 설정 */
		ptrComInfo = COM_GetInfo( );
		if ( ptrComInfo->nMaxHeight < ptrTargetImageData->nH || ptrComInfo->nMaxWidth < ptrTargetImageData->nW )
		{
			COM_Release( ptrLPRConf, ptrOCRConf);
			if ( COM_Initialize( ptrLPRConf, ptrOCRConf, ptrTargetImageData ) != ERR_SUCCESS )
			{
				nFirst = 0;
			}
		}
	}

	ptrChangeImageData = COM_GetChgImageBuf();
	HLP_CopyImageDataNotAlloc( ptrChangeImageData, ptrTargetImageData);

	if ( ptrLPRConf->LPR_DBG_RUN_BASIC_STUDY )
	{
		/*===========================================================================
		[ 기본 학습 문자 검출 단계 ]		
		============================================================================*/
		nValidPlateCnt= BASIC_GetRecognition( ptrLPRConf, ptrOCRConf ,&DIPData, ptrDestImageData, ptrChangeImageData, ptrTargetImageData, ptrPlateRect );
	}
	else
	{
		/*===========================================================================
		[ 차량의 번호판 영역 후보지 검출 단계 ]
		불균등 영역을 판단하여 번호판 후보지 선정
		============================================================================*/
		nValidPlateCnt= PLATE_GetRecognition( ptrLPRConf, ptrOCRConf ,&DIPData, ptrDestImageData, ptrChangeImageData, ptrTargetImageData, ptrPlateRect );
	}

	/*===========================================================================
	[ 번호판내의 문자 영역 검출 단계 ]
	번호판 후보지에서 문자 이미지를 검출,판독
	============================================================================*/
	if ( nValidPlateCnt > 0 )
	{
		CHAR_GetRecognition( ptrLPRConf, ptrOCRConf ,&DIPData, ptrDestImageData, ptrChangeImageData, ptrTargetImageData, ptrPlateRect, ptrLPRChars  );
	}

	/*===========================================================================
	[ 차량의 번호판 영역 후보지 복사 ]
	Debuging을 위해 변환된 번호판 영역부분을 복사한다.
	============================================================================*/
	if( ptrLPRConf->LPR_DBG_COPY_TRANSFER_IMAGE )
	{
		if ( ptrDestImageData != NULL && ptrDestImageData->ptrSrcImage2D != NULL )
		{
			for (i=0; i<ptrChangeImageData->nH; i++)
			{			
				memcpy( *( ptrDestImageData->ptrSrcImage2D + i ) , *( ptrChangeImageData->ptrSrcImage2D + i ) , ptrChangeImageData->nW );		
			}
		}
	}
	
	/*===========================================================================
	[ 메모리 해제 ]	
	============================================================================*/
	HLP_FreeImageAllocData( &ResizeImageData);
	HLP_FreeImageAllocData( &ImageData);

	return ERR_SUCCESS;
}

/*=========================================================================
Difference Process
==========================================================================*/
static int Internal_DIFFProcess( pDIFF_DATA ptrDiffData )
{
	return ERR_SUCCESS;
}

/*=========================================================================
LPR 초기화
Hybrid subregion Feature
DIP, OCR DLL 초기화 
==========================================================================*/
int Initialize( pLIB_INIT_DATA ptrInitData )
{
	int nErrCode = ERR_SUCCESS;
	
	if( ptrInitData != NULL )
	{
		SetWinCurPath( ptrInitData->szwinpath);
	}

	nErrCode= Load_Conf_Data();

	if ( nErrCode != ERR_SUCCESS )
		return nErrCode;

	PLATE_Initialize();
	CHAR_Initialize( );

	nErrCode = Load_LIB( ptrInitData );

	if( ptrInitData != NULL )
	{
		ptrInitData->Version[ 0 ] = LPR_VER_1ST;
		ptrInitData->Version[ 1 ] = LPR_VER_2ND;
	}

	return nErrCode;
}

/*=========================================================================
LPR 해제
==========================================================================*/
int Release( void  )
{
	pLPR_CONF ptrLPRConf = NULL;
	pOCR_CONF ptrOCRConf = NULL;

	ptrLPRConf = GetLPRConf();
	ptrOCRConf = GetOCRConf();

	COM_Release( ptrLPRConf, ptrOCRConf );
	PLATE_Release();
	CHAR_Release();

	Free_LIB();
	return ERR_SUCCESS;
}

/*=========================================================================
이미지의 변화 상태 감지를 위한 Difference Process
현재 사용 안함.
==========================================================================*/
int DIFFProcess( pDIFF_DATA ptrDiffData )
{
	return Internal_DIFFProcess( ptrDiffData );
}

/*=========================================================================
ALPR 처리를 위한 Process
==========================================================================*/
int LPRProcess( pLPR_DATA ptrLPRData )
{
	return Internal_LPRProcess( ptrLPRData );
}

/*=========================================================================
LPR의 주요 기능 이외의 다른 처리를 위한 Reserved Funtion
==========================================================================*/
int IOCTL( pIOCTL_DATA ptrIOCTLData )
{	
	int nErrCode = ERR_SUCCESS;

	if( ptrIOCTLData->uIOCTLMsg == TK_IOCTL_RECONF_MSG )
	{
		nErrCode= Load_Conf_Data();
		return nErrCode;
	}
	
	PLATE_IOCTL( ptrIOCTLData );
	CHAR_IOCTL( ptrIOCTLData  );

	return ERR_SUCCESS;
}