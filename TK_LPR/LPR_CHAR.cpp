
#include "stdafx.h"
#include <malloc.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "../include/TK_TYPE.h"
#include "../include/TK_LIB_API.h"
#include "../include/TK_LIB_Helper.h"

#include "LPR_DIP.h"
#include "LPR_COMMON.h"
#include "LPR_CHAR.h"

static OCR_DATA_t m_OCRData;

static int MergeHangulCharWRect( int nSegPosH, int nPlateTop, pLPR_CONF ptrLPRConf, pLPRCHAR ptrSrcCharRect );

/*=========================================================================
차량 번호 위치 추출 
==========================================================================*/
static int GetCharRectWithLabel( int nLabelCnt, pIMAGE_DATA ptrImageDataSrc, pCVRECT ptrCharRect )
{
	CVRECT_t *ptrTempRect;
	int i, j;
	int nLabel;	
	int ** ptrComMap2 = NULL;

	ptrComMap2 = COM_GetMap2Buf();	

	if ( ptrCharRect == NULL || ptrComMap2 == NULL )
		return ERR_ALLOC_BUF;


	for( i = 0 ; i < nLabelCnt ; i++ )
	{
		ptrCharRect[ i ].nLeft = ptrImageDataSrc->nW;
		ptrCharRect[ i ].nTop =  ptrImageDataSrc->nH;
		ptrCharRect[ i ].nRight = 0;
		ptrCharRect[ i ].nBottom =0;
	}

	/*===========================================================================
	[ 동일 객체 위치 설정 ]	
	============================================================================*/
	for( i = 0 ; i < ptrImageDataSrc->nH ; i++ )
	{
		for( j = 0; j < ptrImageDataSrc->nW; j++ )
		{
			nLabel = ptrComMap2[ i ][ j ];
			if (  nLabel != 0  && nLabel <= nLabelCnt)
			{
				ptrTempRect= (CVRECT_t *)&ptrCharRect[ nLabel -1 ];

				if ( i < ptrTempRect->nTop ) ptrTempRect->nTop = i;
				if ( i > ptrTempRect->nBottom ) ptrTempRect->nBottom = i;						

				if ( j < ptrTempRect->nLeft ) ptrTempRect->nLeft = j;
				if ( j > ptrTempRect->nRight ) ptrTempRect->nRight = j;					
			}
			//ptrDestImageData->ptrSrcImage2D[ i ][ j ] = 255;
		}
	}

	return nLabelCnt;
}

/*=========================================================================
문자 labeing
==========================================================================*/
static int GenerateCharRect( pIMAGE_DATA ptrImageData , pCVRECT ptrCharRact )
{
	int nLabelCnt =0;

	memset( ptrCharRact, 0, MAX_CHAR_LABEL_CNT * sizeof( CVRECT_t));

	/*===========================================================================
	[ 8이웃 라벨링 ]	
	============================================================================*/
	nLabelCnt = COM_DoLabeling8Neighbour( ptrImageData, 0, ptrImageData->nW, 0, ptrImageData->nH );
	if ( nLabelCnt <= 0 )
		return ERR_INVALID_DATA;

	/*===========================================================================
	[ 차량 번호 위치 추출 ]	
	============================================================================*/
	nLabelCnt = GetCharRectWithLabel(  nLabelCnt, ptrImageData, ptrCharRact);
	if ( nLabelCnt <= 0)
		return ERR_INVALID_DATA;

	return nLabelCnt;
}


static int ExtensionCharRect( pLPR_CONF ptrLPRConf, pCVRECT ptrRect , pIMAGE_DATA ptrImageData)
{	
	int nLeft, nTop, nRight, nBottom;

	ptrRect->nLeft -=1;
	ptrRect->nTop -=1;
	ptrRect->nRight +=1;
	ptrRect->nBottom +=1;

	if ( ptrRect->nLeft < 0 ) ptrRect->nLeft =0;
	if ( ptrRect->nTop < 0 ) ptrRect->nTop =0;
	if( ptrRect->nRight > ptrImageData->nW ) ptrRect->nRight = ptrImageData->nW;
	if( ptrRect->nBottom > ptrImageData->nH ) ptrRect->nBottom = ptrImageData->nH;

	return ERR_SUCCESS;
}

