#include "stdafx.h"
#include <malloc.h>
#include <stdlib.h>
#include <math.h>

#include "../include/TK_TYPE.h"
#include "../include/TK_LIB_Helper.h"
#include "../include/TK_CONF.h"

#include "OCR_COM.h"

static OCR_FEATURE_MANAGER_t m_FeatureManager;
static int m_Create = 0;
static int m_FeactureCnt = 0;

static fnNotifyCallBack m_fnNotifyCallBack= NULL;


float OCR_COM_RandVal( void )
{
	float fval = 0;

	fval = (float)(2.0*(rand()/(float)RAND_MAX - 0.5));

	return fval; 
}

float OCR_COM_SigmoidVal( float fVal )
{
	float refval =0.0;

	refval = (float)(1.0/(1.0 + exp(-fVal)));
	return  refval;
}


int OCR_COM_CreateFeatureBuf( void )
{
	if ( m_Create == 0 && m_FeactureCnt > 0 )
	{
		memset( &m_FeatureManager, 0, sizeof( m_FeatureManager));
		m_FeatureManager.ptrFeatureVector = new OCR_FEATURE_T[ m_FeactureCnt ];
		if ( m_FeatureManager.ptrFeatureVector == NULL )
		{			
			return ERR_ALLOC_BUF;
		}

		m_Create = 1;
		return ERR_SUCCESS;
	}
	
	return ERR_ALLOC_BUF;
}

int OCR_COM_RleaseFeatureBuf( void )
{
	if ( m_Create ==1 && m_FeatureManager.ptrFeatureVector != NULL )
	{
		delete [] m_FeatureManager.ptrFeatureVector;
		m_FeatureManager.ptrFeatureVector = NULL;
		m_FeatureManager.nCnt = 0;
	}
	m_Create = 0;

	return ERR_SUCCESS;
}

pOCR_FETURE_MANAGER OCR_GetFeatureManager( void )
{
	return &m_FeatureManager;
}


/*=========================================================================
학습 데이터 저장
특징점 : ptrFeature ( 13개의 특징 벡터 )
==========================================================================*/
int OCR_COM_SaveFeature( pOCR_FEATURE ptrFeature )
{
	FILE * ptrFp = NULL;	
	int i;
	char szTemp[  50  *  ( INPUT_LAYER_NODE_CNT + 3) ];
	char szTemp2[ 50  * 2  ];
	char szpath[ MAX_PATH];

	if ( ( ptrFeature->OCR_CODE < 0x00 ) || ( ptrFeature->OCR_CODE > OCR_MAX_CODE ))
	{
		return ERR_INVALID_DATA;
	}

	memset( szTemp, 0, sizeof( szTemp ));
	memset( szTemp2, 0, sizeof( szTemp2 ));

	/*===========================================================================
	[ 파일 Open  ]	
	마지막 데이터에 추가 할 수 있도록 한다. 
	============================================================================*/
	memset( szpath, 0, sizeof( szpath));
	GetWinCurPath( szpath, TK_OCR_FEATURE_DATA );
	ptrFp = fopen( szpath,"a+");
	if ( ptrFp == NULL )
		return ERR_READ_STUDY_MLP;	
		
	/*===========================================================================
	[ 특징점 저장 버퍼  ]	
	마지막 특징점 뒤 \r\n을붙힌다. 
	============================================================================*/
	for( i = 0 ; i <= INPUT_LAYER_NODE_CNT ; i++ )
	{
		memset( szTemp2, 0, sizeof( szTemp2 ));
		if ( i == INPUT_LAYER_NODE_CNT )
		{	
			sprintf( szTemp2, "%d\r\n", (int)ptrFeature->OCR_CODE) ;
		}
		else
		{
			sprintf( szTemp2, "%f,", ptrFeature->Feature[ i ]) ;
		}
		strcat( szTemp,szTemp2);
	}

	fputs( szTemp, ptrFp );
	fclose( ptrFp );
	return ERR_SUCCESS;
}

int OCR_COM_ReadFeatureOnlyCnt( void)
{
	int i, nCnt = 0;	
	FILE * ptrFp = NULL;
	OCR_FEATURE_T TempFeature;
	char szpath[ MAX_PATH ];	
	int bCon ;


	memset( szpath, 0, sizeof( szpath));
	GetWinCurPath( szpath, TK_OCR_FEATURE_DATA );

	ptrFp = fopen( szpath,"rt");
	
	if ( ptrFp == NULL )
		return ERR_READ_STUDY_MLP;

	bCon =1;
	while( bCon)
	{
		if ( feof( ptrFp ) )
		{
			bCon = 0;
			break;
		}		

		memset( &TempFeature, 0, sizeof( OCR_FEATURE_T));
		for( i = 0 ; i <= INPUT_LAYER_NODE_CNT ; i++ )
		{
			if ( i == INPUT_LAYER_NODE_CNT )
			{				
				fscanf( ptrFp, "%d",&TempFeature.OCR_CODE);
			}
			else
			{
				fscanf( ptrFp, "%f,",&TempFeature.Feature[ i ]);
			}
		}

		nCnt++;
	}
	
	m_FeactureCnt= nCnt;
	nCnt--;

	fclose( ptrFp );

	return nCnt;
}

int OCR_COM_ReadFeature( void)
{
	FILE * ptrFp = NULL;
	OCR_FEATURE_T TempFeature;
	char szpath[ MAX_PATH ];

	memset( szpath, 0, sizeof( szpath));
	GetWinCurPath( szpath, TK_OCR_FEATURE_DATA );

	ptrFp = fopen( szpath,"rt");
	int i;
	int nZeroCnt ;
	int bCon ;

	if ( ptrFp == NULL || m_Create == 0  || m_FeactureCnt == 0 )
		return ERR_READ_STUDY_MLP;
	
	m_FeatureManager.nCnt = 0;

	bCon =1;
	while( bCon)
	{
		if ( feof( ptrFp ) )
		{
			bCon = 0;
			break;
		}

		memset( &TempFeature, 0, sizeof( OCR_FEATURE_T));

		for( i = 0 ; i <= INPUT_LAYER_NODE_CNT ; i++ )
		{
			if ( i == INPUT_LAYER_NODE_CNT )
			{				
				fscanf( ptrFp, "%d",&TempFeature.OCR_CODE);
			}
			else
			{
				fscanf( ptrFp, "%f,",&TempFeature.Feature[ i ]);
			}
		}

		if ( m_FeatureManager.nCnt < m_FeactureCnt )
		{
			nZeroCnt = 0;
			for( i = 0 ; i < INPUT_LAYER_NODE_CNT ; i++ )
			{
				if ( TempFeature.Feature[ i ] == 0 ) nZeroCnt++;				
			}

			if ( nZeroCnt != INPUT_LAYER_NODE_CNT)
			{
				memcpy( &m_FeatureManager.ptrFeatureVector[ m_FeatureManager.nCnt ] , &TempFeature, sizeof( OCR_FEATURE_T ));
				m_FeatureManager.nCnt++;
			}
		}
	}

	fclose( ptrFp );

	return ERR_SUCCESS;
}

int OCR_COM_SetCallBackFunc( fnNotifyCallBack ptrCallBack )
{
	m_fnNotifyCallBack = ptrCallBack;
	return ERR_SUCCESS;
}

int OCR_COM_SendNotifyStatus( int nVal, double dVal )
{
	if ( m_fnNotifyCallBack != NULL )
	{
		m_fnNotifyCallBack( nVal, dVal );
	}

	return ERR_SUCCESS;
}
