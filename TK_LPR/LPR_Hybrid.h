#ifndef _LPR_HYBRID_H__
#define _LPR_HYBRID_H__

int Hybrid_Initialize( void );
int Hybrid_LPR( pDIP_DATA ptrDIPData, pLPR_RESULT ptrLPRResult, pIMAGE_DATA ptrSrcImageData );
int Hybrid_IOCTL( pIOCTL_DATA ptrIOCTLData );
int Hybrid_Release( void );

#endif
