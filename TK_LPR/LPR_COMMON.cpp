#include "stdafx.h"
#include <malloc.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "../include/TK_TYPE.h"
#include "../include/TK_LIB_Helper.h"
#include "LPR_COMMON.h"


static int ** m_ptrEq_tbl= NULL;

static UNIFORMITY_t ** m_ptrUniformitys = NULL;
static COMMONINFO_t m_ConfInfo;

static int * m_ptrnHash = NULL;	
static GRASSFIRE_HEAP_t m_GrassfireHeap;
static int m_Area = CENTER_AREA;

/*========================================
 LPR_CHAR, LPR_PLATE 공용 버퍼
 =========================================*/
static int ** m_ptrMap = NULL;
static int ** m_ptrMap2 = NULL;

static IMAGE_DATA_t m_ChgImageData;
static IMAGE_DATA_t m_TmpImageData;
static IMAGE_DATA_t m_TmpImageData2;
static IMAGE_DATA_t m_CharImageData;

static LPR_CHAR_t m_LPRChars;

static CVRECT_t * m_ptrPlateRect = NULL;
static CVRECT_t * m_ptrTempRect = NULL;

static CVRECT_t * m_ptrCharRect = NULL;

static int * m_ptrHProject = NULL;
static int * m_ptrWProject = NULL;
static int * m_ptrWProject2 = NULL;

static RECT_IMAGE_MANAGER_t m_RectImageMng;


static int Create_RectImageBuf(int nBitCnt, int nWidth, int nHeight );
static int Release_RectImageBuf( void );

/*======================================================================
문자 분할용 Rect Image buffer 생성 및 해제
========================================================================*/
static int Create_RectImageBuf( int nBitCnt, int nWidth, int nHeight )
{
	pRECTIMAGE ptrRectImg = NULL;
	int i, nLineByte;

	for( i = 0 ; i < MAX_PLATE_RECT_IMAGE_CNT ; i++)
	{
		ptrRectImg = ( pRECTIMAGE )&m_RectImageMng.RectImage[ i ] ;

		if ( ptrRectImg != NULL )
		{
			memset( ptrRectImg, 0, sizeof( RECT_IMAGE_t ));												   
		    nLineByte = HLP_GetCalBmpWidth( nWidth, nBitCnt);

			ptrRectImg->ImageData.ptrSrcImage2D =  HLP_AllocMatrix(nHeight, nLineByte );
			
			if ( ptrRectImg->ImageData.ptrSrcImage2D  != NULL )
			{	
				ptrRectImg->nOrgH		= nHeight;
				ptrRectImg->nOrgW		= nWidth;
				ptrRectImg->nOrgBitCnt	= nBitCnt;
			}
		}
	}

	return ERR_SUCCESS;
}

static int Release_RectImageBuf( void )
{
	pRECTIMAGE ptrRectImg = NULL;
	int i, nLineByte;
	
	for( i = 0 ; i < MAX_PLATE_RECT_IMAGE_CNT ; i++)
	{
		ptrRectImg = ( pRECTIMAGE )&m_RectImageMng.RectImage[ i ] ;
		if ( ptrRectImg != NULL && ptrRectImg->ImageData.ptrSrcImage2D != NULL )
		{
			nLineByte = HLP_GetCalBmpWidth( ptrRectImg->nOrgW, ptrRectImg->nOrgBitCnt);
			HLP_FreeMatrix(ptrRectImg->ImageData.ptrSrcImage2D, ptrRectImg->nOrgH, nLineByte );
		}
	}
	
	return ERR_SUCCESS;
}

/*======================================================================
불균등 영역 메모리 해제 
========================================================================*/
static int FreeUniformity(UNIFORMITY_t ** ptrUniformitys, int nUniforH, int nUniforW )
{
	int i;

	(void)nUniforW;

	if ( ptrUniformitys == NULL )
		return ERR_SUCCESS;

	for(i = 0 ; i < nUniforH ; i++)
	{
		if ( ptrUniformitys[ i ]!= NULL )
		{
			delete [] ptrUniformitys[i];
			ptrUniformitys[i] = NULL;
		}
	}

	delete [] ptrUniformitys;
	ptrUniformitys = NULL;

	return ERR_SUCCESS;
}

/*======================================================================
불균등 영역 생성
2차(가로_세로) Matrix 형 메모리 생성
========================================================================*/
static UNIFORMITY_t ** CreateUniformity( int nUniforH, int nUniforW )
{	
	UNIFORMITY_t ** ptrUniformitys = NULL;
	int i;

	ptrUniformitys = new UNIFORMITY_t *[nUniforH];

	if ( ptrUniformitys == NULL )
		return NULL;

	for(i = 0 ; i < nUniforH ; i++)
	{
		ptrUniformitys[ i ] = new UNIFORMITY_t[ nUniforW ];
		if ( ptrUniformitys[ i ] == NULL )
		{
			FreeUniformity( ptrUniformitys, nUniforH, nUniforW);
			return NULL;
		}
		memset( ptrUniformitys[ i ] , 0, sizeof( UNIFORMITY_t) * nUniforW);
	}

	return ptrUniformitys;
}

