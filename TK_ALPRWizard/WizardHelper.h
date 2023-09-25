#pragma once

#define MAX_FILE_NAME_SIZE		50
#define MAX_FILE_CNT			1000

#include "../include/TK_TYPE.h"

typedef struct FILE_INFO_
{
	TCHAR tcName[ MAX_FILE_NAME_SIZE ];
}FILE_INFO_t;

typedef struct FILE_MANAGER_
{
	int Cnt;
	FILE_INFO_t Files[ MAX_FILE_CNT ];
}FILE_MANAGER_t;

class CWizardHelper
{
public:
	CWizardHelper(void);
	~CWizardHelper(void);

	BOOL GetCurrentPath( TCHAR * ptrtcPath );
	BOOL GetCurrentPathChar( char * ptrPath );

	BOOL GetFileInfo( TCHAR * ptrCurPath,  FILE_MANAGER_t * ptrFileManager);

	BYTE **AllocMatrix(int nH, int nW) ;
	int FreeMatrix(BYTE **Image, int nH, int nW); 
	int FreeImageAllocData( IMAGE_DATA_t * ptrImageData );
	int FreeImageAllocDataWithSize( IMAGE_DATA_t * ptrImageData , int nH, int nW );
	BOOL CopyImageData( IMAGE_DATA_t * ptrDestImageData , IMAGE_DATA_t * ptrSrcImageData ,BOOL bCopyBuf );
	int CopyImageDataWithEmpty( IMAGE_DATA_t * ptrDestImageData , int nBitCount , int nHeight,int nWidth );	

	int ReadBmp(LPCTSTR tcFileName, IMAGE_DATA_t * ptrImageData );
	int ReadRaw(LPCTSTR tcFileName, IMAGE_DATA_t * ptrImageData );
	int ReadJpeg(LPCTSTR tcFileName, IMAGE_DATA_t * ptrImageData );
	int SaveBmp( LPCTSTR tcFileName, IMAGE_DATA_t * ptrImageData );
	int SaveRaw( LPCTSTR tcCurPath,  LPCTSTR tcOrgFileName, IMAGE_DATA_t * ptrImageData );

	void DrawWin32( BOOL bChar,HDC hdc, IMAGE_DATA_t * ptrImageData,  int nExtX = 0, int nExtY = 0  );	

	void DrawRectLine( HDC hdc, CVRECT_t * ptrRect, int bDraw, int nExtX=0, int nExtY = 0);
	void DrawCharRectLine( HDC hdc, int nCharCnt, CVRECT_t * ptrPlateRect , LPR_CHAR_t * ptrLPRChar ,int bDraw,  int nExtX=0, int nExtY = 0);

	BOOL MakeDefaultPallet( IMAGE_DATA_t * ptrImageData );
	
	int GetBmpPaletteNums( int nBitCount );
	int GetCalBmpWidth( int nW, int nBitCount );
private:


	TCHAR m_tcCurrentPath[ MAX_PATH ];
};
