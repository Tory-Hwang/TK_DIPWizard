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
#include "OCR_FEATURES.h"

#define MIN_LINE_SIZE		3
#define FILED_EFF_CNT		8
#define SONDE_CNT			4
#define HAAR_LIKE_CNT		8

static int * m_ptrODRWPrj = NULL;
static int * m_ptrODRHPrj = NULL;
static pOCR_CONF m_ptrocr_conf = NULL;

static int m_nLastFeaturePos = 0;


static int ChkFeatureCnt( int nNextCnt )
{
	if ( nNextCnt + m_nLastFeaturePos > INPUT_LAYER_NODE_CNT )
		return ERR_OCR_CHAR_SIZE;

	return ERR_SUCCESS;
}

static int ReleaseTempPrj( void )
{
	int nResult = ERR_SUCCESS;

	if ( m_ptrODRHPrj != NULL )
	{
		delete[] m_ptrODRHPrj;
		m_ptrODRHPrj = NULL;

	}	

	if ( m_ptrODRWPrj != NULL )
	{
		delete [] m_ptrODRWPrj;
		m_ptrODRWPrj = NULL;
	}	

	return nResult;
}

static int MakeTempPrj( int nHeight, int nWidth )
{
	int nResult = ERR_SUCCESS;

	if ( m_ptrODRHPrj == NULL )
	{
		m_ptrODRHPrj = new int[ nHeight ];
		if ( m_ptrODRHPrj == NULL )
		{
			ReleaseTempPrj();
			return ERR_ALLOC_BUF;
		}
	}

	if ( m_ptrODRWPrj == NULL )
	{
		m_ptrODRWPrj = new int[ nWidth ];
		if ( m_ptrODRWPrj == NULL )
		{
			ReleaseTempPrj();
			return ERR_ALLOC_BUF;
		}
	}

	return nResult;
}


static int GetLineSizeH( int *ptrCurPos, int * ptrCurFlag, int *ptrNextPos, int *ptrNextFlag, int * ptrTotalCursize , int *ptrTotalNextsize, 
						int i, int j, pIMAGE_DATA ptrImage )
{
	int nCurSize =0;
	int nNextSize = 0;

	if ( *ptrCurFlag == FALSE  &&  ptrImage->ptrSrcImage2D[ i ][ j ] == 255 )
	{
		*ptrCurPos = i;
		*ptrCurFlag = TRUE;
	}

	if ( *ptrCurFlag == TRUE && ptrImage->ptrSrcImage2D[ i ][ j ] == 0 )
	{
		nCurSize = i - *ptrCurPos;
		*ptrCurFlag = FALSE;

		if ( nCurSize > MIN_LINE_SIZE )
			*ptrTotalCursize += nCurSize;
	}

	if ( *ptrNextFlag == FALSE  &&  ptrImage->ptrSrcImage2D[ i ][ j+1 ] == 255 )
	{
		*ptrNextPos = i;
		*ptrNextFlag = TRUE;
	}
	if ( *ptrNextFlag == TRUE && ptrImage->ptrSrcImage2D[ i ][ j+1 ] == 0 )
	{
		nNextSize = i - *ptrNextPos;
		*ptrNextFlag = FALSE;

		if ( nNextSize > MIN_LINE_SIZE )
			*ptrTotalNextsize += nNextSize;
	}
	return ERR_SUCCESS;
}

static int GetLineSizeW( int *ptrCurPos, int * ptrCurFlag, int *ptrNextPos, int *ptrNextFlag, int * ptrTotalCursize , int * ptrTotalNextsize, 
						  int i, int j, pIMAGE_DATA ptrImage)
{
	int nCurSize =0;
	int nNextSize = 0;

	if ( *ptrCurFlag == FALSE  &&  ptrImage->ptrSrcImage2D[ i ][ j ] == 255 )
	{
		*ptrCurPos = j;
		*ptrCurFlag = TRUE;
	}
	if ( *ptrCurFlag == TRUE && ptrImage->ptrSrcImage2D[ i ][ j ] == 0 )
	{
		nCurSize = j - *ptrCurPos;
		*ptrCurFlag = FALSE;

		if ( nCurSize > MIN_LINE_SIZE )
			* ptrTotalCursize += nCurSize;
	}

	if ( *ptrNextFlag == FALSE  &&  ptrImage->ptrSrcImage2D[ i +1 ][ j ] == 255 )
	{
		*ptrNextPos = j;
		*ptrNextFlag = TRUE;
	}
	if ( *ptrNextFlag == TRUE && ptrImage->ptrSrcImage2D[ i +1 ][ j ] == 0 )
	{
		nNextSize = j - *ptrNextPos;
		*ptrNextFlag = FALSE;

		if ( nNextSize > MIN_LINE_SIZE )
			* ptrTotalNextsize += nNextSize;
	}

	return ERR_SUCCESS;
}

static int GetFieldEffect( int nWidthFE, int nHeightFE, int nTop, int nBottom, int nRight, int nLeft,  int *nB, pIMAGE_DATA ptrImage)
{
	int i, j;

	/*==== N1  =====*/	
	for( j = nWidthFE ; j >= nLeft ; j--)
	{
		if( ptrImage->ptrSrcImage2D[ nHeightFE  ][ j ] == 255 )
			break;
		else
			nB[0]++;
	}

	/*==== N2  =====*/
	j = 0;
	for( i = nHeightFE ; i >= nTop ; i--, j++)
	{
		if( ( nWidthFE - j <= nLeft ) || ( ptrImage->ptrSrcImage2D[ i  ][ nWidthFE - j ] == 255) )			
			break;
		else
			nB[1]++;
	}

	/*==== N3  =====*/
	for( i = nHeightFE ; i >= nTop ; i--)
	{
		if( ptrImage->ptrSrcImage2D[ i  ][ nWidthFE ] == 255 )
			break;
		else
			nB[2]++;
	}

	/*==== N4  =====*/
	j = 0;
	for( i = nHeightFE ; i >= nTop ; i--, j++)
	{
		if( ( nWidthFE + j >= nRight ) || ( ptrImage->ptrSrcImage2D[ i  ][ nWidthFE + j ] == 255) )
			break;
		else
			nB[3]++;
	}

	/*==== N5  =====*/	
	for( j = nWidthFE ; j <=nRight ; j++ )
	{
		if( ptrImage->ptrSrcImage2D[ nHeightFE  ][ j ] == 255 )
			break;
		else
			nB[4]++;
	}

	/*==== N6  =====*/
	j = 0;
	for( i = nHeightFE ; i<= nBottom ; i++, j++)
	{
		if( ( nWidthFE + j >= nRight ) || ( ptrImage->ptrSrcImage2D[ i  ][ nWidthFE + j ] == 255) )
			break;
		else
			nB[5]++;
	}

	/*==== N7  =====*/
	for( i = nHeightFE ; i <= nBottom ; i++)
	{
		if( ptrImage->ptrSrcImage2D[ i  ][ nWidthFE ] == 255 )
			break;
		else
			nB[6]++;
	}

	/*==== N8  =====*/
	j = 0;
	for( i = nHeightFE ; i <= nBottom ; i++, j++)
	{
		if( ( nWidthFE - j <= nLeft ) || ( ptrImage->ptrSrcImage2D[ i  ][ nWidthFE - j ] == 255) )			
			break;
		else
			nB[7]++;
	}

	return ERR_SUCCESS;
}

