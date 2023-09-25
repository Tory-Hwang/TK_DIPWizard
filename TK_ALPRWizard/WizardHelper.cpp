/*================================================================================
Copyright (c) 2011 Tory Hwang 
Modify : Tory in 2013 .01

Mail : tory45@empal.com
*=================================================================================*/



#include "StdAfx.h"
#include "WizardHelper.h"

#include "ALPRWizard_Def.h"
#include "ijl.h"

CWizardHelper::CWizardHelper(void)
{
	memset(m_tcCurrentPath, 0, sizeof( m_tcCurrentPath ));
}

CWizardHelper::~CWizardHelper(void)
{
}

/* ==========================================================================
2차 배열 메모리 할당 ( double point )
============================================================================*/
BYTE **CWizardHelper::AllocMatrix(int nH, int nW) 
{
	BYTE **Temp = NULL;

	Temp = new BYTE *[nH];

	if ( Temp == NULL )
	{
		return NULL;
	}

	for(int y = 0 ; y < nH ; y++)
	{
		Temp[y] = new BYTE[nW];
		if ( Temp[ y ] == NULL )
		{
			return NULL;
		}
	}

	return Temp;
}

/* ==========================================================================
2차 배열 메모리 해제
============================================================================*/
int  CWizardHelper::FreeMatrix(BYTE **Image, int nH, int nW) 
{
	( void )nW;
	if ( Image == NULL )
		return 0;

	for(int y = 0 ; y < nH ; y++)
	{
		delete [] Image[y];
		Image[y] = NULL;
	}

	delete [] Image;

	return 0;
}

/* ==========================================================================
IMAGE_DATA_t 구조체의 메모리 해제 
============================================================================*/
int CWizardHelper::FreeImageAllocData( IMAGE_DATA_t * ptrImageData )
{
	if ( ptrImageData == NULL  )
		return 0;

	if ( ptrImageData->ptrSrcImage2D != NULL )
	{
		for(int y = 0 ; y < ptrImageData->nH ; y++)
		{		
			if ( ptrImageData->ptrSrcImage2D[ y ] != NULL )
			{
				delete [] ptrImageData->ptrSrcImage2D[ y ];
				ptrImageData->ptrSrcImage2D[ y ] = NULL;
			}
		}

		delete [] ptrImageData->ptrSrcImage2D;
		ptrImageData->ptrSrcImage2D = NULL;
	}	

	if ( ptrImageData->ptrPalette != NULL )
	{
		delete [] ptrImageData->ptrPalette;
		ptrImageData->ptrPalette = NULL;
	}
	
	ptrImageData->nBitCount = 0;
	ptrImageData->nW = 0;
	ptrImageData->nH = 0;
	ptrImageData->nImageSize = 0;

	return 0;

}

int CWizardHelper::FreeImageAllocDataWithSize( IMAGE_DATA_t * ptrImageData , int nH, int nW )
{
	if ( ptrImageData == NULL  )
		return 0;

	if ( ptrImageData->ptrSrcImage2D != NULL )
	{
		for(int y = 0 ; y < nH ; y++)
		{		
			if ( ptrImageData->ptrSrcImage2D[ y ] != NULL )
			{
				delete [] ptrImageData->ptrSrcImage2D[ y ];
				ptrImageData->ptrSrcImage2D[ y ] = NULL;
			}
		}

		delete [] ptrImageData->ptrSrcImage2D;
		ptrImageData->ptrSrcImage2D = NULL;
	}	

	if ( ptrImageData->ptrPalette != NULL )
	{
		delete [] ptrImageData->ptrPalette;
		ptrImageData->ptrPalette = NULL;
	}

	ptrImageData->nBitCount = 0;
	ptrImageData->nW = 0;
	ptrImageData->nH = 0;
	ptrImageData->nImageSize = 0;

	return 0;

}


