#ifndef __NMT_PLATE_H__
#define __NMT_PLATE_H__

int PLATE_Initialize( void );
int PLATE_GetRecognition( pLPR_CONF ptrLPRConf, pOCR_CONF ptrOCRConf, pDIP_DATA ptrDIPData, pIMAGE_DATA ptrDestImageData, 
						 pIMAGE_DATA ptrChgImageData, pIMAGE_DATA ptrImageDataSrc, pCVRECT ptrPlateRect );

int PLATE_IOCTL( pIOCTL_DATA ptrIOCTLData );
int PLATE_Release( void );

#endif