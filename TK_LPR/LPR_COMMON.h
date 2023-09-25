#ifndef __TK_COMMON_H__
#define __TK_COMMON_H__


typedef struct UNIFORMITY_
{
	int nX;				/* 원이미지의 x 시작 좌표 */
	int nY;				/* 원이미지의 y 시작 좌표 */
	float fU;			/* 균일성 */
	float fMean;		/* 평균 */
	float fVar;			/* 분산 */
	float fSig;			/* 표준편차 */
	int nUniform;		/* 균일성 여부 1: 불균일, 0 : 균일 */
	int nLabelNum;		/* Label Number */
	int nSum;			/* 합 */
	int nMax;			/* 가장 많은 밝기값 */
	int nCnt;			/* 화소 수 */
}UNIFORMITY_t, *pUNIFORMITY;

typedef struct COMMONINFO_
{
	int nMaxHeight;
	int nMaxWidth;
	int nImageHeight;
	int nImageWidth;	
	int nUniformityH;
	int nUniformityW;
}COMMONINFO_t , * pCOMMONINFO;

/* 사이즈 축소 했을 경우 */
#if 0
/* =========================================================================================
차량 번호판 크기에 의한 차량번호판 인식 판단 시 >>>>> loose한 검증
===========================================================================================*/
#define LINEINTERVAL						2 							/* 불균등 영역 생성을 위한 영상의 세로 간격  */

#define MIN_CHAR_Y							(LINEINTERVAL *3)			/* 문자의 최소 세로 길이 ( 축소한 이미지 기준 )*/
#define MIN_CHAR_X							2							/* 문자의 최소 가로 길이 ( 축소한 이미지 기준 )*/

#define UNIFORMIWIDTH						( MIN_CHAR_X * 7  )			/* 불균등 영역 생성을 위한 영상의 가로 간격 */
#define MIN_PLATE_WIDTH						( UNIFORMIWIDTH * 2 )		/* 번호판의 최소 가로 길이 ( 최소 문자 6개 ) */
#define MIN_PLATE_HEIGHT					( MIN_CHAR_Y + 2 )			/* 번호판의 최소 세로 길이 ( 최소 문자 높이) */

#define MAX_TRIM_X_SIZE						( MIN_CHAR_X * 5 )			/* 노이즈 제거를 위한 Trim 길이 */

/* =========================================================================================
Detail한 정보를 통한 차량 보호판 인식 판단 시 >>>>>>> Detail한 검증 
===========================================================================================*/
#define CHK_UNIFORMITY_CNT					5							/* 수직 불균등 체크 범위, 홀수  */
#define MIN_VALID_UNIFORMITY_CNT			3							/* 최소 수평 불균등영역 수 ( 번호판 가로 크기의 최소는 UNIFORMIWIDTH의 3배가 되어야 한다. )*/

#define MIN_4LABEL_CNT						4							/* 작은영역에서의 객체 최소 개수 */
#define MIN_8LABEL_CNT						6							/* 큰 영역에서의 객체 최소 개수 */
#define MAX_8LABEL_CNT						30							/* 큰 영역에서의 객체 최대 개수 */

#define MIN_PRJ_W_CNT						2							/* 유효하지 않는 수직 Projection 개수 */

/*불필요한 검은색 부분 */
#define EXCEPT_LEFT							5							/* 영상의 제외되는 왼쪽( 검은색 띠 부분 ) */
#define EXCEPT_RIGHT						7							/* 영상의 제외되는 오른쪽( 검은색 띠 부분 ) */

#else

/* =========================================================================================
차량 번호판 크기에 의한 차량번호판 인식 판단 시 >>>>> loose한 검증
===========================================================================================*/
#define MIN_CHAR_Y							10			/* 문자의 최소 세로 길이 ( 축소한 이미지 기준 )*/
#define MIN_CHAR_X							3			/* 문자의 최소 가로 길이 ( 축소한 이미지 기준 )*/
#define LINEINTERVAL						4 			/* 불균등 영역 생성을 위한 영상의 가로 간격  */
#define UNIFORMIWIDTH						( MIN_CHAR_X * 7  )			/* 불균등 영역 생성을 위한 영상의 가로 간격 */