/* ==========================================================================
IMAGE_DATA_t 구조체 복사 
============================================================================*/
BOOL CWizardHelper::CopyImageData( IMAGE_DATA_t * ptrDestImageData , IMAGE_DATA_t * ptrSrcImageData, BOOL bCopyBuf  )
{
	int nLineByte=0;
	int nColors=0;
	int i;

	if ( ptrDestImageData == NULL || ptrSrcImageData == NULL || ptrSrcImageData->ptrSrcImage2D == NULL )
		return FALSE;

	nLineByte = GetCalBmpWidth( ptrSrcImageData->nW, ptrSrcImageData->nBitCount);
	nColors = GetBmpPaletteNums( ptrSrcImageData->nBitCount );

	ptrDestImageData->nBitCount = ptrSrcImageData->nBitCount;
	ptrDestImageData->nW = ptrSrcImageData->nW;
	ptrDestImageData->nH = ptrSrcImageData->nH;
	ptrDestImageData->nImageSize = ptrSrcImageData->nImageSize;
	
	/* image Raw data 복사 */
	if ( ptrDestImageData->ptrSrcImage2D == NULL )
	{
		ptrDestImageData->ptrSrcImage2D = AllocMatrix( ptrSrcImageData->nH , nLineByte);
	}

	if ( ptrDestImageData->ptrSrcImage2D == NULL )
		return FALSE;

	/* Pallet 복사 */
	if ( ptrSrcImageData->nBitCount != 24  && ptrSrcImageData->ptrPalette != NULL )
	{
		ptrDestImageData->ptrPalette = new RGBQUAD[ nColors ];
		if( ptrDestImageData->ptrPalette != NULL )
		{
			memcpy( ptrDestImageData->ptrPalette , ptrSrcImageData->ptrPalette, sizeof( RGBQUAD)* nColors );
		}
	}
	
	if ( bCopyBuf )
	{
		for (i=0; i < ptrSrcImageData->nH; i++)
		{			
			memcpy( *( ptrDestImageData->ptrSrcImage2D + i ) , *( ptrSrcImageData->ptrSrcImage2D + i ) , nLineByte );
		}
	}
	else
	{
		for (i=0; i < ptrSrcImageData->nH; i++)
		{			
			memset( *( ptrDestImageData->ptrSrcImage2D + i ) , 255 , nLineByte );
		}
	}

	return TRUE;
}

/* ==========================================================================
같은 크기의 IMAGE_DATA_t 구조체 생성
============================================================================*/
int CWizardHelper::CopyImageDataWithEmpty( IMAGE_DATA_t * ptrDestImageData , int nBitCount , int nHeight,int nWidth )
{
	int nLineByte=0;
	int nColors=0;

	if ( ptrDestImageData == NULL )
		return FALSE;

	nLineByte = GetCalBmpWidth( nWidth, nBitCount);
	nColors = GetBmpPaletteNums( nBitCount );

	ptrDestImageData->nBitCount = nBitCount;
	ptrDestImageData->nW = nWidth;
	ptrDestImageData->nH = nHeight;
	ptrDestImageData->nImageSize = nHeight * nLineByte;

	if ( ptrDestImageData->ptrSrcImage2D == NULL )
	{
		ptrDestImageData->ptrSrcImage2D = AllocMatrix( nHeight , nLineByte);
	}	

	if ( ptrDestImageData->ptrSrcImage2D == NULL )
		return ERR_ALLOC_BUF;		

	return ERR_SUCCESS;
}

/* ==========================================================================
실행 중인 프로그램의 위치 
============================================================================*/
BOOL CWizardHelper::GetCurrentPath( TCHAR * ptrtcPath )
{
	CString strMsg;	
	TCHAR strBlank[ MAX_PATH ];

	memset( strBlank, 0, sizeof( strBlank) );

	if (GetModuleFileName( AfxGetInstanceHandle( ), strBlank, sizeof( strBlank) ) == 0 )
	{
		return FALSE;
	}		

	strMsg.Format(L"%s", strBlank );
	strMsg.ReleaseBuffer();
	strMsg = strMsg.Left(strMsg.ReverseFind('\\'));
	
	wcscpy_s( m_tcCurrentPath, strMsg.GetLength() + 1 , strMsg );
	if ( ptrtcPath != NULL )
	{
		wcscpy_s( ptrtcPath, wcslen( m_tcCurrentPath) + 1, m_tcCurrentPath );
	}
	

	return TRUE;
}

BOOL CWizardHelper::GetCurrentPathChar( char * ptrPath )
{
	int len;
	
	if( wcslen(m_tcCurrentPath ) == 0 )
	{
		GetCurrentPath(m_tcCurrentPath  );
	}

	len = WideCharToMultiByte(CP_ACP, 0, m_tcCurrentPath, -1, NULL, 0, NULL, NULL);
	if( ptrPath != NULL )
	{
		WideCharToMultiByte(CP_ACP, 0, m_tcCurrentPath, -1, ptrPath, len, 0,0); 
	}
	

	return TRUE;
}
/* ==========================================================================
특정 디렉토리 파일 정보 읽기  
============================================================================*/
BOOL CWizardHelper::GetFileInfo( TCHAR * ptrCurPath, FILE_MANAGER_t * ptrFileManager)
{
	WIN32_FIND_DATA info;
	HANDLE hp;	
	TCHAR strFullPath[ MAX_PATH ];
	TCHAR strFileName[MAX_PATH ];
	int AddCnt = 0;
	CString strMsg;
	CString strTemp;

	FILE_INFO_t * ptrFileInfo= NULL;

	if ( ptrCurPath == NULL || wcslen( ptrCurPath) == 0)
	{
		return FALSE;
	}

	hp = NULL;	
	memset( strFullPath, 0, sizeof( strFullPath ));
	wsprintf( strFullPath,L"%s\\*.*", ptrCurPath);
	ptrFileInfo = ( FILE_INFO_t * )&ptrFileManager->Files[ 0 ];

	hp = FindFirstFile( strFullPath, &info);	
	do
	{			
		memset( strFileName, 0, sizeof( strFileName ));

		if ( AddCnt > MAX_FILE_CNT )
		{
			FindClose(hp);
			return FALSE;
		}

		strTemp.Format(L"%s", info.cFileName );
		strTemp.MakeUpper();

		if ( strTemp.Right( 4 ) == L".RAW" || strTemp.Right( 4 ) == L".BMP" || strTemp.Right( 4 ) == L".JPG")
		{			
			wsprintf( ptrFileInfo->tcName,L"%s", info.cFileName );
			ptrFileInfo++;
			ptrFileManager->Cnt++;			
		}

	}while(FindNextFile(hp, &info));

	if (hp != INVALID_HANDLE_VALUE)
		FindClose(hp);

	return TRUE;
}

