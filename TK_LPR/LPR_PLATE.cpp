#include "stdafx.h"
#include <malloc.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "../include/TK_TYPE.h"
#include "../include/TK_LIB_Helper.h"

#include "LPR_COMMON.h"
#include "LPR_DIP.h"
#include "LPR_PLATE.h"

#define PRIORITY_ORDER_TOP		1
#define PRIORITY_ORDER_BOTTOM	2
#define PRIORITY_ORDER_MID		3

#define BIG4FIND_STEP			1
#define PRJ_STEP				2

#define MAX_INT					2

#define OLD_PLAT_DIV			7
#define NEW_PLAT_DIV			10

#define SHOW_DEL_H_LINE			0
#define SHOW_DEL_NOISE			0

typedef struct POS_
{
	int nPos;
	int nCnt;
}POS_t, * pPOS ;
#define MAX_TEMP_SIZE 20

static POS_t m_TopPos[ MAX_TEMP_SIZE ];
static POS_t m_BottomPos[ MAX_TEMP_SIZE ];

static int m_nThreshold = 0;
static int m_nBeamThreshold = 0;
static int m_nMidBrightThreshold = 0;
static float m_fNewTh = 0;

static int Priority_Proc( pLPR_CONF ptrLPRConf, CVRECT_t * ptrOrgRect, CVRECT_t * ptrPrioRect,  
						  pIMAGE_DATA ptrCandidateImage, pIMAGE_DATA ptrImageDataSrc, int * ptrComWProjection, int * ptrComHProjection );

static int MakeObjectSize( int nLabelCnt, CVRECT_t * ptrRect, int ** ptrMap, pIMAGE_DATA ptrImageDataSrc, int nWStart, int nWEnd ,int nHStart, int nHEnd, int bOrder );
static int CheckValidSize( pLPR_CONF  ptrLPRConf, int nObjCnt, int nSeparTop, int nSeparCnt, CVRECT_t * ptrOrgRect, CVRECT_t * ptrRectList, pIMAGE_DATA ptrImageData );


/*===========================================================================
[ 밝은색 배경일 경우 반전 ]	
============================================================================*/
static int Plate_Inverse(pDIP_DATA ptrDIPData, pIMAGE_DATA ptrImageData  )
{
	int i, j, nErrCode = ERR_SUCCESS;	
	int nBCnt, nWCnt;
	BYTE bVal;

	nBCnt = nWCnt = 0;

	for( i = 0 ; i < ptrImageData->nH ; i++ )
	{
		for( j = 0 ; j < ptrImageData->nW ; j++ )
		{
			bVal = ptrImageData->ptrSrcImage2D[ i ][ j ];
			if ( bVal == 0 )		
				nBCnt++;
			else
				nWCnt++;
		}
	}

	if ( nWCnt >= nBCnt )
	{
		nErrCode = Inverse( ptrDIPData, ptrImageData );
	}

	return nErrCode;
}

static int Del_OutLineWithImage(pIMAGE_DATA ptrImageData )
{
	int i;

	for( i = 0 ; i < ptrImageData->nH ; i++ )
	{
		ptrImageData->ptrSrcImage2D[ i ][ 0 ] = 0;
		ptrImageData->ptrSrcImage2D[ i ][ ptrImageData->nW -1 ] = 0;
	}

	for( i = 0 ; i < ptrImageData->nW ; i++ )
	{
		ptrImageData->ptrSrcImage2D[ 0 ][ i ] = 0;
		ptrImageData->ptrSrcImage2D[  ptrImageData->nH - 1 ][ i ] =0;
	}
	return ERR_SUCCESS;
}

static int Del_OutLineWithRect( pIMAGE_DATA ptrImageDataSrc , pIMAGE_DATA ptrImageData, CVRECT_t * ptrRect )
{
	int i,j;
	int nTop, nBottom, nLeft, nRight;
	
	nTop	= ptrRect->nTop;
	nBottom = ptrRect->nBottom;
	nLeft	= ptrRect->nLeft;
	nRight	= ptrRect->nRight;

#if 1
	for( i = 0 ; i <= ptrImageData->nH ; i++ )
	{
		for( j = 0 ;j <= ptrImageData->nW; j++ )
		{
			if ( i <  ptrImageDataSrc->nH  && j < ptrImageDataSrc->nW )
			{
				if ( i <= nTop )
						ptrImageData->ptrSrcImage2D[ i ][ j ] = 0;
				
				if ( i >= nBottom )
						ptrImageData->ptrSrcImage2D[ i ][ j ] = 0;

				if ( j <= nLeft)
						ptrImageData->ptrSrcImage2D[ i ][ j ] = 0;

				if ( j >= nRight)
						ptrImageData->ptrSrcImage2D[ i ][ j ] = 0;
			}
		}
	}
#else
	for( i = nTop ; i <= nBottom ; i++ )
	{
		for( j = nLeft ;j <= nRight; j++ )
		{
			ptrImageData->ptrSrcImage2D[ i ][ j ] = 0;
		}
	}
#endif
	return ERR_SUCCESS;
}


static int Del_RectImage( int nStartH, int nEndH, int nStartW, int nEndW , pIMAGE_DATA ptrImageData )
{
	int i,j;
	int nTop, nBottom, nLeft, nRight;
	
	
	for( i = nStartH ; i <= nEndH ; i++ )
	{
		if ( i < ptrImageData->nH )
		{
			for( j = nStartW ;j <= nEndW; j++ )
			{

				if ( j < ptrImageData->nW && ptrImageData->ptrSrcImage2D[ i ][ j ] == 255 )
				{
					ptrImageData->ptrSrcImage2D[ i ][ j ] =0;
				}
			}
		}
	}

	return ERR_SUCCESS;
}

/*======================================================================
최소이상 유효한 가로 투영 필터 
========================================================================*/
static int ProjectFilter_MinW( pLPR_CONF ptrLPRConf, int * ptrWProject, pIMAGE_DATA ptrPrjImageData,  CVRECT_t * ptrRect ,float fPlateRatio , int bThin )
{
	int i ;	
	int nTop, nBottom, nLeft, nRight;
	int bCon, nLoopCnt, nCnt, nMinCnt ;
	int nHeight, nWidth, nPos, nSum, nMean;
	
	( void )bThin;
	( void )fPlateRatio;

	nTop	= ptrRect->nTop;
	nBottom = ptrRect->nBottom;
	nLeft	= ptrRect->nLeft;
	nRight	= ptrRect->nRight;
	
	nHeight = nBottom - nTop;
	nWidth  = nRight - nLeft;

	nCnt = nSum = nMean = nPos = 0;

	/*===========================================================================
	최소 문자 길이의 반을 최소로한다. 
	/2는 1이 4개 연속 올경우를 대비한것. 
	============================================================================*/
	nMinCnt = ptrLPRConf->LPR_MIN_CHAR_H / 2 ;
	
	for( i = nLeft ; i <= nRight ; i++ )
	{
		if ( i < ptrPrjImageData->nW )
		{
			nCnt = *( ptrWProject + i );

			if ( nCnt  >= nMinCnt )
			{
				if ( ptrRect->nLeft < i ) ptrRect->nLeft = i;
				break;
			}
		}
	}

	/*===========================================================================
	Right을 시작으로 최소 객체 수 이상인 위치를 새로운 Right으로 잡는다.
	============================================================================*/
	for( i = nRight ; i >= nLeft ; i-- )
	{
		if ( i < ptrPrjImageData->nW )
		{
			nCnt = *( ptrWProject + i );

			if ( nCnt  >= nMinCnt )
			{
				if ( ptrRect->nRight > i ) ptrRect->nRight = i;
				break;
			}
		}
	}
#if 1
	/* 좌측 확장 */
	bCon = 1;
	nLoopCnt = 0;
	while( bCon && nLoopCnt++ < 5)
	{
		nLeft = ptrRect->nLeft -1;
		if ( nLeft > 0 )
		{
			nCnt =  *( ptrWProject + nLeft );
			if( nCnt >=  ptrLPRConf->LPR_MIN_PIXEL - 1)
			{
				ptrRect->nLeft = nLeft;	
			}
			else
				bCon = 0;
		}
		else 
			bCon = 0;
		
	}

	/* 우측 확장 */
	bCon = 1;
	nLoopCnt = 0;
	while( bCon && nLoopCnt++ < 5)
	{
		nRight = ptrRect->nRight +1;
		if ( nRight < ptrPrjImageData->nW )
		{
			nCnt =  *( ptrWProject + nRight );
			if( nCnt >= ptrLPRConf->LPR_MIN_PIXEL - 1 )
			{
				ptrRect->nRight = nRight;	
			}
			else
				bCon = 0;
		}
		else 
			bCon = 0;

	}
#endif
	return ERR_SUCCESS;
}

/*======================================================================
구 번호판 
하단 : 중심을 기준으로 하향 하여 처음 발생되는 최소 문자 수 위치를 Bottom으로 처리
상단 :  중심을 기준으로 상향 하여 2번째 발생되는 최소 문자수 위치를 Top으로 처리 
만약 2번째 최소 문자 수가 나타나지 않을 경우 첫번째 위치를 Top으로 처리 

신 번호판
하단 : 구 번호판과 동일 
상단 : 구번호판의 1번째 최소 문자수의 위치와 동일 

========================================================================*/
static int ProjectFilter_MinH( pLPR_CONF ptrLPRConf, int * ptrHProject, pIMAGE_DATA ptrPrjImageData,  CVRECT_t * ptrRect, float fPlateRatio , int bThin )
{
	int i;	
	int nTop, nBottom, nLeft, nRight;
	int nCnt;
	int nHeight, nWidth, nPos, nSum, nMean, nHalfH;
	int nSeparPos ;

	( void )bThin;

	nTop	= ptrRect->nTop;
	nBottom = ptrRect->nBottom;
	nLeft	= ptrRect->nLeft;
	nRight	= ptrRect->nRight;

	nHeight = nBottom - nTop;
	nWidth  = nRight - nLeft;
	
	nCnt = nSum = nMean = nPos = nHalfH = 0;
	nHalfH = nTop + ( nHeight /2 );

	/* 구번호판 */
	if ( fPlateRatio < ptrLPRConf->LPR_NEW_PLATE_RATIO )
	{
		nSeparPos = -1;
		for( i = nHalfH ; i >= nTop ;i-- )
		{
			nCnt = *( ptrHProject + i );
			if ( nCnt < ptrLPRConf->LPR_MIN_PIXEL )
			{
				if( nSeparPos == -1 )
				{
					nSeparPos = i;
				}
				else 
				{
					if ( nSeparPos - i > ptrLPRConf->LPR_MIN_CHAR_H -1 )
					{
						ptrRect->nTop = i;
						break;
					}
				}
			}
		}	

	}
	/* 신번호판 */
	else
	{
		for( i = nHalfH ; i >= nTop ;i-- )
		{
			nCnt = *( ptrHProject + i );
			if ( nCnt < ptrLPRConf->LPR_MIN_PIXEL )
			{
				ptrRect->nTop = i;
				break;
			}

		}	
	}

	for( i = nHalfH ; i <= nBottom ;i++ )
	{
		if ( i < ptrPrjImageData->nH )
		{
			nCnt = *( ptrHProject + i );
			if ( nCnt < ptrLPRConf->LPR_MIN_PIXEL )
			{
				ptrRect->nBottom = i;
				break;
			}
		}
	}


	return ERR_SUCCESS;
}

/*======================================================================
영상 투영
영상의 가로 / 세로 라인 별 객체의 개수 파악 
========================================================================*/
static int Projection_DIP( pLPR_CONF ptrLPRConf, pDIP_DATA ptrDIPData, pCVRECT ptrTempRect, 
						   pIMAGE_DATA ptrCandidateImageData, BYTE EdgeType )
{
	int nErrCode= ERR_SUCCESS;

	( void )EdgeType;
	( void )ptrLPRConf;	
	( void )ptrTempRect;

	WeightedMean( ptrDIPData, ptrCandidateImageData );	
	Unsharp( ptrDIPData, ptrCandidateImageData , UNSHARP_5MASK );	

	nErrCode = Binarization_Otsu( ptrDIPData, ptrCandidateImageData );

	if ( nErrCode != ERR_SUCCESS )
		return nErrCode;

	Plate_Inverse( ptrDIPData, ptrCandidateImageData);
	Binarization_DeNoise(ptrDIPData, ptrCandidateImageData );
	Del_OutLineWithImage( ptrCandidateImageData );

	return ERR_SUCCESS;
}

#define MAX_DEL_LINE_INFO_CNT 5
typedef struct Del_Wline_
{
	int nY;
	int nStartX;
	int nEndX;
}Del_Wline_t;

typedef struct Del_Hline_
{
	int nX;
	int nStartY;
	int nEndY;
}Del_Hline_t;

typedef struct Del_Wline_Mng_
{
	int nCnt;
	Del_Wline_t Del_WLine[ MAX_DEL_LINE_INFO_CNT ];
}Del_Wline_Mng_t;

typedef struct Del_Hline_Mng_
{
	int nCnt;
	Del_Hline_t Del_HLine[ MAX_DEL_LINE_INFO_CNT ];
}Del_Hline_Mng_t;


Del_Wline_Mng_t g_Del_Wline_Mng;
Del_Hline_Mng_t g_Del_Hline_Mng;


static int Get_MaxLineX( int nCurY, int CurX, pIMAGE_DATA ptrImageData )
{	
	int nNewH;
	int x;

	int nPreCnt, nNextCnt;

	nPreCnt = nNextCnt = 0;
	
	if(  nCurY - 1 >= 0 )
	{
		for( x = CurX ; x < ptrImageData->nW ;x++ )
		{
			if ( ptrImageData->ptrSrcImage2D[ nCurY -1 ][ x ] == 255 )
			{
				nPreCnt++;
			}
			else
				break;
		}
	}

	if( nCurY + 1 < ptrImageData->nH  )
	{
		for( x = CurX ; x < ptrImageData->nW ;x++ )
		{
			if ( ptrImageData->ptrSrcImage2D[ nCurY +1 ][ x ] == 255 )
			{
				nNextCnt++;
			}
			else
				break;
		}
	}

	if ( nPreCnt == 0 && nNextCnt == 0 )
		nNewH = nCurY;
	else if ( nPreCnt > nNextCnt )
		nNewH = nCurY - 1;
	else
		nNewH = nCurY + 1;

	return nNewH;
}


static int Get_MaxLineY( int nNextY, int nCurX, pIMAGE_DATA ptrImageData )
{	
	int nNewW;
	int y;

	int nPreCnt, nNextCnt;

	nPreCnt = nNextCnt = 0;

	if(  nCurX - 1 >= 0 )
	{
		for( y = nNextY ; y < ptrImageData->nH ;y++ )
		{
			if ( ptrImageData->ptrSrcImage2D[ y ][ nCurX - 1  ] == 255 )
			{
				nPreCnt++;
			}
			else
				break;
		}
	}

	if( nCurX + 1 < ptrImageData->nW  )
	{
		for( y = nNextY ; y < ptrImageData->nH ; y++ )
		{
			if ( ptrImageData->ptrSrcImage2D[ y ][ nCurX +1 ] == 255 )
			{
				nNextCnt++;
			}
			else
				break;
		}
	}

	if ( nPreCnt == 0 && nNextCnt == 0 )
		nNewW = nCurX;
	else if ( nPreCnt > nNextCnt )
		nNewW = nCurX - 1;
	else
		nNewW = nCurX + 1;

	return nNewW;
}

/*======================================================================
[ 가로 직선을 제거한다.]
========================================================================*/
static int Del_LineW(  int nMaxLineSize, int nStartHPos, int nStartWPos, CVRECT_t * ptrChgRect, pIMAGE_DATA ptrImageData )
{
	int nResult = 0;
	
	int x, nCurX, nNextX;
	int nLineCnt; 
	int bCon, bStop, nStartX, nEndX;
	
	( void )ptrChgRect;

	nCurX = nStartWPos;
	nNextX = nCurX++;
	bCon =1;

	memset( &g_Del_Wline_Mng, 0, sizeof( Del_Wline_Mng_t ));
	
	g_Del_Wline_Mng.Del_WLine[ g_Del_Wline_Mng.nCnt ].nY = nStartHPos;
	g_Del_Wline_Mng.Del_WLine[ g_Del_Wline_Mng.nCnt ].nStartX = nCurX;
	g_Del_Wline_Mng.Del_WLine[ g_Del_Wline_Mng.nCnt ].nEndX = nCurX;

	while( bCon )
	{
		bStop = 0;

		if ( nNextX >= ptrImageData->nW )
		{
			bStop =1;
		}
		else if( ptrImageData->ptrSrcImage2D[ nStartHPos ][ nCurX ] == 255 && 
		     	ptrImageData->ptrSrcImage2D[ nStartHPos ][ nNextX ] == 255 )
		{
			nCurX +=1;
			nNextX = nCurX+1;		
			g_Del_Wline_Mng.Del_WLine[ g_Del_Wline_Mng.nCnt ].nEndX = nCurX;
		}
		else
		{
			bStop =1;
		}

		if ( bStop == 1 )
		{
			bCon = 0;
			nStartX = g_Del_Wline_Mng.Del_WLine[ g_Del_Wline_Mng.nCnt ].nStartX;
			nEndX = g_Del_Wline_Mng.Del_WLine[ g_Del_Wline_Mng.nCnt ].nEndX;

			nLineCnt = nEndX - nStartX;

			if( nLineCnt > nMaxLineSize )
			{
				for( x = nStartX ; x <= nEndX ; x++ )
				{
					ptrImageData->ptrSrcImage2D[ nStartHPos ][ x ] =0;
				}
				nResult =1;
			}
		}
	}

	return nResult;
}
/*======================================================================
[ 가로 곡선을 제거한다.]
========================================================================*/
static int Del_CuveW( int nDelType, int nMaxLineSize, int * ptrMinH, int * ptrMaxH, int nStartHPos, int nStartWPos, CVRECT_t * ptrChgRect, pIMAGE_DATA ptrImageData )
{
	int nResult = 0;
	int bCon,i, x ;
	int nCnt;
	int nLineSize =0;
	int bNewLine, bStop =0;
	int nCurX, nNextX, nCurY, nPreY, nNextY, nTempY, nNewY, nStartX, nEndX;
	int nYLineSize;

	( void )ptrChgRect;

	memset( &g_Del_Wline_Mng, 0, sizeof( Del_Wline_Mng_t ));	
	nCurY = nStartHPos;
	nNextY = nCurY+1; 
	nPreY  = nCurY-1;

	nCurX = nStartWPos;
	nNextX = nCurX+1;

	bCon =1;

	g_Del_Wline_Mng.Del_WLine[ g_Del_Wline_Mng.nCnt ].nY = nCurY;
	g_Del_Wline_Mng.Del_WLine[ g_Del_Wline_Mng.nCnt ].nStartX = nCurX;

	nLineSize =0;
	bNewLine = 0;
	nTempY = 0;
	nCnt = 0;

	while( bCon )
	{
		bStop = 0;

		/* 영상 크기보다 크면 종료 */
		if( nNextX >= ptrImageData->nW || nNextX < 0 )
		{
			bStop = 1;
		}
		else if( ptrImageData->ptrSrcImage2D[ nCurY ][nNextX ] == 255  )
		{
			
		}
		/* 상,하 모두 객체가 있을경우 */
		else if( ( ptrImageData->ptrSrcImage2D[ nCurY ][nNextX ] == 0  ) &&
			     ( (nPreY >=0 ) && ptrImageData->ptrSrcImage2D[ nPreY ][nNextX ] == 255 ) ||
			     ( ( nNextY < ptrImageData->nH ) &&  ptrImageData->ptrSrcImage2D[ nNextY ][nNextX ] == 255 ) )
		{
			/* 가장 긴 직선을 가진 Y축을 새로운 Y로 변경한다. */
			nTempY = Get_MaxLineX( nCurY, nNextX, ptrImageData );
			if ( nTempY != nCurY )
			{
				nCurY	= nTempY ;
				nNextY	= nCurY+1; 
				nPreY	= nCurY-1;
				
				/* Del Wline 갱신 */			
				if ( g_Del_Wline_Mng.nCnt+1 >= MAX_DEL_LINE_INFO_CNT )
				{
					bStop =1;
				}
				else
				{
					bNewLine = 1;					
				}
			}			
		}
		else
		{
			bStop = 1;
		}
		
		if ( !bStop )
		{	
			if ( bNewLine )
			{
				g_Del_Wline_Mng.Del_WLine[ g_Del_Wline_Mng.nCnt ].nEndX = nCurX;
				g_Del_Wline_Mng.nCnt++;

				g_Del_Wline_Mng.Del_WLine[ g_Del_Wline_Mng.nCnt ].nY		= nTempY;
				g_Del_Wline_Mng.Del_WLine[ g_Del_Wline_Mng.nCnt ].nStartX	= nNextX;
				g_Del_Wline_Mng.Del_WLine[ g_Del_Wline_Mng.nCnt ].nEndX		= nNextX;

				bNewLine = 0;
			}
			else
			{
				g_Del_Wline_Mng.Del_WLine[ g_Del_Wline_Mng.nCnt ].nEndX = nCurX;
			}			
			
			if( nDelType == DEL_L_CUVE )
			{
				nCurX++;
				nNextX = nCurX+1;
			}
			else
			{
				nCurX--;
				nNextX = nCurX-1;
			}
			
		}

		if ( bStop == 1 )
		{
			bCon		= 0;			
			nCnt		= g_Del_Wline_Mng.nCnt;
			
			nLineSize	= g_Del_Wline_Mng.Del_WLine[ nCnt ].nEndX - g_Del_Wline_Mng.Del_WLine[ 0 ].nStartX;
			nYLineSize  = g_Del_Wline_Mng.Del_WLine[ nCnt ].nY - g_Del_Wline_Mng.Del_WLine[ 0 ].nY;
			if ( nLineSize > nMaxLineSize )
			{
				for( i = 0 ; i <= nCnt ; i++ )
				{
					nNewY = g_Del_Wline_Mng.Del_WLine[ i ].nY;
					nStartX = g_Del_Wline_Mng.Del_WLine[ i ].nStartX;
					nEndX = g_Del_Wline_Mng.Del_WLine[ i ].nEndX;

					for( x = nStartX ; x <= nEndX ; x++ )
					{
						ptrImageData->ptrSrcImage2D[ nNewY ][x] = 0;
					}

					if ( *ptrMinH > nNewY ) *ptrMinH = nNewY;
					if ( *ptrMaxH < nNewY ) *ptrMaxH = nNewY;
				}

				nResult = 1;
			}
		}

	}

	return nResult;
}

