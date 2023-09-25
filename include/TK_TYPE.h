#ifndef __TK_TYPE_H__
#define __TK_TYPE_H__


/* ================================================
LINUX ����
===================================================*/
//#define PLATFORM_LINUX	



#define LIMIT_VAL( Value )		( ( Value > 255 ) ? 255 : (( Value < 0 ) ? 0 : Value))
#define CALLAGREEMENT __stdcall

typedef void ( *fnNotifyCallBack )(int , double );


#define LPR_SIDE							1	/* �� ���� */
#define LPR_ZONE							2	/* ���� ���� */
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
�����ڵ� 
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
��輱 ���� ����
===================================================*/
#define OUTER_NONE							0
#define OUTER_SOBEL							1
#define OUTER_ROBERTS						2
#define OUTER_PREWITT						3

/* ================================================
Debug ����
===================================================*/
#define DBG_NONE							1
#define DBG_INFO							2
#define DBG_ERR								3
#define DBG_LINE							4
#define DBG_SEPAR							"================================================================="
/* ================================================
�׵θ� ���� �ܰ�
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
���� �и��� ���� �� 
===================================================*/
#define MAX_LPR_CHAR_CNT					20	/* LPR �ִ� ���� �� */
#define MAX_LPR_AREA_CNT					4	/* LPR �ν� ���� �� */
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
File ����
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
LIB Initialize �Ķ���� ����ü
[ DIP Lib ]
========================================================*/
typedef struct LIB_INIT_DATA_
{
	char szwinpath[ MAX_PATH ];			/* �������� ���� ���*/	
	BYTE Version[ VER_SIZE ];
}LIB_INIT_DATA_t, * pLIB_INIT_DATA;


/*=======================================================
UNSHARP ó�� �Ķ���� ����ü 
[ DIP Lib ]
========================================================*/
typedef struct UNSHARP_PARAM_
{
	int Mask;			/* Unsharp Mask ����  */	
}UNSHARP_PARAM_t, * pUNSHARP_PARAM;

/*=======================================================
�缱�� ����ȭ ��ȯ ó�� �Ķ���� ����ü 
[ DIP Lib ]
========================================================*/
typedef struct XY_
{
	int nX;
	int nY;
}XY_t ;

typedef struct BILINEAR_NOR_PARAM_
{	
	XY_t LT_POINT;		/* ���� ���� ������ */
	XY_t LB_POINT;		/* ���� ���� ������ */
	XY_t RT_POINT;		/* ������ ���� ������ */
	XY_t RB_POINT;		/* ������ ���� ������ */
}BILINEAR_NOR_PARAM_t , * pBILINEAR_NOR_PARAM;

/*=======================================================
ROTATE ��ȯ ó�� �Ķ���� ����ü 
[ DIP Lib ]
========================================================*/
typedef struct ROTATE_PARAM_
{	
	double Angle;	/* ����( 0 ~ 360.0 )*/
}ROTATE_PARAM_t , * pROTATE_PARAM;

/*=======================================================
HOUGH ��ȯ ó�� �Ķ���� ����ü 
[ DIP Lib ]
========================================================*/
typedef struct HOUGH_PARAM_
{
	int bTrans;				/* ���� ���� ���� */
	double Rho;				/* HOUGH �� */
	double Angle;			/* HOUGH ���� �� */
}HOUGH_PARAM_t , * pHOUGH_PARAM;

/*=======================================================
���� ����ȭ ó�� �Ķ���� ����ü
[ DIP Lib ]
========================================================*/
typedef struct THINNING_PARAM_
{
	BYTE ObjColor;				/* ��ü ���� */
}THINNING_PARAM_t , * pTHINNING_PARAM;

