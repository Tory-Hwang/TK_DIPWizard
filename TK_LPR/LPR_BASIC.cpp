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



/*================================================================================
External Functions 
=================================================================================*/
int BASIC_Initialize( void )
{
	return ERR_SUCCESS;
}

/*======================================================================
�⺻ ���� ����

������ ��濡 ��� ���ڷ� �̷���� �����Ƿ� ������ DIP ó�� ���� 
Projection�� ���� ������ �ٷ� �����Ѵ�. 
========================================================================*/
int BASIC_GetRecognition( pLPR_CONF ptrLPRConf, pOCR_CONF ptrOCRConf, pDIP_DATA ptrDIPData, pIMAGE_DATA ptrDestImageData, 
						  pIMAGE_DATA ptrChgImageData, pIMAGE_DATA ptrImageDataSrc, pCVRECT ptrPlateRect)
{
	int i, nIdex, nValidCnt =0;
	int nPos, nHeight, nWidth;
	int nLeft, nRight, nTop, nBottom;

	( void )ptrChgImageData;
	( void )ptrDestImageData;
	( void )ptrDIPData;
	( void )ptrOCRConf;
	( void )ptrLPRConf;


	int * ptrComWProjection = NULL;
	int * ptrComHProjection = NULL;
	CVRECT_t OrgRect;
	pCOMMONINFO ptrComInfo = NULL;
	pIMAGE_DATA ptrPlateImageData = NULL;

	
	ptrComInfo			= COM_GetInfo();	
	ptrComWProjection	= COM_GetWProjectionBuf();
	ptrComHProjection	= COM_GetHProjectionBuf();
	ptrPlateImageData	= COM_GetTempImageBuf();


	if ( ptrComInfo == NULL || ptrComWProjection == NULL || ptrComHProjection == NULL || ptrPlateImageData == NULL)
	{
		return ERR_INVALID_DATA;
	}

	/*===========================================================================
	[ ��ȿ ��ȣ�� ����ȭ ���� ����� ���� �ʱ�ȭ ]
	============================================================================*/
	COM_Init_RectImageBuf();

	memset( &OrgRect, 0, sizeof( OrgRect ));

	OrgRect.nTop		= 0;
	OrgRect.nLeft		= 0;
	OrgRect.nRight		= ptrImageDataSrc->nW;
	OrgRect.nBottom		= ptrImageDataSrc->nH;

	/*===========================================================================
	[ ���� ó�� ó�� ]	
	============================================================================*/	
	memset( ptrComWProjection, 0, sizeof( int ) * ptrComInfo->nImageWidth );
	memset( ptrComHProjection, 0, sizeof( int ) * ptrComInfo->nImageHeight );
	
	if ( HLP_ProjectImg( ptrComHProjection, ptrImageDataSrc, &OrgRect, H_LINE_EDGE ) != ERR_SUCCESS )
		return ERR_INVALID_DATA;

	if ( HLP_ProjectImg( ptrComWProjection, ptrImageDataSrc, &OrgRect, W_LINE_EDGE ) != ERR_SUCCESS )
		return ERR_INVALID_DATA;

	/*===========================================================================
	[ ���ڿ��� ���� ]	
	============================================================================*/
	nTop = nBottom = nRight = nLeft = 0;
	
	for( i = OrgRect.nTop ; i < OrgRect.nBottom ; i++ )
	{
		if ( *( ptrComHProjection + i ) != 0 ) 
		{
			nTop = i;
			break;
		}
	}

	for( i = OrgRect.nBottom -1 ; i >= OrgRect.nTop ; i-- )
	{
		if ( *( ptrComHProjection + i ) != 0 ) 
		{
			nBottom = i;
			break;
		}
	}

	for( i = OrgRect.nLeft ; i < OrgRect.nRight ; i++ )
	{
		if ( *( ptrComWProjection + i ) != 0 ) 
		{
			nLeft = i;
			break;
		}
	}

	for( i = OrgRect.nRight -1 ; i >= OrgRect.nLeft ; i-- )
	{
		if ( *( ptrComWProjection + i ) != 0 ) 
		{
			nRight = i;
			break;
		}
	}
	
	OrgRect.nLeft	= nLeft-1;
	OrgRect.nRight	= nRight + 2;
	OrgRect.nTop	= nTop -1; 
	OrgRect.nBottom	= nBottom + 1 ;

	if( OrgRect.nRight > ptrImageDataSrc->nW) OrgRect.nRight = ptrImageDataSrc->nW -1;
	if( OrgRect.nBottom > ptrImageDataSrc->nH) OrgRect.nBottom = ptrImageDataSrc->nH -1 ;

	nPos	= 0;
	nWidth	= OrgRect.nRight - OrgRect.nLeft;
	nHeight = OrgRect.nBottom - OrgRect.nTop;

	if( nWidth <= 0 ) nWidth = 0;
	if( nHeight <=0 ) nHeight = 0;

	/*===========================================================================
	[ ���ڿ��� ���� ]	
	============================================================================*/	
	for (int i= OrgRect.nTop; i< OrgRect.nBottom; i++, nPos++)
	{
		memcpy( *( ptrPlateImageData->ptrSrcImage2D + nPos ),  *( ptrImageDataSrc->ptrSrcImage2D + i ) + OrgRect.nLeft , nWidth );			
	}

	/*===========================================================================
	[ ��ȿ ��ȣ�� �ĺ��� ����ȭ ���� ���� ]
	���� ���ҿ� �̹����� ����ϱ� ���ؼ�..
	============================================================================*/	
	ptrPlateImageData->nH = nHeight;
	ptrPlateImageData->nW = nWidth;

	nIdex = COM_CopyRectImage( ptrPlateImageData );
	if ( nIdex >= 0 )
	{	
		OrgRect.nLPR_RectIdx = nIdex;
	}
	else
	{
		OrgRect.nLPR_RectIdx = ERR_ALLOC_BUF;
	}

	if ( nIdex >= 0 )
	{		
		memcpy( ptrPlateRect, &OrgRect, sizeof( CVRECT_t ));
		nValidCnt++;
		ptrPlateRect->nSeqNum =1;
	}

	return nValidCnt;
}

int BASIC_IOCTL( pIOCTL_DATA ptrIOCTLData )
{
	( void )ptrIOCTLData;

	return ERR_SUCCESS;
}

int BASIC_Release( void )
{
	return ERR_SUCCESS;
}