/*======================================================================
[ 세로 직선 객체가 없을때까지 삭제한다. ]
========================================================================*/
static int Del_Line_H( pLPR_CONF ptrLPRConf, int nChgPos, int nMaxH, int nWPos, CVRECT_t * ptrChgRect, pIMAGE_DATA ptrImageData )
{
	int nNewW;

	int nLeft, nTop, nBottom, nRight;
	int i, j, nCnt;
	int nHeight;

	nTop	= ptrChgRect->nTop;
	nLeft	= ptrChgRect->nLeft;
	nRight	= ptrChgRect->nRight;
	nBottom = ptrChgRect->nBottom;
	nHeight = ptrChgRect->nBottom - ptrChgRect->nTop;

	/* Left Line */
	nNewW = nLeft + nWPos;
	for( j = nLeft ; j < nNewW ; j++ )
	{
		nCnt =0;
		for( i = nTop ; i < nBottom ; i++ )
		{	
			if ( ptrImageData->ptrSrcImage2D[ i ][ j ] == 255 )
			{
				nCnt++;
			}
		}

		if ( ( nCnt >= nMaxH ) )
		{
			for( i = nTop +1; i < nBottom-1; i++ )
			{
				if ( ptrImageData->ptrSrcImage2D[ i ][ j ] == 255 )
				{
					ptrImageData->ptrSrcImage2D[ i ][ j ] =0;
				}
			}

			if ( nChgPos && ptrChgRect->nLeft < j ) ptrChgRect->nLeft = j;
		}
	}

	/* Right Line */
	nNewW = nRight - nWPos;
	for( j = nRight ; j > nNewW ; j-- )
	{
		for( i = nTop +1; i < nBottom-1; i++ )
		{
			nCnt =0;
			if ( ptrImageData->ptrSrcImage2D[ i ][ j ] == 255 )
			{
				nCnt++;
			}
		}

		if ( nCnt >= nMaxH )
		{
			for( i = nTop +1; i < nBottom-1; i++ )
			{
				if ( ptrImageData->ptrSrcImage2D[ i ][ j ] == 255 )
				{
					ptrImageData->ptrSrcImage2D[ i ][ j ] =0;
				}
			}

			if ( nChgPos &&  ptrChgRect->nRight > j ) ptrChgRect->nRight = j;
		}
	}

	return ERR_SUCCESS;
}

/*======================================================================
[ 가로 곡선을 제거한다.]
========================================================================*/

static int Del_CuveH( int nDelType, pLPR_CONF ptrLPRConf, int nMaxLineSize, int * ptrMinW, int * ptrMaxW, int nStartHPos, int nStartWPos, CVRECT_t * ptrChgRect, pIMAGE_DATA ptrImageData )
{
	int nResult = 0;
	int bCon,i, x, y ;
	int nCnt;
	int nLineSize =0;
	int bNewLine, bStop =0;
	int nCurX, nNextY, nCurY, nPreX, nNextX, nTempX, nNewX, nStartY, nEndY;

	( void )ptrChgRect;

	memset( &g_Del_Hline_Mng, 0, sizeof( g_Del_Hline_Mng ));	

	nCurX = nStartWPos;
	nCurY = nStartHPos;
	nNextY = nCurY+1;
	
	nPreX = nCurX -1;
	nNextX = nCurX+1;
	bCon =1;

	g_Del_Hline_Mng.Del_HLine[ g_Del_Hline_Mng.nCnt ].nX = nCurX;
	g_Del_Hline_Mng.Del_HLine[ g_Del_Hline_Mng.nCnt ].nStartY = nCurY;

	nLineSize =0;
	bNewLine = 0;

	nTempX = 0;
	nCnt = 0;

	while( bCon )
	{
		bStop = 0;

		/* 영상 크기보다 크면 종료 */
		if( nNextY >= ptrImageData->nH || nNextY < 0 )
		{
			bStop = 1;
		}
		else if( ptrImageData->ptrSrcImage2D[ nNextY ][nCurX ] == 255  )
		{

		}
		/* 상,하 모두 객체가 있을경우 */
		else if( ( ptrImageData->ptrSrcImage2D[ nNextY ][nCurX ] == 0  ) &&
			( (nPreX >=0 ) && ptrImageData->ptrSrcImage2D[ nNextY ][nPreX ] == 255 ) ||
			( ( nNextX < ptrImageData->nW ) &&  ptrImageData->ptrSrcImage2D[ nNextY ][nNextX ] == 255 ) )
		{
			/* 가장 긴 직선을 가진 Y축을 새로운 Y로 변경한다. */
			nTempX = Get_MaxLineY( nNextY, nCurX, ptrImageData );
			if ( nTempX != nCurX )
			{
				nCurX	= nTempX ;
				nNextX	= nCurX+1; 
				nPreX	= nCurX-1;

				/* Del Wline 갱신 */			
				if ( g_Del_Wline_Mng.nCnt+1 >= MAX_DEL_LINE_INFO_CNT )
				{
					bStop =1;
				}
				else
				{
					bNewLine = 1;					
				}
			}			
		}
		else
		{
			bStop = 1;
		}

		if ( !bStop )
		{	
			if ( bNewLine )
			{
				g_Del_Hline_Mng.Del_HLine[ g_Del_Hline_Mng.nCnt ].nEndY = nCurY;
				g_Del_Hline_Mng.nCnt++;

				g_Del_Hline_Mng.Del_HLine[ g_Del_Hline_Mng.nCnt ].nX		= nTempX;
				g_Del_Hline_Mng.Del_HLine[ g_Del_Hline_Mng.nCnt ].nStartY	= nNextY;
				g_Del_Hline_Mng.Del_HLine[ g_Del_Hline_Mng.nCnt ].nEndY		= nNextY;

				bNewLine = 0;
			}
			else
			{
				g_Del_Hline_Mng.Del_HLine[ g_Del_Hline_Mng.nCnt ].nEndY = nCurY;
			}			
			nCurY++;
			nNextY = nCurY+1;
		}

		if ( bStop == 1 )
		{
			bCon		= 0;			
			nCnt		= g_Del_Hline_Mng.nCnt;

			nLineSize	= g_Del_Hline_Mng.Del_HLine[ nCnt ].nEndY - g_Del_Hline_Mng.Del_HLine[ 0 ].nStartY;

			if ( ( nLineSize > nMaxLineSize )  && ( g_Del_Hline_Mng.Del_HLine[ 0 ].nStartY < ptrChgRect->nTop + ptrLPRConf->LPR_MIN_PIXEL ||
				                                  g_Del_Hline_Mng.Del_HLine[ nCnt ].nEndY > ptrChgRect->nBottom - ptrLPRConf->LPR_MIN_PIXEL ))
			{
				for( i = 0 ; i <= nCnt ; i++ )
				{
					nNewX = g_Del_Hline_Mng.Del_HLine[ i ].nX;
					nStartY = g_Del_Hline_Mng.Del_HLine[ i ].nStartY;
					nEndY = g_Del_Hline_Mng.Del_HLine[ i ].nEndY;

					for( y = nStartY ; y <= nEndY ; y++ )
					{
						ptrImageData->ptrSrcImage2D[ y ][nNewX] = 0;
					}

					if ( *ptrMinW > nNewX ) *ptrMinW = nNewX;
					if ( *ptrMaxW < nNewX ) *ptrMaxW = nNewX;
				}

				nResult = 1;
			}
		}

	}

	return nResult;
}


/*======================================================================
영상 투영
영상의 가로 / 세로 라인 별 객체의 개수 파악하여 
유효한 객체의 크기와 영역을 파악한다. 
========================================================================*/
static int Del_PlateLine_W( int nDelType, int nChgPos, pLPR_CONF ptrLPRConf, CVRECT_t * ptrChgRect, pIMAGE_DATA ptrImageData,  pCOMMONINFO ptrComInfo, int * ptrComHProjection, int nMaxPrjCntW )
{
	
	int i, j, nHalfH, nHalfW , nHeight, nWidth ;
	int nLeftPos, nRightPos, nTopPos, nBottomPos;

	BOOL bWFirst ;
	int nBPos, nLen;
	int nBCnt = 0;
	int nMinH, nMaxH;
	
	( void )ptrComHProjection;
	( void )ptrComInfo;

	nLeftPos = nRightPos = nTopPos = nBottomPos = nHalfH = nHalfW = 0 ;

	nTopPos		= ptrChgRect->nTop;
	nBottomPos	= ptrChgRect->nBottom;
	nLeftPos	= ptrChgRect->nLeft;
	nRightPos	= ptrChgRect->nRight;
	
	nHeight = nBottomPos - nTopPos;
	nWidth  = nRightPos - nLeftPos ;

	nHalfH = ptrChgRect->nTop + ( nHeight / 2 );
	nHalfW = ptrChgRect->nLeft +( nWidth / 2 );

	/*===========================================================================
	[ 가로 직선 삭제 ]
	 연속된 영역의 길이가 번호판 최소 크기( UF * 2 ) 보다 길면 테두리로 인식
	============================================================================*/
	if ( nDelType == DEL_LINE )
	{	
		for( i = nTopPos ; i < nHalfH ; i++ )
		{			
			for( j = nLeftPos ; j < nRightPos  ; j++ )
			{
				if ( ptrImageData->ptrSrcImage2D[ i  ][ j ] == 255 )
				{
					if ( Del_LineW( nMaxPrjCntW, i, j,  ptrChgRect, ptrImageData ) == 1 )
					{	
						if (nChgPos && i > ptrChgRect->nTop ) ptrChgRect->nTop = i;
					}
				}
			}
		}

		for( i = nHalfH ; i < nBottomPos ; i++ )
		{	
			for( j = nLeftPos ; j < nRightPos  ; j++ )
			{
				if ( ptrImageData->ptrSrcImage2D[ i  ][ j ] == 255 )
				{
					if ( Del_LineW( nMaxPrjCntW, i, j,  ptrChgRect, ptrImageData ) == 1 )
					{	
						if ( nChgPos && i < ptrChgRect->nBottom ) ptrChgRect->nBottom = i;
					}
				}
			}
		}

	}

	/*===========================================================================
	[ 가로 ( Top, Left ) 기준에서 아래로 굽은 선 삭제 ]
	 영역의 연결 길이가 번호판 최소 크기( UF * 2 ) 보다 길면 테두리로 인식
	============================================================================*/	
	if ( nDelType == DEL_L_CUVE || nDelType == DEL_R_CUVE )
	{		
		if ( nDelType == DEL_L_CUVE )
		{
			for( i = nTopPos ; i < nHalfH ; i++ )
			{
				nMinH = ptrChgRect->nTop;
				nMaxH = ptrChgRect->nTop;

				for( j = nLeftPos ; j < nHalfW ; j++ )
				{
					if ( ptrImageData->ptrSrcImage2D[ i  ][ j ] == 255 )
					{
						if ( Del_CuveW( DEL_L_CUVE,nMaxPrjCntW, &nMinH, &nMaxH ,i, j,  ptrChgRect, ptrImageData ) == 1 )
						{	
							if (nChgPos && nMaxH > ptrChgRect->nTop ) ptrChgRect->nTop = nMaxH;
						}
					}
				}
			}

			for( i = nHalfH ; i < nBottomPos ; i++ )
			{
				nMinH = ptrChgRect->nBottom;
				nMaxH = ptrChgRect->nBottom;

				for( j = nLeftPos ; j < nHalfW ; j++ )
				{
					if ( ptrImageData->ptrSrcImage2D[ i  ][ j ] == 255 )
					{
						if ( Del_CuveW( DEL_L_CUVE, nMaxPrjCntW, &nMinH, &nMaxH ,i, j,  ptrChgRect, ptrImageData ) == 1 )
						{	
							if ( nChgPos && nMinH < ptrChgRect->nBottom ) ptrChgRect->nBottom = nMinH;
						}
					}
				}
			}
		}

		if ( nDelType == DEL_R_CUVE )
		{
			for( i = nTopPos ; i < nHalfH ; i++ )
			{
				nMinH = ptrChgRect->nTop;
				nMaxH = ptrChgRect->nTop;
				for( j = nRightPos ; j > nHalfW ; j-- )
				{
					if ( ptrImageData->ptrSrcImage2D[ i  ][ j ] == 255 )
					{
						if ( Del_CuveW( DEL_R_CUVE, nMaxPrjCntW, &nMinH, &nMaxH ,i, j,  ptrChgRect, ptrImageData ) == 1 )
						{	
							if ( nChgPos && nMaxH > ptrChgRect->nTop ) ptrChgRect->nTop = nMaxH;
						}
					}
				}
			}

			for( i = nHalfH ; i < nBottomPos ; i++ )
			{
				nMinH = ptrChgRect->nBottom;
				nMaxH = ptrChgRect->nBottom;
				for( j = nRightPos ; j > nHalfW ; j-- )
				{
					if ( ptrImageData->ptrSrcImage2D[ i  ][ j ] == 255 )
					{
						if ( Del_CuveW( DEL_L_CUVE, nMaxPrjCntW, &nMinH, &nMaxH ,i, j,  ptrChgRect, ptrImageData ) == 1 )
						{	
							if ( nChgPos && nMinH < ptrChgRect->nBottom ) ptrChgRect->nBottom = nMinH;
						}
					}
				}
			}

		}
	}

	/*===========================================================================
	[ 상단 볼트 삭제 ]
	 객체의 중심을 바탕으로 좌, 우  객체의 수가 없을 경우 볼트 부분으로 간주

	 a. 가로 세로 비율이 4.7 이하 ( 구 번호판 )
	 b. 객체의 가로 중심을 포함할 것 
	 c. 객체의 간격이 UF * 2보다 긴것이 존재
	============================================================================*/
	if ( nDelType == DEL_BLT )
	{	
		if ( nHeight == 0 ) nHeight = 1;
		if ( ( nWidth / nHeight ) < ptrLPRConf->LPR_NEW_PLATE_RATIO )		
		{
			
			for( i = nTopPos ; i < nHalfH ; i++ )
			{
				bWFirst = FALSE;				
				nBPos = -1;				
				nLen = 0;

				for( j = nLeftPos ; j < nRightPos ; j++ )
				{
					if (bWFirst == FALSE && ptrImageData->ptrSrcImage2D[ i  ][ j ] == 255 )					 
					{
						if ( nBPos > 0 )
						{
							nLen = ( j - nBPos );

							if ( nLen > ptrLPRConf->LPR_MIN_PLT_W && ( nHalfW >= nBPos && nHalfW <= j ) )
							{
								nBCnt++;
								if ( i > ptrChgRect->nTop && nBCnt > 2 )
								{
									ptrChgRect->nTop = i;
									break;
								}
							}
							else if ( nLen < ptrLPRConf->LPR_MIN_PLT_W && ( nHalfW >= nBPos && nHalfW <= j ) )
							{								
								i = nHalfH;
							}
							else if ( nLen < ptrLPRConf->LPR_MIN_PLT_W )
							{
								nBCnt = 0 ;
							}
						}
						
						nBPos = -1;
						bWFirst = TRUE;
					}
					
					if( bWFirst == TRUE && ptrImageData->ptrSrcImage2D[ i  ][ j ] == 0 )
					{
						nBPos = j;
						bWFirst = FALSE;
					}					
				}
			}
		}
	}

	return ERR_SUCCESS;
}


/*======================================================================
[ 세로 테두리 제거 필터 구현]
1. 유럽형 번호판일 경우 번호판 앞, 뒤 각각 1/4지점까지 검색하여 
번호판 높이전체를 포함하고 있는 직선은 테두리라 판단하여 삭제한다. 

2. 구 번호판의 상위 영역의 앞, 뒤 쪽은 숫자또는 문자가 존재하지 않는 부분으로
이부분에 세로 객체가 존재하면 삭제한다. 
========================================================================*/
#if 0
static int Del_PlateLine_H(  int nDelType,int nChgPos , pLPR_CONF ptrLPRConf, CVRECT_t * ptrChgRect, pIMAGE_DATA ptrPrjImageData,  pCOMMONINFO ptrComInfo, int * ptrComHProjection )
{
	int i, j, nWidth, nHeight, nLeft, nRight,nTop, nBottom, nNewLeft, nNewRight , nHalfH;
	int nSegPosH,nWPos= 0;
	int nMaxPrjCntH;
	int nMinW, nMaxW;

	nTop	= ptrChgRect->nTop;
	nLeft	= ptrChgRect->nLeft;
	nRight	= ptrChgRect->nRight;
	nBottom = ptrChgRect->nBottom;

	nWidth	= ptrChgRect->nRight - ptrChgRect->nLeft;
	nHeight = ptrChgRect->nBottom - ptrChgRect->nTop;	

	nHalfH	=  ptrChgRect->nTop + ( nHeight/ 2);
	nMaxPrjCntH = ( nHeight/ 2);
	nMinW = nMaxW =0;

	if ( nHeight <= 0 )
		return ERR_INVALID_DATA;

	/* 세로 라인삭제지점 계산 */
	nWPos = nWidth / 7;	
	nSegPosH = 0;		
	nSegPosH = COM_GetSeparationH_MeanCnt( ptrLPRConf, ptrComInfo, ptrChgRect, ptrComHProjection,ptrPrjImageData, 1, 0 );

	nNewLeft = nLeft;
	nNewRight = nRight;

	if ( nSegPosH != nTop )
	{
		nNewLeft  = nLeft + nWPos;
		nNewRight = nRight - nWPos;
	}

	if ( nDelType == DEL_LINE )
	{	
		Del_Line_H( ptrLPRConf, nMaxPrjCntH, nChgPos, nWPos, ptrChgRect, ptrPrjImageData );
	}
	else if ( nDelType == DEL_L_CUVE || nDelType == DEL_R_CUVE )
	{
#if 1
		for( j = nLeft ; j <= nNewLeft ; j++ )
		{
			nMinW = nMaxW = j;

			for( i = nTop ; i < nHalfH ; i++ )
			{
				if ( Del_CuveH( DEL_L_CUVE, ptrLPRConf, nMaxPrjCntH, &nMinW, &nMaxW, i, j,  ptrChgRect, ptrPrjImageData ) == 1 )
				{	
					if ( nChgPos && nMaxW > ptrChgRect->nLeft ) ptrChgRect->nLeft = nMaxW;
				}
			}
		}

		for( j = nRight ; j > nNewLeft ; j-- )
		{
			nMinW = nMaxW = j;

			for( i = nTop ; i < nHalfH ; i++ )
			{	
				if ( Del_CuveH( DEL_R_CUVE, ptrLPRConf, nMaxPrjCntH, &nMinW, &nMaxW ,i, j,  ptrChgRect, ptrPrjImageData ) == 1 )
				{	
					if ( nChgPos && nMinW < ptrChgRect->nRight ) ptrChgRect->nRight = nMinW;
				}
			}
		}
#endif
	}

	return ERR_SUCCESS;
}


#else

/*======================================================================
[ L자형 세로 테두리는 중간에 한번 끊어준다.]
========================================================================*/
static int Cut_ThineLineH(pLPR_CONF ptrLPRConf, CVRECT_t * ptrPlateRect, CVRECT_t * ptrRect, pIMAGE_DATA ptrPrjImageData, int * ptrComWProjection, int * ptrStartX, int * ptrEndX )
{
	int nTop, nLeft, nRight, nBottom, nWidth, nPlateWith, nPlateHalfW;
	int x, nObjCnt;
	
	nTop		= ptrRect->nTop;
	nLeft		= ptrRect->nLeft;
	nRight		= ptrRect->nRight;
	nBottom		= ptrRect->nBottom;

	nWidth		= nRight - nLeft;
	nPlateWith  = ptrPlateRect->nRight - ptrPlateRect->nLeft;
	nPlateHalfW		= ptrPlateRect->nLeft + ( nPlateWith/ 2);

	nObjCnt = 0;
	
	*ptrStartX	= nLeft;
	*ptrEndX	= nRight;

	if ( nWidth > ptrLPRConf->LPR_MIN_PIXEL )
	{						
		HLP_ProjectImg(ptrComWProjection, ptrPrjImageData,ptrRect, W_LINE_EDGE );

		/*왼쪽 테두리 일경우 */
		if ( nRight < nPlateHalfW )
		{

			for( x = nLeft ; x <= nRight; x++ )
			{
				nObjCnt = *(ptrComWProjection + x);
				if ( nObjCnt < ptrLPRConf->LPR_MIN_PIXEL )
				{
					*ptrStartX = nLeft;
					*ptrEndX = x;
					break;
				}
			}
		}
		else
		{	
			for( x = nRight ; x >= nLeft; x-- )
			{
				nObjCnt = *(ptrComWProjection + x);
				if ( nObjCnt < ptrLPRConf->LPR_MIN_PIXEL )
				{
					*ptrStartX = x;
					*ptrEndX = nRight;
					break;
				}
			}
		}
	}

	return ERR_SUCCESS;
}