/*======================================================================
공용 메모리 초기화
pIMAGE_DATA ptrImageDataSrc : 원본 이미지 정보
========================================================================*/
int COM_Initialize( pLPR_CONF ptrLPRConf, pOCR_CONF ptrOCRConf, pIMAGE_DATA ptrImageDataSrc )
{

	static int bFirst = 0;

	if ( bFirst == 0 )
	{
		memset(&m_ConfInfo, 0, sizeof( COMMONINFO_t ));
		memset( &m_GrassfireHeap, 0, sizeof(GRASSFIRE_HEAP_t ));
		bFirst = 1;
	}
	
	m_ConfInfo.nImageHeight = ptrImageDataSrc->nH;
	m_ConfInfo.nImageWidth = ptrImageDataSrc->nW;
	
	if( m_ConfInfo.nImageHeight > m_ConfInfo.nMaxHeight )
		m_ConfInfo.nMaxHeight = m_ConfInfo.nImageHeight;

	if( m_ConfInfo.nImageWidth > m_ConfInfo.nMaxWidth )
		m_ConfInfo.nMaxWidth = m_ConfInfo.nImageWidth;

	/*===========================================================================
	[ 불균등 영역의 가로 세로 개수 설정  ]	
	============================================================================*/
	if ( ptrLPRConf == NULL || ptrOCRConf == NULL )
	{
		return ERR_INITIALIZE;
	}
	
	m_ConfInfo.nUniformityH = (( ptrImageDataSrc->nH / ptrLPRConf->LPR_UF_H) +1 );
	m_ConfInfo.nUniformityW = (( ptrImageDataSrc->nW / ptrLPRConf->LPR_UF_W) +1 );

	/*===========================================================================
	[ 번호판 영역 후보지 메모리 생성  ]	
	============================================================================*/
	if ( m_ptrPlateRect == NULL )
	{
		m_ptrPlateRect = new CVRECT_t [ MAX_LABEL_CNT ];
		if ( m_ptrPlateRect == NULL )
		{
			COM_Release( ptrLPRConf, ptrOCRConf );
			return ERR_ALLOC_BUF;
		}
	}	

	/*===========================================================================
	[ 우선순위 번호판 영역 후보지 메모리 생성  ]	
	============================================================================*/
	if ( m_ptrTempRect == NULL )
	{
		m_ptrTempRect = new CVRECT_t [ MAX_LABEL_CNT ];
		if ( m_ptrTempRect == NULL )
		{
			COM_Release(ptrLPRConf, ptrOCRConf);
			return ERR_ALLOC_BUF;
		}
	}	

	/*===========================================================================
	[ 차량 번호 문자 후보지 메모리 생성  ]	
	============================================================================*/
	if ( m_ptrCharRect == NULL )
	{
		m_ptrCharRect = new CVRECT_t [ MAX_CHAR_LABEL_CNT ];
		if ( m_ptrCharRect == NULL )
		{
			COM_Release(ptrLPRConf, ptrOCRConf);
			return ERR_ALLOC_BUF;
		}
	}	

	/*===========================================================================
	[ Labeling을 위한 Eq Table 메모리 생성  ]	
	============================================================================*/
	if ( m_ptrEq_tbl == NULL )
	{
		m_ptrEq_tbl = HLP_IntAllocMatrix( MAX_LABEL_CNT , 2 );
		if ( m_ptrEq_tbl == NULL )
		{
			COM_Release(ptrLPRConf, ptrOCRConf);
			return ERR_ALLOC_BUF;
		}
	}

	/*===========================================================================
	[ 불균등 영역 처리를 위한 메모리 생성  ]	
	============================================================================*/
	if ( m_ptrUniformitys == NULL )
	{
		m_ptrUniformitys = CreateUniformity( m_ConfInfo.nUniformityH, m_ConfInfo.nUniformityW );
		if ( m_ptrUniformitys == NULL )
		{
			COM_Release(ptrLPRConf, ptrOCRConf);
			return ERR_ALLOC_BUF;
		}
	}

	/*===========================================================================
	[ Labeling을 위한 Map 메모리 생성  
	  COM_DoLabeling8Neighbour 에서 사용 ]	
	============================================================================*/
	if ( m_ptrMap == NULL)
	{
		m_ptrMap = HLP_IntAllocMatrix( m_ConfInfo.nUniformityH, m_ConfInfo.nUniformityW );
		if ( m_ptrMap == NULL )
		{
			COM_Release(ptrLPRConf, ptrOCRConf);
			return ERR_ALLOC_BUF;
		}
	}

	/*===========================================================================
	[ Labeling을 위한 Map2 메모리 생성  
	  COM_SubDoLabeling8Neighbour 에서 사용 ]	
	============================================================================*/
	if ( m_ptrMap2 == NULL)
	{
		m_ptrMap2 = HLP_IntAllocMatrix( m_ConfInfo.nMaxHeight, m_ConfInfo.nMaxWidth );
		if ( m_ptrMap2 == NULL )
		{
			COM_Release(ptrLPRConf, ptrOCRConf);
			return ERR_ALLOC_BUF;
		}
	}

	/*===========================================================================
	[ Labeling을 위한 Hash Table 메모리 생성  ]	
	============================================================================*/
	if ( m_ptrnHash == NULL )
	{
		m_ptrnHash = new int [ m_ConfInfo.nMaxHeight ];
		if ( m_ptrnHash == NULL )
		{
			COM_Release(ptrLPRConf, ptrOCRConf);
			return ERR_ALLOC_BUF;
		}
	}

	/*===========================================================================
	[ 가로 투영 용 메모리 생성  ]	
	============================================================================*/
	if ( m_ptrWProject == NULL )
	{
		m_ptrWProject = new int [ m_ConfInfo.nMaxWidth ];
		if ( m_ptrWProject == NULL )
		{
			COM_Release(ptrLPRConf, ptrOCRConf);
			return ERR_ALLOC_BUF;
		}
	}

	/*===========================================================================
	[ 임시 가로 투영 용 메모리 생성  ]	
	============================================================================*/
	if ( m_ptrWProject2 == NULL )
	{
		m_ptrWProject2 = new int [ m_ConfInfo.nMaxWidth ];
		if ( m_ptrWProject2 == NULL )
		{
			COM_Release(ptrLPRConf, ptrOCRConf);
			return ERR_ALLOC_BUF;
		}
	}

	/*===========================================================================
	[ 세로 투영 용 메모리 생성  ]	
	============================================================================*/
	if ( m_ptrHProject == NULL )
	{
		m_ptrHProject = new int [ m_ConfInfo.nMaxHeight ];
		if ( m_ptrHProject == NULL )
		{
			COM_Release(ptrLPRConf, ptrOCRConf);
			return ERR_ALLOC_BUF;
		}
	}
	
	/*===========================================================================
	[ Labeling을 위한 Grassfire 메모리 생성  ]	
	============================================================================*/
	if ( m_GrassfireHeap.Heaps == NULL )
	{
		m_GrassfireHeap.Heaps = new GRASSFIRE_t[ m_ConfInfo.nMaxWidth * m_ConfInfo.nMaxHeight ];
		if ( m_GrassfireHeap.Heaps == NULL )
		{
			COM_Release(ptrLPRConf, ptrOCRConf);
			return ERR_ALLOC_BUF;
		}
	}

	if ( Create_RectImageBuf( ptrImageDataSrc->nBitCount, m_ConfInfo.nMaxWidth ,  m_ConfInfo.nMaxHeight ) != ERR_SUCCESS )
	{
		COM_Release(ptrLPRConf, ptrOCRConf);
		return ERR_ALLOC_BUF;
	}

	HLP_CopyImageDataWithEmpty( &m_ChgImageData,  8,m_ConfInfo.nMaxHeight, m_ConfInfo.nMaxWidth );
	HLP_CopyImageDataWithEmpty( &m_TmpImageData,  8,m_ConfInfo.nMaxHeight, m_ConfInfo.nMaxWidth );	
	HLP_CopyImageDataWithEmpty( &m_TmpImageData2, 8,m_ConfInfo.nMaxHeight, m_ConfInfo.nMaxWidth );
	HLP_CopyImageDataWithEmpty( &m_CharImageData, 8, ptrOCRConf->OCR_CHAR_H, ptrOCRConf->OCR_CHAR_W );
	
	return ERR_SUCCESS;
}

