#ifndef _LPR_H__
#define _LPR_H__

#if (defined(__cplusplus) || defined(c_plusplus))
extern  "C" {
#endif

	int Initialize( pLIB_INIT_DATA  ptrInitData );
	int Release( void  );

	int DIFFProcess( pDIFF_DATA ptrDiffData );
	int LPRProcess( pLPR_DATA ptrLPRData );
	int IOCTL( pIOCTL_DATA ptrIOCTLData );


#if (defined(__cplusplus) || defined(c_plusplus))
}
#endif

#endif