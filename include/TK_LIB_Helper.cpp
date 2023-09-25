#include "stdafx.h"
#include <stdlib.h>
#include "../include/TK_TYPE.h"
#include "../include/TK_LIB_Helper.h"

#ifndef PLATFORM_LINUX
	#pragma warning(disable: 4996)
#endif


#define BUFFERSIZE 512


/*====================================================================
Internal Function
=====================================================================*/



/*======================================================================
영상 투영
영상의 가로 / 세로 라인 별 객체의 개수 파악으로 엣지 정보를 알수 있다. 
========================================================================*/
int HLP_ProjectImg( int * ptrProject, pIMAGE_DATA ptrPrjImageData,  CVRECT_t * ptrChgRect, int PrjType)
{
	int i, j ;	
	int nTop, nBottom, nLeft, nRight;

	nTop	= ptrChgRect->nTop;
	nBottom = ptrChgRect->nBottom;
	nLeft	= ptrChgRect->nLeft;
	nRight	= ptrChgRect->nRight;

	//if ( nBottom >= ptrPrjImageData->nH ) nBottom = ptrPrjImageData->nH -1;
	//if ( nRight >= ptrPrjImageData->nW ) nRight = ptrPrjImageData->nW -1;

	switch ( PrjType )
	{

	case H_LINE_EDGE:
		/*높이별 각 화소의 값의 합 */

		for( i = nTop ; i <= nBottom ; i++ )
		{		
			if ( i < ptrPrjImageData->nH)
			{
				*( ptrProject + i ) =0;

				for( j = nLeft; j <=  nRight ; j++ )
				{	
					if ( j < ptrPrjImageData->nW )
					{
						if ( ( ptrPrjImageData->ptrSrcImage2D[ i ][ j ] != 0 )  )
						{	
							*( ptrProject + i ) += 1;
						}
					}
				}
			}
		}
		break;

	case W_LINE_EDGE:
		/*넓이별 각 화소의 값의 합 */
		for( j =nLeft ; j <= nRight ; j++ )
		{		
			if ( j < ptrPrjImageData->nW )
			{	
				*( ptrProject + j ) =0;

				for( i = nTop; i <=  nBottom ; i++ )
				{	
					if ( i < ptrPrjImageData->nH )
					{	
						if ( ( ptrPrjImageData->ptrSrcImage2D[ i ][ j ] == 255 )  )
						{
							*( ptrProject + j ) += 1;
						}
					}
				}
			}
		}
		break;
	}
	return ERR_SUCCESS;
}

/*====================================================================
External Function
=====================================================================*/
int HLP_GetBmpPaletteNums( int nBitCount )
{
	return 1 << nBitCount;
}

int HLP_GetCalBmpWidth( int nW, int nBitCount )
{
	/*BMP Image의 Width는 4의 배수로 이루어져 있음 */
	return ( ( nW * nBitCount + 31 ) / 32 ) *4 ;
}

BYTE ** HLP_AllocMatrix(int nH, int nW) 
{
	BYTE **Temp = NULL;

	Temp = new BYTE *[nH];

	if ( Temp == NULL )
	{
		return NULL;
	}

	for(int y = 0 ; y < nH ; y++)
	{
		Temp[y] = new BYTE[nW];
		if ( Temp[ y ] == NULL )
		{
			return NULL;
		}

		memset( Temp[ y ], 0, sizeof( BYTE) * nW );
	}
	return Temp;
}

int HLP_FreeMatrix(BYTE **Image, int nH, int nW)
{
	( void )nW;
	if ( Image == NULL )
		return 0;

	for(int y = 0 ; y < nH ; y++)
	{
		delete [] Image[y];
		Image[y] = NULL;
	}

	delete [] Image;
	Image = NULL;
	return 0;
}

int ** HLP_IntAllocMatrix( int nH, int nW)
{
	int **Temp = NULL;

	Temp = new int *[nH];

	if ( Temp == NULL )
	{
		return NULL;
	}

	for(int y = 0 ; y < nH ; y++)
	{
		Temp[y] = new int[nW];		
		if ( Temp[ y ] == NULL )
			return NULL;
		
		memset( Temp[ y ], 0, sizeof( int) * nW );
	}
	return Temp;
}

int HLP_IntFreeMatrix(int **Image, int nH, int nW)
{
	( void )nW;
	if ( Image == NULL )
		return 0;

	for(int y = 0 ; y < nH ; y++)
	{
		if ( Image[ y ]!= NULL )
		{
			delete [] Image[y];
			Image[y] = NULL;
		}
	}

	delete [] Image;
	Image = NULL;
	return 0;
}

