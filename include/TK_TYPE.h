#ifndef __TK_TYPE_H__
#define __TK_TYPE_H__


/* ================================================
LINUX 여부
===================================================*/
//#define PLATFORM_LINUX	



#define LIMIT_VAL( Value )		( ( Value > 255 ) ? 255 : (( Value < 0 ) ? 0 : Value))
#define CALLAGREEMENT __stdcall

typedef void ( *fnNotifyCallBack )(int , double );


#define LPR_SIDE							1	/* 면 단위 */
#define LPR_ZONE							2	/* 구역 단위 */
#define PRINT_DEBUG							1

#define LEFT_AREA							1
#define RIGHT_AREA							2
#define CENTER_AREA							0

#define W_LINE_EDGE							0x01
#define H_LINE_EDGE							0x02
#define HW_LINE_EDGE						0x03

#define SEGMENT_BINARY						0x00
#define SEGMENT_GRAY						0x01

/* ================================================
에러코드 
===================================================*/
#define ERR_SUCCESS							0
#define ERR_NO_PARAM						-1
#define ERR_INITIALIZE						-2
#define ERR_INVALID_DATA					-3
#define ERR_NOT_8BIT						-4
#define ERR_LIB_PROCESS						-5
#define ERR_ALLOC_BUF						-6
#define ERR_NOT_HAVE_RECT					-7
#define ERR_READ_STUDY_MLP					-8
#define ERR_NOT_STUDY_MLP					-9
#define ERR_NOT_MATCH_MLP					-10
#define ERR_OVER_OCR_CODE					-11
#define ERR_OCR_CHAR_SIZE					-12
#define ERR_NO_LPR_CONF						-13
#define ERR_NO_OCR_CONF						-14


/* ================================================
경계선 추출 종류
===================================================*/
#define OUTER_NONE							0
#define OUTER_SOBEL							1
#define OUTER_ROBERTS						2
#define OUTER_PREWITT						3

/* ================================================
Debug 종류
===================================================*/
#define DBG_NONE							1
#define DBG_INFO							2
#define DBG_ERR								3
#define DBG_LINE							4
#define DBG_SEPAR							"================================================================="
/* ================================================
테두리 제거 단계
===================================================*/
#define DEL_LINE							1
#define DEL_BLT								2
#define DEL_L_CUVE							3
#define DEL_R_CUVE							4

#define DEL_TOP								5
#define DEL_BOTTOM							6
#define DEL_LEFT							7
#define DEL_RIGHT							8

#define UNSHARP_5MASK						1
#define UNSHARP_9MASK						2

/* ================================================
차후 분리할 정의 문 
===================================================*/
#define MAX_LPR_CHAR_CNT					20	/* LPR 최대 문자 수 */
#define MAX_LPR_AREA_CNT					4	/* LPR 인식 영역 수 */
#define MAX_LPR_PLATE_CANDIDATE_CNT			5

#define MAX_GRAY_CNT						256
#define MAX_LABEL_CNT						300
#define MAX_PLATE_RECT_IMAGE_CNT			20
#define MAX_CHAR_LABEL_CNT					50


/* ============================================================
DIP TRANSFORM MESSAGE 
==============================================================*/
#define TK_DIP_CONVERTGRAY_MSG					0x1001
#define TK_DIP_HISTOGRAM_MSG					0x1002
#define TK_DIP_HISTOGRAM_EQUAL_MSG				0x1003
#define TK_DIP_BINARIZATION_MSG					0x1004
#define TK_DIP_BINARIZATION_ITER_MSG			0x1005
#define TK_DIP_SOBEL_MSG						0x1006
#define TK_DIP_BRIGHTNESS_MSG					0x1007
#define TK_DIP_BITPLANE_MSG						0x1008
#define TK_DIP_GAUSSIAN_MSG						0x1009
#define TK_DIP_CANNY_MSG						0x100A
#define TK_DIP_LABEL_MSG						0x100B
#define TK_DIP_EROSION_MSG						0x100C
#define TK_DIP_SUBIMAGE_MSG						0x100D
#define TK_DIP_MEDIAN_MSG						0x100E
#define TK_DIP_MEAN_MSG							0x100F

