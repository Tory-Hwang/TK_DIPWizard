// TK_OCR.cpp : DLL 응용 프로그램에 대한 진입점을 정의합니다.
//

#include "stdafx.h"
#include "../include/TK_TYPE.h"

#include "TK_OCR.h"
#include "OCR.h"


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

TK_OCR_API int OCR_Initialize( pLIB_INIT_DATA  ptrInitData  )
{	
	return Initialize( ptrInitData );
}

TK_OCR_API int OCR_Release( void  )
{
	return Release();
}

TK_OCR_API int OCR_OCRProcess( pOCR_DATA ptrOCRData )
{
	if ( ptrOCRData == NULL )
		return ERR_NO_PARAM;

	return OCRProcess( ptrOCRData );
}

TK_OCR_API int OCR_Restudy( void(* fnNotifyCallback)(int , double) )
{
	return OCRRestudy( fnNotifyCallback );
}

TK_OCR_API int OCR_IOCTL( pIOCTL_DATA ptrIOCTLData )
{
	return IOCTL( ptrIOCTLData );
}

#ifdef _MANAGED
#pragma managed(pop)
#endif

