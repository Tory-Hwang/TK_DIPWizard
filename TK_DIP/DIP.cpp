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
#include "../include/TK_CONF.h"

#include "DIP.h"
#include "DIP_Helper.h"

/*==================================================================================
DIP LIB Version
===================================================================================*/
#define DIP_VER_1ST		0x00
#define DIP_VER_2ND		0x01
/*==================================================================================*/


const double PI = acos(-1.0);
const int HOUGH_ANGLE = 360;

/* for Canny Edge */
const int fBit = 10;
const int tan225 =   414;       // tan25.5 << fbit, 0.4142
const int tan675 =   2414;      // tan67.5 << fbit, 2.4142

#define	CERTAIN_EDGE	255
#define PROBABLE_EDGE	100
#define DEFAULT_CANNY_TH_LOW		40
#define DEFAULT_CANNY_TH_HEIGH		110


#define MEDIAN_SORT_CNT				9
#define MAX_GAUSSIAN_DIM			15

typedef int ( * ImageProcFunc )( pDIP_DATA );
typedef struct ImageProcFuncs_
{
	UINT16 uProcMsg;
	ImageProcFunc ptrFunc;
}ImageProcFuncs_t;

/*========================================================
Static Variance
==========================================================*/
static int nFuncCnt=0;
static int m_nImageHeight =0;
static int m_nImageWidth =0;

static int m_nMaxHeight =0;
static int m_nMaxWidth =0;

static int m_initial = 0;

static IMAGE_DATA_t m_TmpImageData;
static IMAGE_DATA_t m_TmpImageData2;

static double ** Gaussian_dTempMatrix= NULL ;
static double * Gaussian_pMake= NULL;
static int m_GaussianMax_Dim = MAX_GAUSSIAN_DIM;
static int * m_Adap_ptrIntergral = NULL;
static int * m_Adap_ptrThreshold = NULL;
static int * m_Adap_ptrData = NULL;

static float m_Otsu_fVector[ MAX_GRAY_CNT ];
static int m_Otsu_Hist[ MAX_GRAY_CNT ];

static float ** m_diff_ptrCpyBuf = NULL;
static float ** m_diff_ptrTmpBuf = NULL;

static pDIP_CONF m_ptrdip_conf = NULL;

/* Hough Trans */
static int m_HoughRHOMax = 0;
static int m_HoughRHOCur = 0;

static int ** m_ptrHoughAcc = NULL;
static double* m_ptrtsin = NULL;
static double* m_ptrtcos = NULL;

/*========================================================
Static Functions 
==========================================================*/
static int IsValidData( IMAGE_DATA_t * ptrImage, int nBitCnt );


/*Image Processing Function */
static int ConvertGrayScale( pDIP_DATA ptrDIPData );

static int Brightness( pDIP_DATA ptrDIPData );
static int BitPlane( pDIP_DATA ptrDIPData );

static int Histogram( pDIP_DATA ptrDIPData );
static int HistogramEqual( pDIP_DATA ptrDIPData );

static int Binarization( pDIP_DATA ptrDIPData );
static int Binarization_Iterate( pDIP_DATA ptrDIPData );
static int Binarization_Adaptive( pDIP_DATA ptrDIPData );
static int Binarization_Otsu( pDIP_DATA ptrDIPData );
static int Binarization_ITG_Adaptive( pDIP_DATA ptrDIPData );


static int Gaussian( pDIP_DATA ptrDIPData );

static int Canny( pDIP_DATA ptrDIPData );
static int Sobel( pDIP_DATA ptrDIPData );
static int Roberts( pDIP_DATA ptrDIPData );
static int Prewitt( pDIP_DATA ptrDIPData );

static int Erosion( pDIP_DATA ptrDIPData );
static int Dilation( pDIP_DATA ptrDIPData );

static int SubImage( pDIP_DATA ptrDIPData );

static int Median( pDIP_DATA ptrDIPData );
static int Mean( pDIP_DATA ptrDIPData );
static int WeightedMean( pDIP_DATA ptrDIPData );
static int Beam( pDIP_DATA ptrDIPData );
static int Contrast( pDIP_DATA ptrDIPData );
static int OuterLine( pDIP_DATA ptrDIPData );
static int Binarization_DeNoise( pDIP_DATA ptrDIPData );
static int Inverse( pDIP_DATA ptrDIPData );
static int ResizeCubic( pDIP_DATA ptrDIPData );

static int Diffusion( pDIP_DATA ptrDIPData );
static int MMSE( pDIP_DATA ptrDIPData );
static int LightInfo( pDIP_DATA ptrDIPData );

static int ZS_Thinning( pDIP_DATA ptrDIPData );
static int HoughLineTrans( pDIP_DATA ptrDIPData );
static int Rotate( pDIP_DATA ptrDIPData );
static int DoG( pDIP_DATA ptrDIPData );
static int Bilinear_Normalization( pDIP_DATA ptrDIPData );
static int Unsharp( pDIP_DATA ptrDIPData );

static int Opening( pDIP_DATA ptrDIPData );
static int Closing( pDIP_DATA ptrDIPData );

/*========================================================
Storage Of Static Functions
==========================================================*/
static ImageProcFuncs_t ImageProcFuncs[ ] =
{
	{ TK_DIP_CONVERTGRAY_MSG, ConvertGrayScale },
	{ TK_DIP_BRIGHTNESS_MSG, Brightness },	
	{ TK_DIP_BITPLANE_MSG, BitPlane },	
	{ TK_DIP_HISTOGRAM_MSG, Histogram },	
	{ TK_DIP_HISTOGRAM_EQUAL_MSG, HistogramEqual },
	{ TK_DIP_BINARIZATION_MSG, Binarization },
	{ TK_DIP_BINARIZATION_ITER_MSG, Binarization_Iterate},
	{ TK_DIP_BINARIZATION_ADAPTIVE_MSG, Binarization_Adaptive},
	{ TK_DIP_BINARIZATION_ITG_ADAPTIVE_MSG, Binarization_ITG_Adaptive},

	{ TK_DIP_BINARIZATION_OTSU_MSG, Binarization_Otsu},
	{ TK_DIP_GAUSSIAN_MSG, Gaussian},
	{ TK_DIP_CANNY_MSG, Canny}, 
	{ TK_DIP_SOBEL_MSG, Sobel},
	
	{ TK_DIP_EROSION_MSG, Erosion},
	{ TK_DIP_DILATION_MSG, Dilation },
	{ TK_DIP_SUBIMAGE_MSG, SubImage},
	{ TK_DIP_MEDIAN_MSG, Median},
	{ TK_DIP_MEAN_MSG, Mean},
	{ TK_DIP_WEIGHTEDMEAN_MSG, WeightedMean},
	{ TK_DIP_BEAM_MSG, Beam},
	{ TK_DIP_ROBERTS_MSG, Roberts},	
	{ TK_DIP_PREWITT_MSG, Prewitt},
	{ TK_DIP_CONTRAST_MSG, Contrast },
	{ TK_DIP_OUTLINE_MSG, OuterLine },
	{ TK_DIP_BINARIZATION_DENOISE_MSG, Binarization_DeNoise },
	{ TK_DIP_INVERSE_MSG, Inverse },
	{ TK_DIP_RESIZE_MSG, ResizeCubic },
	{ TK_DIP_DIFFUSION_MSG, Diffusion },
	{ TK_DIP_MMSE_MSG, MMSE },
	{ TK_DIP_LIGHTINFO_MSG, LightInfo },
	{ TK_DIP_ZS_THINNING_MSG, ZS_Thinning },
	{ TK_DIP_HOUGH_LINE_TRANS_MSG, HoughLineTrans },
	{ TK_DIP_ROTATE_MSG, Rotate },
	{ TK_DIP_DOG_MSG, DoG },
	{ TK_DIP_BILINEAR_NOR_MSG, Bilinear_Normalization },
	{ TK_DIP_UNSHARP_MSG, Unsharp },

	{ TK_DIP_OPENING_MSG, Opening },
	{ TK_DIP_CLOSING_MSG, Closing },

	{ 0 , NULL }

};

/*======================================================================
bmp 데이터 유효성 체크 
========================================================================*/
static int IsValidData( IMAGE_DATA_t * ptrImage, int nBitCnt )
{
	if ( ptrImage == NULL || ptrImage->ptrSrcImage2D == NULL )
		return ERR_INVALID_DATA;

	if ( nBitCnt != 0  && ptrImage->nBitCount != nBitCnt )
		return ERR_INVALID_DATA;

	return ERR_SUCCESS;
	
}

/*======================================================================
영상의 밝기 정보
========================================================================*/
static int LightInfo( pDIP_DATA ptrDIPData )
{
	IMAGE_DATA_t * ptrImageDataSrc = NULL;
	pLIGHTINFO_PARAM ptrLight_Param = NULL;
	
	int i, j,nTemp[ MAX_GRAY_CNT ];
	int nErrCode = 0;
	
	int nBright, nDark, nMean, nMany, nOtsu;
	int nVal, nCnt, nSum, nSum2, nMaxCnt, nMaxPos, nDif;
	
	ptrImageDataSrc = ptrDIPData->ptrImageDataSrc1;
	ptrLight_Param = ( pLIGHTINFO_PARAM )ptrDIPData->ptrInputParam;	

	if( ptrLight_Param == NULL )
		return ERR_NO_PARAM;
	
	nErrCode = IsValidData( ptrImageDataSrc , 8 );
	if ( nErrCode != ERR_SUCCESS )
		return nErrCode ;

	memset( nTemp, 0, sizeof( nTemp ));
	nBright = nDark = nMean = nMany = nOtsu = nVal = nCnt = nSum = nMaxPos = nMaxCnt= nSum2 = nDif = 0;


	for( i = 0 ; i < ptrImageDataSrc->nH ; i++ )
	{
		for( j = 0 ; j < ptrImageDataSrc->nW ; j++ )
		{
			nVal = ptrImageDataSrc->ptrSrcImage2D[ i ][ j ];

			if ( nVal > nBright) nBright = nVal;
			if ( nVal < nDark )  nDark = nVal;
			
			nTemp[ nVal ]++;
			nSum += nVal;			
			nCnt++;
		}
	}

	/* Mean */
	ptrLight_Param->nMean = ( int )( nSum / nCnt );

	/* Variance , Sigma */
	if( ptrLight_Param->bSig )
	{
		for( i = 0 ; i < ptrImageDataSrc->nH ; i++ )
		{
			for( j = 0 ; j < ptrImageDataSrc->nW ; j++ )
			{
				nVal = ptrImageDataSrc->ptrSrcImage2D[ i ][ j ];

				nDif= abs( nVal - ptrLight_Param->nMean );
				nSum2 += ( nDif * nDif );
			}
		}


		for( i = 0 ; i < MAX_GRAY_CNT ; i++ )
		{
			nVal = nTemp[ i ];
			if ( nVal > nMaxCnt )
			{
				nMaxCnt = nVal;
				nMaxPos = i;
			}
		}

		ptrLight_Param->fVar = (float)( nSum2 / nCnt );
		ptrLight_Param->fSig = sqrt( ptrLight_Param->fVar );
	}

	ptrLight_Param->nSum = nSum;
	ptrLight_Param->nBright = nBright;
	ptrLight_Param->nDark = nDark;	
	ptrLight_Param->nMany = nMaxPos;

	if( ptrLight_Param->bThreshold )
	{
		BINARIZATION_PARAM_t Binary_Param;
		DIP_DATA_t DIPData;

		memset(&Binary_Param, 0, sizeof( BINARIZATION_PARAM_t ));
		memset(&DIPData, 0, sizeof( DIP_DATA_t ));

		Binary_Param.bOnlyThreshold = 1;
		DIPData.ptrImageDataSrc1 = ptrImageDataSrc;		
		DIPData.ptrInputParam = &Binary_Param;
		
		if( Binarization_Otsu( &DIPData ) == ERR_SUCCESS )
		{
			ptrLight_Param->nThreshold = Binary_Param.nThreshold;
		}
	}

	return ERR_SUCCESS;
}

/*======================================================================
Grayscale 변환 
========================================================================*/
static int ConvertGrayScale( pDIP_DATA ptrDIPData )
{
	IMAGE_DATA_t * ptrImageDataSrc , * ptrImageDataDest;
	int nLineByte8=0;
	int i, j;
	BYTE * pData, Blue, Green, Red, Y;
	int nErrCode = ERR_SUCCESS;

	ptrImageDataSrc = ptrImageDataDest = NULL;		
	ptrImageDataSrc = ptrDIPData->ptrImageDataSrc1;
	ptrImageDataDest = ptrDIPData->ptrImageDataDest;

	nErrCode = IsValidData( ptrImageDataSrc , 24 );
	if ( nErrCode != ERR_SUCCESS )
		return nErrCode ;

	ptrImageDataDest->nBitCount = 8;
	ptrImageDataDest->nW = ptrImageDataSrc->nW;
	ptrImageDataDest->nH = ptrImageDataSrc->nH;
	ptrImageDataDest->nImageSize = nLineByte8 * ptrImageDataDest->nH;
	nLineByte8 = HLP_GetCalBmpWidth( ptrImageDataSrc->nW, 8);
	ptrImageDataDest->ptrSrcImage2D = HLP_AllocMatrix( ptrImageDataDest->nH , nLineByte8);

	if ( ptrImageDataDest->ptrSrcImage2D == NULL )
	{
		return FALSE;
	}	
	
	for (i=0; i<ptrImageDataSrc->nH; i++)
	{
		pData = (BYTE*)&ptrImageDataSrc->ptrSrcImage2D[ i ][ 0 ];
		for (j=0; j< ptrImageDataSrc->nW ; j++)
		{	
			Blue = (BYTE)(*pData++);
			Green = (BYTE)(*pData++);
			Red = (BYTE)(*pData++);
			Y = (BYTE)LIMIT_VAL((0.299 * Red + 0.587 * Green+ 0.114 * Blue + 0.5));
			ptrImageDataDest->ptrSrcImage2D[ i ][ j ] = Y;
		}
	}
	return ERR_SUCCESS;
}

/*======================================================================
영상 반전( 흑/백 반전 )
========================================================================*/
static int Inverse( pDIP_DATA ptrDIPData )
{
	IMAGE_DATA_t * ptrImageDataSrc = NULL;
	
	int i, j;
	int nErrCode = 0;

	ptrImageDataSrc = ptrDIPData->ptrImageDataSrc1;
	
	nErrCode = IsValidData( ptrImageDataSrc , 8 );
	if ( nErrCode != ERR_SUCCESS )
		return nErrCode ;

	for( i = 0 ; i < ptrImageDataSrc->nH ; i++ )
	{
		for( j = 0 ; j < ptrImageDataSrc->nW ; j++ )
		{
			ptrImageDataSrc->ptrSrcImage2D[ i ][ j ] = 255 - ptrImageDataSrc->ptrSrcImage2D[ i ][ j ];
		}
	}

	return ERR_SUCCESS;
}

/*======================================================================
특정 임계치보다 밝은 부분을 중간값으로 전환 
이는 차량의 해드라이트 부분을 영상 중간값으로 설정 하기 위함.
========================================================================*/
static int Beam( pDIP_DATA ptrDIPData )
{	
	IMAGE_DATA_t * ptrImageDataSrc = NULL;
	pBINARIZATION_PARAM ptrBI_Param = NULL;
	int nThr = 170;

	int i, j , nTemp[ MAX_GRAY_CNT ];
	int nErrCode = 0;
	
	ptrImageDataSrc = ptrDIPData->ptrImageDataSrc1;
	ptrBI_Param = ( pBINARIZATION_PARAM )ptrDIPData->ptrInputParam;
	memset( nTemp, 0, sizeof( nTemp ));

	if ( ptrBI_Param != NULL )
		nThr = ptrBI_Param->nThreshold;

	nErrCode = IsValidData( ptrImageDataSrc , 8 );
	if ( nErrCode != ERR_SUCCESS )
		return nErrCode ;

	for( i = 0 ; i < ptrImageDataSrc->nH ; i++ )
	{
		for( j = 0 ; j < ptrImageDataSrc->nW ; j++ )
		{
			if ( ptrImageDataSrc->ptrSrcImage2D[ i ][ j ] > nThr )
			{
				//ptrImageDataSrc->ptrSrcImage2D[ i ][ j  ] = (BYTE)LIMIT_VAL( ptrImageDataSrc->ptrSrcImage2D[ i ][ j ] - 100 );
				ptrImageDataSrc->ptrSrcImage2D[ i ][ j  ] = 124;
			}			
		}
	}

	return ERR_SUCCESS;
}