#define TK_DIP_WEIGHTEDMEAN_MSG					0x1010
#define TK_DIP_BEAM_MSG							0x1011
#define TK_DIP_ROBERTS_MSG						0x1012
#define TK_DIP_PREWITT_MSG						0x1013
#define TK_DIP_CONTRAST_MSG						0x1014
#define TK_DIP_OUTLINE_MSG						0x1015
#define TK_DIP_BINARIZATION_DENOISE_MSG			0x1016
#define TK_DIP_BINARIZATION_ADAPTIVE_MSG		0x1017
#define TK_DIP_INVERSE_MSG						0x1018
#define TK_DIP_BINARIZATION_OTSU_MSG			0x1019
#define TK_DIP_DILATION_MSG						0x101A
#define TK_DIP_RESIZE_MSG						0x101B
#define TK_DIP_DIFFUSION_MSG					0x101C
#define TK_DIP_MMSE_MSG							0x101D
#define TK_DIP_LIGHTINFO_MSG					0x101E
#define TK_DIP_BINARIZATION_ITG_ADAPTIVE_MSG	0x101F

#define TK_DIP_ZS_THINNING_MSG					0x1020
#define TK_DIP_HOUGH_LINE_TRANS_MSG				0x1021
#define TK_DIP_ROTATE_MSG						0x1022
#define TK_DIP_DOG_MSG							0x1023
#define TK_DIP_BILINEAR_NOR_MSG					0x1024
#define TK_DIP_UNSHARP_MSG						0x1025
#define TK_DIP_OPENING_MSG						0x1026
#define TK_DIP_CLOSING_MSG						0x1027


#define TK_OCR_SAVE_MSG							0x3001
#define TK_OCR_RECOG_MSG						0x3002
#define TK_OCR_RESTUDY_MSG						0x3003

#define TK_IOCTL_RECONF_MSG						0x4001
#define TK_IOCTL_FEATURECNT_MSG					0x4002
#define TK_IOCTL_NN_FORCE_SAVE_MSG				0x4003

/*=======================================================
Default Value 
========================================================*/
#define DEFAULT_BIT_CNT						8
#define DEFAULT_SHOW_LOG					0

#define DEFAULT_OCR_CHAR_H					40
#define DEFAULT_OCR_CHAR_W					30

#define DEFFAULT_RESIZE_THRESHOLD			70
#define DEFAULT_BIN_THRESOLD				40
#define DEFAULT_BRIGHTNESS					50
#define DEFAULT_BITPLANE					6
#define DEFAULT_ITG_SIZE					20

#define DEFAULT_ADAPT_CNT					10
#define DEFAULT_ADAPT_RATIO					0.1

#define DEFAULT_CONTRAST_RATIO				30
#define DEFAULT_BEAM_TH						160

#define DEFAULT_NN_TYPE						0
#define DEFAULT_LEARNING_CNT				10000

#define VER_SIZE							2

/*=======================================================
File 정보
========================================================*/
#define TK_LPR_CONF_DATA					"TK_LPR_CONF.txt"
#define TK_OCR_CONF_DATA					"TK_OCR_CONF.txt"
#define TK_DIP_CONF_DATA					"TK_DIP_CONF.txt"
#define TK_APP_CONF_DATA					"TK_APP_CONF.txt"

#define TK_OCR_FEATURE_DATA					"TK_OCR_FEATURE.dat"

#define TK_EBP_INPUT_WEIGHT_DATA			"TK_EBP_INWEIGHT.dat"
#define TK_EBP_HIDDEN_WEIGHT_DATA			"TK_EBP_HDWEIGHT.dat"

