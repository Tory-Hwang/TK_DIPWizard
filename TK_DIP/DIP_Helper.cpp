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
4�̿� Labeling ���
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

	/*ù��° ��ĵ�� ���� � ���̺� ���� �� �ʱ� ���̺� ����*/
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
				/* ����, ���ʿ� ���̺��� ���� ��� */
				if ( nTopLabel != 0 && nLeftLabel !=0 )
				{
					/* ��label�� ���� �� ��� */
					if ( nTopLabel == nLeftLabel )
					{
						ptrMap[ i ][ j ] = nTopLabel;
						
					}
					/* �� label�� ���� �ٸ� ��� */
					else
					{
						nMax = max(nTopLabel, nLeftLabel);
						nMin = min(nTopLabel, nLeftLabel);

						ptrMap[ i ][ j ] = nMin;

						/* � ���̺� �� ���� */
						nMax_eq = max( m_ptrEq_tbl[ nMax ][ 1 ], m_ptrEq_tbl[ nMin ][ 1 ]);
						nMIn_eq = min( m_ptrEq_tbl[ nMax ][ 1 ], m_ptrEq_tbl[ nMin ][ 1 ]);
						
						/* �ΰ��� � ���̺��� ū � ���̺��� ���� ���� � ���̺� ������ ���� */
						//m_ptrEq_tbl[ m_ptrEq_tbl[ nMax_eq ][ 1 ] ][ 1 ] = nMIn_eq;
						m_ptrEq_tbl[ nMax][ 1 ] = nMIn_eq;
						m_ptrEq_tbl[ nMin ][ 1 ] = nMIn_eq;

					}
				}
				/* ���ʿ��� ���̺��� ���� ��� */
				else if ( nTopLabel != 0 )
				{
					ptrMap[ i ][ j ] = nTopLabel;
				}
				/* ���ʿ��� ���̺��� ���� ��� */
				else if ( ptrMap[ i ][ j -1 ] != 0 )
				{
					ptrMap[ i ][ j ] = nLeftLabel;					
				}
				/* �̿��� ���̺��� ���� ��� */
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

	/* � ���̺� ���� */	
	for( i = 0 ; i < nLabel ; i++ )
	{
		nTemp = m_ptrEq_tbl[ i ][ 1 ];

		/*�� ���ǵ� ����̶�� �� ���ǵ� � ��ġ�� ��( �ּҰ� )���� �����Ѵ� */
		if ( nTemp != m_ptrEq_tbl[ i ][ 0 ] )
			m_ptrEq_tbl[ i ][ 1 ] = m_ptrEq_tbl[ nTemp ][ 1 ];
	}

	ptrnHash = new int [ nLabel ];
	if ( ptrnHash == NULL )
		goto __errproc;

	memset( ptrnHash, 0, sizeof( int ) * ( nLabel ));

	/* �� ������ � ���̺� �����ϴ� ��ü�� ��ȣ��( ���� label�� ���� �����) ���� */
	for( i = 0 ; i < nLabel ; i++ )
	{
		ptrnHash[ m_ptrEq_tbl[ i ][ 1 ] ] = m_ptrEq_tbl[ i ][ 1 ];
	}
	
	/* � ���̺��� ��ü ��ȣ�� 1������ �� ���� */
	for( i = 0 ; i <nLabel ; i++ )
		if ( ptrnHash[ i ] != 0 )
			ptrnHash[ i ] = ++nCnt;
	
	for( i =0 ; i < nLabel ; i++ )
		m_ptrEq_tbl[ i ][ 1 ] = ptrnHash[ m_ptrEq_tbl[i][1] ];


	/*�ι�° ��ĵ�� ���� ��� �ȼ��� ���� ���̺� �ο� */
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
���� ����ȭ( Otsu ) ó�� �� ��� 
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
