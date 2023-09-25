#ifndef _OCR_H__
#define _OCR_H__

#if (defined(__cplusplus) || defined(c_plusplus))
extern  "C" {
#endif

	int Initialize( pLIB_INIT_DATA ptrInitData  );
	int Release( void  );

	int OCRProcess( pOCR_DATA ptrOCRData );
	int OCRRestudy( void(* fnNotifyCallback)(int , double) );
	int IOCTL( pIOCTL_DATA ptrIOCTLData );


#if (defined(__cplusplus) || defined(c_plusplus))
}
#endif

#endif