/*======================================================================
영상의 외곽부분을 어둡게 처리하는 기법으로
차량 번호판의 경계선 및 이진화 처리 
========================================================================*/
static int OuterLine( pDIP_DATA ptrDIPData )
{	
	IMAGE_DATA_t * ptrImageDataSrc1 = NULL;
	IMAGE_DATA_t * ptrImageDataSrc2 = NULL;
	IMAGE_DATA_t * ptrImageDataDest = NULL;

	DIP_DATA_t DIP_Data1, DIP_Data2;
	LINE_EDGE_PARAM_t LineEdgeParam1, LineEdgeParam2;
	
	BINARIZATION_PARAM_t Binary_Param;

	pOUTERLINE_PARAM ptrOuter_Param = NULL;
	int i, j , nTemp[ MAX_GRAY_CNT ];
	int nErrCode = 0;
	int nOuter = OUTER_NONE;

	ptrImageDataSrc1 = ptrDIPData->ptrImageDataSrc1;
	ptrImageDataSrc2 = ptrDIPData->ptrImageDataSrc2;
	ptrImageDataDest = ptrDIPData->ptrImageDataDest;

	ptrOuter_Param = ( pOUTERLINE_PARAM )ptrDIPData->ptrInputParam;

	memset( nTemp, 0, sizeof( nTemp ));

	nErrCode = IsValidData( ptrImageDataSrc1 , 8 );
	if ( nErrCode != ERR_SUCCESS )
		return nErrCode ;

	nErrCode = IsValidData( ptrImageDataSrc2 , 8 );
	if ( nErrCode != ERR_SUCCESS )
		return nErrCode ;

	nErrCode = IsValidData( ptrImageDataDest , 8 );
	if ( nErrCode != ERR_SUCCESS )
		return nErrCode ;

	nOuter = ptrOuter_Param->nOuter;

	memset( &DIP_Data1, 0, sizeof( DIP_DATA_t));
	memset( &DIP_Data2, 0, sizeof( DIP_DATA_t));
	memset( &LineEdgeParam1, 0, sizeof( LINE_EDGE_PARAM_t));
	memset( &LineEdgeParam2, 0, sizeof( LINE_EDGE_PARAM_t));

	memset( &Binary_Param, 0, sizeof( BINARIZATION_PARAM_t));

	DIP_Data1.ptrImageDataSrc1 =ptrImageDataSrc1;
	DIP_Data2.ptrImageDataSrc1 =ptrImageDataSrc2;

	LineEdgeParam1.LineEdgeType = H_LINE_EDGE;
	DIP_Data1.ptrInputParam = &LineEdgeParam1;
	
	LineEdgeParam2.LineEdgeType = W_LINE_EDGE;
	DIP_Data2.ptrInputParam = &LineEdgeParam2;

	if ( nOuter == OUTER_ROBERTS )
	{		
		Roberts( &DIP_Data1);		
		Roberts( &DIP_Data2);
	}
	else if( nOuter == OUTER_PREWITT )
	{
		Prewitt( &DIP_Data1);
		Prewitt( &DIP_Data2);
	}
	else if ( nOuter == OUTER_SOBEL )
	{
		Sobel( &DIP_Data1);
		Sobel( &DIP_Data2);
	}

	DIP_Data1.ptrInputParam = &Binary_Param;
	Binarization_Otsu( &DIP_Data1 );

	DIP_Data2.ptrInputParam = &Binary_Param;
	Binarization_Otsu( &DIP_Data2 );


	for( i = 0 ; i < ptrImageDataDest->nH ; i++ )
	{
		for( j = 0 ; j < ptrImageDataDest->nW ; j++ )
		{
			ptrImageDataDest->ptrSrcImage2D[ i ][ j ] = (BYTE)LIMIT_VAL( ptrImageDataSrc1->ptrSrcImage2D[ i ][ j ]  + ptrImageDataSrc2->ptrSrcImage2D[ i ][ j ]  ) ;
		}
	}
	
	SEGMENT_PARAM_t SegMent;

	memset( &SegMent, 0, sizeof( SEGMENT_PARAM_t ));
	SegMent.SegmentType = SEGMENT_BINARY;

	DIP_Data1.ptrImageDataSrc1 = ptrImageDataDest;
	DIP_Data1.ptrInputParam = &SegMent;

	//Dilation( &DIP_Data1);
	//Erosion( &DIP_Data1);

	return ERR_SUCCESS;
}

/*======================================================================
영상의 밝기 변경 기법
========================================================================*/
static int Brightness( pDIP_DATA ptrDIPData )
{	
	IMAGE_DATA_t * ptrImageDataSrc = NULL;
	pBRIGHT_PARAM ptrBrightParam = NULL;

	int i, j , nTemp[ MAX_GRAY_CNT ];
	int nErrCode = 0;
	int nBrightness = DEFAULT_BRIGHTNESS;

	ptrImageDataSrc = ptrDIPData->ptrImageDataSrc1;
	ptrBrightParam = ( pBRIGHT_PARAM )ptrDIPData->ptrInputParam;

	memset( nTemp, 0, sizeof( nTemp ));

	nErrCode = IsValidData( ptrImageDataSrc , 8 );
	if ( nErrCode != ERR_SUCCESS )
		return nErrCode ;

	if ( ptrBrightParam == NULL )
		nBrightness = DEFAULT_BRIGHTNESS;
	else
		nBrightness = ptrBrightParam->nBright;

	for( i = 0 ; i < ptrImageDataSrc->nH ; i++ )
	{
		for ( j =0 ; j < ptrImageDataSrc->nW ; j++ )
		{
			ptrImageDataSrc->ptrSrcImage2D[ i ][ j ] = (BYTE)LIMIT_VAL( ptrImageDataSrc->ptrSrcImage2D[ i ][ j ] + nBrightness ) ;
		}
	}
	return ERR_SUCCESS;
}

/*======================================================================
영상의 각 Byte에 대해 해당 비트만 추출 하는 기법
========================================================================*/
static int BitPlane( pDIP_DATA ptrDIPData )
{	
	IMAGE_DATA_t * ptrImageDataSrc = NULL;
	int i, j , nTemp[ MAX_GRAY_CNT ];
	int nErrCode = ERR_SUCCESS;
	int nPlanNum = DEFAULT_BITPLANE;

	ptrImageDataSrc = ptrDIPData->ptrImageDataSrc1;
	memset( nTemp, 0, sizeof( nTemp ));

	nErrCode = IsValidData( ptrImageDataSrc , 8 );
	if ( nErrCode != ERR_SUCCESS )
		return nErrCode ;

	/*최 상위 비트 정보만 가져 옴 */
	for( i = 0 ; i < ptrImageDataSrc->nH ; i++ )
	{
		for ( j =0 ; j < ptrImageDataSrc->nW ; j++ )
		{
			ptrImageDataSrc->ptrSrcImage2D[ i ][ j ] = (BYTE)LIMIT_VAL( ptrImageDataSrc->ptrSrcImage2D[ i ][ j ] ) & ( 1 << nPlanNum ) ? 255: 0 ;
		}
	}

	return ERR_SUCCESS;
}

/*======================================================================
히스토그램 / 히스토그램 Normal
========================================================================*/
static int Histogram( pDIP_DATA ptrDIPData )
{
	pHISTOGRAM_PARAM ptrHistoParam = NULL;
	IMAGE_DATA_t * ptrImageDataSrc = NULL;
	int i, j , nTemp[ MAX_GRAY_CNT ];
	float fTemp =0.0;
	int nErrCode = ERR_SUCCESS;

	ptrImageDataSrc = ptrDIPData->ptrImageDataSrc1;
	ptrHistoParam = ( pHISTOGRAM_PARAM)ptrDIPData->ptrInputParam;
	memset( nTemp, 0, sizeof( nTemp ));

	nErrCode = IsValidData( ptrImageDataSrc , 8 );
	if ( ptrHistoParam == NULL || nErrCode != ERR_SUCCESS )
		return nErrCode ;	
	
	/* Normalize가 필요할 경우 */
	if ( ptrHistoParam->bNormalize == 1 )
	{
		/* Histogram */
		for( i = 0 ; i < ptrImageDataSrc->nH ; i++ )
		{
			for ( j =0 ; j < ptrImageDataSrc->nW ; j++ )
			{
				nTemp[ ptrImageDataSrc->ptrSrcImage2D[ i ][ j ] ]++;
			}
		}

		/* Histogram normalization */
		fTemp = (float)ptrImageDataSrc->nH * ptrImageDataSrc->nW;
		for( i = 0 ; i < MAX_GRAY_CNT ; i++ )
		{
			ptrHistoParam->fParam[ i ] = nTemp[ i ] / fTemp;
		}
	}
	else
	{
		/* Histogram */
		for( i = 0 ; i < ptrImageDataSrc->nH ; i++ )
		{
			for ( j =0 ; j < ptrImageDataSrc->nW ; j++ )
			{
				ptrHistoParam->fParam[ ptrImageDataSrc->ptrSrcImage2D[ i ][ j ] ]++;
			}
		}
	}

	return ERR_SUCCESS;
}

/*======================================================================
히스토그램 평활화
========================================================================*/
static int HistogramEqual( pDIP_DATA ptrDIPData )
{
	pHISTOGRAM_PARAM ptrHistoParam = NULL;	
	IMAGE_DATA_t * ptrImageDataSrc = NULL;
	double dCDF[ MAX_GRAY_CNT ];

	int i, j;
	int nErrCode =ERR_SUCCESS;

	ptrImageDataSrc = ptrDIPData->ptrImageDataSrc1;
	ptrHistoParam = ( pHISTOGRAM_PARAM)ptrDIPData->ptrInputParam;

	nErrCode = IsValidData( ptrImageDataSrc , 8 );
	if ( ptrHistoParam == NULL || nErrCode != ERR_SUCCESS )
		return nErrCode ;	

	/* Histogram */
	ptrHistoParam->bNormalize = 1;
	Histogram( ptrDIPData );	


	/* Histogram Accumulate */
	memset( &dCDF, 0, sizeof( dCDF ));	
	dCDF[ 0 ] = ptrHistoParam->fParam[ 0 ];

	for( i =  1 ; i < MAX_GRAY_CNT ; i++ )
	{
		dCDF[ i ] = dCDF[ i - 1 ] + ptrHistoParam->fParam[ i ];
	}

	/* Histogram Equalization */
	for( i = 0 ; i < ptrImageDataSrc->nH ; i++ )
	{
		for( j = 0 ; j < ptrImageDataSrc->nW ; j++ )
		{
			ptrImageDataSrc->ptrSrcImage2D[ i ][ j ] = ( BYTE)LIMIT_VAL ( dCDF[ ptrImageDataSrc->ptrSrcImage2D[ i ][ j ] ] * 255);
		}
	}

	return ERR_SUCCESS;
}

/*======================================================================
영상 이진화
========================================================================*/
static int Binarization( pDIP_DATA ptrDIPData )
{
	pBINARIZATION_PARAM ptrBinParam = NULL;	
	IMAGE_DATA_t * ptrImageDataSrc = NULL;
	int i, j;
	int nErrCode = ERR_SUCCESS;
	int nThreshold ;

	ptrImageDataSrc = ptrDIPData->ptrImageDataSrc1;
	ptrBinParam = ( pBINARIZATION_PARAM)ptrDIPData->ptrInputParam;

	nErrCode = IsValidData( ptrImageDataSrc , 8 );
	if ( nErrCode != ERR_SUCCESS )
		return nErrCode ;

	if ( ptrBinParam == NULL )
	{
		nThreshold = DEFAULT_BIN_THRESOLD;
	}
	else
	{
		nThreshold = ptrBinParam->nThreshold;
	}

	for( i = 0 ; i < ptrImageDataSrc->nH ; i++ )
	{
		for( j = 0 ; j < ptrImageDataSrc->nW  ;j++)
		{			
			ptrImageDataSrc->ptrSrcImage2D[i][j] = ( (BYTE)LIMIT_VAL( ptrImageDataSrc->ptrSrcImage2D[i][j] ) > nThreshold ) ? 255:0;
		}
	}

	return ERR_SUCCESS;
}

/*======================================================================
영상 이진화( 반복 기법 )
========================================================================*/
static int Binarization_Iterate( pDIP_DATA ptrDIPData )
{	
	IMAGE_DATA_t * ptrImageDataSrc = NULL;
	int i;
	int nErrCode = ERR_SUCCESS;
	HISTOGRAM_PARAM_t Histo_Param;
	BINARIZATION_PARAM_t * ptrBin_Param = NULL;
	
	int nSum =0;
	int nTh, nOldTh;
	float nF1, nF2, nF3;
	float nS1, nS2, nS3;

	ptrImageDataSrc = ptrDIPData->ptrImageDataSrc1;
	ptrBin_Param = ( BINARIZATION_PARAM_t * )ptrDIPData->ptrInputParam;
	nErrCode = IsValidData( ptrImageDataSrc , 8 );

	if ( ptrBin_Param == NULL || nErrCode != ERR_SUCCESS )
		return nErrCode ;

	memset( &Histo_Param, 0, sizeof( HISTOGRAM_PARAM_t ));
	ptrDIPData->ptrInputParam = &Histo_Param;

	/* 히스토그램 산출 */
	Histo_Param.bNormalize = 1;
	Histogram( ptrDIPData );
	
	/* 평균 밝기에 의한 Threshold 선정*/
	for( i = 0 ; i < MAX_GRAY_CNT ; i++ )
		nSum += (int)( i * Histo_Param.fParam[ i ] );

	nTh = (int)nSum;

	
	/* 반복에의한 Threshold 선정 */
	do 
	{
		nOldTh = nTh;
		nF1 = nF2 =0.;
		
		for( i = 0 ; i <= nOldTh ; i++ )
		{
			nF1 += ( i * Histo_Param.fParam[ i ]);
			nF2 += ( Histo_Param.fParam[ i ]);
		}

		if ( nF2 == 0. ) nF2 =1.;
		nF3 = nF1 / nF2;
		
		nS1 = nS2 = 0.;
		for ( i = nOldTh + 1; i < MAX_GRAY_CNT  ; i++ )
		{
			nS1 += ( i * Histo_Param.fParam[ i ]);
			nS2 += ( Histo_Param.fParam[ i ]);
		}
		if ( nS2 == 0. ) nS2 =1.;
		nS3 = nS1 / nS2;

		if ( nF1 == 0. ) nF1 = 1;
		if ( nS1 == 0. ) nS1 = 1;

		nTh = (int)(( nF3 + nS3 )/2);

	}while ( nTh != nOldTh );
	
	ptrBin_Param->nThreshold = nTh;
	ptrDIPData->ptrInputParam = ptrBin_Param;
	if ( ptrBin_Param->bOnlyThreshold == 0 )
	{
		return Binarization( ptrDIPData );
	}

	return ERR_SUCCESS;

}