static int Del_PlateLine_H(  int nDelType,int nChgPos , pLPR_CONF ptrLPRConf, CVRECT_t * ptrChgRect, pIMAGE_DATA ptrImageData,  
						      pCOMMONINFO ptrComInfo, int * ptrComHProjection, int * ptrComWProjection )
{
	int i, j, nWidth, nHeight, nLeft, nRight,nTop, nBottom, nNewW;
	float fPlateRatio;
	int nSegPosH,nWPos, nObjCnt, nCnt = 0;
	CVRECT_t TempRect;
	int nOldPlate = 0;
	int ** ptrComMap = NULL;
	CVRECT_t * ptrComRect = NULL;
	CVRECT_t * ptrTempRect = NULL;
	int nMaxH, nHalfW, nSumH, nCntH, nMeanH;
	int nStartX, nEndX, x,y;
	int nVerify = 0;
	
	int nSumTop,nTopCnt,nMeanTop;
	int nSumBottom, nBottomCnt, nMeanBottom ;


	nHeight = ptrChgRect->nBottom - ptrChgRect->nTop;	
	nWidth	= ptrChgRect->nRight - ptrChgRect->nLeft;	

	if ( nHeight <= 0 )
		return ERR_INVALID_DATA;
	
	nTop		= ptrChgRect->nTop;
	nLeft		= ptrChgRect->nLeft;
	nRight		= ptrChgRect->nRight;
	nBottom		= ptrChgRect->nBottom;
	nSegPosH	= nTop;
	nHalfW		= ptrChgRect->nLeft + ( nWidth/ 2);

	ptrComMap	= COM_GetMap2Buf( );
	ptrComRect	= COM_GetTempRectBuf( );
	fPlateRatio = HLP_Cal_ImageRatio( ptrChgRect );
	nMaxH		= nHeight - ptrLPRConf->LPR_MIN_PIXEL;

	nSumH = nCntH = nMeanH = 0;
	nSumTop = nTopCnt = nMeanTop = 0;
	nSumBottom = nBottomCnt = nMeanBottom = 0;

	if ( fPlateRatio < ptrLPRConf->LPR_NEW_PLATE_RATIO )
	{
		ptrComHProjection = COM_GetHProjectionBuf();
		if ( ptrComHProjection == NULL )
		{
			ptrChgRect->nSeqNum = -1;
			return ERR_NO_PARAM;
		}
		nSegPosH = COM_GetSeparationH_MeanCnt( ptrLPRConf, ptrComInfo, ptrChgRect, ptrComHProjection,ptrImageData, 1, 1 );

		nWPos = nWidth / OLD_PLAT_DIV;
	}
	else
	{
		nWPos = nWidth / NEW_PLAT_DIV;
	}
	
	if ( nTop != nSegPosH )
	{
		nOldPlate = 1;
	}	

	nObjCnt = 0;
	nObjCnt = COM_SubDoLabeling8Neighbour( ptrImageData, ptrComMap, nLeft, nRight, nTop, nBottom );
	if ( nObjCnt >= MAX_LABEL_CNT )
	{
		ptrChgRect->nSeqNum = -1;
		return ERR_NO_PARAM;
	}
	
	if ( nObjCnt > 0 )
		MakeObjectSize( nObjCnt, ptrComRect, ptrComMap, ptrImageData, nLeft, nRight, nTop, nBottom, 1 );	

	/* 객체가 테두리 범위에 있으면서 크기가 비슷할때 삭제 */
	for( i = 0 ; i < nObjCnt ; i++ )
	{
		ptrTempRect = (CVRECT_t *)&ptrComRect[ i ];

		if ( ptrTempRect->nSeqNum >= 0 )
		{
			nTop	= ptrTempRect->nTop;
			nBottom = ptrTempRect->nBottom;
			nLeft	= ptrTempRect->nLeft;
			nRight	= ptrTempRect->nRight;

			nHeight = nBottom - nTop;
			nWidth	= nRight - nLeft;

			/* 4자리 숫자에 대해서만 평균 구함 */
			if ( nTop > nSegPosH && nHeight > ptrLPRConf->LPR_MIN_BIG4_H )
			{
				nSumH += nHeight;
				nCntH++;

				nSumTop += nTop;
				nTopCnt++;

				nSumBottom += nBottom;
				nBottomCnt++;

			}
		}
	}
	
	if ( nCntH < ptrLPRConf->LPR_MIN_CHAR_CNT )
	{
		ptrChgRect->nSeqNum = -1;
		return ERR_NO_PARAM;
	}

	nMeanH = nSumH / nCntH;
	nMeanTop = nSumTop / nCntH;
	nMeanBottom = nSumBottom / nCntH;

	/* 객체가 테두리 범위에 있으면서 크기가 비슷할때 삭제 */
	for( i = 0 ; i < nObjCnt ; i++ )
	{
		ptrTempRect = (CVRECT_t *)&ptrComRect[ i ];

		if ( ptrTempRect->nSeqNum >= 0 )
		{
			nTop	= ptrTempRect->nTop;
			nBottom = ptrTempRect->nBottom;
			nLeft	= ptrTempRect->nLeft;
			nRight	= ptrTempRect->nRight;

			nHeight = nBottom - nTop;
			nWidth	= nRight - nLeft;
			
			nVerify = 1;
			
			/*=======================================================
			테두리 영역에 있으면서 넓이가 작은것은 테두리
			========================================================*/
			if ( nWidth < ptrLPRConf->LPR_MIN_PIXEL  && 
			    ( nLeft <= nWPos ||  nRight >= ( ptrChgRect->nRight - nWPos) ))
			{	

				Del_RectImage( nTop, nBottom, nLeft, nRight , ptrImageData );
				ptrTempRect->nSeqNum = -1;				
				nVerify = 0;
				if ( SHOW_DEL_H_LINE ) HLP_DrawLineRect( ptrImageData, ptrTempRect, 180 );
			}

			/*=======================================================
			테두리 영역에 있으면서 4자리 숫자의 평균 높이보다 큰 객체는 테두리라 판단 
			========================================================*/
			if ( nMeanH + ptrLPRConf->LPR_MIN_PIXEL < nHeight && 
			    ( nLeft <= nWPos ||  nRight >= ( ptrChgRect->nRight - nWPos) ))
			{
				Cut_ThineLineH( ptrLPRConf, ptrChgRect, ptrTempRect, ptrImageData, ptrComWProjection, &nStartX, &nEndX);

				Del_RectImage( nTop, nBottom, nStartX, nEndX , ptrImageData );					
				ptrTempRect->nSeqNum = -1;				
				nVerify = 0;

				if ( SHOW_DEL_H_LINE ) HLP_DrawLineRect( ptrImageData, ptrTempRect, 180 );
			}

			/*=======================================================
			테두리 영역에 있으면서 번호판의 높이와 비슷한 객체는
			세로 테두리로 판다. 
			========================================================*/
			/* 테두리 가능 영영만 체크 */
			if ( nVerify && ( nRight <= nWPos  ||
				 nLeft <= nWPos ||
				 nLeft >= ( ptrChgRect->nRight - nWPos) ||
				 nRight >= ( ptrChgRect->nRight - nWPos) ))
			{
				/* 번호판의 크기와 비슷할때 */
				if ( nHeight >= nMaxH  )
				{
					/* L 자형 테두리는 끊어준다. */					
					Cut_ThineLineH( ptrLPRConf,ptrChgRect,  ptrTempRect, ptrImageData, ptrComWProjection, &nStartX, &nEndX);

					Del_RectImage( nTop, nBottom, nStartX, nEndX , ptrImageData );					
					ptrTempRect->nSeqNum = -1;					
					nVerify = 0;

					if ( SHOW_DEL_H_LINE ) HLP_DrawLineRect( ptrImageData, ptrTempRect, 180 );

				}
			}
			
			/*=======================================================
			구 번호판은 TOP위치에 지역 코드 이외의 객체는 테두리로 판단.
			========================================================*/
			if ( nVerify && nOldPlate && ( nTop <= nSegPosH ) &&
			   ( nLeft <= ( ptrChgRect->nLeft +ptrLPRConf->LPR_MIN_PIXEL)  || nRight >= ( ptrChgRect->nRight - ptrLPRConf->LPR_MIN_PIXEL) ))
			{

				Del_RectImage( nTop, nBottom, nLeft, nRight , ptrImageData );
				ptrTempRect->nSeqNum = -1;				
				nVerify = 0;

				if ( SHOW_DEL_H_LINE ) HLP_DrawLineRect( ptrImageData, ptrTempRect, 180 );
			}

			/*=======================================================
			구 번호판은 TOP위치에 지역 코드 이외의 객체는 테두리로 판단.
			========================================================*/
			if ( nVerify && nOldPlate && nWidth < ptrLPRConf->LPR_MIN_CHAR_W * 2  && 
			   ( nTop <= ( nMeanTop - ptrLPRConf->LPR_MIN_PIXEL  ) || ( nTop >= ( nMeanTop + ptrLPRConf->LPR_MIN_PIXEL  ))) &&
			   ( nLeft <= nWPos ||  nRight >= ( ptrChgRect->nRight - nWPos) ))
			{

				Del_RectImage( nTop, nBottom, nLeft, nRight , ptrImageData );
				ptrTempRect->nSeqNum = -1;				
				nVerify = 0;

				if ( SHOW_DEL_H_LINE ) HLP_DrawLineRect( ptrImageData, ptrTempRect, 180 );
			}

			/*=======================================================
			신 번호판이면서 테두리영역이고 TOP BOOTOM이 평균보다 작은 위치.
			========================================================*/
			if ( nVerify && !nOldPlate && ( nTop < ( nMeanTop - ptrLPRConf->LPR_MIN_PIXEL  )) &&
			   ( nLeft <= nWPos ||  nRight >= ( ptrChgRect->nRight - nWPos) ))
			{
				Del_RectImage( nTop, nBottom, nLeft, nRight , ptrImageData );
				ptrTempRect->nSeqNum = -1;				
				nVerify = 0;

				if ( SHOW_DEL_H_LINE ) HLP_DrawLineRect( ptrImageData, ptrTempRect, 180 );
			}
		}
	}	

	return ERR_SUCCESS;
}
#endif


/*======================================================================
크기가 작은 영역의 제거 
- 잡음 제거 필터 수정 
1. 전체 8이웃 Labeling 기법으로 전체 객체의 크기를 구한다. 
2. Left위치로  Order by 한다
3. 4자리 숫자 크기보다 큰 객체의 첫번째 left와 마지막 right을 추출한다
추출한 첫번째 left 보다 이전 위치에 있는 객체 중 with가 좁은것은 잡음으로 간주한다. 
추출한 마지막 right 보다 이후 위치에 있는 객체 중 with가 좁은것은 잡음으로 간주한다. 
4. 4자리 숫자 크기보다 큰 객체의 평균 bottom을 추출하여 평균 botttom 이후에 있는 객체는 잡음으로 간주한다. 

신, 구 번호판을 구분하여 각각의 큰 객체의 평균 top을 구하고 평균 top보다 높은 객체는 잡음으로 간주 한다. 
========================================================================*/
static int internal_Del_Noise( pLPR_CONF ptrLPRConf, CVRECT_t * ptrChgRect, pIMAGE_DATA ptrImageData,pCOMMONINFO ptrComInfo , float fPlateRatio)
{
	int ** ptrComMap = NULL;
	CVRECT_t * ptrComRect = NULL;
	CVRECT_t * ptrTempRect = NULL;
	CVRECT_t * ptrNextRect = NULL;

	int i,j, x,y, nLoopCnt, bCon, nObjCnt = 0;
	int nTop, nBottom, nLeft, nRight, nHeight, nWidth;
	int nNewTop, nNewBottom,nNewLeft, nNewRight;

	int nSegPosH, nOldPlate;
	
	int nSumTop,nTopCnt,nMeanTop, nLastRight, nFirstLeft;
	int nBigSumTop, nBigSumBottom, nBigTopCnt, nBigBottomCnt, nMeanBigTop, nMeanBigBottom;

	int bVerify;
	int * ptrComHProjection = NULL;
	int nPlateHeight, nPlateHalfH;

	int nNextLeft, nNextRight, nNextHeight;
	int nMaxPrjCntW ;
	/*===========================================================================
	[영역내 큰 객체의 Labeling ]
	============================================================================*/	
	ptrComMap  = COM_GetMap2Buf();
	ptrComRect = COM_GetTempRectBuf();
	
	/* 구버전 번호판 */
	nTop	= ptrChgRect->nTop;
	nBottom = ptrChgRect->nBottom;
	nLeft	= ptrChgRect->nLeft;
	nRight	= ptrChgRect->nRight;
	nOldPlate = 0;

	nSegPosH = nTop;
	nPlateHeight = nBottom - nTop;
	nPlateHalfH = ptrChgRect->nTop + ( nPlateHeight / 2 );

	if ( fPlateRatio < ptrLPRConf->LPR_NEW_PLATE_RATIO )
	{			
		ptrComHProjection = COM_GetHProjectionBuf();
		if ( ptrComHProjection == NULL )
		{
			ptrChgRect->nSeqNum = -1;
			return ERR_NO_PARAM;
		}
		nSegPosH = COM_GetSeparationH_MeanCnt( ptrLPRConf, ptrComInfo, ptrChgRect, ptrComHProjection,ptrImageData, 1, 1 );
	}	
	
	if ( nTop != nSegPosH )
	{
		nOldPlate = 1;
	}

	nObjCnt = 0;
	nObjCnt = COM_SubDoLabeling8Neighbour( ptrImageData, ptrComMap, nLeft, nRight, nTop, nBottom );
	if ( nObjCnt >= MAX_LABEL_CNT || nObjCnt < ptrLPRConf->LPR_MIN_CHAR_CNT )
	{
		ptrChgRect->nSeqNum = -1;
		return ERR_NO_PARAM;
	}
	/*===========================================================================
	[번호판 영역 내 객체 크기 정보 산출  ]	
	============================================================================*/
	if ( nObjCnt > 0 )
		MakeObjectSize( nObjCnt, ptrComRect, ptrComMap, ptrImageData, nLeft, nRight, nTop, nBottom, 1 );

	nLoopCnt =0;
	bCon =1;

	while( bCon && nLoopCnt++ < 3 )
	{
		bCon = 0;

		nSumTop = nTopCnt = nMeanTop =  0;
		nBigSumTop = nBigSumBottom = nBigTopCnt = nBigBottomCnt = nMeanBigTop = nMeanBigBottom =0;

		nLastRight = 0;
		nFirstLeft =-1;

		for( i = 0 ; i < nObjCnt ; i++ )
		{
			ptrTempRect = (CVRECT_t *)&ptrComRect[ i ];
			
			if ( ptrTempRect->nSeqNum >= 0 )
			{
				nTop	= ptrTempRect->nTop;
				nBottom = ptrTempRect->nBottom;
				nLeft	= ptrTempRect->nLeft;
				nRight	= ptrTempRect->nRight;

				nHeight = nBottom - nTop;
				nWidth	= nRight - nLeft;		
				bVerify = 1;

				/* =============================================================
				큰 객체의 평균 Top , Bottom 위치 추출 
				================================================================*/
				/* 구번호판 */
				if ( nOldPlate )
				{
					if ( nTop < nSegPosH && nHeight > ptrLPRConf->LPR_MIN_CHAR_H )
					{
						nSumTop += nTop;
						nTopCnt++;
					}
				}
				else
				{
					if ( nHeight > ptrLPRConf->LPR_MIN_BIG4_H )
					{
						nSumTop += nTop;
						nTopCnt++;
					}
				}

				/* 큰 객체의 평균 TOP과 Bottom을 추출한다 */
				if ( nHeight > ptrLPRConf->LPR_MIN_BIG4_H )
				{
					nBigSumTop += nTop;
					nBigTopCnt++;
				}

				if ( nHeight > ptrLPRConf->LPR_MIN_BIG4_H )
				{
					nBigSumBottom += nBottom;
					nBigBottomCnt++;
				}
				
				/* =============================================================
				큰 객체의 최초 Left, 마지막 Right 추출
				================================================================*/
				if ( ( nFirstLeft == -1 ) && 
					 (nHeight > ptrLPRConf->LPR_MIN_BIG4_H /2) && ( nWidth >= ptrLPRConf->LPR_MIN_BIG4_W/2  ) )
				{
					nFirstLeft = ptrTempRect->nLeft;
				}

				if ( ( nHeight > ptrLPRConf->LPR_MIN_BIG4_H /2) && ( nWidth >= ptrLPRConf->LPR_MIN_BIG4_W/2 ) )
				{
					if ( nLastRight < ptrTempRect->nRight )
					{
						nLastRight = ptrTempRect->nRight;
					}
				}
			}
		}
		
		if ( nTopCnt == 0 || nBigTopCnt== 0 || nBigBottomCnt == 0 )
		{
			ptrChgRect->nSeqNum = -1;
			return ERR_NO_PARAM;
		}
		
		nMeanTop		= nSumTop /nTopCnt;
		nMeanBigTop		= nBigSumTop /nBigTopCnt;
		nMeanBigBottom	= nBigSumBottom /nBigBottomCnt;

		/*==================================================================
		Top 이상 또는 Bottom 이하 작은 가로 직선 제거 
		===================================================================*/		
		nMaxPrjCntW = ptrLPRConf->LPR_MIN_CHAR_W * 6;

		for( i = ptrChgRect->nTop ; i < nMeanTop ; i++ )
		{	
			for( j = ptrChgRect->nLeft ; j < ptrChgRect->nRight  ; j++ )
			{
				if ( ptrImageData->ptrSrcImage2D[ i  ][ j ] == 255 &&
					 ptrImageData->ptrSrcImage2D[ i+1][ j ] == 0 &&
					 ptrImageData->ptrSrcImage2D[ i-1][ j ] == 0 )
				{	
					Del_LineW( nMaxPrjCntW, i, j,  ptrChgRect, ptrImageData );
				}
			}
		}

		for( i = nMeanBigBottom ; i < ptrChgRect->nBottom ; i++ )
		{	
			for( j = ptrChgRect->nLeft ; j < ptrChgRect->nRight  ; j++ )
			{
				if ( ptrImageData->ptrSrcImage2D[ i  ][ j ] == 255  &&
					 ptrImageData->ptrSrcImage2D[ i+1][ j ] == 0  &&
					 ptrImageData->ptrSrcImage2D[ i-1][ j ] == 0  )
				{
					Del_LineW( nMaxPrjCntW, i, j,  ptrChgRect, ptrImageData );
				}
			}
		}
		/*==================================================================
		잡음 유무 체크
		===================================================================*/
		for( i = 0 ; i < nObjCnt ; i++ )
		{
			ptrTempRect = (CVRECT_t *)&ptrComRect[ i ];

			if ( ptrTempRect->nSeqNum >= 0 )
			{
				nTop	= ptrTempRect->nTop;
				nBottom = ptrTempRect->nBottom;
				nLeft	= ptrTempRect->nLeft;
				nRight	= ptrTempRect->nRight;

				nHeight = nBottom - nTop;
				nWidth	= nRight - nLeft;		
				bVerify = 1;				

				
				/*=======================================================
				좌, 우에 있는 객체의 넓이가 작을 경우 
				========================================================*/
				if ( i == 0 || i == nObjCnt - 1 )
				{
					/* 숫작 객체보다 bottom이 위에 있는 객체 */
					if ( bVerify && nWidth < ptrLPRConf->LPR_MIN_PIXEL )
					{	
						Del_RectImage( nTop, nBottom, nLeft, nRight , ptrImageData );
						ptrTempRect->nSeqNum = -1;						
						bVerify = 0;

						if ( SHOW_DEL_NOISE ) HLP_DrawLineRect( ptrImageData, ptrTempRect, 180 );
					}
				}

				/*=======================================================
				구 번호판이면서 좌, 우 객체의 넓이가 좁고 객체간 사이가 좁을 때 
				========================================================*/
				if ( nOldPlate )
				{
					if ( i == 0  && nWidth < ptrLPRConf->LPR_MIN_PIXEL * 2 )
					{
						ptrNextRect = (CVRECT_t *)&ptrComRect[ 1 ];

						nNextLeft = ptrNextRect->nLeft;
						nNextRight = ptrNextRect->nRight;
						nNextHeight = ptrNextRect->nBottom - ptrNextRect->nTop;

						if ( nNextHeight > ptrLPRConf->LPR_MIN_CHAR_H &&  ( nNextLeft - nRight ) < ptrLPRConf->LPR_MIN_PIXEL )
						{
							Del_RectImage( nTop, nBottom, nLeft, nRight , ptrImageData );
							ptrTempRect->nSeqNum = -1;							
							bVerify = 0;

							if ( SHOW_DEL_NOISE ) HLP_DrawLineRect( ptrImageData, ptrTempRect, 180 );
						}
					}

					if ( i == nObjCnt - 1 && nWidth < ptrLPRConf->LPR_MIN_PIXEL * 2)
					{
						ptrNextRect = (CVRECT_t *)&ptrComRect[ nObjCnt - 2 ];
						
						nNextLeft = ptrNextRect->nLeft;
						nNextRight = ptrNextRect->nRight;
						nNextHeight = ptrNextRect->nBottom - ptrNextRect->nTop;

						if ( nNextHeight > ptrLPRConf->LPR_MIN_CHAR_H && ( nNextRight - nLeft ) < ptrLPRConf->LPR_MIN_PIXEL )
						{
							Del_RectImage( nTop, nBottom, nLeft, nRight , ptrImageData );
							ptrTempRect->nSeqNum = -1;							
							bVerify = 0;

							if ( SHOW_DEL_NOISE ) HLP_DrawLineRect( ptrImageData, ptrTempRect, 180 );
						}
					}
				}
				
				/*=======================================================
				객체의 Bottom이 4자리 숫자의 TOP 보다 크고 높이가 작을 경우 
				========================================================*/
				if ( bVerify && nBottom < nMeanTop && nHeight <= ptrLPRConf->LPR_MIN_PIXEL )
				{
					Del_RectImage( nTop, nBottom, nLeft, nRight , ptrImageData );
					ptrTempRect->nSeqNum = -1;					
					bVerify = 0;

					if ( SHOW_DEL_NOISE ) HLP_DrawLineRect( ptrImageData, ptrTempRect, 180 );
				}
				
				/*=======================================================
				객체의 길이 , 높이가 모두 작을 경우 
				자신의 2배의 길이, 높이에 또다른 객체가 없을 경우
				========================================================*/
				if ( bVerify &&  nHeight <= ptrLPRConf->LPR_MIN_PIXEL && nWidth <= ptrLPRConf->LPR_MIN_PIXEL )
				{
					nNewTop		= nTop - ptrLPRConf->LPR_MIN_PIXEL;
					nNewBottom	= nBottom + ptrLPRConf->LPR_MIN_PIXEL;

					nNewLeft	= nLeft - ptrLPRConf->LPR_MIN_PIXEL;
					nNewRight	= nRight + ptrLPRConf->LPR_MIN_PIXEL;
					
					if ( nNewTop < ptrChgRect->nTop ) nNewTop = ptrChgRect->nTop;
					if ( nNewBottom > ptrChgRect->nBottom ) nNewBottom = ptrChgRect->nBottom;
					if ( nNewLeft < ptrChgRect->nLeft ) nNewLeft = ptrChgRect->nLeft;
					if ( nNewRight > ptrChgRect->nRight ) nNewRight = ptrChgRect->nRight;

					bVerify =0;

					for( y = nNewTop ; y <= nNewBottom ;y++ )
					{
						if (bVerify == 1 ) break;

						for( x = nNewLeft ;x <= nNewRight ; x++ )
						{
							if ( ( y < nTop || y > nBottom ) &&
								 ( x < nLeft || x > nRight ) )
							{

								if (ptrImageData->ptrSrcImage2D[ y ][ x ]== 255 )
								{
									bVerify =1;
									break;
								}
							}
						}
					}

					if ( bVerify == 0) 
					{
						Del_RectImage( nTop, nBottom, nLeft, nRight , ptrImageData );
						ptrTempRect->nSeqNum = -1;
						if ( SHOW_DEL_NOISE ) HLP_DrawLineRect( ptrImageData, ptrTempRect, 180 );
					}
				}		

				/*=======================================================
				객체의 top이 4자리 숫자의 bottom보다 낮고 높이가 작을 때
				========================================================*/
				if ( bVerify  && ( nTop >= nMeanBigBottom && nHeight < ptrLPRConf->LPR_MIN_PIXEL *2 ))
				{
					Del_RectImage( nTop, nBottom, nLeft, nRight , ptrImageData );
					ptrTempRect->nSeqNum = -1;					
					bVerify = 0;

					if ( SHOW_DEL_NOISE ) HLP_DrawLineRect( ptrImageData, ptrTempRect, 180 );
				}
				
				/*=======================================================
				구번호판이고 위쪽 문자의 높이와 길이가 너무 작을 때
				========================================================*/
				if ( bVerify && nOldPlate && nBottom <= nMeanTop && ( nTop < nSegPosH ) && nHeight < ptrLPRConf->LPR_MIN_PIXEL )
				{
					Del_RectImage( nTop, nBottom, nLeft, nRight , ptrImageData );
					ptrTempRect->nSeqNum = -1;					
					bVerify = 0;

					if ( SHOW_DEL_NOISE ) HLP_DrawLineRect( ptrImageData, ptrTempRect, 180 );
				}				

				/*=======================================================
				구번호판이고 아래쪽 문자의 높이와 길이가 너무 작을 때
				========================================================*/
				if ( bVerify && nOldPlate && ( nTop > nSegPosH ) && nHeight < ptrLPRConf->LPR_MIN_PIXEL )
				{
					Del_RectImage( nTop, nBottom, nLeft, nRight , ptrImageData );
					ptrTempRect->nSeqNum = -1;
					bVerify = 0;

					if ( SHOW_DEL_NOISE ) HLP_DrawLineRect( ptrImageData, ptrTempRect, 180 );
				}				
				
				/*=======================================================
				구번호판이고 TOP의 위치가 번호판의 하단에 존재할때 
				========================================================*/
				if ( bVerify && nOldPlate && ( nTop > ( nPlateHalfH + ptrLPRConf->LPR_MIN_CHAR_H )))
				{
					Del_RectImage( nTop, nBottom, nLeft, nRight , ptrImageData );
					ptrTempRect->nSeqNum = -1;					
					bVerify = 0;

					if ( SHOW_DEL_NOISE ) HLP_DrawLineRect( ptrImageData, ptrTempRect, 180 );
				}				

				/*=======================================================
				첫번째 숫자의 왼쪽보다 오른쪽에 있으면서 넓이, 높이가 작은것 
				========================================================*/
				if ( bVerify && ( nFirstLeft >= ptrTempRect->nRight ) && 
					 nWidth <= ptrLPRConf->LPR_MIN_CHAR_W  && nHeight <= ptrLPRConf->LPR_MIN_BIG4_H  )
				{
					Del_RectImage( nTop, nBottom, nLeft, nRight , ptrImageData );
					ptrTempRect->nSeqNum = -1;					
					bVerify = 0;

					if ( SHOW_DEL_NOISE ) HLP_DrawLineRect( ptrImageData, ptrTempRect, 180 );
				}
				
				/*=======================================================
				마지막 숫자의 오른쪽 보다 왼쪽에 있으면서 넓이, 높이가 작은것 
				========================================================*/
				if ( bVerify && ( nLastRight <= ptrTempRect->nLeft ) && 
					nWidth <= ptrLPRConf->LPR_MIN_CHAR_W && nHeight <= ptrLPRConf->LPR_MIN_BIG4_H )
				{
					Del_RectImage( nTop, nBottom, nLeft, nRight , ptrImageData );
					ptrTempRect->nSeqNum = -1;					
					bVerify = 0;

					if ( SHOW_DEL_NOISE ) HLP_DrawLineRect( ptrImageData, ptrTempRect, 180 );
				}

				/* 최소 픽셀 */
				if ( bVerify == 0 )
				{
					ptrTempRect->nSeqNum = -1;					
					bCon =1;
				}
			}
		}
	}
	return ERR_SUCCESS;
}

