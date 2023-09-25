#ifndef __TK_LPR_H__
#define __TK_LPR_H__


#ifdef TK_LPR_EXPORTS 
#define TK_LPR_API __declspec( dllexport )
#else
#define TK_LPR_API __declspec( dllimport )
#endif

#if (defined(__cplusplus) || defined(c_plusplus))
extern  "C" {
#endif

	/* Must Modify at TK_LPR.def */
	TK_LPR_API int /*CALLAGREEMENT*/ LPR_Initialize( pLIB_INIT_DATA  ptrInitData );
	TK_LPR_API int /*CALLAGREEMENT*/ LPR_Release( void  );
	
	TK_LPR_API int /*CALLAGREEMENT*/ LPR_DIFFProcess( pDIFF_DATA ptrDiffData );
	TK_LPR_API int /*CALLAGREEMENT*/ LPR_LPRProcess( pLPR_DATA ptrLPRData );
	TK_LPR_API int /*CALLAGREEMENT*/ LPR_IOCTL( pIOCTL_DATA ptrIOCTLData );

#if (defined(__cplusplus) || defined(c_plusplus))
}
#endif


#endif