/* ==========================================================================
BMP WIDHT는 4BYTE의 배수로 채워져 있기 때문에 4BYTE 단위가 아닐경우 0 패딩한다.
============================================================================*/
int CWizardHelper::GetCalBmpWidth( int nW, int nBitCount )
{
	return ( ( nW * nBitCount + 31 ) / 32 ) *4 ;
}

/* ==========================================================================
Bitcount에 의한 Pallet 사이즈 
8bit 일 경우 : 2의 8승개 즉, 256개의 Pallet 이 존재한다. 
============================================================================*/
int CWizardHelper::GetBmpPaletteNums( int nBitCount )
{	
	return 1 << nBitCount;
}

/* ==========================================================================
BMP 파일 저장
BITMAPFILEHEADER + BITMAPINFOHEADER + RGBQUAD( Pallet ) + Data
============================================================================*/
int CWizardHelper::SaveRaw( LPCTSTR tcCurPath,  LPCTSTR tcOrgFileName, IMAGE_DATA_t * ptrImageData )

{
	CFile	cfile;
	CString strMsg;

	BITMAPFILEHEADER bmiFileHeader;
	BITMAPINFOHEADER bmInfoHeader;

	int nOffsetSize = 0;
	int nLineByte = 0;
	int nColors =0;
	int nBmpSize = 0;
	BYTE * ptrImage1D = NULL;
	int i, j;
	TCHAR tcFullFileName[ MAX_PATH ];
	TCHAR tcNewFileName[ MAX_PATH ];

	if( tcCurPath == NULL || tcOrgFileName == NULL ||  ptrImageData == NULL )
		return ERR_INVALID_DATA;
	

	memset( tcFullFileName, 0, sizeof( tcFullFileName ));
	memset( tcNewFileName, 0, sizeof( tcNewFileName ));

	
	strMsg.Format(L"%s", tcOrgFileName );
	strMsg.ReleaseBuffer();
	strMsg = strMsg.Left(strMsg.ReverseFind('.'));
	
	wcscpy_s( tcNewFileName, strMsg.GetLength() + 1 , strMsg );
	wsprintf( tcFullFileName, L"%s\\%s_TKW%d_TKH%d.RAW", tcCurPath, tcNewFileName, ptrImageData->nW, ptrImageData->nH);

	if( !cfile.Open( tcFullFileName, CFile::modeCreate | CFile::modeWrite | CFile::shareDenyWrite) )
		return ERR_INVALID_DATA;

	nLineByte = GetCalBmpWidth( ptrImageData->nW, ptrImageData->nBitCount );	
	nBmpSize = nLineByte * ptrImageData->nH;	
	
	ptrImage1D = new BYTE[ nBmpSize ];
	
	if( ptrImage1D == NULL )
	{
		return ERR_INVALID_DATA;
	}

	for( i = 0 ; i < ptrImageData->nH ; i++ )
	{
		for( j = 0 ; j < nLineByte ; j++)
		{
			ptrImage1D[ ( i * nLineByte ) + j ] = ptrImageData->ptrSrcImage2D[ i ][ j ];
		}
	}			
	
	// DIB 구조(BITMAPINFOHEADER + 색상 테이블 + 픽셀 데이터) 쓰기
	cfile.Write( ptrImage1D, nBmpSize);

	if ( ptrImage1D != NULL )
		delete [] ptrImage1D;

	// 파일 닫기
	cfile.Close();

	return ERR_SUCCESS;
}