static int GetProjectCnt( int nStart, int nEnd, int * ptrSmallCnt, int * ptrMidCnt, int * ptrLargeCnt , int * ptrPrj )
{
	int i, nCnt, nSmall, nMid, nLarge;
		
	nSmall		= MIN_LINE_SIZE;
	nMid		= MIN_LINE_SIZE*2;
	nLarge		= MIN_LINE_SIZE*3;

	*ptrSmallCnt = *ptrMidCnt = *ptrLargeCnt = 0;

	for( i = nStart ; i < nEnd ; i++ )
	{
		nCnt =  *( ptrPrj + i );
		if ( nCnt <= nSmall )					*ptrSmallCnt +=1;		/* 소 */
		if ( nCnt > nSmall && nCnt <= nMid )	*ptrMidCnt  +=1;		/* 중 */
		if ( nCnt > nMid  )						*ptrLargeCnt +=1;		/* 대 */
	}

	return ERR_SUCCESS;
}

/*=========================================================================
Morphology_Feature 특징점 추출 
==========================================================================*/
static int  Morphology_Feature( pIMAGE_DATA ptrImage, pOCR_FEATURE ptrFeature )
{
	int i, j, k ;
	int nStartCnt;
	int nCVStartCnt;
	int nCheckPos, nPreObjCnt, nObjCnt;
	int nHSeparCnt;

	int nHSize, nLargeCnt, nMidCnt, nSmallCnt;
	int bCheckLine;

	int nCurFlag, nCurPos, nCurSize, nTotalCursize;
	int nNextFlag, nNextPos, nNextSize, nTotalNextsize;
	int nFirstPos, nSecondPos;

	int nTop, nBottom, nLeft, nRight;
	int nOrgBottom, nOrgRight;

	int nHalfH, nHalfW;	
	int nHeight, nWidth, nFirstPosW, nFirstPosH;
	int nSmall,nMid, nLarge;

	CVRECT_t OrgRect;
	
	/*===========================================================================
	STEP 1 
	문자의 시작 위치 설정 
	============================================================================*/	
	nTop	= ptrImage->nH;
	nBottom = 0;
	nLeft	= ptrImage->nW;
	nRight	= 0;
	
	for( i = 0 ; i < ptrImage->nH; i++ )
	{			
		for( j = 0 ; j < ptrImage->nW ; j++ )
		{
			if( ptrImage->ptrSrcImage2D[ i ][ j ] > 0 )
			{
				if( nTop > i ) nTop = i;
				if ( nBottom < i ) nBottom =i;
				if( nLeft > j ) nLeft = j;
				if ( nRight < j ) nRight = j;
			}
		}
	}
	
	/* 대칭처리를 위한 변수 */
	nOrgBottom	= nBottom;
	nOrgRight	= nRight;

	/* Bottom, Right는 배경 1pixel을 포함한다. */
	nBottom += 1;
	nRight += 1;
	
	memset( &OrgRect, 0, sizeof( OrgRect));
	OrgRect.nTop	= nTop;
	OrgRect.nLeft	= nLeft;
	OrgRect.nBottom = nBottom;
	OrgRect.nRight  = nRight;
	/*===========================================================================
	STEP 2	
	수평 직선 크기 개수 
	============================================================================*/	
	nStartCnt	= 0;
	nCVStartCnt	= 0;
	nLargeCnt	= 0;
	nMidCnt		= 0;
	nSmallCnt	= 0;
	
	nFirstPos	= ptrImage->nW / 3;
	nSecondPos  = nFirstPos *2;

	nCheckPos	= nTop;
	nHSeparCnt	= 0;
	nPreObjCnt	= 0;

	k = 0;
	nCurFlag = nCurPos = nCurSize = nTotalCursize = 0;
	nNextFlag = nNextPos = nNextSize = nTotalNextsize = 0;

	for(i = nTop ; i <= nBottom ; i++ )
	{	
		bCheckLine = 0;
		nObjCnt = 0;

		/* ==================================================
		수평 시작점 개수 파악 우->좌 Mask
		=====================================================*/
		for( j = nRight ; j >= nLeft  ; j-- )
		{	
			if ( ptrImage->ptrSrcImage2D[ i ][ j ] == 255) nObjCnt++;
		}

		/* ===================================================
		수평 라인에 객체가 없을 경우( 분리 )될 경우 수평 분리 개수를 
		저장하고 수평 line의 시작점을 초기화 한다. 
		=====================================================*/
		if( nPreObjCnt !=0 && nObjCnt == 0 )
		{			
			nHSeparCnt++;			
		}		
		nPreObjCnt = nObjCnt;

		/* ===================================================
		수평 Line 길이 체크 유무
		수평 Line 길이는 2Line간격으로 2Line을 동시에 체크한다. 
		=====================================================*/
		if ( nCheckPos == i )
		{
			if( nObjCnt > 0 )
			{
				k = i;			
				bCheckLine = 1;
				nHSize = 0;				
				nCheckPos  += 2;

				nCurFlag = nCurPos = nCurSize = nTotalCursize = 0;
				nNextFlag = nNextPos = nNextSize = nTotalNextsize = 0;
			}
			else
			{
				nCheckPos += 1;
			}
		}
		
		/* ==================================================
		수평 시작점 개수 파악 좌->우 Mask
		수평 직선 크기 파악 
		직선의 크기 파악은 현재라인과 다음라인을 동시에 체크한다. 
		=====================================================*/
		for( j = nLeft ; j <= nRight ; j++ )
		{	
			/* 수평 직선 크기 파악*/
			if ( bCheckLine )
			{	
				GetLineSizeW( &nCurPos, &nCurFlag, &nNextPos, &nNextFlag, &nTotalCursize, &nTotalNextsize, k, j, ptrImage);				
			}
		}

		/* ============================================================
		 2Line의 전체 Line 길이에 따라 대, 중, 소로 분리한다. 
		==============================================================*/
		if ( bCheckLine )
		{
			nHSize = ( nTotalCursize + nTotalNextsize);

			if ( nHSize >= 1 && nHSize <= nFirstPos )				nSmallCnt++;
			if ( nHSize > nFirstPos && nHSize <= nSecondPos )		nMidCnt++;
			if ( nHSize > nSecondPos  )	nLargeCnt++;
		}
	}	
	
	/* 마지막 최종 배경은 제외한다 */
	if( ChkFeatureCnt( FILED_EFF_CNT ) != ERR_SUCCESS ) return ERR_OCR_CHAR_SIZE;
	
	nHSeparCnt -= 1;
	ptrFeature->Feature[ m_nLastFeaturePos++ ] = nHSeparCnt;	
#if 0
	ptrFeature->Feature[ m_nLastFeaturePos++ ] = nLargeCnt;
	ptrFeature->Feature[ m_nLastFeaturePos++ ] = nMidCnt;
	ptrFeature->Feature[ m_nLastFeaturePos++ ] = nSmallCnt;
#endif
	/*===========================================================================
	STEP 3	
	수직 직선 크기 개수 
	============================================================================*/	
	nStartCnt =0;
	nCVStartCnt=0;
	nLargeCnt = 0;
	nMidCnt = 0;
	nSmallCnt = 0;
	
	nFirstPos	= ptrImage->nH / 3;
	nSecondPos  = nFirstPos *2;

	nCheckPos  = nLeft;
	nHSeparCnt = 0;
	nPreObjCnt = 0;

	for(j = nLeft ; j <= nRight ; j++ )
	{	
		bCheckLine = 0;
		nObjCnt = 0;

		/* ==================================================
		수직 시작점 개수 파악 하->상 Mask
		=====================================================*/
		for( i = nBottom ; i >= nTop  ; i-- )
		{	
			if ( ptrImage->ptrSrcImage2D[ i ][ j ] == 255) nObjCnt++;
		}

		/* ===================================================
		수직 라인에 객체가 없을 경우( 분리 )될 경우 수직 분리 개수를 
		저장하고 수직 line의 시작점을 초기화 한다. 
		=====================================================*/
		if( nPreObjCnt !=0 && nObjCnt == 0 )
		{			
			nHSeparCnt++;			
		}		
		nPreObjCnt = nObjCnt;

		/* ===================================================
		수평 Line 길이 체크 유무
		수평 Line 길이는 2Line간격으로 2Line을 동시에 체크한다. 
		=====================================================*/
		if ( nCheckPos == j )
		{
			if( nObjCnt > 0 )
			{
				k = j;			
				bCheckLine = 1;
				nHSize = 0;				
				nCheckPos  += 2;

				nCurFlag = nCurPos = nCurSize = nTotalCursize = 0;
				nNextFlag = nNextPos = nNextSize = nTotalNextsize = 0;
			}
			else
			{
				nCheckPos += 1;
			}
		}

		/* ==================================================
		수직 시작점 개수 파악 상->하 Mask
		수직 직선 크기 파악 
		직선의 크기 파악은 현재라인과 다음라인을 동시에 체크한다. 
		=====================================================*/
		for( i = nTop ; i <= nBottom ; i++ )
		{	
			/* 수직 직선 크기 파악*/
			if ( bCheckLine )
			{
				GetLineSizeH( &nCurPos, &nCurFlag,&nNextPos, &nNextFlag, &nTotalCursize,  &nTotalNextsize, i, k, ptrImage);
			}
		}

		/* ============================================================
		 2Line의 전체 Line 길이에 따라 대, 중, 소로 분리한다. 
		==============================================================*/
		if ( bCheckLine )
		{
			nHSize = ( nTotalCursize + nTotalNextsize);

			if ( nHSize >= 1 && nHSize <= nFirstPos )				nSmallCnt++;
			if ( nHSize > nFirstPos && nHSize <= nSecondPos )		nMidCnt++;
			if ( nHSize > nSecondPos  )								nLargeCnt++;
		}
	}
	
	 /* 마지막 최종 배경은 제외한다 */
	if( ChkFeatureCnt( FILED_EFF_CNT ) != ERR_SUCCESS ) return ERR_OCR_CHAR_SIZE;

	nHSeparCnt -= 1;
	ptrFeature->Feature[ m_nLastFeaturePos++ ] = nHSeparCnt;
#if 0
	ptrFeature->Feature[ m_nLastFeaturePos++ ] = nLargeCnt;
	ptrFeature->Feature[ m_nLastFeaturePos++ ] = nMidCnt;
	ptrFeature->Feature[ m_nLastFeaturePos++ ] = nSmallCnt;	
#endif


	/*===========================================================================
	STEP 4
	초, 종성 영역에서의 Filed Effect 처리 ( 배경의 길이 )
	============================================================================*/	

	int nTopFE, nBottomFE;
	int nCenterWidthFE, nLeftWidthFE, nRightWidthFE, nCenterHeightFE;
	int nB[ FILED_EFF_CNT ];
	int nQuartPosW;

	nHeight			= nBottom - nTop;
	nWidth			= nRight - nLeft;
	
	nFirstPosH		= nHeight/6;	
	nFirstPosW		= nWidth/2;
	nQuartPosW		= nWidth / 4;

	nTopFE			= nTop + ( nFirstPosH );
	nBottomFE		= nTopFE + ( nFirstPosH * 4);
	
	nCenterHeightFE = nTop + nHeight/2;
	nCenterWidthFE	= nLeft + nFirstPosW;
	nLeftWidthFE	= nLeft + nQuartPosW;
	nRightWidthFE	= nRight - nQuartPosW;

	nSmall			= MIN_LINE_SIZE;
	nMid			= MIN_LINE_SIZE*2;
	nLarge			= MIN_LINE_SIZE*3;
#if 0
	/*==========================================================================
	초성 영역에의 Filed Effect 처리
	============================================================================*/
	if( ChkFeatureCnt( FILED_EFF_CNT ) != ERR_SUCCESS ) return ERR_OCR_CHAR_SIZE;
	memset( nB, 0, sizeof( nB ));
	GetFieldEffect( nCenterWidthFE, nTopFE, nTop, nBottom, nRight, nLeft,  nB, ptrImage);
	for( i = 0 ; i < FILED_EFF_CNT ; i++ )
	{
		if ( nB[i] <= nSmall )					nB[i] =1;	/* 소 */
		if ( nB[i] > nSmall && nB[i] <= nMid )	nB[i] =2;	/* 중 */
		if ( nB[i] > nMid  && nB[i] <= nLarge)	nB[i] =3;	/* 대 */
		if ( nB[i] > nLarge  )					nB[i] =4;	/* Big Large대 */

		ptrFeature->Feature[ m_nLastFeaturePos++ ] = (float)nB[i]/10;	
	}	

	/*==========================================================================
	종성 영역에의 Filed Effect 처리
	============================================================================*/
	if( ChkFeatureCnt( FILED_EFF_CNT ) != ERR_SUCCESS ) return ERR_OCR_CHAR_SIZE;
	memset( nB, 0, sizeof( nB ));
	GetFieldEffect( nCenterWidthFE, nBottomFE, nTop, nBottom, nRight, nLeft,  nB, ptrImage);	
	for( i = 0 ; i < FILED_EFF_CNT ; i++ )
	{
		if ( nB[i] <= nSmall )					nB[i] =1;	/* 소 */
		if ( nB[i] > nSmall && nB[i] <= nMid )	nB[i] =2;	/* 중 */
		if ( nB[i] > nMid  && nB[i] <= nLarge)	nB[i] =3;	/* 대 */
		if ( nB[i] > nLarge  )					nB[i] =4;	/* Big Large대 */

		ptrFeature->Feature[ m_nLastFeaturePos++ ] = (float)nB[i]/10;	
	}

	/*==========================================================================
	왼쪽 가운데 영역에의 Filed Effect 처리
	============================================================================*/
	if( ChkFeatureCnt( FILED_EFF_CNT ) != ERR_SUCCESS ) return ERR_OCR_CHAR_SIZE;
	memset( nB, 0, sizeof( nB ));
	GetFieldEffect( nLeftWidthFE, nCenterHeightFE, nTop, nBottom, nRight, nLeft,  nB, ptrImage);	
	for( i = 0 ; i < FILED_EFF_CNT ; i++ )
	{
		if ( nB[i] <= nSmall )					nB[i] =1;	/* 소 */
		if ( nB[i] > nSmall && nB[i] <= nMid )	nB[i] =2;	/* 중 */
		if ( nB[i] > nMid  && nB[i] <= nLarge)	nB[i] =3;	/* 대 */
		if ( nB[i] > nLarge  )					nB[i] =4;	/* Big Large대 */

		ptrFeature->Feature[ m_nLastFeaturePos++ ] = (float)nB[i]/10;	
	}

	/*==========================================================================
	오른쪽 가운데 영역에의 Filed Effect 처리
	============================================================================*/
	if( ChkFeatureCnt( FILED_EFF_CNT ) != ERR_SUCCESS ) return ERR_OCR_CHAR_SIZE;
	memset( nB, 0, sizeof( nB ));
	GetFieldEffect( nRightWidthFE, nCenterHeightFE, nTop, nBottom, nRight, nLeft,  nB, ptrImage);	
	for( i = 0 ; i < FILED_EFF_CNT ; i++ )
	{
		if ( nB[i] <= nSmall )					nB[i] =1;	/* 소 */
		if ( nB[i] > nSmall && nB[i] <= nMid )	nB[i] =2;	/* 중 */
		if ( nB[i] > nMid  && nB[i] <= nLarge)	nB[i] =3;	/* 대 */
		if ( nB[i] > nLarge  )					nB[i] =4;	/* Big Large대 */

		ptrFeature->Feature[ m_nLastFeaturePos++ ] =(float)nB[i]/10;	
	}

#endif

#if 0
	/*===========================================================================
	STEP 5
	Sonde 기법 
	============================================================================*/	
	nHeight		= nBottom - nTop;
	nWidth		= nRight - nLeft;

	nHalfH		= nTop + ( nHeight /2 );
	nHalfW		= nLeft + ( nWidth /2 );

	memset( nB, 0, sizeof( nB ));
	/* A - B 사이 객체*/
	for( i = nTop ; i<= nHalfH ; i++ )
	{
		if( ptrImage->ptrSrcImage2D[ i ][ nHalfW ] == 255 )
		{
			nB[ 0 ] +=1;
		}
	}
	
	/* B - C 사이 객체*/
	for( i = nHalfH ; i <= nBottom ; i++ )
	{
		if( ptrImage->ptrSrcImage2D[ i ][ nHalfW ] == 255 )
		{
			nB[ 1 ] +=1;
		}
	}
	
	/* D - E 사이 객체*/
	nFirstPosH	= nTop + ( ( nHalfH - nTop )/2 );
	for( j = nLeft ; j<= nRight ; j++ )
	{
		if( ptrImage->ptrSrcImage2D[ nFirstPosH ][ j ] == 255 )
		{
			nB[ 2 ] +=1;
		}
	}

	/* F - G 사이 객체*/
	nFirstPosH	= nHalfH + (( nBottom - nHalfH )/2 );
	for( j = nLeft ; j<= nRight ; j++ )
	{
		if( ptrImage->ptrSrcImage2D[ nFirstPosH ][ j ] == 255 )
		{
			nB[ 3 ] +=1;
		}
	}	
	if( ChkFeatureCnt( 4 ) != ERR_SUCCESS ) return ERR_OCR_CHAR_SIZE;
	ptrFeature->Feature[ m_nLastFeaturePos++ ] = nB[0];
	ptrFeature->Feature[ m_nLastFeaturePos++ ] = nB[1];	
	ptrFeature->Feature[ m_nLastFeaturePos++ ] = nB[2];
	ptrFeature->Feature[ m_nLastFeaturePos++ ] = nB[3];
#endif


	/*===========================================================================	
	수직, 수평 Projection 처리 
	============================================================================*/
	memset( m_ptrODRHPrj, 0, sizeof( int ) * m_ptrocr_conf->OCR_CHAR_H );
	memset( m_ptrODRWPrj, 0, sizeof( int ) * m_ptrocr_conf->OCR_CHAR_W );
	
	HLP_ProjectImg( m_ptrODRHPrj, ptrImage,  &OrgRect, H_LINE_EDGE );
	HLP_ProjectImg( m_ptrODRWPrj, ptrImage,  &OrgRect, W_LINE_EDGE );

#if 1
	nFirstPosH	= nHeight/3;	
	nFirstPosW  = nWidth /3;
	
	/*===========================================================================
	STEP 6
	수직 Projection에서의 초, 중, 종 영역에서의 객체 수
	============================================================================*/	
	/* 초성 영역 */
	if( ChkFeatureCnt( 3 ) != ERR_SUCCESS ) return ERR_OCR_CHAR_SIZE;
	GetProjectCnt( nTop, nFirstPosH, &nSmallCnt, &nMidCnt,&nLargeCnt , m_ptrODRHPrj);
	ptrFeature->Feature[ m_nLastFeaturePos++ ] = (float)nLargeCnt/10;
	ptrFeature->Feature[ m_nLastFeaturePos++ ] = (float)nMidCnt/10;
	ptrFeature->Feature[ m_nLastFeaturePos++ ] = (float)nSmallCnt/10;

	/* 중성 영역 */	
	if( ChkFeatureCnt( 3 ) != ERR_SUCCESS ) return ERR_OCR_CHAR_SIZE;
	GetProjectCnt( nFirstPosH, nFirstPosH*2, &nSmallCnt, &nMidCnt,&nLargeCnt , m_ptrODRHPrj);
	ptrFeature->Feature[ m_nLastFeaturePos++ ] = (float)nLargeCnt/10;	
	ptrFeature->Feature[ m_nLastFeaturePos++ ] = (float)nMidCnt/10;	
	ptrFeature->Feature[ m_nLastFeaturePos++ ] = (float)nSmallCnt/10;

	/* 종성 영역 */	
	if( ChkFeatureCnt( 3 ) != ERR_SUCCESS ) return ERR_OCR_CHAR_SIZE;
	GetProjectCnt( nFirstPosH*2, nBottom, &nSmallCnt, &nMidCnt,&nLargeCnt , m_ptrODRHPrj);
	ptrFeature->Feature[ m_nLastFeaturePos++ ] = (float)nLargeCnt/10;	
	ptrFeature->Feature[ m_nLastFeaturePos++ ] = (float)nMidCnt/10;	
	ptrFeature->Feature[ m_nLastFeaturePos++ ] = (float)nSmallCnt/10;

	/*===========================================================================
	STEP 7
	수평 Projection에서의 초, 중, 종 영역에서의 객체 수
	============================================================================*/	
	/* 초성 영역 */
	if( ChkFeatureCnt( 3 ) != ERR_SUCCESS ) return ERR_OCR_CHAR_SIZE;
	GetProjectCnt( nLeft, nFirstPosW, &nSmallCnt, &nMidCnt,&nLargeCnt ,m_ptrODRWPrj);
	ptrFeature->Feature[ m_nLastFeaturePos++ ] = (float)nLargeCnt/10;
	ptrFeature->Feature[ m_nLastFeaturePos++ ] = (float)nMidCnt/10;
	ptrFeature->Feature[ m_nLastFeaturePos++ ] =(float) nSmallCnt/10;
	/* 중성 영역 */
	if( ChkFeatureCnt( 3 ) != ERR_SUCCESS ) return ERR_OCR_CHAR_SIZE;
	GetProjectCnt( nFirstPosW, nFirstPosW*2, &nSmallCnt, &nMidCnt,&nLargeCnt ,m_ptrODRWPrj);
	ptrFeature->Feature[ m_nLastFeaturePos++ ] = (float)nLargeCnt/10;
	ptrFeature->Feature[ m_nLastFeaturePos++ ] = (float)nMidCnt/10;
	ptrFeature->Feature[ m_nLastFeaturePos++ ] = (float)nSmallCnt/10;
	/* 종성 영역 */
	if( ChkFeatureCnt( 3 ) != ERR_SUCCESS ) return ERR_OCR_CHAR_SIZE;
	GetProjectCnt( nFirstPosW*2, nRight, &nSmallCnt, &nMidCnt, &nLargeCnt ,m_ptrODRWPrj);
	ptrFeature->Feature[ m_nLastFeaturePos++ ] = (float)nLargeCnt/10;
	ptrFeature->Feature[ m_nLastFeaturePos++ ] = (float)nMidCnt/10;
	ptrFeature->Feature[ m_nLastFeaturePos++ ] = (float)nSmallCnt/10;
#endif


#if 0
	/*===========================================================================
	STEP 8
	수평 Projection 
	============================================================================*/	
	if( ChkFeatureCnt( m_ptrocr_conf->OCR_CHAR_W ) != ERR_SUCCESS ) return ERR_OCR_CHAR_SIZE;
	for( i = 0 ; i < m_ptrocr_conf->OCR_CHAR_W ; i++ )
	{
		ptrFeature->Feature[ m_nLastFeaturePos++ ] = *( m_ptrODRWPrj + i );	
	}
	/*===========================================================================
	STEP 10
	수직 Projection 
	============================================================================*/	
	if( ChkFeatureCnt( m_ptrocr_conf->OCR_CHAR_H ) != ERR_SUCCESS ) return ERR_OCR_CHAR_SIZE;
	for( i = 0 ; i < m_ptrocr_conf->OCR_CHAR_H ; i++ )
	{
		ptrFeature->Feature[ m_nLastFeaturePos++ ] = *( m_ptrODRHPrj + i );
	}
#endif

	/* =============================================================
	Feature Log 
	===============================================================*/
	if ( m_ptrocr_conf != NULL && m_ptrocr_conf->OCR_DBG_SHOW_LOG )
	{
		HLP_print_dbg( DBG_LINE, NULL);
		HLP_print_dbg( DBG_INFO, "MOR FEATURE - F_CNT[%02d]", m_nLastFeaturePos );
	}


	return ERR_SUCCESS;
}