/*======================================================================
영상 이진화( Adptive )
========================================================================*/
static int Binarization_Adaptive( pDIP_DATA ptrDIPData )
{
	IMAGE_DATA_t * ptrImageDataSrc = NULL;
	BINARIZATION_PARAM_t * ptrBin_Param;
	int i, j;
	int nErrCode = ERR_SUCCESS;		
	
	int nSize , nIndex, nCnt;
	int nSum =0;
	int nX1, nY1, nX2, nY2, nInd, nInd1, nInd2, nInd3;
	int nS = 0;
	int nS2 = 0;

	float fT = 0.0;
	float fIT  = 0.0;
	int nDiff = 0;

	ptrImageDataSrc = ptrDIPData->ptrImageDataSrc1;
	ptrBin_Param = ( BINARIZATION_PARAM_t * )ptrDIPData->ptrInputParam;

	nErrCode = IsValidData( ptrImageDataSrc , 8 );
	if ( ptrBin_Param == NULL || nErrCode != ERR_SUCCESS )
		return nErrCode ;
	
	nSize = ptrImageDataSrc->nH * ptrImageDataSrc->nW;
	

	if ( m_Adap_ptrIntergral == NULL || m_Adap_ptrThreshold == NULL || m_Adap_ptrData  == NULL )
	{		
		return ERR_ALLOC_BUF;
	}

	nCnt = ptrBin_Param->nAdaptCnt;
	fT = ptrBin_Param->fAdaptRatio;
	fIT  = (float)(1.0 - fT);

	nS = ptrImageDataSrc->nW / nCnt;
	nS2 = nS >> 1;

	nIndex = 0;

	for( i = 0 ; i < ptrImageDataSrc->nH ; i++ )
	{
		for( j = 0 ; j < ptrImageDataSrc->nW ; j++ )
		{
			m_Adap_ptrData[ nIndex++ ] = ptrImageDataSrc->ptrSrcImage2D[ i ][ j ];
		}
	}

	
	nInd = 0;

	while( nInd < nSize )
	{
		nSum += m_Adap_ptrData[ nInd ] & 0xFF;
		m_Adap_ptrIntergral[ nInd ] = nSum;
		nInd += ptrImageDataSrc->nW;
	}

	nX1 = 0;
	for( i = 1 ; i <  ptrImageDataSrc->nW ; i++ )
	{
		nSum = 0;
		nInd = i;
		nInd3 = nInd - nS2;

		if ( i > nS ) nX1 = i - nS;
		nDiff = i - nX1;

		for( j = 0 ; j < ptrImageDataSrc->nH ; j++ )
		{
			nSum += m_Adap_ptrData[ nInd ] & 0xFF;
			m_Adap_ptrIntergral[ nInd ] = m_Adap_ptrIntergral[ nInd -1 ] + nSum;
			nInd += ptrImageDataSrc->nW;

			if ( i < nS2 ) continue;
			if ( j < nS2 ) continue;

			nY1 = ( j < nS ? 0 : j - nS );

			nInd1 = nY1 * ptrImageDataSrc->nW;
			nInd2 = j * ptrImageDataSrc->nW;

			if (((m_Adap_ptrData[ nInd3 ] & 0xFF ) *( nDiff * (j - nY1))) < \
				((m_Adap_ptrIntergral[ nInd2 + i ] - m_Adap_ptrIntergral [ nInd1 + i ] - m_Adap_ptrIntergral[ nInd2 + nX1 ] + m_Adap_ptrIntergral[ nInd1 + nX1 ]) *fIT ) )
			{
				m_Adap_ptrThreshold[ nInd3] = 0x00;
			} else {
				m_Adap_ptrThreshold[ nInd3] = 0xFFFFFF;
			}
			nInd3 += ptrImageDataSrc->nW ;

		}
	}

	nY1 = 0;
	for( j = 0 ; j < ptrImageDataSrc->nH ;j++ )
	{
		i = 0;
		nY2 = ptrImageDataSrc->nH -1;
		if ( j < ptrImageDataSrc->nH - nS2 )
		{
			i = ptrImageDataSrc->nW - nS2;
			nY2 = j + nS2;
		}

		nInd = j * ptrImageDataSrc->nW + i;
		if ( j > nS2 ) nY1 = j - nS2;
		nInd1 = nY1 * ptrImageDataSrc->nW;
		nInd2 = nY2 * ptrImageDataSrc->nW;
		nDiff = nY2 - nY1;

		for( ; i < ptrImageDataSrc->nW ; i++, nInd++)
		{
			nX1 = ( i < nS2 ? 0 : i - nS2);
			nX2 = i + nS2;

			if ( nX2 >= ptrImageDataSrc->nW ) nX2 = ptrImageDataSrc->nW -1;
			if (((m_Adap_ptrData[ nInd ]& 0xFF )*((nX2 - nX1) * nDiff)) < \
				((m_Adap_ptrIntergral[ nInd2 + nX2 ] - m_Adap_ptrIntergral[ nInd1 + nX2  ] - m_Adap_ptrIntergral[ nInd2 + nX1 ] + m_Adap_ptrIntergral[ nInd1 + nX1 ]) * fIT))
			{
				m_Adap_ptrThreshold[ nInd ] = 0x00;
			} else {
				m_Adap_ptrThreshold[ nInd] = 0xFFFFFF;
			}
		}
	}

	nIndex =0;

	for( i = 0 ; i < ptrImageDataSrc->nH ; i++ )
	{
		 for( j = 0 ; j < ptrImageDataSrc->nW ; j++ )
		 {
			 ptrImageDataSrc->ptrSrcImage2D[ i ][ j ] = (BYTE)m_Adap_ptrThreshold[ nIndex++ ];
		 }
	}

	return ERR_SUCCESS;
}

/*======================================================================
영상 이진화( Integral Adaptive  )
========================================================================*/
static int Binarization_ITG_Adaptive( pDIP_DATA ptrDIPData )
{
	IMAGE_DATA_t * ptrImageDataSrc = NULL;
	BINARIZATION_PARAM_t * ptrBin_Param;
	int i, j;
	int nErrCode = ERR_SUCCESS;		
	int nIndex = 0;
	int nLineSum ;
	int nWidth, nHeight ;
	int nTop, nBottom, nLeft, nRight, nSum1, nSum2, nSum3, nGraySum, nMaskSize, nMaskArea, nVal;
	int nHalfMask;
	

	ptrImageDataSrc = ptrDIPData->ptrImageDataSrc1;
	ptrBin_Param = ( BINARIZATION_PARAM_t * )ptrDIPData->ptrInputParam;

	nErrCode = IsValidData( ptrImageDataSrc , 8 );
	if ( ptrBin_Param == NULL || nErrCode != ERR_SUCCESS )
		return nErrCode ;

	if ( m_Adap_ptrIntergral == NULL )
	{		
		return ERR_ALLOC_BUF;
	}

	
	if ( HLP_CopyImageDataNotAlloc( &m_TmpImageData, ptrImageDataSrc) != ERR_SUCCESS )
		return ERR_INVALID_DATA;

	nWidth = m_TmpImageData.nW;
	nHeight = m_TmpImageData.nH;
	nMaskSize = ptrBin_Param->nITGSize;
	if( nMaskSize == 0 )
		nMaskSize = DEFAULT_ITG_SIZE;

	nMaskArea = nMaskSize * nMaskSize;
	nHalfMask = nMaskSize / 2 ;

	/* 첫번째 값 대입 */
	m_Adap_ptrIntergral[ nIndex++ ] = m_TmpImageData.ptrSrcImage2D[ 0 ][ 0 ];

	/* 첫째 라인 대입 */
	for( j = 1 ; j < m_TmpImageData.nW ; j++ )
	{
		m_Adap_ptrIntergral[ nIndex++ ] = ( m_TmpImageData.ptrSrcImage2D[ 0 ][ j -1 ] + m_TmpImageData.ptrSrcImage2D[ 0 ][ j ]) ;
	}

	/* Intergral 생성 */
	for( i = 1 ; i < m_TmpImageData.nH ; i++ )
	{
		nLineSum = 0;

		for( j = 0 ; j < m_TmpImageData.nW ; j++ )
		{
			nLineSum += m_TmpImageData.ptrSrcImage2D[ i ][ j ];
			m_Adap_ptrIntergral[ nIndex++ ] = m_Adap_ptrIntergral[ nWidth * ( i - 1 ) + j ] + nLineSum;
		}
	}



	for( i = 0 ; i < m_TmpImageData.nH ; i++ )
	{
		nTop = i - nHalfMask ;

		if ( nTop < 0 ) 
			nTop = 0;
		else if ( nTop > ( nHeight - nMaskSize ) )
			nTop = nHeight - nMaskSize;

		nBottom = nTop + nMaskSize -1;

		for( j = 0 ; j < m_TmpImageData.nW ; j++ )
		{
			nLeft = j -nHalfMask ;
			if ( nLeft < 0 ) 
				nLeft = 0;
			else if ( nLeft > (nWidth - nMaskSize ))
				nLeft = nWidth - nMaskSize;

			nRight = nLeft + nMaskSize -1;

			
			nSum1 = ( nLeft > 0 && nTop > 0 ) ? m_Adap_ptrIntergral[ ( nTop -1 ) * nWidth  + nLeft -1 ]: 0 ;
			nSum2 = ( nLeft > 0 ) ? m_Adap_ptrIntergral[ nBottom * nWidth + nLeft -1 ]: 0 ;
			nSum3 = ( nTop > 0 ) ? m_Adap_ptrIntergral[ ( nTop - 1 ) * nWidth + nRight ]: 0 ;

			nGraySum = m_Adap_ptrIntergral[ nBottom * nWidth + nRight ] - nSum3 - nSum2 + nSum1;

			nVal = ( m_TmpImageData.ptrSrcImage2D[ i ][ j ] + 3 ) * nMaskArea;

			if( nVal <= nGraySum )
				ptrImageDataSrc->ptrSrcImage2D[ i ][ j ] = 255;
			else
				ptrImageDataSrc->ptrSrcImage2D[ i ][ j ] = 0;
		}
	}

	return ERR_SUCCESS;
}

/*======================================================================
영상 이진화( Otsu )
========================================================================*/
static int Binarization_Otsu( pDIP_DATA ptrDIPData )
{	
	IMAGE_DATA_t * ptrImageDataSrc = NULL;
	BINARIZATION_PARAM_t * ptrBin_Param=NULL;
	int i, j;
	int nErrCode = ERR_SUCCESS;	
	
	float  fP1, fP2, fP12, fDiff;
	int nThreshold;
	
	ptrImageDataSrc = ptrDIPData->ptrImageDataSrc1;
	ptrBin_Param = ( BINARIZATION_PARAM_t * )ptrDIPData->ptrInputParam;
	nErrCode = IsValidData( ptrImageDataSrc , 8 );

	if ( ptrBin_Param == NULL || nErrCode != ERR_SUCCESS )
		return nErrCode ;
	
	memset( m_Otsu_fVector, 0, sizeof( float) * MAX_GRAY_CNT );
	memset( m_Otsu_Hist, 0, sizeof( int ) * MAX_GRAY_CNT );
	
	/* Histogram */
	for( i = 0 ; i < ptrImageDataSrc->nH ; i++ )
	{
		for ( j =0 ; j < ptrImageDataSrc->nW ; j++ )
		{
			m_Otsu_Hist[ ptrImageDataSrc->ptrSrcImage2D[ i ][ j ] ]++;
		}
	}	

	for( i  = 1 ; i <  MAX_GRAY_CNT ; i++)
	{
		fP1 = GetPx( 0, i, m_Otsu_Hist);
		fP2 = GetPx( i + 1, MAX_GRAY_CNT -1, m_Otsu_Hist );
		fP12 = fP1 * fP2;

		if ( fP12 == 0.)
			fP12 = 1.;

		fDiff = ( GetMean( 0, i, m_Otsu_Hist) * fP2 ) - ( GetMean( i + 1 , MAX_GRAY_CNT -1, m_Otsu_Hist ) * fP1);
		m_Otsu_fVector[ i ] = (float)fDiff * fDiff / fP12;
	}
	
	nThreshold = GetMax( m_Otsu_fVector, MAX_GRAY_CNT );

	ptrBin_Param->nThreshold = nThreshold;
	ptrDIPData->ptrInputParam = ptrBin_Param;
	if ( ptrBin_Param->bOnlyThreshold == 0 )
	{
		return Binarization( ptrDIPData );
	}

	return ERR_SUCCESS;

}

/*======================================================================
영상 스무스( 가우시안 )
========================================================================*/
static int Gaussian( pDIP_DATA ptrDIPData )
{
	IMAGE_DATA_t * ptrImageDataSrc = NULL;
	int i, j, k, x;
	int nErrCode = ERR_SUCCESS;
	int nDim, nDim2;	
	double dSigma = 1.0;	
	double Sum1, Sum2;
	pGAUSSIAN_PARAM ptrGau_Param= NULL;
	static int maskFlag = 0;

	Sum1= Sum2 =0.0;
	ptrImageDataSrc = ptrDIPData->ptrImageDataSrc1;
	ptrGau_Param = ( pGAUSSIAN_PARAM)ptrDIPData->ptrInputParam;

	nErrCode = IsValidData( ptrImageDataSrc , 8 );
	if ( nErrCode != ERR_SUCCESS )
		return nErrCode ;

	if ( ptrGau_Param != NULL )
	{
		dSigma= ptrGau_Param->dSigma;
	}

	/* ====================================
	1차원 Gaussian Mask 생성 
	=======================================*/
	/* Gaussian Mask 크기 값 */
	nDim = (int)max( 3.0, 2*4*dSigma + 1.0 );

	if( nDim > m_GaussianMax_Dim )
	{
		delete[] Gaussian_pMake;
		Gaussian_pMake = NULL;
		m_GaussianMax_Dim = nDim;

		Gaussian_pMake = new double[ nDim ];
		if ( Gaussian_pMake == NULL )
			return ERR_ALLOC_BUF;

		maskFlag = 0;
	}

	/* Gaussian Mask 크기가 짝수 일경우 홀수로 만듦 */
	if ( nDim % 2 == 0 )
		nDim++;

	/* Gaussian Mask 중간 위치 ( 평균값 0인 위치 ) */
	nDim2 = ( int )nDim / 2;

	if ( maskFlag == 0 )
	{
		for( i = 0; i < nDim ; i++ )
		{
			x = i - nDim2;
			Gaussian_pMake[ i ] = exp(-(x*x)/(2*dSigma*dSigma)) / (sqrt(2*PI)*dSigma);
		}
		maskFlag = 1;
	}
	/* 세로 방향 Gaussian Mask 연산 */
	for( j = 0 ; j < ptrImageDataSrc->nW ; j++ )
	{
		for( i = 0 ; i < ptrImageDataSrc->nH ; i++ )
		{

			Sum1= Sum2 =0.0;
			for( k = 0 ; k < nDim ;k++ )
			{
				x = k - nDim2 + i;
				if ( x >=0 && x < ptrImageDataSrc->nH )
				{
					Sum1 += Gaussian_pMake[ k ];
					Sum2 += ( Gaussian_pMake[ k ] * ptrImageDataSrc->ptrSrcImage2D[ x ][ j ]);
				}
			}
			Gaussian_dTempMatrix[ i ][ j ] = (double)( Sum2 / Sum1 );			
		}	
	}

	/* 가로 방향 Gaussian Mask 연산 */
	for( i = 0 ; i < ptrImageDataSrc->nH ; i++ )
	{
		for( j =0 ; j< ptrImageDataSrc->nW ; j++ )
		{
			Sum1= Sum2 =0.0;
			for( k = 0 ; k < nDim ; k++ )
			{
				x = k -nDim2 + j;
				if ( x >= 0 && x < ptrImageDataSrc->nW )
				{
					Sum1 += Gaussian_pMake[ k ];
					Sum2 += ( Gaussian_pMake[ k ] * Gaussian_dTempMatrix[ i ][ x ]);
				}
			}
			ptrImageDataSrc->ptrSrcImage2D[ i ][ j ]= (BYTE)LIMIT_VAL( Sum2 / Sum1 );
		}
	}

	return ERR_SUCCESS;
	
}

/*======================================================================
영상 외곽선 검출( robert )
========================================================================*/
static int Roberts( pDIP_DATA ptrDIPData )
{
	IMAGE_DATA_t * ptrImageDataSrc = NULL;
	pLINE_EDGE_PARAM ptrLineEdge= NULL;

	int i, j;
	int nErrCode = 0;
	int nX, nY;
	double dRobert;
	BYTE LineEdgeType = HW_LINE_EDGE;

	ptrImageDataSrc = ptrDIPData->ptrImageDataSrc1;
	ptrLineEdge = ( pLINE_EDGE_PARAM )ptrDIPData->ptrInputParam;

	nErrCode = IsValidData( ptrImageDataSrc , 8 );
	if ( nErrCode != ERR_SUCCESS )
		return nErrCode ;
	
	if ( HLP_CopyImageDataNotAlloc( &m_TmpImageData, ptrImageDataSrc) != ERR_SUCCESS )
		return ERR_INVALID_DATA;

	if ( ptrLineEdge != NULL )
		LineEdgeType = ptrLineEdge->LineEdgeType;

	/*========================================================================================
	Roberts Mask( 경계선 추출 )

	Vertical( nY )        Horizon(nX)
	-1 0  0			       0  0  -1
	0  1  0                0  1  0
	0  0  0                0  0  0
	=========================================================================================*/

	for( i = 1 ; i < ptrImageDataSrc->nH -1; i++ )
	{
		for( j = 1 ; j < ptrImageDataSrc->nW -1;j++)
		{
			nY = m_TmpImageData.ptrSrcImage2D[i][j] - m_TmpImageData.ptrSrcImage2D[i-1][j-1];
			nX = m_TmpImageData.ptrSrcImage2D[i][j] - m_TmpImageData.ptrSrcImage2D[i -1 ][j+1];			
			
			if ( LineEdgeType == HW_LINE_EDGE )
			{
				dRobert = sqrt( (double)( nX*nX + nY*nY ));
				ptrImageDataSrc->ptrSrcImage2D[i][j] = (BYTE)LIMIT_VAL( dRobert );
			}
			else if ( LineEdgeType == W_LINE_EDGE )
			{
				ptrImageDataSrc->ptrSrcImage2D[i][j] = (BYTE)LIMIT_VAL( nY );
			}
			else 
			{
				ptrImageDataSrc->ptrSrcImage2D[i][j] = (BYTE)LIMIT_VAL( nX );
			}
		}
	}

	return ERR_SUCCESS;
}