double ** HLP_dAllocMatrix( int nH, int nW)
{
	double **Temp = NULL;

	Temp = new double *[nH];

	if ( Temp == NULL )
	{
		return NULL;
	}

	for(int y = 0 ; y < nH ; y++)
	{
		Temp[y] = new double[nW];		
		if ( Temp[ y ] == NULL )
			return NULL;

		memset( Temp[ y ], 0, sizeof( double ) * nW );
	}
	return Temp;
}

int HLP_dFreeMatrix(double ** Image, int nH, int nW)
{
	( void )nW;
	if ( Image == NULL )
		return 0;

	for(int y = 0 ; y < nH ; y++)
	{
		if ( Image[ y ]!= NULL )
		{
			delete [] Image[y];
			Image[y] = NULL;
		}
	}

	delete [] Image;
	Image = NULL;
	return 0;
}

float ** HLP_fAllocMatrix( int nH, int nW)
{
	float **Temp = NULL;

	Temp = new float *[nH];

	if ( Temp == NULL )
	{
		return NULL;
	}

	for(int y = 0 ; y < nH ; y++)
	{
		Temp[y] = new float[nW];		
		if ( Temp[ y ] == NULL )
			return NULL;

		memset( Temp[ y ], 0, sizeof( float ) * nW );
	}
	return Temp;
}

int HLP_fFreeMatrix(float ** Image, int nH, int nW)
{
	( void )nW;
	if ( Image == NULL )
		return 0;

	for(int y = 0 ; y < nH ; y++)
	{
		if ( Image[ y ]!= NULL )
		{
			delete [] Image[y];
			Image[y] = NULL;
		}
	}

	delete [] Image;
	Image = NULL;
	return 0;
}

int HLP_FreeImageAllocData( IMAGE_DATA_t * ptrImageData )
{
	if ( ptrImageData == NULL  )
		return 0;

	if ( ptrImageData->ptrSrcImage2D != NULL )
	{
		for(int y = 0 ; y < ptrImageData->nH ; y++)
		{		
			if ( ptrImageData->ptrSrcImage2D[ y ] != NULL )
			{
				delete [] ptrImageData->ptrSrcImage2D[ y ];
				ptrImageData->ptrSrcImage2D[ y ] = NULL;
			}
		}

		delete [] ptrImageData->ptrSrcImage2D;
		ptrImageData->ptrSrcImage2D = NULL;
	}	

	if ( ptrImageData->ptrPalette != NULL )
	{
		delete [] ptrImageData->ptrPalette;
		ptrImageData->ptrPalette = NULL;
	}

	ptrImageData->nBitCount = 0;
	ptrImageData->nW = 0;
	ptrImageData->nH = 0;
	ptrImageData->nImageSize = 0;


	return 0;
}

int HLP_CopyImageData( IMAGE_DATA_t * ptrDestImageData , IMAGE_DATA_t * ptrSrcImageData  )
{
	int nLineByte=0;
	int nColors=0;
	int i;

	if ( ptrDestImageData == NULL || ptrSrcImageData == NULL  )
		return FALSE;
	
	nLineByte = HLP_GetCalBmpWidth( ptrSrcImageData->nW, ptrSrcImageData->nBitCount);
	nColors = HLP_GetBmpPaletteNums( ptrSrcImageData->nBitCount );

	ptrDestImageData->nBitCount = ptrSrcImageData->nBitCount;
	ptrDestImageData->nW = ptrSrcImageData->nW;
	ptrDestImageData->nH = ptrSrcImageData->nH;
	ptrDestImageData->nImageSize = ptrSrcImageData->nImageSize;

	if ( ptrDestImageData->ptrSrcImage2D == NULL )
	{
		ptrDestImageData->ptrSrcImage2D = HLP_AllocMatrix( ptrSrcImageData->nH , nLineByte);
	}	

	if ( ptrDestImageData->ptrSrcImage2D == NULL )
		return ERR_ALLOC_BUF;

	if ( ptrSrcImageData->nBitCount != 24  && ptrSrcImageData->ptrPalette != NULL )
	{
		ptrDestImageData->ptrPalette = new RGBQUAD[ nColors ];
		if( ptrDestImageData->ptrPalette != NULL )
		{
			memcpy( ptrDestImageData->ptrPalette , ptrSrcImageData->ptrPalette, sizeof( RGBQUAD)* nColors );
		}
	}	

	if ( ptrSrcImageData->ptrSrcImage2D != NULL )
	{
		for (i=0; i<ptrSrcImageData->nH; i++)
		{			
			memcpy( *( ptrDestImageData->ptrSrcImage2D + i ) , *( ptrSrcImageData->ptrSrcImage2D + i ) , nLineByte );		
		}
	}

	return 0;
}

