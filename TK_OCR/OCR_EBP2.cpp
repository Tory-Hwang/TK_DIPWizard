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


static float	**m_bias_k			= NULL; /* 각각 노드의 달린 bias weight */
static float	**m_delta_k			= NULL;	/* 각각 노드들의 에러(델타) 측정 */
static float	**m_biasmoment_k	= NULL;	/* 각각 bais_weight들의 모멘트 항 측정 */
static float	**m_node			= NULL;	/* 각각 노드들의 출력 값 */
static float	**m_study_node		= NULL;	/* 각각 노드들의 출력 값 */

static float	***m_moment_k		= NULL;	/* 각각 weight들의 모멘트 항 측정 */
static float	***m_weight			= NULL;	/* 모든 노드들이 share를 해야한다. [layer][i][j] : from i node on layer th to j node on layer+1 th.*/

static int		m_LayerCnt			= 0;	/* Layer의 개수 */
static float	m_ErrRate			= 0;	/* 0.5*LEAST SQUARED ERROR */


static int ReleaseBuffer( void );
static int ReleaseNNBuffer( void );

static int m_Layerinfo[] = { INPUT_LAYER_NODE_CNT ,EBP_HD_1_NODE_CNT,EBP_OUTPUT_NODE_CNT };
//static int m_Layerinfo[] = { INPUT_LAYER_NODE_CNT, ( EBP_OUTPUT_NODE_CNT * 3 ), ( EBP_OUTPUT_NODE_CNT * 2 ), EBP_OUTPUT_NODE_CNT };
static float m_out_data[ EBP_OUTPUT_NODE_CNT ] ;