static int VerifyCharRect( pLPR_CONF ptrLPRConf, pCVRECT ptrRect)
{	
	int nWidth, nHeight;

	nWidth = ptrRect->nRight - ptrRect->nLeft;
	nHeight = ptrRect->nBottom - ptrRect->nTop;

	
	if ( ptrRect->nSeqNum <= 0 || nWidth <= 0 || nHeight <= 0 )
	{	
		return ERR_INVALID_DATA;	
	}

	if( ptrLPRConf->LPR_DBG_RUN_BASIC_STUDY )
	{
		if ( nWidth < ptrLPRConf->LPR_MIN_CHAR_W || nHeight < ptrLPRConf->LPR_MIN_CHAR_H - 5 )
		{	
			return ERR_INVALID_DATA;
		}
	}
	else
	{
		if ( nWidth < ptrLPRConf->LPR_MIN_CHAR_W || nHeight < ptrLPRConf->LPR_MIN_CHAR_H )
		{	
			return ERR_INVALID_DATA;
		}
	}

	return ERR_SUCCESS;
}


static int MakeCharHSize( pLPR_CONF ptrLPRConf, pCVRECT  ptrRect, int nMinCnt, int * ptrComHProjection, pIMAGE_DATA ptrImageData)
{
	int i, nTop, nBottom, nLeft, nRight, nWidth, nHeight ;
	
	nTop    = ptrRect->nTop;
	nBottom = ptrRect->nBottom;
	
	for( i = nTop ; i <= nBottom ; i++ )
	{
		if ( i < ptrImageData->nH )
		{
			if (  *( ptrComHProjection + i ) >= nMinCnt  )
			{
				ptrRect->nTop = i;
				break;
			}
		}
	}

	nTop    = ptrRect->nTop;
	nBottom = ptrRect->nBottom;	

	for( i = nBottom  ; i >= nTop ; i-- )
	{
		if ( i < ptrImageData->nH )
		{
			if (  *( ptrComHProjection + i ) >= nMinCnt )
			{
				ptrRect->nBottom = i +1 ;
				break;
			}
		}
	}

	if( ptrLPRConf->LPR_DBG_SHOW_H_LINE_SEG)
	{
		HLP_PartDrawLineRect( ptrImageData, ptrRect, 150 ,H_LINE_EDGE);
	}

	return ERR_SUCCESS;
}

static int MakeCharWSize( pLPR_CONF ptrLPRConf, pCVRECT  ptrRect, int nMinCnt, int nMinWith, int * ptrComWProjection ,pLPRCHAR ptrLPRChars , pIMAGE_DATA ptrImageData)
{
	int i,  nTop, nBottom, nLeft, nRight, nWidth, nHeight ;
	int nTemp;
	BOOL bFlag;
	int nRectCnt =0;	

	CVRECT_t TempRect, * ptrDestRect;


	bFlag = FALSE;	
	nTemp = 0;
	
	nTop    = ptrRect->nTop;
	nBottom = ptrRect->nBottom;
	nLeft	= ptrRect->nLeft;
	nRight  = ptrRect->nRight;
	
	for( i = nLeft ; i <= nRight ; i++ )
	{
		if ( i < ptrImageData->nW )
		{
			if ( !bFlag && ( *( ptrComWProjection + i ) >= nMinCnt ) )
			{
				bFlag = TRUE;
				nTemp = i;
			}

			if ( bFlag && ( *( ptrComWProjection + i ) < nMinCnt ))
			{		
				TempRect.nTop = nTop;
				TempRect.nBottom = nBottom;
				TempRect.nLeft = nTemp;
				TempRect.nRight = i;

				nWidth = TempRect.nRight - TempRect.nLeft;
				nHeight = TempRect.nBottom - TempRect.nTop;
				bFlag = FALSE;

				if ( nWidth >= nMinWith )
				{
					if( ptrLPRConf->LPR_DBG_SHOW_W_LINE_SEG)
					{
						HLP_PartDrawLineRect( ptrImageData, &TempRect, 150 ,W_LINE_EDGE);
					}

					ptrDestRect = &ptrLPRChars->Chars[ ptrLPRChars->nCharCnt ].ChRect;
					memcpy( ptrDestRect, &TempRect, sizeof( CVRECT_t ));
					ptrDestRect->nSeqNum =1;
					ptrLPRChars->nCharCnt++;
					nRectCnt++;
				}
			}
		}
	}
	
	return nRectCnt ;
}


