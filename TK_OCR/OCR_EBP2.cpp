/*
	creator: Hyeongyong, Jeon (fantajeon@cnu.ac.kr)
	         NN_EBP.C
			 http://blog.naver.com/PostView.nhn?blogId=fantajeon&logNo=80052255150

	modify : Tory Hwang( tory45@empal.com )
*/

#include "stdafx.h"
#include <malloc.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <time.h>

#include "../include/TK_TYPE.h"
#include "../include/TK_LIB_Helper.h"
#include "../include/TK_LIB_DEBUG.h"
#include "../include/TK_CONF.h"

#include "OCR_COM.h"
#include "OCR_EBP2.h"

static int m_bStudy = 0;
static int m_bLearing = 0;
static volatile int m_forcesave = 0;
static int m_bCreateBuf = 0;


static float	**m_bias_k			= NULL; /* ���� ����� �޸� bias weight */
static float	**m_delta_k			= NULL;	/* ���� ������ ����(��Ÿ) ���� */
static float	**m_biasmoment_k	= NULL;	/* ���� bais_weight���� ���Ʈ �� ���� */
static float	**m_node			= NULL;	/* ���� ������ ��� �� */
static float	**m_study_node		= NULL;	/* ���� ������ ��� �� */

static float	***m_moment_k		= NULL;	/* ���� weight���� ���Ʈ �� ���� */
static float	***m_weight			= NULL;	/* ��� ������ share�� �ؾ��Ѵ�. [layer][i][j] : from i node on layer th to j node on layer+1 th.*/

static int		m_LayerCnt			= 0;	/* Layer�� ���� */
static float	m_ErrRate			= 0;	/* 0.5*LEAST SQUARED ERROR */


static int ReleaseBuffer( void );
static int ReleaseNNBuffer( void );

static int m_Layerinfo[] = { INPUT_LAYER_NODE_CNT ,EBP_HD_1_NODE_CNT,EBP_OUTPUT_NODE_CNT };
//static int m_Layerinfo[] = { INPUT_LAYER_NODE_CNT, ( EBP_OUTPUT_NODE_CNT * 3 ), ( EBP_OUTPUT_NODE_CNT * 2 ), EBP_OUTPUT_NODE_CNT };
static float m_out_data[ EBP_OUTPUT_NODE_CNT ] ;

/*=========================================================================
�н��� ����ġ ���� �б�
�н��� ����ġ : Input Weight, Hidden Weight 
==========================================================================*/
static int SaveStudiedWeight( void )
{
	int i,j,k;
	int nLayerCnt, nNodeCnt, nPreNodeCnt;

	float *ptrBuff= NULL;	
	FILE *ptrFp= NULL;
	char szpath[ MAX_PATH ];
	/*===============================================
	BIAS
	=================================================*/
	nLayerCnt = m_LayerCnt;
	nNodeCnt = 0;

	memset( szpath, 0, sizeof( szpath));
	GetWinCurPath( szpath, TK_EBP2_BIAS_DATA );

	ptrFp=fopen(szpath,"wb+");
	if ( ptrFp == NULL )  return ERR_INVALID_DATA;	
	for( i = 1; i < nLayerCnt ; i++ )
	{
		nNodeCnt = m_Layerinfo[ i ];
		ptrBuff = m_bias_k[ i ];
		fwrite((char*)ptrBuff,sizeof(float), nNodeCnt, ptrFp);
	}	
	if ( ptrFp != NULL )
	{
		fclose(ptrFp);
		ptrFp = NULL;
	}
			
	/*===============================================
	NODE
	=================================================*/
	memset( szpath, 0, sizeof( szpath));
	GetWinCurPath( szpath, TK_EBP2_NODE_DATA );

	ptrFp=fopen(szpath,"wb+");
	if ( ptrFp == NULL ) return ERR_INVALID_DATA;
		
	nNodeCnt = 0;
	for( i = 1; i < nLayerCnt ; i++ )
	{
		nNodeCnt = m_Layerinfo[ i ];
		ptrBuff = m_node[ i ];
		fwrite((char*)ptrBuff,sizeof(float), nNodeCnt, ptrFp);
	}
	if ( ptrFp != NULL )
	{
		fclose(ptrFp);
		ptrFp = NULL;
	}

	/*===============================================
	Weight
	=================================================*/
	memset( szpath, 0, sizeof( szpath));
	GetWinCurPath( szpath, TK_EBP2_WEIGHT_DATA );

	ptrFp=fopen(szpath,"wb+");
	if ( ptrFp == NULL ) return ERR_INVALID_DATA;
		
	nNodeCnt = 0;
	nPreNodeCnt = 0;

	for( i = 1; i < nLayerCnt ; i++ )
	{
		nNodeCnt	= m_Layerinfo[ i ];
		nPreNodeCnt = m_Layerinfo[ i -1 ];

		for( k = 0; k < nPreNodeCnt ; k++ )
		{
			ptrBuff = m_weight[ i ][ k ];
			fwrite((char*)ptrBuff,sizeof(float), nNodeCnt, ptrFp);			
		}		
	}

	if ( ptrFp != NULL )
	{
		fclose(ptrFp);
		ptrFp = NULL;
	}

	return ERR_SUCCESS;
}


