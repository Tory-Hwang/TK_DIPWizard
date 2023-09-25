#ifndef _LPR_DIP_H__
#define _LPR_DIP_H__

int Histogram( pDIP_DATA ptrDIPData, pIMAGE_DATA ptrImageData, HISTOGRAM_PARAM_t * ptrParam );
int HistogramEqual( pDIP_DATA ptrDIPData, pIMAGE_DATA ptrImageData );

int Brightness( pDIP_DATA ptrDIPData, pIMAGE_DATA ptrImageData , int nBright);
int Beam( pDIP_DATA ptrDIPData, pIMAGE_DATA ptrImageData , int nTh);
int Binarization( pDIP_DATA ptrDIPData, pIMAGE_DATA ptrImageData, int nTh );
int Binarization_Iterate( pDIP_DATA ptrDIPData, pIMAGE_DATA ptrImageData );
int Binarization_Adaptive( pDIP_DATA ptrDIPData, pIMAGE_DATA ptrImageData, int nAdaptCnt, float fAdaptRatio );
int Binarization_ITG_Adaptive( pDIP_DATA ptrDIPData, pIMAGE_DATA ptrImageData, int nMaskSize );

int Binarization_Otsu( pDIP_DATA ptrDIPData, pIMAGE_DATA ptrImageData );

int GetBinarizationThreshod( pDIP_DATA ptrDIPData, pIMAGE_DATA ptrImageData );
int Binarization_DeNoise( pDIP_DATA ptrDIPData, pIMAGE_DATA ptrImageData );

int Roberts( pDIP_DATA ptrDIPData, pIMAGE_DATA ptrImageData, BYTE LineEdgeType );
int Prewitt( pDIP_DATA ptrDIPData, pIMAGE_DATA ptrImageData , BYTE LineEdgeType);
int Sobel( pDIP_DATA ptrDIPData, pIMAGE_DATA ptrImageData , BYTE LineEdgeType);
int Gaussian( pDIP_DATA ptrDIPData, pIMAGE_DATA ptrImageData, double dSigma );
int Labeling( pDIP_DATA ptrDIPData, pIMAGE_DATA ptrImageData );
int Canny( pDIP_DATA ptrDIPData, pIMAGE_DATA ptrImageData );
int Erosion( pDIP_DATA ptrDIPData, pIMAGE_DATA ptrImageData , BYTE SegmentType );
int Dilation( pDIP_DATA ptrDIPData, pIMAGE_DATA ptrImageData, BYTE SegmentType );

int SubImage( pDIP_DATA ptrDIPData, pIMAGE_DATA ptrImageData1, pIMAGE_DATA ptrImageData2, pIMAGE_DATA ptrImageDest );
int Median( pDIP_DATA ptrDIPData, pIMAGE_DATA ptrImageData );
int Mean( pDIP_DATA ptrDIPData, pIMAGE_DATA ptrImageData );
int WeightedMean( pDIP_DATA ptrDIPData, pIMAGE_DATA ptrImageData );
int Contrast( pDIP_DATA ptrDIPData, pIMAGE_DATA ptrImageData, int nRatio );
int OuterLine( pDIP_DATA ptrDIPData, pIMAGE_DATA ptrImageData1, pIMAGE_DATA ptrImageData2, pIMAGE_DATA ptrImageDest, BYTE bOuterType );

int Inverse( pDIP_DATA ptrDIPData, pIMAGE_DATA ptrImageData);
int ReSizeCubic( pDIP_DATA ptrDIPData, pIMAGE_DATA ptrImageData, pIMAGE_DATA ptrImageDest );

int Diffusion( pDIP_DATA ptrDIPData, pIMAGE_DATA ptrImageData, int nIter );
int MMSE( pDIP_DATA ptrDIPData, pIMAGE_DATA ptrImageData);

int LightInfo( pDIP_DATA ptrDIPData, pIMAGE_DATA ptrImageData, pLIGHTINFO_PARAM ptrParam );

int ZhangSuen_Thinning( pDIP_DATA ptrDIPData, pIMAGE_DATA ptrImageData, int objColor );
int Contrast_Binary( int nThreshold, pIMAGE_DATA ptrImageData );
int Make_BoxLine( pIMAGE_DATA ptrImageData );
int HoughLineTrans( pDIP_DATA ptrDIPData, pIMAGE_DATA ptrImageData, pHOUGH_PARAM ptrParam );

int Bilinear( pDIP_DATA ptrDIPData, pIMAGE_DATA ptrImageData, pIMAGE_DATA ptrImageDest, pBILINEAR_NOR_PARAM ptrParam );
int Unsharp( pDIP_DATA ptrDIPData, pIMAGE_DATA ptrImageData, int nMask );

int Opening( pDIP_DATA ptrDIPData, pIMAGE_DATA ptrImageData );
int Closing( pDIP_DATA ptrDIPData, pIMAGE_DATA ptrImageData );

#endif
