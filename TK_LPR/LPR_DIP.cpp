#include "stdafx.h"
#include <malloc.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "../include/TK_TYPE.h"
#include "../include/TK_LIB_API.h"
#include "../include/TK_LIB_Helper.h"
#include "LPR_DIP.h"

/*=========================================================================
Histogram ó�� 
==========================================================================*/
int Histogram( pDIP_DATA ptrDIPData, pIMAGE_DATA ptrImageData, HISTOGRAM_PARAM_t * ptrParam )
{
	
	memset( ptrParam, 0, sizeof( HISTOGRAM_PARAM_t ));
	ptrDIPData->ptrImageDataSrc1 = ptrImageData;
	ptrDIPData->uDIPMsg = TK_DIP_HISTOGRAM_MSG;
	ptrDIPData->ptrInputParam = ptrParam;

	return DIP_ImageProcess( ptrDIPData);	
}

/*=========================================================================
Histogram ��Ȱȭó�� 
==========================================================================*/
 int HistogramEqual( pDIP_DATA ptrDIPData, pIMAGE_DATA ptrImageData )
{	
	HISTOGRAM_PARAM_t Histo_Param;	
	memset( &Histo_Param, 0, sizeof( HISTOGRAM_PARAM_t ));

	ptrDIPData->ptrImageDataSrc1 = ptrImageData;
	ptrDIPData->uDIPMsg = TK_DIP_HISTOGRAM_EQUAL_MSG;
	ptrDIPData->ptrInputParam = &Histo_Param;

	return DIP_ImageProcess( ptrDIPData);	
}

 /*=========================================================================
 ��� ó�� 
==========================================================================*/
 int Brightness( pDIP_DATA ptrDIPData, pIMAGE_DATA ptrImageData , int nBright)
{	
	BRIGHT_PARAM_t BrightParam;	
	memset( &BrightParam, 0, sizeof( BRIGHT_PARAM_t ));

	BrightParam.nBright = nBright;

	ptrDIPData->ptrImageDataSrc1 = ptrImageData;
	ptrDIPData->uDIPMsg = TK_DIP_BRIGHTNESS_MSG;
	ptrDIPData->ptrInputParam = &BrightParam;

	return DIP_ImageProcess( ptrDIPData);	
}

 /*=========================================================================
 BEAM FILTER ó�� 
==========================================================================*/
 int Beam( pDIP_DATA ptrDIPData, pIMAGE_DATA ptrImageData , int nTh)
{	
	BINARIZATION_PARAM_t BI_Param;
	memset( &BI_Param, 0, sizeof( BINARIZATION_PARAM_t ));

	BI_Param.nThreshold = nTh;
	ptrDIPData->ptrImageDataSrc1 = ptrImageData;
	ptrDIPData->uDIPMsg = TK_DIP_BEAM_MSG;
	ptrDIPData->ptrInputParam = &BI_Param;

	return DIP_ImageProcess( ptrDIPData);	
}

/*=========================================================================
 ����ȭ ó�� 
==========================================================================*/
int Binarization( pDIP_DATA ptrDIPData, pIMAGE_DATA ptrImageData, int nTh )
{
	BINARIZATION_PARAM_t Binary_Param;

	memset(&Binary_Param, 0, sizeof( BINARIZATION_PARAM_t ));
	Binary_Param.nThreshold = nTh;

	ptrDIPData->ptrImageDataSrc1 = ptrImageData;
	ptrDIPData->uDIPMsg = TK_DIP_BINARIZATION_MSG;
	ptrDIPData->ptrInputParam = &Binary_Param;

	return DIP_ImageProcess( ptrDIPData );
}

/*=========================================================================
 �ݺ� ����ȭ ó�� 
==========================================================================*/
 int Binarization_Iterate( pDIP_DATA ptrDIPData, pIMAGE_DATA ptrImageData )
{
	BINARIZATION_PARAM_t Binary_Param;

	memset(&Binary_Param, 0, sizeof( BINARIZATION_PARAM_t ));
	
	ptrDIPData->ptrImageDataSrc1 = ptrImageData;
	ptrDIPData->uDIPMsg = TK_DIP_BINARIZATION_ITER_MSG;
	ptrDIPData->ptrInputParam = &Binary_Param;

	return DIP_ImageProcess( ptrDIPData );
}