/* ==========================================================================
BMP 파일 저장
BITMAPFILEHEADER + BITMAPINFOHEADER + RGBQUAD( Pallet ) + Data
============================================================================*/
int CWizardHelper::SaveBmp( LPCTSTR tcFileName, IMAGE_DATA_t * ptrImageData )
{
	CFile	cfile;
	BITMAPFILEHEADER bmiFileHeader;
	BITMAPINFOHEADER bmInfoHeader;

	int nOffsetSize = 0;
	int nLineByte = 0;
	int nColors =0;
	int nBmpSize = 0;
	BYTE * ptrImage1D = NULL;
	int i, j;

	if( tcFileName == NULL || ptrImageData == NULL )
		return ERR_INVALID_DATA;
	
	if( !cfile.Open( tcFileName, CFile::modeCreate | CFile::modeWrite | CFile::shareDenyWrite) )
		return ERR_INVALID_DATA;

	nLineByte = GetCalBmpWidth( ptrImageData->nW, ptrImageData->nBitCount );
	nColors = GetBmpPaletteNums( ptrImageData->nBitCount );
	nBmpSize = nLineByte * ptrImageData->nH;

	/*
	typedef struct IMAGE_DATA_
	{
		int nBitCount;
		int nW;
		int nH;
		int nImageSize;	
		BYTE ** ptrSrcImage2D;
		RGBQUAD * ptrPalette;	
	}IMAGE_DATA_t, *pIMAGE_DATA;
	*/

	if ( ptrImageData->nBitCount == 24 )
	{
		nOffsetSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
	}
	else
	{
		nOffsetSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + ( sizeof(RGBQUAD) * nColors );
	}

	//BITMAPFILEHEADER 변수설정
	bmiFileHeader.bfType			= 'M'*0x0100 + 'B';	// 항상 "BM"
	bmiFileHeader.bfSize			= (DWORD)(  nOffsetSize + nBmpSize );
	bmiFileHeader.bfReserved1		= 0;		// 항상 0
	bmiFileHeader.bfReserved2		= 0;		// 항상 0
	bmiFileHeader.bfOffBits			= (DWORD)( nOffsetSize );

	//INFOHEADER 변수설정 
	bmInfoHeader.biSize				= sizeof(BITMAPINFOHEADER);
	bmInfoHeader.biPlanes			= 1;
	bmInfoHeader.biBitCount			= (WORD)ptrImageData->nBitCount;
	bmInfoHeader.biCompression		= BI_RGB;
	bmInfoHeader.biXPelsPerMeter	= 0;
	bmInfoHeader.biYPelsPerMeter	= 0;
	bmInfoHeader.biClrUsed			= 0;
	bmInfoHeader.biClrImportant		= 0;

	bmInfoHeader.biWidth			= ptrImageData->nW;
	bmInfoHeader.biHeight			= ptrImageData->nH;
	bmInfoHeader.biSizeImage		= ptrImageData->nImageSize;


	ptrImage1D = new BYTE[ nBmpSize ];
	
	if( ptrImage1D == NULL )
	{
		return ERR_INVALID_DATA;
	}

	for( i = 0 ; i < ptrImageData->nH ; i++ )
	{
		for( j = 0 ; j < nLineByte ; j++)
		{
			ptrImage1D[ ( i * nLineByte ) + j ] = ptrImageData->ptrSrcImage2D[ ptrImageData->nH - 1 - i ][ j ];
		}
	}	
		
	// BITMAPFILEHEADER
	cfile.Write((LPSTR)&bmiFileHeader, sizeof( bmiFileHeader));
	// BMPINFOHEADER
	cfile.Write((LPSTR)&bmInfoHeader, sizeof( bmInfoHeader));
	// PALETTE 
	if ( ptrImageData->nBitCount != 24 )
	{
		cfile.Write(ptrImageData->ptrPalette, ( sizeof(RGBQUAD) * nColors ) );
	}

	// DIB 구조(BITMAPINFOHEADER + 색상 테이블 + 픽셀 데이터) 쓰기
	cfile.Write( ptrImage1D, nBmpSize);

	if ( ptrImage1D != NULL )
		delete [] ptrImage1D;

	// 파일 닫기
	cfile.Close();

	return ERR_SUCCESS;
}
/* ==========================================================================
BMP 파일 읽기 
BITMAPFILEHEADER + BITMAPINFOHEADER + RGBQUAD( Pallet ) + Data
============================================================================*/
int CWizardHelper::ReadBmp(LPCTSTR tcFileName, IMAGE_DATA_t * ptrImageData )
{
	BITMAPFILEHEADER bmiFileHeader;
	BITMAPINFOHEADER bmiHeader;

	CFile cfile;
	BOOL bOpen;
	int nColors;
	BYTE *ptrImage1D = NULL;
	int BmpSize=0;
	int nLineByte = 0;	
	int i, j;	

	if ( ptrImageData == NULL )
		return ERR_INVALID_DATA;

	bOpen = cfile.Open(tcFileName, CFile::modeRead | CFile::typeBinary);

	if(!bOpen) 
		return ERR_INVALID_DATA;

	/* Get BMP File Info */
	cfile.Read(&bmiFileHeader, sizeof(BITMAPFILEHEADER));

	/* check BM */
	if(bmiFileHeader.bfType != 'M'*0x0100 + 'B') 
	{
		cfile.Close();
		return ERR_INVALID_DATA;
	}

	/* Get BMP Info */
	cfile.Read(&bmiHeader, sizeof(BITMAPINFOHEADER));
	/* Check Compression */
	if(bmiHeader.biCompression != BI_RGB) 
	{
		cfile.Close();
		return ERR_INVALID_DATA;
	}
	
	nLineByte = GetCalBmpWidth( bmiHeader.biWidth, bmiHeader.biBitCount);
	nColors = GetBmpPaletteNums( bmiHeader.biBitCount );

	ptrImageData->nBitCount = bmiHeader.biBitCount;
	ptrImageData->nW = bmiHeader.biWidth;
	ptrImageData->nH = bmiHeader.biHeight;
	
	BmpSize = nLineByte * ptrImageData->nH;
	ptrImageData->nImageSize = BmpSize;

	ptrImage1D = new BYTE[ BmpSize ];	
	ptrImageData->ptrSrcImage2D = AllocMatrix( ptrImageData->nH , nLineByte);
	
	if ( ptrImage1D == NULL || ptrImageData->ptrSrcImage2D == NULL )
	{
		if ( ptrImage1D != NULL )
			delete [] ptrImage1D;

		return ERR_INVALID_DATA;
	}	

	/* bitcount가 24( 트루컬러 )일 경우Pallet이 존재하지 않는다. */
	if ( bmiHeader.biBitCount != 24 )
	{
		ptrImageData->ptrPalette = new RGBQUAD[ nColors ];
		if( ptrImageData->ptrPalette == NULL )
		{
			if ( ptrImage1D != NULL )
				delete [] ptrImage1D;

			return ERR_INVALID_DATA;
		}
		cfile.Read(ptrImageData->ptrPalette, sizeof(RGBQUAD) * nColors );
	}

	/* Move Offset at bmp pixel */
	cfile.Seek(bmiFileHeader.bfOffBits, CFile::begin);
	cfile.Read(ptrImage1D, BmpSize );	
	cfile.Close();

	/* BMP 이미지 데이터는 상하 역순으로 저장되어 있다.  */
	for (i=0; i<bmiHeader.biHeight; i++)
	{			
		for (j=0; j< nLineByte; j++)
		{

			ptrImageData->ptrSrcImage2D[ bmiHeader.biHeight -  1 - i ][ j ] = *( ptrImage1D +  ( i * nLineByte ) + j );
		}
	}
	
	if ( ptrImage1D != NULL )
		delete [] ptrImage1D;
	
	return ERR_SUCCESS;
}

