#ifndef _DIP_LABELING_H__
#define _DIP_LABELING_H__

int DoLabeling( pIMAGE_DATA ptrImageData );

float GetPx( int nInit, int nEnd, int * ptrHist );
float GetMean( int nInit, int nEnd, int * ptrHist );
int GetMax( float * ptrVector, int nCnt );

int ReleaseLabeling( void );

#endif