static double GeometricMoment( pIMAGE_DATA ptrImage, int p, int q)
{
	int i, j, k;
	int nWidth, nHeight;	
	double moment, temp;

	nWidth = ptrImage->nW;
	nHeight = ptrImage->nH;
	moment =0.0;

	for( i = 0 ; i < nHeight ; i++ )
	{
		for( j = 0 ; j < nWidth ; j++ )
		{
			temp = 1;

			for( k = 0 ; k < p ; k++ ) temp *= j;
			for( k = 0 ; k < q ; k++ ) temp *= i;

			moment += (temp*ptrImage->ptrSrcImage2D[i][j]);
		}
	}
	return moment;
}


static int  InvariantMoment_Feature( pIMAGE_DATA ptrImage, pOCR_FEATURE ptrFeature )
{
	int i;
	double g00, g10, g01, g20, g02, g11, g30, g03, g21, g12;
	double c00, c20, c02, c11, c30, c03, c21, c12;
	double n20, n02, n11, n30, n03, n21, n12;

	double cx;
	double cy;
	double in_moment[ INVARIANT_MOMAN_CNT ];

	memset( &in_moment, 0, sizeof( in_moment ));
	/* =============================================================
	 기하학적 모멘트 구하기
	===============================================================*/
	g00 = GeometricMoment( ptrImage, 0, 0);
	g10 = GeometricMoment( ptrImage, 1, 0);
	g01 = GeometricMoment( ptrImage, 0, 1);
	g20 = GeometricMoment( ptrImage, 2, 0);
	g02 = GeometricMoment( ptrImage, 0, 2);
	g11 = GeometricMoment( ptrImage, 1, 1);
	g30 = GeometricMoment( ptrImage, 3, 0);
	g03 = GeometricMoment( ptrImage, 0, 3);
	g21 = GeometricMoment( ptrImage, 2, 1);
	g12 = GeometricMoment( ptrImage, 1, 2);

	/* =============================================================
	 중심 모멘트 구하기
	===============================================================*/
	cx = g10 / g00;
	cy = g01 / g00;

	c00 = g00;
	c20 = g20 - cx*g10;
	c02 = g02 - cy*g01;
	c11 = g11 - cx*g01;
	c30 = g30 - 3*cx*g20 + 2*cx*cx*g10;
	c03 = g03 - 3*cy*g02 + 2*cy*cy*g01;
	c21 = g21 - 2*cx*g11 - cy*g20 + 2*cx*cx*g01;
	c12 = g12 - 2*cy*g11 - cx*g02 + 2*cy*cy*g10;
	
	/* =============================================================
	 정규화된 중심 모멘트
	===============================================================*/
	n20 = c20 / pow(c00, 2.);
	n02 = c02 / pow(c00, 2.);
	n11 = c11 / pow(c00, 2.);
	n30 = c30 / pow(c00, 2.5);
	n03 = c03 / pow(c00, 2.5);
	n21 = c21 / pow(c00, 2.5);
	n12 = c12 / pow(c00, 2.5);

	
	/* =============================================================
	 불변 모멘트 구하기
	===============================================================*/
	in_moment[0] = n20 + n02;
	in_moment[1] = (n20 - n02)*(n20 - n02) + 4*n11*n11;
	in_moment[2] = (n30 - 3*n12)*(n30 - 3*n12) + (3*n21 - n03)*(3*n21 - n03);
	in_moment[3] = (n30 + n12)*(n30 + n12) + (n21 + n03)*(n21 + n03);
	in_moment[4] = (n30 - 3*n12)*(n30 + n12)*((n30 + n12)*(n30 + n12) - 3*(n21 + n03)*(n21 + n03)) + (3*n21 - n03)*(n21 + n03)*(3*(n30 + n12)*(n30 + n12) - (n21 + n03)*(n21 + n03));
	in_moment[5] = (n20 - n02)*((n30 + n12)*(n30 + n12) - (n21 + n03)*(n21 + n03))+ 4*n11*(n30 + n12)*(n21 + n03);
	in_moment[6] = (3*n21 - n03)*(n30 + n12)*((n30 + n12)*(n30 + n12) - 3*(n21 + n03)*(n21 + n03)) + (3*n12 - n30)*(n21 + n03)*(3*(n30 + n12)*(n30 + n12) - (n21 + n03)*(n21 + n03));
	
	/* =============================================================
	 불변 모멘트값이 너무 작기 때문에 * 1000을 한다.
	 불변 모멘트값 중 0, 1 까지만 유효하다. 
	===============================================================*/
	if( ChkFeatureCnt( MOMANT_INPUT_CNT ) != ERR_SUCCESS ) return ERR_OCR_CHAR_SIZE;	
	for( i = 0 ; i < MOMANT_INPUT_CNT ; i++ )
	{	
		ptrFeature->Feature[ m_nLastFeaturePos++ ] = in_moment[ i ] * 1000;
	}	
	
	/* =============================================================
	Feature Log 
	===============================================================*/
	if ( m_ptrocr_conf != NULL && m_ptrocr_conf->OCR_DBG_SHOW_LOG )
	{
		HLP_print_dbg( DBG_LINE, NULL);
		HLP_print_dbg( DBG_INFO, "FEATURE - F_CNT[%02d]", m_nLastFeaturePos );
	}	

	return ERR_SUCCESS;
}