/*=======================================================
���� ��� ���� ��û �Ķ���� ����ü
[ DIP Lib ]
========================================================*/
typedef struct LIGHTINFO_PARAM_
{	
	int bThreshold;		/* threshold �� ȹ�� ���� ( �Է� )*/
	int bSig;			/* ǥ�� ���� �� ȹ�� ���� ( �Է� )*/
	int nBright;		/* ���� ���� ȭ�� �� */
	int nDark;			/* ���� ��ο� ȭ�� �� */
	int nSum;
	float fVar;			/* �л� �� */
	float fSig;			/* ǥ�� ���� �� */
	int nMean;			/* ��� ȭ�� �� */
	int nMany;			/* ���� ���� ȭ���� �� */
	int nThreshold;		/* ���� �Ӱ谪 */
}LIGHTINFO_PARAM_t, * pLIGHTINFO_PARAM;

/*=======================================================
�����׵θ� ó�� �Ķ���� ����ü
[ DIP Lib ]
========================================================*/
typedef struct OUTERLINE_PARAM_
{
	BYTE nOuter;	/* ��輱 ���� ��� */
}OUTERLINE_PARAM_t, * pOUTERLINE_PARAM;

/*=======================================================
������׷� ó�� �Ķ���� ����ü
[ DIP Lib ]
========================================================*/
typedef struct HISTOGRAM_PARAM_
{
	int bNormalize;						/* ����ȭ ó�� ���� */
	float fParam[ MAX_GRAY_CNT ];		/* Histogram output(Result) */
}HISTOGRAM_PARAM_t, * pHISTOGRAM_PARAM;

/*=======================================================
����ȭ ó�� �Ķ���� ����ü
[ DIP Lib ]
========================================================*/
typedef struct BINARIZATION_PARAM_
{
	int nMean;				/* ��� */
	int nThreshold;			/* �Ӱ谪 */	
	int bOnlyThreshold;		/* �Ӱ谪 ��û ó�� ���� */
	int nITGSize;			/* integral Adaptive Mask ũ�� */
	int nAdaptCnt;			/* Normal Adaptive �ݺ� Ƚ�� */
	float fAdaptRatio;		/* Normal Adaptive factor ����( 0.1~0.5) */
}BINARIZATION_PARAM_t, * pBINARIZATION_PARAM;

/*=======================================================
���� ħ��, ��â ó�� �Ķ���� ����ü
[ DIP Lib ]
========================================================*/
typedef struct SEGMENT_PARAM_
{
	BYTE SegmentType;		/* ��â, ħ�� ���� */
}SEGMENT_PARAM_t, *pSEGMENT_PARAM;

/*=======================================================
�ܰ��� ���� ó�� �Ķ���� ����ü
[ DIP Lib ]
========================================================*/
typedef struct LINE_EDGE_PARAM_
{
	BYTE LineEdgeType;		/* �ܰ��� ����( ����, ���� ) */
}LINE_EDGE_PARAM_t, *pLINE_EDGE_PARAM;

/*=======================================================
���� contrast ó�� �Ķ���� ����ü
[ DIP Lib ]
========================================================*/
typedef struct CONTRAST_PARAM_
{
	int nRatio;
}CONTRAST_PARAM_t, *pCONTRAST_PARAM;

/*=======================================================
���� ��� ó�� �Ķ���� ����ü
[ DIP Lib ]
========================================================*/
typedef struct BRIGHT_PARAM_
{
	int nBright;
}BRIGHT_PARAM_t, *pBRIGHT_PARAM;

/*=======================================================
����þ� ó�� �Ķ���� ����ü
[ DIP Lib ]
========================================================*/
typedef struct GAUSSIAN_PARAM_
{
	double dSigma;
	double dDoGSigma1;
	double dDoGSigma2;
}GAUSSIAN_PARAM_t, *pGAUSSIAN_PARAM;

/*=======================================================
���� ħ�� ó�� �Ķ���� ����ü
[ DIP Lib ]
========================================================*/
typedef struct DIFFUSION_PARAM_
{
	int nIter;			/* �ݺ� ȸ�� */
}DIFFUSION_PARAM_t, *pDIFFUSION_PARAM;


/*=======================================================
GRASSFIRE ����ü 
8-�̿� Labeling ó�� �� ���
========================================================*/
typedef struct GRASSFIRE_
{
	int nH;
	int nW;
	int nLabel;
	BYTE bFlag;
}GRASSFIRE_t, *pGRASSFIRE;

