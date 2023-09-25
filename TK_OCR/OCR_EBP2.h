#ifndef _OCR_EBP_EXT_H__
#define _OCR_EBP_EXT_H__

#include "../include/TK_OCR_TYPE.h"

int EBP2_Initialize( int bCreate );
int EBP2_Save_NeuralNetwork( pOCR_FEATURE ptrFeature );
int EBP2_Recog_NeuralNetwork( pOCR_FEATURE ptrFeature );
int EBP2_ReStudy( void(* fnNotifyCallback)(int , double ),  pOCR_CONF ptrOCRConf );

int EBP2_NN_ForceSave( void );
int EBP2_Release( void );

#endif