#include "stdafx.h"
#include <malloc.h>
#include <stdlib.h>
#include <math.h>

#include "../include/TK_TYPE.h"
#include "../include/TK_LIB_Helper.h"
#include "../include/TK_CONF.h"

#include "OCR.h"
#include "OCR_EBP.h"
#include "OCR_EBP2.h"
#include "OCR_COM.h"
#include "OCR_FEATURES.h"

/*==================================================================================
DIP LIB Version
===================================================================================*/
#define OCR_VER_1ST		0x00
#define OCR_VER_2ND		0x02
/*==================================================================================*/

typedef int ( * OCRProcFunc )( pOCR_DATA );

typedef struct OCRProcFuncs_
{
	UINT16 uProcMsg;
	OCRProcFunc ptrFunc;
}OCRProcFuncs_t;

/*========================================================
Static Variance
==========================================================*/
static int nFuncCnt=0;
static pOCR_CONF m_ptrocr_conf = NULL;

/*========================================================
Static Functions 
==========================================================*/
static int RecogOCRData( pOCR_DATA ptrOCRData );
static int SaveOCRData( pOCR_DATA ptrOCRData );
static int ReleaseTempPrj( void );

/*========================================================
Storage Of Static Functions
==========================================================*/
static OCRProcFuncs_t OCRProcFuncs[ ] =
{
	{ TK_OCR_SAVE_MSG, SaveOCRData },
	{ TK_OCR_RECOG_MSG, RecogOCRData }
};

static int GetChar( int nOCRPos, unsigned short * ptrsz )
{
	if ( nOCRPos <= 0 || nOCRPos > OCR_MAX_CODE)
	{	
		*ptrsz = STR_NOT_EXIST;
	}
	else
	{
		memcpy( ptrsz, &OCR_UNIDATA[ nOCRPos ], sizeof( unsigned short ));	
	}
	return ERR_SUCCESS;
}

/*=========================================================================
OCR 인식
==========================================================================*/
static int RecogOCRData( pOCR_DATA ptrOCRData )
{
	int nErrCode = ERR_SUCCESS;
	int nOCRPos = 0;

	pIMAGE_DATA ptrImageSrc = NULL;
	OCR_FEATURE_T Feature;

	ptrImageSrc = ptrOCRData->ptrImageDataSrc;
	memset( &Feature, 0, sizeof( OCR_FEATURE_T ));	
	
	/*===========================================================================
	[ 받은 문자 이미지에 대한 특징 벡터 생성 ]	
	============================================================================*/
	nErrCode = FT_SetFeatureImage( ptrImageSrc, &Feature , -1 );
	if ( nErrCode != ERR_SUCCESS )
		return nErrCode;

	/*===========================================================================
	[ 생성된 특징 벡터를 학습된 신경망에 투입하여 유사성이 높은 기존 학습 데이터의
	  위치를 가져 온다 ]
	============================================================================*/
	if( m_ptrocr_conf->OCR_NN_TYPE == DEFAULT_NN_TYPE )
	{
		
		nOCRPos = EBP_Recog_NeuralNetwork( &Feature );
	}
	else
	{
		nOCRPos = EBP2_Recog_NeuralNetwork( &Feature );
	}
	
	/*===========================================================================
	[ 신경망에의해 구해진 학습 데이터의 위치에 대한 문자 정보 인출 ]
	============================================================================*/
	nErrCode = GetChar( nOCRPos , &ptrOCRData->OCRData );
		
	/*===========================================================================
	[ 문자 순번 설정 ]
	============================================================================*/

	ptrOCRData->OCR_CODE = ( BYTE )nOCRPos;
	return ERR_SUCCESS;
}

/*=========================================================================
OCR 저장( 학습 데이터 저장 )
==========================================================================*/
static int SaveOCRData( pOCR_DATA ptrOCRData )
{	
	int nErrCode = ERR_SUCCESS;
	pIMAGE_DATA ptrImageSrc = NULL;
	OCR_FEATURE_T Feature;

	ptrImageSrc = ptrOCRData->ptrImageDataSrc;	
	memset( &Feature, 0, sizeof( OCR_FEATURE_T ));

	/*===========================================================================
	[ 받은 문자 이미지에 대한 특징 벡터 생성 ]	
	============================================================================*/
	nErrCode = FT_SetFeatureImage( ptrImageSrc, &Feature , (int)ptrOCRData->OCR_CODE );
	if ( nErrCode != ERR_SUCCESS )
		return nErrCode;
	
	/*===========================================================================
	[ 새로운 학습데이터( 특징벡터 ) 저장( 저장만 하고 재 학습은 하지 않음 ) ]	
	============================================================================*/
	if( m_ptrocr_conf->OCR_NN_TYPE == DEFAULT_NN_TYPE )	
	{
		nErrCode = EBP_Save_NeuralNetwork( &Feature );		
	}
	else
	{
		nErrCode = EBP2_Save_NeuralNetwork( &Feature );
	}
	
	if ( nErrCode != ERR_SUCCESS )
		return nErrCode;


	return ERR_SUCCESS;
}

