#ifndef __TK_DIP_H__
#define __TK_DIP_H__


#ifdef TK_DIP_EXPORTS 
#define TK_DIP_API __declspec( dllexport )
#else
#define TK_DIP_API __declspec( dllimport )
#endif

#if (defined(__cplusplus) || defined(c_plusplus))
extern  "C" {
#endif

	/* Must Modify at TK_DIP.def */
	TK_DIP_API int /*CALLAGREEMENT*/ DIP_Initialize( void  );
	TK_DIP_API int /*CALLAGREEMENT*/ DIP_Release( void  );

	TK_DIP_API int /*CALLAGREEMENT*/ DIP_CopyImageAllocData( pIMAGE_DATA ptrImageDataDest , pIMAGE_DATA ptrImageDataSrc );
	TK_DIP_API int /*CALLAGREEMENT*/ DIP_FreeAllocData( pIMAGE_DATA ptrImageData );
	TK_DIP_API int /*CALLAGREEMENT*/ DIP_ImageProcess( pDIP_DATA ptrDIPData );
	TK_DIP_API int /*CALLAGREEMENT*/ DIP_IOCTL( pIOCTL_DATA ptrIOCTLData );

#if (defined(__cplusplus) || defined(c_plusplus))
}
#endif


#endif