/*=======================================================
GRASSFIRE HEAP ����ü 
8-�̿� Labeling ó�� �� ���
========================================================*/
typedef struct GRASSFIRE_HEAP_
{
	int nStackPos;
	pGRASSFIRE Heaps;
}GRASSFIRE_HEAP_t;

/*=======================================================
���� ����ü 
========================================================*/
typedef struct CVRECT_
{
	int nLeft;
	int nRight;
	int nTop;
	int nBottom;
	int nSeqNum;				/* ������ �켱 ���� [ LPR Lib ]*/
	int nLPR_RectIdx;			/* RECT_IMAGE_MANAGER_t index [ LPR Lib ]*/ 
	BILINEAR_NOR_PARAM_t BPoint; /* Bilinear Point [ LPR Lib ] */
} CVRECT_t, * pCVRECT;


/*=======================================================
���� ����( ī�޶� ������ )����ü
[ LPR Lib ]
========================================================*/
typedef struct CAMERA_DATA_
{
	int nArea;				/* ī�޶� ���� ���� ��ġ ( [ 0 : Center ] [ 1 : LEFT ] [ 2 : Right ] ) */
	int nBitCount;			/* ���� Bit ( �⺻ 8bit : Grayscale ) */
	int nW;					/* ���� ���� ���� */
	int nH;					/* ���� ���� ���� */
	int nImageSize;			/* ���� ��ü ũ�� */
	BYTE * ptrSrcImage1D;	/* ���� ���� ������( 1���迭 ) */
}CAMERA_DATA_t, *pCAMERA_DATA;

/*=======================================================
���� Matrix ����ü
[ LPR, DIP, OCR Lib ]
========================================================*/
typedef struct IMAGE_DATA_
{
	int nBitCount;			/* ���� Bit ( �⺻ 8bit : Grayscale ) */ 
	int nW;					/* ���� ���� ���� */
	int nH;					/* ���� ���� ���� */
	int nImageSize;			/* ���� ��ü ũ��( ���� 4byte padding ���� ) */
	BYTE ** ptrSrcImage2D;	/* ���� ���� ������( 2���迭 ) */
	RGBQUAD * ptrPalette;	/* ���� Palette ���� */
}IMAGE_DATA_t, *pIMAGE_DATA;

/*=======================================================
���� �̹��� ����ü
========================================================*/
typedef struct RECT_IMAGE_
{	
	int nOrgW;					/* ���� �� ���� ũ�� */
	int nOrgH;					/* ���� �� ���� ũ�� */
	int nOrgBitCnt;
	int nUsed;					/* ��� ���� */
	IMAGE_DATA_t ImageData;		/* �̹��� ���� ���� */
	CVRECT_t ImageRect;
}RECT_IMAGE_t, *pRECTIMAGE;

/*=======================================================
����ȭ ó���� ��ȣ�� ���� ���� ����ü
LPR Lib
========================================================*/
typedef struct RECT_IMAGE_MANAGER_
{
	int UsedIdx;
	RECT_IMAGE_t RectImage[ MAX_PLATE_RECT_IMAGE_CNT ];
}RECT_IMAGE_MANAGER_t , pRECT_IMAGE_MANAGER;

/*=======================================================
����( ���� ��ȣ) ����ü 
========================================================*/
typedef struct CHAR_
{
	CVRECT_t ChRect;			/* ���� ���� */
	IMAGE_DATA_t ImageData;		/* ���� �̹��� */
}CHAR_t , * pCHAR;


/*=======================================================
���� ���� ��ȣ�� ����ü 
========================================================*/
typedef struct LPR_CHAR_
{
	int nCharCnt;						/* ������ ���� */
	int nPlateNumerLen;					/* ������ �ڵ� ���� */
	CHAR_t Chars[ MAX_LPR_CHAR_CNT ];	/* ���� ����ü */
	unsigned short nPlateNumber[ MAX_LPR_CHAR_CNT ]; /* ���� �ڵ�( UNICODE ) */
}LPR_CHAR_t, * pLPRCHAR;

