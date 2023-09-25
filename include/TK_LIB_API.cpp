/*================================================================================
Copyright (c) 2011 Tory Hwang 
Modify : Tory in 2013 .01

Mail : tory45@empal.com
*=================================================================================*/


#include "stdafx.h"
#include "../include/TK_TYPE.h"
#include "../include/TK_LIB_API.h"

//#define TK_LIB_CHECK(VAL, INSTANCE) if(VAL == NULL) {INSTANCE = NULL; return FALSE; }


HINSTANCE g_hTKDIPInstance = NULL;
HINSTANCE g_hTKLPRInstance = NULL;
HINSTANCE g_hTKOCRInstance = NULL;

/*========================================================================
Initialize TK_DIP.dll Function Point 
=========================================================================*/
int ( * DIP_Initialize )( pLIB_INIT_DATA ) = NULL;
int ( * DIP_Release )( void )= NULL;

int ( * DIP_CopyImageAllocData )( pIMAGE_DATA, pIMAGE_DATA ) = NULL;
int ( * DIP_FreeAllocData )( pIMAGE_DATA ) = NULL;
int ( * DIP_ImageProcess)( pDIP_DATA ) = NULL;
int ( * DIP_IOCTL)( pIOCTL_DATA )= NULL;

/*========================================================================
Initialize TK_LPR.dll Function Point
=========================================================================*/
int ( * LPR_Initialize )( pLIB_INIT_DATA ) = NULL;
int ( * LPR_Release )( void  )= NULL;

int ( * LPR_DIFFProcess )( pDIFF_DATA )= NULL;
int ( * LPR_LPRProcess )( pLPR_DATA )= NULL;
int ( * LPR_IOCTL )( pIOCTL_DATA )= NULL;


/*========================================================================
Initialize TK_OCR.dll Function Point
=========================================================================*/
int ( * OCR_Initialize )( pLIB_INIT_DATA  ) = NULL;
int ( * OCR_Release )( void  ) = NULL;

int ( * OCR_OCRProcess )( pOCR_DATA /*ptrOCRData*/ ) = NULL;
int ( * OCR_Restudy )( void (* /* fnNotifyCallBack */)(int /* nLoopCnt */, double /*dErr*/)) =NULL;
int ( * OCR_IOCTL )( pIOCTL_DATA /*ptrIOCTLData*/ ) = NULL;

/*========================================================================
User Function
=========================================================================*/
static BOOL IsValidLIB( void * ptrFARPROC )
{
	if ( ptrFARPROC == NULL )
		return FALSE;

	return TRUE;
}

/*========================================================================
DIP.dll Load Function
=========================================================================*/
BOOL Load_DIP_LIB( LPCTSTR szModulePathName )
{
	if ( g_hTKDIPInstance != NULL )
		return TRUE;

	if ( ( g_hTKDIPInstance = LoadLibrary( szModulePathName ) ) == NULL )
		return FALSE;

	if ( IsValidLIB( ( DIP_Initialize = (int ( * )( pLIB_INIT_DATA ) )GetProcAddress( g_hTKDIPInstance , "DIP_Initialize") ))== FALSE)
	{
		Free_DIP_LIB();
		return FALSE;
	}

	if ( IsValidLIB( ( DIP_Release = (int ( * )( void ) )GetProcAddress( g_hTKDIPInstance , "DIP_Release") ))== FALSE)
	{
		Free_DIP_LIB();
		return FALSE;
	}

	if ( IsValidLIB( ( DIP_CopyImageAllocData = (int ( * )( pIMAGE_DATA , pIMAGE_DATA) )GetProcAddress( g_hTKDIPInstance , "DIP_CopyImageAllocData") ))== FALSE)
	{
		Free_DIP_LIB();
		return FALSE;
	}

	if ( IsValidLIB( ( DIP_FreeAllocData = (int ( * )( pIMAGE_DATA ) )GetProcAddress( g_hTKDIPInstance , "DIP_FreeAllocData") ))== FALSE)
	{
		Free_DIP_LIB();
		return FALSE;
	}

	if ( IsValidLIB( ( DIP_ImageProcess = (int ( * )( pDIP_DATA ) )GetProcAddress( g_hTKDIPInstance , "DIP_ImageProcess") ))== FALSE)
	{
		Free_DIP_LIB();
		return FALSE;
	}

	if ( IsValidLIB( ( DIP_IOCTL = (int ( * )( pIOCTL_DATA ) )GetProcAddress( g_hTKDIPInstance , "DIP_IOCTL") ))== FALSE)
	{
		Free_DIP_LIB();
		return FALSE;
	}
	
	return TRUE;

}
/*========================================================================
DIP.dll Free Function
=========================================================================*/
BOOL Free_DIP_LIB( void )
{
	if(g_hTKDIPInstance != NULL) 
	{
		if ( DIP_Release != NULL )
			DIP_Release();

		FreeLibrary(g_hTKDIPInstance);
		g_hTKDIPInstance = NULL;
	}	

	DIP_Initialize = NULL;
	DIP_Release = NULL;
	DIP_CopyImageAllocData = NULL;
	DIP_FreeAllocData = NULL;
	DIP_ImageProcess = NULL;
	DIP_IOCTL = NULL;

	return TRUE;
}

