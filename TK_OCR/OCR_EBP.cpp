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
#include "OCR_EBP.h"

#define NN_MIN_VALIDRATE						0.5

//RAND_MAX 0x7fff
//#define RAND_MAX 32767

static int m_nInputCnt  =INPUT_LAYER_NODE_CNT;
static int m_nHiddenCnt = MLP_HD_NODE_CNT;
static int m_nOutputCnt = MLP_OUTPUT_NODE_CNT;

static double* m_ptrInputNode = NULL; 
static double* m_ptrHiddenNode = NULL;
static double* m_ptrOutputNode = NULL; 
static double* m_ptrHiddenDelta = NULL;
static double* m_ptrOutputDelta = NULL;  
static double* m_ptrTarget = NULL;    
static double** m_ptrInputWeight = NULL;
static double** m_ptrHiddenWeight = NULL;
static double** m_ptrInput_PrevWeight  = NULL;
static double** m_ptrHidden_PrevWeight = NULL;

static double* m_ptrReco_InputNode = NULL;
static double* m_ptrReco_HiddenNode = NULL;
static double* m_ptrReco_OutputNode = NULL;
static double** m_ptrReco_InputWeight = NULL;
static double** m_ptrReco_HiddenWeight = NULL;


/*==============================================================================================================================
Neural Network 관련 변수 
================================================================================================================================*/
/*------------------------------------------------------------------------------------------------------------------------------*/
static int m_bStudy = 0;
static int m_bLearing =0;
static volatile int m_forcesave =0;


static int ReleaseBuffer( void );
static int CreateNNBuffer( void );
static int ReleaseNNBuffer( void );
static int Bulid_NeuralNetwork(  void );
static int Learning_NeuralNetwork( void );

/*=========================================================================
가중치 저장
==========================================================================*/
static int SaveWeight(double **w,int n1,int n2, char* name)
{
	int i,j;
	double *ptrBuff= NULL;
	FILE *ptrFp= NULL;
	char szpath[ MAX_PATH ];

	memset( szpath, 0, sizeof( szpath));
	GetWinCurPath( szpath, name );
	ptrFp=fopen(szpath,"wb+");

	if ( ptrFp == NULL )
	{
		return ERR_INVALID_DATA;
	}

	ptrBuff=(double*)new double[ (n1+1)*(n2+1) ];

	if ( ptrBuff == NULL )
	{
		fclose(ptrFp);
		return ERR_ALLOC_BUF;
	}

	for(i=0;i<=n1;i++)
	{
		for(j=0;j<=n2;j++)
			ptrBuff[i*(n2+1)+j]=w[i][j];
	}

	fwrite((char*)ptrBuff,sizeof(double),(n1+1)*(n2+1),ptrFp);
	fclose(ptrFp);
	delete[] ptrBuff;

	return ERR_SUCCESS;
}

/*=========================================================================
가중치 읽기
==========================================================================*/
static int ReadWeight(double **w,int n1,int n2, char*name)
{
	int i,j;
	double *ptrBuff= NULL;
	FILE *ptrFp= NULL;
	char szpath[ MAX_PATH];

	memset( szpath, 0, sizeof( szpath));
	GetWinCurPath( szpath, name );

	if((ptrFp=fopen(szpath,"rb"))==NULL)
	{
		return ERR_INVALID_DATA;		
	}
	if ( ptrFp == NULL )
	{
		return ERR_INVALID_DATA;
	}

	ptrBuff=(double*)new double[ (n1+1)*(n2+1) ];

	if ( ptrBuff == NULL )
	{
		fclose(ptrFp);
		return ERR_ALLOC_BUF;
	}
	
	fread((char*)ptrBuff,sizeof(double),(n1+1)*(n2+1),ptrFp);

	for(i=0;i<=n1;i++)
	{
		for(j=0;j<=n2;j++)
			w[i][j]=ptrBuff[i*(n2+1)+j];
	}
	fclose(ptrFp);
	delete [] ptrBuff;	

	return ERR_SUCCESS;
}

