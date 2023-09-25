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
OCR �ν�
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
	[ ���� ���� �̹����� ���� Ư¡ ���� ���� ]	
	============================================================================*/
	nErrCode = FT_SetFeatureImage( ptrImageSrc, &Feature , -1 );
	if ( nErrCode != ERR_SUCCESS )
		return nErrCode;

	/*===========================================================================
	[ ������ Ư¡ ���͸� �н��� �Ű���� �����Ͽ� ���缺�� ���� ���� �н� ��������
	  ��ġ�� ���� �´� ]
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
	[ �Ű�������� ������ �н� �������� ��ġ�� ���� ���� ���� ���� ]
	============================================================================*/
	nErrCode = GetChar( nOCRPos , &ptrOCRData->OCRData );
		
	/*===========================================================================
	[ ���� ���� ���� ]
	============================================================================*/

	ptrOCRData->OCR_CODE = ( BYTE )nOCRPos;
	return ERR_SUCCESS;
}

/*=========================================================================
OCR ����( �н� ������ ���� )
==========================================================================*/
static int SaveOCRData( pOCR_DATA ptrOCRData )
{	
	int nErrCode = ERR_SUCCESS;
	pIMAGE_DATA ptrImageSrc = NULL;
	OCR_FEATURE_T Feature;

	ptrImageSrc = ptrOCRData->ptrImageDataSrc;	
	memset( &Feature, 0, sizeof( OCR_FEATURE_T ));

	/*===========================================================================
	[ ���� ���� �̹����� ���� Ư¡ ���� ���� ]	
	============================================================================*/
	nErrCode = FT_SetFeatureImage( ptrImageSrc, &Feature , (int)ptrOCRData->OCR_CODE );
	if ( nErrCode != ERR_SUCCESS )
		return nErrCode;
	
	/*===========================================================================
	[ ���ο� �н�������( Ư¡���� ) ����( ���常 �ϰ� �� �н��� ���� ���� ) ]	
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
�������� �б�
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
OCR �ʱ�ȭ
MLP( Multilayer perceptron )�ʱ�ȭ
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
MLP( Multilayer perceptron ) ����
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
fnNotifyCallback : int, double �������� �Ű������� �޴� callback �Լ� ������ 
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