/*=========================================================================
학습된 가중치 정보 읽기
학습된 가중치 : Input Weight, Hidden Weight 
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
학습된 가중치 정보 읽기
학습된 가중치 : Input Weight, Hidden Weight 
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

	/* 입력층 노드에 데이터 갱신 */	
	for( j = 0 ; j < INPUT_LAYER_NODE_CNT ; j++ )
	{
		m_node[0][j] =(ptrFeature->Feature[j]);
	}	
	
	/* 입력층 이후 각 노드의 계산 */
	for(x=1; x < m_LayerCnt; x++) 
	{
		/* 현재 Layer 의 노드 */
		nCurNodeCnt = m_Layerinfo[x];
		nPreNodeCnt = m_Layerinfo[x-1];

		for(k=0; k < nCurNodeCnt ; k++) 
		{
			sum = 0;
			/* 이전 Layer 의 노드 */
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
	/* STEP 1 : Output layer ( 1개 )에러률( delta ) 계산 */
	for( k=0; k < m_Layerinfo[m_LayerCnt-1]; k++) 
	{
		m_delta_k[m_LayerCnt-1][k] = ( m_out_data[k] - m_node[m_LayerCnt-1][k] ) * ( 1.0 - m_node[m_LayerCnt-1][k])*m_node[m_LayerCnt-1][k];
	}

	/* STEP 2 : Hidden Layer ( N개 ) 에러률( delta ) 계산 */
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
BPNN( Back-Propagation Neural Network ) 초기값 설정
신경망 학습 
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
객체 인식 신경망
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

	/* 학습된 node로 초기화 한다 */	
#if 0
	for(x=0; x < m_LayerCnt; x++) 
	{
		nCurNodeCnt			=  m_Layerinfo[x];
		
		memset(m_node[x], 0, sizeof(float)* nCurNodeCnt );
		memcpy( m_node[x], m_study_node[x], sizeof(float)* nCurNodeCnt );
	}	
#endif

	SetNodeWithSigmoid( ptrFeature );

	/* 출력층에서 가장 높은 값을 가진 순번을 찾아 낸다  */
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
	
	/* 최대 값이 최소 0.5 이상일때 유효하다고 판단.  */
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

	/* Layer 개수만큼의 생성 */
	
	m_delta_k			= new float*[m_LayerCnt];
	m_bias_k			= new float*[m_LayerCnt];
	m_node				= new float*[m_LayerCnt];
	m_study_node		= new float*[m_LayerCnt];
	m_biasmoment_k		= new float*[m_LayerCnt];

	m_moment_k			= new float**[m_LayerCnt];
	m_weight			= new float**[m_LayerCnt];
	

	/* 각 Layer의 노드를 생성 한다 */
	for(x=0; x < m_LayerCnt; x++) 
	{
		nCurNodeCnt			=  m_Layerinfo[x];
		m_node[x]			= new float[ nCurNodeCnt ];
		m_study_node[x]		= new float[ nCurNodeCnt ];

		memset(m_node[x], 0, sizeof(float)* nCurNodeCnt );
		memset(m_study_node[x], 0, sizeof(float)* nCurNodeCnt );

	}

	/* 모든 노드들의 m_weight 구조를 생성한다(node <--> node) */
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

	/* 초기 가중치, 모먼트 설정 
	   Weight  : 3차 배열 , m_weight[ 이전Layer의 노드 수 ][ 현재 Layer의 노드 수  ][ 임의 가충치 ]
	             이전 Layer의 노드와 현재 Layer의 노드 사이의 가중치 처리 
	   Moment : 현재 Layer의 노드에 대한 Moment 처리 
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
특징벡터 벡터 메모리 해제 
Neural Network 노드, 가중치 메모리 해제
==========================================================================*/
static int ReleaseNNBuffer( void )
{

	int x,k;
	// 모든 노드들의 m_weight 구조를 파괴한다.

	
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
EBP( Error Back-propgation Neural Network ) 초기화

1. 공용 메모리 생성 
2. 학습된 가중치 읽음 
==========================================================================*/
int EBP2_Initialize( int bCreate )
{
	int nErrCode = ERR_SUCCESS;	

	/*===========================================================================
	[ 학습데이터( 특징 벡터)를 담을 버퍼 생성 ]	
	============================================================================*/
	if ( bCreate )
	{
		nErrCode = CreateNNBuffer(  );
		if ( nErrCode != ERR_SUCCESS )
			return nErrCode;
	}
	/*===========================================================================
	[ 학습된 Weight값을 파일에서 읽음 ]	
	Input Weight, Hidden Weight
	============================================================================*/
	nErrCode = ReadStudiedWeight( );
	if ( nErrCode != ERR_SUCCESS )
		return nErrCode ;

	return ERR_SUCCESS;
}

/*=========================================================================
특징 벡터 파일 저장
새로운 학습데이터( 특징 벡터 )의 저장 ( 재 학습은 하지 않음 )
==========================================================================*/
int EBP2_Save_NeuralNetwork( pOCR_FEATURE ptrFeature )
{
	if ( ptrFeature ==NULL )
		return ERR_INVALID_DATA;
	
	return OCR_COM_SaveFeature( ptrFeature );
}

/*=========================================================================
객체 인식
LPR 최종단계에서 넘어온 문자 이미지의 특징점 벡터
==========================================================================*/
int EBP2_Recog_NeuralNetwork( pOCR_FEATURE ptrFeature )
{	
	return Recognition_NeuralNetwork( ptrFeature );
}

/*=========================================================================
특징 벡터에 의한 신경망 재 학습

파일에 저장된 특징 벡터의 정보를 BPNN( Back-Propagation Neural Network )을 이용해
학습 시킴 
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
	[ 학습용 데이터의 메모리 복사 ]	
	============================================================================*/
	nErrCode = OCR_COM_ReadFeature();
	if ( nErrCode != ERR_SUCCESS )
		return nErrCode ;
	
	/*===========================================================================
	[ 신경망 학습 ]	
	============================================================================*/
	nErrCode = RunStudyAndSaveWeight( ptrOCRConf );
	if ( nErrCode != ERR_SUCCESS )
		return nErrCode ;

	/*===========================================================================
	[ 재 학습을 통해 얻은 가중치 새로 읽기 ]	
	============================================================================*/
	nErrCode = ReadStudiedWeight( );
	if ( nErrCode != ERR_SUCCESS )
		return nErrCode ;
	
	return ERR_SUCCESS;
}

/*=========================================================================
학습 신경망 강제 저장
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