static int Haar_like( CVRECT_t * ptrRect, pIMAGE_DATA ptrImage, pOCR_FEATURE ptrFeature)
{
	
	int i, j;
	int nInter, nWeight;
	int nPosH1, nPosH2, nPosH3;
	int nPosW1, nPosW2, nPosW3;
	int nDark,nBright ;
	int HaarFeature[ HAAR_LIKE_CNT + 1];
	int nHaarCnt =0;

	memset( &HaarFeature, 0, sizeof( HaarFeature ));
	nWeight = 1;
	/* =============================================================
	Haar like - X2 Mask
	===============================================================*/	
	nBright = nDark = 0;
	nInter = ( ptrRect->nRight- ptrRect->nLeft ) / 2;	
	nPosW1 = ptrRect->nLeft+ nInter;

	for( i = ptrRect->nTop ; i < ptrRect->nBottom ; i++ )
	{
		for( j = ptrRect->nLeft ; j < ptrRect->nRight ; j++ )
		{
			if ( j >= ptrRect->nLeft && j < nPosW1  )
			{
				nDark += ptrImage->ptrSrcImage2D[ i ][ j ];
			}
			else
			{
				nBright += ptrImage->ptrSrcImage2D[ i ][ j ];
			}
		}
	}
	
	/* 영역의 비율(1:1)이 맞으므로 가중치(nWeight)에 1을 넣는다 */
	nWeight = 1;
	nDark	= ( nDark/255 ) * nWeight;
	nBright = nBright/255;	
	HaarFeature[ nHaarCnt++ ] = ( nBright - nDark );

	/* =============================================================
	Haar like - X3 Mask
	===============================================================*/
	nBright = nDark = 0;
	nInter = ( ptrRect->nRight- ptrRect->nLeft ) / 3;	
	nPosW1 = ptrRect->nLeft+ nInter;
	nPosW2 = nPosW1 + nInter;

	for( i = ptrRect->nTop ; i < ptrRect->nBottom ; i++ )
	{
		for( j = ptrRect->nLeft ; j < ptrRect->nRight ; j++ )
		{
			if ( j >= ptrRect->nLeft && j < nPosW1  )
			{
				nBright += ptrImage->ptrSrcImage2D[ i ][ j ];
			}
			else if ( j >= nPosW1 && j < nPosW2 )
			{
				nDark += ptrImage->ptrSrcImage2D[ i ][ j ];
			}
			else
			{
				nBright += ptrImage->ptrSrcImage2D[ i ][ j ];
			}
		}
	}
	
	/* 영역의 비율(2:1)이 맞지 않으므로 가중치(nWeight)를 넣는다 */
	//nWeight = 2;
	nDark	= ( nDark/255 ) * nWeight;
	nBright = nBright/255;
	HaarFeature[ nHaarCnt++ ] = ( nBright - nDark );

	/* =============================================================
	Haar like - X4 Mask
	===============================================================*/
	nBright = nDark = 0;
	nInter = ( ptrRect->nRight- ptrRect->nLeft ) / 4;	
	nPosW1 = ptrRect->nLeft+ nInter;
	nPosW2 = nPosW1 + nInter;
	nPosW3 = nPosW2 + nInter;

	for( i = ptrRect->nTop ; i < ptrRect->nBottom ; i++ )
	{
		for( j = ptrRect->nLeft ; j < ptrRect->nRight ; j++ )
		{
			if ( j >= ptrRect->nLeft && j < nPosW1  )
			{
				nBright += ptrImage->ptrSrcImage2D[ i ][ j ];
			}
			else if ( j >= nPosW1 && j < nPosW3 )
			{
				nDark += ptrImage->ptrSrcImage2D[ i ][ j ];
			}
			else
			{
				nBright += ptrImage->ptrSrcImage2D[ i ][ j ];
			}
		}
	}

	/* 영역의 비율(1:1)이 맞으므로 가중치(nWeight)에 1을 넣는다 */
	nWeight = 1;
	nDark	= ( nDark/255 ) * nWeight;
	nBright = nBright/255;
	HaarFeature[ nHaarCnt++ ] = ( nBright - nDark );

	/* =============================================================
	Haar like - Y2 Mask
	===============================================================*/
	nBright = nDark = 0;
	nInter = ( ptrRect->nBottom- ptrRect->nTop ) / 2;	
	nPosH1 = ptrRect->nTop+ nInter;
	
	for( j = ptrRect->nLeft ; j < ptrRect->nRight ; j++ )
	{
		for( i = ptrRect->nTop ; i < ptrRect->nBottom ; i++ )
		{
			if ( i >= ptrRect->nTop && i < nPosH1  )
			{
				nDark += ptrImage->ptrSrcImage2D[ i ][ j ];
			}			
			else
			{
				nBright += ptrImage->ptrSrcImage2D[ i ][ j ];
			}
		}
	}

	/* 영역의 비율(1:1)이 맞으므로 가중치(nWeight)에 1을 넣는다 */
	nWeight = 1;
	nDark	= ( nDark/255 ) * nWeight;
	nBright = nBright/255;
	HaarFeature[ nHaarCnt++ ] = ( nBright - nDark );

	/* =============================================================
	Haar like - Y3 Mask
	===============================================================*/
	nBright = nDark = 0;
	nInter = ( ptrRect->nBottom- ptrRect->nTop ) / 3;	
	nPosH1 = ptrRect->nTop+ nInter;
	nPosH2 = nPosH1+ nInter;
	
	for( j = ptrRect->nLeft ; j < ptrRect->nRight ; j++ )
	{
		for( i = ptrRect->nTop ; i < ptrRect->nBottom ; i++ )
		{
			if ( i >= ptrRect->nTop && i < nPosH1  )
			{
				nBright += ptrImage->ptrSrcImage2D[ i ][ j ];
			}			
			else if ( i >= nPosH1 && i < nPosH2  )
			{
				nDark += ptrImage->ptrSrcImage2D[ i ][ j ];
			}
			else
			{
				nBright += ptrImage->ptrSrcImage2D[ i ][ j ];
			}
		}
	}

	/* 영역의 비율(2:1)이 맞으므로 가중치(nWeight)에 1을 넣는다 */
	//nWeight = 2;
	nDark	= ( nDark/255 ) * nWeight;
	nBright = nBright/255;
	HaarFeature[ nHaarCnt++ ] = ( nBright - nDark );
	
	/* =============================================================
	Haar like - Y4 Mask
	===============================================================*/
	nBright = nDark = 0;
	nInter = ( ptrRect->nBottom- ptrRect->nTop ) / 4;	
	nPosH1 = ptrRect->nTop+ nInter;
	nPosH2 = nPosH1+ nInter;
	nPosH3 = nPosH2+ nInter;

	for( j = ptrRect->nLeft ; j < ptrRect->nRight ; j++ )
	{
		for( i = ptrRect->nTop ; i < ptrRect->nBottom ; i++ )
		{
			if ( i >= ptrRect->nTop && i < nPosH1  )
			{
				nBright += ptrImage->ptrSrcImage2D[ i ][ j ];
			}			
			else if ( i >= nPosH1 && i < nPosH3  )
			{
				nDark += ptrImage->ptrSrcImage2D[ i ][ j ];
			}
			else
			{
				nBright += ptrImage->ptrSrcImage2D[ i ][ j ];
			}
		}
	}

	/* 영역의 비율(1:1)이 맞으므로 가중치(nWeight)에 1을 넣는다 */
	nWeight = 1;
	nDark	= ( nDark/255 ) * nWeight;
	nBright = nBright/255;
	HaarFeature[ nHaarCnt++ ] = ( nBright - nDark );

	/* =============================================================
	Haar like - X2Y2 Mask
	===============================================================*/
	nBright = nDark = 0;

	nPosH1 = ptrRect->nTop + (( ptrRect->nBottom - ptrRect->nTop ) / 2 );
	nPosW1 = ptrRect->nLeft+ (( ptrRect->nRight - ptrRect->nLeft ) / 2 );
	
	
	for( i = ptrRect->nTop ; i < ptrRect->nBottom ; i++ )
	{
		for( j = ptrRect->nLeft ; j < ptrRect->nRight ; j++ )
		{
			if ( i>= ptrRect->nTop && i < nPosH1 )
			{
				if ( j >= ptrRect->nLeft && j < nPosW1 )
				{
					nDark += ptrImage->ptrSrcImage2D[ i ][ j ];
				}
				else
				{
					nBright += ptrImage->ptrSrcImage2D[ i ][ j ];
				}
			}
			else
			{
				if ( j >= ptrRect->nLeft && j < nPosW1 )
				{
					nBright += ptrImage->ptrSrcImage2D[ i ][ j ];
				}
				else
				{
					nDark += ptrImage->ptrSrcImage2D[ i ][ j ];
				}
			}			
		}
	}

	/* 영역의 비율(1:1)이 맞으므로 가중치(nWeight)에 1을 넣는다 */
	nWeight = 1;
	nDark	= ( nDark/255 ) * nWeight;
	nBright = nBright/255;
	HaarFeature[ nHaarCnt++ ] = ( nBright - nDark );

	/* =============================================================
	Haar like -X3Y3 Mask
	===============================================================*/
	nBright = nDark = 0;
	
	nInter = (( ptrRect->nBottom - ptrRect->nTop ) / 2 );
	nPosH1 = ptrRect->nTop + nInter;
	nPosH2 = nPosH1 + nInter;

	nInter = (( ptrRect->nRight - ptrRect->nLeft ) / 2 );
	nPosW1 = ptrRect->nLeft + nInter;
	nPosW2 = nPosW1 + nInter;
	
	for( i = ptrRect->nTop ; i < ptrRect->nBottom ; i++ )
	{
		for( j = ptrRect->nLeft ; j < ptrRect->nRight ; j++ )
		{
			if ( i>= nPosH1 && i < nPosH2 )
			{
				if ( j >= nPosW1 && j < nPosW2 )
				{
					nDark += ptrImage->ptrSrcImage2D[ i ][ j ];
				}
				else
				{
					nBright += ptrImage->ptrSrcImage2D[ i ][ j ];
				}
			}
			else
			{	
				nBright += ptrImage->ptrSrcImage2D[ i ][ j ];
			}			
		}
	}

	/* 영역의 비율(8:1)이 맞으므로 가중치(nWeight)에 8을 넣는다 */
	//nWeight = 8;
	nDark	= ( nDark/255 ) * nWeight;
	nBright = nBright/255;
	HaarFeature[ nHaarCnt++ ] = ( nBright - nDark );

	/* =============================================================
	Feature Vector 에 등록 
	===============================================================*/
	if( ChkFeatureCnt( HAAR_LIKE_CNT ) != ERR_SUCCESS ) return ERR_OCR_CHAR_SIZE;
	for( i = 0 ; i < HAAR_LIKE_CNT ; i++ )
	{
		ptrFeature->Feature[ m_nLastFeaturePos++ ] = (float)( HaarFeature[ i ] ) / 10 ;
	}

	return ERR_SUCCESS;
}