/*=========================================================================
가중치 복사 
==========================================================================*/
static int CopyWeight( double ** ptrDestWeight, double ** ptrOrgWeight, int n1, int n2 )
{
	int i, j;

	for(i=0;i<=n1;i++)
	{
		for(j=0;j<=n2;j++)
		{
			ptrDestWeight[i][j]= ptrOrgWeight[i][ j ];
		}
	}
	return ERR_SUCCESS;
}

/*=========================================================================
랜덤 변수 생성 
==========================================================================*/
void BPNN_Initialize(int seed)
{	
	srand(seed);
}

/*=========================================================================
Weight 랜덤 초기화
==========================================================================*/
void BPNN_Randomize_Weights(double **w, int m, int n)
{
	int i, j;

	for (i = 0; i <= m; i++) 
	{
		for (j = 0; j <= n; j++) 
		{
			w[i][j] = OCR_COM_RandVal();
		}
	}
}

/*=========================================================================
Weight 0 초기화
==========================================================================*/
void BPNN_Zero_Weights(double **w, int m, int n)
{
	int i, j;

	for (i = 0; i <= m; i++) 
	{
		for (j = 0; j <= n; j++) 
		{
			w[i][j] = 0.0;
		}
	}
}

/*=========================================================================
Weight 재 설정 
BPNN_Layerforward( m_ptrReco_InputNode, m_ptrReco_HiddenNode, m_ptrReco_InputWeight, m_nInputCnt, m_nHiddenCnt);
상위 노드의 가중치와 node값을 sigmoid 처리하여 하위 노드의 값으로 사용
==========================================================================*/
void BPNN_Layerforward(double *l1, double *l2, double **conn, int n1, int n2)
{
	double sum;
	int j, k;
	
	l1[0] = 1.0;
	
	for (j = 1; j <= n2; j++) 
	{
		sum = 0.0;
		for (k = 0; k <= n1; k++) 
		{
			sum += conn[k][j] * l1[k];
		}
		l2[j] = OCR_COM_SigmoidVal(sum);
	}
}

/*=========================================================================
출력 계층 오류
BPNN_Output_error(m_ptrOutputDelta,m_ptrTarget,m_ptrOutputNode,m_nOutputCnt);
m_ptrTarget : 예상 출력값
==========================================================================*/
void BPNN_Output_error(double *delta, double *m_ptrTarget, double *output, int nj)
{
	int j;
	double o, t;
	
	for (j = 1; j <= nj; j++) 
	{
		o = output[j];
		t = m_ptrTarget[j];

		/* 출력 오류 계산 */
		delta[j] = (double)(o * (1.0 - o) * (t - o));
	}	
}

/*=========================================================================
Hidden 계층 오류
BPNN_Hidden_error(m_ptrHiddenDelta,m_nHiddenCnt, m_ptrOutputDelta, 
                  m_nOutputCnt,m_ptrHiddenWeight, m_ptrHiddenNode);
==========================================================================*/
void BPNN_Hidden_error(double* delta_h, int nh, double *delta_o, int no, double **who, double *hidden)
{
	int j, k;
	double h, sum;
	
	for (j = 1; j <= nh; j++) 
	{
		h = hidden[j];
		sum = 0.0;
		for (k = 1; k <= no; k++) 
		{
			sum += delta_o[k] * who[j][k];
		}

		/* hidden 오류 계산 */
		delta_h[j] = (double)(h * (1.0 - h) * sum);
	}
}

/*  */

/*=========================================================================
가중치를 조정
BPNN_Adjust_weights(m_ptrOutputDelta,m_nOutputCnt, m_ptrHiddenNode,m_nHiddenCnt, 
                    m_ptrHiddenWeight, m_ptrHidden_PrevWeight, m_dEta, m_dMoment); 
==========================================================================*/
void BPNN_Adjust_weights(double *delta, int ndelta, double *ly, int nly, double** w, double **oldw, double eta, double momentum)
{
	double new_dw;
	int k, j;

	ly[0] = 1.0;
	for (j = 1; j <= ndelta; j++) 
	{
		for (k = 0; k <= nly; k++) 
		{
			/* 새로운 pre weight */
			new_dw = ((eta * delta[j] * ly[k]) + (momentum * oldw[k][j]));
			
			/* 새로운 weight */
			w[k][j] += new_dw;

			/* 전환 */
			oldw[k][j] = new_dw;
		}
	}
}