/*======================================================================
공용 메모리 해제
========================================================================*/
int COM_Release( pLPR_CONF ptrLPRConf,  pOCR_CONF ptrOCRConf )
{

	( void )ptrLPRConf;
	
	if ( m_ptrPlateRect != NULL )
	{
		delete [] m_ptrPlateRect;
		m_ptrPlateRect = NULL;
	}

	if ( m_ptrTempRect != NULL )
	{
		delete [] m_ptrTempRect;
		m_ptrTempRect = NULL;
	}

	if ( m_ptrCharRect != NULL )
	{
		delete [] m_ptrCharRect;
		m_ptrCharRect = NULL;
	}

	if ( m_ptrEq_tbl != NULL )
	{
		HLP_IntFreeMatrix( m_ptrEq_tbl,MAX_LABEL_CNT , 2 );
		m_ptrEq_tbl = NULL;
	}

	if ( m_ptrUniformitys != NULL )
	{
		FreeUniformity( m_ptrUniformitys, m_ConfInfo.nUniformityH, m_ConfInfo.nUniformityW );
		m_ptrUniformitys = NULL;
	}

	if ( m_ptrMap != NULL )
	{
		HLP_IntFreeMatrix( m_ptrMap , m_ConfInfo.nUniformityH, m_ConfInfo.nUniformityW );
		m_ptrMap = NULL;
	}

	if ( m_ptrMap2 != NULL )
	{
		HLP_IntFreeMatrix( m_ptrMap2 , m_ConfInfo.nMaxHeight, m_ConfInfo.nMaxWidth );
		m_ptrMap2 = NULL;
	}

	if ( m_ptrnHash != NULL )
	{
		delete [] m_ptrnHash;
		m_ptrnHash = NULL;
	}

	if ( m_ptrWProject != NULL )
	{
		delete[] m_ptrWProject;
		m_ptrWProject = NULL;
	}

	if ( m_ptrWProject2 != NULL )
	{
		delete[] m_ptrWProject2;
		m_ptrWProject2 = NULL;
	}


	if ( m_ptrHProject != NULL )
	{
		delete[] m_ptrHProject;
		m_ptrHProject = NULL;
	}

	if ( m_GrassfireHeap.Heaps != NULL )
	{
		delete [] m_GrassfireHeap.Heaps;
		m_GrassfireHeap.Heaps = NULL;
	}

	

	HLP_FreeMatrix( m_ChgImageData.ptrSrcImage2D, m_ConfInfo.nMaxHeight,m_ConfInfo.nMaxWidth );
	HLP_FreeMatrix( m_TmpImageData.ptrSrcImage2D, m_ConfInfo.nMaxHeight,m_ConfInfo.nMaxWidth );
	HLP_FreeMatrix( m_TmpImageData2.ptrSrcImage2D, m_ConfInfo.nMaxHeight,m_ConfInfo.nMaxWidth );
	
	if( ptrOCRConf != NULL )
	{
		HLP_FreeMatrix( m_CharImageData.ptrSrcImage2D, ptrOCRConf->OCR_CHAR_H, ptrOCRConf->OCR_CHAR_W );
	}

	Release_RectImageBuf( );
	
	m_ChgImageData.ptrSrcImage2D = NULL;
	m_TmpImageData.ptrSrcImage2D = NULL;
	m_TmpImageData2.ptrSrcImage2D = NULL;
	m_CharImageData.ptrSrcImage2D = NULL;

	return ERR_SUCCESS;
}

/*======================================================================
Grassfire heap 메모리 초기화
이미지 크기만큼 생성 
========================================================================*/
static int InitGrassfireHeap( void )
{
	pCOMMONINFO ptrComInfo= NULL;

	ptrComInfo = COM_GetInfo();
	if ( ptrComInfo == NULL )
	{
		return ERR_INVALID_DATA;
	}

	if ( m_GrassfireHeap.Heaps != NULL )
	{
		memset( m_GrassfireHeap.Heaps, 0, sizeof( GRASSFIRE_t )* ( ptrComInfo->nMaxHeight * ptrComInfo->nMaxWidth ));
		m_GrassfireHeap.nStackPos = 0;
	}

	return ERR_SUCCESS;
}