/*=========================================================================
문자의 상,하,좌,우 좌표를 재 설정한다. 
문자 사이 간격이 2픽셀 미만이면 동일 객체로 처리한다. 
==========================================================================*/
static int MergeHangulCharWRect( int nSegPosH, int nPlateTop, pLPR_CONF ptrLPRConf, pLPRCHAR ptrSrcCharRect )
{
	int i ;
	int nCharCnt =0;
	int nInter = 0;
	int nTop, nCurWidth, nNextWidth;
	int nMaxInterSize=0;
	int nMaxMergeSize = 0;
	int nMaxNextWidth = 0;

	CVRECT_t *ptrCurRect, * ptrNextRect;
	nCharCnt = ptrSrcCharRect->nCharCnt;

	if ( ptrLPRConf->LPR_DBG_RUN_BASIC_STUDY)
	{		
		nMaxMergeSize	= ptrLPRConf->LPR_MAX_HG_MG_W * 2;
	}
	else
	{		
		nMaxMergeSize	= ptrLPRConf->LPR_MAX_HG_MG_W;
	}
	
	for(i = 0 ; i < nCharCnt -1 ; i++ )
	{
		ptrCurRect	= &ptrSrcCharRect->Chars[ i ].ChRect;
		ptrNextRect = &ptrSrcCharRect->Chars[ i + 1 ].ChRect;
		
		nCurWidth		= ptrCurRect->nRight - ptrCurRect->nLeft;
		nNextWidth		= ptrNextRect->nRight - ptrNextRect->nLeft;
		nTop			= ptrCurRect->nTop;
		
		if ( ptrCurRect != NULL && ptrNextRect != NULL &&
			 ptrCurRect->nSeqNum > 0 && ptrNextRect->nSeqNum > 0 )
		{
			nInter = ptrNextRect->nLeft - ptrCurRect->nRight;

			if ( nInter > 0 )
			{				
				nMaxNextWidth = nCurWidth - ptrLPRConf->LPR_MIN_PIXEL ;
				
				/*구번호 상위 또는 신번호 */
				if ( ptrCurRect->nTop == nSegPosH || ptrCurRect->nTop < nSegPosH)				
				{
					nMaxInterSize	= ptrLPRConf->LPR_MAX_HG_MG_INT;

					if (ptrLPRConf->LPR_DBG_RUN_BASIC_STUDY   )
					{
						nMaxInterSize	= ptrLPRConf->LPR_MAX_HG_MG_INT+5;
					}

					/* 구 번호판 상단 */ 
					if ( ptrCurRect->nTop < nSegPosH || ptrLPRConf->LPR_DBG_RUN_BASIC_STUDY ) 
					{
						nMaxNextWidth = nCurWidth;
					}
				}
				else
				{
					nMaxInterSize	= ptrLPRConf->LPR_MAX_HG_MG_INT+1;
				}
			}
			
			/* ===================================================================
			1. 현재 문자와 다음 문자의 간격이 0보다 크고 
			2. 간격이 구 / 신 번호판 일반 간격보다 작아야 하고
			3. 현재 문자 넓이가 다음 문자의 넓이보다 커야 하고 
			4. 다음 문자의 넓이는 현재 문자넓이의 -3 보다 작아야 한다.
			   단 구번호 상단 일때는 작으면 된다. 
			====================================================================*/	
			if ( ( nInter > 0 ) &&  
				 ( nInter <= nMaxInterSize ) && 
				 ( nCurWidth <  nMaxMergeSize ) &&
				 ( ( (( nCurWidth >  nNextWidth ) && ( nNextWidth <  nMaxNextWidth ) ) )) )
			{
				ptrCurRect->nRight = ptrNextRect->nRight;

				if ( ptrCurRect->nTop > ptrNextRect->nTop ) ptrCurRect->nTop = ptrNextRect->nTop;
				if ( ptrCurRect->nBottom < ptrNextRect->nBottom ) ptrCurRect->nBottom = ptrNextRect->nBottom;

				ptrNextRect->nSeqNum = -1;
			}
		}
	}

	return ERR_SUCCESS;	
}