/*=========================================================================
BPNN( Back-Propagation Neural Network ) 생성 
초기 가중치 설정
==========================================================================*/
static int Bulid_NeuralNetwork(  void )
{
	time_t t; 
	BPNN_Initialize((unsigned)time(&t));

	//다양한 초기화의 가중치를 초기화
	BPNN_Randomize_Weights( m_ptrInputWeight,m_nInputCnt,m_nHiddenCnt);
	BPNN_Randomize_Weights( m_ptrHiddenWeight,m_nHiddenCnt,m_nOutputCnt);

	BPNN_Zero_Weights(m_ptrInput_PrevWeight, m_nInputCnt,m_nHiddenCnt );
	BPNN_Zero_Weights(m_ptrHidden_PrevWeight,m_nHiddenCnt,m_nOutputCnt );

	return ERR_SUCCESS;
}

/*=========================================================================
신경망 학습

==========================================================================*/
static int Learning_NeuralNetwork( pOCR_CONF ptrOCRConf )
{
	int i, j;
	pOCR_FEATURE ptrFeature= NULL;
	pOCR_FETURE_MANAGER ptrFeatureManager = NULL;

	float fErr=0.0;
	int nLoopCnt=0;
	
	m_bLearing =1;	
	
	ptrFeatureManager = OCR_GetFeatureManager();
	if ( ptrFeatureManager == NULL )
		return ERR_NOT_STUDY_MLP;

	/*===========================================================================
	[ 최대 학습 횟수 20000 ]
	BPNN( Back-propagation Neural network 
	하위단 오류값을 이용하여 상위단 오류를 계산 
	============================================================================*/
	while( m_bLearing && m_forcesave == 0 )
	{		
		fErr = 0;
		for( j = 0 ; j < ptrFeatureManager->nCnt ; j++ )
		{
			ptrFeature = ( pOCR_FEATURE)&ptrFeatureManager->ptrFeatureVector[ j ];
			if ( ptrFeature != NULL )
			{
				/* 입력층 등록 */
				for(i=1;i<=m_nInputCnt;i++)
				{
					m_ptrInputNode[i] = (double)(ptrFeature->Feature[ i -1 ] * 1.0);
				}

				/* 예상 출력 등록*/
				for(i=1;i<=m_nOutputCnt;i++)
				{
					m_ptrTarget[i]=(double)OCR_OUTPUT[ ptrFeature->OCR_CODE  ][i-1];
				}				
				
				/* Hidden Node 값 생성  */
				BPNN_Layerforward(m_ptrInputNode,m_ptrHiddenNode,m_ptrInputWeight, m_nInputCnt,m_nHiddenCnt);
				
				/* Output Node 값 생성  */
				BPNN_Layerforward(m_ptrHiddenNode, m_ptrOutputNode,m_ptrHiddenWeight,m_nHiddenCnt,m_nOutputCnt);
				
				/* 출력층 오류 계산 */
				BPNN_Output_error(m_ptrOutputDelta,m_ptrTarget,m_ptrOutputNode,m_nOutputCnt);

				/* 출력층 오류를 이용한 히든 층 오류 계산 */
				BPNN_Hidden_error(m_ptrHiddenDelta,m_nHiddenCnt, m_ptrOutputDelta, m_nOutputCnt,m_ptrHiddenWeight, m_ptrHiddenNode);
				
				/* 히든 층 가중치 조정 */
				BPNN_Adjust_weights(m_ptrOutputDelta,m_nOutputCnt, m_ptrHiddenNode,m_nHiddenCnt, m_ptrHiddenWeight, m_ptrHidden_PrevWeight, (double)NN_ETHA, (double)NN_MOMENT); 

				/* 입력층 가중치 조정 */
				BPNN_Adjust_weights(m_ptrHiddenDelta, m_nHiddenCnt, m_ptrInputNode, m_nInputCnt, m_ptrInputWeight, m_ptrInput_PrevWeight, (double)NN_ETHA, (double)NN_MOMENT);  

				for(i=1;i<=m_nOutputCnt;i++)
				{
					fErr += (float)(( m_ptrOutputNode[i] - OCR_OUTPUT[ ptrFeature->OCR_CODE  ][i-1] ) * ( m_ptrOutputNode[i] - OCR_OUTPUT[ ptrFeature->OCR_CODE  ][i-1] )) ;
				}

			}
		}
		
		/*===========================================================================
		[ 에러율 계산 ]	
		계산된 Output Node의 값과 Hidden , Input의 조정된 Weight 값을 이용하여 
		OCR OUTPUT Table의 오차의 제곱합이 m_dMinErrRate 이하가 되도록 반복 한다. 
		============================================================================*/
		
		//평균 제곱 오류를 계산
		//fErr = fErr / (float)( ptrFeatureManager->nCnt * m_nOutputCnt   );
		fErr = fErr / (float)( ptrFeatureManager->nCnt );
		nLoopCnt++;
		OCR_COM_SendNotifyStatus( nLoopCnt, fErr );

		if( fErr < NN_MIN_ERR || nLoopCnt > ptrOCRConf->OCR_LEARNING_CNT )
		{
			m_bLearing =0;
			break;
		}
	}

	/* 학습된 Weight값을 저장 ( 차후 Recognition에 사용 )*/
	/*===========================================================================
	[ 조종된 Weight 값 저장 ]		
	============================================================================*/

	SaveWeight(m_ptrInputWeight,m_nInputCnt,m_nHiddenCnt,TK_EBP_INPUT_WEIGHT_DATA);	
	SaveWeight(m_ptrHiddenWeight,m_nHiddenCnt,m_nOutputCnt,TK_EBP_HIDDEN_WEIGHT_DATA);
	
	m_forcesave = 0;
	m_bLearing = 0;

	return ERR_SUCCESS;
}