#define MIN_PLATE_WIDTH						( UNIFORMIWIDTH * 2 )		/* 번호판의 최소 가로 길이 ( 최소 문자 6개 ) */
#define MIN_PLATE_HEIGHT					( MIN_CHAR_Y + 2 )			/* 번호판의 최소 세로 길이 ( 최소 문자 높이) */

#define MAX_TRIM_X_SIZE						( MIN_CHAR_X * 5 )			/* 노이즈 제거를 위한 Trim 길이 */

/* =========================================================================================
Detail한 정보를 통한 차량 보호판 인식 판단 시 >>>>>>> Detail한 검증 
===========================================================================================*/
#define CHK_UNIFORMITY_CNT					5							/* 수직 불균등 체크 범위, 홀수  */
#define MIN_VALID_UNIFORMITY_CNT			3							/* 최소 수평 불균등영역 수 ( 번호판 가로 크기의 최소는 UNIFORMIWIDTH의 3배가 되어야 한다. )*/

#define MIN_4LABEL_CNT						4							/* 작은영역에서의 객체 최소 개수 */
#define MIN_8LABEL_CNT						6							/* 큰 영역에서의 객체 최소 개수 */
#define MAX_8LABEL_CNT						30							/* 큰 영역에서의 객체 최대 개수 */

#define MIN_PRJ_W_CNT						2							/* 유효하지 않는 수직 Projection 개수 */

/*불필요한 검은색 부분 */
#define EXCEPT_LEFT							5							/* 영상의 제외되는 왼쪽( 검은색 띠 부분 ) */
#define EXCEPT_RIGHT						7							/* 영상의 제외되는 오른쪽( 검은색 띠 부분 ) */


#endif

/* =========================================================================================
일반적인 정의 내용
===========================================================================================*/
#define RESIZE_IMAGE_W						350

#define LOOSE_PLATE_RECT					1
#define DETAIL_PLATE_RECT					2
#define BIG4_CHAR_RECT						3

#define PLATE_PART_CNT						1
#define PLATE_FULL_CNT						2
#define PRIORITY_LABEL_CNT					3
#define PRIORITY_BK_CNT						4

#define PRIORITY_GAUSSIAN					1.4

#define MIN_PLATE_RATIO						1.2							/* 번호판 최소 비율 ( 가로 / 세로 ) */
#define LOOSE_MAX_PLATE_RATIO				20.0						/* 번호판 최대 비율 ( 가로 / 세로 ) */
#define DETAIL_MAX_PLATE_RATIO				12.0						/* 번호판 최대 비율 ( 가로 / 세로 ) */

#define BRIGHT_UNIFORMITY_THRESHOLD			120							/* 강한 조명이 존재하는 주차장을 판단하는 반복 이진화 임계값 */
#define DARK_UNIFORMITY_THRESHOLD			70							/* 조명이 약한 주차장을 판단하는 반복 이진화 임계값 값 */

#define NORMAL_MID_BRIGHT					124							/* 일반적인 주차장 바닥의 밝기 레벨*/
#define NORMAL_MAX_BEAM_VAL					170							/* 일반적인 주차장의 강한 조명 영역 판단을 위한 밝기 레벨 */

#define BRIGHT_MID_BRIGHT					130							/* 강한 (또는 어두운 )조명이 존재하는 주차장 바닥의 밝기 레벨*/
#define BRIGHT_MAX_BEAM_VAL					160							/* 강한 (또는 어두운 )조명이 존재하는 주자창의 강한 조명 영역 판단을 위한 밝기 레벨*/

/* ==================================================================================================
<< Debug 우선순위 >>
DBG_RUN_DEL_THICK_UNIFORMITY > DBG_RUN_GROUPING_LABELING > DBG_RUN_PROJECTION >  DBG_RUN_PRIORITY 
====================================================================================================*/
#define DBG_COPY_TRANSFER_IMAGE				1							/* 번호판 영역 이미지 복사 실행 */