/*=========================================================================
차량번호 분리
==========================================================================*/
static int SegmentCharsW( pLPR_CONF ptrLPRConf, pCOMMONINFO ptrComInfo ,int * ptrComHProjection , int * ptrComWProjection , 
						  pCVRECT ptrPlateRect, pIMAGE_DATA ptrImageData,  pLPRCHAR ptrLPRChars, int * ptrSegPosH )
{
	
	int nSegPosH;	
	int nMinCnt;
	int nMinWith;
	int * ptrComWProjection2 = NULL;	
	CVRECT_t PrjRect;
	int i,j;

	ptrComWProjection2 = COM_GetWProjection2Buf();

	if ( ptrComInfo == NULL || ptrComWProjection == NULL || ptrComHProjection == NULL || ptrComWProjection2 == NULL ||
		 ptrPlateRect == NULL || ptrImageData == NULL || ptrLPRChars == NULL )
	{
		return ERR_INVALID_DATA;
	}
	memset( ptrComWProjection, 0, sizeof( int ) * ptrComInfo->nImageWidth );		
	memset( ptrComWProjection2, 0, sizeof( int ) * ptrComInfo->nImageWidth );
	nSegPosH = ptrPlateRect->nTop;

	for( i = ptrPlateRect->nTop ; i <= ptrPlateRect->nBottom ; i++ )
	{
		for( j = ptrPlateRect->nLeft ;j <= ptrPlateRect->nRight; j++ )
		{
			if ( i <  ptrImageData->nH  && j < ptrImageData->nW )
			{
				if ( i <= ptrPlateRect->nTop )
					ptrImageData->ptrSrcImage2D[ i ][ j ] = 0;

				if ( i >= ptrPlateRect->nBottom )
					ptrImageData->ptrSrcImage2D[ i ][ j ] = 0;

				if ( j <= ptrPlateRect->nLeft)
					ptrImageData->ptrSrcImage2D[ i ][ j ] = 0;

				if ( j >= ptrPlateRect->nRight)
					ptrImageData->ptrSrcImage2D[ i ][ j ] = 0;
			}
		}
	}


	nSegPosH = COM_GetSeparationH_MinCnt( ptrLPRConf, ptrComInfo, ptrPlateRect, ptrComHProjection, ptrImageData);

	nMinWith = ptrLPRConf->LPR_MIN_CHAR_W;	
	nMinCnt = ( ptrLPRConf->LPR_MIN_PIXEL / 2 ); 

	if ( ptrLPRConf->LPR_DBG_RUN_BASIC_STUDY )
	{
		nMinWith = ptrLPRConf->LPR_MIN_CHAR_W -2;	
	}
	/*===========================================================================
	[ 상, 하 분리번호판 여부 ]
	nSegPosH > 0 : 상 하 분리된 번호판
	============================================================================*/	
	if ( nSegPosH != ptrPlateRect->nTop )
	{		
		*ptrSegPosH = nSegPosH;
		/*===========================================================================
		[ 상단 가로 분리 ]
		============================================================================*/	
		PrjRect.nTop = 0;
		PrjRect.nBottom = nSegPosH -1;
		PrjRect.nLeft =0;
		PrjRect.nRight = ptrPlateRect->nRight;
		
		HLP_ProjectImg( ptrComWProjection, ptrImageData,  &PrjRect, W_LINE_EDGE );
		MakeCharWSize( ptrLPRConf, &PrjRect,nMinCnt, nMinWith, ptrComWProjection, ptrLPRChars, ptrImageData );
		
		/*===========================================================================
		[ 하단 가로 분리 ]
		============================================================================*/	
		PrjRect.nTop = nSegPosH;
		PrjRect.nBottom = ptrPlateRect->nBottom;
		PrjRect.nLeft =0;
		PrjRect.nRight = ptrPlateRect->nRight;

		nMinCnt = ptrLPRConf->LPR_MIN_PIXEL -1;
		HLP_ProjectImg( ptrComWProjection2, ptrImageData,  &PrjRect, W_LINE_EDGE );
		MakeCharWSize( ptrLPRConf, &PrjRect,nMinCnt, nMinWith,ptrComWProjection2, ptrLPRChars, ptrImageData);

	}
	else
	{
		PrjRect.nTop	= ptrPlateRect->nTop;
		PrjRect.nBottom = ptrPlateRect->nBottom;
		PrjRect.nLeft	= ptrPlateRect->nLeft;
		PrjRect.nRight	= ptrPlateRect->nRight;
		
		HLP_ProjectImg( ptrComWProjection, ptrImageData,  &PrjRect, W_LINE_EDGE );
		MakeCharWSize( ptrLPRConf, &PrjRect,nMinCnt,nMinWith, ptrComWProjection, ptrLPRChars, ptrImageData);
	}

	MergeHangulCharWRect(nSegPosH, PrjRect.nTop, ptrLPRConf, ptrLPRChars);

	return ERR_SUCCESS;
}
/*=========================================================================
정확한 문자 위치 설정
==========================================================================*/
static int SegmentCharsH( pLPR_CONF ptrLPRConf, pCOMMONINFO ptrComInfo ,int * ptrComHProjection , int * ptrComWProjection , 
						 pCVRECT ptrPlateRect, pIMAGE_DATA ptrImageData,  pLPRCHAR ptrLPRChars , int * ptrSegPosH )
{
	int nSegPosH;	
	int nMinCnt;
	int nCharCnt, i;

	CVRECT_t *ptrRect;
	CHAR_t * ptrChar= NULL;	

	if ( ptrComInfo == NULL || ptrComWProjection == NULL || ptrComHProjection == NULL ||
		 ptrPlateRect == NULL || ptrImageData == NULL || ptrLPRChars == NULL )
	{
		return ERR_INVALID_DATA;
	}
	
	nSegPosH = *ptrSegPosH;

	nCharCnt = 0;	
	nCharCnt = ptrLPRChars->nCharCnt;
	nMinCnt = ptrLPRConf->LPR_MIN_PIXEL-1;

	for( i = 0 ; i < nCharCnt; i++ )
	{
		ptrChar = &ptrLPRChars->Chars[ i ];

		if ( ptrChar != NULL && ptrChar->ChRect.nSeqNum > 0 )
		{
			memset( ptrComHProjection, 0, sizeof( int ) * ptrComInfo->nImageHeight );
			HLP_ProjectImg( ptrComHProjection, ptrImageData, &ptrChar->ChRect, H_LINE_EDGE );

			/* 구번호판 상위또는 신 번호판일 경우 MinCount를 1로 잡는다 */
			if ( ptrChar->ChRect.nTop == nSegPosH || ptrChar->ChRect.nTop < nSegPosH)
			{
				nMinCnt = ptrLPRConf->LPR_MIN_PIXEL/2;
			}

			MakeCharHSize( ptrLPRConf, &ptrChar->ChRect, nMinCnt, ptrComHProjection, ptrImageData);
		}
	}

	return ERR_SUCCESS;
}