/*=========================================================================
객체 인식 신경망

특징점 : ptrFeature ( 13개의 특징 벡터 )
결과 : nMaxResult = 2^8
==========================================================================*/
static int Recognition_NeuralNetwork( pOCR_FEATURE ptrFeature )
{
	int i;
	int nResult=0 ;
	int nMaxResult=0;

	nMaxResult =  (int)pow(2,double( m_nOutputCnt ));
	if ( m_bStudy == 0 )
		return ERR_NOT_STUDY_MLP;
	
	/*===========================================================================
	[ 각층의 노드 초기화 : 학습된 신경망 복사( 인식 과정에서 신경망 정보가 갱신되기때문 ]	
	============================================================================*/
	memset( m_ptrReco_InputNode, 0, sizeof( double)* ( m_nInputCnt + 1 ));
	memset( m_ptrReco_HiddenNode, 0, sizeof( double)* ( m_nHiddenCnt + 1 ));
	memset( m_ptrReco_OutputNode, 0, sizeof( double)* ( m_nOutputCnt + 1 ));

	/*===========================================================================
	[ 각층의 가중치 초기화	]

	Input 항목 가중치 개수 : input 항목 개수 
	input 항목 가중치 크기 : Hidden 항목의 개수( 모든 Hidden 항목 접근 가능 )

	Hidden 항목 가중치 개수 : Hidden 항목 개수 
	Hidden 항목 가중치 크기 : Output 항목의 개수( 모든 Output 항목 접근 가능 )

	============================================================================*/
	for(i = 0 ; i < ( m_nInputCnt + 1 ) ; i++)
	{		
		memset( m_ptrReco_InputWeight[ i ], 0, sizeof( double ) * ( m_nHiddenCnt + 1 ) );
	}
	for(i = 0 ; i < ( m_nHiddenCnt + 1 ) ; i++)
	{		
		memset( m_ptrReco_HiddenWeight[ i ], 0, sizeof( double ) * ( m_nOutputCnt + 1 ) );
	}

	/*===========================================================================
	[ 가중치 복사 ]
	학습되어 있는 Input Weight 와 Hidden Weight를 인식용 Weight에 복사한다.
	============================================================================*/
	CopyWeight(m_ptrReco_InputWeight, m_ptrInputWeight, m_nInputCnt,m_nHiddenCnt);
	CopyWeight(m_ptrReco_HiddenWeight, m_ptrHiddenWeight, m_nHiddenCnt,m_nOutputCnt);
	
	/*===========================================================================
	[ 추출된 특징점을 입력층으로 복사  ]	
	============================================================================*/
	for(i=1;i<=m_nInputCnt;i++)
	{
		m_ptrReco_InputNode[i]=(double)(ptrFeature->Feature[ i-1 ] *1.0);
	}

	/*===========================================================================
	[ 상위 노드와 학습된 가중치를 통해 하위 노드의 값을 획득한다. ]	
	  <<Sigmoid 값>>
	============================================================================*/
#if 1
	BPNN_Layerforward( m_ptrReco_InputNode, m_ptrReco_HiddenNode, m_ptrReco_InputWeight, m_nInputCnt, m_nHiddenCnt);
	BPNN_Layerforward( m_ptrReco_HiddenNode, m_ptrReco_OutputNode, m_ptrReco_HiddenWeight, m_nHiddenCnt, m_nOutputCnt);
#else
	BPNN_Layerforward( m_ptrReco_InputNode, m_ptrReco_HiddenNode, m_ptrInputWeight, m_nInputCnt, m_nHiddenCnt);
	BPNN_Layerforward( m_ptrReco_HiddenNode, m_ptrReco_OutputNode, m_ptrHiddenWeight, m_nHiddenCnt, m_nOutputCnt);
#endif

	/*===========================================================================
	[ 모든 출력을 검사  ]	
	Output Node 값이 m_dMinValidRate(0.5) 이상일 경우 유효하다고 판단하고
	유효한 output node 위치에 대한 2의 자승을 구함으로써 강한 특징을 가지고 있는 output
	위치에 높은값이 산출됨으로 해당 위치에서 유사성이 높다고 판단할 수 있게 된다.   
	============================================================================*/
	for(i=1;i<=m_nOutputCnt;i++)
	{		
		if(m_ptrReco_OutputNode[ i ] > NN_MIN_VALIDRATE )
		{
			nResult+=(int)pow(2,double( m_nOutputCnt - i ));
			//nResult+=(int)pow(2,double( 4 - i ));
		}
	}
	
	if(nResult<= nMaxResult)
	//if(nResult<= 9 )
	{
		return nResult;
	}

	return ERR_NOT_MATCH_MLP;
}