/*========================================================================
LPR.dll Load Function
=========================================================================*/
BOOL Load_LPR_LIB( LPCTSTR szModulePathName )
{
	
	if ( g_hTKLPRInstance != NULL )
		return TRUE;

	if ( ( g_hTKLPRInstance = LoadLibrary( szModulePathName ) ) == NULL )
		return FALSE;

	if ( IsValidLIB( ( LPR_Initialize = (int ( * )( pLIB_INIT_DATA ) )GetProcAddress( g_hTKLPRInstance , "LPR_Initialize") ))== FALSE)
	{
		Free_LPR_LIB();
		return FALSE;
	}

	if ( IsValidLIB( ( LPR_Release = (int ( * )( void ) )GetProcAddress( g_hTKLPRInstance , "LPR_Release") ))== FALSE)
	{
		Free_LPR_LIB();
		return FALSE;
	}

	if ( IsValidLIB( ( LPR_DIFFProcess = (int ( * )( pDIFF_DATA ) )GetProcAddress( g_hTKLPRInstance , "LPR_DIFFProcess") ))== FALSE)
	{
		Free_LPR_LIB();
		return FALSE;
	}

	if ( IsValidLIB( ( LPR_LPRProcess = (int ( * )( pLPR_DATA ) )GetProcAddress( g_hTKLPRInstance , "LPR_LPRProcess") ))== FALSE)
	{
		Free_LPR_LIB();
		return FALSE;
	}

	if ( IsValidLIB( ( LPR_IOCTL = (int ( * )( pIOCTL_DATA ) )GetProcAddress( g_hTKLPRInstance , "LPR_IOCTL") ))== FALSE)
	{
		Free_LPR_LIB();
		return FALSE;
	}

	return TRUE;
}
/*========================================================================
LPR.dll Free Function
=========================================================================*/
BOOL Free_LPR_LIB( void )
{
	if(g_hTKLPRInstance != NULL) 
	{
		if ( LPR_Release != NULL )
			LPR_Release( );

		FreeLibrary(g_hTKLPRInstance);
		g_hTKLPRInstance = NULL;
	}	

	LPR_Initialize = NULL;
	LPR_Release= NULL;
	LPR_DIFFProcess= NULL;
	LPR_LPRProcess= NULL;
	LPR_IOCTL= NULL;

	return TRUE;
}

/*========================================================================
OCR.dll Load Function
=========================================================================*/
BOOL Load_OCR_LIB( LPCTSTR szModulePathName )
{

	if ( g_hTKOCRInstance != NULL )
		return TRUE;

	if ( ( g_hTKOCRInstance = LoadLibrary( szModulePathName ) ) == NULL )
		return FALSE;

	if ( IsValidLIB( ( OCR_Initialize = (int ( * )( pLIB_INIT_DATA ) )GetProcAddress( g_hTKOCRInstance , "OCR_Initialize") ))== FALSE)
	{
		Free_OCR_LIB();
		return FALSE;
	}

	if ( IsValidLIB( ( OCR_Release = (int ( * )( void ) )GetProcAddress( g_hTKOCRInstance , "OCR_Release") ))== FALSE)
	{
		Free_OCR_LIB();
		return FALSE;
	}

	if ( IsValidLIB( ( OCR_OCRProcess = (int ( * )( pOCR_DATA ) )GetProcAddress( g_hTKOCRInstance , "OCR_OCRProcess") ))== FALSE)
	{
		Free_OCR_LIB();
		return FALSE;
	}

	if ( IsValidLIB( ( OCR_Restudy = ( int( * )( void ( * )(int , double) ))GetProcAddress( g_hTKOCRInstance , "OCR_Restudy") ))== FALSE)
	{
		Free_OCR_LIB();
		return FALSE;
	}

	if ( IsValidLIB( ( OCR_IOCTL = (int ( * )( pIOCTL_DATA ) )GetProcAddress( g_hTKOCRInstance , "OCR_IOCTL") ))== FALSE)
	{
		Free_OCR_LIB();
		return FALSE;
	}

	return TRUE;
}

/*========================================================================
OCR.dll Free Function
=========================================================================*/
BOOL Free_OCR_LIB( void )
{
	if(g_hTKOCRInstance != NULL) 
	{
		if ( OCR_Release != NULL )
			OCR_Release();

		FreeLibrary(g_hTKOCRInstance);
		g_hTKOCRInstance = NULL;
	}	

	OCR_Initialize = NULL;
	OCR_Release= NULL;
	OCR_OCRProcess= NULL;
	OCR_Restudy = NULL;
	OCR_IOCTL= NULL;

	return TRUE;
}