int HLP_CopyImageDataNotAlloc( IMAGE_DATA_t * ptrDestImageData , IMAGE_DATA_t * ptrSrcImageData )
{
	int nLineByte=0;
	int nColors=0;
	int i;

	if ( ptrDestImageData == NULL || ptrSrcImageData == NULL || ptrSrcImageData->ptrSrcImage2D == NULL )
		return FALSE;

	nLineByte = HLP_GetCalBmpWidth( ptrSrcImageData->nW, ptrSrcImageData->nBitCount);
	nColors = HLP_GetBmpPaletteNums( ptrSrcImageData->nBitCount );

	ptrDestImageData->nBitCount = ptrSrcImageData->nBitCount;
	ptrDestImageData->nW = ptrSrcImageData->nW;
	ptrDestImageData->nH = ptrSrcImageData->nH;
	ptrDestImageData->nImageSize = ptrSrcImageData->nImageSize;

	if ( ptrDestImageData->ptrSrcImage2D == NULL )
		return ERR_ALLOC_BUF;

	for (i=0; i<ptrSrcImageData->nH; i++)
	{			
		memcpy( *( ptrDestImageData->ptrSrcImage2D + i ) , *( ptrSrcImageData->ptrSrcImage2D + i ) , nLineByte );		
	}

	return ERR_SUCCESS;
}	

int HLP_CopyImageDataWithRect( IMAGE_DATA_t * ptrDestImageData , IMAGE_DATA_t * ptrSrcImageData, CVRECT_t *ptrRect, int nExtend )
{
	int nLineByte=0;
	int nColors=0;
	int i;
	int nWidth,nOrgWidth, nHeight;
	int nTopPos, nLeftPos = 0;
	float fRatio=0;
	int nOne = 0;
	int nOneLineByte=0;

	nWidth = ptrRect->nRight - ptrRect->nLeft;
	nHeight = ptrRect->nBottom - ptrRect->nTop;	
	nOrgWidth = nWidth;

	if ( nWidth <= 0 ) return ERR_ALLOC_BUF;
	if ( nHeight <= 0 ) return ERR_ALLOC_BUF;	

	if ( nExtend )
	{
		fRatio = (float)nHeight/nWidth;
		nWidth	+=2;
		nHeight +=2;		

		/*"1"문자 일때"*/
		if ( fRatio >=2 )
		{
			nWidth	+=5;
			nOne =1;
			nLeftPos = 6;
			nOneLineByte = HLP_GetCalBmpWidth( nOrgWidth, ptrSrcImageData->nBitCount);
		}
		else
		{
			nLeftPos = 1;
		}
		nTopPos  =1;		
	}
	else
	{
		nTopPos	 = 0;
		nLeftPos = 0;
	}

	if ( ptrDestImageData == NULL || ptrSrcImageData == NULL || ptrSrcImageData->ptrSrcImage2D == NULL )
		return FALSE;

	nLineByte = HLP_GetCalBmpWidth( nWidth, ptrSrcImageData->nBitCount);
	nColors = HLP_GetBmpPaletteNums( ptrSrcImageData->nBitCount );

	ptrDestImageData->nBitCount = ptrSrcImageData->nBitCount;
	ptrDestImageData->nW = nWidth;
	ptrDestImageData->nH = nHeight;
	ptrDestImageData->nImageSize = nHeight * nLineByte;

	if ( ptrDestImageData->ptrSrcImage2D == NULL )
	{
		ptrDestImageData->ptrSrcImage2D = HLP_AllocMatrix( nHeight + 1 , nLineByte);
	}	

	if ( ptrDestImageData->ptrSrcImage2D == NULL )
		return ERR_ALLOC_BUF;

	if ( ptrSrcImageData->nBitCount != 24  && ptrSrcImageData->ptrPalette != NULL )
	{
		ptrDestImageData->ptrPalette = new RGBQUAD[ nColors ];
		if( ptrDestImageData->ptrPalette != NULL )
		{
			memcpy( ptrDestImageData->ptrPalette , ptrSrcImageData->ptrPalette, sizeof( RGBQUAD)* nColors );
		}
	}	

	for (i=ptrRect->nTop; i<= ptrRect->nBottom; i++, nTopPos++ )
	{
		if ( i < ptrSrcImageData->nH )
		{	
			if (nOne  && nExtend )
			{
				memcpy( *( ptrDestImageData->ptrSrcImage2D + nTopPos ) + nLeftPos , *( ptrSrcImageData->ptrSrcImage2D + i )+ ptrRect->nLeft , nOneLineByte );
			}
			else
			{
				memcpy( *( ptrDestImageData->ptrSrcImage2D + nTopPos ) + nLeftPos , *( ptrSrcImageData->ptrSrcImage2D + i )+ ptrRect->nLeft , nLineByte );
			}
			
		}
	}

	return ERR_SUCCESS;
}