/*======================================================================
영상 외곽선 검출( Prewitt )
========================================================================*/
static int Prewitt( pDIP_DATA ptrDIPData )
{
	IMAGE_DATA_t * ptrImageDataSrc = NULL;
	pLINE_EDGE_PARAM ptrLineEdge= NULL;
	BYTE LineEdgeType = HW_LINE_EDGE;

	int i, j;
	int nErrCode = 0;
	int nX, nY;
	double dPrewitt;

	ptrImageDataSrc = ptrDIPData->ptrImageDataSrc1;
	ptrLineEdge = ( pLINE_EDGE_PARAM )ptrDIPData->ptrInputParam;

	nErrCode = IsValidData( ptrImageDataSrc , 8 );
	if ( nErrCode != ERR_SUCCESS )
		return nErrCode ;
	
	if ( HLP_CopyImageDataNotAlloc( &m_TmpImageData, ptrImageDataSrc) != ERR_SUCCESS )
		return ERR_INVALID_DATA;

	if ( ptrLineEdge != NULL )
		LineEdgeType = ptrLineEdge->LineEdgeType;

	/*========================================================================================
	dPrewitt Mask( 경계선 추출 )

	Vertical( nY )        Horizon(nX)
	-1 -1 -1			   -1  0  1
	0  0  0               -1  0  1
	1  1  1               -1  0  1
	=========================================================================================*/

	for( i = 0 ; i < ptrImageDataSrc->nH; i++ )
	{
		for( j = 0 ; j < ptrImageDataSrc->nW;j++)
		{

			if ( i == 0 || i == ptrImageDataSrc->nH -1 || j == 0 || j == ptrImageDataSrc->nW - 1)
			{
				ptrImageDataSrc->ptrSrcImage2D[i][j] = 0;
			}
			else
			{
				nY = - m_TmpImageData.ptrSrcImage2D[i-1][j-1] - m_TmpImageData.ptrSrcImage2D[i-1][j] - m_TmpImageData.ptrSrcImage2D[i-1][j+1]
				+ m_TmpImageData.ptrSrcImage2D[i+1][j-1] + m_TmpImageData.ptrSrcImage2D[i+1][j] + m_TmpImageData.ptrSrcImage2D[i+1][j+1];

				nX = - m_TmpImageData.ptrSrcImage2D[i-1][j-1] - m_TmpImageData.ptrSrcImage2D[i][j-1] - m_TmpImageData.ptrSrcImage2D[i+1][j-1]
				+ m_TmpImageData.ptrSrcImage2D[i-1][j+1] + m_TmpImageData.ptrSrcImage2D[i][j+1] + m_TmpImageData.ptrSrcImage2D[i+1][j+1];			

				if ( LineEdgeType == HW_LINE_EDGE )
				{
					dPrewitt = sqrt( (double)( nX*nX + nY*nY ));
					ptrImageDataSrc->ptrSrcImage2D[i][j] = (BYTE)LIMIT_VAL( dPrewitt );
				}
				else if ( LineEdgeType == W_LINE_EDGE )
				{
					ptrImageDataSrc->ptrSrcImage2D[i][j] = (BYTE)LIMIT_VAL( nY );
				}
				else 
				{
					ptrImageDataSrc->ptrSrcImage2D[i][j] = (BYTE)LIMIT_VAL( nX );
				}
			}

		}
	}	

	return ERR_SUCCESS;
}

/*======================================================================
영상 외곽선 검출( Sobel )
========================================================================*/
static int Sobel( pDIP_DATA ptrDIPData )
{
	IMAGE_DATA_t  * ptrImageDataSrc = NULL;
	int i, j;
	int nErrCode = 0;
	int nX, nY;
	double dSobel;
	pLINE_EDGE_PARAM ptrLineEdge= NULL;
	BYTE LineEdgeType = HW_LINE_EDGE;

	ptrImageDataSrc = ptrDIPData->ptrImageDataSrc1;
	ptrLineEdge = ( pLINE_EDGE_PARAM )ptrDIPData->ptrInputParam;

	nErrCode = IsValidData( ptrImageDataSrc , 8 );
	if ( nErrCode != ERR_SUCCESS )
		return nErrCode ;
		
	if ( HLP_CopyImageDataNotAlloc( &m_TmpImageData, ptrImageDataSrc) != ERR_SUCCESS )
		return ERR_INVALID_DATA;
	
	if ( ptrLineEdge != NULL )
		LineEdgeType = ptrLineEdge->LineEdgeType;

	/*========================================================================================
	Sobel Mask( 경계선 추출 )

	Vertical( nY )        Horizon(nX)
	-1 -2 -1			   -1  0  1
	 0  0  0               -2  0  2
	 1  2  1               -1  0  1
	 =========================================================================================*/
	

	for( i = 0 ; i < ptrImageDataSrc->nH; i++ )
	{
		for( j = 0 ; j < ptrImageDataSrc->nW;j++)
		{

			if ( i == 0 || i == ptrImageDataSrc->nH -1 || j == 0 || j == ptrImageDataSrc->nW - 1)
			{
				ptrImageDataSrc->ptrSrcImage2D[i][j] = 0;
			}
			else
			{
				nY = - m_TmpImageData.ptrSrcImage2D[i-1][j-1] - 2*m_TmpImageData.ptrSrcImage2D[i-1][j] - m_TmpImageData.ptrSrcImage2D[i-1][j+1]
				+ m_TmpImageData.ptrSrcImage2D[i+1][j-1] + 2*m_TmpImageData.ptrSrcImage2D[i+1][j] + m_TmpImageData.ptrSrcImage2D[i+1][j+1];

				nX = - m_TmpImageData.ptrSrcImage2D[i-1][j-1] - 2*m_TmpImageData.ptrSrcImage2D[i][j-1] - m_TmpImageData.ptrSrcImage2D[i+1][j-1]
				+ m_TmpImageData.ptrSrcImage2D[i-1][j+1] + 2*m_TmpImageData.ptrSrcImage2D[i][j+1] + m_TmpImageData.ptrSrcImage2D[i+1][j+1];			

				if ( LineEdgeType == HW_LINE_EDGE )
				{
					dSobel = sqrt( (double)( nX*nX + nY*nY ));
					ptrImageDataSrc->ptrSrcImage2D[i][j] = (BYTE)LIMIT_VAL( dSobel );
				}
				else if ( LineEdgeType == W_LINE_EDGE )
				{
					ptrImageDataSrc->ptrSrcImage2D[i][j] = (BYTE)LIMIT_VAL( nY );
				}
				else 
				{
					ptrImageDataSrc->ptrSrcImage2D[i][j] = (BYTE)LIMIT_VAL( nX );
				}
			}
			
		}
	}

	return ERR_SUCCESS;
}

/*======================================================================
영상 외곽선 검출( Canny )
========================================================================*/
static int Canny( pDIP_DATA ptrDIPData )
{
	IMAGE_DATA_t * ptrImageDataSrc = NULL;
	int i, j;
	int nErrCode =ERR_SUCCESS;
	int ** ptrDX_tbl, **ptrDY_tbl, **ptrMag_tbl;	
	int nMag, nDX, nDY, nSlope, nTh_low, nTh_High, nDirection;	
	BOOL bMaxima = TRUE;

	nTh_low = DEFAULT_CANNY_TH_LOW;
	nTh_High = DEFAULT_CANNY_TH_HEIGH;
	nDirection =0;

	ptrImageDataSrc = ptrDIPData->ptrImageDataSrc1;

	nErrCode = IsValidData( ptrImageDataSrc , 8 );
	if ( nErrCode != ERR_SUCCESS )
		return nErrCode ;		
	

	if ( HLP_CopyImageDataNotAlloc( &m_TmpImageData, ptrImageDataSrc) != ERR_SUCCESS )
		return ERR_INVALID_DATA;

	ptrDX_tbl = ptrDY_tbl = ptrMag_tbl = NULL;

	for( i = 0; i < ptrImageDataSrc->nH ; i++ )
		for( j = 0 ; j < ptrImageDataSrc->nW ; j++ )
			ptrImageDataSrc->ptrSrcImage2D[ i ][ j ] = 0;

	/* Create X buf */
	if ( ( ptrDX_tbl = HLP_IntAllocMatrix( ptrImageDataSrc->nH, ptrImageDataSrc->nW ) )== NULL )
	{
		return ERR_INVALID_DATA;
	}		

	/* Create Y buf */
	if ( ( ptrDY_tbl = HLP_IntAllocMatrix( ptrImageDataSrc->nH, ptrImageDataSrc->nW ) )== NULL )
	{
		HLP_IntFreeMatrix( ptrDX_tbl, ptrImageDataSrc->nH, ptrImageDataSrc->nW);
		return ERR_INVALID_DATA;
	}
	
	/* Magnitude buf */
	if ( ( ptrMag_tbl = HLP_IntAllocMatrix( ptrImageDataSrc->nH, ptrImageDataSrc->nW ) )== NULL )
	{
		HLP_IntFreeMatrix( ptrDX_tbl, ptrImageDataSrc->nH, ptrImageDataSrc->nW);
		HLP_IntFreeMatrix( ptrDY_tbl, ptrImageDataSrc->nH, ptrImageDataSrc->nW);
		return ERR_INVALID_DATA;
	}	
	
	/* sobel 
	Vertical( nY )        Horizon(nX)
		-1 -2 -1			   -1  0  1
		0  0  0               -2  0  2
		1  2  1               -1  0  1
	 */

	for( i = 1 ; i < ptrImageDataSrc->nH -1; i++ )
	{
		for( j = 1 ; j < ptrImageDataSrc->nW -1;j++)
		{	
			nDY= - m_TmpImageData.ptrSrcImage2D[i-1][j-1] - 2*m_TmpImageData.ptrSrcImage2D[i-1][j] - m_TmpImageData.ptrSrcImage2D[i-1][j+1]
			+ m_TmpImageData.ptrSrcImage2D[i+1][j-1] + 2*m_TmpImageData.ptrSrcImage2D[i+1][j] + m_TmpImageData.ptrSrcImage2D[i+1][j+1];

			nDX = - m_TmpImageData.ptrSrcImage2D[i-1][j-1] - 2*m_TmpImageData.ptrSrcImage2D[i][j-1] - m_TmpImageData.ptrSrcImage2D[i+1][j-1]
			+ m_TmpImageData.ptrSrcImage2D[i-1][j+1] + 2*m_TmpImageData.ptrSrcImage2D[i][j+1] + m_TmpImageData.ptrSrcImage2D[i+1][j+1];

			/* magnitude */
			//nMag = abs( nDX ) + abs( nDY);
			nMag = (int)sqrt( (double)( nDX * nDX + nDY*nDY));

			ptrDX_tbl[ i ][ j ]  = nDX;
			ptrDY_tbl[ i ][ j ]  = nDY;
			ptrMag_tbl[ i ][ j ] = nMag;
		}
	}
	
	for( i = 1 ; i < ptrImageDataSrc->nH -1; i++ )
	{
		for( j = 1 ; j < ptrImageDataSrc->nW -1;j++)
		{
			nMag = ptrMag_tbl[ i ][ j ];
			
			/* 엣지 후보 선정 ( edge Orientation )*/
			if ( nMag > nTh_low )
			{
				nDX = ptrDX_tbl[ i ][ j ];
				nDY = ptrDY_tbl[ i ][ j ];

				if ( nDX != 0 )
				{
					/* Edge Orientation */
					nSlope = ( nDY << fBit ) / nDX;
					if ( nSlope > 0 )
					{
						if ( nSlope < tan225)
							nDirection = 0;
						else if ( nSlope < tan675 )
							nDirection = 1;
						else
							nDirection = 2;
					}
					else
					{
						if ( -nSlope > tan675 )
							nDirection = 2;
						else if ( -nSlope > tan225 )
							nDirection = 3;
						else
							nDirection =0;
					}
				}
				else
				{
					nDirection = 2;
				}
				
				/* NON-MAXIA 설정 */
				bMaxima = TRUE;
				if ( nDirection == 0 )
				{
					if ( nMag < ptrMag_tbl[ i ][ j -1 ] || nMag < ptrMag_tbl[ i ][ j + 1 ])
						bMaxima = FALSE;

				}

				if ( nDirection == 2 )
				{
					if ( nMag < ptrMag_tbl[ i + 1 ][ j ] || nMag < ptrMag_tbl[ i -1 ][ j ])
						bMaxima = FALSE;						
				}
				else if ( nDirection == 1 )
				{
					if ( nMag < ptrMag_tbl[ i + 1 ][ j +1 ] || nMag < ptrMag_tbl[ i -1 ][ j -1 ])
						bMaxima = FALSE;
				}

				else if ( nDirection == 2 )
				{
					if ( nMag < ptrMag_tbl[ i + 1 ][ j ] || nMag < ptrMag_tbl[ i -1 ][ j ])
						bMaxima = FALSE;
				}
				else //if ( nDirection == 3 )
				{
					if ( nMag < ptrMag_tbl[ i + 1 ][ j -1 ] || nMag < ptrMag_tbl[ i -1 ][ j + 1])
						bMaxima = FALSE;
				}
				
				if ( bMaxima )
				{
					if ( nMag > nTh_High )
					{
						ptrImageDataSrc->ptrSrcImage2D[ i ][ j ] = CERTAIN_EDGE;
					}
					else
					{
						ptrImageDataSrc->ptrSrcImage2D[ i ][ j ] = PROBABLE_EDGE;
					}
				}

			}
		}
	}

	BOOL bDone = FALSE;
	BYTE * ptrTemp;

	/* Hysteresis analysis ( 엣지선 연결 )*/
	while( bDone != TRUE )
	{
		bDone = TRUE;

		for( i =1; i < ptrImageDataSrc->nH -1; i++ )
		{
			for( j=1 ; j < ptrImageDataSrc->nW -1; j++ )
			{
				ptrTemp = ( BYTE*)&ptrImageDataSrc->ptrSrcImage2D[ i ][ j + 1 ];
				if ( *ptrTemp == PROBABLE_EDGE)
				{
					*ptrTemp = CERTAIN_EDGE;
					bDone = FALSE;
				}

				ptrTemp = ( BYTE*)&ptrImageDataSrc->ptrSrcImage2D[ i ][ j - 1 ];
				if ( *ptrTemp == PROBABLE_EDGE)
				{
					*ptrTemp = CERTAIN_EDGE;
					bDone = FALSE;
				}
				
				ptrTemp = ( BYTE*)&ptrImageDataSrc->ptrSrcImage2D[ i + 1 ][ j ];
				if ( *ptrTemp == PROBABLE_EDGE)
				{
					*ptrTemp = CERTAIN_EDGE;
					bDone = FALSE;
				}
				
				ptrTemp = ( BYTE*)&ptrImageDataSrc->ptrSrcImage2D[ i - 1 ][ j ];
				if ( *ptrTemp == PROBABLE_EDGE)
				{
					*ptrTemp = CERTAIN_EDGE;
					bDone = FALSE;
				}

				ptrTemp = ( BYTE*)&ptrImageDataSrc->ptrSrcImage2D[ i - 1 ][ j - 1 ];
				if ( *ptrTemp == PROBABLE_EDGE)
				{
					*ptrTemp = CERTAIN_EDGE;
					bDone = FALSE;
				}

				ptrTemp = ( BYTE*)&ptrImageDataSrc->ptrSrcImage2D[ i - 1 ][ j + 1 ];
				if ( *ptrTemp == PROBABLE_EDGE)
				{
					*ptrTemp = CERTAIN_EDGE;
					bDone = FALSE;
				}

				ptrTemp = ( BYTE*)&ptrImageDataSrc->ptrSrcImage2D[ i + 1 ][ j - 1 ];
				if ( *ptrTemp == PROBABLE_EDGE)
				{
					*ptrTemp = CERTAIN_EDGE;
					bDone = FALSE;
				}

				ptrTemp = ( BYTE*)&ptrImageDataSrc->ptrSrcImage2D[ i + 1 ][ j + 1 ];
				if ( *ptrTemp == PROBABLE_EDGE)
				{
					*ptrTemp = CERTAIN_EDGE;
					bDone = FALSE;
				}
			}
		}
	}
	
	HLP_IntFreeMatrix( ptrDX_tbl, ptrImageDataSrc->nH, ptrImageDataSrc->nW);
	HLP_IntFreeMatrix( ptrDY_tbl, ptrImageDataSrc->nH, ptrImageDataSrc->nW);
	HLP_IntFreeMatrix( ptrMag_tbl, ptrImageDataSrc->nH, ptrImageDataSrc->nW);

	return ERR_SUCCESS;
}