/*=========================================================================
 Adpative ����ȭ ó�� 
==========================================================================*/
 int Binarization_Adaptive( pDIP_DATA ptrDIPData, pIMAGE_DATA ptrImageData, int nAdaptCnt, float fAdaptRatio )
 {
	 BINARIZATION_PARAM_t Binary_Param;
	 
	 memset(&Binary_Param, 0, sizeof( BINARIZATION_PARAM_t ));	
	 Binary_Param.nAdaptCnt = nAdaptCnt;
	 Binary_Param.fAdaptRatio = fAdaptRatio;

	 ptrDIPData->ptrImageDataSrc1 = ptrImageData;
	 ptrDIPData->uDIPMsg = TK_DIP_BINARIZATION_ADAPTIVE_MSG;
	 ptrDIPData->ptrInputParam = &Binary_Param;

	 return DIP_ImageProcess( ptrDIPData );

 }

 /*=========================================================================
Integral Adaptive ����ȭ ó�� 
==========================================================================*/
 int Binarization_ITG_Adaptive( pDIP_DATA ptrDIPData, pIMAGE_DATA ptrImageData, int nMaskSize )
 {	 
	 BINARIZATION_PARAM_t Binary_Param;

	 memset(&Binary_Param, 0, sizeof( BINARIZATION_PARAM_t ));
	 Binary_Param.nITGSize = nMaskSize;	

	 ptrDIPData->ptrImageDataSrc1 = ptrImageData;
	 ptrDIPData->uDIPMsg = TK_DIP_BINARIZATION_ITG_ADAPTIVE_MSG;
	 ptrDIPData->ptrInputParam = &Binary_Param;

	 return DIP_ImageProcess( ptrDIPData );
 }

/*=========================================================================
 OTSU ����ȭ ó�� 
==========================================================================*/
 int Binarization_Otsu( pDIP_DATA ptrDIPData, pIMAGE_DATA ptrImageData )
 {	 
	 BINARIZATION_PARAM_t Binary_Param;

	 memset(&Binary_Param, 0, sizeof( BINARIZATION_PARAM_t ));

	 ptrDIPData->ptrImageDataSrc1 = ptrImageData;
	 ptrDIPData->uDIPMsg = TK_DIP_BINARIZATION_OTSU_MSG;
	 ptrDIPData->ptrInputParam = &Binary_Param;

	 return DIP_ImageProcess( ptrDIPData );
 }

 /*=========================================================================
 OTSU ����ȭ �Ӱ谪 ��û
==========================================================================*/
int GetBinarizationThreshod( pDIP_DATA ptrDIPData, pIMAGE_DATA ptrImageData )
{
	int nErrCode =0;
	BINARIZATION_PARAM_t Binary_Param;

	memset(&Binary_Param, 0, sizeof( BINARIZATION_PARAM_t ));
	Binary_Param.bOnlyThreshold = 1;
	
	ptrDIPData->ptrImageDataSrc1 = ptrImageData;
	ptrDIPData->uDIPMsg = TK_DIP_BINARIZATION_OTSU_MSG;
	ptrDIPData->ptrInputParam = &Binary_Param;

	nErrCode=DIP_ImageProcess( ptrDIPData );
	if ( nErrCode != ERR_SUCCESS )
		return nErrCode;

	return Binary_Param.nThreshold;
}