/*=======================================================
LPR ��� ����ü 
========================================================*/
typedef struct LPR_
{
	int nValid;											/* ��ȿ ���� */
	CVRECT_t PlateRect[ MAX_LPR_PLATE_CANDIDATE_CNT ];	/* ��ȣ�� �ĺ��� ���� */
	IMAGE_DATA_t TempImdateData;						/* �ӽ� �̹��� ���� */
	LPR_CHAR_t LPRChars;								/* ���� ���� ��ȣ�� ������ */
}LPR_RESULT_t, *pLPR_RESULT;


/*=======================================================
DIFF ��� ����ü 
========================================================*/
typedef struct DIFF_RESULT_
{
	int nDif;						/* ó���� Dif �� */
	int nExistence;					/* ���� ���� �Ǵ� */
	IMAGE_DATA_t TempImdateData;	/* �ӽ� �̹��� ���� */
}DIFF_RESULT_t, *pDIFF_RESULT;

/*=======================================================
DIFF ��û ����ü 
========================================================*/
typedef struct DIFF_DATA_
{
	pCAMERA_DATA ptrCameraData;						/* ī�޶� ���� ����ü */
	DIFF_RESULT_t LPRDIFResult[ MAX_LPR_AREA_CNT ];	/* DIF ��� ����ü */
}DIFF_DATA_t, * pDIFF_DATA;

/*=======================================================
LPR ��û ����ü 
========================================================*/
typedef struct LPR_DATA_
{
	pCAMERA_DATA ptrCameraData;						/* ī�޶� ���� ����ü */
	LPR_RESULT_t LPRResult[ MAX_LPR_AREA_CNT ];		/* LPR ��� ����ü */
}LPR_DATA_t, * pLPR_DATA;


/*=======================================================
DIP ��û ����ü 
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
OCR ��û ����ü 
========================================================*/
typedef struct OCR_DATA_
{
	UINT16 uOCRMsg;									/* OCR ó�� MSG ID */
	BOOL bValid;									/* ��ȿ ���� */
	pIMAGE_DATA ptrImageDataSrc;					/* OCRó���� ImageData����ü */
	BYTE OCR_CODE;									/* OCR index : �⺻ �Ű�� ��� �� */
	unsigned short OCRData;							/* OCR CODE ( UNICODE ) */
}OCR_DATA_t, * pOCR_DATA;

/*=======================================================
IOCTL ��û ����ü 
========================================================*/
typedef struct IOCTL_DATA_
{
	UINT16 uIOCTLMsg;
	void * ptrInput;
	void * ptrOutput;
}IOCTL_DATA_t, *pIOCTL_DATA;

/*=======================================================
OCR LIB ���� ���� ����ü 
========================================================*/
typedef struct OCR_CONF_
{
	BYTE  OCR_DBG_SHOW_LOG;					/* LOG ó�� ���� */
	BYTE  OCR_NN_TYPE;						/* �Ű�� ����  0: Default EBP , 1 : Ȯ�� EBP */
	BYTE  OCR_RESIZE_THRESHOLD;				/* ���� �̹��� ������ ���� �� ����ȭ �Ӱ谪 */
	int   OCR_LEARNING_CNT;					/* �н� ī���� */
	short OCR_CHAR_H;						/* OCR �� ���� ���� */
	short OCR_CHAR_W;						/* OCR �� ���� ���� */


}OCR_CONF_t, * pOCR_CONF;

/*=======================================================
DIP LIB ���� ���� ����ü 
========================================================*/
typedef struct DIP_CONF_
{
	BYTE DIP_DBG_SHOW_LOG;
}DIP_CONF_t, * pDIP_CONF;

/*=======================================================
APP LIB ���� ���� ����ü 
========================================================*/
typedef struct APP_CONF_
{
	BYTE APP_DBG_SHOW_LOG;
	char IMG_PATH[ MAX_PATH ];	
}APP_CONF_t, * pAPP_CONF;