/*======================================================================
영상 침식
분할 방식 : 
   SEGMENT_BINARY      : 3*3 Mask에서 하나라도 0 이면 0
   Not SEGMENT_BINARY  : 3*3 Mask에서 가장 작은 값
========================================================================*/
static int Erosion( pDIP_DATA ptrDIPData )
{
	IMAGE_DATA_t  *ptrImageDataSrc = NULL;
	int i, j, m, n;
	int nErrCode =ERR_SUCCESS;
	pSEGMENT_PARAM ptrSegment = NULL;
	BYTE SegmentType = SEGMENT_BINARY;
	int  pMin, x, y ;

	ptrImageDataSrc = ptrDIPData->ptrImageDataSrc1;
	ptrSegment = ( pSEGMENT_PARAM ) ptrDIPData->ptrInputParam;
		
	nErrCode = IsValidData( ptrImageDataSrc , 8 );
	if ( nErrCode != ERR_SUCCESS )
		return nErrCode;
	
	nErrCode = HLP_CopyImageDataNotAlloc( &m_TmpImageData, ptrImageDataSrc );
	if ( nErrCode != ERR_SUCCESS )
		return nErrCode;

	if ( ptrSegment != NULL )
	{
		SegmentType = ptrSegment->SegmentType;
	}


	if ( SegmentType == SEGMENT_BINARY )
	{
		for( i = 1 ; i < m_TmpImageData.nH -1 ; i++ )
		{
			for( j =1 ; j < m_TmpImageData.nW -1 ;j++ )
			{
				if ( m_TmpImageData.ptrSrcImage2D[ i ][ j ] != 0 )
				{
					if ( m_TmpImageData.ptrSrcImage2D[ i-1 ][ j -1 ] == 0 ||
						m_TmpImageData.ptrSrcImage2D[ i-1 ][ j    ] == 0 ||
						m_TmpImageData.ptrSrcImage2D[ i-1 ][ j +1 ] == 0 ||
						m_TmpImageData.ptrSrcImage2D[ i   ][ j -1 ] == 0 ||
						m_TmpImageData.ptrSrcImage2D[ i   ][ j +1 ] == 0 ||
						m_TmpImageData.ptrSrcImage2D[ i+1 ][ j -1 ] == 0 ||
						m_TmpImageData.ptrSrcImage2D[ i+1 ][ j	   ] == 0 ||
						m_TmpImageData.ptrSrcImage2D[ i+1 ][ j+1  ] == 0 )
					{
						ptrImageDataSrc->ptrSrcImage2D[ i ][ j ] = 0;
					}
				}
			}
		}	
	}
	else
	{
		for( i = 1 ; i < m_TmpImageData.nH -1 ; i++ )
		{
			for( j =1 ; j < m_TmpImageData.nW -1 ;j++ )
			{
				pMin = 255;

				for( n = -1 ; n <= 1 ; n++ )
				{
					for( m = -1 ; m <= 1 ; m++ )
					{
						y = i + n;
						x = j + m;

						if( x >= 0 && x < ptrImageDataSrc->nW && y >= 0 && y < ptrImageDataSrc->nH )
						{
							if( m_TmpImageData.ptrSrcImage2D[y][x] < pMin )
							{
								pMin = m_TmpImageData.ptrSrcImage2D[y][x];
							}
						}
					}
				}
				ptrImageDataSrc->ptrSrcImage2D[ i ][ j ] = (BYTE)pMin;
			}
		}
	}
		
	return ERR_SUCCESS;
}

/*======================================================================
영상 팽창
분할 방식 : 
   SEGMENT_BINARY      : 3*3 Mask에서 하나라도 255 이면 255
   Not SEGMENT_BINARY  : 3*3 Mask에서 가장 큰 값
========================================================================*/
static int Dilation( pDIP_DATA ptrDIPData )
{
	IMAGE_DATA_t  *ptrImageDataSrc = NULL;
	int i, j, m, n;
	int nErrCode =ERR_SUCCESS;
	pSEGMENT_PARAM ptrSegment = NULL;
	BYTE SegmentType = SEGMENT_BINARY;
	int  pMax, x, y ;

	ptrImageDataSrc = ptrDIPData->ptrImageDataSrc1;
	ptrSegment = ( pSEGMENT_PARAM ) ptrDIPData->ptrInputParam;

	nErrCode = IsValidData( ptrImageDataSrc , 8 );
	if ( nErrCode != ERR_SUCCESS )
		return nErrCode;

	nErrCode = HLP_CopyImageDataNotAlloc( &m_TmpImageData, ptrImageDataSrc );
	if ( nErrCode != ERR_SUCCESS )
		return nErrCode;

	if ( ptrSegment != NULL )
	{
		SegmentType = ptrSegment->SegmentType;
	}

	if ( SegmentType == SEGMENT_BINARY )
	{
		for( i = 1 ; i < m_TmpImageData.nH -1 ; i++ )
		{
			for( j =1 ; j < m_TmpImageData.nW -1 ;j++ )
			{
				if ( m_TmpImageData.ptrSrcImage2D[ i ][ j ] == 0 )
				{
					if ( m_TmpImageData.ptrSrcImage2D[ i-1 ][ j -1 ] != 0 ||
						m_TmpImageData.ptrSrcImage2D[ i-1 ][ j    ] != 0 ||
						m_TmpImageData.ptrSrcImage2D[ i-1 ][ j +1 ] != 0 ||
						m_TmpImageData.ptrSrcImage2D[ i   ][ j -1 ] != 0 ||
						m_TmpImageData.ptrSrcImage2D[ i   ][ j +1 ] != 0 ||
						m_TmpImageData.ptrSrcImage2D[ i+1 ][ j -1 ] != 0 ||
						m_TmpImageData.ptrSrcImage2D[ i+1 ][ j	   ] != 0 ||
						m_TmpImageData.ptrSrcImage2D[ i+1 ][ j+1  ] != 0 )
					{
						ptrImageDataSrc->ptrSrcImage2D[ i ][ j ] = 255;
					}
				}
			}
		}	
	}
	else
	{
		for( i = 1 ; i < m_TmpImageData.nH ; i++ )
		{
			for( j =1 ; j < m_TmpImageData.nW ;j++ )
			{
				pMax = 0;
				for( n = -1 ; n <= 1 ; n++ )
				{
					for( m = -1 ; m <= 1 ; m++ )
					{
						y = i + n;
						x = j + m;

						if( x >= 0 && x < ptrImageDataSrc->nW && y >= 0 && y < ptrImageDataSrc->nH )
						{
							if( m_TmpImageData.ptrSrcImage2D[y][x] > pMax )
							{
								pMax = m_TmpImageData.ptrSrcImage2D[y][x];
							}
						}
					}
				}
				ptrImageDataSrc->ptrSrcImage2D[ i ][ j ] = (BYTE)pMax;
			}
		}
	}
	return ERR_SUCCESS;
}

/*======================================================================
영상 뺄셈
========================================================================*/
static int SubImage( pDIP_DATA ptrDIPData )
{
	IMAGE_DATA_t *ptrImageDataSrc1, *ptrImageDataSrc2 , *ptrImageDataDest= NULL;
	int i, j;
	int nErrCode =ERR_SUCCESS;

	ptrImageDataSrc1 = ptrImageDataSrc2 = ptrImageDataDest = NULL;

	ptrImageDataSrc1 = ptrDIPData->ptrImageDataSrc1;
	nErrCode = IsValidData( ptrImageDataSrc1 , 8 );
	if ( nErrCode != ERR_SUCCESS )
		return nErrCode;

	ptrImageDataSrc2 = ptrDIPData->ptrImageDataSrc2;
	nErrCode = IsValidData( ptrImageDataSrc2 , 8 );
	if ( nErrCode != ERR_SUCCESS )
		return nErrCode;

	ptrImageDataDest = ptrDIPData->ptrImageDataDest;
	nErrCode = IsValidData( ptrImageDataDest , 8 );
	if ( nErrCode != ERR_SUCCESS )
		return nErrCode;

	for( i = 0 ; i < ptrImageDataSrc1->nH ; i++ )
	{
		for( j =0 ; j < ptrImageDataSrc1->nW ;j++ )
		{	
			ptrImageDataDest->ptrSrcImage2D[ i ][ j ] = (BYTE)LIMIT_VAL( ptrImageDataSrc1->ptrSrcImage2D[ i ][ j ] - ptrImageDataSrc2->ptrSrcImage2D[ i ][ j ]);		
		}
	}
	
	return ERR_SUCCESS;
}
/*======================================================================
영상 정렬
========================================================================*/
static int InsertSort( int * ptrBuf, int nCnt )
{
	int i, j;
	int temp;

	for( i = 1 ; i < nCnt ; i++ )
	{
		temp = ptrBuf[i];
		j = i - 1;

		while( j > -1 && ptrBuf[j] > temp )
		{
			ptrBuf[j+1] = ptrBuf[j];
			j--;
		}

		ptrBuf[j+1] = temp;
	}

	return ERR_SUCCESS;
}

/*======================================================================
중간값 필터
========================================================================*/
static int Median( pDIP_DATA ptrDIPData )
{
	IMAGE_DATA_t  *ptrImageDataSrc = NULL;
	int i, j;
	int nErrCode =ERR_SUCCESS;
	int nM[ MEDIAN_SORT_CNT ];

	ptrImageDataSrc = ptrDIPData->ptrImageDataSrc1;
	nErrCode = IsValidData( ptrImageDataSrc , 8 );
	if ( nErrCode != ERR_SUCCESS )
		return nErrCode;

	for( i = 1; i < ptrImageDataSrc->nH -1 ; i++ )
	{
		for( j =1 ; j < ptrImageDataSrc->nW -1 ; j++ )
		{			
			nM[0] = ptrImageDataSrc->ptrSrcImage2D[i-1][j-1]; 
			nM[1] = ptrImageDataSrc->ptrSrcImage2D[i-1][j]; 
			nM[2] = ptrImageDataSrc->ptrSrcImage2D[i-1][j+1];	
			nM[3] = ptrImageDataSrc->ptrSrcImage2D[i  ][j-1]; 
			nM[4] = ptrImageDataSrc->ptrSrcImage2D[i  ][j]; 
			nM[5] = ptrImageDataSrc->ptrSrcImage2D[i  ][j+1];
			nM[6] = ptrImageDataSrc->ptrSrcImage2D[i+1][j-1]; 
			nM[7] = ptrImageDataSrc->ptrSrcImage2D[i+1][j]; 
			nM[8] = ptrImageDataSrc->ptrSrcImage2D[i+1][j+1];

			InsertSort( nM, MEDIAN_SORT_CNT );

			ptrImageDataSrc->ptrSrcImage2D[ i ][ j ] = ( BYTE)nM[ 4 ];
		}
	}

	return ERR_SUCCESS;

}

/*======================================================================
평균값 필터
========================================================================*/
static int Mean( pDIP_DATA ptrDIPData )
{
	IMAGE_DATA_t *ptrImageDataSrc = NULL;
	int i, j;
	int nErrCode =ERR_SUCCESS;
	int nSum=0;

	ptrImageDataSrc = ptrDIPData->ptrImageDataSrc1;
	nErrCode = IsValidData( ptrImageDataSrc , 8 );
	if ( nErrCode != ERR_SUCCESS )
		return nErrCode;
	
	HLP_CopyImageDataNotAlloc( &m_TmpImageData, ptrImageDataSrc );

	for( i = 1; i < ptrImageDataSrc->nH -1 ; i++ )
	{
		for( j =1 ; j < ptrImageDataSrc->nW -1 ; j++ )
		{			
			nSum =m_TmpImageData.ptrSrcImage2D[i-1][j-1] + m_TmpImageData.ptrSrcImage2D[i-1][j] + m_TmpImageData.ptrSrcImage2D[i-1][j+1]
			+ m_TmpImageData.ptrSrcImage2D[i][j-1] + m_TmpImageData.ptrSrcImage2D[i][j] + m_TmpImageData.ptrSrcImage2D[i][j+1]
			+ m_TmpImageData.ptrSrcImage2D[i+1][j-1] + m_TmpImageData.ptrSrcImage2D[i+1][j] + m_TmpImageData.ptrSrcImage2D[i+1][j+1];
			

			ptrImageDataSrc->ptrSrcImage2D[i][j] = (BYTE)LIMIT_VAL(nSum/9. + 0.5);
		}
	}

	return ERR_SUCCESS;
}

/*======================================================================
가중 평균값 필터
========================================================================*/
static int WeightedMean( pDIP_DATA ptrDIPData )
{
	IMAGE_DATA_t *ptrImageDataSrc = NULL;
	int i, j;
	int nErrCode =ERR_SUCCESS;
	int nSum=0;

	ptrImageDataSrc = ptrDIPData->ptrImageDataSrc1;
	nErrCode = IsValidData( ptrImageDataSrc , 8 );
	if ( nErrCode != ERR_SUCCESS )
		return nErrCode;
	
	HLP_CopyImageDataNotAlloc( &m_TmpImageData, ptrImageDataSrc );

	for( i = 1; i < ptrImageDataSrc->nH -1 ; i++ )
	{
		for( j =1 ; j < ptrImageDataSrc->nW -1 ; j++ )
		{			
			nSum =m_TmpImageData.ptrSrcImage2D[i-1][j-1] + 2*m_TmpImageData.ptrSrcImage2D[i-1][j] + m_TmpImageData.ptrSrcImage2D[i-1][j+1]
				+ 2*m_TmpImageData.ptrSrcImage2D[i][j-1] + 4*m_TmpImageData.ptrSrcImage2D[i][j] + 2*m_TmpImageData.ptrSrcImage2D[i][j+1]
				+ m_TmpImageData.ptrSrcImage2D[i+1][j-1] + 2*m_TmpImageData.ptrSrcImage2D[i+1][j] + m_TmpImageData.ptrSrcImage2D[i+1][j+1];


			ptrImageDataSrc->ptrSrcImage2D[i][j] = (BYTE)LIMIT_VAL(nSum/16. + 0.5);
		}
	}
	

	return ERR_SUCCESS;
}

/*======================================================================
영상 콘트라스트
========================================================================*/
static int Contrast( pDIP_DATA ptrDIPData )
{
	IMAGE_DATA_t *ptrImageDataSrc = NULL;
	int i, j;
	int nErrCode =ERR_SUCCESS;

	BYTE bVal ;
	int nRatio = DEFAULT_CONTRAST_RATIO;
	pCONTRAST_PARAM ptrParam = NULL;

	ptrImageDataSrc = ptrDIPData->ptrImageDataSrc1;
	ptrParam = ( pCONTRAST_PARAM )ptrDIPData->ptrInputParam;

	if ( ptrParam == NULL )
		nRatio = DEFAULT_CONTRAST_RATIO;
	else
		nRatio = ptrParam->nRatio;

	nErrCode = IsValidData( ptrImageDataSrc , 8 );
	if ( nErrCode != ERR_SUCCESS )
		return nErrCode;
	
	for( i = 0; i < ptrImageDataSrc->nH ; i++ )
	{
		for( j =0 ; j < ptrImageDataSrc->nW ; j++ )
		{			
			bVal = ptrImageDataSrc->ptrSrcImage2D[ i ][ j ];
			ptrImageDataSrc->ptrSrcImage2D[ i ][ j ] = ( BYTE)LIMIT_VAL( bVal + (( bVal - 128 ) * nRatio / 100 ));
		}
	}
	
	return ERR_SUCCESS;
}