/*=========================================================================
�н��� ����ġ ���� �б�
�н��� ����ġ : Input Weight, Hidden Weight 
==========================================================================*/
static int ReadStudiedWeight( void )
{
	int i,j,k;
	int nLayerCnt, nNodeCnt, nPreNodeCnt;

	float *ptrBuff= NULL;	
	FILE *ptrFp= NULL;
	char szpath[ MAX_PATH ];

	/*===============================================
	BIAS
	=================================================*/
	nLayerCnt = m_LayerCnt;
	nNodeCnt = 0;

	memset( szpath, 0, sizeof( szpath));
	GetWinCurPath( szpath, TK_EBP2_BIAS_DATA );

	ptrFp=fopen(szpath,"rb");
	if ( ptrFp == NULL )  return ERR_INVALID_DATA;	

	for( i = 1; i < nLayerCnt ; i++ )
	{
		nNodeCnt = m_Layerinfo[ i ];
		ptrBuff = m_bias_k[ i ];
		fread((char*)ptrBuff,sizeof(float), nNodeCnt, ptrFp);		
	}	
	if ( ptrFp != NULL )
	{
		fclose(ptrFp);
		ptrFp = NULL;
	}

	/*===============================================
	NODE
	=================================================*/
	nLayerCnt = m_LayerCnt;
	nNodeCnt = 0;

	memset( szpath, 0, sizeof( szpath));
	GetWinCurPath( szpath, TK_EBP2_NODE_DATA );

	ptrFp=fopen(szpath,"rb");
	if ( ptrFp == NULL )  return ERR_INVALID_DATA;

	for( i = 1; i < nLayerCnt ; i++ )
	{
		nNodeCnt = m_Layerinfo[ i ];
		ptrBuff = m_node[ i ];
		fread((char*)ptrBuff,sizeof(float), nNodeCnt, ptrFp);
		memcpy( m_study_node[i], ptrBuff,  sizeof(float) * nNodeCnt);
	}	
	if ( ptrFp != NULL )
	{
		fclose(ptrFp);
		ptrFp = NULL;
	}

	/*===============================================
	Weight
	=================================================*/
	memset( szpath, 0, sizeof( szpath));
	GetWinCurPath( szpath, TK_EBP2_WEIGHT_DATA );

	ptrFp=fopen(szpath,"rb");
	if ( ptrFp == NULL ) return ERR_INVALID_DATA;
		
	nNodeCnt = 0;
	nPreNodeCnt = 0;

	for( i = 1; i < nLayerCnt ; i++ )
	{
		nNodeCnt	= m_Layerinfo[ i ];
		nPreNodeCnt = m_Layerinfo[ i -1 ];

		for( k = 0; k < nPreNodeCnt ; k++ )
		{
			ptrBuff = m_weight[ i ][ k ];
			fread((char*)ptrBuff,sizeof(float), nNodeCnt, ptrFp);
		}
	}

	if ( ptrFp != NULL )
	{
		fclose(ptrFp);
		ptrFp = NULL;
	}

	m_bStudy = 1;
	return ERR_SUCCESS;
}


static void LSE( float * ptroupt_node )
{
	float tmp;
	int i;

	m_ErrRate = 0;	

	for( i=0; i < m_Layerinfo[m_LayerCnt-1]; i++) 
	{
		tmp		   = ( m_node[m_LayerCnt-1][i] - ptroupt_node[i] );
		m_ErrRate += ( tmp * tmp );
	}
#if 1
	m_ErrRate *= 0.5;
#endif

}