/*======================================================================
번호판 좌, 우 영역에 불필요하게 포함된 악세사리 영역 제거
( 번호판 좌, 우 영역에 존재하는 해드 라이트, 악세사리등  )
========================================================================*/
static int Del_PlateSideAreaH( int nChgPos, pLPR_CONF ptrLPRConf, CVRECT_t * ptrChgRect, pIMAGE_DATA ptrImageData,  pCOMMONINFO ptrComInfo, int * ptrComWProjection )
{
	int nHalfW, nLeft,nRight, nTop, nBotttom, nHeight;
	int i,j, nCnt;
	int bCheck;
	int nMinCnt, nLastMinCnt ; 
	int nMaxLen;
	int nFirstPos ;
	int nLastPos ;
	int nDel ;

	nLeft	= ptrChgRect->nLeft;
	nRight	= ptrChgRect->nRight;	
	nTop	= ptrChgRect->nTop;
	nBotttom = ptrChgRect->nBottom;	

	nHalfW = nLeft + ( nRight - nLeft )/2;	
	nHeight = nBotttom - nTop;

	nMinCnt		= nHeight / 3; 
	nMaxLen		= ptrLPRConf->LPR_UF_W ;

	if ( HLP_ProjectImg( ptrComWProjection, ptrImageData, ptrChgRect, W_LINE_EDGE) != ERR_SUCCESS )
		return ERR_NO_PARAM;
	
	/* =================================================
	Left 기준 
	===================================================*/
	nCnt = bCheck = nFirstPos = nLastPos = nDel = 0;
	nFirstPos = -1;
	for( i = 0 ; i < nHalfW ; i++ )
	{
		nCnt = *( ptrComWProjection + i );
		if ( nCnt > nMinCnt )
		{
			if ( nFirstPos < 0 ) 
			{
				nFirstPos  = i;
			}
			else
			{
				if ( ( i - nFirstPos ) > nMaxLen )
				{
					nDel = 1;
				}
			}			
		}
		else if ( nFirstPos >= 0 && nCnt < nMinCnt )
		{
			nLastPos = i;
			break;
		}
	}

	if ( nFirstPos >= 0 && nLastPos == 0)
	{
		nLastPos = nHalfW;
	}

	if ( nDel && ( nLastPos - nFirstPos >= nMaxLen ))
	{
		for( i = nTop ; i < nBotttom ; i++ )
		{
			for( j = nFirstPos ; j <= nLastPos ; j++)
			{
				ptrImageData->ptrSrcImage2D[ i ][ j ] = 0;
			}
		}

		if ( nChgPos && ( ptrChgRect->nLeft < nLastPos )) 
			ptrChgRect->nLeft = nLastPos;
	}

	/* =================================================
	Right 기준 
	===================================================*/
	nCnt = bCheck = nFirstPos = nLastPos = nDel = 0;
	nFirstPos = -1;
	for( i = nRight ; i > nHalfW ; i-- )
	{
		nCnt = *( ptrComWProjection + i );
		if ( nCnt > nMinCnt )
		{
			if ( nFirstPos < 0 ) 
			{
				nFirstPos  = i;
			}
			else
			{
				if ( ( nFirstPos - i ) > nMaxLen )
				{
					nDel = 1;
				}
			}			
		}
		else if ( nFirstPos >= 0 && nCnt < nMinCnt )
		{
			nLastPos = i;
			break;
		}
	}

	if ( nFirstPos >= 0 && nLastPos == 0)
	{
		nLastPos = nHalfW;
	}

	if ( nDel && ( nFirstPos - nLastPos >= nMaxLen ))
	{
		for( i = nTop ; i < nBotttom ; i++ )
		{
			for( j = nLastPos ; j <= nFirstPos ; j++)
			{
				ptrImageData->ptrSrcImage2D[ i ][ j ] = 0;
			}
		}

		if ( nChgPos && ( ptrChgRect->nRight > nLastPos )) 
			ptrChgRect->nRight = nLastPos;
	}

	return ERR_SUCCESS;
}

/*======================================================================
테두리 영역 제거 
========================================================================*/
static int internal_Del_Outer( pLPR_CONF ptrLPRConf, CVRECT_t * ptrChgRect, pIMAGE_DATA ptrImageData,pCOMMONINFO ptrComInfo, int * ptrComWProjection, int * ptrComHProjection, int nStep )
{
	int nMaxPrjCntW =0;

	memset( ptrComWProjection, 0, sizeof( int ) * ptrComInfo->nImageWidth );
	memset( ptrComHProjection, 0, sizeof( int ) * ptrComInfo->nImageHeight );

	/*===========================================================================
	[ 가로 테두리 제거 ]	 
	 STEP 1 : 직선 삭제
	 STEP 2 : 볼트 삭제 
	 STEP 3 : 오른쪽으로 굽은 선 삭제 
	 STEP 4 : 왼쪽으로 굽은 선 삭제 
	============================================================================*/
	if( nStep == PRJ_STEP )
	{		
		Del_PlateSideAreaH( 1 , ptrLPRConf, ptrChgRect, ptrImageData, ptrComInfo, ptrComWProjection);
	}

	memset( ptrComWProjection, 0, sizeof( int ) * ptrComInfo->nImageWidth );
	memset( ptrComHProjection, 0, sizeof( int ) * ptrComInfo->nImageHeight );

	if ( nStep == BIG4FIND_STEP )
	{
		nMaxPrjCntW = ptrLPRConf->LPR_MIN_PLT_W ;

		if( ptrLPRConf->LPR_DBG_RUN_BIG4_DEL_W_CUVE )
		{
			Del_PlateLine_W( DEL_L_CUVE, 0, ptrLPRConf, ptrChgRect, ptrImageData, ptrComInfo, ptrComHProjection , nMaxPrjCntW);
			Del_PlateLine_W( DEL_R_CUVE, 0,  ptrLPRConf, ptrChgRect, ptrImageData, ptrComInfo, ptrComHProjection , nMaxPrjCntW);	
		}

		if( ptrLPRConf->LPR_DBG_RUN_BIG4_DEL_W_LINE )
			Del_PlateLine_W( DEL_LINE,  0, ptrLPRConf, ptrChgRect, ptrImageData, ptrComInfo, ptrComHProjection , nMaxPrjCntW);

		if( ptrLPRConf->LPR_DBG_RUN_BIG4_DEL_W_BLT )
			Del_PlateLine_W( DEL_BLT,  0, ptrLPRConf, ptrChgRect, ptrImageData, ptrComInfo, ptrComHProjection , nMaxPrjCntW);
	}
	else if ( nStep == PRJ_STEP )
	{
		nMaxPrjCntW = ptrLPRConf->LPR_MIN_PLT_W * 1.6 ;		

		if( ptrLPRConf->LPR_DBG_RUN_PRJ_DEL_W_CUVE )
		{
			Del_PlateLine_W( DEL_L_CUVE,  1, ptrLPRConf, ptrChgRect, ptrImageData, ptrComInfo, ptrComHProjection , nMaxPrjCntW);
			Del_PlateLine_W( DEL_R_CUVE,  1, ptrLPRConf, ptrChgRect, ptrImageData, ptrComInfo, ptrComHProjection , nMaxPrjCntW);	
		}

		if( ptrLPRConf->LPR_DBG_RUN_PRJ_DEL_W_LINE )
			Del_PlateLine_W( DEL_LINE,  1, ptrLPRConf, ptrChgRect, ptrImageData, ptrComInfo, ptrComHProjection , nMaxPrjCntW);
		
		if( ptrLPRConf->LPR_DBG_RUN_PRJ_DEL_W_BLT )
			Del_PlateLine_W( DEL_BLT,  1, ptrLPRConf, ptrChgRect, ptrImageData, ptrComInfo, ptrComHProjection , nMaxPrjCntW);
	}
	
	/*===========================================================================
	[ 세로 테두리 삭제 ]	 
	============================================================================*/
	if( nStep == PRJ_STEP && ptrLPRConf->LPR_DBG_RUN_PRJ_DEL_H_LINE )
	{
		memset( ptrComWProjection, 0, sizeof( int ) * ptrComInfo->nImageWidth );
		memset( ptrComHProjection, 0, sizeof( int ) * ptrComInfo->nImageHeight );
		Del_PlateLine_H( DEL_LINE, 1 , ptrLPRConf, ptrChgRect, ptrImageData, ptrComInfo, ptrComHProjection, ptrComWProjection);
	}

	return ERR_SUCCESS;
}


static int internal_Project( pLPR_CONF ptrLPRConf, CVRECT_t * ptrChgRect, pIMAGE_DATA ptrImageData, pCOMMONINFO ptrComInfo, int * ptrComWProjection, 
							 int * ptrComHProjection, float fPlateRatio  , int bThin  )
{

	/*===========================================================================
	[ 투영 처리 처리 ]
	수평 엣지 처리는 2회 반복한다	
	============================================================================*/	
	memset( ptrComWProjection, 0, sizeof( int ) * ptrComInfo->nImageWidth );
	memset( ptrComHProjection, 0, sizeof( int ) * ptrComInfo->nImageHeight );
		
	/*===========================================================================
	[ 수평 엣지( 투영) 처리 ]
	============================================================================*/
	if ( HLP_ProjectImg( ptrComHProjection, ptrImageData, ptrChgRect, H_LINE_EDGE ) != ERR_SUCCESS )
		return ERR_NO_PARAM;

	if ( HLP_ProjectImg( ptrComWProjection, ptrImageData, ptrChgRect, W_LINE_EDGE ) != ERR_SUCCESS )
		return ERR_NO_PARAM;

	/*===========================================================================
	[ 수직 투영 필터  ]
	객체의 테두리부분과 잡음 영역을 제거한 수직 영역(top, bottom )을 재 설정 한다. 
	============================================================================*/
	if ( ProjectFilter_MinH(ptrLPRConf, ptrComHProjection, ptrImageData, ptrChgRect, fPlateRatio, bThin ) != ERR_SUCCESS )
		return ERR_NO_PARAM;
	
	/*===========================================================================
	[ 수평 투영 필터  ]
	객체의 테두리부분과 잡음 영역을 제거한 수평 영역(left, right )을 재 설정 한다. 
	============================================================================*/
	if ( ProjectFilter_MinW(ptrLPRConf, ptrComWProjection, ptrImageData, ptrChgRect, fPlateRatio, bThin ) != ERR_SUCCESS )
		return ERR_NO_PARAM;

	return ERR_SUCCESS;
}

static int  Projection_Proc( pLPR_CONF ptrLPRConf, pDIP_DATA ptrDIPData, CVRECT_t * ptrOrgRect, CVRECT_t * PrjRect, 
							 pIMAGE_DATA ptrCandidateImage, pIMAGE_DATA ptrImageDataSrc, int * ptrComWProjection, int * ptrComHProjection  )
{
	pCOMMONINFO ptrComInfo = NULL;
	float fPlateRatio;	

	ptrComInfo = COM_GetInfo();	
	
	if ( ptrComWProjection == NULL  || ptrComHProjection == NULL || ptrComInfo == NULL )
		return ERR_NO_PARAM;

	/*===========================================================================
	[ Projection을 위한 기본 DIP  ]
	============================================================================*/		
	Projection_DIP( ptrLPRConf, ptrDIPData, ptrOrgRect, ptrCandidateImage, H_LINE_EDGE );
	
	/*===========================================================================
	[ 가로/가로 잡음 직선 제거 삭제 ]
	============================================================================*/	
	internal_Del_Outer( ptrLPRConf, PrjRect,ptrCandidateImage,ptrComInfo,ptrComWProjection,ptrComHProjection, PRJ_STEP );
		
	/*===========================================================================
	[ 잡음 제거 ]
	============================================================================*/
	fPlateRatio = HLP_Cal_ImageRatio( PrjRect );

	if( ptrLPRConf->LPR_DBG_RUN_PRJ_DEL_NOISE )
	{
		/* 유효하지 않은 영역은 모두 배경 처리 한다. */
		Del_OutLineWithRect( ptrImageDataSrc, ptrCandidateImage, PrjRect );
		internal_Del_Noise( ptrLPRConf, PrjRect, ptrCandidateImage, ptrComInfo, fPlateRatio );
	}
	/*===========================================================================
	[ 투영 처리 처리 ]
	수평 엣지 처리는 2회 반복한다	
	============================================================================*/
	if( ptrLPRConf->LPR_DBG_RUN_PRJ_DETAIL_RECT )
	{
		internal_Project( ptrLPRConf, PrjRect,ptrCandidateImage,ptrComInfo,ptrComWProjection,ptrComHProjection, fPlateRatio, FALSE );
	}

	ptrOrgRect->nLeft += ( PrjRect->nLeft - 0 );
	ptrOrgRect->nRight += ( PrjRect->nRight - ptrCandidateImage->nW ) ;
	ptrOrgRect->nTop += ( PrjRect->nTop - 0 );
	ptrOrgRect->nBottom += ( PrjRect->nBottom - ptrCandidateImage->nH );

	return ERR_SUCCESS;
}

/* =========================================================================================================================================
Priority
============================================================================================================================================*/

/*=========================================================================
번호판 내에 객체 사이즈의 유효성 판단.
==========================================================================*/
static int CheckValidSize( pLPR_CONF  ptrLPRConf, int nObjCnt, int nSeparTop, int nSeparCnt, CVRECT_t * ptrOrgRect, 
						    CVRECT_t * ptrRectList, pIMAGE_DATA ptrImageData )
{
	int nOrgCnt , i, k,q;
	int nWidth, nHeight, nLeft, nTop, nRight, nBottom;
	int nMaxWidth;

	CVRECT_t * ptrTempRect;
	int nDelCnt;
	int nVerifyCnt;
	int nSubSeparCnt;
	BOOL bVerify;	
	
	float fMeanTop, fMeanBottom, fSigTop, fSigBottom, fVarTop, fVarBottom;
	int nDelTopCnt, nDelBottomCnt, nSumTop, nSumBottom, nMinTop, nMaxTop, nMinBottom, nMaxBottom ;
	
	( void )nSeparTop;

	nOrgCnt = nObjCnt;
	nWidth = nHeight = nTop = nBottom = nLeft = nRight = nDelCnt = nMaxWidth =nVerifyCnt=nSubSeparCnt=0;

	fMeanTop = fMeanBottom = fSigTop = fSigBottom= fVarTop= fVarBottom =0.0;
	nSumTop = nSumBottom = nMinTop= nMaxTop = nMinBottom= nMaxBottom= nDelTopCnt = nDelBottomCnt = 0;

	/*===============================================================
	평균 
	================================================================*/
	for( i = 0 ; i < nObjCnt ; i++ )
	{
		ptrTempRect = (CVRECT_t *)&ptrRectList[ i ];
		
		nTop = ptrTempRect->nTop;
		nBottom = ptrTempRect->nBottom;

		nSumTop += nTop;
		nSumBottom += nBottom;
	}

	fMeanTop = (float)nSumTop / nObjCnt;
	fMeanBottom = (float)nSumBottom / nObjCnt;
	
	/* =============================================================
	분산, 표준 편차 
	================================================================*/
	for( i = 0 ; i < nObjCnt ; i++ )
	{
		ptrTempRect = (CVRECT_t *)&ptrRectList[ i ];
		
		nTop = ptrTempRect->nTop;
		nBottom = ptrTempRect->nBottom;

		fVarTop += abs( nTop - fMeanTop );
		fVarBottom += abs( nBottom - fMeanBottom );
	}

	fSigTop = fVarTop / nObjCnt;
	fSigBottom = fVarBottom / nObjCnt;

	nMinTop = (int)(fMeanTop - fSigTop);
	nMaxTop = (int)(fMeanTop + fSigTop);
	nMinBottom = (int)( fMeanBottom - fSigBottom );
	nMaxBottom = (int)( fMeanBottom + fSigBottom );

	if ( nMinTop < 0 ) nMinTop =0;
	if ( nMaxBottom > ptrOrgRect->nBottom ) nMaxBottom = ptrOrgRect->nBottom;

	/* =============================================================
	객체 크기 체크 
	================================================================*/
	for( i = 0 ; i < nObjCnt ; i++ )
	{
		ptrTempRect = (CVRECT_t *)&ptrRectList[ i ];
		
		nTop = ptrTempRect->nTop;
		nBottom = ptrTempRect->nBottom;
		nLeft = ptrTempRect->nLeft;
		nRight = ptrTempRect->nRight;

		nHeight = nBottom - nTop;
		nWidth = nRight - nLeft;
		
		bVerify = TRUE;
		/* 객체의 최대 가로 길이 */
		if( nMaxWidth < nWidth ) nMaxWidth = nWidth;

		/* 최소 픽셀 */
		if ( ( nWidth < ( ptrLPRConf->LPR_MIN_CHAR_W / 2 ) ) || ( nHeight < ptrLPRConf->LPR_MIN_CHAR_H / 2 ) )
		{
			bVerify = 0;
		}

		/* 객체의 세로 길이가 문자 최소 길이 보다 작을 경우  */
		if( bVerify && nHeight < ptrLPRConf->LPR_MIN_CHAR_H )
		{
			bVerify = 0;
		}
		
		/* 객체의 가로 길이가 문자 최소 길이 보다 작을 경우  */
		if( bVerify && nWidth < ptrLPRConf->LPR_MIN_CHAR_W )
		{
			bVerify = 0;
		}

		/* 세로 *2보다  가로길이가 길경우 */
		if ( bVerify && nHeight < nWidth )
		{
			bVerify = 0;
		}

		if ( bVerify && ( nTop < nMinTop || nTop > nMaxTop ) )
		{
			nDelTopCnt++;
		}

		if ( bVerify &&  ( nBottom < nMinBottom || nBottom > nMaxBottom) )
		{
			nDelBottomCnt++;
		}

		if( bVerify == 0 )
		{
			nDelCnt++;
			
		}
	}

	nVerifyCnt = nObjCnt - nDelCnt;	
	nSubSeparCnt = abs( nSeparCnt - nVerifyCnt );
	
	/* 객체 TOP 균등성 확인 */
	if ( nDelTopCnt > ptrLPRConf->LPR_MIN_CHAR_CNT ) ptrOrgRect->nSeqNum = -1;

	/* 객체 BOTTOM 균등성 확인 */
	if ( nDelBottomCnt > ptrLPRConf->LPR_MIN_CHAR_CNT ) ptrOrgRect->nSeqNum = -1;

	/* 적정 객체수 와 구분 영역 수 의 차이 확인 */
	if ( nSubSeparCnt > ptrLPRConf->LPR_MIN_CHAR_CNT ) ptrOrgRect->nSeqNum = -1;

	/* 무효 문자로 판단되는 개수 확인 */
	if ( nDelCnt > ( ptrLPRConf->LPR_MIN_CHAR_CNT *2 ) ) ptrOrgRect->nSeqNum = -1;

	/* 유효 문자의 개수 확인 */
	if ( nVerifyCnt < ptrLPRConf->LPR_MIN_CHAR_CNT || nVerifyCnt > ptrLPRConf->LPR_MAX_CHAR_CNT )  ptrOrgRect->nSeqNum = -1;
		
	return nVerifyCnt;
}

/*=========================================================================
번호판 내에 존재하는 각 객체의 크기를 산출
==========================================================================*/
static int MakeObjectSize( int nObjCnt, CVRECT_t * ptrRect, int ** ptrMap, pIMAGE_DATA ptrImageDataSrc , int nWStart, int nWEnd ,int nHStart, int nHEnd, int bOrder)
{
	CVRECT_t *ptrTempRect;
	CVRECT_t * ptrCurRect, * ptrNextRect, TmpRect;

	int i, j;
	int nLabel;	

	for( i = 0 ; i < nObjCnt ; i++ )
	{
		ptrRect[ i ].nLeft = nWEnd;
		ptrRect[ i ].nTop =  nHEnd;
		ptrRect[ i ].nRight = 0;
		ptrRect[ i ].nBottom = 0;
	}

	for( i = nHStart ; i <= nHEnd ; i++ )
	{
		for( j = nWStart; j <= nWEnd; j++ )
		{
			if ( i < ptrImageDataSrc->nH && j < ptrImageDataSrc->nW )
			{
				nLabel = ptrMap[ i ][ j ];

				if ( nLabel != 0  && nLabel <= nObjCnt )
				{
					ptrTempRect= (CVRECT_t *)&ptrRect[ nLabel -1 ];

					if ( i < ptrTempRect->nTop ) ptrTempRect->nTop = i  ;
					if ( i > ptrTempRect->nBottom ) ptrTempRect->nBottom = i ;

					if ( j < ptrTempRect->nLeft ) ptrTempRect->nLeft = j ;
					if ( j > ptrTempRect->nRight ) ptrTempRect->nRight = j;
				}
			}
		}
	}	

	if ( bOrder )
	{
		for (i = nObjCnt-1 ; i >= 0 ; --i) 
		{    
			for (j = 0; j < nObjCnt-1; ++j) 
			{ 
				ptrCurRect = (CVRECT_t *)&ptrRect[ j ];
				ptrNextRect = (CVRECT_t *)&ptrRect[ j +1 ];				
				
				if ( ptrCurRect->nLeft > ptrNextRect->nLeft ) 
				{    
					memset( &TmpRect, 0, sizeof( CVRECT_t));
					memcpy( &TmpRect, ptrCurRect, sizeof( CVRECT_t));

					memcpy( ptrCurRect, ptrNextRect, sizeof( CVRECT_t));
					memcpy( ptrNextRect, &TmpRect, sizeof( CVRECT_t));
				}
			}
		}
	}

	return 0;
}

