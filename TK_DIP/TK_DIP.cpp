// TK_DIP.cpp : DLL 응용 프로그램에 대한 진입점을 정의합니다.
//


#include "stdafx.h"
#include "../include/TK_TYPE.h"

#include "DIP.h"
#include "TK_DIP.h"


#ifdef _MANAGED
#pragma managed(push, off)
#endif

BOOL APIENTRY DllMain( HMODULE hModule,
					  DWORD  ul_reason_for_call,
					  LPVOID lpReserved
					  )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		break;
	case DLL_THREAD_ATTACH:
		break;
	case DLL_THREAD_DETACH:
		break;
	case DLL_PROCESS_DETACH:
		break;
	}

	return TRUE;
}

TK_DIP_API int DIP_Initialize( pLIB_INIT_DATA ptrInitData  )
{
	return Initialize( ptrInitData );
}

TK_DIP_API int DIP_Release( void  )
{
	return Release();
}

TK_DIP_API int DIP_CopyImageAllocData( pIMAGE_DATA ptrImageDataDest , pIMAGE_DATA ptrImageDataSrc )
{
	if ( ptrImageDataSrc == NULL )
		return ERR_NO_PARAM;

	return CopyImageAllocData( ptrImageDataDest, ptrImageDataSrc);
}

TK_DIP_API int DIP_FreeAllocData( pIMAGE_DATA ptrImageData )
{
	if ( ptrImageData == NULL )
		return ERR_NO_PARAM;

	return FreeImageAllocData( ptrImageData);
}

TK_DIP_API int DIP_ImageProcess( pDIP_DATA ptrDIPData )
{
	if ( ptrDIPData == NULL )
		return ERR_NO_PARAM;

	return ImageProcess( ptrDIPData );
}

TK_DIP_API int DIP_IOCTL( pIOCTL_DATA ptrIOCTLData )
{
	return IOCTL( ptrIOCTLData );
}


#ifdef _MANAGED
#pragma managed(pop)
#endif