/*======================================================================
Grassfire heap 등록
Grassfire 객체에 이미지의 현재 위치와 label 정보 저장
========================================================================*/
static int PushGrassfireHeap( int nY, int nX, int nLabel )
{
	pGRASSFIRE ptrTempGrass = NULL;
	pCOMMONINFO ptrComInfo= NULL;

	ptrComInfo = COM_GetInfo();
	if ( ptrComInfo == NULL )
	{
		return ERR_INVALID_DATA;
	}

	if ( m_GrassfireHeap.nStackPos >= ptrComInfo->nMaxHeight * ptrComInfo->nMaxWidth )
		return ERR_INVALID_DATA;

	ptrTempGrass = (pGRASSFIRE)&m_GrassfireHeap.Heaps[ m_GrassfireHeap.nStackPos ];
	if ( ptrTempGrass != NULL )
	{		
		ptrTempGrass->nH = nY;
		ptrTempGrass->nW = nX;
		ptrTempGrass->nLabel = nLabel;
		m_GrassfireHeap.nStackPos++;
		return ERR_SUCCESS;
	}

	return ERR_INVALID_DATA;
}

/*======================================================================
Grassfire heap 추출
Grassfire의 현재 위치 정보를 하나 끄집어 내고 현재 위치를 이전으로 이동
========================================================================*/
static pGRASSFIRE PopGrassfireHeap( void)
{
	pGRASSFIRE ptrTempGrass = NULL;	

	if ( m_GrassfireHeap.nStackPos -1 >= 0)
	{
		ptrTempGrass = (pGRASSFIRE)&m_GrassfireHeap.Heaps[ m_GrassfireHeap.nStackPos -1 ];		
		m_GrassfireHeap.nStackPos--;
	}

	return ptrTempGrass;
}

/*======================================================================
Grassfire heap 처리 

이웃화소를 비교하여 하나의 객체로 판단하기위한 알고리즘

마지막 Grassfire 객체 정보를 가져와서 주위 3*3 이웃과 비교하여 Label이 있을 경우
동일 label을 없을 경우 새로운 label을 생성한다. 
========================================================================*/
static int Grassfire( int ** ptrMap, pIMAGE_DATA ptrImageData, int nWStart, int nWEnd , int nHStart, int nHEnd )
{
	int k, q, i, j, nLabel, nValid;
	pGRASSFIRE ptrGrassfire = PopGrassfireHeap( );
	
	if ( ptrGrassfire == NULL )
		return ERR_SUCCESS;

	nValid = 1;

	while( ptrGrassfire != NULL )
	{
		i = ptrGrassfire->nH;
		j = ptrGrassfire->nW;
		nLabel = ptrGrassfire->nLabel;
		ptrMap[ i ][ j ] = (BYTE)LIMIT_VAL( nLabel );

		for ( k = i -1 ; k <= i + 1 ;k++ )
		{
			for( q = j -1 ; q <= j + 1 ; q++ )
			{
				if( k <  ptrImageData->nH && q < ptrImageData->nW) 
				{
					if ( !( k < nHStart || k > nHEnd ) && ( !(q < nWStart || q > nWEnd ) ) ) 
					{
						if ( ptrImageData->ptrSrcImage2D[ k ][ q ] != 0  &&  ptrMap[ k ][ q ] == 0 )
						{				
							PushGrassfireHeap( k, q, nLabel );
						}
					}
				}

			}
		}

		ptrGrassfire = PopGrassfireHeap();
	}	

	return ERR_SUCCESS;
}
/*======================================================================
8-이웃 근접 방식을 이용한 객체 인식 알고리즘
번호판 영역 후보지 추출 , 번호판 문자 추출 시 사용
이미지의 전체 영역에 대해 Labeling 처리 할 경우 사용
========================================================================*/
int COM_DoLabeling8Neighbour( pIMAGE_DATA ptrImageData, int nWStart, int nWEnd, int nHStart, int nHEnd )
{
	int i, j;
	int nLabel;

	nLabel =0;

	for( i = 0 ; i < ptrImageData->nH ; i++)
	{
		memset( m_ptrMap2[ i ], 0, sizeof( int) * ptrImageData->nW );
	}

	InitGrassfireHeap();

	/*첫번째 스캔을 통한 등가 테이블 생성 및 초기 레이블 지정*/
	for( i = nHStart ; i < nHEnd ; i++ )
	{
		for( j=nWStart ; j < nWEnd ;j++ )
		{
			if ( i < ptrImageData->nH && j < ptrImageData->nW )
			{
				if ( ptrImageData->ptrSrcImage2D[ i ][ j ] != 0  && m_ptrMap2[ i ][ j ] == 0 )
				{
					nLabel++;
					PushGrassfireHeap( i, j, nLabel );
					Grassfire( m_ptrMap2, ptrImageData , nWStart, nWEnd , nHStart, nHEnd);
				}
			}
		}
	}	

	return nLabel;

}
/*======================================================================
8-이웃 근접 방식을 이용한 객체 인식 알고리즘
번호판 영역 우선 순위 추출 시 사용하는 알고리즘으로
이미지의 특정 영역에 대해 Labeling 처리 할 경우 사용
========================================================================*/
int COM_SubDoLabeling8Neighbour( pIMAGE_DATA ptrImageData,  int ** ptrMap, int nWStart, int nWEnd ,int nHStart, int nHEnd)
{
	int i, j;
	int nLabel;	

	if ( ptrMap == NULL)
		return -1;

	nLabel =0;

	for( i = 0 ; i < ptrImageData->nH ; i++)
	{
		memset( ptrMap[ i ], 0, sizeof( int) * ptrImageData->nW );
	}

	InitGrassfireHeap();

	/*첫번째 스캔을 통한 등가 테이블 생성 및 초기 레이블 지정*/
	for( i = nHStart ; i <= nHEnd; i++ )
	{
		for( j = nWStart  ; j <= nWEnd  ;j++ )
		{
			if ( i < ptrImageData->nH && j < ptrImageData->nW )
			{
				if ( ptrImageData->ptrSrcImage2D[ i ][ j ] != 0  && ptrMap[ i ][ j ] == 0 )
				{
					nLabel++;
					PushGrassfireHeap( i, j, nLabel );
					Grassfire( ptrMap, ptrImageData , nWStart, nWEnd, nHStart, nHEnd );
				}
			}
		}
	}	

	return nLabel;
}


