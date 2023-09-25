// TK_LPR.cpp : DLL 응용 프로그램에 대한 진입점을 정의합니다.
//

#include "stdafx.h"
#include "../include/TK_TYPE.h"

#include "TK_LPR.h"
#include "LPR.h"


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

TK_LPR_API int LPR_Initialize( pLIB_INIT_DATA  ptrInitData)
{	
	return Initialize( ptrInitData );
}

TK_LPR_API int LPR_Release( void  )
{
	return Release();
}

TK_LPR_API int LPR_DIFFProcess( pDIFF_DATA ptrDiffData )
{
	if ( ptrDiffData == NULL )
		return ERR_NO_PARAM;

	return DIFFProcess ( ptrDiffData );
}

TK_LPR_API int LPR_LPRProcess( pLPR_DATA ptrLPRData )
{
	if ( ptrLPRData == NULL )
		return ERR_NO_PARAM;

	return LPRProcess( ptrLPRData );
}

TK_LPR_API int LPR_IOCTL( pIOCTL_DATA ptrIOCTLData )
{
	return IOCTL( ptrIOCTLData );
}

#ifdef _MANAGED
#pragma managed(pop)
#endif