static int SetNodeWithSigmoid( pOCR_FEATURE ptrFeature )
{
	int nCurNodeCnt = 0;
	int nPreNodeCnt = 0;
	int x, j, k;
	float sum;

	/* �Է��� ��忡 ������ ���� */	
	for( j = 0 ; j < INPUT_LAYER_NODE_CNT ; j++ )
	{
		m_node[0][j] =(ptrFeature->Feature[j]);
	}	
	
	/* �Է��� ���� �� ����� ��� */
	for(x=1; x < m_LayerCnt; x++) 
	{
		/* ���� Layer �� ��� */
		nCurNodeCnt = m_Layerinfo[x];
		nPreNodeCnt = m_Layerinfo[x-1];

		for(k=0; k < nCurNodeCnt ; k++) 
		{
			sum = 0;
			/* ���� Layer �� ��� */
			for(j=0; j < nPreNodeCnt ; j++) 
			{
				sum += m_weight[x][j][k] * m_node[x-1][j];
			}

			m_node[x][k] = OCR_COM_SigmoidVal(sum + m_bias_k[x][k]);
		}
	}
	
	return ERR_SUCCESS;
}


static int Learning_NeuralNetwork( pOCR_FEATURE ptrFeature  )
{
	int x, j, k ;
	float sum;
	float tmp_delta_k;
	
	SetNodeWithSigmoid( ptrFeature );

	memset(m_out_data, 0, sizeof(m_out_data));
	m_out_data[  ptrFeature->OCR_CODE ] = 1.0;

	// calculate delta error	
	/* STEP 1 : Output layer ( 1�� )������( delta ) ��� */
	for( k=0; k < m_Layerinfo[m_LayerCnt-1]; k++) 
	{
		m_delta_k[m_LayerCnt-1][k] = ( m_out_data[k] - m_node[m_LayerCnt-1][k] ) * ( 1.0 - m_node[m_LayerCnt-1][k])*m_node[m_LayerCnt-1][k];
	}

	/* STEP 2 : Hidden Layer ( N�� ) ������( delta ) ��� */
	for( x= m_LayerCnt -2 ; x > 0 ; x--) 
	{
		// for Hidden node
		for( k=0; k < m_Layerinfo[x]; k++) 
		{
			sum = 0.0;
			for( j=0; j < m_Layerinfo[x+1]; j++)
			{
				sum += m_delta_k[x+1][j]*m_weight[x+1][k][j];
			}

			m_delta_k[x][k] = sum * (1.0-m_node[x][k])*m_node[x][k];
		}
	}
	
	/* STEP 3 :  calculate all weightes & moment terms...*/	
	for( x=1; x < m_LayerCnt; x++) 
	{
		for( j=0; j < m_Layerinfo[x]; j++) 
		{
			for( k=0; k < m_Layerinfo[x-1]; k++) 
			{
				// update m_weight from node kth to node jth on layer x
				tmp_delta_k = NN_ETHA * m_delta_k[x][j] * m_node[x-1][k];
				m_weight[x][k][j] += tmp_delta_k + NN_MOMENT*m_moment_k[x][k][j];
				m_moment_k[x][k][j] = tmp_delta_k;
			}

			tmp_delta_k = NN_ETHA*m_delta_k[x][j];
			m_bias_k[x][j] += tmp_delta_k + NN_MOMENT*m_biasmoment_k[x][j];
			m_biasmoment_k[x][j] = tmp_delta_k;	// BUGFIX: added 2008.06.09.
		}
	}

	LSE( m_out_data );
	return ERR_SUCCESS;
}