/*=========================================================================
  ROBERT ��輱 ����
==========================================================================*/
 int Roberts( pDIP_DATA ptrDIPData, pIMAGE_DATA ptrImageData, BYTE LineEdgeType )
{
	LINE_EDGE_PARAM_t LineEdgeParam;

	memset( &LineEdgeParam, 0, sizeof( LineEdgeParam ));
	LineEdgeParam.LineEdgeType = LineEdgeType;

	ptrDIPData->ptrImageDataSrc1 = ptrImageData;
	ptrDIPData->uDIPMsg = TK_DIP_ROBERTS_MSG;
	ptrDIPData->ptrInputParam = &LineEdgeParam;

	return DIP_ImageProcess( ptrDIPData );
}

 /*=========================================================================
  PREWITT ��輱 ����
==========================================================================*/
 int Prewitt( pDIP_DATA ptrDIPData, pIMAGE_DATA ptrImageData , BYTE LineEdgeType)
{
	LINE_EDGE_PARAM_t LineEdgeParam;

	memset( &LineEdgeParam, 0, sizeof( LineEdgeParam ));
	LineEdgeParam.LineEdgeType = LineEdgeType;

	ptrDIPData->ptrImageDataSrc1 = ptrImageData;
	ptrDIPData->uDIPMsg = TK_DIP_PREWITT_MSG;
	ptrDIPData->ptrInputParam = &LineEdgeParam;

	return DIP_ImageProcess( ptrDIPData );
}

 /*=========================================================================
  SOBEL ��輱 ����
==========================================================================*/
 int Sobel( pDIP_DATA ptrDIPData, pIMAGE_DATA ptrImageData , BYTE LineEdgeType)
{
	LINE_EDGE_PARAM_t LineEdgeParam;

	memset( &LineEdgeParam, 0, sizeof( LineEdgeParam ));
	LineEdgeParam.LineEdgeType = LineEdgeType;

	ptrDIPData->ptrImageDataSrc1 = ptrImageData;
	ptrDIPData->uDIPMsg = TK_DIP_SOBEL_MSG;
	ptrDIPData->ptrInputParam = &LineEdgeParam;

	return DIP_ImageProcess( ptrDIPData );
}

/*=========================================================================
  GAUSSIAN ��輱 ����
==========================================================================*/
 int Gaussian( pDIP_DATA ptrDIPData, pIMAGE_DATA ptrImageData , double dSigma)
{
	GAUSSIAN_PARAM_t GaussianParam;

	memset( &GaussianParam, 0, sizeof( GAUSSIAN_PARAM_t ));
	GaussianParam.dSigma = dSigma;

	ptrDIPData->ptrImageDataSrc1 = ptrImageData;
	ptrDIPData->uDIPMsg = TK_DIP_GAUSSIAN_MSG;
	ptrDIPData->ptrInputParam = &GaussianParam;

	return DIP_ImageProcess( ptrDIPData );
}

/*=========================================================================  
  ������� ����

  LPR_COMMON�� Labeling �˰��� ���
==========================================================================*/
 int Labeling( pDIP_DATA ptrDIPData, pIMAGE_DATA ptrImageData )
{
	int nLabelCnt =0;
	
	ptrDIPData->ptrImageDataSrc1 = ptrImageData;
	ptrDIPData->uDIPMsg = TK_DIP_LABEL_MSG;
	ptrDIPData->ptrInputParam = NULL;
	nLabelCnt= DIP_ImageProcess( ptrDIPData );

	if ( nLabelCnt >= 0 )
		return nLabelCnt;

	return ERR_INVALID_DATA; 
}

/*=========================================================================  
  Canny ��輱 ����
==========================================================================*/
 int Canny( pDIP_DATA ptrDIPData, pIMAGE_DATA ptrImageData )
{
	ptrDIPData->ptrImageDataSrc1 = ptrImageData;
	ptrDIPData->uDIPMsg = TK_DIP_CANNY_MSG;
	ptrDIPData->ptrInputParam = NULL;

	return DIP_ImageProcess( ptrDIPData );
}

/*=========================================================================  
   ���� ħ��
==========================================================================*/
 int Erosion( pDIP_DATA ptrDIPData, pIMAGE_DATA ptrImageData , BYTE SegmentType )
{
	SEGMENT_PARAM_t SegmentParam;

	SegmentParam.SegmentType = SegmentType;
	ptrDIPData->ptrImageDataSrc1 = ptrImageData;
	ptrDIPData->uDIPMsg = TK_DIP_EROSION_MSG;
	ptrDIPData->ptrInputParam = &SegmentParam;

	return DIP_ImageProcess( ptrDIPData );
}
/*=========================================================================  
   ���� ��â
==========================================================================*/
 int Dilation( pDIP_DATA ptrDIPData, pIMAGE_DATA ptrImageData,  BYTE SegmentType )
 {
	 SEGMENT_PARAM_t SegmentParam;
	 SegmentParam.SegmentType = SegmentType;

	 ptrDIPData->ptrImageDataSrc1 = ptrImageData;
	 ptrDIPData->uDIPMsg = TK_DIP_DILATION_MSG;
	 ptrDIPData->ptrInputParam = &SegmentParam;

	 return DIP_ImageProcess( ptrDIPData );
 }

 /*=========================================================================  
   ���� ����
==========================================================================*/
 int SubImage( pDIP_DATA ptrDIPData, pIMAGE_DATA ptrImageData1, pIMAGE_DATA ptrImageData2, pIMAGE_DATA ptrImageDest )
{
	ptrDIPData->ptrImageDataSrc1 = ptrImageData1;
	ptrDIPData->ptrImageDataSrc2 = ptrImageData2;
	ptrDIPData->ptrImageDataDest = ptrImageDest;

	ptrDIPData->uDIPMsg = TK_DIP_SUBIMAGE_MSG;
	ptrDIPData->ptrInputParam = NULL;

	return DIP_ImageProcess( ptrDIPData );
}

