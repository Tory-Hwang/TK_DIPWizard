#ifndef _TK_LIB_HELPER_H__
#define _TK_LIB_HELPER_H__

#include <cstdio>
#include <iostream>

#define LOC std::cout << "debug:" << __FILE__ << ":" << __LINE__ << " ";

/* macro using var args */
#define DEBUG_PRINT(fmt,...) LOC OutputDebugStringA(fmt,__VA_ARGS__);


int HLP_GetBmpPaletteNums( int nBitCount );
int HLP_GetCalBmpWidth( int nW, int nBitCount );	

BYTE ** HLP_AllocMatrix(int nH, int nW) ;
int HLP_FreeMatrix(BYTE **Image, int nH, int nW); 

int ** HLP_IntAllocMatrix( int nH, int nW); 
int HLP_IntFreeMatrix(int **Image, int nH, int nW); 

double ** HLP_dAllocMatrix( int nH, int nW); 
int HLP_dFreeMatrix(double ** Image, int nH, int nW); 

float ** HLP_fAllocMatrix( int nH, int nW); 
int HLP_fFreeMatrix(float ** Image, int nH, int nW); 


int HLP_FreeImageAllocData( IMAGE_DATA_t * ptrImageData );
int HLP_CopyImageData( IMAGE_DATA_t * ptrDestImageData , IMAGE_DATA_t * ptrSrcImageData  );
int HLP_CopyImageDataWithRect( IMAGE_DATA_t * ptrDestImageData , IMAGE_DATA_t * ptrSrcImageData, CVRECT_t *ptrRect,int nExtend  );
int HLP_CopyImageDataWithEmpty( IMAGE_DATA_t * ptrDestImageData , int nBitCount , int nHeight,int nWidth );
int HLP_CopyImageDataNotAlloc( IMAGE_DATA_t * ptrDestImageData , IMAGE_DATA_t * ptrSrcImageData );

int HLP_MakeDefaultPallet( IMAGE_DATA_t * ptrImageData );

int HLP_DrawLineRect( pIMAGE_DATA ptrImageData, pCVRECT ptrRect, int nBright );
int HLP_PartDrawLineRect( pIMAGE_DATA ptrImageData, pCVRECT ptrRect, int nBright, int nLine );

int HLP_ProjectImg( int * ptrProject, pIMAGE_DATA ptrPrjImageData,  CVRECT_t * ptrChgRect, int PrjType);

void HLP_print_dbg(int nTtype, char * ptrString,...);
void HLP_print_buf(BYTE * ptrdata, int nSize);
void HLP_print_int_buf(int * ptrdata, int nSize);

float HLP_Cal_ImageRatio( CVRECT_t * ptrRect );
#endif