static int Priority_Proc( pLPR_CONF ptrLPRConf, CVRECT_t * ptrOrgRect, CVRECT_t * ptrPrioRect,  
						  pIMAGE_DATA ptrCandidateImage, pIMAGE_DATA ptrImageDataSrc, int * ptrComWProjection, int * ptrComHProjection )
{
	
	int nTop, nLeft, nRight, nBottom, nWidth, nHeight, i, j , nSegPosH;
	int nPrjCnt, nMinWCnt;
	int nSeparCnt, nPos1, nPos2, nBLen, nSum;
	int nObjCnt, nVerifyCnt;

	BOOL bFlag ;
	pCVRECT ptrComObjectRect = NULL;
	int ** ptrComMap2 = NULL;
	pCOMMONINFO ptrComInfo = NULL;
	float fPlateRatio;

	( void )ptrImageDataSrc;	
	
	ptrComObjectRect	= COM_GetTempRectBuf();
	ptrComMap2			= COM_GetMap2Buf();
	ptrComInfo			= COM_GetInfo();	
	
	nVerifyCnt = 0;

	if( ptrComObjectRect == NULL || ptrComMap2 == NULL || ptrComInfo == NULL )
	{
		return ERR_NO_PARAM;
	}

	memset( ptrComWProjection, 0, sizeof( int ) * ptrComInfo->nImageWidth );
	memset( ptrComHProjection, 0, sizeof( int ) * ptrComInfo->nImageHeight );
	memset( ptrComObjectRect, 0, sizeof( CVRECT_t ) * MAX_LABEL_CNT );

	nPrjCnt = nSeparCnt = nPos1 = nPos2 = nBLen = nSum = nSegPosH = 0;
	
	nTop	= ptrPrioRect->nTop;
	nLeft	= ptrPrioRect->nLeft;
	nRight	= ptrPrioRect->nRight;
	nBottom = ptrPrioRect->nBottom;

	nHeight = nBottom - nTop;
	nWidth = nRight - nLeft;
	fPlateRatio   = HLP_Cal_ImageRatio( ptrPrioRect ) ;

	/*===========================================================================
	[ 번호판의 비율이 맞지 않을 경우  ]	
	============================================================================*/
	if ( fPlateRatio < ptrLPRConf->LPR_MIN_PLATE_RATIO || fPlateRatio > ptrLPRConf->LPR_MAX_PLATE_RATIO)
	{
		ptrOrgRect->nSeqNum = -1;
		return ERR_NO_PARAM;
	}
	/*===========================================================================
	[ 기본적인 번호판 최소 크기 체크 ]	
	============================================================================*/
	if ( nWidth < ptrLPRConf->LPR_MIN_PLT_W  || ( nHeight < ptrLPRConf->LPR_MIN_PLT_H ) )
	{
		ptrOrgRect->nSeqNum = -1;
		return ERR_NO_PARAM;
	}	

	nMinWCnt = ptrLPRConf->LPR_MIN_PIXEL;	
	nSegPosH = nTop;
	

	/*===========================================================================
	[객체의 분리를 개수 파악 ]
	4자리 숫자 영역의 분리 개수 파악 
	============================================================================*/
	nSegPosH = COM_GetSeparationH_MinCnt( ptrLPRConf, ptrComInfo, ptrPrioRect, ptrComHProjection, ptrCandidateImage);
	
	/* 객체 분리 영역 개수 파악 */
	bFlag = TRUE;
	nSeparCnt = nPos1 = nPos2 = nBLen = nSum = 0;
	
	for( j =nLeft ; j <= nRight ; j++ )
	{		
		if ( j < ptrCandidateImage->nW )
		{	
			*( ptrComWProjection + j ) =0;

			for( i = nSegPosH; i <=  nBottom ; i++ )
			{	
				if (i < ptrCandidateImage->nH && ( ptrCandidateImage->ptrSrcImage2D[ i ][ j ] == 255 )  )
					*( ptrComWProjection + j ) += 1;				
			}
		}
	}

	for( j = nLeft ; j <= nRight ; j++ )
	{
		if ( j < ptrCandidateImage->nW )
		{	
			nPrjCnt = *( ptrComWProjection + j );
			if ( nPrjCnt <= nMinWCnt && bFlag )
			{
				nSeparCnt++;
				bFlag = FALSE;
				nPos1 = j;
			}
			if ( nPrjCnt > nMinWCnt  && ( !bFlag ))
			{
				nPos2 = j;
				bFlag = TRUE;
				nBLen = ( nPos2 - nPos1 );
				nSum += nBLen;
			}
		}
	}

	/* 최소 객체 분리 영역 개수는 최소 문자 수 */
	if ( nSeparCnt < ( ptrLPRConf->LPR_MIN_CHAR_CNT -1 ) || nSeparCnt > ptrLPRConf->LPR_MAX_CHAR_CNT )
	{
		ptrOrgRect->nSeqNum = -1;
		return ERR_NO_PARAM;
	}

	/*===========================================================================
	[번호판 영역 내 객체 Labeling ]	
	상하 분리된 번호판은 하단 부분에 있는 객체만 체크 한다.
	특히 객체의 평균 top과 bottom을 체크 하기 때문에 하단 영역부분만 체크하는것이 필요하다.
	============================================================================*/
	nObjCnt =0;
	nObjCnt= COM_SubDoLabeling8Neighbour( ptrCandidateImage, ptrComMap2, nLeft, nRight, nSegPosH, nBottom );
		
	/*===========================================================================
	[번호판 영역 내 객체 크기 정보 산출  ]
	객체 수가 LPR_MIN_CHAR_CNT 보다 작거나 LPR_MAX_CHAR_CNT 보다 클 경우 제외 
	============================================================================*/
	MakeObjectSize( nObjCnt, ptrComObjectRect, ptrComMap2, ptrCandidateImage, nLeft, nRight, nSegPosH, nBottom, 0 );
	if ( nObjCnt < ptrLPRConf->LPR_MIN_CHAR_CNT || nObjCnt  > ptrLPRConf->LPR_MAX_CHAR_CNT )
	{
		ptrOrgRect->nSeqNum = -1;
		return ERR_NO_PARAM;
	}

	/*===========================================================================
	[번호판 영역 내 객체 크기 유효성 체크 ]
	============================================================================*/
	nVerifyCnt= CheckValidSize( ptrLPRConf, nObjCnt, nSegPosH, nSeparCnt, ptrOrgRect, ptrComObjectRect, ptrCandidateImage );	


	/*===========================================================================
	번호판의 비율이 구, 신 번호판에 점수를 더 준다. 	
	============================================================================*/	
	if ( ptrOrgRect->nSeqNum > 0  )
	{	
		if ( fPlateRatio > ptrLPRConf->LPR_NEW_PLATE_RATIO )
		{
			ptrOrgRect->nSeqNum++;
		}
	}

	return nVerifyCnt;
}

/* ===================================== Make Uniformity  =========================================================================================
===================================================================================================================================================
참조 논문 : 분할된 영역의 특성을 이용한 차량 번호판 포착( 한국 정보과학회논문지 )
디지털 신호 분석 기법을 이용한 다양한 번호판 추출 방법( 전자 공학회 논문지 )
U( m, G( maxpixel ) , s) = | m- G(maxpixel ) | + s
m : 평균 = ( 1/ K * L ) * sigma( G ( i, j )
s : 분산 = ( 1/ K * L ) * sigma( | G( i, j ) - m ) | )

T : 임계값 = ( 1/ N * M ) * sigma( U( m, G(maxpixel ), s )

K, L : 영역 크기
N, M : 영상 크기
G( i, j ) : 영상 밝기
G( maxpixel ) : 가장 많은 개수를 가진 밝기 값
==================================================================================================================================================*/

/*=========================================================================
유효하지 않는 불균등 영역은 삭제한다. 
0 : 불균등 영역 
x : 균등 영역 
1)        2)       3)       4)      5)
x x x     x x x    x 0 x    x x 0   0 x x
x 0 x     0 0 0    x 0 x    x 0 x   x 0 x 
x x x     x x x    x 0 x    0 x x   x x 0
==========================================================================*/
static int Del_InvalidUniformity( pLPR_CONF ptrLPRConf, UNIFORMITY_t **ptrUniformitys, int nUniforH, int nUniforW )
{
	int nRepeat = 1;
	int i, j;
	int nLoopCnt = 0;
	int nUnCnt = 0;
	int nMidFlag = 0;
	
	( void )ptrLPRConf;

	while( nRepeat > 0 )
	{
		nRepeat = 0;

		if( nLoopCnt++ > 50 )
		{
			HLP_print_dbg(DBG_INFO, "1Del_InvalidUniformity LOOPCNT :%d", nLoopCnt );
			break;
		}		
		
		for( i = 1; i < nUniforH -1 ; i++ )
		{
			for( j = 1 ; j < nUniforW -1; j++ )
			{
				/* 1 Mask */
				if (( ptrUniformitys[ i ][ j ].nUniform == 1  ) &&

					( ptrUniformitys[ i-1 ][ j-1 ].nUniform == 0 ) && 
					( ptrUniformitys[ i   ][ j-1 ].nUniform == 0 ) && 
					( ptrUniformitys[ i+1 ][ j-1 ].nUniform == 0 ) && 

					( ptrUniformitys[ i-1 ][ j ].nUniform == 0 ) && 					 
					( ptrUniformitys[ i+1 ][ j ].nUniform == 0 ) && 

					( ptrUniformitys[ i-1 ][ j+1 ].nUniform == 0 ) && 
					( ptrUniformitys[ i   ][ j+1 ].nUniform == 0 ) && 
					( ptrUniformitys[ i+1 ][ j+1 ].nUniform == 0 ) )
				{					
					ptrUniformitys[ i ][ j ].nUniform = 0;
					nRepeat = 1;
				}

				/* 2 Mask */
				if ( nRepeat != 1 && ( ptrUniformitys[ i-1 ][ j ].nUniform == 1  ) &&
					( ptrUniformitys[ i  ][ j ].nUniform == 1  ) &&
					( ptrUniformitys[ i+1 ][ j ].nUniform == 1  ) &&

					( ptrUniformitys[ i-1 ][ j-1 ].nUniform == 0 ) && 
					( ptrUniformitys[ i   ][ j-1 ].nUniform == 0 ) && 
					( ptrUniformitys[ i+1 ][ j-1 ].nUniform == 0 ) && 
					
					( ptrUniformitys[ i-1 ][ j+1 ].nUniform == 0 ) && 
					( ptrUniformitys[ i   ][ j+1 ].nUniform == 0 ) && 
					( ptrUniformitys[ i+1 ][ j+1 ].nUniform == 0 ) )
				{					
					ptrUniformitys[ i-1 ][ j ].nUniform = 0;
					ptrUniformitys[ i   ][ j ].nUniform = 0;
					ptrUniformitys[ i+1 ][ j ].nUniform = 0;

					nRepeat = 1;
				}

				/* 3 Mask */
				if (nRepeat != 1 &&  ( ptrUniformitys[ i ][j-1 ].nUniform == 1  ) &&
					( ptrUniformitys[ i ][j ].nUniform == 1  ) &&
					( ptrUniformitys[ i ][j +1].nUniform == 1  ) &&

					( ptrUniformitys[ i-1 ][ j-1 ].nUniform == 0 ) && 
					( ptrUniformitys[ i-1 ][ j ].nUniform == 0 ) && 
					( ptrUniformitys[ i-1 ][ j+1 ].nUniform == 0 ) && 

					( ptrUniformitys[ i+1 ][ j-1 ].nUniform == 0 ) && 
					( ptrUniformitys[ i+1 ][ j ].nUniform == 0 ) && 
					( ptrUniformitys[ i+1 ][ j+1 ].nUniform == 0 ) )
				{					
					ptrUniformitys[ i ][j-1].nUniform = 0;
					ptrUniformitys[ i ][ j ].nUniform = 0;
					ptrUniformitys[ i ][j+1].nUniform = 0;

					nRepeat = 1;
				}

				/* 4 Mask */
				if (nRepeat != 1 &&  ( ptrUniformitys[ i+1][j-1 ].nUniform == 1  ) &&
					( ptrUniformitys[ i ][ j ].nUniform == 1  ) &&
					( ptrUniformitys[ i-1 ][j +1].nUniform == 1  ) &&

					( ptrUniformitys[ i-1 ][ j-1 ].nUniform == 0 ) && 
					( ptrUniformitys[ i  ][ j -1].nUniform == 0 ) && 
					
					( ptrUniformitys[ i-1 ][ j ].nUniform == 0 ) && 
					( ptrUniformitys[ i+1 ][ j ].nUniform == 0 ) && 

					( ptrUniformitys[ i ][ j +1].nUniform == 0 ) && 
					( ptrUniformitys[ i+1 ][ j+1 ].nUniform == 0 ) )
				{					
					ptrUniformitys[ i+1][j-1 ].nUniform = 0;
					ptrUniformitys[ i ][ j ].nUniform = 0;
					ptrUniformitys[ i-1 ][j +1].nUniform = 0;

					nRepeat = 1;
				}

				/* 5 Mask */
				if ( nRepeat != 1 &&  ( ptrUniformitys[ i-1][j-1 ].nUniform == 1  ) &&
					( ptrUniformitys[ i ][ j ].nUniform == 1  ) &&
					( ptrUniformitys[ i+1 ][j +1].nUniform == 1  ) &&

					( ptrUniformitys[ i ][ j-1 ].nUniform == 0 ) && 
					( ptrUniformitys[ i+1 ][ j -1].nUniform == 0 ) && 

					( ptrUniformitys[ i-1 ][ j ].nUniform == 0 ) && 
					( ptrUniformitys[ i+1 ][ j ].nUniform == 0 ) && 

					( ptrUniformitys[ i-1][ j +1].nUniform == 0 ) && 
					( ptrUniformitys[ i ][ j+1 ].nUniform == 0 ) )
				{					
					ptrUniformitys[ i-1][j-1 ].nUniform = 0;
					ptrUniformitys[ i ][ j ].nUniform = 0;
					ptrUniformitys[ i+1 ][j +1].nUniform = 0;

					nRepeat = 1;
				}			

			}

		}

	}

	nRepeat =1;
	nLoopCnt = 0;

	while( nRepeat > 0 )
	{
		nRepeat = 0;

		if( nLoopCnt++ > 10 )
		{
			HLP_print_dbg(DBG_INFO, "2Del_InvalidUniformity LOOPCNT :%d", nLoopCnt );
			break;
		}

		for( j = 1; j < nUniforW -1 ; j++ )
		{
			
			for( i = 1 ; i < nUniforH -1; i++ )
			{
				/* 6 Mask */				
				nMidFlag = 0;
				nUnCnt = 0;
				if ( ptrUniformitys[ i ][ j ].nUniform == 1  )
				{					
					nUnCnt++;

					if ( i-2 >= 0 )
					{
						nMidFlag +=1;
						if ( ptrUniformitys[ i-2][j ].nUniform == 1 ) nUnCnt++;
					}

					if ( ptrUniformitys[ i-1][j ].nUniform == 1 ) nUnCnt++;
					if ( ptrUniformitys[ i+1][j ].nUniform == 1 ) nUnCnt++;

					if ( i+2 <= nUniforH -1 )
					{
						nMidFlag +=1;
						if ( ptrUniformitys[ i+2][j ].nUniform == 1 ) nUnCnt++;
					}

					if (  nMidFlag < 2 && nUnCnt <  2 /* (ptrLPRConf->LPR_UF_H -1) */ )
					{
						ptrUniformitys[ i ][ j ].nUniform = 0;
						nRepeat = 1;
					}
					else if ( nMidFlag >= 2 && nUnCnt < 3 /* ptrLPRConf->LPR_UF_H */)
					{
						ptrUniformitys[ i ][ j ].nUniform = 0;
						nRepeat = 1;
					}

					//int k = ptrUniformitys[ i ][ j ].nUniform ;
				}
				
			}

		}
	}

	return ERR_SUCCESS;
}


/*=========================================================================
불균등 영역 설정 
==========================================================================*/
static int MakeUniformityArea( pLPR_CONF ptrLPRConf, pDIP_DATA ptrDIPData, UNIFORMITY_t **ptrUniformitys, 
							   int nUniforH, int nUniforW , pIMAGE_DATA ptrImageDataSrc )
{
	int i, j, k, q, z,x;
	int nVal, nSum1;
	float fSum2;
	float fMean, fDif ,fVar, fU, fTh,fTotalUni, fSig;		
	
	int nValCnt;
	int nHSize = 0;
	int nWSize = 0;
	
	( void )ptrDIPData;

	for(i = 0 ; i < nUniforH ; i++)
	{
		memset( ptrUniformitys[ i ] , 0, sizeof( UNIFORMITY_t) * nUniforW );
	}

	z =0;
	fTotalUni = 0;
	x =0;
	
	
	nHSize = ptrLPRConf->LPR_UF_H;
	nWSize = ptrLPRConf->LPR_UF_W;
	/*===========================================================================
	[ 이미지의 모든 가로를 처리하는것이 아니라 일정 간격으로 두고 처리한다. 
	  차량 영상의 이웃은 서로 비슷한 속성을 가지고 있으며 이를 통해 처리속도에 이점이 있다. ]	
	============================================================================*/
	for( i = 0 ; i < ptrImageDataSrc->nH -1 ; i += (nHSize) )
	{
		fMean= 0;
		fDif = 0;
		fVar = 0;
		fSig = 0;

		q =0;
		x =0;		

		for( j = 1 ; j < ptrImageDataSrc->nW -1; j += nWSize)
		{	
			if ( j >= ptrImageDataSrc->nW -1 )
			{
				continue;;
			}
			nSum1 = 0;
			fSum2 = 0;
			
			/*===========================================================================
			 [ Uniform 단위 평균  ]
			============================================================================*/
			nValCnt = 0;
			q = i;
			//for( q = i ; q < ( i + nHSize -1) ; q++  )
			{			
				for( k = j; k < ( j + nWSize )  ; k++  )
				{	
					if( q < ptrImageDataSrc->nH -1 && k < ptrImageDataSrc->nW -1 )
					{
						nVal = ptrImageDataSrc->ptrSrcImage2D[ i][ k ] ;

						nSum1 += nVal;
						nValCnt++;
					}
				}
			}
			if ( nValCnt > 0 )
			{
				fMean = (float)( nSum1 / ( nValCnt   ) );	/* 평균 */
			}
			
			/*===========================================================================
			 [ Uniform 단위 표준 편차  ]
			============================================================================*/
			nValCnt = 0;
			q = i;
			//for( q = i ; q < ( i + nHSize -1) ; q++  )
			{			
				for( k = j; k < ( j + nWSize )  ; k++  )
				{
					if( k < ptrImageDataSrc->nW -1 )
					{
						nVal = ptrImageDataSrc->ptrSrcImage2D[ i ][ k ] ;


						fDif = abs( nVal - fMean ) ;	
						fSum2 += ( fDif * fDif ) ;				
						//fSum2 += fDif;
						nValCnt++;
					}				
				}
			}
			if ( nValCnt > 0 )
			{
				fVar = (float)( fSum2 / nValCnt );			/* 분산		*/
				fSig = sqrt( fVar )	;						/* 표준편차 */
				//fU = ( abs( fMean - nMaxVal ) + fVar ) ;	/* 균일성	*/
				//fU = ( abs( fMean - nMaxVal ) + fSig ) ;	/* 균일성	*/
				fU =0 ;	/* 균일성	*/
				/*===========================================================================
				 [ Uniform 단위 정보  ]
				============================================================================*/
				fTotalUni += fSig;				
				ptrUniformitys[ z ][ x ].fU = fU;				/* 균일성	*/
				ptrUniformitys[ z ][ x ].nSum = nSum1;		/* 합		*/
				ptrUniformitys[ z ][ x ].nCnt = nValCnt;		/* 합		*/				
				
				ptrUniformitys[ z ][ x ].fMean = fMean;		/* 평균		*/
				ptrUniformitys[ z ][ x ].fVar = fVar;			/* 분산		*/
				ptrUniformitys[ z ][ x ].fSig = fSig;			/* 표준편차 */
				ptrUniformitys[ z ][ x ].nX = j;
				ptrUniformitys[ z ][ x ].nY = i;
				x++;
			}
		}
		z++;
	}

	/*===========================================================================
	 [ 균일성 설정 ]
	 fTotalUni가 높을수록 영상 전체가 불규칙하게 이루어져 있음  

	 ※전체 영상의 불균등이 높을수록 번호판 영역의 불균등은 높음 
	 Uniformy Width에 따라 nA 값을 변경해준다. 

	 평균 fTotalUni은 약 15 ~ 30 사이의 값이 나오나 영상에 따라 다르게 나올수 있음. 
	============================================================================*/
	fTh = ( fTotalUni / ( x * z ) );
		
	float nA ;
	
	if ( fTh < 19 )			/* 불균등이 낮을 경우 */
		nA = 2.0;
	else if ( fTh < 23 )	/* 불균등이 중간 경우 */
		nA = 2.5;
	else					/* 불균등이 높을 경우 */
		nA = 3.0;
	
	m_fNewTh = fTh * nA;
	
	if( ptrLPRConf->LPR_DBG_SHOW_LOG )
	{
		HLP_print_dbg(DBG_INFO, "fTh : %f, nA : %f, Threshold : %f  =============", fTh ,nA,m_fNewTh );
	}

	for( i = 0; i < nUniforH ; i++ )
	{
		for( j = 0 ; j < nUniforW ;j++ )
		{			
			if ( ptrUniformitys[ i][ j ].fSig >= m_fNewTh  )
			{
				ptrUniformitys[ i][ j ].nUniform = 1;
			}
		}
	}

	return ERR_SUCCESS;
}

static int inter_DrawlineRect( int nLabelCnt, CVRECT_t * ptrRect , pIMAGE_DATA ptrImageDataSrc)
{
	CVRECT_t *ptrValidRect;
	int i;
	BYTE lineColor = 255;

	for ( i = 0 ; i < nLabelCnt ; i++ )
	{		
		ptrValidRect = NULL;
		ptrValidRect= (CVRECT_t *)&ptrRect[ i ];
		
		if ( ptrValidRect->nSeqNum > 0 )
		{			
			lineColor = 255;
			
		}
		else
		{		
			lineColor = 0;
		}

		HLP_DrawLineRect( ptrImageDataSrc, ptrValidRect, lineColor );
	}

	return ERR_SUCCESS;
};

/*=========================================================================
번호판 영역 유효성 체크는 
처음은 모두 체크하고 두번째부터는 기존에 유효하다고 판단한것을 다시 체크한다.
==========================================================================*/

/*======================================================================
추출된 객체의 기본 크기 체크 
========================================================================*/
static BOOL IsValidPlateRect( pLPR_CONF ptrLPRConf,CVRECT_t * ptrRect , pIMAGE_DATA ptrImageDataSrc, int nRectType )
{
	int nTop, nBottom, nLeft, nRight;
	int nWidth, nHeight;
	int nMinWidth, nMaxWidth, nMinHeight, nMaxHeight;
	float fPlateRatio=0;

	( void )ptrImageDataSrc;

	nTop = nBottom = nLeft = nRight = nWidth = nHeight =0;
	nMinWidth = nMaxWidth = nMinHeight = nMaxHeight =0;

	nTop = ptrRect->nTop;
	nBottom = ptrRect->nBottom;
	nLeft = ptrRect->nLeft;
	nRight = ptrRect->nRight;

	nWidth = nRight - nLeft;
	nHeight = nBottom - nTop;

	nMaxWidth = ptrImageDataSrc->nW/2 ;	
	nMaxHeight = ptrImageDataSrc->nH/2 ;

	/*번호판 영역의 크기의 유효성은 단계에서 따라 다르게 책정된다 */
	if ( nRectType == BIG4_CHAR_RECT )
	{
		nMinWidth  = ptrLPRConf->LPR_MIN_BIG4_W;
		nMinHeight = ptrLPRConf->LPR_MIN_BIG4_H;
		//nMinWidth  = ptrLPRConf->LPR_MIN_PLT_W;		
		//nMinHeight = ptrLPRConf->LPR_MIN_PLT_H;
	}
	else if ( nRectType == LOOSE_PLATE_RECT )
	{
		/* 전체 이미지에서 번호판 후보지 선택시 */
		nMinWidth  = ptrLPRConf->LPR_MIN_PLT_W;		
		nMinHeight = ptrLPRConf->LPR_MIN_PLT_H;
	}
	else
	{
		/* DETAIL_PLATE_RECT */
		/* 번호판 후보지 영역내의 객체( 문자 판단 시 ) */
		nMinWidth =  ptrLPRConf->LPR_MIN_PLT_W  / 2 ;
		nMinHeight = ptrLPRConf->LPR_MIN_PLT_H / 2 ;
	}

	if ( (nWidth < nMinWidth  || nWidth > nMaxWidth ) || (  nHeight < nMinHeight || nHeight > nMaxHeight ) )
	{
		return FALSE;
	}

	/* 번호판 가로 세로의 비율 */
	fPlateRatio = HLP_Cal_ImageRatio( ptrRect );	

	if ( fPlateRatio < ptrLPRConf->LPR_MIN_PLATE_RATIO || fPlateRatio > ptrLPRConf->LPR_MAX_PLATE_RATIO)
	{
		return FALSE;
	}

	return TRUE;
}


