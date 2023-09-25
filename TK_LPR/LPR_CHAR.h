#ifndef _LPR_CHAR_H__
#define _LPR_CHAR_H__


int CHAR_Initialize( void );
int CHAR_GetRecognition(pLPR_CONF ptrLPRConf, pOCR_CONF ptrOCRConf, pDIP_DATA ptrDIPData, pIMAGE_DATA ptrDestImageData,
				   pIMAGE_DATA ptrChgImageData,pIMAGE_DATA ptrImageDataSrc, pCVRECT ptrPlateRect, pLPRCHAR ptrLPRChars );

int CHAR_IOCTL( pIOCTL_DATA ptrIOCTLData );
int CHAR_Release( void );

#endif