/*======================================================================
이진 영상에서의 영상 잡음 제거 
자신의 4이웃 기준으로 1이상의 이웃객체가 없을 경우 잡음으로 판단. 
========================================================================*/
static int Binarization_DeNoise( pDIP_DATA ptrDIPData )
{
	IMAGE_DATA_t * ptrImageDataSrc = NULL;
	
	int i, j;
	int nErrCode = 0;	
	int bCon ;
	int nLoopCnt = 0;
	int nBCnt = 0;

	ptrImageDataSrc = ptrDIPData->ptrImageDataSrc1;
	nErrCode = IsValidData( ptrImageDataSrc , 8 );
	if ( nErrCode != ERR_SUCCESS )
		return nErrCode ;
		
	
	for( i = 0 ; i < ptrImageDataSrc->nH ; i++ )
	{
		for( j = 0 ; j < ptrImageDataSrc->nW ;j++ )
		{
			if ( ptrImageDataSrc->ptrSrcImage2D[ i ][ j ] ==255 )
			{
				nBCnt = 0;
				if( j -1 > 0 && ptrImageDataSrc->ptrSrcImage2D[ i ][ j-1] == 255 )  nBCnt++;
				if( i -1 > 0 && ptrImageDataSrc->ptrSrcImage2D[ i-1][j] == 255 )    nBCnt++;
				if( j+ 1 < ptrImageDataSrc->nW && ptrImageDataSrc->ptrSrcImage2D[ i][ j +1] == 255 ) nBCnt++;
				if( i+ 1 < ptrImageDataSrc->nH && ptrImageDataSrc->ptrSrcImage2D[ i+1][ j] == 255 ) nBCnt++;
				
				if( nBCnt < 1)
				{
					ptrImageDataSrc->ptrSrcImage2D[ i ][ j ] = 0;
				}
			}
				 
		}
	}

	bCon =1;
	nLoopCnt = 0;

	while ( bCon && nLoopCnt < 5 )
	{
		bCon = 0;
		nLoopCnt++;

		for( i = 1 ; i < ptrImageDataSrc->nH -1; i++ )
		{
			for( j = 1 ; j < ptrImageDataSrc->nW -1;j++ )
			{
				if ( ptrImageDataSrc->ptrSrcImage2D[ i ][ j ] ==255 )
				{	
					if( ptrImageDataSrc->ptrSrcImage2D[ i- 1][ j ] == 0  && 
						ptrImageDataSrc->ptrSrcImage2D[ i+ 1][ j ] == 0  &&
						ptrImageDataSrc->ptrSrcImage2D[ i- 1][ j+1 ] == 255  &&
						ptrImageDataSrc->ptrSrcImage2D[ i   ][ j+1 ] == 255  &&
						ptrImageDataSrc->ptrSrcImage2D[ i+1 ][ j +1 ] == 255)
					{
						ptrImageDataSrc->ptrSrcImage2D[ i ][ j ] = 0;
						bCon =1;
					}
				}

				if ( ptrImageDataSrc->ptrSrcImage2D[ i ][ j ] ==255 )
				{	
					if( ptrImageDataSrc->ptrSrcImage2D[ i- 1][ j ] == 0  && 
						ptrImageDataSrc->ptrSrcImage2D[ i+ 1][ j ] == 0  &&
						ptrImageDataSrc->ptrSrcImage2D[ i- 1][ j-1 ] == 255  &&
						ptrImageDataSrc->ptrSrcImage2D[ i   ][ j-1 ] == 255  &&
						ptrImageDataSrc->ptrSrcImage2D[ i+1 ][ j-1 ] == 255)
					{
						ptrImageDataSrc->ptrSrcImage2D[ i ][ j ] = 0;
						bCon =1;
					}
				}

				if ( ptrImageDataSrc->ptrSrcImage2D[ i ][ j ] ==255 )
				{	
					if( ptrImageDataSrc->ptrSrcImage2D[ i][ j -1] == 0  && 
						ptrImageDataSrc->ptrSrcImage2D[ i][ j +1] == 0  &&
						ptrImageDataSrc->ptrSrcImage2D[ i-1][j-1] == 255  &&
						ptrImageDataSrc->ptrSrcImage2D[ i-1][j  ] == 255  &&
						ptrImageDataSrc->ptrSrcImage2D[ i-1][j+1] == 255)
					{
						ptrImageDataSrc->ptrSrcImage2D[ i ][ j ] = 0;
						bCon =1;
					}
				}

				if ( ptrImageDataSrc->ptrSrcImage2D[ i ][ j ] ==255 )
				{	
					if( ptrImageDataSrc->ptrSrcImage2D[ i][ j -1] == 0  && 
						ptrImageDataSrc->ptrSrcImage2D[ i][ j +1] == 0  &&
						ptrImageDataSrc->ptrSrcImage2D[ i+1][j-1] == 255  &&
						ptrImageDataSrc->ptrSrcImage2D[ i+1][j  ] == 255  &&
						ptrImageDataSrc->ptrSrcImage2D[ i+1][j+1] == 255)
					{
						ptrImageDataSrc->ptrSrcImage2D[ i ][ j ] = 0;
						bCon =1;
					}
				}

			}
		}
	}


	return ERR_SUCCESS;
}

/*======================================================================
 입방 보간 알고리즘
========================================================================*/
static double cubic_interpolation(double v1, double v2, double v3, double v4, double d) 
{
	double v, p1, p2, p3, p4;

	p1 = v2;
	p2 = -v1 + v3;
	p3 = 2*(v1 - v2) + v3 - v4;
	p4 = -v1 + v2 - v3 + v4;

	v = p1 + d*(p2 + d*(p3 + d*p4));

	return v;
}
/*======================================================================
영상 변경( 확대/축소 )
========================================================================*/
static int ResizeCubic( pDIP_DATA ptrDIPData )
{
	IMAGE_DATA_t *ptrImageDataSrc1, *ptrImageDataDest;
	int i, j;
	int nErrCode =ERR_SUCCESS;
	int x1, x2, x3, x4, y1, y2, y3, y4;
	double v1, v2, v3, v4, v;
	double rx, ry, p, q;
	int nOrgH, nOrgW, nNewH, nNewW;

	ptrImageDataSrc1 =  ptrImageDataDest = NULL;

	ptrImageDataSrc1 = ptrDIPData->ptrImageDataSrc1;
	//nErrCode = IsValidData( ptrImageDataSrc1 , 8 );
	if ( nErrCode != ERR_SUCCESS )
		return nErrCode;

	ptrImageDataDest = ptrDIPData->ptrImageDataDest;
	//nErrCode = IsValidData( ptrImageDataDest , 8 );
	if ( nErrCode != ERR_SUCCESS )
		return nErrCode;

	
	nOrgH = ptrImageDataSrc1->nH;
	nOrgW = ptrImageDataSrc1->nW;
	nNewH = ptrImageDataDest->nH;
	nNewW = ptrImageDataDest->nW;

	for( j = 0 ; j < ptrImageDataDest->nH ; j++ )
	{
		for( i = 0 ; i < ptrImageDataDest->nW ; i++ )
		{
			rx = (double)nOrgW*i/nNewW;
			ry = (double)nOrgH*j/nNewH;

			x2 = (int)rx;
			x1 = x2 - 1; if( x1 <  0 ) x1 = 0;
			x3 = x2 + 1; if( x3 >= nOrgW ) x3 = nOrgW - 1;
			x4 = x2 + 2; if( x4 >= nOrgW ) x4 = nOrgW - 1;
			p  = rx - x2;

			y2 = (int)ry;
			y1 = y2 - 1; if( y1 <  0 ) y1 = 0;
			y3 = y2 + 1; if( y3 >= nOrgH ) y3 = nOrgH - 1;
			y4 = y2 + 2; if( y4 >= nOrgH ) y4 = nOrgH - 1;
			q  = ry - y2;

			v1 = cubic_interpolation(ptrImageDataSrc1->ptrSrcImage2D[y1][x1], ptrImageDataSrc1->ptrSrcImage2D[y1][x2], ptrImageDataSrc1->ptrSrcImage2D[y1][x3], ptrImageDataSrc1->ptrSrcImage2D[y1][x4], p);
			v2 = cubic_interpolation(ptrImageDataSrc1->ptrSrcImage2D[y2][x1], ptrImageDataSrc1->ptrSrcImage2D[y2][x2], ptrImageDataSrc1->ptrSrcImage2D[y2][x3], ptrImageDataSrc1->ptrSrcImage2D[y2][x4], p);
			v3 = cubic_interpolation(ptrImageDataSrc1->ptrSrcImage2D[y3][x1], ptrImageDataSrc1->ptrSrcImage2D[y3][x2], ptrImageDataSrc1->ptrSrcImage2D[y3][x3], ptrImageDataSrc1->ptrSrcImage2D[y3][x4], p);
			v4 = cubic_interpolation(ptrImageDataSrc1->ptrSrcImage2D[y4][x1], ptrImageDataSrc1->ptrSrcImage2D[y4][x2], ptrImageDataSrc1->ptrSrcImage2D[y4][x3], ptrImageDataSrc1->ptrSrcImage2D[y4][x4], p);

			v  = cubic_interpolation(v1, v2, v3, v4, q);

			ptrImageDataDest->ptrSrcImage2D[j][i] = (BYTE)LIMIT_VAL(v);
		}
	}

	return ERR_SUCCESS;
}

/*======================================================================
 비등방성 확산 필터( 저주파 )
========================================================================*/
static int Diffusion( pDIP_DATA ptrDIPData )
{
	/* Lamda = 0.25 ( 0<= lamda <= 0.25 ), fK = 4를 유지 해야 함 
	   nIter 3 ~ 30
	   반복 해수가 많을 수록 좋은 결과를 가져 오지만
	   그만큼 시간이 많이 소요된다. 
	*/
	float fLambda = 0.25;
	float fK = 4 ;
	int nIter = 5;
	float fK2 =0;

	float gradn, grads, grade, gradw;
	float gcn, gcs, gce, gcw;
	float fTmp;
	IMAGE_DATA_t *ptrImageDataSrc= NULL;
	int nErrCode = ERR_SUCCESS;
	int i, j, k;
	pDIFFUSION_PARAM ptrParam= NULL;

	ptrParam = (pDIFFUSION_PARAM)ptrDIPData->ptrInputParam;
	ptrImageDataSrc = ptrDIPData->ptrImageDataSrc1;

	nErrCode = IsValidData( ptrImageDataSrc , 8 );
	
	if ( nErrCode != ERR_SUCCESS )
		return nErrCode;

	if ( ptrParam != NULL )
	{
		nIter = ptrParam->nIter;
	}
	
	fK2 = fK*fK;

	for( i = 0 ; i < ptrImageDataSrc->nH ; i ++ )
	{
		for( j = 0 ; j < ptrImageDataSrc->nW ; j++ )
		{
			m_diff_ptrCpyBuf[ i ][ j ] = (float)ptrImageDataSrc->ptrSrcImage2D[ i ][ j ];
		}
	}

	for( k = 0 ; k < nIter ; k++ )
	{
		for( i = 1 ; i < ptrImageDataSrc->nH -1; i ++ )
		{
			for( j = 1 ; j < ptrImageDataSrc->nW -1; j++ )
			{
				fTmp = m_diff_ptrCpyBuf[ i ][ j ];

				gradn = m_diff_ptrCpyBuf[i-1][j  ] - fTmp;
				grads = m_diff_ptrCpyBuf[i+1][j  ] - fTmp;
				grade = m_diff_ptrCpyBuf[i  ][j-1] - fTmp;
				gradw = m_diff_ptrCpyBuf[i  ][j+1] - fTmp;

				gcn = gradn / (1.0f + gradn*gradn/fK2);
				gcs = grads / (1.0f + grads*grads/fK2);
				gce = grade / (1.0f + grade*grade/fK2);
				gcw = gradw / (1.0f + gradw*gradw/fK2);

				m_diff_ptrTmpBuf[i][j] = m_diff_ptrCpyBuf[i][j] + fLambda*(gcn + gcs + gce + gcw);
			}
		}

		for( i = 0 ; i < ptrImageDataSrc->nH ; i++ )
		{
			memcpy( m_diff_ptrCpyBuf[ i ] , m_diff_ptrTmpBuf[ i ], sizeof( float ) * ptrImageDataSrc->nW );
		}
	}


	for( i = 0 ; i < ptrImageDataSrc->nH ; i ++ )
	{
		for( j = 0 ; j < ptrImageDataSrc->nW ; j++ )
		{
			ptrImageDataSrc->ptrSrcImage2D[ i ][ j ] = LIMIT_VAL( (BYTE)m_diff_ptrCpyBuf[ i ][ j ] );
		}
	}

	return ERR_SUCCESS;
}

#define MMSE_MASK_SIZE	5

/*======================================================================
(Minimum Mean Squared Error) 
경계선을 보존하면서 곙계선 주변의 잡음제거 필터
========================================================================*/
static int MMSE( pDIP_DATA ptrDIPData )
{
	IMAGE_DATA_t *ptrImageDataSrc= NULL;
	int nErrCode = ERR_SUCCESS;
	int i, j, x, y, k;
	
	double fNoise_Var = 100.0;
	double fSum, fAve, fDid, fVar, fRatio;
	int nPos;
	int nMask_H , nMask_W, nMask_Size;
	BYTE Mask[ MMSE_MASK_SIZE * MMSE_MASK_SIZE  ];

	ptrImageDataSrc = ptrDIPData->ptrImageDataSrc1;
	nErrCode = IsValidData( ptrImageDataSrc , 8 );	
	if ( nErrCode != ERR_SUCCESS )
		return nErrCode;
	
	HLP_CopyImageDataNotAlloc( &m_TmpImageData, ptrImageDataSrc );

	nMask_H = nMask_W = MMSE_MASK_SIZE;
	nMask_Size = ( nMask_H * nMask_W );
	memset( Mask, 0, sizeof( Mask ));

	for( i = 0 ; i < m_TmpImageData.nH - nMask_H ; i++ )
	{
		for( j = 0 ; j < m_TmpImageData.nW - nMask_W ; j++ )
		{
			nPos = 0;
			fSum = 0.0;
			fAve = 0.0;
			fVar = 0.0;
			fRatio = 0.0;

			for( y = 0 ; y < nMask_H ; y++ )
			{
				for( x = 0 ; x < nMask_W ; x++ )
				{
					Mask[ nPos ] = m_TmpImageData.ptrSrcImage2D[ i + y ][ j + x ];
					fSum += (double)m_TmpImageData.ptrSrcImage2D[ i + y ][ j + x ];
					nPos++;
				}
			}

			fAve = fSum / nMask_Size;
			fSum = 0.0;

			for( k = 0 ; k < nMask_Size ; k++ )
			{
				fDid = ( Mask[ k ] - fAve );
				fSum += ( fDid * fDid );
			}

			fVar = fSum / nMask_Size;
			fRatio = fNoise_Var / fVar;

			if ( fRatio > 1.0 )
			{
				//fVar = fAve;
			}
			else
			{
				//fVar = ( 1.0 - fRatio ) * Mask[ nMask_Size / 2 ] + fRatio * fAve;
			}
			//fVar = m_TmpImageData.ptrSrcImage2D[ i ][ j ];
			ptrImageDataSrc->ptrSrcImage2D[ i ][ j ] = (BYTE)LIMIT_VAL( (int)fVar );
		}
	}

	
	return ERR_SUCCESS;
}

static void Delpixel_Thin( pIMAGE_DATA ptrOrgImg, pIMAGE_DATA ptrThinImg)
{
	int i, j;
	int nH, nW;

	nH = ptrOrgImg->nH;
	nW = ptrOrgImg->nW;

	for(i=0; i < nH; i++)
	{
		for(j=0; j<nW ; j++)
		{
			if( ptrThinImg->ptrSrcImage2D[i][j] )
			{
				ptrOrgImg->ptrSrcImage2D[i][j] = 0;
				ptrThinImg->ptrSrcImage2D[i][j] = 0;
			}
		}
	}
}


static int NAYS_Thin( pIMAGE_DATA ptrOrgImg, int i, int j)
{
	int k,l,N=0;


	for(k=i-1;k<=i+1;k++)
	{
		for(l=j-1;l<=j+1;l++)
		{
			if(k!=i || l!=j)
			{
				if(ptrOrgImg->ptrSrcImage2D[k][l] >=1) 
				{
					N++;
				}
			}
		}
	}
	return N;
} 

static int Connect_Thin( pIMAGE_DATA ptrOrgImg, int i, int j)
{
	int n=0;

	if( ptrOrgImg->ptrSrcImage2D[i][j+1] >= 1   && ptrOrgImg->ptrSrcImage2D[i-1][j+1] == 0 ) n++;
	if( ptrOrgImg->ptrSrcImage2D[i-1][j+1] >= 1 && ptrOrgImg->ptrSrcImage2D[i-1][j] == 0 )  n++;
	if( ptrOrgImg->ptrSrcImage2D[i-1][j] >= 1   && ptrOrgImg->ptrSrcImage2D[i-1][j-1] == 0 ) n++;
	if( ptrOrgImg->ptrSrcImage2D[i-1][j-1] >= 1 && ptrOrgImg->ptrSrcImage2D[i][j-1] == 0 ) n++;

	if( ptrOrgImg->ptrSrcImage2D[i][j-1] >= 1   && ptrOrgImg->ptrSrcImage2D[i+1][j-1] == 0 ) n++;
	if( ptrOrgImg->ptrSrcImage2D[i+1][j-1] >= 1 && ptrOrgImg->ptrSrcImage2D[i+1][j] == 0 ) n++;
	if( ptrOrgImg->ptrSrcImage2D[i+1][j] >= 1   && ptrOrgImg->ptrSrcImage2D[i+1][j+1] == 0 ) n++;
	if( ptrOrgImg->ptrSrcImage2D[i+1][j+1] >= 1 && ptrOrgImg->ptrSrcImage2D[i][j+1] == 0 ) n++;

	return n;
}

