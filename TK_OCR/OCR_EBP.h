#ifndef _OCR_MLP_H__
#define _OCR_MLP_H__

#include "../include/TK_OCR_TYPE.h"

int EBP_Initialize( int bCreate );
int EBP_Save_NeuralNetwork( pOCR_FEATURE ptrFeature );
int EBP_Recog_NeuralNetwork( pOCR_FEATURE ptrFeature );
int EBP_ReStudy( void(* fnNotifyCallback)(int , double ), pOCR_CONF ptrOCRConf );

int EBP_NN_ForceSave( void );
int EBP_Release( void );

#endif