/*=========================================================================
BPNN( Back-Propagation Neural Network ) �ʱⰪ ����
�Ű�� �н� 
==========================================================================*/
static int RunStudyAndSaveWeight( pOCR_CONF ptrOCRConf )
{	
	int j ;
	
	int nLoopCnt =0;
	int nTrainCnt =0;
	pOCR_FEATURE ptrFeature= NULL;
	pOCR_FETURE_MANAGER ptrFeatureManager = NULL;

	int nStudyCnt;
	float fErr =0.0;
	float fSqureErr = 0.0;
	int nResult = ERR_SUCCESS;

	ptrFeatureManager = OCR_GetFeatureManager();
	if ( ptrFeatureManager == NULL )
		return ERR_NOT_STUDY_MLP;

	nStudyCnt = ptrFeatureManager->nCnt;
	m_bLearing =1;

	while( m_bLearing && m_forcesave == 0 )
	{

		fErr =0.0;
		for(j=0; j <  nStudyCnt ; j++) 
		{
			ptrFeature = ( pOCR_FEATURE)&ptrFeatureManager->ptrFeatureVector[ j ];

			if( ptrFeature != NULL )
			{	
				Learning_NeuralNetwork( ptrFeature );
				
				fErr += m_ErrRate;
				nTrainCnt++;
			}
		}

		nLoopCnt++;
		//fSqureErr = fErr/(nStudyCnt*OCR_MAX_CODE);		
		fSqureErr = fErr/(float)nStudyCnt;
		OCR_COM_SendNotifyStatus( nLoopCnt, fSqureErr );
		if( fSqureErr < NN_MIN_ERR || nLoopCnt > ptrOCRConf->OCR_LEARNING_CNT )
		{
			m_bLearing = 0;
		}
	}

	nResult = SaveStudiedWeight();
	m_forcesave = 0;
	m_bLearing = 0;
	
	return nResult;
}

/*=========================================================================
��ü �ν� �Ű��
==========================================================================*/
static int Recognition_NeuralNetwork( pOCR_FEATURE ptrFeature )
{
	int i, x, nCurNodeCnt;
	
	float max_fval = -9999999;
	float fval =0;

	int charID = -1;
	int nNodeCnt = 0;

	if ( m_bStudy == 0 )
	{
		return ERR_NOT_STUDY_MLP;
	}

	/* �н��� node�� �ʱ�ȭ �Ѵ� */	
#if 0
	for(x=0; x < m_LayerCnt; x++) 
	{
		nCurNodeCnt			=  m_Layerinfo[x];
		
		memset(m_node[x], 0, sizeof(float)* nCurNodeCnt );
		memcpy( m_node[x], m_study_node[x], sizeof(float)* nCurNodeCnt );
	}	
#endif

	SetNodeWithSigmoid( ptrFeature );

	/* ��������� ���� ���� ���� ���� ������ ã�� ����  */
	nNodeCnt = m_Layerinfo[ m_LayerCnt-1 ];
	for( i=0; i < nNodeCnt; i++) 
	{
		fval = m_node[ m_LayerCnt-1 ][ i ];

		if( max_fval < fval  ) 
		{
			charID = i;
			max_fval= fval;
		}
	}
	
	/* �ִ� ���� �ּ� 0.5 �̻��϶� ��ȿ�ϴٰ� �Ǵ�.  */
	if (max_fval > 0.001 )
	{
		return charID;
	}
	
	return ERR_NOT_MATCH_MLP;	
}