#define TK_EBP2_WEIGHT_DATA					"TK_EBP2_WEIGHT.dat"
#define TK_EBP2_NODE_DATA					"TK_EBP2_NODE.dat"
#define TK_EBP2_BIAS_DATA					"TK_EBP2_BIAS.dat"

#ifndef PLATFORM_LINUX	
#pragma pack(push, 1)
#endif

typedef struct VERSION_
{
	BYTE APP_VER[ VER_SIZE ];
	BYTE DIP_VER[ VER_SIZE ];
	BYTE LPR_VER[ VER_SIZE ];
	BYTE OCR_VER[ VER_SIZE ];
}VERSION_t, * pVERSION;


/*=======================================================
LIB Initialize 파라미터 구조체
[ DIP Lib ]
========================================================*/
typedef struct LIB_INIT_DATA_
{
	char szwinpath[ MAX_PATH ];			/* 설정파일 저장 경로*/	
	BYTE Version[ VER_SIZE ];
}LIB_INIT_DATA_t, * pLIB_INIT_DATA;


/*=======================================================
UNSHARP 처리 파라미터 구조체 
[ DIP Lib ]
========================================================*/
typedef struct UNSHARP_PARAM_
{
	int Mask;			/* Unsharp Mask 종류  */	
}UNSHARP_PARAM_t, * pUNSHARP_PARAM;

/*=======================================================
양선형 정규화 변환 처리 파라미터 구조체 
[ DIP Lib ]
========================================================*/
typedef struct XY_
{
	int nX;
	int nY;
}XY_t ;

typedef struct BILINEAR_NOR_PARAM_
{	
	XY_t LT_POINT;		/* 왼쪽 상위 교차점 */
	XY_t LB_POINT;		/* 왼쪽 하위 교차점 */
	XY_t RT_POINT;		/* 오른쪽 상위 교차점 */
	XY_t RB_POINT;		/* 오른쪽 하위 교차점 */
}BILINEAR_NOR_PARAM_t , * pBILINEAR_NOR_PARAM;

/*=======================================================
ROTATE 변환 처리 파라미터 구조체 
[ DIP Lib ]
========================================================*/
typedef struct ROTATE_PARAM_
{	
	double Angle;	/* 각도( 0 ~ 360.0 )*/
}ROTATE_PARAM_t , * pROTATE_PARAM;

/*=======================================================
HOUGH 변환 처리 파라미터 구조체 
[ DIP Lib ]
========================================================*/
typedef struct HOUGH_PARAM_
{
	int bTrans;				/* 영상 변경 여부 */
	double Rho;				/* HOUGH ρ값 */
	double Angle;			/* HOUGH 수평 각 */
}HOUGH_PARAM_t , * pHOUGH_PARAM;

/*=======================================================
영상 세션화 처리 파라미터 구조체
[ DIP Lib ]
========================================================*/
typedef struct THINNING_PARAM_
{
	BYTE ObjColor;				/* 객체 색깔 */
}THINNING_PARAM_t , * pTHINNING_PARAM;

/*=======================================================
영상 밝기 정보 요청 파라미터 구조체
[ DIP Lib ]
========================================================*/
typedef struct LIGHTINFO_PARAM_
{	
	int bThreshold;		/* threshold 값 획득 유무 ( 입력 )*/
	int bSig;			/* 표준 편차 값 획득 유무 ( 입력 )*/
	int nBright;		/* 가장 밝은 화소 값 */
	int nDark;			/* 가장 어두운 화소 값 */
	int nSum;
	float fVar;			/* 분산 값 */
	float fSig;			/* 표준 편차 값 */
	int nMean;			/* 평균 화소 값 */
	int nMany;			/* 가장 많은 화소의 값 */
	int nThreshold;		/* 이진 임계값 */
}LIGHTINFO_PARAM_t, * pLIGHTINFO_PARAM;

/*=======================================================
영상테두리 처리 파라미터 구조체
[ DIP Lib ]
========================================================*/
typedef struct OUTERLINE_PARAM_
{
	BYTE nOuter;	/* 경계선 추출 방법 */
}OUTERLINE_PARAM_t, * pOUTERLINE_PARAM;