/* ==========================================================================
RAW 파일 읽기 
사용되는 RAW 파일은 GRAYSCALE만 처리하도록 제한을 두며 
VISION SENSOR에서 발취한 이미지만을 사용한것으로 한다. 
============================================================================*/
int CWizardHelper::ReadRaw(LPCTSTR tcFileName, IMAGE_DATA_t * ptrImageData )
{
	CFile cfile;
	BOOL bOpen;
	BYTE *ptrImage1D = NULL;
	int nRawSize=0;
	int nLineByte = 0;
	int ReadSize =0;
	
	int nColors=0;
	int i, j;	
	
	CString strFileName;
	CString strH,strW;
	
	int nTKWPos, nTKHPos, nFilePos;
	int nLen ;

	if ( ptrImageData == NULL || ptrImageData->nBitCount != 8 )
		return ERR_INVALID_DATA;

	nTKWPos = nTKHPos = nLen = nFilePos= 0;
	strFileName.Format(L"%s", tcFileName );
	strFileName.ReleaseBuffer();
	strFileName = strFileName.Mid(strFileName.ReverseFind('\\')+1);

	nTKWPos = strFileName.Find(L"TKW");
	nTKHPos = strFileName.Find(L"TKH");
	nFilePos = strFileName.Find(L".RAW");

	/* Format이 맞지 않으면 에러로 처리 한다 */
	if ( nTKHPos < 0 || nTKWPos < 0 )
		return ERR_INVALID_DATA;

	nLen = (nTKHPos - nTKWPos ) -4;
	strW = strFileName.Mid(nTKWPos+3, nLen );

	nLen = (nFilePos - nTKHPos ) -3;
	strH = strFileName.Mid(nTKHPos+3, nLen );	
	bOpen = cfile.Open(tcFileName, CFile::modeRead | CFile::typeBinary);

	if(!bOpen) 
	{
		return ERR_INVALID_DATA;
	}

	ptrImageData->nW = _ttoi( strW ) ;
	ptrImageData->nH = _ttoi( strH ) ;
	ptrImageData->nBitCount = DEFAULT_BIT_CNT;
	nLineByte = GetCalBmpWidth(ptrImageData->nW, ptrImageData->nBitCount );
	nColors = GetBmpPaletteNums( ptrImageData->nBitCount );
	nRawSize = ptrImageData->nH * nLineByte; 
	ptrImageData->nImageSize = nRawSize;

	/* ptrPalette, ptrImage의 free 는 ImageData가 소멸되는 시점에 됨 */
	ptrImage1D = new BYTE[ nRawSize ];
	
	ptrImageData->ptrPalette = new RGBQUAD[ nColors  ];
	
	ptrImageData->ptrSrcImage2D = AllocMatrix( ptrImageData->nH,  nLineByte );
	
	if( ptrImage1D == NULL || ptrImageData->ptrPalette == NULL || ptrImageData->ptrSrcImage2D == NULL )
	{
		if ( ptrImage1D != NULL )
			delete [] ptrImage1D;

		return ERR_INVALID_DATA;
	}

	/* Make 8bit Palette */
	memset( ptrImage1D, 0, nRawSize );	
	memset( ptrImageData->ptrPalette, 0, sizeof( RGBQUAD )*nColors);
	for(i = 0 ; i < nColors ; i++) 
	{
		ptrImageData->ptrPalette[i].rgbBlue = BYTE(i);
		ptrImageData->ptrPalette[i].rgbGreen = BYTE(i);
		ptrImageData->ptrPalette[i].rgbRed = BYTE(i);
		ptrImageData->ptrPalette[i].rgbReserved = 0x00;
	}
	
	ReadSize = cfile.Read(ptrImage1D, nRawSize );	
	cfile.Close();
	
	for( i = 0 ; i < ptrImageData->nH ; i++ )
	{
		for( j= 0 ; j< nLineByte ; j++ )
		{
			ptrImageData->ptrSrcImage2D[ i ][ j ] = *( ptrImage1D +  ( i * nLineByte ) + j );
		}
	}	

	if ( ptrImage1D != NULL )
		delete [] ptrImage1D;

	return ERR_SUCCESS;
}