static int CreateNNBuffer( void )
{	
	int x, j, k;
	int nCurNodeCnt;
	int nPreNodeCnt;
	time_t t;

	if ( m_bCreateBuf != 0 )
		return ERR_SUCCESS;

	srand( (unsigned)time(&t) );
	m_LayerCnt = sizeof( m_Layerinfo )/ sizeof( int );

	/* Layer ������ŭ�� ���� */
	
	m_delta_k			= new float*[m_LayerCnt];
	m_bias_k			= new float*[m_LayerCnt];
	m_node				= new float*[m_LayerCnt];
	m_study_node		= new float*[m_LayerCnt];
	m_biasmoment_k		= new float*[m_LayerCnt];

	m_moment_k			= new float**[m_LayerCnt];
	m_weight			= new float**[m_LayerCnt];
	

	/* �� Layer�� ��带 ���� �Ѵ� */
	for(x=0; x < m_LayerCnt; x++) 
	{
		nCurNodeCnt			=  m_Layerinfo[x];
		m_node[x]			= new float[ nCurNodeCnt ];
		m_study_node[x]		= new float[ nCurNodeCnt ];

		memset(m_node[x], 0, sizeof(float)* nCurNodeCnt );
		memset(m_study_node[x], 0, sizeof(float)* nCurNodeCnt );

	}

	/* ��� ������ m_weight ������ �����Ѵ�(node <--> node) */
	for(x= 0; x < m_LayerCnt; x++) 
	{
		nCurNodeCnt			=  m_Layerinfo[x];
		m_delta_k[x]		= new float[ nCurNodeCnt ];
		m_bias_k[x]			= new float[ nCurNodeCnt ];
		m_biasmoment_k[x]	= new float[ nCurNodeCnt ];

		memset(m_delta_k[x], 0, sizeof(float)*nCurNodeCnt );
		memset(m_bias_k[x],  0, sizeof(float)*nCurNodeCnt );
		memset(m_biasmoment_k[x], 0, sizeof(float)*nCurNodeCnt );
	}

	/* �ʱ� ����ġ, ���Ʈ ���� 
	   Weight  : 3�� �迭 , m_weight[ ����Layer�� ��� �� ][ ���� Layer�� ��� ��  ][ ���� ����ġ ]
	             ���� Layer�� ���� ���� Layer�� ��� ������ ����ġ ó�� 
	   Moment : ���� Layer�� ��忡 ���� Moment ó�� 
	*/
	for(x=1; x < m_LayerCnt; x++) 
	{
		nCurNodeCnt = m_Layerinfo[ x ];
		nPreNodeCnt = m_Layerinfo[ x -1 ];

		m_weight[x]	= new float*[ nPreNodeCnt ];
		m_moment_k[x]= new float*[ nPreNodeCnt ];

		for(k=0; k < nPreNodeCnt ; k++) 
		{
			m_weight[x][k] = new float[ nCurNodeCnt ];
			m_moment_k[x][k] = new float[ nCurNodeCnt ];

			memset(m_weight[x][k], 0, sizeof(float)*nCurNodeCnt);
			memset(m_moment_k[x][k], 0, sizeof(float)*nCurNodeCnt);

			for(j=0; j < nCurNodeCnt ; j++) 
			{
				//m_weight[x][k][j] = 4.0* OCR_COM_RandVal();
				m_weight[x][k][j] = OCR_COM_RandVal();
			}
		}
	}

	m_bias_k[0] = m_delta_k[0] = NULL; 
	m_biasmoment_k[0] = NULL; 
	m_weight[0] = NULL;
	m_ErrRate = 0; 
	m_bCreateBuf =1;

	return ERR_SUCCESS;	
}


/*=========================================================================
Ư¡���� ���� �޸� ���� 
Neural Network ���, ����ġ �޸� ����
==========================================================================*/
static int ReleaseNNBuffer( void )
{

	int x,k;
	// ��� ������ m_weight ������ �ı��Ѵ�.

	
	for(x=0; x < m_LayerCnt; x++) 
	{
		if ( m_node != NULL && m_node[x] != NULL ) delete [] m_node[x];
	}	
	
	for(x=0; x < m_LayerCnt; x++) 
	{
		if ( m_delta_k != NULL && m_delta_k[x] != NULL ) delete [] m_delta_k[x];
		if( m_bias_k != NULL && m_bias_k[x] != NULL ) delete [] m_bias_k[x];
		if( m_biasmoment_k != NULL && m_biasmoment_k[x] != NULL ) delete [] m_biasmoment_k[x];
	}

	for(x=1; x < m_LayerCnt; x++) 
	{
		for(k=0; k < m_Layerinfo[x-1]; k++) 
		{
			if ( m_weight != NULL && m_weight[x][k] != NULL ) delete [] m_weight[x][k];
			if ( m_moment_k != NULL && m_moment_k[x][k] != NULL ) delete [] m_moment_k[x][k];
		}

		if ( m_weight != NULL && m_weight[x] != NULL )	delete [] m_weight[x];
		if ( m_moment_k != NULL && m_moment_k[x] != NULL )	delete [] m_moment_k[x];
	}	
	
	if( m_delta_k != NULL ) delete [] m_delta_k;
	if( m_bias_k != NULL ) delete [] m_bias_k; 
	if( m_node != NULL ) delete [] m_node;
	if( m_study_node != NULL ) delete [] m_study_node;

	if( m_biasmoment_k != NULL ) delete [] m_biasmoment_k; 
	if( m_moment_k != NULL ) delete [] m_moment_k;
	if( m_weight != NULL ) delete [] m_weight;
	
	m_delta_k		= NULL;
	m_bias_k		= NULL; 
	m_node			= NULL;
	m_study_node	= NULL;

	m_moment_k		= NULL; 
	m_biasmoment_k	= NULL; 
	m_weight		= NULL;

	m_ErrRate		= 0;
	m_bCreateBuf	= 0;
	
	return ERR_SUCCESS;
}

