#include "stdafx.h"
#include <malloc.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "../include/TK_TYPE.h"
#include "../include/TK_LIB_Helper.h"
#include "LPR_DIP.h"

#include "LPR_Hybrid.h"

/*======================================================================
이미지 변환( 축소/확대 )
원본 이미지를 이용하여 특정 크기( 가로 세로 비율 고정 )로 변환

큰 이미지를 작은 이미지로 축소하여 LPR처리를 할 경우 처리 시간이 줄어
드는 장점이 있음. 
========================================================================*/
static int ResizeImage(pDIP_DATA ptrDIPData, pIMAGE_DATA ptrSrcImageData, pIMAGE_DATA ptrDestImageData )
{
	
	int nNewH, nNewW, nLineByte;
	float fHRatio = 0.0;

	fHRatio = ptrSrcImageData->nW / ptrSrcImageData->nH;

	nNewW = RESIZE_IMAGE_W;
	nNewH = nNewW / fHRatio;

	
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
Plate Module 초기화
Char Module 초기화 
========================================================================*/
int Hybrid_Initialize( void )
{
	PLATE_Initialize();
	CHAR_Initialize( );

	return ERR_SUCCESS;
}

/*======================================================================
Hibrid Process

pDIP_DATA ptrDIPData		: LPR 과정에 필요한 DIP처리용 메모리 
pLPR_RESULT ptrLPRResult	: LPR 처리 결과 용 메모리
pIMAGE_DATA ptrSrcImageData	: LPR 처리용 원본 2차원 메모리
========================================================================*/
int Hybrid_LPR( pDIP_DATA ptrDIPData, pLPR_RESULT ptrLPRResult, pIMAGE_DATA ptrSrcImageData)
{
	static int nFirst = 0;
	int i;
	
	pIMAGE_DATA ptrDestImageData = NULL;
	pIMAGE_DATA ptrTargetImageData = NULL;	
	pCVRECT ptrPlateRect = NULL;
	pLPRCHAR ptrLPRChars = NULL;
	pCOMMONINFO ptrComInfo = NULL;
	IMAGE_DATA_t ResizeImageData;
	
	ptrDestImageData = (pIMAGE_DATA )&ptrLPRResult->TempImdateData;
	ptrPlateRect = (pCVRECT)&ptrLPRResult->PlateRect[ 0 ];
	ptrLPRChars = ( pLPRCHAR)&ptrLPRResult->LPRChars;
	memset( &ResizeImageData, 0, sizeof( IMAGE_DATA_t));


#if 1
	ptrTargetImageData = ptrSrcImageData;
#else
	/*===========================================================================
	[ 사이즈 변경 단계 ]
	원본 이미지를 절반 크기로 줄여 처리 속도를 향상 한다. 
	============================================================================*/
	ResizeImage( ptrDIPData, ptrSrcImageData , &ResizeImageData );

	ptrTargetImageData = (pIMAGE_DATA)&ResizeImageData;
#endif

	
	/*===========================================================================
	[ 공용 메모리 생성 및 재 생성 단계 ]
	LPR 처리 중 사용하게 될 공유 메모리를 미리 생성 함으로 처리 속도를 향상 시킨다.
	============================================================================*/
	if ( nFirst == 0 )
	{
		nFirst = 1;
		/*공용 메모리 초기화 */
		if ( COM_Initialize(  ptrTargetImageData ) != ERR_SUCCESS )
		{
			nFirst = 0;
		}
	}
	else
	{
		/*이전 이미지와 현재 이미지의 사이즈가 다를 경우 공용 메모리 재 설정 */
		ptrComInfo = COM_GetInfo( );
		if ( ptrComInfo->nImageHeight != ptrTargetImageData->nH || ptrComInfo->nImageWidth != ptrTargetImageData->nW )
		{
			COM_Release();
			if ( COM_Initialize(  ptrTargetImageData ) != ERR_SUCCESS )
			{
				nFirst = 0;
			}
		}
	}	
	
	/*===========================================================================
	[ 차량의 번호판 영역 후보지 검출 단계 ]
	불균등 영역을 판단하여 번호판 후보지 선정
	============================================================================*/
	PLATE_GetRecognition(ptrDIPData, ptrDestImageData, ptrTargetImageData, ptrPlateRect );


	/*===========================================================================
	[ 번호판내의 문자 영역 검출 단계 ]
	번호판 후보지에서 문자 이미지를 검출,판독
	============================================================================*/
#if DBG_RUN_RECOGNITION
	CHAR_GetRecognition( ptrDIPData, ptrDestImageData, ptrTargetImageData, ptrPlateRect, ptrLPRChars  );
#endif  /* DBG_RUN_RECOGNITION */


	/*===========================================================================
	[ 차량의 번호판 영역 후보지 복사 ]
	Debuging을 위해 변환된 번호판 영역부분을 복사한다.
	============================================================================*/
#if DBG_COPY_TRANSFER_IMAGE
	if ( ptrDestImageData != NULL && ptrDestImageData->ptrSrcImage2D != NULL )
	{
		for (i=0; i<ptrTargetImageData->nH; i++)
		{			
			memcpy( *( ptrDestImageData->ptrSrcImage2D + i ) , *( ptrTargetImageData->ptrSrcImage2D + i ) , ptrTargetImageData->nW );		
		}
	}
#endif

	HLP_FreeImageAllocData( &ResizeImageData);
	
	return ERR_SUCCESS;
}

/*======================================================================
Plate Module 전달
Char Module 전달
========================================================================*/
int Hybrid_IOCTL( pIOCTL_DATA ptrIOCTLData )
{
	PLATE_IOCTL( ptrIOCTLData );
	CHAR_IOCTL( ptrIOCTLData  );

	return ERR_SUCCESS;
}
/*======================================================================
메모리 해제 
========================================================================*/
int Hybrid_Release( void )
{	
	COM_Release();
	PLATE_Release();
	CHAR_Release();

	return ERR_SUCCESS;
}