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
LPR ���� �� DIP DLL�� OCR DLL�� ����ϱ⶧���� 
LOAD �� ���´�. 
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
BMP ���� 1���� DATA�� ���� ó���ϱ� ���� 2���� ������ �� �����Ѵ�. 
�ʿ��� ��� TEMP 2���� �޸𸮸� �Ҵ� �Ѵ�. 
========================================================================*/
static int MakeMatrix( pCAMERA_DATA ptrCameraData, pIMAGE_DATA ptrImageData , pIMAGE_DATA ptrImageData2 )
{
	int i, j, nLineByte=0;
	BYTE Data;

	/* Camera Image ���� ���� */
	ptrImageData->nBitCount = ptrCameraData->nBitCount;
	ptrImageData->nH = ptrCameraData->nH;
	ptrImageData->nW = ptrCameraData->nW;
	ptrImageData->nImageSize = ptrCameraData->nImageSize;	

	if ( ptrCameraData->nBitCount != DEFAULT_BIT_CNT )
		return ERR_INVALID_DATA;

	/* ArrayData --> Matrix ������ buf ����*/
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

	/* ArrayData --> Matrix ����*/
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
�̹��� ��ȯ( ���/Ȯ�� )
���� �̹����� �̿��Ͽ� Ư�� ũ��( ���� ���� ���� ���� )�� ��ȯ

ū �̹����� ���� �̹����� ����Ͽ� LPRó���� �� ��� ó�� �ð��� �پ�
��� ������ ����. 
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
LPR Process 
1.�Ѱ� ���� BMP ������ ����
2.Hybrid subregion feature ��Ŀ� ���� ��ȣ�� ���� ����

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

	/*Camera�� Array Data�� Matrix�����ͷ� ���� */
	nErrCode = MakeMatrix( ptrCameraData, &ImageData, NULL );
	if ( nErrCode != ERR_SUCCESS )
		return nErrCode;	
	
	ptrDestImageData	= (pIMAGE_DATA )&ptrLPRResult->TempImdateData;
	ptrPlateRect		= (pCVRECT)&ptrLPRResult->PlateRect[ 0 ];
	ptrLPRChars			= ( pLPRCHAR)&ptrLPRResult->LPRChars;
	
	/*���� ��,�� ���⼺ ���� */
	COM_SetArea( ptrCameraData->nArea );
	/*===========================================================================
	[ ������ ���� �ܰ� ]
	���� �̹����� ���� ũ��� �ٿ� ó�� �ӵ��� ��� �Ѵ�. 
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
	[ ���� �޸� ���� �� �� ���� �ܰ� ]
	LPR ó�� �� ����ϰ� �� ���� �޸𸮸� �̸� ���� ������ ó�� �ӵ��� ��� ��Ų��.
	============================================================================*/
	if ( nFirst == 0 )
	{
		nFirst = 1;
		/*���� �޸� �ʱ�ȭ */
		if ( COM_Initialize( ptrLPRConf, ptrOCRConf, ptrTargetImageData ) != ERR_SUCCESS )
		{
			nFirst = 0;
		}
	}
	else
	{
		/*���� �̹����� ���� �̹����� ����� �ٸ� ��� ���� �޸� �� ���� */
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
		[ �⺻ �н� ���� ���� �ܰ� ]		
		============================================================================*/
		nValidPlateCnt= BASIC_GetRecognition( ptrLPRConf, ptrOCRConf ,&DIPData, ptrDestImageData, ptrChangeImageData, ptrTargetImageData, ptrPlateRect );
	}
	else
	{
		/*===========================================================================
		[ ������ ��ȣ�� ���� �ĺ��� ���� �ܰ� ]
		�ұյ� ������ �Ǵ��Ͽ� ��ȣ�� �ĺ��� ����
		============================================================================*/
		nValidPlateCnt= PLATE_GetRecognition( ptrLPRConf, ptrOCRConf ,&DIPData, ptrDestImageData, ptrChangeImageData, ptrTargetImageData, ptrPlateRect );
	}

	/*===========================================================================
	[ ��ȣ�ǳ��� ���� ���� ���� �ܰ� ]
	��ȣ�� �ĺ������� ���� �̹����� ����,�ǵ�
	============================================================================*/
	if ( nValidPlateCnt > 0 )
	{
		CHAR_GetRecognition( ptrLPRConf, ptrOCRConf ,&DIPData, ptrDestImageData, ptrChangeImageData, ptrTargetImageData, ptrPlateRect, ptrLPRChars  );
	}

	/*===========================================================================
	[ ������ ��ȣ�� ���� �ĺ��� ���� ]
	Debuging�� ���� ��ȯ�� ��ȣ�� �����κ��� �����Ѵ�.
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
	[ �޸� ���� ]	
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
LPR �ʱ�ȭ
Hybrid subregion Feature
DIP, OCR DLL �ʱ�ȭ 
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
LPR ����
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
�̹����� ��ȭ ���� ������ ���� Difference Process
���� ��� ����.
==========================================================================*/
int DIFFProcess( pDIFF_DATA ptrDiffData )
{
	return Internal_DIFFProcess( ptrDiffData );
}

/*=========================================================================
ALPR ó���� ���� Process
==========================================================================*/
int LPRProcess( pLPR_DATA ptrLPRData )
{
	return Internal_LPRProcess( ptrLPRData );
}

/*=========================================================================
LPR�� �ֿ� ��� �̿��� �ٸ� ó���� ���� Reserved Funtion
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