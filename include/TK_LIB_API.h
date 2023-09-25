#ifndef __TK_LIB_API_H__
#define __TK_LIB_API_H__

#define TK_DIP_LIB_NAME  L"TK_DIP.dll"
#define TK_LPR_LIB_NAME  L"TK_LPR.dll"
#define TK_OCR_LIB_NAME  L"TK_OCR.dll"

BOOL Load_DIP_LIB( LPCTSTR szModulePathName );
BOOL Free_DIP_LIB( void );

BOOL Load_LPR_LIB( LPCTSTR szModulePathName );
BOOL Free_LPR_LIB( void );

BOOL Load_OCR_LIB( LPCTSTR szModulePathName );
BOOL Free_OCR_LIB( void );

/*===================================================
TK_DIP Extern Function Point
====================================================*/
extern int ( * DIP_Initialize )( pLIB_INIT_DATA  );
extern int ( * DIP_Release )( void  );

extern int ( * DIP_CopyImageAllocData )( pIMAGE_DATA /*ptrImageDataDest*/ , pIMAGE_DATA /*ptrImageDataSrc*/ );
extern int ( * DIP_FreeAllocData )( pIMAGE_DATA /*ptrImageData*/ );
extern int ( * DIP_ImageProcess )( pDIP_DATA /*ptrDIPData*/ );
extern int ( * DIP_IOCTL )( pIOCTL_DATA /*ptrIOCTLData*/ );

/*===================================================
TK_LPR Extern Function Point
====================================================*/
extern int ( * LPR_Initialize )( pLIB_INIT_DATA );
extern int ( * LPR_Release )( void  );

extern int ( * LPR_DIFFProcess )( pDIFF_DATA /*ptrDiffData*/ );
extern int ( * LPR_LPRProcess )( pLPR_DATA /*ptrLPRData*/ );
extern int ( * LPR_IOCTL )( pIOCTL_DATA /*ptrIOCTLData*/ );

/*===================================================
TK_OCR Extern Function Point
====================================================*/
extern int ( * OCR_Initialize )( pLIB_INIT_DATA  );
extern int ( * OCR_Release )( void  );

extern int ( * OCR_OCRProcess )( pOCR_DATA /*ptrOCRData*/ );
extern int ( * OCR_Restudy )( void (* /* fnNotifyCallBack */)(int /* nLoopCnt */, double /*dErr*/));
extern int ( * OCR_IOCTL )( pIOCTL_DATA /*ptrIOCTLData*/ );


#endif