#ifndef _DIP_H__
#define _DIP_H__

#if (defined(__cplusplus) || defined(c_plusplus))
extern  "C" {
#endif

	int Initialize( pLIB_INIT_DATA ptrInitData  );
	int Release( void  );

	int CopyImageAllocData( pIMAGE_DATA ptrImageDataDest, pIMAGE_DATA ptrImageDataSrc );
	int FreeImageAllocData( pIMAGE_DATA ptrImageData );
	int ImageProcess( pDIP_DATA ptrDIPData );
	int IOCTL( pIOCTL_DATA ptrIOCTLData );


#if (defined(__cplusplus) || defined(c_plusplus))
}
#endif

#endif