static int inter_IsvalidPlateRect( int bFirst, pLPR_CONF ptrLPRConf, int nLabelCnt, 
								   CVRECT_t * ptrRect , pIMAGE_DATA ptrImageDataSrc, int nRectType )
{
	CVRECT_t *ptrValidRect;
	int i;
	int nValidCnt = 0;
	int bCheck = 0;

	for ( i = 0 ; i < nLabelCnt ; i++ )
	{		
		ptrValidRect = NULL;
		ptrValidRect= (CVRECT_t *)&ptrRect[ i ];
		bCheck = 0;

		if ( ptrValidRect != NULL )
		{
			if ( bFirst == 1 )
			{
				bCheck = 1;
			}
			else
			{
				if( ptrValidRect->nSeqNum > 0 ) bCheck = 1;
			}

			if ( bCheck == 1 && IsValidPlateRect( ptrLPRConf, ptrValidRect, ptrImageDataSrc , nRectType ) == TRUE )
			{
				ptrValidRect->nSeqNum++;
				nValidCnt++;
			}
			else
			{
				ptrValidRect->nSeqNum = 0;
			}
		}
	}
	
	return nValidCnt;
}

/*=========================================================================
번호판 1차 후보지 생성 
불균등 영역이 가지고 있는 동일 Label을 기준으로 번호판 후보 영역을 산출하고
번호판 후보 영역중에서 유효성 체크하여 유효한 영역만 가져 온다. 
영역내에 존재하는 다른 객체는 모두 귀속 시킨다. 
==========================================================================*/
static int GeneratePlateRectWithUniformify( pLPR_CONF ptrLPRConf, int nLabelCnt, CVRECT_t * ptrRect, 
									    UNIFORMITY_t ** ptrUniformitys, int nUniforH, int nUniforW , pIMAGE_DATA ptrImageDataSrc )
{
	CVRECT_t *ptrValidRect, *ptrTempRect;
	int i, j;
	int nLabel;
	UNIFORMITY_t TempUnifor;
	CVRECT_t *ptrRect1 , *ptrRect2;

	int nLeft1, nRight1, nTop1, nBottom1;
	int nLeft2, nRight2, nTop2, nBottom2;
	int x,y;

	ptrValidRect = NULL;

	for( i = 0 ; i < nLabelCnt ; i++ )
	{
		ptrRect[ i ].nLeft = ptrImageDataSrc->nW;
		ptrRect[ i ].nTop =  ptrImageDataSrc->nH;
		ptrRect[ i ].nRight = 0;
		ptrRect[ i ].nBottom = 0;
	}

	/*===========================================================================
	 [ Uniform 설정된 동일 Label에 대한 Rect를 계산한다]
	============================================================================*/
	for( i = 0 ; i < nUniforH ; i++ )
	{
		for( j = 0; j < nUniforW; j++ )
		{
			TempUnifor	= ptrUniformitys[ i ][ j ];
			nLabel		= TempUnifor.nLabelNum;

			if (  TempUnifor.nUniform == 1 && nLabel != 0  && nLabel <= nLabelCnt )
			{
				/*===========================================================================
				[ Uniform 이 가지고 있는 동일 Label의 Rect를 갱신( 확대 ) ]
				============================================================================*/
				ptrTempRect= (CVRECT_t *)&ptrRect[ nLabel -1 ];

				if ( TempUnifor.nY < ptrTempRect->nTop ) ptrTempRect->nTop = TempUnifor.nY  ;
				if ( TempUnifor.nY + ptrLPRConf->LPR_UF_H > ptrTempRect->nBottom ) ptrTempRect->nBottom = TempUnifor.nY+ ptrLPRConf->LPR_UF_H ;

				if ( TempUnifor.nX < ptrTempRect->nLeft ) ptrTempRect->nLeft = TempUnifor.nX ;
				if ( TempUnifor.nX + ptrLPRConf->LPR_UF_W > ptrTempRect->nRight ) ptrTempRect->nRight = TempUnifor.nX + ptrLPRConf->LPR_UF_W;

				if( ptrTempRect->nTop < 0 ) ptrTempRect->nTop = 0;
				if( ptrTempRect->nLeft < 0 ) ptrTempRect->nLeft = 0;
				
				if( ptrTempRect->nBottom > ptrImageDataSrc->nH ) ptrTempRect->nBottom = ptrImageDataSrc->nH;
				if( ptrTempRect->nRight > ptrImageDataSrc->nW ) ptrTempRect->nRight = ptrImageDataSrc->nW;

			}
		}
	}
	
	/*===========================================================================
	 [ 영역의 유효성 체크 ]
	 [ 가로는 Uniform 길이 보다 커야 하고 세로는 최소 문자 길이 보다 커야 한다.  ]
	============================================================================*/
	inter_IsvalidPlateRect( 1, ptrLPRConf, nLabelCnt, ptrRect, ptrImageDataSrc, BIG4_CHAR_RECT);
		
	/*===========================================================================
	 [ 자신에 포함된 객체는 모두 귀속한다. ]	 
	============================================================================*/
	for ( i = 0 ; i < nLabelCnt ; i++ )
	{		
		ptrRect1 = NULL;
		ptrRect1= (CVRECT_t *)&ptrRect[ i ];

		if ( ptrRect1->nSeqNum > 0 )
		{
			nLeft1	= ptrRect1->nLeft;
			nRight1 = ptrRect1->nRight;
			nTop1	= ptrRect1->nTop;
			nBottom1= ptrRect1->nBottom;


			for( j = 0 ; j < nLabelCnt ; j++  )
			{
				ptrRect2 = NULL;
				ptrRect2= (CVRECT_t *)&ptrRect[ j ];

				if ( ( ptrRect2->nSeqNum > 0 ) && ( i != j ) )				
				{
					nLeft2	= ptrRect2->nLeft;
					nRight2 = ptrRect2->nRight;
					nTop2	= ptrRect2->nTop;
					nBottom2= ptrRect2->nBottom;

					if( ( nLeft1 <= nLeft2 ) &&
						( nRight1 >= nRight2 ) &&
						( nTop1 <= nTop2 ) &&
						( nBottom1 >= nBottom2 ) )
					{
						ptrRect2->nSeqNum =-2;
					}
				}
			}
		}
	}

	/*===========================================================================	 
	 객체가 있는 첫번째 위치로 [ TOP, BOOTOM, LEFT, RIGHT 를 재 설정한다. ]
	============================================================================*/	
	int bStop = 0;
	int nObjCnt = 0;
	int nMinWCnt = ptrLPRConf->LPR_MIN_PIXEL;
	for ( i = 0 ; i < nLabelCnt ; i++ )
	{		
		ptrRect1 = NULL;
		ptrRect1 = (CVRECT_t *)&ptrRect[ i ];

		if ( ptrRect1->nSeqNum > 0 )
		{
			nLeft1	= ptrRect1->nLeft;
			nRight1 = ptrRect1->nRight;
			nTop1	= ptrRect1->nTop;
			nBottom1= ptrRect1->nBottom;

			bStop = 0;
			for( y = nTop1 ; y < nBottom1 ; y++ )
			{
				if ( bStop ) break;				
				nObjCnt = 0;

				for( x = nLeft1; x < nRight1 ; x++)
				{
					if( ptrImageDataSrc->ptrSrcImage2D[ y ][ x ] != 0 ) 
					{
						nObjCnt++;
						if ( nObjCnt >= nMinWCnt )
						{
							ptrRect1->nTop = y;						
							bStop =1;
							break;
						}
					}					
				}				
			}

			bStop = 0;
			for( y = nBottom1 -1 ; y > nTop1 ; y-- )
			{
				if ( bStop ) break;
				nObjCnt = 0;

				for( x = nLeft1; x < nRight1 ; x++)
				{	
					if( ptrImageDataSrc->ptrSrcImage2D[ y ][ x ] != 0 ) 
					{
						nObjCnt++;
						if ( nObjCnt >= nMinWCnt )
						{
							ptrRect1->nBottom = y;						
							bStop =1;
							break;
						}
					}					
				}				
			}

			bStop = 0;
			for( x = nLeft1 ; x < nRight1 ; x++ )
			{
				if ( bStop ) break;
				nObjCnt = 0;

				for( y = nTop1; y < nBottom1 ; y++)
				{
					if( ptrImageDataSrc->ptrSrcImage2D[ y ][ x ] != 0 ) 
					{
						nObjCnt++;
						if ( nObjCnt >= nMinWCnt )
						{
							ptrRect1->nLeft = x;						
							bStop =1;
							break;
						}
					}					
				}				
			}

			bStop = 0;
			for( x = nRight1 -1; x > nLeft1 ; x-- )
			{
				if ( bStop ) break;
				nObjCnt = 0;

				for( y = nTop1; y < nBottom1 ; y++)
				{
					
					if( ptrImageDataSrc->ptrSrcImage2D[ y ][ x ] != 0 ) 
					{
						nObjCnt++;
						if ( nObjCnt >= nMinWCnt )
						{
							ptrRect1->nRight = x;						
							bStop =1;
							break;
						}

						
					}					
				}				
			}

		}
	}
	
	/*===========================================================================
	 [ 영역의 유효성 체크 ]
	 [ 가로는 Uniform 길이 보다 커야 하고 세로는 최소 문자 길이 보다 커야 한다.  ]
	============================================================================*/
	inter_IsvalidPlateRect( 1, ptrLPRConf, nLabelCnt, ptrRect, ptrImageDataSrc, BIG4_CHAR_RECT);
	return nLabelCnt;
}
/*======================================================================
번호판 후보영역의 병합
영상이 클 경우 번호판의 상하 , 좌우가 떨어져 처리되는 경우가 존재 함 
상하 병합, 좌우 병합
========================================================================*/
static int MergePlateRect(pLPR_CONF ptrLPRConf, int nLabelCnt, CVRECT_t * ptrRect , pIMAGE_DATA ptrImageDataSrc )
{
	int i, j ;	

	CVRECT_t *ptrRect1 , *ptrRect2;

	int nLeft1, nRight1, nTop1, nBottom1, nMidW1, nMidH1;
	int nLeft2, nRight2, nTop2, nBottom2, nMidW2, nMidH2;
	int nValidCnt = 0;

	int nMaxTop;
	int nMaxBottom;
#if 0
	/*===========================================================================
	[ 상하 병합 ]
	1. 밑에 있는 객체의 UF_H 이내에 존재하고    
	2. 밑에 있는 객체의 가로 세로비율이   5.0 을 넘지 말아야 하고( 구 번호판 )
    3. 위에 있는 객체의 길이가  밑의 객체 길이의 반 보다 작아야 한다.  ( 구 번호판 )
	4. 위에 있는 객체가 아래의 객체 범위에서 벗어나지 말아야 한다. 
	============================================================================*/
	for ( i = 0 ; i < nLabelCnt ; i++ )
	{		
		ptrRect1 = NULL;
		ptrRect1= (CVRECT_t *)&ptrRect[ i ];

		/* 아래 객체( 기준 ) */
		if ( ptrRect1->nSeqNum > 0 )
		{
			nLeft1	= ptrRect1->nLeft;
			nRight1 = ptrRect1->nRight;
			nTop1	= ptrRect1->nTop;
			nBottom1= ptrRect1->nBottom;
			nSizeH1	= nBottom1 - nTop1;
			fRatio1  = ( nRight1 - nLeft1 ) / nSizeH1;
			nMidW1 = ( nRight1 - nLeft1 )/2 ;

			nMaxLeft = nLeft1 - ptrLPRConf->LPR_UF_W;
			nMaxRight = nRight1 + ptrLPRConf->LPR_UF_W;
			

			if( nMaxLeft < 0 ) nMaxLeft = 0;
			if( nMaxRight > ptrImageDataSrc->nW ) nMaxRight = ptrImageDataSrc->nW;

			for( j = 0 ; j < nLabelCnt ; j++  )
			{
				ptrRect2 = NULL;
				ptrRect2= (CVRECT_t *)&ptrRect[ j ];

				/* 위 객체 */
				if ( ( ptrRect2->nSeqNum > 0 ) && ( i != j ) )
				{
					nLeft2	= ptrRect2->nLeft;
					nRight2 = ptrRect2->nRight;
					nTop2	= ptrRect2->nTop;
					nBottom2= ptrRect2->nBottom;					
					nSizeH2	= nBottom2 - nTop2;
					nMidW2 = ( nRight2 - nLeft2 )/2 ;

					if( //( nBottom2 < nTop1 ) && 
						( abs( nTop1 - nBottom2) <= ptrLPRConf->LPR_UF_H ) &&
						( fRatio1 <= ptrLPRConf->LPR_NEW_PLATE_RATIO ) &&						
						( nSizeH1 > nSizeH2 ) )
					{
						/* 상하 범위가 비슷할때 */ 
						if( ( nLeft2  > nMaxLeft && nRight2 < nMaxRight) ||
							( ( nLeft2 >= nLeft1 && nRight2 <= nRight1 ) &&
							( abs( nMidW2  - nMidW1) <= ptrLPRConf->LPR_UF_W) ) )
						{
							/* Rect Merge */
							ptrRect1->nTop = ptrRect2->nTop;

							if( ptrRect1->nLeft > ptrRect2->nLeft ) ptrRect1->nLeft = ptrRect2->nLeft;
							if( ptrRect1->nRight < ptrRect2->nRight ) ptrRect1->nRight = ptrRect2->nRight;
							
							ptrRect2->nSeqNum = -2;
						}
					}
				}
			}
		}
	}	
#endif
	/*===========================================================================
	[ 좌우 병합 ]
	1. 우 객체가 UF_W 이내에 존재해야 하고   
	2. 좌 객체의 RIGHT_TOP과 우 객체의 LEFT_TOP의 차이가 문자 최소 길이 사이즈 이내어야 한다. 

	============================================================================*/
	for ( i = 0 ; i < nLabelCnt ; i++ )
	{		
		ptrRect1 = NULL;
		ptrRect1= (CVRECT_t *)&ptrRect[ i ];

		/* 왼쪽 객체( 기준 ) */
		if ( ptrRect1->nSeqNum > 0 )
		{
			nLeft1	= ptrRect1->nLeft;
			nRight1 = ptrRect1->nRight;
			nTop1	= ptrRect1->nTop;
			nBottom1= ptrRect1->nBottom;

			nMidW1 =  ( nLeft1+ nRight1 ) /2;
			nMidH1 = ( nTop1 + nBottom1 ) /2;

			nMaxTop = nTop1 - ptrLPRConf->LPR_UF_H;
			nMaxBottom = nBottom1 + ptrLPRConf->LPR_UF_H;

			if( nMaxTop < 0 ) nMaxTop = 0;
			if( nMaxBottom > ptrImageDataSrc->nH ) nMaxBottom = ptrImageDataSrc->nH;

			for( j = 0 ; j < nLabelCnt ; j++  )
			{
				ptrRect2 = NULL;
				ptrRect2= (CVRECT_t *)&ptrRect[ j ];

				/* 오른쪽 객체 */
				if ( ( ptrRect2->nSeqNum > 0 ) && ( i != j ) )
				{
					nLeft2	= ptrRect2->nLeft;
					nRight2 = ptrRect2->nRight;
					nTop2	= ptrRect2->nTop;
					nBottom2= ptrRect2->nBottom;
					nMidW2  = ( nLeft2 + nRight2 ) /2;
					nMidH2  = ( nBottom2 + nTop2 )/2;

					if( ( nRight1 <= nLeft2 ) && 
						( ( nLeft2 - nRight1) < ( ptrLPRConf->LPR_UF_W * 2 )) )
					{
						/* 좌우 범위가 비슷할때 */ 
						if( nTop1  <= nMidH2 && nBottom1 >= nMidH2 )
						{
							/* Rect Merge */
							ptrRect1->nRight = ptrRect2->nRight;

							if( ptrRect1->nTop > ptrRect2->nTop ) ptrRect1->nTop = ptrRect2->nTop;
							if( ptrRect1->nBottom < ptrRect2->nBottom ) ptrRect1->nBottom = ptrRect2->nBottom;
							
							ptrRect2->nSeqNum = -2;
						}
					}
					else 
					{
						if( ( ( nTop1 <= nMidH2 && nTop1 >= nTop2 ) && ( nRight1 >= nMidW2  && nRight1 <= nRight2 )) || 
							( ( nBottom1 >= nMidH2 && nBottom1 <= nBottom2 ) && ( nRight1 >= nMidW2  && nRight1 <= nRight2 )) || 
							( ( nTop2 <= nMidH1 && nTop2 >= nTop1 ) && ( nLeft2 <= nMidW1 && nLeft2 >= nLeft1 )) ||
							( ( nBottom2 >= nMidH1 && nBottom2 <= nBottom1 ) && ( nLeft2 <= nMidW1 && nLeft2 >= nLeft1 )) )
						{
							/* Rect Merge */
							if ( ptrRect1->nTop > ptrRect2->nTop ) ptrRect1->nTop = ptrRect2->nTop;							
							if ( ptrRect1->nBottom < ptrRect2->nBottom ) ptrRect1->nBottom = ptrRect2->nBottom;

							if ( ptrRect1->nLeft > ptrRect2->nLeft ) ptrRect1->nLeft = ptrRect2->nLeft;
							if ( ptrRect1->nRight < ptrRect2->nRight ) ptrRect1->nRight = ptrRect2->nRight;

							ptrRect2->nSeqNum = -2;
						}
					}
				}
			}
		}
	}

	/*===========================================================================
	 [ 영역의 유효성 체크 ]
	 [ 가로는 Uniform 길이 보다 커야 하고 세로는 최소 문자 길이 보다 커야 한다.  ]
	============================================================================*/
	nValidCnt = inter_IsvalidPlateRect( 0, ptrLPRConf, nLabelCnt, ptrRect, ptrImageDataSrc, BIG4_CHAR_RECT);

	return nValidCnt;
}

/*======================================================================
번호판 후보영역의 확장
일반적으로 번호판의 테두리와 차량 번호의 경계가 모호 할경우를 대비한것으로
번호판 후보영역의 확장은 2픽셀단위로 비슷한 밝기일 경우에만 확장한다.
========================================================================*/
static int ExtensionPlateRect( pLPR_CONF ptrLPRConf,int nLabelCnt, CVRECT_t * ptrRectOrg , pIMAGE_DATA ptrImageDataSrc )
{
	int q, i, j;
	int nTop, nLeft, nRight, nBottom, nWith, nHalfW, nExtTop, nExceptLine;
	int nUFW_L, nUFW_R;

	int nSum, nObjCnt, nCnt ;
	int nStop;
	int nVal;
	int nExcepSize;
	int nChgTop, nNotSigCnt;
	float fMean, fVar, fSig, fSum2, fDif, fExtTop,fExceptLine ;

	CVRECT_t *ptrRect;
	

	for ( q = 0 ; q < nLabelCnt ; q++ )
	{		
		ptrRect = NULL;
		ptrRect= (CVRECT_t *)&ptrRectOrg[ q ];
	
		if( ptrRect!= NULL && ptrRect->nSeqNum > 0 )
		{
			nTop		= ptrRect->nTop;
			nBottom		= ptrRect->nBottom;
			nLeft		= ptrRect->nLeft;
			nRight		= ptrRect->nRight;
			
			nWith		=( nRight - nLeft );

			/*객체의 중간위치 */
			nHalfW		= nLeft + (nWith/2 );
			nUFW_L		= nHalfW - ( ptrLPRConf->LPR_UF_W  );
			nUFW_R		= nHalfW + ( ptrLPRConf->LPR_UF_W  );

			if ( nUFW_L < 0 ) nUFW_L = 0;
			if ( nUFW_R >= nRight ) nUFW_R = nRight;

			/* 객체 길이의 0.8배 : 확장 체크 끝 위치  */
			fExtTop		= (float)(( nBottom - nTop ) *  0.8 );
			nExtTop		= (int)( nTop - fExtTop );

			/* 확장 체크 제외 범위 : 확장 체크 시작 위치 */
			fExceptLine = (float)(( nBottom - nTop ) * 0.3);
			nExceptLine = (int)( nTop - fExceptLine );

			if ( nExceptLine < 0 ) nExceptLine = 0;
			if ( nExtTop < 0 ) nExtTop = 0;
		
			if ( nExtTop >= nExceptLine ) continue;
			nExcepSize = nTop - nExceptLine ;

			if ( nExcepSize <=0 ) nExceptLine = 1;

			/*===========================================================================
			 [ 상단 확장 ]
			============================================================================*/
			nObjCnt = 0;
			nNotSigCnt =0;
			for( i = nTop ; i > nExtTop ; i-- )
			{		
				fDif	= 0;
				fSum2	= 0;
				nSum	= 0;
				nCnt	= 0;
				fMean	= 0;
				fVar	= 0;
				fSig	= 0;
				nStop	= 0;
				nChgTop	= 0;
				

				for( j = nUFW_L ; j < nUFW_R ; j++ )
				{
					nVal= ptrImageDataSrc->ptrSrcImage2D[ i][ j ];
					if( nVal > 0)
					{
						nObjCnt++;
					}

					nSum += nVal;
					nCnt++;
				}

				if ( i <= nExceptLine && nObjCnt <=  ptrLPRConf->LPR_MIN_PIXEL * 2 )
				{
					nStop = 1;
					break;
				}

				/* 평균 */
				if ( nCnt > 0 )
				{
					fMean = (float)( nSum / ( nCnt   ) );
				}
				
				for( j = nUFW_L ; j < nUFW_R ; j++ )
				{
					nVal = ptrImageDataSrc->ptrSrcImage2D[ i][ j ];
					fDif = abs( nVal - fMean ) ;	
					fSum2 += ( fDif * fDif ) ;		
				}

				fVar = (float)( fSum2 / nCnt );
				fSig = sqrt( fVar )	;

				if ( i <= nExceptLine && fSig >= m_fNewTh )
				{
					ptrRect->nTop = i;
					nChgTop = 1;
				}
				else if( i <= nExceptLine && fSig < m_fNewTh )
				{
					nNotSigCnt++;
					if ( nChgTop == 1 || nNotSigCnt > ptrLPRConf->LPR_MIN_CHAR_CNT -1)
						break;
				}
			}
			
		}
	}

	inter_IsvalidPlateRect( 0, ptrLPRConf, nLabelCnt, ptrRectOrg, ptrImageDataSrc, LOOSE_PLATE_RECT);

#if 1
	int bCon, nExtCnt;
	for ( q = 0 ; q < nLabelCnt ; q++ )
	{		
		ptrRect = NULL;
		ptrRect= (CVRECT_t *)&ptrRectOrg[ q ];
	
		if( ptrRect!= NULL && ptrRect->nSeqNum > 0 )
		{

			/* =================== 상단 확장 ==================*/
			bCon	= 1;
			nExtCnt = 0;			
			nLeft = ptrRect->nLeft;
			nRight= ptrRect->nRight;

			while( bCon )
			{
				nTop  = ptrRect->nTop;
				nObjCnt = 0;

				for( j = nLeft ; j < nRight ; j++ )
				{
					if ( ptrImageDataSrc->ptrSrcImage2D[ nTop ][ j ] != 0 ) nObjCnt++;				
				}

				if ( nObjCnt > ptrLPRConf->LPR_MIN_PIXEL )
				{	
					if (( ptrRect->nTop -1 ) < 0 ) 
						bCon = 0;
					else
						ptrRect->nTop--;
				}
				else
					bCon = 0;

				nExtCnt++;
				if ( nExtCnt >= 5 ) bCon = 0;
			}

			/* =================== 하단 확장 ==================*/
			bCon	= 1;
			nExtCnt = 0;			
			nLeft = ptrRect->nLeft;
			nRight= ptrRect->nRight;

			while( bCon )
			{
				nBottom  = ptrRect->nBottom;
				nObjCnt = 0;

				for( j = nLeft ; j < nRight ; j++ )
				{
					if ( ptrImageDataSrc->ptrSrcImage2D[ nBottom ][ j ] != 0 ) nObjCnt++;				
				}

				if ( nObjCnt > ptrLPRConf->LPR_MIN_PIXEL )
				{	
					if (( ptrRect->nBottom +1 ) >= ptrImageDataSrc->nH ) 
						bCon = 0;
					else
						ptrRect->nBottom++;
				}
				else 
					bCon = 0;

				nExtCnt++;
				if ( nExtCnt >= 5 ) bCon = 0;
			}

			/* =================== 좌단 확장 ==================*/
			bCon	= 1;
			nExtCnt = 0;			
			nTop	= ptrRect->nTop;
			nBottom	= ptrRect->nBottom;

			while( bCon )
			{
				nLeft  = ptrRect->nLeft;
				nObjCnt = 0;

				for( i = nTop ; i < nBottom ; i++ )
				{
					if ( ptrImageDataSrc->ptrSrcImage2D[ i ][ nLeft ] != 0 ) nObjCnt++;				
				}

				if ( nObjCnt > ptrLPRConf->LPR_MIN_PIXEL )
				{	
					if (( ptrRect->nLeft - 1 ) < 0 ) 
						bCon = 0;
					else
						ptrRect->nLeft--;
				}
				else 
					bCon = 0;

				nExtCnt++;
				if ( nExtCnt >= 5 ) bCon = 0;
			}

			/* =================== 우단 확장 ==================*/
			bCon	= 1;
			nExtCnt = 0;			
			nTop	= ptrRect->nTop;
			nBottom	= ptrRect->nBottom;

			while( bCon )
			{
				nRight  = ptrRect->nRight;
				nObjCnt = 0;

				for( i = nTop ; i < nBottom ; i++ )
				{
					if ( ptrImageDataSrc->ptrSrcImage2D[ i ][ nRight ] != 0 ) nObjCnt++;				
				}

				if ( nObjCnt > ptrLPRConf->LPR_MIN_PIXEL )
				{	
					if (( ptrRect->nRight + 1 ) >= ptrImageDataSrc->nW ) 
						bCon = 0;
					else
						ptrRect->nRight++;
				}
				else 
					bCon = 0;

				nExtCnt++;
				if ( nExtCnt >= 5 ) bCon = 0;
			}

		}
	}
#endif

	return ERR_SUCCESS;
}