/*=======================================================
LPR LIB ���� ���� ����ü 
========================================================*/
typedef struct LPR_CONF_
{	
	BYTE LPR_DBG_SHOW_LOG;				/* LOG ó�� ���� */
	BYTE LPR_TYPE;						/* LPR Ÿ�� */
	BYTE LPR_AREACNT;					/* �ν� �� �� */
	
	/* =========================================================================
	��ȣ�� ���� ũ�� 
	============================================================================*/
	short LPR_RESIZE_W;					/* �̹��� ��� ���� */
	short LPR_MIN_PIXEL;				/* �̹��� �ּ� �ȼ� ũ�� */
	short LPR_MIN_CHAR_H;				/* �ּ� ���� ���� */
	short LPR_MIN_CHAR_W;				/* �ּ� ���� ���� */

	short LPR_MIN_BIG4_H;				/* 4�ڸ� ���� �ּ� ���� */
	short LPR_MIN_BIG4_W;				/* 4�ڸ� ���� �ּ� ���� */

	short LPR_UF_H;						/* Uniformity Height */
	short LPR_UF_W;						/* Uniformity Width */
	
	short LPR_MIN_PLT_H;				/* �ּ� ��ȣ�� ���� */
	short LPR_MIN_PLT_W;				/* �ּ� ��ȣ�� ���� */
	
	short LPR_MIN_CHAR_CNT;				/* ��ȣ�� �� �ּ� ���� �� ( ����ȣ�� �� ��� �ϴ� �κи� )*/
	short LPR_MAX_CHAR_CNT;				/* ��ȣ�� �� �ִ� ���� �� */

	short LPR_MAX_HG_MG_W;				/* �ѱ� ��, ���� ���� ���� �ִ� ���� ������ */
	short LPR_MAX_HG_MG_INT;			/* �ѱ� ��, ����, ���� ���� �ִ� ���� ������ */

	float LPR_MIN_PLATE_RATIO;			/* ��ȣ�� �ּ� ���� ( ���� / ���� ) */
	float LPR_MAX_PLATE_RATIO;			/* ��ȣ�� �ִ� ���� ( ���� / ���� ) */	
	float LPR_NEW_PLATE_RATIO;			/* ������ ��ȣ�� ���� */	

	BYTE LPR_L_ANGLE_PIXEL;				/* �� ���� ���� ȭ�� �ȼ��� ���� ũ�� */
	BYTE LPR_R_ANGLE_PIXEL;				/* �� ���� ���� ȭ�� �ȼ��� ���� ũ�� */

	/* =========================================================================
	Main Process 
	============================================================================*/
	BYTE LPR_DBG_RUN_BASIC_STUDY;			/* �⺻ �н� ó�� */
	BYTE LPR_DBG_RUN_RESIZE;				/* �̹��� ��� ���� */
	BYTE LPR_DBG_RUN_FIND_UF;				/* ��ȣ�� ���� ���� */
	BYTE LPR_DBG_RUN_FIND_BIG4;				/* ��ȣ�� ���� ���� */
	BYTE LPR_DBG_RUN_PROJECTION;			/* ��ȣ�� �ĺ� ���� Projection */
	BYTE LPR_DBG_RUN_PRIORITY;				/* �ĺ� ��ȣ�� �켱 ���� ���� */
	BYTE LPR_DBG_RUN_CHOICE_PLATE;			/* �켱 ���� ���� ��ȣ�� ���� */
	BYTE LPR_DBG_RUN_CHAR_SEGMENT;			/* ��ȣ�� ���� ���� */ 	
	BYTE LPR_DBG_RUN_CHAR_RECOGNITION;		/* ���� �ν� ó�� */
	
	/* =========================================================================
	Sub Process 
	============================================================================*/
	BYTE LPR_DBG_RUN_UF_PREDIP;			/* Uniformity �ܰ��� DIP ó�� ���� */	
	BYTE LPR_DBG_RUN_UF_DEL_THICK;			/* Uniformity ������ ���� ���� ���� */
	BYTE LPR_DBG_RUN_UF_LABELING;		/* Uniformity Labeling ���� */
	BYTE LPR_DBG_RUN_UF_MERGE;		/* Uniformity Labeling ���� */
	BYTE LPR_DBG_RUN_UF_EXTENSION;	/* Uniformity Labeling Ȯ�� */
	
	BYTE LPR_DBG_RUN_BIG4_PREDIP;	/* Projection �ܰ��� DIP ó�� ���� */
	BYTE LPR_DBG_RUN_BIG4_DEL_W_LINE;	/* 4�ڸ� ���� ���� ���� ���� ���� */
	BYTE LPR_DBG_RUN_BIG4_DEL_W_BLT;	/* 4�ڸ� ���� ���� ��Ʈ ���� ���� */
	BYTE LPR_DBG_RUN_BIG4_DEL_W_CUVE;	/* 4�ڸ� ���� ���� � ���� ���� */	
	BYTE LPR_DBG_RUN_BIG4_DEL_H_LINE;	/* 4�ڸ� ���� ���� � ���� ���� */
	BYTE LPR_DBG_RUN_BIG4_BILINEAR;			/* ��ȣ�� ���� ���� */

	BYTE LPR_DBG_RUN_PRJ_PREDIP;	/* Projection �ܰ��� DIP ó�� ���� */
	BYTE LPR_DBG_RUN_PRJ_DEL_W_LINE;	/* ���� ���� ���� ���� ���� */
	BYTE LPR_DBG_RUN_PRJ_DEL_W_BLT;	/* ���� ���� ��Ʈ ���� ���� */
	BYTE LPR_DBG_RUN_PRJ_DEL_W_CUVE;	/* ���� ���� � ���� ���� */	
	BYTE LPR_DBG_RUN_PRJ_DEL_H_LINE;	/* ���� ���� ���� ���� ���� */
	BYTE LPR_DBG_RUN_PRJ_DEL_NOISE;	/* ���� ���� ���� ���� ���� */

	BYTE LPR_DBG_RUN_PRJ_DETAIL_RECT; /* ���� �� ���� 1�� �� ���� */	


	BYTE LPR_DBG_RUN_CHAR_SAMPLING;			/* ��ȣ�� ���� ����( �н� ������ ���� ) */ 
	BYTE LPR_DBG_RUN_CHAR_RESIZE;			/* ���� ũ�� ����ȭ */
	BYTE LPR_DBG_RUN_CHAR_THINNING;			/* ����ȭ ���� */

	/* =========================================================================
	Image Debug
	============================================================================*/
	BYTE LPR_DBG_COPY_TRANSFER_IMAGE;		/* ��ȣ�� ���� �̹��� ���� ���� */

	BYTE LPR_DBG_SHOW_UF_DIP;				/* Uniformity DIP ��ó�� �̹��� ���̱� */
	BYTE LPR_DBG_SHOW_UF_LINE;				/* Uniformity Line ���̱� */
	BYTE LPR_DBG_SHOW_UF_VALID_BOX;			/* �ĺ� ��ȣ�� ���� ���� ���̱� */

	BYTE LPR_DBG_SHOW_BIG4_DIP;				/* 4�ڸ� ���� DIP �̹��� ����  */
	BYTE LPR_DBG_SHOW_BIG4_BOX;				/* 4�ڸ� ���� ���� ���� */

	BYTE LPR_DBG_SHOW_PRJ_DIP;		/* ��ȣ�� Projection ����ȭ �̹��� ���̱� */	
	BYTE LPR_DBG_SHOW_PRJ_VALID_BOX;	/* ��ȣ�� Projection ���� ���̱� */
	
	BYTE LPR_DBG_SHOW_LAST_PLATE_BOX;		/* ���� ��ȣ�� ���� ���� ���̱� */
	BYTE LPR_DBG_SHOW_LAST_CHAR_BOX;		/* ���� ���� ���� ���� ���̱� */

	BYTE LPR_DBG_SHOW_CHAR_SEG;		/* ���� ���� ���� ���� ���̱� */
	BYTE LPR_DBG_SHOW_W_LINE_SEG;		/* ���� ���� ���� ���� ���̱� */
	BYTE LPR_DBG_SHOW_H_LINE_SEG;		/* ���� ���� ���� ���� ���̱� */

}LPR_CONF_t, *pLPR_CONF;

#ifndef PLATFORM_LINUX	
#pragma pack(pop)
#endif


#endif