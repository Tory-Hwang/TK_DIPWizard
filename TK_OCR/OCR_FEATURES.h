#ifndef _OCR_FEATURE_H__
#define _OCR_FEATURE_H__

#include "../include/TK_OCR_TYPE.h"

int FT_Initialize( void );
int FT_SetFeatureImage( pIMAGE_DATA ptrImage, pOCR_FEATURE ptrFeature , int nOCR_CODE );
int FT_SetOCRConf( pOCR_CONF ptrOCRConf );

int FT_Release( void );




#endif