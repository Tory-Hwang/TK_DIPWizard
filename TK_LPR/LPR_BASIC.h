#ifndef __NMT_BASIC_H__
#define __NMT_BASIC_H__

int BASIC_Initialize( void );
int BASIC_GetRecognition( pLPR_CONF ptrLPRConf, pOCR_CONF ptrOCRConf, pDIP_DATA ptrDIPData, pIMAGE_DATA ptrDestImageData, 
						 pIMAGE_DATA ptrChgImageData, pIMAGE_DATA ptrImageDataSrc, pCVRECT ptrPlateRect );

int BASIC_IOCTL( pIOCTL_DATA ptrIOCTLData );
int BASIC_Release( void );

#endif