/*===========================================================================
설정정보 읽기
============================================================================*/
static int Load_OCR_Conf_Data( pLIB_INIT_DATA ptrInitData )
{
	if( ptrInitData != NULL )
	{
		SetWinCurPath( ptrInitData->szwinpath);
		ptrInitData->Version[ 0 ] = OCR_VER_1ST;
		ptrInitData->Version[ 1 ] = OCR_VER_2ND;
	}

	if ( loadOCRConf() == ERR_SUCCESS )
	{
		m_ptrocr_conf = GetOCRConf();
		FT_SetOCRConf( m_ptrocr_conf );
	}

	return ERR_SUCCESS;
}

/*=========================================================================
OCR 초기화
MLP( Multilayer perceptron )초기화
==========================================================================*/
int Initialize( pLIB_INIT_DATA ptrInitData  )
{
	int nResult = ERR_SUCCESS;
	nFuncCnt = ( sizeof( OCRProcFuncs) / sizeof( OCRProcFuncs_t ) );	
	
	Load_OCR_Conf_Data( ptrInitData );

	nResult= FT_Initialize( );

	if ( nResult == ERR_SUCCESS )
	{
		if( m_ptrocr_conf->OCR_NN_TYPE == DEFAULT_NN_TYPE )	
		{
			nResult = EBP_Initialize( 1 );
		}
		else
		{
			nResult = EBP2_Initialize( 1 );	
		}
	}

	return nResult;
}

/*=========================================================================
MLP( Multilayer perceptron ) 해제
==========================================================================*/
int Release( void  )
{
	FT_Release();

	if( m_ptrocr_conf->OCR_NN_TYPE == DEFAULT_NN_TYPE )	
	{
		EBP_Release();
	}
	else
	{
		EBP2_Release();
	}

	return ERR_SUCCESS;
}

/*=========================================================================
OCR Process
typedef struct OCR_DATA_
{
	UINT16 uOCRMsg;
	BOOL bValid;
	pIMAGE_DATA ptrImageDataSrc;
	BYTE OCR_CODE;
	unsigned short OCRData;	
}OCR_DATA_t, * pOCR_DATA;

TK_OCR_SAVE_MSG	: SaveOCRData
TK_OCR_RECOG_MSG	: RecogOCRData

==========================================================================*/
int OCRProcess( pOCR_DATA ptrOCRData )
{
	int i =0;
	OCRProcFuncs_t * ptrFuncT= NULL;
	UINT16 l_uMsg;
	OCRProcFunc l_ptrFunc = NULL ;

	for( i = 0 ; i < nFuncCnt ; i++ )
	{
		ptrFuncT = ( OCRProcFuncs_t *  )&OCRProcFuncs[ i ];
		if ( ptrFuncT != NULL )
		{
			l_uMsg = ptrFuncT->uProcMsg;
			l_ptrFunc = ptrFuncT->ptrFunc;

			if ( l_uMsg != 0x0000 && l_ptrFunc != 0x00  && ptrOCRData != NULL )
			{
				if ( l_uMsg == ptrOCRData->uOCRMsg ) 
				{
					return l_ptrFunc( ptrOCRData );
				}
			}			
		}
		else
			break;
	}
	return ERR_NO_PARAM;
}

/*=========================================================================
OCR Study
fnNotifyCallback : int, double 인자형을 매개변수로 받는 callback 함수 포인터 
==========================================================================*/
int OCRRestudy( void(* fnNotifyCallback)(int , double) )
{
	int nResult = ERR_SUCCESS;

	if( m_ptrocr_conf->OCR_NN_TYPE == DEFAULT_NN_TYPE )	
	{
		
		nResult= EBP_ReStudy( fnNotifyCallback,  m_ptrocr_conf);
	}
	else
	{
		nResult= EBP2_ReStudy( fnNotifyCallback , m_ptrocr_conf);
	}

	return nResult;
}

/*=========================================================================
OCR IOCTL
==========================================================================*/
int IOCTL( pIOCTL_DATA ptrIOCTLData )
{
	int nErrCode = ERR_SUCCESS;
	int nCnt =0;
	
	if( ptrIOCTLData->uIOCTLMsg == TK_IOCTL_RECONF_MSG )
	{
		nErrCode= Load_OCR_Conf_Data( NULL );
		
		if( m_ptrocr_conf->OCR_NN_TYPE == DEFAULT_NN_TYPE )	
		{
			nErrCode = EBP_Initialize( 0 );
		}
		else
		{
			nErrCode = EBP2_Initialize( 0 );	
		}

		return nErrCode;
	}
	else if( ptrIOCTLData->uIOCTLMsg == TK_IOCTL_FEATURECNT_MSG)
	{
		nCnt = OCR_COM_ReadFeatureOnlyCnt();
		memcpy( ptrIOCTLData->ptrOutput, &nCnt, sizeof( int ));
	}
	else if( ptrIOCTLData->uIOCTLMsg == TK_IOCTL_NN_FORCE_SAVE_MSG)
	{
		if( m_ptrocr_conf->OCR_NN_TYPE == DEFAULT_NN_TYPE )	
		{
			nErrCode= EBP_NN_ForceSave( );
		}
		else
		{
			nErrCode= EBP2_NN_ForceSave( );
		}
	}

	return nErrCode;
}