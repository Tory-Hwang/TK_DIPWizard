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
�̹��� ��ȯ( ���/Ȯ�� )
���� �̹����� �̿��Ͽ� Ư�� ũ��( ���� ���� ���� ���� )�� ��ȯ

ū �̹����� ���� �̹����� ����Ͽ� LPRó���� �� ��� ó�� �ð��� �پ�
��� ������ ����. 
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

	/* ArrayData --> Matrix ������ buf ����*/	
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
Plate Module �ʱ�ȭ
Char Module �ʱ�ȭ 
========================================================================*/
int Hybrid_Initialize( void )
{
	PLATE_Initialize();
	CHAR_Initialize( );

	return ERR_SUCCESS;
}

/*======================================================================
Hibrid Process

pDIP_DATA ptrDIPData		: LPR ������ �ʿ��� DIPó���� �޸� 
pLPR_RESULT ptrLPRResult	: LPR ó�� ��� �� �޸�
pIMAGE_DATA ptrSrcImageData	: LPR ó���� ���� 2���� �޸�
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
	[ ������ ���� �ܰ� ]
	���� �̹����� ���� ũ��� �ٿ� ó�� �ӵ��� ��� �Ѵ�. 
	============================================================================*/
	ResizeImage( ptrDIPData, ptrSrcImageData , &ResizeImageData );

	ptrTargetImageData = (pIMAGE_DATA)&ResizeImageData;
#endif

	
	/*===========================================================================
	[ ���� �޸� ���� �� �� ���� �ܰ� ]
	LPR ó�� �� ����ϰ� �� ���� �޸𸮸� �̸� ���� ������ ó�� �ӵ��� ��� ��Ų��.
	============================================================================*/
	if ( nFirst == 0 )
	{
		nFirst = 1;
		/*���� �޸� �ʱ�ȭ */
		if ( COM_Initialize(  ptrTargetImageData ) != ERR_SUCCESS )
		{
			nFirst = 0;
		}
	}
	else
	{
		/*���� �̹����� ���� �̹����� ����� �ٸ� ��� ���� �޸� �� ���� */
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
	[ ������ ��ȣ�� ���� �ĺ��� ���� �ܰ� ]
	�ұյ� ������ �Ǵ��Ͽ� ��ȣ�� �ĺ��� ����
	============================================================================*/
	PLATE_GetRecognition(ptrDIPData, ptrDestImageData, ptrTargetImageData, ptrPlateRect );


	/*===========================================================================
	[ ��ȣ�ǳ��� ���� ���� ���� �ܰ� ]
	��ȣ�� �ĺ������� ���� �̹����� ����,�ǵ�
	============================================================================*/
#if DBG_RUN_RECOGNITION
	CHAR_GetRecognition( ptrDIPData, ptrDestImageData, ptrTargetImageData, ptrPlateRect, ptrLPRChars  );
#endif  /* DBG_RUN_RECOGNITION */


	/*===========================================================================
	[ ������ ��ȣ�� ���� �ĺ��� ���� ]
	Debuging�� ���� ��ȯ�� ��ȣ�� �����κ��� �����Ѵ�.
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
Plate Module ����
Char Module ����
========================================================================*/
int Hybrid_IOCTL( pIOCTL_DATA ptrIOCTLData )
{
	PLATE_IOCTL( ptrIOCTLData );
	CHAR_IOCTL( ptrIOCTLData  );

	return ERR_SUCCESS;
}
/*======================================================================
�޸� ���� 
========================================================================*/
int Hybrid_Release( void )
{	
	COM_Release();
	PLATE_Release();
	CHAR_Release();

	return ERR_SUCCESS;
}