/* ==========================================================================
JPEG 파일 읽기 
../lib/ijl15.lib : Jpeg 이미지 로드용 lib
../include/ijl.h : ijl15.lib Head 파일
============================================================================*/
int CWizardHelper::ReadJpeg(LPCTSTR tcFileName, IMAGE_DATA_t * ptrImageData )
{
	IJLERR jerr;
	DWORD nchannels;
	DWORD dib_pad_bytes;
	char szFileName[ MAX_PATH ];
	BYTE *ptrImage1D = NULL;
	int nJpegSize=0;
	int nLineByte = 0;

	int i, j;	
	JPEG_CORE_PROPERTIES jcprops;	
	
	BYTE * pData, Blue, Green, Red, Y;

	USES_CONVERSION;

	jerr = ijlInit(&jcprops);

	if(IJL_OK != jerr)
	{		
		return ERR_INVALID_DATA;
	}
	
	memset( szFileName, 0, sizeof( szFileName ));
	strcpy_s( szFileName, wcslen( tcFileName ) + 1 , W2A( tcFileName) );
	jcprops.JPGFile = const_cast<LPSTR>(szFileName);

	jerr = ijlRead(&jcprops,IJL_JFILE_READPARAMS);
	if(IJL_OK != jerr)
	{		
		return ERR_INVALID_DATA;
	}

	ptrImageData->nW = jcprops.JPGWidth;
	ptrImageData->nH = jcprops.JPGHeight;
	ptrImageData->nBitCount = 24;
	
	nchannels = 3; 
	nLineByte = GetCalBmpWidth(ptrImageData->nW, ptrImageData->nBitCount );
	dib_pad_bytes = IJL_DIB_PAD_BYTES(ptrImageData->nW,nchannels);

	//dib_line_width = ptrImageData->nW * nchannels;
	//wholeimagesize = ( dib_line_width + dib_pad_bytes ) * ptrImageData->nH;

	nJpegSize = ptrImageData->nH * nLineByte; 	
	ptrImageData->nImageSize = nJpegSize;
	ptrImage1D = new BYTE[ nJpegSize ];
	
	if ( ptrImage1D == NULL  )
	{
		ijlFree(&jcprops);
		return ERR_INVALID_DATA;
	}

	jcprops.DIBWidth = ptrImageData->nW;
	jcprops.DIBHeight = -ptrImageData->nH; 
	jcprops.DIBChannels = nchannels;
	jcprops.DIBColor = IJL_BGR;
	jcprops.DIBPadBytes = dib_pad_bytes;	
	jcprops.DIBBytes = reinterpret_cast<BYTE*>(ptrImage1D );

	switch(jcprops.JPGChannels)
	{
	case 1:
		{
			jcprops.JPGColor = IJL_G;
			break;
		}
	case 3:
		{
			jcprops.JPGColor = IJL_YCBCR;
			break;
		}
	default:
		{
			jcprops.DIBColor = (IJL_COLOR)IJL_OTHER;
			jcprops.JPGColor = (IJL_COLOR)IJL_OTHER;
			break;
		}
	}
	
	jerr = ijlRead(&jcprops,IJL_JFILE_READWHOLEIMAGE);
	if(IJL_OK != jerr)
	{
		ijlFree(&jcprops);
		return ERR_INVALID_DATA;
	}
	
	/* Glayscale 이미지인 경우 */
	if ( jcprops.JPGColor == IJL_G )
	{
		int nLineByte8bit =0;
		
		ptrImageData->nBitCount = 8;
		nLineByte8bit = GetCalBmpWidth(ptrImageData->nW, ptrImageData->nBitCount );		
		ptrImageData->nImageSize = ptrImageData->nH * nLineByte8bit;
		ptrImageData->ptrSrcImage2D = AllocMatrix( ptrImageData->nH , nLineByte8bit);

		if ( ptrImageData->ptrSrcImage2D == NULL  )
		{
			ijlFree(&jcprops);
			return ERR_INVALID_DATA;
		}

		for (i=0; i<ptrImageData->nH; i++)
		{			
			pData = ( ptrImage1D +  ( (ptrImageData->nH - i -1 ) * nLineByte ) );
			for (j=0; j< ptrImageData->nW ; j++)
			{	
				Blue = (BYTE)(*pData++);
				Green = (BYTE)(*pData++);
				Red = (BYTE)(*pData++);
				Y = (BYTE)LIMIT_VAL((0.299 * Red + 0.587 * Green+ 0.114 * Blue + 0.5));
				ptrImageData->ptrSrcImage2D[ i ][ j ] = Y;
			}
		}
	}
	else
	{		
		ptrImageData->ptrSrcImage2D = AllocMatrix( ptrImageData->nH , nLineByte);

		if ( ptrImageData->ptrSrcImage2D == NULL  )
		{
			ijlFree(&jcprops);
			return ERR_INVALID_DATA;
		}

		for (i=0; i<ptrImageData->nH; i++)
		{			
			for (j=0; j< nLineByte; j++)
			{
				ptrImageData->ptrSrcImage2D[ ptrImageData->nH -  1 - i ][ j ] = *( ptrImage1D +  ( i * nLineByte ) + j );
			}
		}
	}	
	
	if (ptrImage1D != NULL )
		delete [] ptrImage1D ;
	
	ijlFree(&jcprops);
	
	
	return ERR_SUCCESS;
}