/*=========================================================================
학습된 가중치 정보 읽기
학습된 가중치 : Input Weight, Hidden Weight 
==========================================================================*/
static int ReadStudiedWeight( void )
{
	
	int i;
	int nErrCode = ERR_SUCCESS;
	
	/* 학습된 신경망 복사( 인식 과정에서 신경망 정보가 갱신되기때문 */	
	memset( m_ptrInputNode, 0, sizeof( double)* ( m_nInputCnt + 1 ));
	memset( m_ptrHiddenNode, 0, sizeof( double)* ( m_nHiddenCnt + 1 ));
	memset( m_ptrOutputNode, 0, sizeof( double)* ( m_nOutputCnt + 1 ));


	for(i = 0 ; i < ( m_nInputCnt + 1 ) ; i++)
	{		
		memset( m_ptrInputWeight[ i ], 0, sizeof( double ) * ( m_nHiddenCnt + 1 ) );
	}
	for(i = 0 ; i < ( m_nHiddenCnt + 1 ) ; i++)
	{		
		memset( m_ptrHiddenWeight[ i ], 0, sizeof( double ) * ( m_nOutputCnt + 1 ) );
	}

	nErrCode = ReadWeight(m_ptrInputWeight,m_nInputCnt,m_nHiddenCnt,TK_EBP_INPUT_WEIGHT_DATA);
	if ( nErrCode != ERR_SUCCESS)
		return nErrCode;
		
	nErrCode = ReadWeight(m_ptrHiddenWeight,m_nHiddenCnt,m_nOutputCnt,TK_EBP_HIDDEN_WEIGHT_DATA);
	if ( nErrCode != ERR_SUCCESS)
		return nErrCode;	

	m_bStudy = 1;
	return ERR_SUCCESS;
}