/*======================================================================
번호판 후보지역 Line 처리 
========================================================================*/
int COM_DrawCandidateLine( int nY, int nStartX, int nEndX, pIMAGE_DATA ptrImageDataSrc )
{
	int i;
	int nUniform =0;
	for( i = 0 ; i < ptrImageDataSrc->nW ; i++ )
	{
		if ( i >= nStartX && i <= nEndX  )
		{				
			if ( nUniform == 0 )
			{
				nUniform = 255;
			}			
			else
			{
				nUniform = 0;
			}
			ptrImageDataSrc->ptrSrcImage2D[ nY ][ i ] = (BYTE)nUniform;
		}			
	}	
	return ERR_SUCCESS;
}

#define DIR_LEFT 1
#define DIR_TOP	 2

/*======================================================================
이웃화소의 유사성 판단 
========================================================================*/
static int IsSimilarity( UNIFORMITY_t **ptrUniformitys,  int i, int j, int nDirect, int nUniforH, int nUniforW )
{	
	UNIFORMITY_t * ptrUCenter, *ptrNew;
	float fu1, fu2;
	float fMean;

	( void )nUniforH;
	( void )nUniforW;
	
	ptrUCenter =& ptrUniformitys[ i ][ j ];
	
	if ( nDirect == DIR_LEFT )
		ptrNew = &ptrUniformitys[ i ][ j -1 ];
	else
		ptrNew = &ptrUniformitys[ i -1 ][ j ];

	
	fu1 = fu2 = 0.0;

	fu1 = ptrUCenter->fU;
	fu2 = ptrNew->fU;
	
	fMean = fu1 + fu2 / 2;

	if (  ptrNew->fU > fMean )
	{
		return 0;
	}	

	return 1;
}

/*======================================================================
4이웃 Labeling은 Uniformity 영역 통합에만 사용할 수 있다
이웃끼리 서로 유사한 평균과 편차를 가지고 있을 때만 동일 그룹으로 설정한다.
========================================================================*/
int COM_DoLabeling4Neighbour( UNIFORMITY_t **ptrUniformitys, int nUniforH, int nUniforW, pIMAGE_DATA ptrImageDataSrc )
{
	int i, j;
	int nLabel, nMax, nMin, nMIn_eq, nMax_eq;		
	int nTopLabel, nLeftLabel;
	int nTemp=0;
	int y;	
	int nCnt = 0;
	pCOMMONINFO ptrComInfo = NULL;
	int nNewFlag = 0;
	
	int nDirection = DIR_LEFT;

	nLabel =0;

	( void )ptrImageDataSrc;
	ptrComInfo = COM_GetInfo();

	if ( m_ptrMap == NULL || ptrComInfo == NULL)
		return ERR_ALLOC_BUF;

	for( y = 0 ; y < nUniforH ; y++)
	{
		memset( m_ptrMap[ y ], 0, sizeof( int) * nUniforW );
	}

	for( y = 0 ; y < MAX_LABEL_CNT ; y++)
	{
		memset( m_ptrEq_tbl[ y ], 0, sizeof( int) * 2 );
	}

	/*첫번째 스캔을 통한 등가 테이블 생성 및 초기 레이블 지정*/
	for( i = 0 ; i < nUniforH ; i++ )
	{
		for( j= 0 ; j < nUniforW ;j++ )
		{
			nNewFlag = 0;
			if ( ptrUniformitys[ i ][ j ].nUniform == 1 &&  ptrUniformitys[ i ][ j ].nLabelNum == 0)
			{
				
				if ( i == 0  && j == 0 )
				{
					nTopLabel = 0;
					nLeftLabel = 0;
				}
				else if ( i == 0  )
				{
					nTopLabel = 0;
					nLeftLabel = m_ptrMap[ i ][ j -1 ];
				}
				else if ( j == 0 )
				{
					nLeftLabel = 0;
					nTopLabel = m_ptrMap[ i -1 ][ j ];
				}
				else
				{
					nTopLabel = m_ptrMap[ i -1][ j ];
					nLeftLabel = m_ptrMap[ i ][ j -1 ];
				}

				/* 위쪽, 왼쪽에 레이블이 있을 경우 */
				if ( nTopLabel != 0 && nLeftLabel !=0 )
				{
					/* 두label이 동일 할 경우 */
					if ( nTopLabel == nLeftLabel )
					{
						if ( IsSimilarity( ptrUniformitys,  i, j, DIR_TOP,nUniforH, nUniforW ) )
						{
							m_ptrMap[ i ][ j ] = nTopLabel;
						}
						else
						{
							nNewFlag = 1;
						}
					}
					/* 두 label이 서로 다를 경우 */
					else
					{ 

						nMax = max(nTopLabel, nLeftLabel);
						nMin = min(nTopLabel, nLeftLabel);
						
						if ( nMin == nLeftLabel )
						{
							nDirection = DIR_LEFT;
						}
						else
						{
							nDirection = DIR_TOP;
						}						

						if ( IsSimilarity( ptrUniformitys,  i, j, nDirection,nUniforH, nUniforW ) )
						{
							m_ptrMap[ i ][ j ] = nMin;

							/* 등가 테이블 재 구성 */
							nMax_eq = max( m_ptrEq_tbl[ nMax ][ 1 ], m_ptrEq_tbl[ nMin ][ 1 ]);
							nMIn_eq = min( m_ptrEq_tbl[ nMax ][ 1 ], m_ptrEq_tbl[ nMin ][ 1 ]);

							/* 두개의 등가 테이블에서 큰 등가 테이블을 값을 작은 등가 테이블 값으로 변경 */
							//m_ptrEq_tbl[ m_ptrEq_tbl[ nMax_eq ][ 1 ] ][ 1 ] = nMIn_eq;
							m_ptrEq_tbl[ nMax][ 1 ] = nMIn_eq;
							m_ptrEq_tbl[ nMin ][ 1 ] = nMIn_eq;
						}
						else
						{
							nNewFlag = 1;
						}
					}
				}
				/* 위쪽에만 레이블이 있을 경우 */
				else if ( nTopLabel != 0 )
				{
					if ( IsSimilarity(  ptrUniformitys,  i, j, DIR_TOP,nUniforH, nUniforW  ) )
					{
						m_ptrMap[ i ][ j ] = nTopLabel;
					}
					else
					{
						nNewFlag = 1;
					}
					
				}
				/* 왼쪽에만 레이블이 있을 경우 */
				else if ( nLeftLabel != 0 )
				{
					if ( IsSimilarity(  ptrUniformitys,  i, j, DIR_LEFT,nUniforH, nUniforW  ) )
					{
						m_ptrMap[ i ][ j ] = nLeftLabel;
					}
					else
					{
						nNewFlag = 1;
					}
				}
				/* 이웃에 레이블이 없을 경우 */
				else
				{					
					if ( nLabel >= MAX_LABEL_CNT )
					{
						goto __errproc;
					}

					m_ptrMap[ i ][ j ] = nLabel;					
					m_ptrEq_tbl[ nLabel ][ 0 ]= nLabel;
					m_ptrEq_tbl[ nLabel ][ 1 ] =nLabel;
					nLabel++;
				}
			}

			if ( nNewFlag == 1 )
			{
				if ( nLabel >= MAX_LABEL_CNT )
				{
					goto __errproc;
				}

				m_ptrMap[ i ][ j ] = nLabel;					
				m_ptrEq_tbl[ nLabel ][ 0 ]= nLabel;
				m_ptrEq_tbl[ nLabel ][ 1 ] =nLabel;
				nLabel++;
			}
		}
	}

	/* 등가 테이블 정리 */	
	for( i = 0 ; i < nLabel ; i++ )
	{
		nTemp = m_ptrEq_tbl[ i ][ 1 ];

		/*재 정의된 등가값이라면 재 정의된 등가 위치의 값( 최소값 )으로 설정한다 */
		if ( nTemp != m_ptrEq_tbl[ i ][ 0 ] )
			m_ptrEq_tbl[ i ][ 1 ] = m_ptrEq_tbl[ nTemp ][ 1 ];
	}


	if ( m_ptrnHash == NULL )
		goto __errproc;

	memset( m_ptrnHash, 0, sizeof( int ) * ( ptrComInfo->nImageHeight ));

	/* 재 조정된 등가 테이블에 존재하는 객체의 번호만( 동일 label은 같은 저장소) 저장 */
	for( i = 0 ; i < nLabel ; i++ )
	{
		m_ptrnHash[ m_ptrEq_tbl[ i ][ 1 ] ] = m_ptrEq_tbl[ i ][ 1 ];
	}

	/* 등가 테이블의 객체 번호를 1번부터 재 정의 */
	for( i = 0 ; i <nLabel ; i++ )
	{
		if ( m_ptrnHash[ i ] != 0 )
			m_ptrnHash[ i ] = ++nCnt;
	}

	for( i =0 ; i < nLabel ; i++ )
	{
		m_ptrEq_tbl[ i ][ 1 ] = m_ptrnHash[ m_ptrEq_tbl[i][1] ];
	}

	/*두번째 스캔에 의한 모든 픽셀의 고유 레이블 부여 */
	for( i = 0 ; i < nUniforH ; i++ )
	{		
		for( j = 0; j < nUniforW; j++ )
		{	
			if ( ptrUniformitys[ i ][ j ].nUniform == 1)
			{
				nTemp = m_ptrMap[ i ][ j ];
				ptrUniformitys[ i ][ j ].nLabelNum = ( BYTE)(m_ptrEq_tbl[ nTemp][ 1 ]);
			}			
		}
	}

	return nCnt;

__errproc:	
	return ERR_INVALID_DATA;
}


