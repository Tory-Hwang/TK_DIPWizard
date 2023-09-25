#ifndef _OCR_COM_H__
#define _OCR_COM_H__

#include "../include/TK_OCR_TYPE.h"

int OCR_COM_CreateFeatureBuf( void );
int OCR_COM_RleaseFeatureBuf( void );
pOCR_FETURE_MANAGER OCR_GetFeatureManager( void );


int OCR_COM_SaveFeature( pOCR_FEATURE ptrFeature );
int OCR_COM_ReadFeature( void );
int OCR_COM_ReadFeatureOnlyCnt( void );


int OCR_COM_SetCallBackFunc( fnNotifyCallBack ptrCallBack );
int OCR_COM_SendNotifyStatus( int nVal, double dVal );

float OCR_COM_RandVal( void );
float OCR_COM_SigmoidVal( float fVal );

#endif