static int GetOCRData( pIMAGE_DATA ptrOCRImage, unsigned short * ptrPlateNumber, int * ptrPlateNumberLen )
{
	int nErrCode = ERR_SUCCESS;

	m_OCRData.uOCRMsg = TK_OCR_RECOG_MSG;
	m_OCRData.ptrImageDataSrc = ptrOCRImage;
	nErrCode = OCR_OCRProcess( &m_OCRData );

	if ( nErrCode == ERR_SUCCESS )
	{
		memcpy( &ptrPlateNumber[ * ptrPlateNumberLen ], &m_OCRData.OCRData , sizeof( unsigned short));
		*ptrPlateNumberLen +=1;
	}
	return nErrCode;
}

/*=========================================================================
문자 초기화
==========================================================================*/
int CHAR_Initialize( void )
{
	return ERR_SUCCESS;
}

/*=========================================================================
문자 인식
==========================================================================*/
int CHAR_GetRecognition( pLPR_CONF ptrLPRConf, pOCR_CONF ptrOCRConf, pDIP_DATA ptrDIPData, pIMAGE_DATA ptrDestImageData,
						 pIMAGE_DATA ptrChgImageData, pIMAGE_DATA ptrImageDataSrc, pCVRECT ptrOrgPlateRect, pLPRCHAR ptrLPRChars )
{
	int i;
	int nRectIdx = -1;	
	int nWidth, nHeight;
	int nLabelCnt = 0;
	int nVerifyCnt = 0;
	float fPlateRatio;
	int nSegPosH = 0;

	pRECTIMAGE  ptrRectImage = NULL;
	pIMAGE_DATA ptrPlateImage =NULL;
	pIMAGE_DATA ptrCharImage = NULL;
	pIMAGE_DATA ptrResizeCharImage = NULL;
	pIMAGE_DATA ptrTargetImage = NULL;
	
	LPR_CHAR_t * ptrComLPRChars = NULL;
	CVRECT_t * ptrComCharRect= NULL ;	
	CVRECT_t * ptrRect = NULL;
	CVRECT_t * ptrPlateRect = NULL;

	CHAR_t * ptrChar= NULL;	

	int * ptrComHProjection = NULL;
	int * ptrComWProjection = NULL;
	pCOMMONINFO ptrComInfo = NULL;
	
	/*===========================================================================
	[ 문자 분할 처리 설정 확인 ]
	============================================================================*/
	if( ptrLPRConf->LPR_DBG_RUN_CHAR_SEGMENT == 0 )
	{
		return ERR_SUCCESS;
	}

	ptrComLPRChars		= COM_GetLPRCharBuf();
	ptrComCharRect		= COM_GetCharRectBuf();
	
	ptrCharImage		= COM_GetTempImage2Buf();
	ptrResizeCharImage	= COM_GetCharImageBuf();
	ptrComHProjection	= COM_GetHProjectionBuf();
	ptrComWProjection	= COM_GetWProjectionBuf();
	
	ptrComInfo			= COM_GetInfo();	
	/*===========================================================================
	[ 최종 번호판 후보지 이진화 영상 가져 오기 ]	
	============================================================================*/
	if ( ptrOrgPlateRect != NULL )
	{		
		nRectIdx = ptrOrgPlateRect->nLPR_RectIdx;

		if ( nRectIdx < 0 ) 
		{
			return ERR_NOT_HAVE_RECT;
		}
		
		/* 최종 이미지 추출 */
		ptrRectImage			= COM_GetRectImage( nRectIdx );
		ptrPlateImage			= (pIMAGE_DATA)&ptrRectImage->ImageData;		
		ptrPlateRect			= &ptrRectImage->ImageRect;

		ptrPlateRect->nLeft		-=1;
		ptrPlateRect->nRight	+=1;
		ptrPlateRect->nTop		-=1;
		ptrPlateRect->nBottom	+=1;

		ptrOrgPlateRect->nLeft	-=1;
		ptrOrgPlateRect->nRight	+=1;
		ptrOrgPlateRect->nTop	-=1;
		ptrOrgPlateRect->nBottom +=1;

		if ( ptrPlateRect->nLeft < 0 ) ptrPlateRect->nLeft =0;
		if ( ptrPlateRect->nTop < 0 ) ptrPlateRect->nTop = 0;
		if ( ptrPlateRect->nRight > ptrPlateImage->nW ) ptrPlateRect->nRight = ptrPlateImage->nW ;
		if ( ptrPlateRect->nBottom > ptrPlateImage->nH ) ptrPlateRect->nBottom = ptrPlateImage->nH ;
	
		if ( ptrOrgPlateRect->nLeft < 0 ) ptrOrgPlateRect->nLeft =0;
		if ( ptrOrgPlateRect->nTop < 0 ) ptrOrgPlateRect->nTop = 0;
		if ( ptrOrgPlateRect->nRight > ptrRectImage->nOrgW ) ptrOrgPlateRect->nRight = ptrRectImage->nOrgW ;
		if ( ptrOrgPlateRect->nBottom > ptrRectImage->nOrgH ) ptrOrgPlateRect->nBottom = ptrRectImage->nOrgH ;
		
		fPlateRatio = HLP_Cal_ImageRatio( ptrPlateRect );
	}

	/*===========================================================================
	[ 메모리 체크 ]	
	============================================================================*/
	if ( ptrRectImage == NULL || ptrPlateImage == NULL || ptrPlateImage->ptrSrcImage2D == NULL || 
		 ptrComLPRChars == NULL || ptrCharImage == NULL || ptrLPRChars == NULL || ptrComHProjection == NULL || ptrComWProjection == NULL )
	{
		return ERR_NOT_HAVE_RECT;
	}
	
	ptrLPRChars->nCharCnt =0;
	memset( ptrComLPRChars, 0, sizeof( LPR_CHAR_t ));

	/*===========================================================================
	[ 번호판 문자 분리 ]
	============================================================================*/
	SegmentCharsW( ptrLPRConf, ptrComInfo, ptrComHProjection, ptrComWProjection, ptrPlateRect, ptrPlateImage , ptrComLPRChars, &nSegPosH);
	SegmentCharsH( ptrLPRConf, ptrComInfo, ptrComHProjection, ptrComWProjection, ptrPlateRect, ptrPlateImage , ptrComLPRChars ,&nSegPosH);
	
	memset( &ptrLPRChars->nPlateNumber, 0, sizeof( unsigned short) * MAX_LPR_CHAR_CNT );
	ptrLPRChars->nPlateNumerLen =0;

	/*===========================================================================
	[ 번호판 문자 이미지 추출 및 판독 ]
	============================================================================*/
	for( i = 0 ; i < ptrComLPRChars->nCharCnt ; i++ )
	{
		ptrRect = ( CVRECT_t*)&ptrComLPRChars->Chars[ i ].ChRect;		

		if ( VerifyCharRect(ptrLPRConf, ptrRect ) != ERR_SUCCESS )
		{
			continue;
		}

		/*===========================================================================
		[ 문자 이미지 복사 ]
		============================================================================*/
		ptrCharImage		= COM_GetTempImage2Buf();
		if ( HLP_CopyImageDataWithRect( ptrCharImage, ptrPlateImage, ptrRect , 1 ) != ERR_SUCCESS )
		{
			continue;
		}

		/*===========================================================================
		[ 인식용 사이즈로 정규화 변환 ]
		사이즈 변환 후 신경망에 의한 문자 정보 추출
		사이즈 변경된 문자 이미지 복사( Display , 신경망 학습용 )
		============================================================================*/			
		if( ptrLPRConf->LPR_DBG_RUN_CHAR_RESIZE )
		{	
			if ( ReSizeCubic( ptrDIPData, ptrCharImage, ptrResizeCharImage ) == ERR_SUCCESS )
			{	
				if ( Contrast_Binary( ptrOCRConf->OCR_RESIZE_THRESHOLD, ptrResizeCharImage ) != ERR_SUCCESS ) continue;
			}
			else
				continue;
			
			ptrTargetImage = ptrResizeCharImage;
		}
		else
		{
			if( ( ptrCharImage->nH > ptrOCRConf->OCR_CHAR_H  ) || ( ptrCharImage->nW > ptrOCRConf->OCR_CHAR_W ))
			{
				if ( ReSizeCubic( ptrDIPData, ptrCharImage, ptrResizeCharImage ) == ERR_SUCCESS )
				{
					if ( Contrast_Binary( ptrOCRConf->OCR_RESIZE_THRESHOLD, ptrResizeCharImage ) != ERR_SUCCESS ) continue;
				}
				else
					continue;
			
				ptrTargetImage = ptrResizeCharImage;
			}
			else
				ptrTargetImage = ptrCharImage;
		}				
		
		
		/*===========================================================================
		[ 문자 세션화 ]
		============================================================================*/		
		//ptrTargetImage = ptrCharImage;
		if( ptrLPRConf->LPR_DBG_RUN_CHAR_THINNING )
		{

			if ( ZhangSuen_Thinning( ptrDIPData, ptrTargetImage, 255 ) != ERR_SUCCESS )
			{
				continue;
			}
		}		
		
		/*===========================================================================
		[ 문자 인식 ]
		============================================================================*/
		if( ptrLPRConf->LPR_DBG_RUN_CHAR_RECOGNITION )
		{
			GetOCRData( ptrTargetImage,  ptrLPRChars->nPlateNumber, &ptrLPRChars->nPlateNumerLen );
		}

		/*===========================================================================
		[ 문자 복사 ( 학습 데이터를 위한 조치 ) ]
		============================================================================*/
		if( ptrLPRConf->LPR_DBG_RUN_CHAR_SAMPLING  || ptrLPRConf->LPR_DBG_SHOW_LAST_CHAR_BOX )
		{
			ptrChar = &ptrLPRChars->Chars[ ptrLPRChars->nCharCnt ];

			if ( ptrChar != NULL )
			{	
				if( ptrLPRConf->LPR_DBG_SHOW_LAST_CHAR_BOX )
				{
					memcpy( &ptrChar->ChRect, ptrRect, sizeof( CVRECT_t ));

					ptrChar->ChRect.nLeft -= ptrRectImage->ImageRect.nLeft;
					ptrChar->ChRect.nTop -= ptrRectImage->ImageRect.nTop;

					ptrChar->ChRect.nRight -= ptrRectImage->ImageRect.nLeft;
					ptrChar->ChRect.nBottom -= ptrRectImage->ImageRect.nTop;
				
				}

				if ( ptrChar->ImageData.ptrSrcImage2D != NULL )
				{
					HLP_CopyImageDataNotAlloc( &ptrChar->ImageData, ptrTargetImage );
					ptrLPRChars->nCharCnt++;
				}
			}
		}
	}

	/*===========================================================================
	[ 문자분할 영역 보기]
	============================================================================*/
	int nPos = 0;
	nPos = ptrRectImage->ImageRect.nTop;
	if (ptrLPRConf->LPR_DBG_SHOW_CHAR_SEG )
	{
		if ( ptrOrgPlateRect->nLeft != 0 )
		{
			nWidth = ( ptrOrgPlateRect->nRight - ptrOrgPlateRect->nLeft + 1 );
		}
		else
		{
			nWidth = ( ptrOrgPlateRect->nRight - ptrOrgPlateRect->nLeft );
		}

		for (int i= ptrOrgPlateRect->nTop; i<=ptrOrgPlateRect->nBottom; i++, nPos++)
		{
			if ( i < ptrChgImageData->nH )
				memcpy( *( ptrChgImageData->ptrSrcImage2D + i ) + ptrOrgPlateRect->nLeft , *( ptrRectImage->ImageData.ptrSrcImage2D + nPos ) + ptrRectImage->ImageRect.nLeft, nWidth );				
		}			
	}

	return ERR_SUCCESS;
}

int	CHAR_IOCTL( pIOCTL_DATA ptrIOCTLData )
{
	return ERR_SUCCESS;
}

int CHAR_Release( void )
{
	return ERR_SUCCESS;
}