static int ReleaseBuffer( void )
{	
	OCR_COM_RleaseFeatureBuf();
	ReleaseNNBuffer();
	m_bStudy = 0;

	return ERR_SUCCESS;
}

/*=========================================================================
EBP( Error Back-propgation Neural Network ) �ʱ�ȭ

1. ���� �޸� ���� 
2. �н��� ����ġ ���� 
==========================================================================*/
int EBP2_Initialize( int bCreate )
{
	int nErrCode = ERR_SUCCESS;	

	/*===========================================================================
	[ �н�������( Ư¡ ����)�� ���� ���� ���� ]	
	============================================================================*/
	if ( bCreate )
	{
		nErrCode = CreateNNBuffer(  );
		if ( nErrCode != ERR_SUCCESS )
			return nErrCode;
	}
	/*===========================================================================
	[ �н��� Weight���� ���Ͽ��� ���� ]	
	Input Weight, Hidden Weight
	============================================================================*/
	nErrCode = ReadStudiedWeight( );
	if ( nErrCode != ERR_SUCCESS )
		return nErrCode ;

	return ERR_SUCCESS;
}

/*=========================================================================
Ư¡ ���� ���� ����
���ο� �н�������( Ư¡ ���� )�� ���� ( �� �н��� ���� ���� )
==========================================================================*/
int EBP2_Save_NeuralNetwork( pOCR_FEATURE ptrFeature )
{
	if ( ptrFeature ==NULL )
		return ERR_INVALID_DATA;
	
	return OCR_COM_SaveFeature( ptrFeature );
}

/*=========================================================================
��ü �ν�
LPR �����ܰ迡�� �Ѿ�� ���� �̹����� Ư¡�� ����
==========================================================================*/
int EBP2_Recog_NeuralNetwork( pOCR_FEATURE ptrFeature )
{	
	return Recognition_NeuralNetwork( ptrFeature );
}

/*=========================================================================
Ư¡ ���Ϳ� ���� �Ű�� �� �н�

���Ͽ� ����� Ư¡ ������ ������ BPNN( Back-Propagation Neural Network )�� �̿���
�н� ��Ŵ 
==========================================================================*/
int EBP2_ReStudy( void(* fnNotifyCallback)(int , double ),  pOCR_CONF ptrOCRConf )
{
	int nErrCode = ERR_SUCCESS;
	
	ReleaseBuffer();
	OCR_COM_SetCallBackFunc( fnNotifyCallback);

	if( OCR_COM_CreateFeatureBuf() != ERR_SUCCESS )
	{
		ReleaseBuffer();
		return ERR_ALLOC_BUF;
	}

	nErrCode = CreateNNBuffer(  );
	if ( nErrCode != ERR_SUCCESS )
		return nErrCode ;
		
	/*===========================================================================
	[ �н��� �������� �޸� ���� ]	
	============================================================================*/
	nErrCode = OCR_COM_ReadFeature();
	if ( nErrCode != ERR_SUCCESS )
		return nErrCode ;
	
	/*===========================================================================
	[ �Ű�� �н� ]	
	============================================================================*/
	nErrCode = RunStudyAndSaveWeight( ptrOCRConf );
	if ( nErrCode != ERR_SUCCESS )
		return nErrCode ;

	/*===========================================================================
	[ �� �н��� ���� ���� ����ġ ���� �б� ]	
	============================================================================*/
	nErrCode = ReadStudiedWeight( );
	if ( nErrCode != ERR_SUCCESS )
		return nErrCode ;
	
	return ERR_SUCCESS;
}

/*=========================================================================
�н� �Ű�� ���� ����
==========================================================================*/
int EBP2_NN_ForceSave( void )
{
	if( m_bLearing )
	{
		m_forcesave = 1;
	}
	else
		return ERR_INVALID_DATA;

	return ERR_SUCCESS;
}


int EBP2_Release( void )
{
	ReleaseBuffer();
	return ERR_SUCCESS;
}