/*======================================================================
구번호판이면서 상하 구분이 되지 않을 경우 
객체가 가장 작은 위치에서 강제로 구분한다. 
========================================================================*/
static int del_force_separ( pLPR_CONF ptrLPRConf, pCVRECT ptrRect, int * ptrComHProjection,  pIMAGE_DATA ptrImageDataSrc )
{
	int nTop , nLeft, nRight, nBottom;
	int i;

	int nPrjCnt;
	int nFirstPos, bFirstFlag;
	int bBlank, nMinCnt, nMinPos; ;
	
	nTop = ptrRect->nTop;	
	nLeft = ptrRect->nLeft;
	nRight = ptrRect->nRight;
	nBottom = ptrRect->nBottom;

	nFirstPos = bFirstFlag = bBlank = 0  ; 
	nMinCnt = 999;
	nMinPos = -1;

	/* 상하 분리 위치 찾기 */
	for( i = nTop ; i < nBottom/2 ; i++ )
	{
		nPrjCnt = *( ptrComHProjection + i );

		if ( nPrjCnt > 0 )
		{
			if ( bFirstFlag == 0 )
			{
				nFirstPos = i;
				bFirstFlag = 1;
			}

			if ( nPrjCnt < nMinCnt  )
			{
				nMinPos = i;
				nMinCnt = nPrjCnt;
			}
		}
		else
		{
			if ( bFirstFlag == 1 )
			{
				if ( ( i - nFirstPos ) > ptrLPRConf->LPR_MIN_CHAR_H )
				{
					bBlank = 1;
					break;
				}
			}
		}
	}

	if ( bBlank == 0 && nMinPos > 0 )
	{
		for( i = nLeft ; i <= nRight ; i++ )
		{
			if ( i < ptrImageDataSrc->nW )
			{
				ptrImageDataSrc->ptrSrcImage2D[ nMinPos ][ i ] = 0;
			}
		}

		*( ptrComHProjection + nMinPos ) = 0;
	}

	return ERR_SUCCESS;
}
/*======================================================================
번호판 상하 분리 지점 
========================================================================*/
int COM_GetSeparationH_MeanCnt( pLPR_CONF ptrLPRConf, pCOMMONINFO ptrComInfo, pCVRECT ptrRect, int * ptrComHProjection,  pIMAGE_DATA ptrImageDataSrc, int bEndPos , int bDelBlank )
{

	int j, nTop, nLeft, nRight, nBottom, nHeight, nWidth, nHalfH, nMinWCnt, nSeparTop, nPrjCnt ;
	float fRatio =0.0;
	BOOL bFlag;
	int nPos1, nPos2, nBLen;
	int nCnt, nSum, nSum2, nMean, nVar, nSig, nDif;
	
	nTop = ptrRect->nTop;	
	nLeft = ptrRect->nLeft;
	nRight = ptrRect->nRight;
	nBottom = ptrRect->nBottom;

	nPrjCnt	= nPos1 = nPos2 = nBLen = 0;

	nHeight = ptrRect->nBottom - ptrRect->nTop;
	nWidth = ptrRect->nRight - ptrRect->nLeft;
	nSeparTop	= nTop;

	nHalfH		= nHeight/2;	
	//nMinWCnt	= ptrLPRConf->LPR_MIN_PIXEL;
	fRatio		= (float)nWidth/ nHeight;

	memset( ptrComHProjection, 0, sizeof( int ) * ptrComInfo->nMaxHeight );
	nCnt = nSum = nSum2 = nMean = nVar = nSig = nDif = 0;
	

	if ( fRatio < ptrLPRConf->LPR_NEW_PLATE_RATIO )
	{
		bFlag = TRUE;
		HLP_ProjectImg( ptrComHProjection, ptrImageDataSrc, ptrRect, H_LINE_EDGE );

		/* 구번호판의 상,하 구분이 없을 경우 강제로 Blank를 만든다. */
		if ( bDelBlank == 1 )
		{	
			del_force_separ( ptrLPRConf, ptrRect, ptrComHProjection, ptrImageDataSrc );
		}
		
		/* 상하 분리 위치 찾기 */
		for( j = nTop ; j < nBottom/2 ; j++ )
		{
			nPrjCnt = *( ptrComHProjection + j );
			nCnt++;
			nSum += nPrjCnt;
		}
		
		if ( nCnt == 0 ) return nSeparTop;

		nMean = nSum / nCnt;
		for( j = nTop ; j < nBottom/2 ; j++ )
		{
			nPrjCnt = *( ptrComHProjection + j );
			nDif = abs( nPrjCnt - nMean );
			nSum2 += ( nDif * nDif );
		}
		nVar = nSum2 / nCnt;
		nSig = (int)sqrt( (float)nVar );
		
		nMinWCnt = nMean - nSig;

		/* 상하 분리 위치 찾기 */
		for( j = nTop ; j < nBottom/2 ; j++ )
		{
			nPrjCnt = *( ptrComHProjection + j );
			if ( nPrjCnt > ptrLPRConf->LPR_MIN_PIXEL && bFlag )
			{			
				bFlag = FALSE;
				nPos1 = j;
			}
			if ( nPrjCnt <= nMinWCnt  && ( !bFlag ))
			{
				nPos2 = j;
				bFlag = TRUE;
				nBLen = ( nPos2 - nPos1 );

				if( nBLen > ptrLPRConf->LPR_MIN_CHAR_H )
				{
					nSeparTop = j;
					break;
				}
			}
		}

		/* 상단 Bottom을 찾지 않을 경우 */
		if ( bEndPos == 0 && nSeparTop != nTop )
		{
			/* 하단 Top 찾기 */
			for( j = nSeparTop ; j < nBottom/2 ; j++ )
			{
				nPrjCnt = *( ptrComHProjection + j );

				if ( nPrjCnt >= nMinWCnt)
				{
					if ( j > nSeparTop )
					{
						nSeparTop = j;
						break;
					}
				}
			}
		}
	}

	return nSeparTop;
}

