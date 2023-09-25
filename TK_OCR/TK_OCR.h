#ifndef __TK_OCR_H__
#define __TK_OCR_H__


#ifdef TK_OCR_EXPORTS 
#define TK_OCR_API __declspec( dllexport )
#else
#define TK_OCR_API __declspec( dllimport )
#endif

#if (defined(__cplusplus) || defined(c_plusplus))
extern  "C" {
#endif

	/* Must Modify at TK_OCR.def */
	TK_OCR_API int /*CALLAGREEMENT*/ OCR_Initialize( void  );
	TK_OCR_API int /*CALLAGREEMENT*/ OCR_Release( void  );

	TK_OCR_API int /*CALLAGREEMENT*/ OCR_OCRProcess( pOCR_DATA ptrOCRData );
	TK_OCR_API int /*CALLAGREEMENT*/ OCR_Restudy( void(* fnNotifyCallback)(int , double) );	
	TK_OCR_API int /*CALLAGREEMENT*/ OCR_IOCTL( pIOCTL_DATA ptrIOCTLData );

#if (defined(__cplusplus) || defined(c_plusplus))
}
#endif


#endif