static int  Haar_like_Feature( pIMAGE_DATA ptrImage, pOCR_FEATURE ptrFeature )
{
	CVRECT_t HaarRect;	
	int i, j, nSepH, nSepW;
	int nResult = ERR_SUCCESS;

	memset( &HaarRect, 0, sizeof( CVRECT_t));
	
	nSepH = ptrImage->nH /3;
	nSepW = ptrImage->nW /3;
	
	/* =============================================================
	 전체 영역에 대한 HAAR LIKE MASK
	===============================================================*/
	HaarRect.nTop		= 0;
	HaarRect.nLeft		= 0;
	HaarRect.nRight		= ptrImage->nW;
	HaarRect.nBottom	= ptrImage->nH;

	nResult = Haar_like( &HaarRect, ptrImage, ptrFeature);
	
	/* =============================================================
	 각 영역( 9등방 영역 )에 대한 HAAR LIKE MASK
	===============================================================*/
	if ( nResult == ERR_SUCCESS )
	{
		for( i = 0 ; i < ptrImage->nH ;  i += nSepH )
		{
			for( j = 0 ; j < ptrImage->nW ; j+= nSepW )
			{

				HaarRect.nTop		= i;
				HaarRect.nLeft		= j;
				HaarRect.nRight		= j + nSepW;
				HaarRect.nBottom	= i + nSepH ;

				if ( HaarRect.nRight > ptrImage->nW  || HaarRect.nBottom > ptrImage->nH) 
				{
					break;
				}

				if ( Haar_like( &HaarRect, ptrImage, ptrFeature) != ERR_SUCCESS )
				{
					nResult = ERR_INVALID_DATA;
					break;
				}
			}
		}
	}

	/* =============================================================
	Feature Log 
	===============================================================*/
	if ( m_ptrocr_conf != NULL && m_ptrocr_conf->OCR_DBG_SHOW_LOG )
	{
		HLP_print_dbg( DBG_LINE, NULL);
		HLP_print_dbg( DBG_INFO, "FEATURE - F_CNT[%02d]", m_nLastFeaturePos );
	}

	return nResult;
}