/*=======================================================
히스토그램 처리 파라미터 구조체
[ DIP Lib ]
========================================================*/
typedef struct HISTOGRAM_PARAM_
{
	int bNormalize;						/* 정규화 처리 여부 */
	float fParam[ MAX_GRAY_CNT ];		/* Histogram output(Result) */
}HISTOGRAM_PARAM_t, * pHISTOGRAM_PARAM;

/*=======================================================
이진화 처리 파라미터 구조체
[ DIP Lib ]
========================================================*/
typedef struct BINARIZATION_PARAM_
{
	int nMean;				/* 평균 */
	int nThreshold;			/* 임계값 */	
	int bOnlyThreshold;		/* 임계값 요청 처리 유무 */
	int nITGSize;			/* integral Adaptive Mask 크기 */
	int nAdaptCnt;			/* Normal Adaptive 반복 횟수 */
	float fAdaptRatio;		/* Normal Adaptive factor 비율( 0.1~0.5) */
}BINARIZATION_PARAM_t, * pBINARIZATION_PARAM;

/*=======================================================
영상 침식, 팽창 처리 파라미터 구조체
[ DIP Lib ]
========================================================*/
typedef struct SEGMENT_PARAM_
{
	BYTE SegmentType;		/* 팽창, 침식 기준 */
}SEGMENT_PARAM_t, *pSEGMENT_PARAM;

/*=======================================================
외각선 검출 처리 파라미터 구조체
[ DIP Lib ]
========================================================*/
typedef struct LINE_EDGE_PARAM_
{
	BYTE LineEdgeType;		/* 외각선 종류( 수직, 수평 ) */
}LINE_EDGE_PARAM_t, *pLINE_EDGE_PARAM;

/*=======================================================
영상 contrast 처리 파라미터 구조체
[ DIP Lib ]
========================================================*/
typedef struct CONTRAST_PARAM_
{
	int nRatio;
}CONTRAST_PARAM_t, *pCONTRAST_PARAM;

/*=======================================================
영상 밝기 처리 파라미터 구조체
[ DIP Lib ]
========================================================*/
typedef struct BRIGHT_PARAM_
{
	int nBright;
}BRIGHT_PARAM_t, *pBRIGHT_PARAM;

/*=======================================================
가우시안 처리 파라미터 구조체
[ DIP Lib ]
========================================================*/
typedef struct GAUSSIAN_PARAM_
{
	double dSigma;
	double dDoGSigma1;
	double dDoGSigma2;
}GAUSSIAN_PARAM_t, *pGAUSSIAN_PARAM;

/*=======================================================
영상 침식 처리 파라미터 구조체
[ DIP Lib ]
========================================================*/
typedef struct DIFFUSION_PARAM_
{
	int nIter;			/* 반복 회수 */
}DIFFUSION_PARAM_t, *pDIFFUSION_PARAM;


/*=======================================================
GRASSFIRE 구조체 
8-이웃 Labeling 처리 시 사용
========================================================*/
typedef struct GRASSFIRE_
{
	int nH;
	int nW;
	int nLabel;
	BYTE bFlag;
}GRASSFIRE_t, *pGRASSFIRE;

/*=======================================================
GRASSFIRE HEAP 구조체 
8-이웃 Labeling 처리 시 사용
========================================================*/
typedef struct GRASSFIRE_HEAP_
{
	int nStackPos;
	pGRASSFIRE Heaps;
}GRASSFIRE_HEAP_t;

/*=======================================================
영역 구조체 
========================================================*/
typedef struct CVRECT_
{
	int nLeft;
	int nRight;
	int nTop;
	int nBottom;
	int nSeqNum;				/* 영역의 우선 순위 [ LPR Lib ]*/
	int nLPR_RectIdx;			/* RECT_IMAGE_MANAGER_t index [ LPR Lib ]*/ 
	BILINEAR_NOR_PARAM_t BPoint; /* Bilinear Point [ LPR Lib ] */
} CVRECT_t, * pCVRECT;