int COM_GetSeparationH_MinCnt( pLPR_CONF ptrLPRConf, pCOMMONINFO ptrComInfo, pCVRECT ptrRect, int * ptrComHProjection,  pIMAGE_DATA ptrImageDataSrc  )
{

	int j, nTop, nLeft, nRight, nBottom, nHeight, nWidth, nHalfH, nMinWCnt, nSeparTop, nPrjCnt ;
	float fRatio =0.0;
	BOOL bFlag;
	int nPos1, nPos2, nBLen;
	
	nTop = ptrRect->nTop;	
	nLeft = ptrRect->nLeft;
	nRight = ptrRect->nRight;
	nBottom = ptrRect->nBottom;

	nPrjCnt	= nPos1 = nPos2 = nBLen = 0;

	nHeight = ptrRect->nBottom - ptrRect->nTop;
	nWidth = ptrRect->nRight - ptrRect->nLeft;
	nSeparTop	= nTop;

	nHalfH		= nHeight/2;	
	nMinWCnt	= ptrLPRConf->LPR_MIN_PIXEL;
	fRatio		= (float)nWidth/ nHeight;

	memset( ptrComHProjection, 0, sizeof( int ) * ptrComInfo->nMaxHeight );
	

	if ( fRatio < ptrLPRConf->LPR_NEW_PLATE_RATIO )
	{
		bFlag = TRUE;
		HLP_ProjectImg( ptrComHProjection, ptrImageDataSrc, ptrRect, H_LINE_EDGE );					

		/* 상하 분리 위치 찾기 */
		for( j = nTop ; j < nBottom/2 ; j++ )
		{
			nPrjCnt = *( ptrComHProjection + j );
			if ( nPrjCnt > nMinWCnt && bFlag )
			{			
				bFlag = FALSE;
				nPos1 = j;
			}
			if ( nPrjCnt <= nMinWCnt  && ( !bFlag ))
			{
				nPos2 = j;
				bFlag = TRUE;
				nBLen = ( nPos2 - nPos1 );

				if( nBLen > ptrLPRConf->LPR_MIN_CHAR_H )
				{
					nSeparTop = j;
					break;
				}
			}
		}

		if ( nSeparTop != nTop )
		{
			/* 하단 Top 찾기 */
			for( j = nSeparTop ; j < nBottom/2 ; j++ )
			{
				nPrjCnt = *( ptrComHProjection + j );
				
				if ( nPrjCnt >= nMinWCnt)
				{
					if ( j > nSeparTop )
					{
						nSeparTop = j;
						break;
					}
				}
			}
		}
	}

	return nSeparTop;
}