/*======================================================================
(Zhang suen Thinning) 
Thinning 처리 
========================================================================*/
static int ZS_Thinning( pDIP_DATA ptrDIPData )
{
	pTHINNING_PARAM ptrParam= NULL;
	IMAGE_DATA_t *ptrImageDataSrc= NULL;
	int i, j, nErrCode = ERR_SUCCESS;
	int N;

	BYTE objColor = 255;
	BYTE bConntinue = 1;

	ptrParam = (pTHINNING_PARAM)ptrDIPData->ptrInputParam;
	ptrImageDataSrc = ptrDIPData->ptrImageDataSrc1;

	nErrCode = IsValidData( ptrImageDataSrc , 8 );

	if ( nErrCode != ERR_SUCCESS )
		return nErrCode;

	if ( ptrParam != NULL )
	{
		objColor = ptrParam->ObjColor;
	}
	else
	{
		objColor = 255;
	}

	nErrCode = HLP_CopyImageDataNotAlloc( &m_TmpImageData, ptrImageDataSrc );
	if ( nErrCode != ERR_SUCCESS )
		return nErrCode;

	for( i = 0 ; i < ptrImageDataSrc->nH ; i++ )
	{
		ptrImageDataSrc->ptrSrcImage2D[ i ][ 0 ] = 0;
		ptrImageDataSrc->ptrSrcImage2D[ i ][ ptrImageDataSrc->nW -1] = 0;
	}

	for( j = 0 ; j < ptrImageDataSrc->nW ; j++ )
	{
		ptrImageDataSrc->ptrSrcImage2D[ 0 ][ j ] = 0;
		ptrImageDataSrc->ptrSrcImage2D[ ptrImageDataSrc->nH -1 ][ j ] = 0;
	}

	for( i = 0 ; i < ptrImageDataSrc->nH ; i++ )
	{
		for( j = 0 ; j < ptrImageDataSrc->nW ; j++ )
		{			
			if(ptrImageDataSrc->ptrSrcImage2D[i][j] == objColor) 
			{
				/* 객체이면 0*/
				ptrImageDataSrc->ptrSrcImage2D[i][j] = 1; 
			}
			else
			{
				/* 배경이면1 */ 
				ptrImageDataSrc->ptrSrcImage2D[i][j] = 0;
			}

			/* Temp 초기화 */
			m_TmpImageData.ptrSrcImage2D[i][j] = 0;       
		}
	}

	while( bConntinue )
	{
		bConntinue = 0;
		
		/* first sub-iteration */
		for( i=1;i< ptrImageDataSrc->nH -1;i++)
		{
			for(j=1;j< ptrImageDataSrc->nW -1;j++)
			{
				if( ptrImageDataSrc->ptrSrcImage2D[i][j] != 1) continue;

				/*영상의 중앙 부분을 중심으로 1인( 객체 ) 영상의 개수 계산.*/
				N = 0;
				N = NAYS_Thin(ptrImageDataSrc, i, j);

				/* 객체의개수가 2~6개이고 연결되었지?*/
				if( (N>=2 && N<=6) && Connect_Thin(ptrImageDataSrc, i,j) ==1 )
				{
					if( ( ptrImageDataSrc->ptrSrcImage2D[i][j+1] * ptrImageDataSrc->ptrSrcImage2D[i-1][j] * ptrImageDataSrc->ptrSrcImage2D[i][j-1]) == 0 && 
						( ptrImageDataSrc->ptrSrcImage2D[i-1][j] * ptrImageDataSrc->ptrSrcImage2D[i+1][j] * ptrImageDataSrc->ptrSrcImage2D[i][j-1]) == 0)
					{
						m_TmpImageData.ptrSrcImage2D[i][j] = 1;
						bConntinue = 1;
					}
				}
			}
		}
		
		/* 배경 삭제 */
		Delpixel_Thin(ptrImageDataSrc, &m_TmpImageData );

		if(bConntinue == 0) break;

		/* Second sub-iteration */
		for(i=1; i< ptrImageDataSrc->nH -1;i++)
		{
			for(j=1;j< ptrImageDataSrc->nW -1;j++)
			{
				if( ptrImageDataSrc->ptrSrcImage2D[i][j] != 1) continue;
				
				N=0;
				N = NAYS_Thin(ptrImageDataSrc, i, j);

				if((N>=2 && N<=6) && Connect_Thin(ptrImageDataSrc,i,j) == 1)
				{
					if( (ptrImageDataSrc->ptrSrcImage2D[i-1][j] * ptrImageDataSrc->ptrSrcImage2D[i][j+1] * ptrImageDataSrc->ptrSrcImage2D[i+1][j]) == 0 &&
						(ptrImageDataSrc->ptrSrcImage2D[i][j+1] * ptrImageDataSrc->ptrSrcImage2D[i+1][j] * ptrImageDataSrc->ptrSrcImage2D[i][j-1]) == 0)
					{
						m_TmpImageData.ptrSrcImage2D[i][j] = 1;
						bConntinue = 1;
					}
				}
			}
		}

		Delpixel_Thin(ptrImageDataSrc, &m_TmpImageData );
	}
	
	// 1을 0로 0를 255로 다시 환원
	for(i=0;i< ptrImageDataSrc->nH ;i++)
	{
		for(j=0;j< ptrImageDataSrc->nW ;j++)
		{			
			if(ptrImageDataSrc->ptrSrcImage2D[i][j] > 0) 
			{
				/* 배경은 어둡게 */
				ptrImageDataSrc->ptrSrcImage2D[i][j] = objColor;
			}
			else 
			{
				/* 객체는 밝게 */
				ptrImageDataSrc->ptrSrcImage2D[i][j] = 0;
			}
		}
	}

	return ERR_SUCCESS;
}

/*======================================================================
HoughLineTrans
========================================================================*/
static int HoughLineTrans( pDIP_DATA ptrDIPData )
{
	int i,j, n,m ;
	pHOUGH_PARAM ptrParam= NULL;
	HOUGH_PARAM_t Hough_Pararm;
	ROTATE_PARAM_t Rotate_Param;

	IMAGE_DATA_t  *ptrImageDataSrc= NULL;	
	int nErrCode =  ERR_SUCCESS;	
	DIP_DATA_t DIP_Data1;
	
	ptrParam = (pHOUGH_PARAM)ptrDIPData->ptrInputParam;
	ptrImageDataSrc = ptrDIPData->ptrImageDataSrc1;	
	
	nErrCode = IsValidData( ptrImageDataSrc , 8 );
	if ( nErrCode != ERR_SUCCESS )
		return nErrCode;		
	
	memset( &Hough_Pararm, 0, sizeof( Hough_Pararm));
	memset( &DIP_Data1, 0, sizeof( DIP_DATA_t));
		
	if ( ptrParam == NULL )
		ptrParam = &Hough_Pararm;	
	

	m_HoughRHOCur = (int)(sqrt((double) ptrImageDataSrc->nW * ptrImageDataSrc->nW + ptrImageDataSrc->nH*ptrImageDataSrc->nH) * 2 );
	for(i = 0 ; i < m_HoughRHOMax ; i++)
	{
		memset( m_ptrHoughAcc[ i ], 0, sizeof( int) * HOUGH_ANGLE );
	}

	for( j = 0 ; j < ptrImageDataSrc->nH ; j++ )
	{
		for( i = 0 ; i < ptrImageDataSrc->nW ; i++ )
		{
			if( ptrImageDataSrc->ptrSrcImage2D[j][i] > 128 )
			{
				for( n = 0 ; n < HOUGH_ANGLE ; n++ )
				{
					m = (int)floor( i * m_ptrtsin[ n ] + j * m_ptrtcos[ n ] + 0.5 );
					m += (m_HoughRHOCur/2);

					m_ptrHoughAcc[m][n]++;
				}
			}
		}
	}
	
	ptrParam->Rho = ptrParam->Angle = 0;

	int AccMax = 0;
	for( m = 0 ; m < m_HoughRHOCur ; m++ )
	{
		for( n = 0 ; n < HOUGH_ANGLE ; n++ )
		{
			if( m_ptrHoughAcc[m][n] > AccMax ) 
			{
				AccMax				= m_ptrHoughAcc[m][n];
				ptrParam->Rho	= m - ( m_HoughRHOCur/2 );
				ptrParam->Angle	= n*180.0/HOUGH_ANGLE;
			}
		}
	}	
	
	if ( ptrParam->bTrans )
	{
		memset(&Rotate_Param, 0, sizeof( ROTATE_PARAM_t ));
		DIP_Data1.ptrImageDataSrc1 =ptrImageDataSrc;
		Rotate_Param.Angle = ptrParam->Angle;
		DIP_Data1.ptrInputParam = &Rotate_Param;

		Rotate( &DIP_Data1);
	}
	

	return ERR_SUCCESS;
}
/*======================================================================
영상 회전 
========================================================================*/
static int Rotate( pDIP_DATA ptrDIPData )
{
	int i,j;
	pROTATE_PARAM ptrParam= NULL;	
	IMAGE_DATA_t  *ptrImageDataSrc= NULL;
	int nErrCode =  ERR_SUCCESS;

	double angle, rad = 0;
	double cos_value = 0;
	double sin_value = 0;
	int nx, ny, minx, miny, maxx, maxy, nw, nh;
	int nH, nW;
	int x1, x2, y1, y2;
	double rx, ry, p, q, temp;

	ptrParam = (pROTATE_PARAM)ptrDIPData->ptrInputParam;
	ptrImageDataSrc = ptrDIPData->ptrImageDataSrc1;	

	nErrCode = IsValidData( ptrImageDataSrc , 8 );
	if ( nErrCode != ERR_SUCCESS )
		return nErrCode;	

	nErrCode = HLP_CopyImageDataNotAlloc( &m_TmpImageData, ptrImageDataSrc );
	if ( nErrCode != ERR_SUCCESS )
		return nErrCode ;

	if ( ptrParam == NULL )
		angle = 0;
	else
		angle = ptrParam->Angle;

	if ( angle < 0 || angle > 360 )
	{
		return ERR_INVALID_DATA;
	}

	rad = (angle*PI)/180.;
	cos_value = cos(rad);
	sin_value = sin(rad);

	minx = maxx = 0;
	miny = maxy = 0;

	nH = ptrImageDataSrc->nH;
	nW = ptrImageDataSrc->nW;

	nx = (int)floor(nW*cos_value + 0.5);
	ny = (int)floor(nW*sin_value + 0.5);
	minx = ( minx < nx ) ? minx:nx;	maxx = ( maxx > nx ) ? maxx:nx;
	miny = ( miny < ny ) ? miny:ny;	maxy = ( maxy > ny ) ? maxy:ny;

	nx = (int)floor(-nH*sin_value + 0.5);
	ny = (int)floor( nH*cos_value + 0.5);
	minx = ( minx < nx ) ? minx:nx;	maxx = ( maxx > nx ) ? maxx:nx;
	miny = ( miny < ny ) ? miny:ny;	maxy = ( maxy > ny ) ? maxy:ny;

	nx = (int)floor(nW*cos_value - nH*sin_value + 0.5);
	ny = (int)floor(nW*sin_value + nH*cos_value + 0.5);
	minx = ( minx < nx ) ? minx:nx;	maxx = ( maxx > nx ) ? maxx:nx;
	miny = ( miny < ny ) ? miny:ny;	maxy = ( maxy > ny ) ? maxy:ny;

	nw = maxx - minx;
	nh = maxy - miny;
	
	
	for( i = 0 ; i < ptrImageDataSrc->nH ; i++ )
	{
		for( j = 0 ; j < ptrImageDataSrc->nW ; j++ )
		{
			ptrImageDataSrc->ptrSrcImage2D[  i ][ j ] = 0;
		}
	}
	
	for( i = miny ; i < maxy ; i++ )
	{
		for( j = minx ; j < maxx ; j++ )
		{
			rx =  j*cos_value + i*sin_value;
			ry = -j*sin_value + i*cos_value;

			// 원본 영상 내에 포함된 좌표가 아니라면 무시.
			if( rx < 0 || rx > nW-1 || ry < 0 || ry > nH-1 )
				continue;

			x1 = (int)rx;
			y1 = (int)ry;

			x2 = x1 + 1; if( x2 == nW ) x2 = nW - 1;
			y2 = y1 + 1; if( y2 == nH ) y2 = nH - 1;

			p = rx - x1;
			q = ry - y1;

			temp = (1.0-p)*(1.0-q)*m_TmpImageData.ptrSrcImage2D[y1][x1] + p*(1.0-q)*m_TmpImageData.ptrSrcImage2D[y1][x2]
				 + (1.0-p)*q*m_TmpImageData.ptrSrcImage2D[y2][x1] + p*q*m_TmpImageData.ptrSrcImage2D[y2][x2];

			if ( ( i-miny )< ptrImageDataSrc->nH && ( j-minx )< ptrImageDataSrc->nW )
			{
				ptrImageDataSrc->ptrSrcImage2D[i-miny][j-minx] = (BYTE)LIMIT_VAL(temp);
			}
		}
	}
	return ERR_SUCCESS;
}

/*======================================================================
DoG( Differance Of Gaussian ) Filter
========================================================================*/
static int DoG( pDIP_DATA ptrDIPData )
{
	int i,j;
	BYTE val;
	pGAUSSIAN_PARAM ptrParam= NULL;	
	GAUSSIAN_PARAM_t GaussianParam;
	DIP_DATA_t DIP_Data1;	

	IMAGE_DATA_t  *ptrImageDataSrc= NULL;
	int nErrCode =  ERR_SUCCESS;

	ptrParam = (pGAUSSIAN_PARAM)ptrDIPData->ptrInputParam;
	ptrImageDataSrc = ptrDIPData->ptrImageDataSrc1;	

	if( ptrParam == NULL )
		return ERR_INVALID_DATA;

	nErrCode = IsValidData( ptrImageDataSrc , 8 );
	if ( nErrCode != ERR_SUCCESS )
		return nErrCode;	

	nErrCode = HLP_CopyImageDataNotAlloc( &m_TmpImageData, ptrImageDataSrc );
	if ( nErrCode != ERR_SUCCESS )
		return nErrCode ;

	nErrCode = HLP_CopyImageDataNotAlloc( &m_TmpImageData2, ptrImageDataSrc );
	if ( nErrCode != ERR_SUCCESS )
		return nErrCode ;

	memset( &GaussianParam, 0, sizeof( GAUSSIAN_PARAM_t));
	memset( &DIP_Data1, 0, sizeof( DIP_DATA_t));

	
	DIP_Data1.ptrImageDataSrc1	= &m_TmpImageData;
	GaussianParam.dSigma		= ptrParam->dDoGSigma1;
	DIP_Data1.ptrInputParam		= &GaussianParam;
	Gaussian( &DIP_Data1);

	DIP_Data1.ptrImageDataSrc1	= &m_TmpImageData2;
	GaussianParam.dSigma		= ptrParam->dDoGSigma2;
	DIP_Data1.ptrInputParam		= &GaussianParam;
	Gaussian( &DIP_Data1);

	for( i = 0 ; i < ptrImageDataSrc->nH ; i++ )
	{
		for( j = 0 ; j < ptrImageDataSrc->nW ; j++ )
		{
			val = (BYTE)LIMIT_VAL( ( m_TmpImageData.ptrSrcImage2D[ i ][ j ] - m_TmpImageData2.ptrSrcImage2D[ i ][ j ]) );
			ptrImageDataSrc->ptrSrcImage2D[ i ][ j ] =val; 
		}
	}

	return ERR_SUCCESS;
}