int HLP_CopyImageDataWithEmpty( IMAGE_DATA_t * ptrDestImageData , int nBitCount, int nHeight,int nWidth )
{
	int nLineByte=0;
	int nColors=0;
	
	if ( ptrDestImageData == NULL )
		return FALSE;

	nLineByte = HLP_GetCalBmpWidth( nWidth, nBitCount);
	nColors = HLP_GetBmpPaletteNums( nBitCount );

	ptrDestImageData->nBitCount = nBitCount;
	ptrDestImageData->nW = nWidth;
	ptrDestImageData->nH = nHeight;
	ptrDestImageData->nImageSize = nHeight * nLineByte;

	if ( ptrDestImageData->ptrSrcImage2D == NULL )
	{
		ptrDestImageData->ptrSrcImage2D = HLP_AllocMatrix( nHeight , nLineByte);
	}	

	if ( ptrDestImageData->ptrSrcImage2D == NULL )
		return ERR_ALLOC_BUF;		
	
	return ERR_SUCCESS;
}

int HLP_MakeDefaultPallet( IMAGE_DATA_t * ptrImageData )
{
	int i, nColors=0;

	if ( ptrImageData == NULL )
		return -1;

	if ( ptrImageData->nBitCount == 24 )
		return -1;

	nColors = HLP_GetBmpPaletteNums( ptrImageData->nBitCount );

	ptrImageData->ptrPalette = new RGBQUAD[ nColors  ];
	if ( ptrImageData->ptrPalette == NULL )
		return -1;

	memset( ptrImageData->ptrPalette, 0, sizeof( RGBQUAD )*nColors);
	for(i = 0 ; i < nColors ; i++) 
	{
		ptrImageData->ptrPalette[i].rgbBlue = BYTE(i);
		ptrImageData->ptrPalette[i].rgbGreen = BYTE(i);
		ptrImageData->ptrPalette[i].rgbRed = BYTE(i);
		ptrImageData->ptrPalette[i].rgbReserved = 0x00;
	}

	return ERR_SUCCESS;
}


int HLP_DrawLineRect( pIMAGE_DATA ptrImageData, pCVRECT ptrRect, int nBright )
{
	int i,j ;	

	if ( ptrImageData == NULL || ptrRect == NULL )
		return ERR_INVALID_DATA;


	if ( ptrRect->nTop < 0 ) ptrRect->nTop = 0;
	if ( ptrRect->nLeft < 0 ) ptrRect->nLeft = 0;

	if( ptrRect->nBottom >= ptrImageData->nH )
		ptrRect->nBottom = ptrImageData->nH -1 ;

	if( ptrRect->nRight >= ptrImageData->nW )
		ptrRect->nRight = ptrImageData->nW -1 ;

	for( i = ptrRect->nTop ; i < ptrRect->nBottom ; i++ )
	{		
		ptrImageData->ptrSrcImage2D[ i ][ ptrRect->nLeft ] = ( BYTE )nBright;
		ptrImageData->ptrSrcImage2D[ i ][ ptrRect->nRight ] = ( BYTE )nBright;
	}

	for( j = ptrRect->nLeft ; j < ptrRect->nRight ; j++ )
	{
		ptrImageData->ptrSrcImage2D[ ptrRect->nTop ][ j ] = ( BYTE )nBright;
		ptrImageData->ptrSrcImage2D[ ptrRect->nBottom ][ j ] = ( BYTE )nBright;
	}

	return ERR_SUCCESS;
}