/*======================================================================
가로 투영 메모리 요청 
========================================================================*/
int * COM_GetWProjectionBuf( void )
{
	return m_ptrWProject;
}

/*======================================================================
임시 가로 투영 메모리 요청 
차량 번호 처리용 투영 메모리 
========================================================================*/
int * COM_GetWProjection2Buf( void )
{
	return m_ptrWProject2;
}

/*======================================================================
세로 투영 메모리 요청 
========================================================================*/
int * COM_GetHProjectionBuf( void )
{
	return m_ptrHProject;
}


IMAGE_DATA_t * COM_GetChgImageBuf( void )
{
	return &m_ChgImageData;
}


IMAGE_DATA_t * COM_GetTempImageBuf( void )
{
	//int i;
	//for( i = 0 ; i < m_ConfInfo.nMaxHeight ; i++ )
	//{
	//	memset( m_TmpImageData.ptrSrcImage2D[ i ], 0, sizeof( BYTE) * m_ConfInfo.nMaxWidth );
	//}
	return &m_TmpImageData;
}

IMAGE_DATA_t * COM_GetTempImage2Buf( void )
{
	int i;
	for( i = 0 ; i < m_ConfInfo.nMaxHeight ; i++ )
	{
		memset( m_TmpImageData2.ptrSrcImage2D[ i ], 0, sizeof( BYTE) * m_ConfInfo.nMaxWidth );
	}	
	return &m_TmpImageData2;
}

IMAGE_DATA_t * COM_GetCharImageBuf( void )
{
	return &m_CharImageData;
}

LPR_CHAR_t * COM_GetLPRCharBuf( void )
{
	return &m_LPRChars;
}

CVRECT_t * COM_GetCharRectBuf( void )
{
	return m_ptrCharRect;
}

int ** COM_GetMapBuf( void )
{
	int y;
	for(y = 0 ; y < m_ConfInfo.nUniformityH ; y++)
	{		
		memset( m_ptrMap[ y ], 0, sizeof( int) * m_ConfInfo.nUniformityW );
	}
	return m_ptrMap;
}

int ** COM_GetMap2Buf( void )
{
	int y;

	for( y = 0 ; y < m_ConfInfo.nMaxHeight ; y++)
	{		
		memset( m_ptrMap2[ y ], 0, sizeof( int) * m_ConfInfo.nMaxWidth );
	}

	return m_ptrMap2;
}

pCOMMONINFO COM_GetInfo( void )
{
	return &m_ConfInfo;
}
UNIFORMITY_t ** COM_GetUniformityBuf( void )
{
	return m_ptrUniformitys;
}

CVRECT_t * COM_GetPlateRectBuf( void )
{
	return m_ptrPlateRect;
}

CVRECT_t * COM_GetTempRectBuf( void )
{
	memset( m_ptrTempRect, 0, sizeof( CVRECT_t) * MAX_LABEL_CNT);
	return m_ptrTempRect;
}

int COM_Init_RectImageBuf( void )
{
	int i, j, nUseIdx ;
	pRECTIMAGE ptrRectImg = NULL;
		
	nUseIdx = m_RectImageMng.UsedIdx;

	for( i = 0 ; i < nUseIdx ;i++ )
	{
		ptrRectImg = ( pRECTIMAGE )&m_RectImageMng.RectImage[ i ] ;
		if( ptrRectImg != NULL && ptrRectImg->ImageData.ptrSrcImage2D != NULL)
		{
			ptrRectImg->nUsed = 0;
			ptrRectImg->ImageData.nBitCount = 0;
			ptrRectImg->ImageData.nH =0;
			ptrRectImg->ImageData.nW =0;
			ptrRectImg->ImageData.nImageSize =0;

			for( j = 0 ; j < m_ConfInfo.nMaxHeight ; j++ )
			{
				memset( ptrRectImg->ImageData.ptrSrcImage2D[ j ], 0, sizeof( BYTE) * m_ConfInfo.nMaxWidth );
			}
		}
	}
	m_RectImageMng.UsedIdx=0;

	return ERR_SUCCESS;
}


int COM_CopyRectImage( IMAGE_DATA_t * ptrOrgImage )
{
	pRECTIMAGE ptrRectImg = NULL;
	int nUseIdx = 0;
	int nResult = ERR_ALLOC_BUF;

	nUseIdx = m_RectImageMng.UsedIdx;

	if ( nUseIdx < MAX_PLATE_RECT_IMAGE_CNT -1)
	{
		ptrRectImg = ( pRECTIMAGE )&m_RectImageMng.RectImage[ nUseIdx ];
		if ( ptrRectImg != NULL && ptrRectImg->ImageData.ptrSrcImage2D != NULL )
		{
			if( HLP_CopyImageDataNotAlloc( &ptrRectImg->ImageData, ptrOrgImage ) == ERR_SUCCESS )
			{
				nResult					= nUseIdx;
				ptrRectImg->nUsed		= 1;
				m_RectImageMng.UsedIdx	= nUseIdx+1;
				ptrRectImg->ImageRect.nTop =0;
				ptrRectImg->ImageRect.nLeft =0;
				ptrRectImg->ImageRect.nRight =ptrRectImg->ImageData.nW;
				ptrRectImg->ImageRect.nBottom =ptrRectImg->ImageData.nH;
			}
		}
	}

	return nResult;
}

pRECTIMAGE COM_GetRectImage( int nRectIdx )
{
	pRECTIMAGE ptrRectImg = NULL;
		
	if ( nRectIdx >=0 && nRectIdx < MAX_PLATE_RECT_IMAGE_CNT -1)
	{
		ptrRectImg = ( pRECTIMAGE )&m_RectImageMng.RectImage[ nRectIdx ] ;
	}

	return ptrRectImg;
}

int COM_SetArea( int nArea )
{
	m_Area = nArea;
	return ERR_SUCCESS;
}

int COM_GetArea( void )
{
	return m_Area;
}