/*=========================================================================  
   �߰��� ����
==========================================================================*/
 int Median( pDIP_DATA ptrDIPData, pIMAGE_DATA ptrImageData )
{
	ptrDIPData->ptrImageDataSrc1 = ptrImageData;
	ptrDIPData->uDIPMsg = TK_DIP_MEDIAN_MSG;
	ptrDIPData->ptrInputParam = NULL;

	return DIP_ImageProcess( ptrDIPData );
}
/*=========================================================================  
   ��հ� ����
==========================================================================*/
 int Mean( pDIP_DATA ptrDIPData, pIMAGE_DATA ptrImageData )
{
	ptrDIPData->ptrImageDataSrc1 = ptrImageData;
	ptrDIPData->uDIPMsg = TK_DIP_MEAN_MSG;
	ptrDIPData->ptrInputParam = NULL;

	return DIP_ImageProcess( ptrDIPData );
}
/*=========================================================================  
   ���� ��հ� ����
==========================================================================*/
 int WeightedMean( pDIP_DATA ptrDIPData, pIMAGE_DATA ptrImageData )
{
	ptrDIPData->ptrImageDataSrc1 = ptrImageData;
	ptrDIPData->uDIPMsg = TK_DIP_WEIGHTEDMEAN_MSG;
	ptrDIPData->ptrInputParam = NULL;

	return DIP_ImageProcess( ptrDIPData );
}
/*=========================================================================  
   CONTRAST ����
==========================================================================*/
 int Contrast( pDIP_DATA ptrDIPData, pIMAGE_DATA ptrImageData, int nRatio )
{
	CONTRAST_PARAM_t ContrastParam;

	memset(&ContrastParam, 0, sizeof( CONTRAST_PARAM_t ));

	ContrastParam.nRatio = nRatio;
	ptrDIPData->ptrImageDataSrc1 = ptrImageData;
	ptrDIPData->uDIPMsg = TK_DIP_CONTRAST_MSG;
	ptrDIPData->ptrInputParam = &ContrastParam;

	return DIP_ImageProcess( ptrDIPData );
}

/*======================================================================
������ �ܰ��κ��� ��Ӱ� ó���ϴ� �������
���� ��ȣ���� ��輱 �κ��� ������ �����ϱ� ���
========================================================================*/
int OuterLine( pDIP_DATA ptrDIPData, pIMAGE_DATA ptrImageData1, pIMAGE_DATA ptrImageData2, pIMAGE_DATA ptrImageDest, BYTE bOuterType )
{	
	OUTERLINE_PARAM_t Outer_Param;

	memset( &Outer_Param, 0, sizeof( OUTERLINE_PARAM_t));
	ptrDIPData->ptrImageDataSrc1 = ptrImageData1;
	ptrDIPData->ptrImageDataSrc2 = ptrImageData2;
	ptrDIPData->ptrImageDataDest = ptrImageDest;

	Outer_Param.nOuter = bOuterType;
	ptrDIPData->ptrInputParam = &Outer_Param;

	ptrDIPData->uDIPMsg = TK_DIP_OUTLINE_MSG;
	ptrDIPData->ptrInputParam = &Outer_Param;

	return DIP_ImageProcess( ptrDIPData );
}

