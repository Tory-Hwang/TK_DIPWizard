#ifndef __TK_COMMON_H__
#define __TK_COMMON_H__


typedef struct UNIFORMITY_
{
	int nX;				/* ���̹����� x ���� ��ǥ */
	int nY;				/* ���̹����� y ���� ��ǥ */
	float fU;			/* ���ϼ� */
	float fMean;		/* ��� */
	float fVar;			/* �л� */
	float fSig;			/* ǥ������ */
	int nUniform;		/* ���ϼ� ���� 1: �ұ���, 0 : ���� */
	int nLabelNum;		/* Label Number */
	int nSum;			/* �� */
	int nMax;			/* ���� ���� ��Ⱚ */
	int nCnt;			/* ȭ�� �� */
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

/* ������ ��� ���� ��� */
#if 0
/* =========================================================================================
���� ��ȣ�� ũ�⿡ ���� ������ȣ�� �ν� �Ǵ� �� >>>>> loose�� ����
===========================================================================================*/
#define LINEINTERVAL						2 							/* �ұյ� ���� ������ ���� ������ ���� ����  */

#define MIN_CHAR_Y							(LINEINTERVAL *3)			/* ������ �ּ� ���� ���� ( ����� �̹��� ���� )*/
#define MIN_CHAR_X							2							/* ������ �ּ� ���� ���� ( ����� �̹��� ���� )*/

#define UNIFORMIWIDTH						( MIN_CHAR_X * 7  )			/* �ұյ� ���� ������ ���� ������ ���� ���� */
#define MIN_PLATE_WIDTH						( UNIFORMIWIDTH * 2 )		/* ��ȣ���� �ּ� ���� ���� ( �ּ� ���� 6�� ) */
#define MIN_PLATE_HEIGHT					( MIN_CHAR_Y + 2 )			/* ��ȣ���� �ּ� ���� ���� ( �ּ� ���� ����) */

#define MAX_TRIM_X_SIZE						( MIN_CHAR_X * 5 )			/* ������ ���Ÿ� ���� Trim ���� */

/* =========================================================================================
Detail�� ������ ���� ���� ��ȣ�� �ν� �Ǵ� �� >>>>>>> Detail�� ���� 
===========================================================================================*/
#define CHK_UNIFORMITY_CNT					5							/* ���� �ұյ� üũ ����, Ȧ��  */
#define MIN_VALID_UNIFORMITY_CNT			3							/* �ּ� ���� �ұյ�� �� ( ��ȣ�� ���� ũ���� �ּҴ� UNIFORMIWIDTH�� 3�谡 �Ǿ�� �Ѵ�. )*/

#define MIN_4LABEL_CNT						4							/* �������������� ��ü �ּ� ���� */
#define MIN_8LABEL_CNT						6							/* ū ���������� ��ü �ּ� ���� */
#define MAX_8LABEL_CNT						30							/* ū ���������� ��ü �ִ� ���� */

#define MIN_PRJ_W_CNT						2							/* ��ȿ���� �ʴ� ���� Projection ���� */

/*���ʿ��� ������ �κ� */
#define EXCEPT_LEFT							5							/* ������ ���ܵǴ� ����( ������ �� �κ� ) */
#define EXCEPT_RIGHT						7							/* ������ ���ܵǴ� ������( ������ �� �κ� ) */

#else

/* =========================================================================================
���� ��ȣ�� ũ�⿡ ���� ������ȣ�� �ν� �Ǵ� �� >>>>> loose�� ����
===========================================================================================*/
#define MIN_CHAR_Y							10			/* ������ �ּ� ���� ���� ( ����� �̹��� ���� )*/
#define MIN_CHAR_X							3			/* ������ �ּ� ���� ���� ( ����� �̹��� ���� )*/
#define LINEINTERVAL						4 			/* �ұյ� ���� ������ ���� ������ ���� ����  */
#define UNIFORMIWIDTH						( MIN_CHAR_X * 7  )			/* �ұյ� ���� ������ ���� ������ ���� ���� */


#define MIN_PLATE_WIDTH						( UNIFORMIWIDTH * 2 )		/* ��ȣ���� �ּ� ���� ���� ( �ּ� ���� 6�� ) */
#define MIN_PLATE_HEIGHT					( MIN_CHAR_Y + 2 )			/* ��ȣ���� �ּ� ���� ���� ( �ּ� ���� ����) */

#define MAX_TRIM_X_SIZE						( MIN_CHAR_X * 5 )			/* ������ ���Ÿ� ���� Trim ���� */

/* =========================================================================================
Detail�� ������ ���� ���� ��ȣ�� �ν� �Ǵ� �� >>>>>>> Detail�� ���� 
===========================================================================================*/
#define CHK_UNIFORMITY_CNT					5							/* ���� �ұյ� üũ ����, Ȧ��  */
#define MIN_VALID_UNIFORMITY_CNT			3							/* �ּ� ���� �ұյ�� �� ( ��ȣ�� ���� ũ���� �ּҴ� UNIFORMIWIDTH�� 3�谡 �Ǿ�� �Ѵ�. )*/

#define MIN_4LABEL_CNT						4							/* �������������� ��ü �ּ� ���� */
#define MIN_8LABEL_CNT						6							/* ū ���������� ��ü �ּ� ���� */
#define MAX_8LABEL_CNT						30							/* ū ���������� ��ü �ִ� ���� */

#define MIN_PRJ_W_CNT						2							/* ��ȿ���� �ʴ� ���� Projection ���� */

/*���ʿ��� ������ �κ� */
#define EXCEPT_LEFT							5							/* ������ ���ܵǴ� ����( ������ �� �κ� ) */
#define EXCEPT_RIGHT						7							/* ������ ���ܵǴ� ������( ������ �� �κ� ) */


#endif

/* =========================================================================================
�Ϲ����� ���� ����
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

#define MIN_PLATE_RATIO						1.2							/* ��ȣ�� �ּ� ���� ( ���� / ���� ) */
#define LOOSE_MAX_PLATE_RATIO				20.0						/* ��ȣ�� �ִ� ���� ( ���� / ���� ) */
#define DETAIL_MAX_PLATE_RATIO				12.0						/* ��ȣ�� �ִ� ���� ( ���� / ���� ) */

#define BRIGHT_UNIFORMITY_THRESHOLD			120							/* ���� ������ �����ϴ� �������� �Ǵ��ϴ� �ݺ� ����ȭ �Ӱ谪 */
#define DARK_UNIFORMITY_THRESHOLD			70							/* ������ ���� �������� �Ǵ��ϴ� �ݺ� ����ȭ �Ӱ谪 �� */

#define NORMAL_MID_BRIGHT					124							/* �Ϲ����� ������ �ٴ��� ��� ����*/
#define NORMAL_MAX_BEAM_VAL					170							/* �Ϲ����� �������� ���� ���� ���� �Ǵ��� ���� ��� ���� */

#define BRIGHT_MID_BRIGHT					130							/* ���� (�Ǵ� ��ο� )������ �����ϴ� ������ �ٴ��� ��� ����*/
#define BRIGHT_MAX_BEAM_VAL					160							/* ���� (�Ǵ� ��ο� )������ �����ϴ� ����â�� ���� ���� ���� �Ǵ��� ���� ��� ����*/

/* ==================================================================================================
<< Debug �켱���� >>
DBG_RUN_DEL_THICK_UNIFORMITY > DBG_RUN_GROUPING_LABELING > DBG_RUN_PROJECTION >  DBG_RUN_PRIORITY 
====================================================================================================*/
#define DBG_COPY_TRANSFER_IMAGE				1							/* ��ȣ�� ���� �̹��� ���� ���� */

#define	DBG_RUN_DEL_THICK_UNIFORMITY		0							/* Uniformity ������ ���� ���� */
#define	DBG_RUN_GROUPING_LABELING			0							/* Uniformity Labeling ���� */
#define	DBG_RUN_PROJECTION					0							/* ��ȣ�� �ĺ� ���� Projection( 1�� �ĺ� ���� ) */
#define	DBG_RUN_PRIORITY					0							/* �ĺ� ��ȣ�� �켱 ���� ���� */
#define DBG_RUN_CHOICE_PLATE				0							/* �켱 ���� ���� ��ȣ�� ���� */
#define DBG_RUN_RECOGNITION					0							/* ���� �ν� ó�� */

#define DBG_SHOW_UNIFORMITY_LINE			1							/* Uniformity Line ���̱� */
#define DBG_SHOW_PROJECTION_IMAGE			0							/* ��ȣ�� Projection ����ȭ �̹��� ���̱� */
#define DBG_SHOW_PRIORITY_IMAGE				0							/* ��ȣ�� Priority ����ȭ �̹��� ���̱� */
#define DBG_SHOW_CHAR_IMAGE					0							/* ��ȣ�� ���� ����ȭ �̹��� ���̱� */

#define	DGB_SHOW_VALID_LABEL_BOX			0							/* �ĺ� ��ȣ�� ���� ���� ���̱� */
#define	DGB_SHOW_VALID_PRJECTION_BOX		0							/* ��ȣ�� Projection ���� ���̱� */
#define	DGB_SHOW_VALID_PRIORITY_BOX			1							/* ��ȣ�� Projection ���� ���̱� */


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