/*=======================================================
영상 원본( 카메라 데이터 )구조체
[ LPR Lib ]
========================================================*/
typedef struct CAMERA_DATA_
{
	int nArea;				/* 카메라 기준 영상 위치 ( [ 0 : Center ] [ 1 : LEFT ] [ 2 : Right ] ) */
	int nBitCount;			/* 영상 Bit ( 기본 8bit : Grayscale ) */
	int nW;					/* 영상 가로 길이 */
	int nH;					/* 영상 세로 길이 */
	int nImageSize;			/* 영상 전체 크기 */
	BYTE * ptrSrcImage1D;	/* 영상 원본 데이터( 1차배열 ) */
}CAMERA_DATA_t, *pCAMERA_DATA;

/*=======================================================
영상 Matrix 구조체
[ LPR, DIP, OCR Lib ]
========================================================*/
typedef struct IMAGE_DATA_
{
	int nBitCount;			/* 영상 Bit ( 기본 8bit : Grayscale ) */ 
	int nW;					/* 영상 가로 길이 */
	int nH;					/* 영상 세로 길이 */
	int nImageSize;			/* 영상 전체 크기( 가로 4byte padding 포함 ) */
	BYTE ** ptrSrcImage2D;	/* 영상 원본 데이터( 2차배열 ) */
	RGBQUAD * ptrPalette;	/* 영상 Palette 정보 */
}IMAGE_DATA_t, *pIMAGE_DATA;

/*=======================================================
영역 이미지 구조체
========================================================*/
typedef struct RECT_IMAGE_
{	
	int nOrgW;					/* 생성 시 가로 크기 */
	int nOrgH;					/* 생성 시 세로 크기 */
	int nOrgBitCnt;
	int nUsed;					/* 사용 유무 */
	IMAGE_DATA_t ImageData;		/* 이미지 저장 버퍼 */
	CVRECT_t ImageRect;
}RECT_IMAGE_t, *pRECTIMAGE;

/*=======================================================
이진화 처리된 번호판 영역 정보 구조체
LPR Lib
========================================================*/
typedef struct RECT_IMAGE_MANAGER_
{
	int UsedIdx;
	RECT_IMAGE_t RectImage[ MAX_PLATE_RECT_IMAGE_CNT ];
}RECT_IMAGE_MANAGER_t , pRECT_IMAGE_MANAGER;

/*=======================================================
문자( 차량 번호) 구조체 
========================================================*/
typedef struct CHAR_
{
	CVRECT_t ChRect;			/* 문자 영역 */
	IMAGE_DATA_t ImageData;		/* 문자 이미지 */
}CHAR_t , * pCHAR;


/*=======================================================
최종 차량 번호판 구조체 
========================================================*/
typedef struct LPR_CHAR_
{
	int nCharCnt;						/* 문자의 개수 */
	int nPlateNumerLen;					/* 문자의 코드 개수 */
	CHAR_t Chars[ MAX_LPR_CHAR_CNT ];	/* 문자 구조체 */
	unsigned short nPlateNumber[ MAX_LPR_CHAR_CNT ]; /* 문자 코드( UNICODE ) */
}LPR_CHAR_t, * pLPRCHAR;

/*=======================================================
LPR 결과 구조체 
========================================================*/
typedef struct LPR_
{
	int nValid;											/* 유효 유무 */
	CVRECT_t PlateRect[ MAX_LPR_PLATE_CANDIDATE_CNT ];	/* 번호판 후보지 영역 */
	IMAGE_DATA_t TempImdateData;						/* 임시 이미지 정보 */
	LPR_CHAR_t LPRChars;								/* 최종 차량 번호판 구조제 */
}LPR_RESULT_t, *pLPR_RESULT;