/* ==========================================================================
BITMAP 이미지 출력 
============================================================================*/
void CWizardHelper::DrawWin32( BOOL bChar, HDC hdc, IMAGE_DATA_t * ptrImageData, int nExtX , int nExtY   )
{
	BITMAPINFOHEADER BmiHeader;
	LPBITMAPINFO pBmpInfo;	
	BYTE * pTemp, *ptrImage1D = NULL;
	int nPaletteNum=0;
	int i, j;
	int nLineByte = 0;
	
	if ( hdc == NULL || ptrImageData == NULL /*|| ptrImageData->ptrSrcImage2D == NULL*/ )
	{
		return ;
	}

	nPaletteNum = GetBmpPaletteNums( ptrImageData->nBitCount );
	if ( ptrImageData->nBitCount != 24 )
	{
		pTemp= new BYTE[ sizeof(BITMAPINFOHEADER) + sizeof(RGBQUAD) * nPaletteNum  ];
	}
	else
	{
		pTemp= new BYTE[ sizeof(BITMAPINFOHEADER) ];
	}

	pBmpInfo = ( LPBITMAPINFO)pTemp;

	BmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	BmiHeader.biPlanes = 1;
	BmiHeader.biBitCount = (WORD)ptrImageData->nBitCount;
	BmiHeader.biCompression = BI_RGB;
	BmiHeader.biXPelsPerMeter = 0;
	BmiHeader.biYPelsPerMeter = 0;
	BmiHeader.biClrUsed = 0;
	BmiHeader.biClrImportant = 0;

	BmiHeader.biWidth = ptrImageData->nW;
	BmiHeader.biHeight = ptrImageData->nH;
	BmiHeader.biSizeImage = ptrImageData->nImageSize;
	
	nLineByte = GetCalBmpWidth(ptrImageData->nW, ptrImageData->nBitCount );
	ptrImage1D = new BYTE[ ptrImageData->nH * nLineByte ];

	for( i = 0 ; i < ptrImageData->nH ; i++ )
	{
		for( j = 0 ; j < nLineByte ; j++)
		{
			ptrImage1D[ ( i * nLineByte ) + j ] = ptrImageData->ptrSrcImage2D[ ptrImageData->nH - 1 - i ][ j ];
		}
	}

	memcpy( &pBmpInfo->bmiHeader, &BmiHeader, sizeof(BITMAPINFOHEADER) );
	if ( ptrImageData->ptrPalette != NULL )
	{
		memcpy( &pBmpInfo->bmiColors, ptrImageData->ptrPalette, sizeof(RGBQUAD) * nPaletteNum);	
	}		
	else
	{
		if ( ptrImageData->nBitCount != 24 )
		{
			MakeDefaultPallet(  ptrImageData );
			memcpy( &pBmpInfo->bmiColors, ptrImageData->ptrPalette, sizeof(RGBQUAD) * nPaletteNum);
		}
	}

	if ( bChar )
	{
		StretchDIBits(hdc,
			nExtX,
			nExtY,
			BmiHeader.biWidth ,
			BmiHeader.biHeight,
			0,0,
			BmiHeader.biWidth,
			BmiHeader.biHeight,
			ptrImage1D,		
			pBmpInfo,
			DIB_RGB_COLORS,
			SRCCOPY);
	}
	else
	{
		StretchDIBits(hdc,
			nExtX,
			nExtY,
			BmiHeader.biWidth,
			BmiHeader.biHeight,
			0,0,
			BmiHeader.biWidth,
			BmiHeader.biHeight,
			ptrImage1D,		
			pBmpInfo,
			DIB_RGB_COLORS,
			SRCCOPY);

	}
	
	if ( ptrImage1D != NULL )
		delete [] ptrImage1D;

	if ( pTemp != NULL )
		delete[] pTemp;
}