/*=========================================================================
특징 추출 초기화
==========================================================================*/
int FT_Initialize( void )
{
	if( MakeTempPrj( m_ptrocr_conf->OCR_CHAR_H, m_ptrocr_conf->OCR_CHAR_W ) != ERR_SUCCESS )
		return ERR_ALLOC_BUF;

	return ERR_SUCCESS;
}

/*=========================================================================
객체 특징점 추출 
==========================================================================*/
int FT_SetFeatureImage( pIMAGE_DATA ptrImage, pOCR_FEATURE ptrFeature , int nOCR_CODE )
{
	int nResult = ERR_SUCCESS;

	if ( nOCR_CODE >= 0 )
	{
		ptrFeature->OCR_CODE = ( BYTE )nOCR_CODE;
	}	
	m_nLastFeaturePos =0;

	/*===========================================================================
	[ 특징점 추출 알고리즘 ]	
	============================================================================*/
	nResult = Morphology_Feature( ptrImage, ptrFeature );

	if ( nResult == ERR_SUCCESS )
	{
		nResult = Haar_like_Feature( ptrImage, ptrFeature );
	}

	if ( nResult == ERR_SUCCESS )
	{
		nResult = InvariantMoment_Feature( ptrImage, ptrFeature );
	}	

	return nResult;
}

/*=========================================================================
OCR 설정 정보 
==========================================================================*/
int FT_SetOCRConf( pOCR_CONF ptrOCRConf )
{
	m_ptrocr_conf = ptrOCRConf; 

	return ERR_SUCCESS;
}

/*=========================================================================
객체 특징점 릴리즈
==========================================================================*/
int FT_Release( void )
{
	ReleaseTempPrj();

	return ERR_SUCCESS;
}