/*=======================================================
DIFF 결과 구조체 
========================================================*/
typedef struct DIFF_RESULT_
{
	int nDif;						/* 처리된 Dif 값 */
	int nExistence;					/* 차량 유무 판단 */
	IMAGE_DATA_t TempImdateData;	/* 임시 이미지 정보 */
}DIFF_RESULT_t, *pDIFF_RESULT;

/*=======================================================
DIFF 요청 구조체 
========================================================*/
typedef struct DIFF_DATA_
{
	pCAMERA_DATA ptrCameraData;						/* 카메라 원본 구조체 */
	DIFF_RESULT_t LPRDIFResult[ MAX_LPR_AREA_CNT ];	/* DIF 결과 구조체 */
}DIFF_DATA_t, * pDIFF_DATA;

/*=======================================================
LPR 요청 구조체 
========================================================*/
typedef struct LPR_DATA_
{
	pCAMERA_DATA ptrCameraData;						/* 카메라 원본 구조체 */
	LPR_RESULT_t LPRResult[ MAX_LPR_AREA_CNT ];		/* LPR 결과 구조체 */
}LPR_DATA_t, * pLPR_DATA;


/*=======================================================
DIP 요청 구조체 
========================================================*/
typedef struct DIP_DATA_
{
	UINT16 uDIPMsg;
	pIMAGE_DATA ptrImageDataSrc1;
	pIMAGE_DATA ptrImageDataSrc2;
	pIMAGE_DATA ptrImageDataDest;
	void * ptrInputParam;
}DIP_DATA_t, *pDIP_DATA;


/*=======================================================
OCR 요청 구조체 
========================================================*/
typedef struct OCR_DATA_
{
	UINT16 uOCRMsg;									/* OCR 처리 MSG ID */
	BOOL bValid;									/* 유효 여부 */
	pIMAGE_DATA ptrImageDataSrc;					/* OCR처리용 ImageData구조체 */
	BYTE OCR_CODE;									/* OCR index : 기본 신경망 사용 시 */
	unsigned short OCRData;							/* OCR CODE ( UNICODE ) */
}OCR_DATA_t, * pOCR_DATA;

/*=======================================================
IOCTL 요청 구조체 
========================================================*/
typedef struct IOCTL_DATA_
{
	UINT16 uIOCTLMsg;
	void * ptrInput;
	void * ptrOutput;
}IOCTL_DATA_t, *pIOCTL_DATA;

/*=======================================================
OCR LIB 설정 정보 구조체 
========================================================*/
typedef struct OCR_CONF_
{
	BYTE  OCR_DBG_SHOW_LOG;					/* LOG 처리 여부 */
	BYTE  OCR_NN_TYPE;						/* 신경망 종류  0: Default EBP , 1 : 확장 EBP */
	BYTE  OCR_RESIZE_THRESHOLD;				/* 이진 이미지 사이즈 변경 후 이진화 임계값 */
	int   OCR_LEARNING_CNT;					/* 학습 카운터 */
	short OCR_CHAR_H;						/* OCR 용 문자 높이 */
	short OCR_CHAR_W;						/* OCR 용 문자 넓이 */


}OCR_CONF_t, * pOCR_CONF;

/*=======================================================
DIP LIB 설정 정보 구조체 
========================================================*/
typedef struct DIP_CONF_
{
	BYTE DIP_DBG_SHOW_LOG;
}DIP_CONF_t, * pDIP_CONF;

/*=======================================================
APP LIB 설정 정보 구조체 
========================================================*/
typedef struct APP_CONF_
{
	BYTE APP_DBG_SHOW_LOG;
	char IMG_PATH[ MAX_PATH ];	
}APP_CONF_t, * pAPP_CONF;