/*======================================================================
양선형 정규화 처리 
========================================================================*/
static int Bilinear_Normalization( pDIP_DATA ptrDIPData )
{
	int i, j;
	int nErrCode =ERR_SUCCESS;	
	BYTE val;

	double BI, INC_W, INC_H, dx, dy;	
	int ORG_X0, ORG_X1, ORG_X2, ORG_X3, ORG_Y0, ORG_Y1, ORG_Y2, ORG_Y3,  ORG_W, ORG_H;
	double ORG_X01 , ORG_X32, ORG_Y03, ORG_Y12, ORG_X, ORG_Y;
	int nNewH, nNewW;

	IMAGE_DATA_t *ptrImageDataSrc, *ptrImageDataDest;
	pBILINEAR_NOR_PARAM ptrParam= NULL;	
	
	ptrImageDataSrc =  ptrImageDataDest = NULL;

	ptrImageDataSrc = ptrDIPData->ptrImageDataSrc1;
	nErrCode = IsValidData( ptrImageDataSrc , 8 );
	if ( nErrCode != ERR_SUCCESS )
		return nErrCode;

	ptrImageDataDest = ptrDIPData->ptrImageDataDest;
	nErrCode = IsValidData( ptrImageDataDest , 8 );
	if ( nErrCode != ERR_SUCCESS )
		return nErrCode;

	ptrParam = (pBILINEAR_NOR_PARAM)ptrDIPData->ptrInputParam;
	if ( ptrParam == NULL )
		return ERR_INVALID_DATA;

	nNewH = ptrImageDataDest->nH;
	nNewW = ptrImageDataDest->nW;

	ORG_X0 = ptrParam->LT_POINT.nX;
	ORG_X1 = ptrParam->RT_POINT.nX;
	ORG_X2 = ptrParam->RB_POINT.nX;
	ORG_X3 = ptrParam->LB_POINT.nX;

	ORG_Y0 = ptrParam->LT_POINT.nY;
	ORG_Y1 = ptrParam->RT_POINT.nY;
	ORG_Y2 = ptrParam->RB_POINT.nY;
	ORG_Y3 = ptrParam->LB_POINT.nY;

	for( i = 0 ; i < nNewH; i++ )
	{
		dy = (double)i / nNewH;

		for( j = 0 ; j <nNewW ; j++ )
		{
			dx = (double)j/ nNewW;

			ORG_X01 =  ORG_X0 + ( ORG_X1 - ORG_X0 ) * dx;
			ORG_X32 =  ORG_X3 + ( ORG_X2 - ORG_X3 ) * dx;
			
			ORG_Y03 =  ORG_Y0 + ( ORG_Y3 - ORG_Y0 ) * dy;
			ORG_Y12 =  ORG_Y1 + ( ORG_Y2 - ORG_Y1 ) * dy;

			ORG_X =   ORG_X01 + ( ORG_X32 -ORG_X01 ) * dy;
			ORG_Y =   ORG_Y03 + ( ORG_Y12 -ORG_Y03 ) * dx;

			ORG_W = (int)ceil( ORG_X );
			ORG_H = (int)ceil( ORG_Y );
						
			INC_W = ORG_X - ORG_W;
			INC_H = ORG_Y - ORG_H;

			if ( ORG_W >= ptrImageDataSrc->nW -1 )
				ORG_W = ptrImageDataSrc->nW -1;
			 
			if ( ORG_H >= ptrImageDataSrc->nH -1 )
				ORG_H = ptrImageDataSrc->nH -1;
			
			BI = (   ( 1.0 - INC_W ) * ( 1.0 - INC_H ) * ptrImageDataSrc->ptrSrcImage2D[ ORG_H ][ ORG_W ] )
				+  ( ( 1.0 - INC_W ) * INC_H           * ptrImageDataSrc->ptrSrcImage2D[ ORG_H + 1][ ORG_W  ] ) 
				+  ( INC_W           * ( 1.0 - INC_H ) * ptrImageDataSrc->ptrSrcImage2D[ ORG_H ][ ORG_W + 1 ] ) 
				+  ( INC_W           * INC_H           * ptrImageDataSrc->ptrSrcImage2D[ ORG_H + 1][ ORG_W + 1 ] );

			val = (BYTE)LIMIT_VAL( BI );
			ptrImageDataDest->ptrSrcImage2D[ i ][ j ] = val;
		}
	}

	return ERR_SUCCESS;
}

static int Unsharp( pDIP_DATA ptrDIPData )
{
	int i,j;	
	IMAGE_DATA_t  *ptrImageDataSrc= NULL;
	pUNSHARP_PARAM ptrParam= NULL;	

	int nErrCode =  ERR_SUCCESS;
	int MaskType;
	BYTE val;

	ptrImageDataSrc = ptrDIPData->ptrImageDataSrc1;	
	
	nErrCode = IsValidData( ptrImageDataSrc , 8 );
	if ( nErrCode != ERR_SUCCESS )
		return nErrCode;	

	nErrCode = HLP_CopyImageDataNotAlloc( &m_TmpImageData, ptrImageDataSrc );
	if ( nErrCode != ERR_SUCCESS )
		return nErrCode ;

	ptrParam = (pUNSHARP_PARAM)ptrDIPData->ptrInputParam;
	
	if( ptrParam == NULL )
		MaskType = UNSHARP_5MASK;
	else
		MaskType = ptrParam->Mask;

	for( i = 1 ; i < ptrImageDataSrc->nH-1; i++ )
	{
		for( j = 1 ; j < ptrImageDataSrc->nW -1; j++ )
		{
			val =0;
			if ( MaskType == UNSHARP_5MASK )
			{
				val = LIMIT_VAL( 5 * m_TmpImageData.ptrSrcImage2D[ i ][ j ] - m_TmpImageData.ptrSrcImage2D[ i -1 ][ j ]- m_TmpImageData.ptrSrcImage2D[ i  ][ j -1] - m_TmpImageData.ptrSrcImage2D[ i  ][ j +1] - m_TmpImageData.ptrSrcImage2D[ i+1 ][ j] );
			}
			else
			{
				val = LIMIT_VAL ( 9 * m_TmpImageData.ptrSrcImage2D[ i ][ j ] - m_TmpImageData.ptrSrcImage2D[ i -1 ][ j -1 ] - m_TmpImageData.ptrSrcImage2D[ i -1 ][ j ]- m_TmpImageData.ptrSrcImage2D[ i -1 ][ j+1 ]
				      - m_TmpImageData.ptrSrcImage2D[ i ][ j -1 ] - m_TmpImageData.ptrSrcImage2D[ i  ][ j+1 ]
					  - m_TmpImageData.ptrSrcImage2D[ i+1 ][ j -1 ] - m_TmpImageData.ptrSrcImage2D[ i+1 ][ j ]- m_TmpImageData.ptrSrcImage2D[ i+1 ][ j+1 ] );
			}

			ptrImageDataSrc->ptrSrcImage2D[ i ][ j ] =  val;
		}
	}

	return ERR_SUCCESS;
}


static int Opening( pDIP_DATA ptrDIPData )
{
	Erosion( ptrDIPData);
	Dilation( ptrDIPData );

	return ERR_SUCCESS;
}

static int Closing( pDIP_DATA ptrDIPData )
{
	Dilation( ptrDIPData );
	Erosion( ptrDIPData);

	return ERR_SUCCESS;
}

/*======================================================================
Hough 처리용 Lookup table 생성 
========================================================================*/
static int CreateHoughLookupTable( void )
{
	int i;

	if ( m_ptrtsin == NULL )
	{
		m_ptrtsin = new double[ HOUGH_ANGLE ];
	}
	if ( m_ptrtcos == NULL )
	{
		m_ptrtcos = new double[ HOUGH_ANGLE ];
	}

	if( m_ptrtcos == NULL || m_ptrtsin == NULL )
	{
		Release();
		return ERR_INVALID_DATA;
	}

	for( i = 0 ; i < HOUGH_ANGLE ; i++ )
	{
		m_ptrtsin[i] = (double)sin(i*PI/HOUGH_ANGLE);
		m_ptrtcos[i] = (double)cos(i*PI/HOUGH_ANGLE);
	}

	return ERR_SUCCESS;
}
/*======================================================================
DIP 시작 시 필요한 공용 메모리를 미리 할당 해 놓으므로써
운영중 DIP 처리 시간을 단축 하려는 목적
========================================================================*/
static int AllocGlobalBuffer(  pIMAGE_DATA ptrImageDataSrc )
{
	m_nImageHeight = ptrImageDataSrc->nH;
	m_nImageWidth = ptrImageDataSrc->nW;
	
	if ( m_nImageHeight > m_nMaxHeight )
		m_nMaxHeight = m_nImageHeight;

	if ( m_nImageWidth > m_nMaxWidth )
		m_nMaxWidth = m_nImageWidth;

	memset( &m_TmpImageData, 0, sizeof( IMAGE_DATA_t ));
	HLP_CopyImageDataWithEmpty( &m_TmpImageData, 8,m_nMaxHeight, m_nMaxWidth );

	memset( &m_TmpImageData2, 0, sizeof( IMAGE_DATA_t ));
	HLP_CopyImageDataWithEmpty( &m_TmpImageData2, 8,m_nMaxHeight, m_nMaxWidth );


	/* Temp buf 할당 */
	Gaussian_dTempMatrix = HLP_dAllocMatrix(m_nMaxHeight, m_nMaxWidth );
	if ( Gaussian_dTempMatrix == NULL )
		return ERR_ALLOC_BUF;

	Gaussian_pMake = new double[ MAX_GAUSSIAN_DIM ];
	if ( Gaussian_pMake == NULL )
		return ERR_ALLOC_BUF;

	m_Adap_ptrData = new int[ m_nMaxHeight * m_nMaxWidth ];
	m_Adap_ptrIntergral = new int[ m_nMaxHeight * m_nMaxWidth ];
	m_Adap_ptrThreshold = new int[ m_nMaxHeight * m_nMaxWidth ];

	/* Diffusion buffer 할당 */
	m_diff_ptrCpyBuf = HLP_fAllocMatrix( m_nMaxHeight, m_nMaxWidth );
	m_diff_ptrTmpBuf = HLP_fAllocMatrix( m_nMaxHeight, m_nMaxWidth );

	/* Hough Accumulate Array 할당 */
	m_HoughRHOMax = (int)(sqrt((double) m_nMaxWidth * m_nMaxWidth + m_nMaxHeight*m_nMaxHeight) * 2 );
	m_ptrHoughAcc = HLP_IntAllocMatrix(m_HoughRHOMax, HOUGH_ANGLE );

	return ERR_SUCCESS;
}

/*======================================================================
DIP 공용 메모리 해제 
========================================================================*/
static int ReleaseGlobalBuffer( void )
{
	HLP_FreeMatrix( m_TmpImageData.ptrSrcImage2D, m_nMaxHeight,m_nMaxWidth );
	HLP_FreeMatrix( m_TmpImageData2.ptrSrcImage2D, m_nMaxHeight,m_nMaxWidth );

	HLP_dFreeMatrix( Gaussian_dTempMatrix, m_nMaxHeight, m_nMaxWidth);

	HLP_fFreeMatrix(m_diff_ptrCpyBuf , m_nMaxHeight, m_nMaxWidth);
	HLP_fFreeMatrix(m_diff_ptrTmpBuf , m_nMaxHeight, m_nMaxWidth);

	if ( Gaussian_pMake != NULL ) delete [] Gaussian_pMake;
	if ( m_Adap_ptrIntergral != NULL ) delete [] m_Adap_ptrIntergral;
	if ( m_Adap_ptrThreshold != NULL ) delete [] m_Adap_ptrThreshold;
	if ( m_Adap_ptrData != NULL ) delete [] m_Adap_ptrData;

	m_TmpImageData.ptrSrcImage2D = NULL;
	m_TmpImageData2.ptrSrcImage2D = NULL;

	Gaussian_dTempMatrix = NULL;
	Gaussian_pMake = NULL;

	m_Adap_ptrIntergral = NULL;
	m_Adap_ptrThreshold = NULL;
	m_Adap_ptrData = NULL;
	
	m_diff_ptrCpyBuf = NULL;
	m_diff_ptrTmpBuf = NULL;

	/* Hough buffer */
	HLP_IntFreeMatrix(m_ptrHoughAcc , m_HoughRHOMax, HOUGH_ANGLE);
	m_ptrHoughAcc = NULL;

	return ERR_SUCCESS;
}

static int ReleaseHoughLookupTable( void )
{
	if ( m_ptrtsin != NULL )
	{
		delete [] m_ptrtsin;
		m_ptrtsin = NULL;
	}

	if ( m_ptrtcos != NULL )
	{
		delete [] m_ptrtcos;
		m_ptrtcos = NULL;
	}

	return ERR_SUCCESS;
}
/*======================================================================
DIP 처리 전 활당된 공용 메모리와 현재 이미지 사이즈를 비교하여
다를 경우 현재 이미지를 기준으로 새로운 공용 메모리를 활당. 
========================================================================*/
static int PreWorkDIP( pDIP_DATA ptrDIPData )
{
	static int nFirst = 0;
	pIMAGE_DATA ptrImageDataSrc = NULL;

	ptrImageDataSrc = ptrDIPData->ptrImageDataSrc1;
	
	if ( ptrImageDataSrc == NULL )
		return ERR_INVALID_DATA;


	if ( nFirst == 0 )
	{
		nFirst = 1;
		if ( AllocGlobalBuffer(  ptrImageDataSrc ) != ERR_SUCCESS )
		{
			nFirst = 0;
		}
	}
	else
	{
		if ( ptrImageDataSrc->nH > m_nMaxHeight || ptrImageDataSrc->nW > m_nMaxWidth )
		{
			ReleaseGlobalBuffer();
			if ( AllocGlobalBuffer(  ptrImageDataSrc ) != ERR_SUCCESS )
			{
				nFirst = 0;
			}
		}
	}

	return ERR_SUCCESS;
}

/*======================================================================
설정 정보 읽어 오기
========================================================================*/
static int Load_DIP_Conf_Data( pLIB_INIT_DATA ptrInitData )
{
	if( ptrInitData != NULL )
	{
		SetWinCurPath( ptrInitData->szwinpath);
		ptrInitData->Version[ 0 ] = DIP_VER_1ST;
		ptrInitData->Version[ 1 ] = DIP_VER_2ND;
	}

	if ( loadDIPConf() == ERR_SUCCESS )
	{
		m_ptrdip_conf= GetDIPConf();
	}

	return ERR_SUCCESS;
}

/*======================================================================
Export Function 
========================================================================*/
int Initialize( pLIB_INIT_DATA ptrInitData  )
{
	int nErrCode = ERR_SUCCESS;
	nFuncCnt = ( sizeof( ImageProcFuncs) / sizeof( ImageProcFuncs_t ) );
	
	nErrCode = CreateHoughLookupTable();
	if ( nErrCode != ERR_SUCCESS)
		return nErrCode;

	nErrCode = Load_DIP_Conf_Data( ptrInitData );
	if ( nErrCode != ERR_SUCCESS)
		return nErrCode;


	m_initial = 1;
	return nErrCode;
}

int Release( void  )
{
	ReleaseGlobalBuffer();
	ReleaseLabeling();
	ReleaseHoughLookupTable();
	return ERR_SUCCESS;
}

/*======================================================================
DIP DLL 내부에서 메모리 생성  
========================================================================*/
int CopyImageAllocData( pIMAGE_DATA ptrImageDataDest, pIMAGE_DATA ptrImageDataSrc )
{
	HLP_CopyImageData( ptrImageDataDest, ptrImageDataSrc );
	return ERR_SUCCESS;
}

/*======================================================================
DIP DLL 내부에서 메모리 생성  
========================================================================*/
int FreeImageAllocData( pIMAGE_DATA ptrImageData )
{
	HLP_FreeImageAllocData( ptrImageData );
	return ERR_SUCCESS;
}

int ImageProcess( pDIP_DATA ptrDIPData )
{
	int i =0;
	ImageProcFuncs_t * ptrFuncT= NULL;
	UINT16 l_uMsg;
	ImageProcFunc l_ptrFunc = NULL ;	
	int nErrCode = ERR_SUCCESS;

	nErrCode = PreWorkDIP( ptrDIPData );
	if ( nErrCode != ERR_SUCCESS || m_initial != 1 )
		return nErrCode;

	for( i = 0 ; i < nFuncCnt ; i++ )
	{
		ptrFuncT = ( ImageProcFuncs_t *  )&ImageProcFuncs[ i ];
		if ( ptrFuncT != NULL )
		{
			l_uMsg = ptrFuncT->uProcMsg;
			l_ptrFunc = ptrFuncT->ptrFunc;

			if ( l_uMsg != 0x0000 && l_ptrFunc != 0x00  && ptrDIPData != NULL )
			{
				if ( l_uMsg == ptrDIPData->uDIPMsg ) 
				{
					return l_ptrFunc( ptrDIPData );
				}
			}			
		}
		else
			break;
	}

	return ERR_NO_PARAM;
}

int IOCTL( pIOCTL_DATA ptrIOCTLData )
{
	int nErrCode = ERR_SUCCESS;

	if( ptrIOCTLData->uIOCTLMsg == TK_IOCTL_RECONF_MSG )
	{
		nErrCode= Load_DIP_Conf_Data( NULL );
		return nErrCode;
	}

	return ERR_SUCCESS;
}