/*======================================================================
���������� ������ �����Ѵ�. 
========================================================================*/
 int Binarization_DeNoise( pDIP_DATA ptrDIPData, pIMAGE_DATA ptrImageData )
 {
	 ptrDIPData->ptrImageDataSrc1 = ptrImageData;
	 ptrDIPData->uDIPMsg = TK_DIP_BINARIZATION_DENOISE_MSG;
	 
	 return DIP_ImageProcess( ptrDIPData );
 }

/*======================================================================
���� ��� ���� ����
========================================================================*/
 int Inverse( pDIP_DATA ptrDIPData, pIMAGE_DATA ptrImageData)
 {
	 ptrDIPData->ptrImageDataSrc1 = ptrImageData;
	 ptrDIPData->uDIPMsg = TK_DIP_INVERSE_MSG;
	 ptrDIPData->ptrInputParam = NULL;

	 return DIP_ImageProcess( ptrDIPData );

 }

/*======================================================================
���� ������ ����
========================================================================*/
 int ReSizeCubic( pDIP_DATA ptrDIPData, pIMAGE_DATA ptrImageData, pIMAGE_DATA ptrImageDest)
 {
	 ptrDIPData->ptrImageDataSrc1 = ptrImageData;	 
	 ptrDIPData->ptrImageDataDest = ptrImageDest;

	 ptrDIPData->uDIPMsg = TK_DIP_RESIZE_MSG;
	 ptrDIPData->ptrInputParam = NULL;

	 return DIP_ImageProcess( ptrDIPData );

 }

 /*======================================================================
 ���漺 Ȯ�� ����( ������ )
========================================================================*/
 int Diffusion( pDIP_DATA ptrDIPData, pIMAGE_DATA ptrImageData, int nIter )
 {
	 DIFFUSION_PARAM_t DiffusionParam;

	 memset( &DiffusionParam, 0, sizeof( DiffusionParam));

	 DiffusionParam.nIter = nIter;
	 ptrDIPData->ptrImageDataSrc1 = ptrImageData;
	 ptrDIPData->uDIPMsg = TK_DIP_DIFFUSION_MSG;
	 ptrDIPData->ptrInputParam = &DiffusionParam;

	 return DIP_ImageProcess( ptrDIPData );

 }

/*======================================================================
(Minimum Mean Squared Error) 
��輱�� �����ϸ鼭 �ڰ輱 �ֺ��� �������� ����
========================================================================*/
 int MMSE( pDIP_DATA ptrDIPData, pIMAGE_DATA ptrImageData)
 {
	 ptrDIPData->ptrImageDataSrc1 = ptrImageData;
	 ptrDIPData->uDIPMsg = TK_DIP_MMSE_MSG;
	 ptrDIPData->ptrInputParam = NULL;

	 return DIP_ImageProcess( ptrDIPData );

 }

 /*====================================================================== 
 ������ ��� ������ ���� �´�. 
 ========================================================================*/
 int LightInfo( pDIP_DATA ptrDIPData, pIMAGE_DATA ptrImageData, pLIGHTINFO_PARAM ptrParam )
 {
	ptrDIPData->ptrImageDataSrc1 = ptrImageData;
	ptrDIPData->uDIPMsg = TK_DIP_LIGHTINFO_MSG;
	ptrDIPData->ptrInputParam = ptrParam;
	
	return DIP_ImageProcess( ptrDIPData );
 }

 /*====================================================================== 
 ���� ������ ��������� �����Ѵ�. 
 ========================================================================*/
 int ZhangSuen_Thinning( pDIP_DATA ptrDIPData, pIMAGE_DATA ptrImageData, int objColor )
 {	 
	THINNING_PARAM_t Param;

	memset( &Param, 0, sizeof( THINNING_PARAM_t));

	Param.ObjColor = (BYTE)objColor;
	ptrDIPData->ptrImageDataSrc1 = ptrImageData;
	ptrDIPData->uDIPMsg = TK_DIP_ZS_THINNING_MSG;
	ptrDIPData->ptrInputParam = &Param;

	return DIP_ImageProcess( ptrDIPData );

 }
 /*====================================================================== 
 ������( 0 )�� �ƴ� ȭ�Ҹ� ��� 255�� �����Ѵ�. 
 ========================================================================*/
 int Contrast_Binary( int nThreshold, pIMAGE_DATA ptrImageData )
 {
	int i, j, nH, nW;
	
	nH = ptrImageData->nH;
	nW = ptrImageData->nW;
	
	if( nThreshold ==0 )
		nThreshold = DEFFAULT_RESIZE_THRESHOLD;
	
	
	for(i=0;i< nH ;i++)
	{
		for(j=0;j< nW ;j++)
		{

			if(ptrImageData->ptrSrcImage2D[i][j] >= nThreshold ) 
			{	
				ptrImageData->ptrSrcImage2D[i][j] = 255;
			}
			else 
			{			
				ptrImageData->ptrSrcImage2D[i][j] = 0;
			}
		}
	}

	return ERR_SUCCESS;
	
 }

 /*====================================================================== 
 ������ ������ ��� ó���Ѵ�. 
 ========================================================================*/
 int Make_BoxLine( pIMAGE_DATA ptrImageData )
 {
	int i, j;

	for( i = 0 ; i < ptrImageData->nH ; i++ )
	{
		ptrImageData->ptrSrcImage2D[ i ][ 0 ] = 0;
		ptrImageData->ptrSrcImage2D[ i ][ ptrImageData->nW -1] = 0;
	}

	for( j = 0 ; j < ptrImageData->nW ; j++ )
	{
		ptrImageData->ptrSrcImage2D[ 0 ][ j ] = 0;
		ptrImageData->ptrSrcImage2D[ ptrImageData->nH -1 ][ j ] = 0;
	}

	return ERR_SUCCESS;
 }


 /*======================================================================
Hough ���� ��ȯ( ���� ���� )
========================================================================*/
int HoughLineTrans( pDIP_DATA ptrDIPData, pIMAGE_DATA ptrImageData, pHOUGH_PARAM ptrParam )
{		
	ptrDIPData->ptrImageDataSrc1 = ptrImageData;

	ptrDIPData->uDIPMsg = TK_DIP_HOUGH_LINE_TRANS_MSG;
	ptrDIPData->ptrInputParam = ptrParam;

	return DIP_ImageProcess( ptrDIPData );
}