int HLP_PartDrawLineRect( pIMAGE_DATA ptrImageData, pCVRECT ptrRect, int nBright, int nLine )
{
	int i,j ;	

	if ( ptrImageData == NULL || ptrRect == NULL )
		return ERR_INVALID_DATA;


	if ( ptrRect->nTop < 0 ) ptrRect->nTop = 0;
	if ( ptrRect->nLeft < 0 ) ptrRect->nLeft = 0;

	if( ptrRect->nBottom >= ptrImageData->nH )
		ptrRect->nBottom = ptrImageData->nH -1 ;

	if( ptrRect->nRight >= ptrImageData->nW )
		ptrRect->nRight = ptrImageData->nW -1 ;

	if ( (nLine & W_LINE_EDGE ) == W_LINE_EDGE  )
	{
		for( i = ptrRect->nTop ; i < ptrRect->nBottom ; i++ )
		{		
			ptrImageData->ptrSrcImage2D[ i ][ ptrRect->nLeft ] = ( BYTE )nBright;
			ptrImageData->ptrSrcImage2D[ i ][ ptrRect->nRight ] = ( BYTE )nBright;
		}
	}
	
	if ( ( nLine & H_LINE_EDGE ) == H_LINE_EDGE )
	{
		for( j = ptrRect->nLeft ; j < ptrRect->nRight ; j++ )
		{
			ptrImageData->ptrSrcImage2D[ ptrRect->nTop ][ j ] = ( BYTE )nBright;
			ptrImageData->ptrSrcImage2D[ ptrRect->nBottom ][ j ] = ( BYTE )nBright;
		}
	}

	return ERR_SUCCESS;
}




void HLP_print_int_buf(int * ptrdata, int nSize )
{
	int i; 	
	char szdebug[ 20 ];

	memset(szdebug , 0, sizeof( szdebug));

	for( i = 0 ; i < nSize ; i++ )
	{
		sprintf( szdebug, "%02d ", ptrdata[ i ] & 0xFFFFFFFF );
		if ( i != 0 && i % 20 == 0 )
			sprintf( szdebug, "\r\n" );

#ifndef PLATFORM_LINUX
		OutputDebugStringA(szdebug);
#else
		putstr( szdebug );
#endif
	}

	sprintf_s( szdebug, "\r\n" );

#ifndef PLATFORM_LINUX
	OutputDebugStringA(szdebug);
#else
	putstr( szdebug );
#endif

}



void HLP_print_buf(BYTE * ptrdata, int nSize )
{
	int i; 	
	char szdebug[ 20 ];

	memset(szdebug , 0, sizeof( szdebug));

	for( i = 0 ; i < nSize ; i++ )
	{
		sprintf_s( szdebug, "0x%02x ", ptrdata[ i ] & 0xFF );
		if ( i != 0 && i % 16 == 0 )
			sprintf_s( szdebug, "\r\n" );

#ifndef PLATFORM_LINUX
		OutputDebugStringA(szdebug);
#else
		putstr( szdebug );
#endif


	}	
}

void HLP_print_dbg(int nTtype, char * ptrString,...)
{	
	va_list args; 
	int i; 
	char szstring[ BUFFERSIZE ];
	char szdebug[ BUFFERSIZE ];

	memset(szstring , 0, sizeof( szstring));
	memset(szdebug , 0, sizeof( szdebug));

	if ( ptrString != NULL )
	{
		va_start(args, ptrString); 
		i=vsprintf_s( szstring,ptrString,args );
		va_end(args); 
	}
	
	if( nTtype == DBG_NONE)
	{
		sprintf_s(szdebug, "%s", "[ TK INFO ] ");
	}
	else if( nTtype == DBG_INFO )
	{
		sprintf_s(szdebug, "%s", "[ TK INFO ] ");
	}
	else if( nTtype == DBG_ERR )
	{
		sprintf_s(szdebug, "%s", "[ TK INFO ] ");
	}
	else if( nTtype == DBG_LINE )
	{
		sprintf_s(szdebug, "%s", DBG_SEPAR);
	}

	if ( strlen(szstring) > ( BUFFERSIZE - 50 ) )
	{
		szstring[ BUFFERSIZE - ( 50 +1 ) ] = 0x00;
		szstring[ BUFFERSIZE - 50 ] = 0x00;		
	}
	
	strcat_s( szdebug, szstring );
	strcat_s( szdebug, "\r\n" );
	

#ifndef PLATFORM_LINUX
	OutputDebugStringA(szdebug);
#else
	putstr( szdebug );
#endif
}



/*===========================================================================
[ 이미지의 가로 / 세로 비율 ]	
============================================================================*/
float HLP_Cal_ImageRatio( CVRECT_t * ptrRect )
{
	float fPlateRatio = 0;
	int nWidth, nHeight;
	int nTop, nBottom, nLeft, nRight;

	nTop = ptrRect->nTop;
	nBottom = ptrRect->nBottom;
	nLeft = ptrRect->nLeft;
	nRight = ptrRect->nRight;

	nWidth = nRight - nLeft;
	nHeight = nBottom - nTop;

	if ( nHeight == 0 ) nHeight = 1;
	fPlateRatio = ( (float)nWidth / (float)nHeight );

	return fPlateRatio;
}
