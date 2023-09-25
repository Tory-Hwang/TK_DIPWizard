/*================================================================================
Copyright (c) 2011 Tory Hwang 
Modify : Tory in 2013 .01
Mail : tory45@empal.com
*=================================================================================*/
#include "stdafx.h"
#include <malloc.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "../include/TK_TYPE.h"
#include "../include/TK_LIB_Helper.h"

#include "DIP_Helper.h"


static int ** m_ptrEq_tbl= NULL;


static int Internal_DoLabeling4Neighbour( pIMAGE_DATA ptrImageData);
static int Internal_DoLabeling8Neighbour( pIMAGE_DATA ptrImageData);

/*======================================================================
4이웃 Labeling 기법
========================================================================*/
static int Internal_DoLabeling4Neighbour( pIMAGE_DATA ptrImageData)
{
	int i, j;
	int ** ptrMap = NULL;	
	int nLabel, nMax, nMin, nMIn_eq, nMax_eq;
	
	int nTopLabel, nLeftLabel;
	int nTemp=0;

	int * ptrnHash = NULL;	
	int nCnt = 0;

	nLabel =0;

	ptrMap = HLP_IntAllocMatrix( ptrImageData->nH, ptrImageData->nW );
	if ( ptrMap == NULL )
		return ERR_ALLOC_BUF;

	/*첫번째 스캔을 통한 등가 테이블 생성 및 초기 레이블 지정*/
	for( i = 0 ; i < ptrImageData->nH ; i++ )
	{
		for( j=0 ; j < ptrImageData->nW ;j++ )
		{
			if ( ptrImageData->ptrSrcImage2D[ i ][ j ] != 0 )
			{

				if ( i == 0  && j == 0 )
				{
					nTopLabel = 0;
					nLeftLabel = 0;
				}
				else if ( i == 0  )
				{
					nTopLabel = 0;
					nLeftLabel = ptrMap[ i ][ j -1 ];
				}
				else if ( j == 0 )
				{
					nLeftLabel = 0;
					nTopLabel = ptrMap[ i -1 ][ j ];
				}
				else
				{
					nTopLabel = ptrMap[ i -1][ j ];
					nLeftLabel = ptrMap[ i ][ j -1 ];
				}
				/* 위쪽, 왼쪽에 레이블이 있을 경우 */
				if ( nTopLabel != 0 && nLeftLabel !=0 )
				{
					/* 두label이 동일 할 경우 */
					if ( nTopLabel == nLeftLabel )
					{
						ptrMap[ i ][ j ] = nTopLabel;
						
					}
					/* 두 label이 서로 다를 경우 */
					else
					{
						nMax = max(nTopLabel, nLeftLabel);
						nMin = min(nTopLabel, nLeftLabel);

						ptrMap[ i ][ j ] = nMin;

						/* 등가 테이블 재 구성 */
						nMax_eq = max( m_ptrEq_tbl[ nMax ][ 1 ], m_ptrEq_tbl[ nMin ][ 1 ]);
						nMIn_eq = min( m_ptrEq_tbl[ nMax ][ 1 ], m_ptrEq_tbl[ nMin ][ 1 ]);
						
						/* 두개의 등가 테이블에서 큰 등가 테이블을 값을 작은 등가 테이블 값으로 변경 */
						//m_ptrEq_tbl[ m_ptrEq_tbl[ nMax_eq ][ 1 ] ][ 1 ] = nMIn_eq;
						m_ptrEq_tbl[ nMax][ 1 ] = nMIn_eq;
						m_ptrEq_tbl[ nMin ][ 1 ] = nMIn_eq;

					}
				}
				/* 위쪽에만 레이블이 있을 경우 */
				else if ( nTopLabel != 0 )
				{
					ptrMap[ i ][ j ] = nTopLabel;
				}
				/* 왼쪽에만 레이블이 있을 경우 */
				else if ( ptrMap[ i ][ j -1 ] != 0 )
				{
					ptrMap[ i ][ j ] = nLeftLabel;					
				}
				/* 이웃에 레이블이 없을 경우 */
				else
				{					
					if ( nLabel >= MAX_LABEL_CNT )
					{
						goto __errproc;
					}

					
					ptrMap[ i ][ j ] = nLabel;					
					m_ptrEq_tbl[ nLabel ][ 0 ]= nLabel;
					m_ptrEq_tbl[ nLabel ][ 1 ] =nLabel;
					nLabel++;
				}
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

	ptrnHash = new int [ nLabel ];
	if ( ptrnHash == NULL )
		goto __errproc;

	memset( ptrnHash, 0, sizeof( int ) * ( nLabel ));

	/* 재 조정된 등가 테이블에 존재하는 객체의 번호만( 동일 label은 같은 저장소) 저장 */
	for( i = 0 ; i < nLabel ; i++ )
	{
		ptrnHash[ m_ptrEq_tbl[ i ][ 1 ] ] = m_ptrEq_tbl[ i ][ 1 ];
	}
	
	/* 등가 테이블의 객체 번호를 1번부터 재 정의 */
	for( i = 0 ; i <nLabel ; i++ )
		if ( ptrnHash[ i ] != 0 )
			ptrnHash[ i ] = ++nCnt;
	
	for( i =0 ; i < nLabel ; i++ )
		m_ptrEq_tbl[ i ][ 1 ] = ptrnHash[ m_ptrEq_tbl[i][1] ];


	/*두번째 스캔에 의한 모든 픽셀의 고유 레이블 부여 */
	for( i = 0 ; i < ptrImageData->nH ; i++ )
	{		
		for( j = 0; j < ptrImageData->nW; j++ )
		{			
			nTemp = ptrMap[ i ][ j ];
			//ptrImageData->ptrSrcImage2D[ i ][ j ] = ( BYTE)(m_ptrEq_tbl[ nTemp][ 1 ]);			
		}
	}
	
	
	HLP_IntFreeMatrix( ptrMap , ptrImageData->nH, ptrImageData->nW );
	if ( ptrnHash != NULL )
		delete [] ptrnHash;

	return nCnt;

__errproc:
	HLP_IntFreeMatrix( ptrMap , ptrImageData->nH, ptrImageData->nW );
	if ( ptrnHash != NULL )
		delete [] ptrnHash;

	return ERR_INVALID_DATA;
}

/*=====================================================================
Expert Function
======================================================================*/
int DoLabeling( pIMAGE_DATA ptrImageData )
{
	if ( m_ptrEq_tbl == NULL )
	{
		m_ptrEq_tbl = HLP_IntAllocMatrix( MAX_LABEL_CNT, 2 );
	}
	else
	{
		for(int y = 0 ; y < MAX_LABEL_CNT ; y++)
		{
			m_ptrEq_tbl[y] = new int[2];		
			if ( m_ptrEq_tbl[ y ] == NULL )
				return NULL;

			memset( m_ptrEq_tbl[ y ], 0, sizeof( int) * 2 );
		}
	}
	
	return Internal_DoLabeling4Neighbour( ptrImageData );
}

int ReleaseLabeling( void )
{	
	if ( m_ptrEq_tbl != NULL )
	{
		HLP_IntFreeMatrix( m_ptrEq_tbl, MAX_LABEL_CNT, 2 );
		m_ptrEq_tbl = NULL;
	}
	return ERR_SUCCESS;
}

/*======================================================================
영상 이진화( Otsu ) 처리 시 사용 
========================================================================*/
float GetPx( int nInit, int nEnd, int * ptrHist )
{
	int nSum = 0;
	int i;

	for (i = nInit; i <= nEnd; i++)
		nSum += ptrHist[ i ];

	return (float)nSum;
}

float GetMean( int nInit, int nEnd, int * ptrHist )
{
	int nSum = 0;
	int i;
	
	for (i = nInit; i <= nEnd; i++)
		nSum += i * ptrHist[ i ];

	return (float)nSum;
}

int GetMax( float * ptrVector, int nCnt )
{
	float fmaxVec = 0.;
	int nidx=0;
	int i;

	for (i = 1; i < nCnt - 1; i++)
	{
		if (ptrVector[ i ] > fmaxVec)
		{
			fmaxVec = ptrVector[ i ];
			nidx = i;
		}
	}

	return nidx;

}