/*=========================================================================
BPNN( Back-Propagation Neural Network ) 초기값 설정
신경망 학습 
==========================================================================*/
static int RunStudyAndSaveWeight( pOCR_CONF ptrOCRConf  )
{	
	int nErrCode = ERR_SUCCESS;
	
	/*===========================================================================
	[ 신경망 초기값 설정 ]	
	============================================================================*/
	nErrCode = Bulid_NeuralNetwork( );
	if ( nErrCode != ERR_SUCCESS )
		return nErrCode;

	/*===========================================================================
	[ 신경망 학습 ]
	BPNN( BACK-PROPAGATION NEURAL NETWORK )
	하위 노드의 Error율을 이용하여 상위의 Error율을 계산한다. 
	============================================================================*/
	nErrCode = Learning_NeuralNetwork( ptrOCRConf );	
	
	return ERR_SUCCESS;
}

/*=========================================================================
특징벡터 벡터 메모리 해제 
Neural Network 노드, 가중치 메모리 해제
==========================================================================*/
static int ReleaseBuffer( void )
{
	OCR_COM_RleaseFeatureBuf();
	ReleaseNNBuffer();

	return ERR_SUCCESS;
}

/*=========================================================================
Neural Network 노드, 가중치 메모리 생성 
학습용 Input, Hidden, Output Node 메모리
학습용 Input, Hidden, Output Delta 메모리
학습용 Input, Hidden, Output Weight 메모리

인식용 Input, Hidden, Output Node 메모리
인식용 Input, Hidden, Output Delta 메모리
인식용 Input, Hidden, Output Weight 메모리

==========================================================================*/
static int CreateNNBuffer( void )
{
	if ( m_ptrInputNode == NULL )
	{
		m_ptrInputNode= (double *) new double[ m_nInputCnt + 1];
		if ( m_ptrInputNode == NULL )
		{
			ReleaseNNBuffer();
			return ERR_ALLOC_BUF;
		}
	}
	
	if ( m_ptrHiddenNode == NULL )
	{
		m_ptrHiddenNode=(double *) new double[ m_nHiddenCnt + 1];
		if ( m_ptrHiddenNode == NULL )
		{
			ReleaseNNBuffer();
			return ERR_ALLOC_BUF;
		}
	}
	
	if ( m_ptrOutputNode == NULL )
	{
		m_ptrOutputNode=(double *) new double[ m_nOutputCnt + 1];
		if ( m_ptrOutputNode == NULL )
		{
			ReleaseNNBuffer();
			return ERR_ALLOC_BUF;
		}
	}
	
	if ( m_ptrHiddenDelta == NULL )
	{
		m_ptrHiddenDelta = (double *) new double[ m_nHiddenCnt + 1];
		if ( m_ptrHiddenDelta == NULL )
		{
			ReleaseNNBuffer();
			return ERR_ALLOC_BUF;
		}
	}
	
	if ( m_ptrOutputDelta == NULL )
	{
		m_ptrOutputDelta = (double *) new double[ m_nOutputCnt + 1];
		if ( m_ptrOutputDelta == NULL )
		{
			ReleaseNNBuffer();
			return ERR_ALLOC_BUF;
		}
	}
	
	if ( m_ptrTarget == NULL )
	{
		m_ptrTarget = (double *) new double[ m_nOutputCnt + 1];
		if ( m_ptrTarget == NULL )
		{
			ReleaseNNBuffer();
			return ERR_ALLOC_BUF;
		}
	}
	
	if ( m_ptrInputWeight == NULL )
	{
		m_ptrInputWeight=HLP_dAllocMatrix(m_nInputCnt + 1, m_nHiddenCnt + 1);
		if ( m_ptrInputWeight == NULL )
		{
			ReleaseNNBuffer();
			return ERR_ALLOC_BUF;
		}
	}

	if ( m_ptrHiddenWeight == NULL )
	{
		m_ptrHiddenWeight = HLP_dAllocMatrix( m_nHiddenCnt + 1, m_nOutputCnt + 1);
		if ( m_ptrHiddenWeight == NULL )
		{
			ReleaseNNBuffer();
			return ERR_ALLOC_BUF;
		}
	}

	if ( m_ptrInput_PrevWeight == NULL )
	{
		m_ptrInput_PrevWeight = HLP_dAllocMatrix(m_nInputCnt + 1, m_nHiddenCnt + 1);		
		if ( m_ptrInput_PrevWeight == NULL )
		{
			ReleaseNNBuffer();
			return ERR_ALLOC_BUF;
		}
	}

	if ( m_ptrHidden_PrevWeight == NULL )
	{
		m_ptrHidden_PrevWeight = HLP_dAllocMatrix(m_nHiddenCnt + 1, m_nOutputCnt + 1);
		if ( m_ptrHidden_PrevWeight == NULL )
		{
			ReleaseNNBuffer();
			return ERR_ALLOC_BUF;
		}
	}
	
	
	if ( m_ptrReco_InputNode == NULL )
	{
		m_ptrReco_InputNode= (double *) new double[ m_nInputCnt + 1];
		if ( m_ptrReco_InputNode == NULL )
		{
			ReleaseNNBuffer();
			return ERR_ALLOC_BUF;
		}
	}
	
	if ( m_ptrReco_HiddenNode == NULL )
	{
		m_ptrReco_HiddenNode=(double *) new double[ m_nHiddenCnt + 1];
		if ( m_ptrReco_HiddenNode == NULL )
		{
			ReleaseNNBuffer();
			return ERR_ALLOC_BUF;
		}
	}
	
	if ( m_ptrReco_OutputNode == NULL )
	{
		m_ptrReco_OutputNode=(double *) new double[ m_nHiddenCnt + 1];
		if ( m_ptrReco_OutputNode == NULL )
		{
			ReleaseNNBuffer();
			return ERR_ALLOC_BUF;
		}
	}

	if ( m_ptrReco_OutputNode == NULL )
	{
		m_ptrReco_OutputNode=(double *) new double[ m_nOutputCnt + 1];
		if ( m_ptrReco_OutputNode == NULL )
		{
			ReleaseNNBuffer();
			return ERR_ALLOC_BUF;
		}
	}

	if ( m_ptrReco_InputWeight == NULL )
	{
		m_ptrReco_InputWeight=HLP_dAllocMatrix(m_nInputCnt + 1, m_nHiddenCnt + 1);
		if ( m_ptrReco_InputWeight == NULL )
		{
			ReleaseNNBuffer();
			return ERR_ALLOC_BUF;
		}
	}
	
	if ( m_ptrReco_HiddenWeight == NULL )
	{
		m_ptrReco_HiddenWeight = HLP_dAllocMatrix(m_nHiddenCnt + 1, m_nOutputCnt + 1);
		if ( m_ptrReco_HiddenWeight == NULL )
		{
			ReleaseNNBuffer();
			return ERR_ALLOC_BUF;
		}
	}	

	return ERR_SUCCESS;
}