/*======================================================================
Bilinear ���� ��ȯ( ���� ũ�� ���� ��ȯ )
========================================================================*/
int Bilinear( pDIP_DATA ptrDIPData, pIMAGE_DATA ptrImageData, pIMAGE_DATA ptrImageDest, pBILINEAR_NOR_PARAM ptrParam )
{

	ptrDIPData->ptrImageDataSrc1 = ptrImageData;
	ptrDIPData->ptrImageDataDest = ptrImageDest;
	ptrDIPData->ptrInputParam = ptrParam;

	ptrDIPData->uDIPMsg = TK_DIP_BILINEAR_NOR_MSG;	

	return DIP_ImageProcess( ptrDIPData );
}

/*======================================================================
Unsharp ���� ��ȯ( ���� ��ī�Ӱ� ��ȯ )
========================================================================*/
int Unsharp( pDIP_DATA ptrDIPData, pIMAGE_DATA ptrImageData, int nMask )
{
	UNSHARP_PARAM_t Unsharp_Mask;

	memset( &Unsharp_Mask, 0, sizeof( UNSHARP_PARAM_t ));
	Unsharp_Mask.Mask = nMask;

	ptrDIPData->ptrImageDataSrc1 = ptrImageData;	
	ptrDIPData->ptrInputParam = &Unsharp_Mask;

	ptrDIPData->uDIPMsg = TK_DIP_UNSHARP_MSG;	

	return DIP_ImageProcess( ptrDIPData );
}

/*======================================================================
���� ����( �ε巴�� )
========================================================================*/
int Opening( pDIP_DATA ptrDIPData, pIMAGE_DATA ptrImageData )
{
	ptrDIPData->ptrImageDataSrc1 = ptrImageData;
	ptrDIPData->uDIPMsg = TK_DIP_OPENING_MSG;	

	return DIP_ImageProcess( ptrDIPData );
}

/*======================================================================
���� �ݱ�( �����ϰ� )
========================================================================*/
int Closing( pDIP_DATA ptrDIPData, pIMAGE_DATA ptrImageData )
{
	ptrDIPData->ptrImageDataSrc1 = ptrImageData;
	ptrDIPData->uDIPMsg = TK_DIP_CLOSING_MSG;	

	return DIP_ImageProcess( ptrDIPData );
}