#define	DBG_RUN_DEL_THICK_UNIFORMITY		0							/* Uniformity 노이즈 제거 실행 */
#define	DBG_RUN_GROUPING_LABELING			0							/* Uniformity Labeling 실행 */
#define	DBG_RUN_PROJECTION					0							/* 번호판 후보 영역 Projection( 1차 후보 선정 ) */
#define	DBG_RUN_PRIORITY					0							/* 후보 번호판 우선 순위 설정 */
#define DBG_RUN_CHOICE_PLATE				0							/* 우선 순위 높은 번호판 선정 */
#define DBG_RUN_RECOGNITION					0							/* 문자 인식 처리 */

#define DBG_SHOW_UNIFORMITY_LINE			1							/* Uniformity Line 보이기 */
#define DBG_SHOW_PROJECTION_IMAGE			0							/* 번호판 Projection 이진화 이미지 보이기 */
#define DBG_SHOW_PRIORITY_IMAGE				0							/* 번호판 Priority 이진화 이미지 보이기 */
#define DBG_SHOW_CHAR_IMAGE					0							/* 번호판 문자 이진화 이미지 보이기 */

#define	DGB_SHOW_VALID_LABEL_BOX			0							/* 후보 번호판 영역 라인 보이기 */
#define	DGB_SHOW_VALID_PRJECTION_BOX		0							/* 번호판 Projection 라인 보이기 */
#define	DGB_SHOW_VALID_PRIORITY_BOX			1							/* 번호판 Projection 라인 보이기 */


int COM_DrawCandidateLine( int nY, int nStartX, int nEndX, pIMAGE_DATA ptrImageDataSrc );
int COM_Initialize( pLPR_CONF ptrLPRConf,  pOCR_CONF ptrOCRConf,pIMAGE_DATA ptrImageDataSrc );
int COM_Release( pLPR_CONF ptrLPRConf,  pOCR_CONF ptrOCRConf );

int COM_DoLabeling4Neighbour( UNIFORMITY_t **ptrUniformitys, int nUniforH, int nUniforW, pIMAGE_DATA ptrImageDataSrc );
int COM_DoLabeling8Neighbour( pIMAGE_DATA ptrImageData, int nWStart, int nWEnd, int nHStart, int nHEnd );
int COM_SubDoLabeling8Neighbour( pIMAGE_DATA ptrImageData, int ** ptrMap, int nWStart, int nWEnd , int nHStart, int nHEnd);

int COM_GetSeparationH_MinCnt( pLPR_CONF ptrLPRConf, pCOMMONINFO ptrComInfo, pCVRECT ptrRect, int * ptrComHProjection, pIMAGE_DATA ptrImageDataSrc  );
int COM_GetSeparationH_MeanCnt( pLPR_CONF ptrLPRConf, pCOMMONINFO ptrComInfo, pCVRECT ptrRect, int * ptrComHProjection,  pIMAGE_DATA ptrImageDataSrc, int bEndPos , int bDelBlank );

pCOMMONINFO COM_GetInfo( void );
IMAGE_DATA_t * COM_GetChgImageBuf( void );
IMAGE_DATA_t * COM_GetTempImageBuf( void );
IMAGE_DATA_t * COM_GetTempImage2Buf( void );
IMAGE_DATA_t * COM_GetCharImageBuf( void );


LPR_CHAR_t * COM_GetLPRCharBuf( void );
UNIFORMITY_t ** COM_GetUniformityBuf( void );

CVRECT_t * COM_GetTempRectBuf( void );
CVRECT_t * COM_GetPlateRectBuf( void );
CVRECT_t * COM_GetCharRectBuf( void );


int * COM_GetHProjectionBuf( void );
int * COM_GetWProjectionBuf( void );
int * COM_GetWProjection2Buf( void );
int ** COM_GetMapBuf( void );
int ** COM_GetMap2Buf( void );

int COM_Init_RectImageBuf( void );
int COM_CopyRectImage( IMAGE_DATA_t * ptrOrgImage );
pRECTIMAGE COM_GetRectImage( int nRectIdx );

int COM_SetArea( int nArea );
int COM_GetArea( void );



#endif