/*======================================================================
큰4자리 숫자를 찾기위한 DIP 처리 
========================================================================*/
static int Big4NumRact_DIP( pLPR_CONF ptrLPRConf, pDIP_DATA ptrDIPData, pCVRECT ptrTempRect, 
						   pIMAGE_DATA ptrComTmpImageData, pIMAGE_DATA ptrImageDataSrc, BYTE EdgeType )
{
	int i;	
	int nErrCode= ERR_SUCCESS;
	int nWidth, nHeight, nPos;	


	( void )EdgeType;
	( void )ptrLPRConf;

	nWidth = ptrTempRect->nRight - ptrTempRect->nLeft;
	nHeight = ptrTempRect->nBottom - ptrTempRect->nTop;

	ptrComTmpImageData->nBitCount = ptrImageDataSrc->nBitCount;
	ptrComTmpImageData->nW = nWidth;
	ptrComTmpImageData->nH = nHeight;
	ptrComTmpImageData->nImageSize = ( nWidth * nHeight );
	
	nPos=0;
	for (i=ptrTempRect->nTop; i< ptrTempRect->nBottom; i++, nPos++ )
	{	
		memcpy( *( ptrComTmpImageData->ptrSrcImage2D + nPos ) , *( ptrImageDataSrc->ptrSrcImage2D + i ) + ptrTempRect->nLeft , nWidth );
	}
	
	nErrCode = Binarization_Otsu( ptrDIPData, ptrComTmpImageData );	
	if ( nErrCode != ERR_SUCCESS )
		return nErrCode;
	
	/*===========================================================================
	[ 밝은색 배경일 경우 반전 ]	
	============================================================================*/
	Plate_Inverse( ptrDIPData, ptrComTmpImageData);

	return ERR_SUCCESS;
}


static int  CandidatePlateRect( pLPR_CONF ptrLPRConf, pDIP_DATA ptrDIPData, CVRECT_t * ptrOrgRect, CVRECT_t * PrjRect, 
							  pIMAGE_DATA ptrCom_PrjImageData, pIMAGE_DATA ptrChgImageData, pIMAGE_DATA ptrImageDataSrc, int * ptrComWProjection, int * ptrComHProjection  )
{
	pCOMMONINFO ptrComInfo = NULL;

	int i;
	int nPos, nWidth, nHeight;
	int nErrCode = ERR_SUCCESS;

	int ** ptrComMap2 = NULL;
	pCVRECT ptrComObjectRect = NULL;
	pCVRECT ptrTempRect = NULL;

	int nObjCnt ;
	int nTop, nBottom, nLeft, nRight, nNewX;

	int nValid, nSumH, nSumW,nSumH2, nSumW2,nValidCnt, nMeanH,nMeanW, nDifH, nDifW, nSigH, nSigW;
	int nSumT, nSumB,nSumT2, nSumB2,nMeanT,nMeanB, nDifT, nDifB, nSigT, nSigB;
	int nPreRight, nSumIN, nSumIN2, nMeanIN, nDifIN, nSigIN;

	CVRECT_t TempRect[ 10 ];
	int nRT, nLT, nGap, absnGap, nArea;
	float fPlateRatio;

	ptrComInfo = COM_GetInfo();	
	
	if ( ptrComWProjection == NULL  || ptrComHProjection == NULL || ptrComInfo == NULL )
		return ERR_NO_PARAM;

	/*===========================================================================
	[ Projection을 위한 기본 DIP  ]
	가로 , 세로 Projection 을 용이하도록 후보 영역에 한하여 이진화 처리를 함 
	sobel, binarization_otsu
	ptrComTmpImageData : DIP 처리된 영상 이미지(번호판 후보지 )
	============================================================================*/		
	Big4NumRact_DIP( ptrLPRConf, ptrDIPData, ptrOrgRect, ptrCom_PrjImageData , ptrImageDataSrc, H_LINE_EDGE );

	PrjRect->nTop =0;
	PrjRect->nBottom = ptrCom_PrjImageData->nH;
	PrjRect->nLeft = 0;
	PrjRect->nRight = ptrCom_PrjImageData->nW;	

	/*===========================================================================
	[ 가로/가로 잡음 직선 제거 삭제 및 영역 재 정의 ]
	============================================================================*/
	internal_Del_Outer( ptrLPRConf, PrjRect,ptrCom_PrjImageData,ptrComInfo,ptrComWProjection,ptrComHProjection, BIG4FIND_STEP );

	ptrOrgRect->nLeft += ( PrjRect->nLeft - 0 );
	ptrOrgRect->nRight += ( PrjRect->nRight - ptrCom_PrjImageData->nW );
	ptrOrgRect->nTop += ( PrjRect->nTop - 0 );
	ptrOrgRect->nBottom += ( PrjRect->nBottom - ptrCom_PrjImageData->nH );

	nPos	= 0;
	nWidth	= PrjRect->nRight - PrjRect->nLeft;
	nHeight = PrjRect->nBottom - PrjRect->nTop;
	
	if( nWidth <= 0 ) nWidth = 0;
	if( nHeight <=0 ) nHeight = 0;
	if ( nHeight <= 1 || nWidth <= 1 )
		return ERR_SUCCESS;

	/*===========================================================================
	[ Bilinear Point 설정 ]
	============================================================================*/
	ptrOrgRect->BPoint.LT_POINT.nX= ptrOrgRect->nLeft;
	ptrOrgRect->BPoint.LT_POINT.nY= ptrOrgRect->nTop;

	ptrOrgRect->BPoint.LB_POINT.nX= ptrOrgRect->nLeft;
	ptrOrgRect->BPoint.LB_POINT.nY= ptrOrgRect->nBottom;

	ptrOrgRect->BPoint.RT_POINT.nX= ptrOrgRect->nRight;
	ptrOrgRect->BPoint.RT_POINT.nY= ptrOrgRect->nTop;

	ptrOrgRect->BPoint.RB_POINT.nX= ptrOrgRect->nRight;
	ptrOrgRect->BPoint.RB_POINT.nY= ptrOrgRect->nBottom;
	

	/*===========================================================================
	[영역내 큰 객체의 Labeling ]	
	영역내 4자리 숫자를 찾기위한 객체의 Labeling 처리 
	============================================================================*/	
	ptrComMap2 = COM_GetMap2Buf();
	ptrComObjectRect = COM_GetTempRectBuf();

	nObjCnt =0;
	nObjCnt= COM_SubDoLabeling8Neighbour( ptrCom_PrjImageData, ptrComMap2, PrjRect->nLeft, PrjRect->nRight, PrjRect->nTop, PrjRect->nBottom );
	if ( nObjCnt >= MAX_LABEL_CNT )
	{
		ptrOrgRect->nSeqNum = -1;
		return ERR_NO_PARAM;
	}

	/*===========================================================================
	[번호판 영역 내 객체 크기 정보 산출  ]	
	============================================================================*/
	MakeObjectSize( nObjCnt, ptrComObjectRect, ptrComMap2, ptrCom_PrjImageData, PrjRect->nLeft, PrjRect->nRight, PrjRect->nTop, PrjRect->nBottom, 1 );
	if ( nObjCnt < ptrLPRConf->LPR_MIN_CHAR_CNT )
	{
		ptrOrgRect->nSeqNum = -1;
		return ERR_NO_PARAM;
	}
	
	/* =============================================================
	객체 크기 체크 
	================================================================*/
	nSumH = nSumW = nSumH2 = nSumW2 = nValidCnt = nMeanH = nMeanW = nDifH = nDifW = nSigH = nSigW =0;
	nSumT = nSumB = nSumT2 = nSumB2 = nMeanT = nMeanB = nDifT = nDifB = nSigT = nSigB =0;
	nPreRight = nSumIN =  nSumIN2 = nMeanIN = nDifIN = nSigIN =0;

	for( i = 0 ; i < nObjCnt ; i++ )
	{
		ptrTempRect = (CVRECT_t *)&ptrComObjectRect[ i ];
		
		if ( ptrTempRect != NULL )
		{
			nTop	= ptrTempRect->nTop;
			nBottom = ptrTempRect->nBottom;
			nLeft	= ptrTempRect->nLeft;
			nRight	= ptrTempRect->nRight;

			nHeight = nBottom - nTop;
			nWidth	= nRight - nLeft;
			
			if ( ( nWidth > ptrLPRConf->LPR_MIN_PIXEL ) && ( nHeight > ptrLPRConf->LPR_MIN_BIG4_H ) )
			{					
				nValidCnt++;
				nSumH += nHeight;
				nSumW += nWidth;
				nSumT += nTop;
				nSumB += nBottom;

				if ( nPreRight != 0 )
				{
					nSumIN  = nLeft - nPreRight;
				}
				nPreRight = nRight;
			}
			else
			{
				ptrTempRect->nSeqNum = -1;

				
			}
		}
		
	}
	/* =============================================================
	큰 객체의 개수가 너무 작거나 많을 경우 
	================================================================*/
	if ( nValidCnt <= 1 || nValidCnt > ptrLPRConf->LPR_MAX_CHAR_CNT ) 
	{
		ptrOrgRect->nSeqNum = -1;
		return ERR_NO_PARAM;
	}

	nMeanH = nSumH / nValidCnt;
	nMeanW = nSumW / nValidCnt;
	nMeanT = nSumT / nValidCnt;
	nMeanB = nSumB / nValidCnt;
	nMeanIN = nSumIN / nValidCnt;

	nPreRight = 0;
	for( i = 0 ; i < nObjCnt ; i++ )
	{
		ptrTempRect = (CVRECT_t *)&ptrComObjectRect[ i ];

		if ( ptrTempRect != NULL  && ptrTempRect->nSeqNum >= 0)
		{
			nTop	= ptrTempRect->nTop;
			nBottom = ptrTempRect->nBottom;
			nLeft	= ptrTempRect->nLeft;
			nRight	= ptrTempRect->nRight;

			nHeight = nBottom - nTop;
			nWidth	= nRight - nLeft;

			nDifH	= abs( nHeight - nMeanH );
			nDifW	= abs( nWidth - nMeanW );
			nDifT	= abs( nTop - nMeanT );
			nDifB	= abs( nBottom - nMeanB );

			if ( nPreRight != 0 )
			{
				nDifIN  = abs( nLeft - nPreRight );
			}
			
			nSumH2 += ( nDifH * nDifH );
			nSumW2 += ( nDifW * nDifW );
			
			nSumT2 += ( nDifT * nDifT );
			nSumB2 += ( nDifB * nDifB );
			
			nSumIN2 += ( nDifIN * nDifIN);
			nPreRight = nRight;
		}
	}

	nSigH = (int)sqrt( (float)(nSumH2 / nValidCnt));
	nSigW = (int)sqrt( (float)(nSumW2 / nValidCnt));
	nSigT = (int)sqrt( (float)(nSumT2 / nValidCnt));
	nSigB = (int)sqrt( (float)(nSumB2 / nValidCnt));
	nSigIN =(int)sqrt( (float)(nSumIN2 / nValidCnt));

	/* 객체 사이의 간격이 너무 많이 벌어진 번호판 후보지 제거 */
	if ( nSigIN > ptrLPRConf->LPR_UF_W * 2 )
	{
		ptrOrgRect->nSeqNum = -1;
		return ERR_NO_PARAM;
	}

	int nThW , nThH, nThT, nThB;
	
	if ( nSigH <= ptrLPRConf->LPR_MIN_PIXEL  )
		nThH = ptrLPRConf->LPR_MIN_PIXEL + 2 ;
	else
		nThH = ptrLPRConf->LPR_MIN_PIXEL * 3;

	if ( nSigW <= ptrLPRConf->LPR_MIN_PIXEL )
		nThW = ptrLPRConf->LPR_MIN_PIXEL  +2;
	else
		nThW = ptrLPRConf->LPR_MIN_PIXEL * 3;

	if ( nSigT <= ptrLPRConf->LPR_MIN_PIXEL )
		nThT = ptrLPRConf->LPR_MIN_PIXEL  +2;
	else
		nThT = ptrLPRConf->LPR_MIN_PIXEL * 3;

	if ( nSigB <= ptrLPRConf->LPR_MIN_PIXEL )
		nThB = ptrLPRConf->LPR_MIN_PIXEL +2;
	else
		nThB = ptrLPRConf->LPR_MIN_PIXEL * 3;

	/* =============================================================
	큰 객체에 대한 유효성 검사
	a. 전체 평균 높이와 길이에 대한 분산을 구해서 분산 보다 높은 객체는 삭제
	b. 전체 평균 넓이와 길이에 대한 분산을 구해서 분산 보다 넓은 객체는 삭제
	c. 객체가 5개 이상일 일때는 마지막 객체를 제외하고 평균, 분산을 계산한다. 
	================================================================*/

	for( i = 0 ; i < nObjCnt ; i++ )
	{
		ptrTempRect = (CVRECT_t *)&ptrComObjectRect[ i ];

		if ( ptrTempRect != NULL  && ptrTempRect->nSeqNum >= 0)
		{
			nValid = 1;

			nTop	= ptrTempRect->nTop;
			nBottom = ptrTempRect->nBottom;
			nLeft	= ptrTempRect->nLeft;
			nRight	= ptrTempRect->nRight;

			nHeight = nBottom - nTop;
			nWidth	= nRight - nLeft;

			if ( nSigH != 0 && ( ( nHeight < ( nMeanH - nThH )) ) || ( nHeight > ( nMeanH + nThH ) )) 
			{
				nValid = 0;
			}
			
			if ( nValid == 1 &&  nSigW != 0 && ( nWidth < ( ptrLPRConf->LPR_MIN_CHAR_W ) ) || ( nWidth > ( nMeanW + nThW )) )
			{
				nValid = 0;
			}

			if ( nValid == 1 &&  ( nSigT != 0 && ( nTop < ( nMeanT - nThT )) || ( nTop > ( nMeanT +nThT ) ) ))
			{
				nValid = 0;
			}

			if ( nValid == 1 &&  ( nSigB != 0 && ( nBottom < ( nMeanB - nThB )) || ( nBottom > ( nMeanB + nThB )) ))
			{
				nValid = 0;
			}
			
			if ( nValid != 1 )
			{
				ptrTempRect->nSeqNum = -1;
				if( ptrLPRConf->LPR_DBG_SHOW_BIG4_DIP )
				{
					HLP_DrawLineRect( ptrCom_PrjImageData, ptrTempRect, 128 );
				}
			}
			else
			{
				if( ptrLPRConf->LPR_DBG_SHOW_BIG4_DIP )
				{
					HLP_DrawLineRect( ptrCom_PrjImageData, ptrTempRect, 255 );
				}
				
			}
		}
	}	

	if( ptrLPRConf->LPR_DBG_SHOW_BIG4_DIP )
	{
		nPos = 0;
		if (ptrOrgRect->nLeft == 0 )
		{
			nWidth = ptrOrgRect->nRight - ptrOrgRect->nLeft;
		}
		else
		{
			nWidth = ptrOrgRect->nRight - ptrOrgRect->nLeft + 1;
		}

		for (int i= ptrOrgRect->nTop; i< ptrOrgRect->nBottom; i++, nPos++)
		{
			memcpy( *( ptrChgImageData->ptrSrcImage2D + i ) + ptrOrgRect->nLeft , *( ptrCom_PrjImageData->ptrSrcImage2D + nPos ) + PrjRect->nLeft, nWidth );
		}
	}

	if( ptrLPRConf->LPR_DBG_RUN_BIG4_BILINEAR )
	{
		
		/* =============================================================
		[ 큰 객체를 기준으로 전체 번호판 영역의 재 설정 ]
		a. 4자리 숫자 중 가장 오른쪽 숫자의 높이와 넓이를 측정한다. 
		b. 4자리 숫자 중 가장 왼쪽 숫자의 높이와 넓이를 측정한다. 
		c. a와 b의 높이가 다를 경우 그 차감 정도에 비례해서 번호판의 Y 좌표를 다시 계산한다. 
		d. 카메라 위치를 기준으로 좌, 우를 판단하여 번호판의 X 좌표를 다시 계산한다. 

		================================================================*/
		
		nValidCnt = 0;
		memset( &TempRect, 0, sizeof( TempRect));
		nArea = COM_GetArea();

		for( i = nObjCnt-1 ; i >=0 ; i-- )
		{
			ptrTempRect = (CVRECT_t *)&ptrComObjectRect[ i ];

			if ( ptrTempRect != NULL  && ptrTempRect->nSeqNum >= 0)
			{	
				memcpy( &TempRect[ nValidCnt++ ], ptrTempRect, sizeof( CVRECT_t ));
				
				/* Max 10 */
				if ( nValidCnt >= 10 )
					break;			
			}
		}
		
		nRT = nLT = nGap = absnGap =0;

		/* =============================================================
		[ 카메라 위치 보정 ]
		================================================================*/
		/* 왼쪽 영상일경우 */
		if ( nArea == LEFT_AREA )
		{	
			ptrOrgRect->BPoint.LB_POINT.nX += ptrLPRConf->LPR_L_ANGLE_PIXEL;			
			ptrOrgRect->BPoint.RT_POINT.nX -= ptrLPRConf->LPR_L_ANGLE_PIXEL;

			if ( ptrOrgRect->BPoint.LB_POINT.nX > ptrImageDataSrc->nW ) ptrOrgRect->BPoint.RB_POINT.nX = ptrImageDataSrc->nW;
			if ( ptrOrgRect->BPoint.RT_POINT.nX < 0 ) ptrOrgRect->BPoint.RT_POINT.nX =0;
			
		}

		/* 오른쪽 영상일 경우 */
		else if ( nArea == RIGHT_AREA )
		{
			ptrOrgRect->BPoint.LT_POINT.nX += ptrLPRConf->LPR_R_ANGLE_PIXEL;
			ptrOrgRect->BPoint.RB_POINT.nX -= ptrLPRConf->LPR_R_ANGLE_PIXEL;			

			if ( ptrOrgRect->BPoint.LT_POINT.nX > ptrImageDataSrc->nW ) ptrOrgRect->BPoint.LT_POINT.nX = ptrImageDataSrc->nW;
			if ( ptrOrgRect->BPoint.RB_POINT.nX < 0 ) ptrOrgRect->BPoint.RB_POINT.nX = 0;
		}	
		
		/*4자리 숫자의 가장 오른쪽 객체의 위치를 번호판 오른쪽으로 잡는다 */
		if ( TempRect[ 0 ].nRight != 0 && nValidCnt >= ptrLPRConf->LPR_MIN_CHAR_CNT -1)
		{
			nNewX = ( ptrOrgRect->nLeft + TempRect[ 0 ].nRight + 1);
			nWidth =  TempRect[ 0 ].nRight -  TempRect[ 0 ].nLeft;
			nWidth = nWidth/2;

			if ( nNewX >ptrImageDataSrc->nW  ) 
				nNewX = ptrImageDataSrc->nW;

			ptrOrgRect->BPoint.RT_POINT.nX = nNewX ;
			ptrOrgRect->BPoint.RB_POINT.nX = nNewX;

			/* 가장 오른쪽 객체의 기울기를 이동한다.  */
			if ( nArea == LEFT_AREA )
			{	
				ptrOrgRect->BPoint.RT_POINT.nX  += nWidth ;
				ptrOrgRect->BPoint.RB_POINT.nX  += nWidth;
				ptrOrgRect->BPoint.RT_POINT.nX -= ptrLPRConf->LPR_L_ANGLE_PIXEL;
			}
			if ( nArea == RIGHT_AREA )
			{	
				ptrOrgRect->BPoint.RT_POINT.nX  += nWidth;
				ptrOrgRect->BPoint.RB_POINT.nX  += nWidth;
				ptrOrgRect->BPoint.RB_POINT.nX  -= ptrLPRConf->LPR_R_ANGLE_PIXEL;
			}
		}

		/* =============================================================
		[ 카메라 각 / 자동차 각 보정 ]
		================================================================*/
		if ( nValidCnt >= ptrLPRConf->LPR_MIN_CHAR_CNT -1 )
		{
			
			nRT		 = TempRect[ 0 ].nTop;				/* 최우측 정보 */
			//*첫번째는 테두리일 가능성이 높음 */
			nLT		 = TempRect[ nValidCnt -2 ].nTop;	/* 최좌측 정보 */

			nGap = ( nRT - nLT );
			absnGap = abs( nGap );

			/* 왼쪽이 높을 때 */
			if ( nGap > 0 && absnGap >= ptrLPRConf->LPR_MIN_PIXEL )
			{
				ptrOrgRect->BPoint.RT_POINT.nY += ( absnGap +1);
				ptrOrgRect->BPoint.LB_POINT.nY -= ( absnGap +1);

				if ( ptrOrgRect->BPoint.RT_POINT.nY > ptrImageDataSrc->nH ) ptrOrgRect->BPoint.RT_POINT.nY = ptrImageDataSrc->nH;
				if ( ptrOrgRect->BPoint.LB_POINT.nY < 0  ) ptrOrgRect->BPoint.LB_POINT.nY = 0;
			}

			/* 오른쪽이 높을 때 */
			if ( nGap < 0 && absnGap >= ptrLPRConf->LPR_MIN_PIXEL )
			{
				ptrOrgRect->BPoint.LT_POINT.nY += ( absnGap +1);
				ptrOrgRect->BPoint.RB_POINT.nY -= ( absnGap +1);

				if ( ptrOrgRect->BPoint.LT_POINT.nY > ptrImageDataSrc->nH ) ptrOrgRect->BPoint.LT_POINT.nY = ptrImageDataSrc->nH;
				if ( ptrOrgRect->BPoint.RB_POINT.nY < 0 ) ptrOrgRect->BPoint.RB_POINT.nY = 0;
			}
		}

		/* =============================================================
		[ 구 번호판의 경우 기본 영역보다 크게 처리한다. ]
		================================================================*/
		nWidth  = ptrOrgRect->nRight - ptrOrgRect->nLeft;
		nHeight = ptrOrgRect->nBottom - ptrOrgRect->nTop;

		if ( nHeight == 0 ) nHeight = 1;
		fPlateRatio = HLP_Cal_ImageRatio( ptrOrgRect );
#if 1
		if ( fPlateRatio < ptrLPRConf->LPR_NEW_PLATE_RATIO )
		{			
			ptrOrgRect->nLeft -=  ( ptrLPRConf->LPR_MIN_PIXEL);
			ptrOrgRect->nTop -= ( ptrLPRConf->LPR_MIN_PIXEL);		
			ptrOrgRect->nRight +=  ( ptrLPRConf->LPR_MIN_PIXEL);
			ptrOrgRect->nBottom += ( ptrLPRConf->LPR_MIN_PIXEL );		

			if ( ptrOrgRect->nTop < 0 )  ptrOrgRect->nTop =0;
			if ( ptrOrgRect->nLeft < 0 )  ptrOrgRect->nLeft =0;
			if ( ptrOrgRect->nRight > ptrImageDataSrc->nW )  ptrOrgRect->nRight = ptrImageDataSrc->nW;
			if ( ptrOrgRect->nBottom > ptrImageDataSrc->nH ) ptrOrgRect->nBottom = ptrImageDataSrc->nH;

			ptrCom_PrjImageData->nW  = ptrOrgRect->nRight - ptrOrgRect->nLeft;
			ptrCom_PrjImageData->nH =  ptrOrgRect->nBottom - ptrOrgRect->nTop;	
		}
		else
		{
			ptrCom_PrjImageData->nW  = ptrOrgRect->nRight - ptrOrgRect->nLeft;
			ptrCom_PrjImageData->nH =  ptrOrgRect->nBottom - ptrOrgRect->nTop;	
		}
#endif	

		nErrCode = Bilinear( ptrDIPData, ptrImageDataSrc, ptrCom_PrjImageData, &ptrOrgRect->BPoint );

		if( nErrCode == ERR_SUCCESS && ptrLPRConf->LPR_DBG_SHOW_BIG4_BOX )
		{
			nPos = 0;
			if ( ptrOrgRect->nLeft == 0 )
			{
				nWidth = ptrOrgRect->nRight - ptrOrgRect->nLeft;
			}
			else
			{
				nWidth = ptrOrgRect->nRight - ptrOrgRect->nLeft+1;
			}
			

			for (int i= ptrOrgRect->nTop; i< ptrOrgRect->nBottom; i++, nPos++)
			{
				memcpy( *( ptrChgImageData->ptrSrcImage2D + i ) + ptrOrgRect->nLeft , *( ptrCom_PrjImageData->ptrSrcImage2D +  nPos ), nWidth );
			}

			//HLP_DrawLineRect( ptrChgImageData, ptrOrgRect, 255 );
		}		

	}

	return nErrCode;
}