/* ==========================================================================
LINE 출력 
============================================================================*/
void CWizardHelper::DrawRectLine( HDC hdc, CVRECT_t * ptrRect , int bDraw, int nExtX , int nExtY )
{
	int i ;
	CVRECT_t * ptrTempRect =NULL;	
	HPEN current_pen, old_pen;

	if ( bDraw == 0 )
		return;

	current_pen=CreatePen(PS_SOLID, 1, RGB(255, 0, 0) );
	old_pen=(HPEN)SelectObject(hdc, current_pen);
	
	SelectObject(hdc, GetStockObject(NULL_BRUSH) );
	for( i = 0 ; i < MAX_LPR_AREA_CNT ; i++ )
	{
		ptrTempRect = ( CVRECT_t*)&ptrRect[ i ];
		if ( ptrTempRect != NULL && ptrTempRect->nSeqNum > 0 )
		{
			Rectangle(hdc, ptrTempRect->nLeft + nExtX, ptrTempRect->nTop + nExtY, ptrTempRect->nRight + nExtX, ptrTempRect->nBottom+ nExtY );
		}
	}

	DeleteObject( SelectObject(hdc, old_pen) );
	DeleteObject( current_pen );
}

/* ==========================================================================
차량 번호판 LINE 출력 
============================================================================*/

void CWizardHelper::DrawCharRectLine( HDC hdc, int nCharCnt, CVRECT_t * ptrPlateRect ,  LPR_CHAR_t * ptrLPRChar ,int bDraw,  int nExtX, int nExtY)
{
	HPEN current_pen, old_pen;
	int i, nLeft, nRight, nTop, nBottom;
	CVRECT_t *  ptrCharRect;
	if ( bDraw == 0 )
		return ;

	current_pen=CreatePen(PS_SOLID, 1, RGB(210, 105, 30) );
	old_pen=(HPEN)SelectObject(hdc, current_pen);
	SelectObject(hdc, GetStockObject(NULL_BRUSH) );

	for( i = 0 ; i < nCharCnt ; i++ )
	{	
		ptrCharRect = (pCVRECT)&ptrLPRChar->Chars[ i ].ChRect;

		if ( ptrCharRect != NULL )
		{
			nLeft = ptrPlateRect->nLeft + ptrCharRect->nLeft + nExtX;
			nRight = ptrPlateRect->nLeft + ptrCharRect->nRight + nExtX;
			nTop = ptrPlateRect->nTop + ptrCharRect->nTop + nExtY;
			nBottom = ptrPlateRect->nTop +ptrCharRect->nBottom+ nExtY;

			Rectangle(hdc, nLeft, nTop, nRight, nBottom );
		}
	}

	DeleteObject( SelectObject(hdc, old_pen) );
	DeleteObject( current_pen );
}

/* ==========================================================================
기본 Grayscale Pallet 생성 
============================================================================*/
BOOL CWizardHelper::MakeDefaultPallet( IMAGE_DATA_t * ptrImageData )
{
	int i, nColors=0;

	if ( ptrImageData == NULL )
		return FALSE;

	if ( ptrImageData->nBitCount == 24 )
		return FALSE;

	nColors = GetBmpPaletteNums( ptrImageData->nBitCount );

	ptrImageData->ptrPalette = new RGBQUAD[ nColors  ];
	if ( ptrImageData->ptrPalette == NULL )
		return FALSE;

	memset( ptrImageData->ptrPalette, 0, sizeof( RGBQUAD )*nColors);
	for(i = 0 ; i < nColors ; i++) 
	{
		ptrImageData->ptrPalette[i].rgbBlue = BYTE(i);
		ptrImageData->ptrPalette[i].rgbGreen = BYTE(i);
		ptrImageData->ptrPalette[i].rgbRed = BYTE(i);
		ptrImageData->ptrPalette[i].rgbReserved = 0x00;
	}
	return TRUE;
}