/*=======================================================
LPR LIB 설정 정보 구조체 
========================================================*/
typedef struct LPR_CONF_
{	
	BYTE LPR_DBG_SHOW_LOG;				/* LOG 처리 여부 */
	BYTE LPR_TYPE;						/* LPR 타입 */
	BYTE LPR_AREACNT;					/* 인식 면 수 */
	
	/* =========================================================================
	번호판 기준 크기 
	============================================================================*/
	short LPR_RESIZE_W;					/* 이미지 축소 넓이 */
	short LPR_MIN_PIXEL;				/* 이미지 최소 픽세 크기 */
	short LPR_MIN_CHAR_H;				/* 최소 문자 높이 */
	short LPR_MIN_CHAR_W;				/* 최소 문자 넓이 */

	short LPR_MIN_BIG4_H;				/* 4자리 숫자 최소 높이 */
	short LPR_MIN_BIG4_W;				/* 4자리 숫자 최소 넓이 */

	short LPR_UF_H;						/* Uniformity Height */
	short LPR_UF_W;						/* Uniformity Width */
	
	short LPR_MIN_PLT_H;				/* 최소 번호판 높이 */
	short LPR_MIN_PLT_W;				/* 최소 번호판 넓이 */
	
	short LPR_MIN_CHAR_CNT;				/* 번호판 내 최소 문자 수 ( 구번호판 일 경우 하단 부분만 )*/
	short LPR_MAX_CHAR_CNT;				/* 번호판 내 최대 문자 수 */

	short LPR_MAX_HG_MG_W;				/* 한글 자, 모음 병합 가능 최대 가로 사이즈 */
	short LPR_MAX_HG_MG_INT;			/* 한글 ㅈ, 모음, 병밥 가능 최대 간격 사이즈 */

	float LPR_MIN_PLATE_RATIO;			/* 번호판 최소 비율 ( 가로 / 세로 ) */
	float LPR_MAX_PLATE_RATIO;			/* 번호판 최대 비율 ( 가로 / 세로 ) */	
	float LPR_NEW_PLATE_RATIO;			/* 유럽형 번호판 비율 */	

	BYTE LPR_L_ANGLE_PIXEL;				/* 좌 영상에 따른 화각 픽셀의 보정 크기 */
	BYTE LPR_R_ANGLE_PIXEL;				/* 우 영상에 따른 화각 픽셀의 보정 크기 */

	/* =========================================================================
	Main Process 
	============================================================================*/
	BYTE LPR_DBG_RUN_BASIC_STUDY;			/* 기본 학습 처리 */
	BYTE LPR_DBG_RUN_RESIZE;				/* 이미지 축소 여부 */
	BYTE LPR_DBG_RUN_FIND_UF;				/* 번호판 각도 보정 */
	BYTE LPR_DBG_RUN_FIND_BIG4;				/* 번호판 각도 보정 */
	BYTE LPR_DBG_RUN_PROJECTION;			/* 번호판 후보 영역 Projection */
	BYTE LPR_DBG_RUN_PRIORITY;				/* 후보 번호판 우선 순위 설정 */
	BYTE LPR_DBG_RUN_CHOICE_PLATE;			/* 우선 순위 높은 번호판 선정 */
	BYTE LPR_DBG_RUN_CHAR_SEGMENT;			/* 번호판 문자 분할 */ 	
	BYTE LPR_DBG_RUN_CHAR_RECOGNITION;		/* 문자 인식 처리 */
	
	/* =========================================================================
	Sub Process 
	============================================================================*/
	BYTE LPR_DBG_RUN_UF_PREDIP;			/* Uniformity 단계의 DIP 처리 여부 */	
	BYTE LPR_DBG_RUN_UF_DEL_THICK;			/* Uniformity 고립영역 제거 제거 실행 */
	BYTE LPR_DBG_RUN_UF_LABELING;		/* Uniformity Labeling 실행 */
	BYTE LPR_DBG_RUN_UF_MERGE;		/* Uniformity Labeling 통합 */
	BYTE LPR_DBG_RUN_UF_EXTENSION;	/* Uniformity Labeling 확장 */
	
	BYTE LPR_DBG_RUN_BIG4_PREDIP;	/* Projection 단계의 DIP 처리 여부 */
	BYTE LPR_DBG_RUN_BIG4_DEL_W_LINE;	/* 4자리 숫자 가로 직선 제거 유무 */
	BYTE LPR_DBG_RUN_BIG4_DEL_W_BLT;	/* 4자리 숫자 가로 볼트 제거 유무 */
	BYTE LPR_DBG_RUN_BIG4_DEL_W_CUVE;	/* 4자리 숫자 가로 곡선 제거 유무 */	
	BYTE LPR_DBG_RUN_BIG4_DEL_H_LINE;	/* 4자리 숫자 가로 곡선 제거 유무 */
	BYTE LPR_DBG_RUN_BIG4_BILINEAR;			/* 번호판 각도 보정 */

	BYTE LPR_DBG_RUN_PRJ_PREDIP;	/* Projection 단계의 DIP 처리 여부 */
	BYTE LPR_DBG_RUN_PRJ_DEL_W_LINE;	/* 투영 가로 직선 제거 유무 */
	BYTE LPR_DBG_RUN_PRJ_DEL_W_BLT;	/* 투영 가로 볼트 제거 유무 */
	BYTE LPR_DBG_RUN_PRJ_DEL_W_CUVE;	/* 투영 가로 곡선 제거 유무 */	
	BYTE LPR_DBG_RUN_PRJ_DEL_H_LINE;	/* 투영 세로 직선 제거 유무 */
	BYTE LPR_DBG_RUN_PRJ_DEL_NOISE;	/* 투영 세로 직선 제거 유무 */

	BYTE LPR_DBG_RUN_PRJ_DETAIL_RECT; /* 투영 후 영역 1차 재 설정 */	


	BYTE LPR_DBG_RUN_CHAR_SAMPLING;			/* 번호판 문자 추출( 학습 데이터 추출 ) */ 
	BYTE LPR_DBG_RUN_CHAR_RESIZE;			/* 문자 크기 정규화 */
	BYTE LPR_DBG_RUN_CHAR_THINNING;			/* 세션화 유무 */

	/* =========================================================================
	Image Debug
	============================================================================*/
	BYTE LPR_DBG_COPY_TRANSFER_IMAGE;		/* 번호판 영역 이미지 복사 실행 */

	BYTE LPR_DBG_SHOW_UF_DIP;				/* Uniformity DIP 선처리 이미지 보이기 */
	BYTE LPR_DBG_SHOW_UF_LINE;				/* Uniformity Line 보이기 */
	BYTE LPR_DBG_SHOW_UF_VALID_BOX;			/* 후보 번호판 영역 라인 보이기 */

	BYTE LPR_DBG_SHOW_BIG4_DIP;				/* 4자리 숫자 DIP 이미지 보기  */
	BYTE LPR_DBG_SHOW_BIG4_BOX;				/* 4자리 숫자 영역 보기 */

	BYTE LPR_DBG_SHOW_PRJ_DIP;		/* 번호판 Projection 이진화 이미지 보이기 */	
	BYTE LPR_DBG_SHOW_PRJ_VALID_BOX;	/* 번호판 Projection 라인 보이기 */
	
	BYTE LPR_DBG_SHOW_LAST_PLATE_BOX;		/* 최종 번호판 영역 라인 보이기 */
	BYTE LPR_DBG_SHOW_LAST_CHAR_BOX;		/* 최종 문자 영역 라인 보이기 */

	BYTE LPR_DBG_SHOW_CHAR_SEG;		/* 최종 문자 영역 라인 보이기 */
	BYTE LPR_DBG_SHOW_W_LINE_SEG;		/* 최종 문자 영역 라인 보이기 */
	BYTE LPR_DBG_SHOW_H_LINE_SEG;		/* 최종 문자 영역 라인 보이기 */

}LPR_CONF_t, *pLPR_CONF;

#ifndef PLATFORM_LINUX	
#pragma pack(pop)
#endif


#endif