/*=========================================================================
4숫자 후보지 선정 ( 투영 방법 )
==========================================================================*/
static int FindPlateRect(pLPR_CONF ptrLPRConf, int nLabelCnt , pDIP_DATA ptrDIPData, 
						  CVRECT_t * ptrRect, pIMAGE_DATA ptrChgImageData, pIMAGE_DATA ptrImageDataSrc )
{
	int k;
	int nErrCode = ERR_SUCCESS;
	pCVRECT ptrOrgRect = NULL;
	CVRECT_t PrjRect;
	CVRECT_t PrioRect;
	int nIdex;
	int nValidCnt = 0;
	pIMAGE_DATA ptrCom_PrjImageData =NULL;


	int * ptrComWProjection = NULL;
	int * ptrComHProjection = NULL;

	ptrCom_PrjImageData		= COM_GetTempImageBuf();	

	if ( ptrCom_PrjImageData == NULL )
		return ERR_INVALID_DATA;

	ptrComWProjection = COM_GetWProjectionBuf();
	ptrComHProjection = COM_GetHProjectionBuf();	

	for( k = 0 ; k < nLabelCnt ; k++ )
	{
		ptrOrgRect = (pCVRECT)&ptrRect[ k ];

		if ( ptrOrgRect->nSeqNum <= 0 )
			continue;

		nErrCode = ERR_SUCCESS;		
		/*===========================================================================
		[ 번호판 영역 보정  ]
		4자리 번호를 기준으로 번호판의 기울기 및 카메라 위치를 보정한다. 
		============================================================================*/
		memset(&PrjRect, 0, sizeof( CVRECT_t));
		memset(&PrioRect, 0, sizeof( CVRECT_t));
		
		if ( CandidatePlateRect( ptrLPRConf, ptrDIPData, ptrOrgRect, &PrjRect, 
							  ptrCom_PrjImageData ,ptrChgImageData, ptrImageDataSrc, ptrComWProjection, ptrComHProjection ) != ERR_SUCCESS )
		{
			continue;
		}
		else
			nValidCnt++;
		
		/*===========================================================================
		[ 모든 번호판 보정 후보지 복사 ]
		보정된 번호판 영역은 원본이미지보다 클 뿐만아니라 영역이 중복될 수 있으므로 별도
		Buffer에서 관리한다. 
		============================================================================*/
		nIdex = -1;
		nIdex = COM_CopyRectImage( ptrCom_PrjImageData );
		if ( nIdex >= 0 )
		{
			ptrOrgRect->nLPR_RectIdx = nIdex;
		}
		else
		{
			ptrOrgRect->nLPR_RectIdx = ERR_ALLOC_BUF;
		}
	}
	
	return nValidCnt;
}
/*=========================================================================
번호판 2차 후보지 선정 ( 투영 방법 )
==========================================================================*/
static int VerifyPlateRect(pLPR_CONF ptrLPRConf, int nLabelCnt , pDIP_DATA ptrDIPData, 
						  CVRECT_t * ptrRect, pIMAGE_DATA ptrChgImageData, pIMAGE_DATA ptrImageDataSrc )
{
	int nWidth;
	int k;
	int nErrCode = ERR_SUCCESS;
	pCVRECT ptrOrgRect = NULL;

	int nResult , nValidCnt = 0;	
	pRECTIMAGE ptrRectImg				= NULL;

	int * ptrComWProjection				= NULL;
	int * ptrComHProjection				= NULL;
	int nPos;
	
	ptrComWProjection = COM_GetWProjectionBuf();
	ptrComHProjection = COM_GetHProjectionBuf();	

	for( k = 0 ; k < nLabelCnt ; k++ )
	{
		ptrOrgRect = (pCVRECT)&ptrRect[ k ];

		if ( ptrOrgRect->nSeqNum <= 0 )
			continue;

		/*===========================================================================
		[ 별도 보관된 번호판 영역의 후보지의 버퍼를 가져온다.  ]
		============================================================================*/
		ptrRectImg = COM_GetRectImage( ptrOrgRect->nLPR_RectIdx );
		if ( ptrRectImg == NULL  || ptrRectImg->ImageData.ptrSrcImage2D == NULL)
			continue;

		if( ptrLPRConf->LPR_DBG_RUN_PROJECTION )
		{	
			if ( Projection_Proc( ptrLPRConf, ptrDIPData, ptrOrgRect, &ptrRectImg->ImageRect, 
								  &ptrRectImg->ImageData, ptrImageDataSrc, ptrComWProjection, ptrComHProjection ) != ERR_SUCCESS )
			{
				ptrRectImg->ImageRect.nSeqNum = ptrOrgRect->nSeqNum;
				continue;
			}
		}
		
		/*===========================================================================
		[ 후보지 우선 순위 설정  ]	
		============================================================================*/
		if( ptrLPRConf->LPR_DBG_RUN_PRIORITY )
		{		
			nResult = Priority_Proc( ptrLPRConf,  ptrOrgRect, &ptrRectImg->ImageRect, 
				           &ptrRectImg->ImageData, ptrImageDataSrc, ptrComWProjection, ptrComHProjection );

			ptrRectImg->ImageRect.nSeqNum = ptrOrgRect->nSeqNum;
			if ( nResult > 0 ) nValidCnt++;

		}

		/*===========================================================================
		[ 모든 번호판 후보지 이진화 영상 복사 ]
		============================================================================*/
		nPos = ptrRectImg->ImageRect.nTop;
		if (ptrLPRConf->LPR_DBG_SHOW_PRJ_DIP )
		{
			if ( ptrOrgRect->nLeft != 0 )
			{
				nWidth = ( ptrOrgRect->nRight - ptrOrgRect->nLeft + 1 );
			}
			else
			{
				nWidth = ( ptrOrgRect->nRight - ptrOrgRect->nLeft );
			}

			for (int i= ptrOrgRect->nTop; i<=ptrOrgRect->nBottom; i++, nPos++)
			{
				if ( i < ptrChgImageData->nH )
					memcpy( *( ptrChgImageData->ptrSrcImage2D + i ) + ptrOrgRect->nLeft , *( ptrRectImg->ImageData.ptrSrcImage2D + nPos ) + ptrRectImg->ImageRect.nLeft, nWidth );				
			}			
		}
		
		/*===========================================================================
		[ 유효 번호판 후보지 이진화 영상 BOX 라인  ]
		============================================================================*/
		if (ptrLPRConf->LPR_DBG_SHOW_PRJ_VALID_BOX && ptrOrgRect->nSeqNum > 0 )
		{
			HLP_DrawLineRect( ptrChgImageData, ptrOrgRect, 255 );
		}
	}
	
	return nValidCnt;
}

/*=========================================================================
최종 후보지 선정
==========================================================================*/
static int GetExcellentPlate( pDIP_DATA ptrDIPData, int nRectCnt, CVRECT_t * ptrOrgRect, CVRECT_t *ptrDestRect, pIMAGE_DATA ptrImageDataSrc )
{
	int i;
	int nMaxSeq, nMaxPos;
	int nResult = ERR_NO_PARAM;
	int nValidCnt = 0;
	CVRECT_t * ptrTempRect= NULL;
	pRECTIMAGE ptrRectImg				= NULL;
	int nTop, nMaxTop;
		
	( void )ptrImageDataSrc;
	( void )ptrDIPData;	
	

	for( i = 0; i < nRectCnt ; i++)
	{
		ptrTempRect = (CVRECT_t *)&ptrOrgRect[ i ];
		if ( ptrTempRect != NULL )
		{	
			if ( ptrTempRect->nSeqNum > 0  )
			{	
				nValidCnt++;
			}
		}
	}

	
	if ( nValidCnt > 1 )
	{
		/*===========================================================================
		[ 번호판은 다른 번호판 영역보다 하위에 존재한다.  ]	
		============================================================================*/	
		nMaxPos= nMaxSeq= -1;
		nTop = nMaxTop = -1;
		for( i = 0; i < nRectCnt ; i++)
		{
			ptrTempRect = (CVRECT_t *)&ptrOrgRect[ i ];
			if ( ptrTempRect != NULL )
			{	
				if ( ptrTempRect->nSeqNum > 0  )
				{	
					nTop = ptrTempRect->nTop;
					if ( nTop > nMaxTop )
					{
						nMaxPos = i;
						nMaxTop = nTop;
					}
				}
			}
		}
		if ( nMaxPos >=0 ) ptrOrgRect[ nMaxPos ].nSeqNum++;


	}

	/*===========================================================================
	[ seqnum가 가장 높은 후보지 선정 ]	
	============================================================================*/	
	nMaxPos= nMaxSeq= -1;
	/* SeqNum이 가장 큰것을 찾는다 */
	for( i = 0; i < nRectCnt ; i++)
	{
		ptrTempRect = (CVRECT_t *)&ptrOrgRect[ i ];
		if ( ptrTempRect != NULL )
		{	
			if ( ptrTempRect->nSeqNum > 0  )
			{	
				if ( nMaxSeq < ptrTempRect->nSeqNum )
				{
					nMaxSeq = ptrTempRect->nSeqNum;
					nMaxPos = i;
				}
			}
		}
	}

	if ( nMaxPos >= 0 )
	{
		ptrTempRect = (CVRECT_t *)&ptrOrgRect[ nMaxPos ];
#if 0
		ptrRectImg = COM_GetRectImage( ptrTempRect->nLPR_RectIdx );
		if ( ptrRectImg != NULL )
		{
			memcpy( ptrDestRect, &ptrRectImg->ImageRect, sizeof( CVRECT_t ));
			nResult = ERR_SUCCESS;
		}
		else
		{
			nResult = ERR_INVALID_DATA;
		}
#else
		memcpy( ptrDestRect, ptrTempRect, sizeof( CVRECT_t ));
#endif
	}

	return nResult;
}

/*================================================================================
External Functions 
=================================================================================*/
int PLATE_Initialize( void )
{
	return ERR_SUCCESS;
}

/*======================================================================
번호판 영역 추출

pDIP_DATA ptrDIPData			: DIP 처리용 Image 메모리
pIMAGE_DATA ptrDestImageData	: 변환 처리 후 저자용 Image 메모리
pIMAGE_DATA ptrImageDataSrc		: 원본 데이터 Image 메모리 
pCVRECT ptrPlateRect			: 번호판 데이터 메모리 

========================================================================*/
int PLATE_GetRecognition( pLPR_CONF ptrLPRConf, pOCR_CONF ptrOCRConf, pDIP_DATA ptrDIPData, pIMAGE_DATA ptrDestImageData, 
						  pIMAGE_DATA ptrChgImageData, pIMAGE_DATA ptrImageDataSrc, pCVRECT ptrPlateRect)
{
	int nUniforH , nUniforW, nCandidateCnt;	
	pCOMMONINFO ptrComInfo = NULL;
	UNIFORMITY_t ** ptrComUniformitys = NULL;
	pCVRECT ptrComPlateRect = NULL;
	int nValidCnt =0;
	int nLabelCnt = 0;

	( void )ptrDestImageData;

	/*===========================================================================
	[ 공용 메모리 요청 ]
	============================================================================*/
	ptrComInfo = COM_GetInfo();
	ptrComUniformitys = COM_GetUniformityBuf();
	ptrComPlateRect = COM_GetPlateRectBuf();

	/*===========================================================================
	[ 유효 번호판 이진화 영상 저장용 버퍼 초기화 ]
	============================================================================*/
	COM_Init_RectImageBuf();


	if ( ptrComUniformitys == NULL || ptrComInfo == NULL || ptrPlateRect == NULL || ptrLPRConf == NULL ||  ptrOCRConf == NULL )
	{
		return ERR_ALLOC_BUF;
	}

	/*===========================================================================
	[ 이미지 스무딩 필터링 단계 ]
	강한 잡음을 먼저 제거하여 불균형 영역 인식률을 높인다. 
	============================================================================*/
	if ( ptrLPRConf->LPR_DBG_RUN_FIND_UF && ptrLPRConf->LPR_DBG_RUN_UF_PREDIP )
	{	
		Sobel( ptrDIPData, ptrChgImageData, H_LINE_EDGE );
		Binarization_Otsu( ptrDIPData, ptrChgImageData);
	}

	nUniforH = ptrComInfo->nUniformityH;
	nUniforW = ptrComInfo->nUniformityW;

	/*===========================================================================
	[ 라인별 불균일 영역 검출 ]	
	============================================================================*/	
	if ( ptrLPRConf->LPR_DBG_RUN_FIND_UF && MakeUniformityArea( ptrLPRConf, ptrDIPData, ptrComUniformitys, nUniforH, nUniforW, ptrChgImageData ) != ERR_SUCCESS )
	{
		return ERR_INVALID_DATA;
	}
	/*===========================================================================
	[ 잡음 불균일 영역 제외 ]	
	============================================================================*/	
	if( ptrLPRConf->LPR_DBG_RUN_FIND_UF && ptrLPRConf->LPR_DBG_RUN_UF_DEL_THICK )
	{
		Del_InvalidUniformity( ptrLPRConf, ptrComUniformitys , nUniforH, nUniforW);	
	}
	/*===========================================================================
	[ 불균등 영역 라벨링 ]
	서로 유사한 평균, 분산을 가지고 있을 경우 한 그룹으로 처리한다.
	============================================================================*/		
	if( ptrLPRConf->LPR_DBG_RUN_FIND_UF && ptrLPRConf->LPR_DBG_RUN_UF_LABELING )
	{	
		nCandidateCnt = COM_DoLabeling4Neighbour( ptrComUniformitys, nUniforH, nUniforW, ptrChgImageData );	
		if ( nCandidateCnt <= 0 )
		{
			return ERR_INVALID_DATA;
		}		

		/*===========================================================================
		[ Label Grouping ]
		1. 최소 / 최대 사이즈 유효성 검정
		2. 객체 내에 존재하는 다른 객체는 귀속 한다. 
		3. 객체의 영역을 최소화 한다. 
		============================================================================*/		
		memset( ptrComPlateRect, 0, sizeof( CVRECT_t ) * MAX_LABEL_CNT);
		nLabelCnt = GeneratePlateRectWithUniformify( ptrLPRConf, nCandidateCnt ,ptrComPlateRect, 
												 ptrComUniformitys, nUniforH, nUniforW , ptrChgImageData );
		

	}
	
	/*===========================================================================
	[ 차량 번호판 영역의 상 하, 좌 우 영역을 Merge 한다. ]
	============================================================================*/
	if( ptrLPRConf->LPR_DBG_RUN_FIND_UF && ptrLPRConf->LPR_DBG_RUN_UF_MERGE  && nLabelCnt > 0 )
	{
		nValidCnt= MergePlateRect( ptrLPRConf,nLabelCnt, ptrComPlateRect, ptrChgImageData );
	}
	else
	{
		nValidCnt = nLabelCnt;
	}

	/*===========================================================================
	[ 차량 번호판 영역의 문자크기보다 조금 크게 확장한다. 
	  이는 저해상 영상에서 문자의 크기에 맞게 자를 경우 글자 객체를 해손하는 경우가 
	  심하기 때문이다. ]
	============================================================================*/
	if( ptrLPRConf->LPR_DBG_RUN_FIND_UF && ptrLPRConf->LPR_DBG_RUN_UF_EXTENSION && nValidCnt > 0 )
	{
		ExtensionPlateRect( ptrLPRConf,nLabelCnt, ptrComPlateRect, ptrChgImageData );		
	}	

	/*===========================================================================
	[ 차량 4자리 숫자 찾기 ]
	1. 원본 이미지 복사
	2. 영상 이진화 	
	3. 객체 크기 유효성 판단 
	4. 유효한 4자리 검출 
	============================================================================*/
	if( ptrLPRConf->LPR_DBG_RUN_FIND_UF && !ptrLPRConf->LPR_DBG_SHOW_UF_DIP )
	{	
		HLP_CopyImageDataNotAlloc( ptrChgImageData,ptrImageDataSrc );
	}

	/*===========================================================================
	[ 번호판 후보영역 표시 ]
	============================================================================*/
	if( ptrLPRConf->LPR_DBG_RUN_FIND_UF && ptrLPRConf->LPR_DBG_SHOW_UF_VALID_BOX  )
	{
		inter_DrawlineRect( nLabelCnt,ptrComPlateRect,ptrChgImageData );
	}

	/*===========================================================================
	[ 번호판 후보지 검출 ]
	번호판 4자리 숫자영역을 기준으로 후보 번호판 영역을 찾는다 
	번호판 영역은 원본이미지보다 클 뿐만아니라 영역이 중복될 수 있으므로 별도Buffer에서 관리된다.
	============================================================================*/	
	if ( ptrLPRConf->LPR_DBG_RUN_FIND_BIG4 && nValidCnt > 0 )
	{
		nValidCnt= FindPlateRect( ptrLPRConf, nLabelCnt, ptrDIPData, ptrComPlateRect, ptrChgImageData, ptrImageDataSrc );
	}
	
	/*===========================================================================
	[ 번호판 후보지 유효성 체크 ]
	별도 관리된 Buffer에서 유효 번호판 영역을 호출 한다. .
	============================================================================*/
	if ( ptrLPRConf->LPR_DBG_RUN_PROJECTION && nValidCnt > 0 )
	{
		nValidCnt= VerifyPlateRect( ptrLPRConf, nLabelCnt, ptrDIPData, ptrComPlateRect, ptrChgImageData, ptrImageDataSrc );
	}	
	
	/*===========================================================================
	[ 최종 후보지 선택 ]	
	============================================================================*/		
	if( ptrLPRConf->LPR_DBG_RUN_CHOICE_PLATE && nValidCnt > 0)
	{				
		GetExcellentPlate( ptrDIPData,  nLabelCnt, ptrComPlateRect, ptrPlateRect, ptrImageDataSrc ); 
		if ( ptrPlateRect->nRight == 0 || ptrPlateRect->nBottom == 0 )
		{
			return ERR_NOT_HAVE_RECT;
		}
	}
	else
	{
		nValidCnt = 0;
	}
	
	/*===========================================================================
	[ 불균등 영역 이미지 복사 ]
	============================================================================*/
	if( ptrLPRConf->LPR_DBG_SHOW_UF_LINE )
	{	
		int i,j;
		for( i = 0; i < nUniforH ; i++ )
		{
			for( j = 0 ; j < nUniforW ; j++ )
			{
				if ( ptrComUniformitys[ i][ j ].nUniform == 1 )
				{	
					COM_DrawCandidateLine( ptrComUniformitys[ i][ j ].nY, ptrComUniformitys[ i][ j ].nX, ptrComUniformitys[ i][ j ].nX + ptrLPRConf->LPR_UF_W, ptrChgImageData );
				}
			}		
		}
	}

	return nValidCnt;
}

int PLATE_IOCTL( pIOCTL_DATA ptrIOCTLData )
{
	( void )ptrIOCTLData;

	return ERR_SUCCESS;
}

int PLATE_Release( void )
{
	return ERR_SUCCESS;
}