/*=========================================================================
Neural Network 노드, 가중치 메모리 해제
==========================================================================*/
static int ReleaseNNBuffer( void )
{
	if (m_ptrInputNode != NULL )
	{
		delete [] m_ptrInputNode;
		m_ptrInputNode = NULL;
	}

	if ( m_ptrHiddenNode != NULL )
	{
		delete []  m_ptrHiddenNode;
		m_ptrHiddenNode = NULL;
	}
	
	if ( m_ptrOutputNode != NULL )
	{
		delete [] m_ptrOutputNode ;
		m_ptrOutputNode = NULL;
	}
	
	if ( m_ptrHiddenDelta != NULL )
	{
		delete [] m_ptrHiddenDelta;
		m_ptrHiddenDelta = NULL;

	}
	
	if ( m_ptrOutputDelta != NULL )
	{
		delete [] m_ptrOutputDelta;
		m_ptrOutputDelta = NULL;

	}

	if ( m_ptrTarget != NULL )
	{
		delete [] m_ptrTarget;
		m_ptrTarget = NULL;

	}
	
	if ( m_ptrInputWeight != NULL )
	{
		HLP_dFreeMatrix( m_ptrInputWeight, m_nInputCnt + 1, m_nHiddenCnt + 1 );
		m_ptrInputWeight = NULL;
	}

	if ( m_ptrHiddenWeight != NULL )
	{		
		HLP_dFreeMatrix( m_ptrHiddenWeight, m_nHiddenCnt + 1, m_nOutputCnt + 1 );
		m_ptrHiddenWeight = NULL;		
	}

	if ( m_ptrInput_PrevWeight != NULL )
	{
		HLP_dFreeMatrix( m_ptrInput_PrevWeight, m_nInputCnt + 1, m_nHiddenCnt + 1 );
		m_ptrInput_PrevWeight = NULL;
	}

	if ( m_ptrHidden_PrevWeight != NULL )
	{
		HLP_dFreeMatrix( m_ptrHidden_PrevWeight, m_nHiddenCnt + 1, m_nOutputCnt + 1 );
		m_ptrHidden_PrevWeight = NULL;		
	}

	if ( m_ptrReco_InputNode != NULL )
	{
		delete [] m_ptrReco_InputNode ;
		m_ptrReco_InputNode = NULL;
	}

	if ( m_ptrReco_HiddenNode != NULL )
	{
		delete [] m_ptrReco_HiddenNode ;
		m_ptrReco_HiddenNode = NULL;
	}

	if ( m_ptrReco_OutputNode != NULL )
	{
		delete [] m_ptrReco_OutputNode ;
		m_ptrReco_OutputNode = NULL;
	}

	if ( m_ptrReco_OutputNode != NULL )
	{
		delete [] m_ptrReco_OutputNode;
		m_ptrReco_OutputNode = NULL;
	}

	if ( m_ptrReco_InputWeight != NULL )
	{
		HLP_dFreeMatrix( m_ptrReco_InputWeight, m_nInputCnt + 1, m_nHiddenCnt + 1 );
		m_ptrReco_InputWeight = NULL;
	}

	if ( m_ptrReco_HiddenWeight == NULL )
	{
		HLP_dFreeMatrix( m_ptrReco_HiddenWeight, m_nHiddenCnt + 1, m_nOutputCnt + 1 );
		m_ptrReco_HiddenWeight = NULL;
	}	

	return ERR_SUCCESS;	
}
/*=========================================================================
MLP(Multi layer perceptron ) 초기화

1. 공용 메모리 생성 
2. 학습된 가중치 읽음 
==========================================================================*/
int EBP_Initialize( int bCreate  )
{
	int nErrCode = ERR_SUCCESS;
	
	/*===========================================================================
	[ 학습데이터( 특징 벡터)를 담을 버퍼 생성 ]	
	============================================================================*/
	if ( bCreate )
	{
		nErrCode = CreateNNBuffer(  );
		if ( nErrCode != ERR_SUCCESS )
			return nErrCode ;
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
int EBP_Save_NeuralNetwork( pOCR_FEATURE ptrFeature )
{
	if ( ptrFeature ==NULL )
		return ERR_INVALID_DATA;
	
	return OCR_COM_SaveFeature( ptrFeature );
}

/*=========================================================================
객체 인식
LPR 최종단계에서 넘어온 문자 이미지의 특징점 벡터
==========================================================================*/
int EBP_Recog_NeuralNetwork( pOCR_FEATURE ptrFeature )
{	
	return Recognition_NeuralNetwork( ptrFeature );
}

/*=========================================================================
특징 벡터에 의한 신경망 재 학습

파일에 저장된 특징 벡터의 정보를 BPNN( Back-Propagation Neural Network )을 이용해
학습 시킴 
==========================================================================*/
int EBP_ReStudy( void(* fnNotifyCallback)(int , double ),  pOCR_CONF ptrOCRConf )
{
	int nErrCode = ERR_SUCCESS;

	m_bStudy=0;
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
int EBP_NN_ForceSave( void )
{
	if (m_bLearing )
	{
		m_forcesave = 1;
	}
	else
		return ERR_INVALID_DATA;
	
	return ERR_SUCCESS;
}

int EBP_Release( void )
{
	ReleaseBuffer();
	return ERR_SUCCESS;
}