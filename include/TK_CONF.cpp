#include "stdafx.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/TK_CONF.h"

#define CONF_BUF_SIZE	500

#ifndef PLATFORM_LINUX

#pragma warning(disable: 4996)
#pragma pack(push, 1)

typedef struct element_t
{
	tListHead list;
	char *key;
	char *value;
}element_t;

typedef struct session_element_t
{
	tListHead list;
	tListHead elem_list;
	char *key;
}session_element_t;

typedef struct conf_t
{
	char *filename;
	char *delim;
	tListHead elements;
}conf_t;

#pragma pack(pop)

#else
typedef struct element_t
{
	tListHead list;
	char *key;
	char *value;
}__attribute__((packed)) element_t;

typedef struct session_element_t
{
	tListHead list;
	tListHead elem_list;
	char *key;
}__attribute__((packed)) session_element_t;

typedef struct conf_t
{
	char *filename;
	char *delim;
	tListHead elements;
}__attribute__((packed)) conf_t;


#endif

#define MAXBUF_SIZ				1024


static LPR_CONF_t g_lpr_conf;
static OCR_CONF_t g_ocr_conf;
static DIP_CONF_t g_dip_conf;
static APP_CONF_t g_app_conf;

static char g_wincurpath[ MAX_PATH ];

static void ltrim( char *str );
static void rtrim( char *str );
static void trim( char *str );

static conf_t * init_conf( char * filename, char * delim );
static conf_t * load_conf( char * filename, char * delim );
static int save_and_release_conf( conf_t * conf );
static void release_conf( conf_t * conf );
static void release_element( element_t * e );
static int commit_conf( conf_t * conf );
//static void trace_conf( conf_t * conf );
//static int find_session( conf_t *conf, char * name );

//static char * conf_add_value( conf_t * conf, char * key, char * value );
//static char * conf_get_value( conf_t * conf, char * key );
//static char * conf_set_value( conf_t * conf, char * key , char * value );

static char * conf_session_add_value( conf_t * conf, char * name, char * key, char * value );
static char * conf_session_get_value( conf_t * conf, char * name, char * key );
static char * conf_session_set_value( conf_t * conf, char * name, char * key , char * value );

static int save_session_conf_with_buffer( char *buffer, session_element_t *session, char *delim );
static int save_buffer( FILE *fp, char *buffer );
static void release_session_element( session_element_t *e );

static session_element_t *internal_find_session( conf_t *conf, const char *name, int create );
static char *internal_conf_add_value( session_element_t *session, const char *key, const char *value );
//static void trace_session_elem( session_element_t *session, char *delim );
static char *internal_conf_set_value( session_element_t *session, const char *key, const char *value, int *err );
static char *internal_conf_get_value( session_element_t *session, const char *key );

static void ltrim( char *str )
{
	char *s = str;

	if ( strlen( str ) == 0 ) return;

	while( isspace( *s ))
		s++;
	while(( *str++ = *s++ ));
}

static void rtrim( char *str )
{
	char * s = str;
	register int i;

	if ( strlen( str ) == 0 ) return;

	for( i = (int)strlen( s ) -1; isspace( *( s+i ) ); i-- )
		*( s+i ) = '\0';
}

static void trim( char *str )
{
	rtrim( str );
	ltrim( str );
}

#ifndef PLATFORM_LINUX 

static int strcasecmp(const char *s1, const char *s2)
{
	const u_char *us1 = (const u_char *)s1, *us2 = (const u_char *)s2;

	while (tolower(*us1) == tolower(*us2)) {
		if (*us1++ == '\0')
			return (0);
		us2++;
	}
	return (tolower(*us1) - tolower(*us2));
}


static char * strsep( char **stringp, char *delim) 
{
	register char *s;
	register const char *spanp;
	register int c, sc;
	char *tok;

	if ((s = *stringp) == NULL)
		return (NULL);
	for (tok = s;;) {
		c = *s++;
		spanp = delim;
		do {
			if ((sc = *spanp++) == c) {
				if (c == 0)
					s = NULL;
				else
					s[-1] = 0;
				*stringp = s;
				return (tok);
			}
		} while (sc != 0);
	}
}
#endif



static int save_session_conf_with_buffer( char *buffer, session_element_t *session, char *delim )
{
	element_t *e;
	char file_tmp[ 1024 ];

	if( session->key != NULL && session->key[ 0 ] != '\x00' )
	{
		memset( file_tmp, 0, sizeof( file_tmp ) );
		sprintf( file_tmp, "[%s]\n", session->key );
		strcat( buffer, file_tmp ); 
	}
	while( !UT_ListIsEmpty( &session->elem_list ) )
	{
		e = UT_ListGetObj( session->elem_list.Next, element_t, list );
		if( e->key && e->value )
		{
			memset( file_tmp, 0, sizeof( file_tmp ) );
			sprintf( file_tmp, "%s%s%s\n",  e->key, delim, e->value );
			strcat( buffer, file_tmp ); 
			free( e->key );
			free( e->value );
		}

		UT_ListDel( &e->list );

		free( e );
	}
	return 0;
}

static int save_buffer( FILE *fp, char *buffer )
{
	int len;
	int result;
	if( fp == NULL )
		return -1;

	len = (int)strlen( buffer );
	if( len == 0 )
		return -1;
	result = fprintf( fp, "%s", buffer );
	return 0;
}

#if 0
static void trace_session_elem( session_element_t *session, char *delim )
{
	tListHead *p;
	element_t *e;

	if( session->key != NULL && session->key[ 0 ] != '\x00' )
	{
		printf( "[%s]\n", session->key );
	}

	UT_ListForEach( p, &session->elem_list )
	{
		e = UT_ListGetObj( p, element_t, list );
		printf( "%s%s%s\n", e->key, delim, e->value );
	}
}
#endif


static session_element_t *internal_find_session( conf_t *conf, const char *name, int create )
{
	tListHead *p;
	session_element_t *e;

	if( !conf )
		return NULL;

	UT_ListForEach( p, &conf->elements )
	{
		e = UT_ListGetObj( p, session_element_t, list );
		if( name != NULL && name[ 0 ] != '\x00' )
		{
			if( strcmp( e->key, name ) == 0 )
				return e;
		}
		else
		{
			if( e->key == NULL )
				return e;
		}
	}
	if( create != 0 )
	{
		e = ( session_element_t * )calloc( 1, sizeof( session_element_t ) );
		if( !e )
			return NULL;

		e->key = NULL;
		if( name != NULL && name[ 0 ] != '\x00' )
		{
			e->key = (char *)calloc( strlen( name ) + 1, sizeof( char ) );
			strcpy( e->key, name );
		}
		INIT_LIST_HEAD( &e->list );
		INIT_LIST_HEAD( &e->elem_list );
		UT_ListAddTail( &e->list, &conf->elements );
		return e;
	}
	return NULL;
}


static char *internal_conf_add_value( session_element_t *session, const char *key, const char *value )
{
	element_t *e;

	if( !session )
		return NULL;

	if( !key ||  !value || !strlen( key ) || !strlen( value ) )
		return NULL;

	e = (element_t *)calloc( 1, sizeof( element_t ) );
	if( !e )
		return NULL;

	e->key = (char *)calloc( strlen( key ) + 1, sizeof( char ) );
	if( !e->key )
	{
		release_element( e );
		return NULL;
	}
	strcpy( e->key, key );

	e->value = (char *)calloc( strlen( value ) + 1, sizeof( char ) );
	if( !e->value )
	{
		release_element( e );
		return NULL;
	}
	strcpy( e->value, value );

	UT_ListAddTail( &e->list, &session->elem_list );
	// UT_ListAddTail( &e->list, &conf->elements );

	return e->value;
}


static char *internal_conf_get_value( session_element_t *session, const char *key )
{
	tListHead *p;
	element_t *e;

	UT_ListForEach(p, &session->elem_list)
	{
		e = UT_ListGetObj( p, element_t, list );
		if( !strcasecmp( key, e->key ) )
			return e->value;
	}
	return NULL;
}

static char *internal_conf_set_value( session_element_t *session, const char *key, const char *value, int *err )
{
	tListHead *p;
	element_t *e;
	char *old_value;

	*err = 0;
	UT_ListForEach(p, &session->elem_list)
	{
		e = UT_ListGetObj( p, element_t, list );
		if( !strcasecmp( key, e->key ) )
		{
			if( strlen( e->value ) < strlen( value ) )
			{
				old_value = e->value;
				e->value = (char *)calloc( strlen( value ) + 1, sizeof( char ) );
				if( !e->value )
				{
					*err = -1;
					e->value  = old_value;
					return NULL;
				}
				free( old_value );
			}
			strcpy( e->value, value );
			return e->value;
		}
	}
	*err = -2;
	return NULL;
}
static void release_session_element( session_element_t *e )
{
	element_t *elem;

	while( !UT_ListIsEmpty( &e->elem_list ) )
	{
		elem = UT_ListGetObj( e->elem_list.Next, element_t, list );

		UT_ListDel( &elem->list );
		release_element( elem );
	}
	if( e->key != NULL )
		free( e->key );
	free( e );
	e = NULL;
}

static conf_t * init_conf( char * filename, char * delim )
{
	conf_t *conf;
	conf = (conf_t *)calloc( 1, sizeof( conf_t ));
	if( !conf )
		return NULL;

	conf->filename = (char *)calloc( strlen( filename ) + 1, sizeof( char ));
	if( !conf->filename )
	{
		release_conf( conf );
		return NULL;
	}
	strcpy( conf->filename, filename );

	conf->delim = (char *)calloc( strlen( delim ) + 1, sizeof( char ));
	if( !conf->delim )
	{
		release_conf( conf );
		return NULL;
	}
	strcpy( conf->delim, delim );

	INIT_LIST_HEAD( &conf->elements );

	return conf;
}

static conf_t * load_conf( char * filename, char * delim )
{
	FILE *fp = NULL;
	char *tok = NULL;
	char *ptr = NULL;
	char *session = NULL;

	char name[MAXBUF_SIZ];
	char key[MAXBUF_SIZ];
	char val[MAXBUF_SIZ];
	char read_buf[ MAXBUF_SIZ ];
	char szpath[ MAX_PATH ];

	conf_t *conf = NULL;
	element_t *e = NULL;

	conf = init_conf( filename, delim );

	if( !conf )
		return NULL;

	memset( szpath, 0, sizeof( szpath));
	GetWinCurPath( szpath, filename );	
	fp = fopen( szpath, "r" );

	if( !fp )
		return conf;

	memset( read_buf, 0, sizeof( read_buf ));

	session = NULL;

	while( fgets( read_buf, sizeof( read_buf ) - 1, fp ) )
	{
		e = NULL;
		memset( key, 0, sizeof( key ) );
		memset( val, 0, sizeof( val ) );

		trim( read_buf );
		if( !strlen( read_buf ) || read_buf[0] == '#' )
			continue;

		ptr =  read_buf;
		tok = strchr( ptr, '#' );
		if( tok )
			*tok = '\0';

		tok = (char *)strsep( &ptr, delim );
		strcpy( key, tok );
		if( ptr )
		{
			tok = (char *)strsep( &ptr, delim );
			strcpy( val, tok );
		}
		else
		{
			int len;

			len = (int)strlen( key );
			if( key[ 0 ] == '[' && key[ len - 1 ] == ']' )
			{
				memset( name, 0, sizeof( name ) );
				strcpy( name, key + 1 );
				name[ len - 2 ] = '\x00';
				trim( name );
				session = name;
			}
		}

		trim( key );
		trim( val );

		if( strlen( key ) && strlen( val ) )
		{
			if( !conf_session_add_value( conf, session, key, val ) )
			{
				release_conf( conf );
				fclose( fp );
				return NULL;
			}
		}

		memset( read_buf, 0, sizeof( read_buf ));
	}

	fclose( fp );

	return conf;
}


static int save_and_release_conf( conf_t *conf )
{
	session_element_t *e;
	FILE *fp;
	char file_buf[ 10240 ];
	char szpath[ MAX_PATH ];

	if( !conf )
		return -1;

	memset( file_buf, 0, sizeof( file_buf ) );
	memset( szpath, 0, sizeof( szpath));

	GetWinCurPath( szpath, conf->filename );
	fp = fopen( szpath, "w" );

	if( !fp )
	{
		release_conf( conf );	
		return -1;
	}

	while( !UT_ListIsEmpty( &conf->elements ) )
	{
		e = UT_ListGetObj( conf->elements.Next, session_element_t, list );
		save_session_conf_with_buffer( file_buf, e, conf->delim );

		UT_ListDel( &e->list );
		release_session_element( e );
	}


	save_buffer( fp, file_buf );


	fflush( fp );
	fclose( fp );

	if( conf->filename )
		free( conf->filename );

	if( conf->delim )
		free( conf->delim );

	free( conf );	
	
	return 0;
}

static void release_conf( conf_t *conf )
{
	session_element_t *e;

	if( !conf )
		return;

	while( !UT_ListIsEmpty( &conf->elements ) )
	{
		e = UT_ListGetObj( conf->elements.Next, session_element_t, list );

		UT_ListDel( &e->list );
		release_session_element( e );
	}

	if( conf->filename )
		free( conf->filename );

	if( conf->delim )
		free( conf->delim );

	free( conf );
}

static void release_element( element_t *e )
{
	if( !e )
		return;

	if( e->key )
		free( e->key );
	if( e->value )
		free( e->value );
	UT_ListDel( &e->list );
	free( e );
	e = NULL;
}

#if 0
static void trace_conf( conf_t *conf )
{
	tListHead *p;
	session_element_t *e;

	if( !conf )
		return;

	printf( "\n\"%s\"\n", conf->filename );

	UT_ListForEach( p, &conf->elements )
	{
		e = UT_ListGetObj( p, session_element_t, list );
		trace_session_elem( e, conf->delim );
	}
}

static int find_session( conf_t *conf, char * name )
{
	session_element_t * e;

	e = internal_find_session( conf, name, 0 );
	if( e != NULL )
		return 0;
	return -1;
}

static char *conf_add_value( conf_t *conf, char *key, char *value )
{
	return conf_session_add_value( conf, NULL, key, value );
}

static char *conf_get_value( conf_t *conf, char *key )
{
	return conf_session_get_value( conf, NULL, key );
}

static char *conf_set_value( conf_t *conf, char *key, char *value )
{
	return conf_session_set_value( conf, NULL, key, value );
}

#endif
static char * conf_session_add_value( conf_t * conf, char * name, char * key, char * value )
{
	session_element_t * e;

	e = internal_find_session( conf, name, 1 );
	if( e != NULL )
		return internal_conf_add_value( e, key, value );
	return NULL;
}

static char *conf_session_get_value( conf_t *conf, char *name, char *key )
{
	session_element_t * e;

	e = internal_find_session( conf, name, 0 );
	if( e != NULL )
		return internal_conf_get_value( e, key );
	return NULL;
}


static char * conf_session_set_value( conf_t *conf, char *name, char *key, char *value )
{
	int err;
	char *tmp;
	session_element_t * e;

	e = internal_find_session( conf, name, 0 );
	if( e != NULL )
	{
		tmp = internal_conf_set_value( e, key, value, &err );
		if( tmp != NULL || err == -1 )
			return tmp;
	}
	return conf_session_add_value( conf, name, key, value );
}

/*===================================================================================
External Function
====================================================================================*/


/*====================================================================================
LPR configuration
=====================================================================================*/
static int SetReferVal( void )
{
	g_lpr_conf.LPR_MIN_PIXEL					= g_lpr_conf.LPR_MIN_CHAR_W;
	g_lpr_conf.LPR_MIN_BIG4_H					= ( g_lpr_conf.LPR_MIN_PIXEL * 5 );
	g_lpr_conf.LPR_MIN_BIG4_W					= ( g_lpr_conf.LPR_UF_W );
	g_lpr_conf.LPR_MIN_PLT_H					= ( g_lpr_conf.LPR_MIN_BIG4_H + 6 );
	g_lpr_conf.LPR_MIN_PLT_W					= ( g_lpr_conf.LPR_MIN_BIG4_W + 6 );	
	g_lpr_conf.LPR_MAX_HG_MG_W					= ( g_lpr_conf.LPR_MIN_CHAR_W * 5 );
	g_lpr_conf.LPR_MAX_HG_MG_INT				= g_lpr_conf.LPR_MIN_PIXEL;
	
	return ERR_SUCCESS;
}

int loadLPRConf( void )
{
	conf_t * ptrConf = NULL;
	char * ptrVal = NULL;

	memset( &g_lpr_conf, 0, sizeof( LPR_CONF_t));
	
	/* =========================================================================
	Default Value
	============================================================================*/
	g_lpr_conf.LPR_DBG_SHOW_LOG					= 0;
	g_lpr_conf.LPR_TYPE							= 0;
	g_lpr_conf.LPR_AREACNT						= 0;	

	/* =========================================================================
	번호판 기준 크기 
	============================================================================*/
	g_lpr_conf.LPR_RESIZE_W						= 350;

#if 1
	g_lpr_conf.LPR_MIN_CHAR_H					= 10;
	g_lpr_conf.LPR_MIN_CHAR_W					= 3;	
#else
	g_lpr_conf.LPR_MIN_CHAR_H					= 9;
	g_lpr_conf.LPR_MIN_CHAR_W					= 2;	
#endif
	
	g_lpr_conf.LPR_UF_H							= 3;
	g_lpr_conf.LPR_UF_W							= 21;
	
	g_lpr_conf.LPR_MIN_CHAR_CNT					= 4;
	g_lpr_conf.LPR_MAX_CHAR_CNT					= 17;
	
	g_lpr_conf.LPR_L_ANGLE_PIXEL				= 15;
	g_lpr_conf.LPR_R_ANGLE_PIXEL				= 8;

	g_lpr_conf.LPR_MIN_PLATE_RATIO				= (float)1.2;
	g_lpr_conf.LPR_MAX_PLATE_RATIO				= (float)20.0;	
	g_lpr_conf.LPR_NEW_PLATE_RATIO				= (float)4.5;

	SetReferVal();

	
	/* =========================================================================
	Main Process 
	============================================================================*/
	g_lpr_conf.LPR_DBG_RUN_BASIC_STUDY				= 0;
	g_lpr_conf.LPR_DBG_RUN_RESIZE					= 0;
	g_lpr_conf.LPR_DBG_RUN_FIND_UF					= 1;
	g_lpr_conf.LPR_DBG_RUN_FIND_BIG4				= 1;
	g_lpr_conf.LPR_DBG_RUN_PROJECTION				= 1;
	g_lpr_conf.LPR_DBG_RUN_PRIORITY					= 1;	
	g_lpr_conf.LPR_DBG_RUN_CHOICE_PLATE				= 1;
	g_lpr_conf.LPR_DBG_RUN_CHAR_SEGMENT				= 1;
	g_lpr_conf.LPR_DBG_RUN_CHAR_RECOGNITION			= 1;

	/* =========================================================================
	Sub Process 
	============================================================================*/
	
	g_lpr_conf.LPR_DBG_RUN_UF_PREDIP				= 1;	
	g_lpr_conf.LPR_DBG_RUN_UF_DEL_THICK				= 1;
	g_lpr_conf.LPR_DBG_RUN_UF_LABELING				= 1;
	g_lpr_conf.LPR_DBG_RUN_UF_MERGE					= 1;
	g_lpr_conf.LPR_DBG_RUN_UF_EXTENSION				= 1;
	
	
	g_lpr_conf.LPR_DBG_RUN_BIG4_PREDIP				= 1;
	g_lpr_conf.LPR_DBG_RUN_BIG4_DEL_W_LINE			= 1;
	g_lpr_conf.LPR_DBG_RUN_BIG4_DEL_W_BLT			= 1;
	g_lpr_conf.LPR_DBG_RUN_BIG4_DEL_W_CUVE			= 1;
	g_lpr_conf.LPR_DBG_RUN_BIG4_DEL_H_LINE			= 1;
	g_lpr_conf.LPR_DBG_RUN_BIG4_BILINEAR			= 1;

	g_lpr_conf.LPR_DBG_RUN_PRJ_PREDIP				= 1;
	g_lpr_conf.LPR_DBG_RUN_PRJ_DEL_W_LINE			= 1;
	g_lpr_conf.LPR_DBG_RUN_PRJ_DEL_W_BLT			= 1;
	g_lpr_conf.LPR_DBG_RUN_PRJ_DEL_W_CUVE			= 1;
	g_lpr_conf.LPR_DBG_RUN_PRJ_DEL_H_LINE			= 1;
	g_lpr_conf.LPR_DBG_RUN_PRJ_DEL_NOISE			= 1;

	g_lpr_conf.LPR_DBG_RUN_PRJ_DETAIL_RECT			= 1;
	
	g_lpr_conf.LPR_DBG_RUN_CHAR_SAMPLING			= 0;
	g_lpr_conf.LPR_DBG_RUN_CHAR_RESIZE				= 1;
	g_lpr_conf.LPR_DBG_RUN_CHAR_THINNING			= 1;

	
	/* =========================================================================
	Image Debug
	============================================================================*/
	g_lpr_conf.LPR_DBG_COPY_TRANSFER_IMAGE		= 0;
	
	g_lpr_conf.LPR_DBG_SHOW_UF_DIP				= 0;
	g_lpr_conf.LPR_DBG_SHOW_UF_LINE				= 0;
	g_lpr_conf.LPR_DBG_SHOW_UF_VALID_BOX		= 0;

	g_lpr_conf.LPR_DBG_SHOW_BIG4_DIP			= 0;
	g_lpr_conf.LPR_DBG_SHOW_BIG4_BOX			= 0;
	
	g_lpr_conf.LPR_DBG_SHOW_PRJ_DIP				= 0;	
	g_lpr_conf.LPR_DBG_SHOW_PRJ_VALID_BOX		= 0;
	
	g_lpr_conf.LPR_DBG_SHOW_LAST_PLATE_BOX		= 0;	         
	g_lpr_conf.LPR_DBG_SHOW_LAST_CHAR_BOX		= 0;

	g_lpr_conf.LPR_DBG_SHOW_CHAR_SEG			= 0;
	g_lpr_conf.LPR_DBG_SHOW_W_LINE_SEG			= 0;
	g_lpr_conf.LPR_DBG_SHOW_H_LINE_SEG			= 0;

	ptrConf = load_conf ( TK_LPR_CONF_DATA, STR_DELIM );

	if ( ptrConf == NULL )
	{
		return ERR_NO_LPR_CONF;
	}

	ptrVal = conf_session_get_value( ptrConf, SESSION_LPR , LPR_KEY_SHOW_LOG );
	if ( ptrVal )
	{
		g_lpr_conf.LPR_DBG_SHOW_LOG = (BYTE)atoi( ptrVal );		
	}

	ptrVal = conf_session_get_value( ptrConf, SESSION_LPR , LPR_KEY_TYPE );
	if ( ptrVal )
	{
		g_lpr_conf.LPR_TYPE = (BYTE)atoi( ptrVal );		
	}

	ptrVal = conf_session_get_value( ptrConf, SESSION_LPR , LPR_KEY_AREA_CNT );
	if ( ptrVal )
	{
		g_lpr_conf.LPR_AREACNT = (BYTE)atoi( ptrVal );		
	}

	ptrVal = conf_session_get_value( ptrConf, SESSION_LPR , LPR_KEY_RESIZE_W );
	if ( ptrVal )
	{
		g_lpr_conf.LPR_RESIZE_W = (short)atoi( ptrVal );		
	}

	ptrVal = conf_session_get_value( ptrConf, SESSION_LPR , LPR_KEY_MIN_CHAR_H );
	if ( ptrVal )
	{
		g_lpr_conf.LPR_MIN_CHAR_H = (short)atoi( ptrVal );		
	}	

	ptrVal = conf_session_get_value( ptrConf, SESSION_LPR , LPR_KEY_MIN_CHAR_W );
	if ( ptrVal )
	{
		g_lpr_conf.LPR_MIN_CHAR_W = (short)atoi( ptrVal );		
	}

	ptrVal = conf_session_get_value( ptrConf, SESSION_LPR , LPR_KEY_UF_H );
	if ( ptrVal )
	{
		g_lpr_conf.LPR_UF_H = (short)atoi( ptrVal );		
	}

	ptrVal = conf_session_get_value( ptrConf, SESSION_LPR , LPR_KEY_UF_W );
	if ( ptrVal )
	{
		g_lpr_conf.LPR_UF_W = (short)atoi( ptrVal );
	}

	ptrVal = conf_session_get_value( ptrConf, SESSION_LPR , LPR_KEY_MIN_CHAR_CNT );
	if ( ptrVal )
	{
		g_lpr_conf.LPR_MIN_CHAR_CNT = (short)atoi( ptrVal );		
	}

	ptrVal = conf_session_get_value( ptrConf, SESSION_LPR , LPR_KEY_MAX_CHAR_CNT );
	if ( ptrVal )
	{
		g_lpr_conf.LPR_MAX_CHAR_CNT = (short)atoi( ptrVal );		
	}

	ptrVal = conf_session_get_value( ptrConf, SESSION_LPR , LPR_KEY_MIN_PLATE_RATIO );
	if ( ptrVal )
	{
		g_lpr_conf.LPR_MIN_PLATE_RATIO =  ((float)atoi( ptrVal ) / 10 );
	}

	ptrVal = conf_session_get_value( ptrConf, SESSION_LPR , LPR_KEY_MAX_PLATE_RATIO );
	if ( ptrVal )
	{
		g_lpr_conf.LPR_MAX_PLATE_RATIO = ((float)atoi( ptrVal ) / 10 );
	}

	ptrVal = conf_session_get_value( ptrConf, SESSION_LPR , LPR_KEY_NEW_PLATE_RATIO );
	if ( ptrVal )
	{
		g_lpr_conf.LPR_NEW_PLATE_RATIO =  ((float)atoi( ptrVal ) / 10 );
	}

	/* ===========================================================================
	main process 
	===============================================================================*/
	ptrVal = conf_session_get_value( ptrConf, SESSION_LPR , LPR_KEY_RUN_BASIC_STUDY );
	if ( ptrVal )
	{
		g_lpr_conf.LPR_DBG_RUN_BASIC_STUDY =(BYTE) atoi( ptrVal );
	}
	
	ptrVal = conf_session_get_value( ptrConf, SESSION_LPR , LPR_KEY_RUN_RESIZE );
	if ( ptrVal )
	{
		g_lpr_conf.LPR_DBG_RUN_RESIZE = (BYTE)atoi( ptrVal );
	}

	ptrVal = conf_session_get_value( ptrConf, SESSION_LPR , LPR_KEY_RUN_FIND_UF );
	if ( ptrVal )
	{
		g_lpr_conf.LPR_DBG_RUN_FIND_UF = (BYTE)atoi( ptrVal );
	}

	ptrVal = conf_session_get_value( ptrConf, SESSION_LPR , LPR_KEY_RUN_FIND_BIG4 );
	if ( ptrVal )
	{
		g_lpr_conf.LPR_DBG_RUN_FIND_BIG4 = (BYTE)atoi( ptrVal );
	}

	ptrVal = conf_session_get_value( ptrConf, SESSION_LPR , LPR_KEY_RUN_PROJECTION );
	if ( ptrVal )
	{
		g_lpr_conf.LPR_DBG_RUN_PROJECTION = (BYTE)atoi( ptrVal );
	}

	ptrVal = conf_session_get_value( ptrConf, SESSION_LPR , LPR_KEY_RUN_PRIORITY );
	if ( ptrVal )
	{
		g_lpr_conf.LPR_DBG_RUN_PRIORITY = (BYTE)atoi( ptrVal );
	}
	
	ptrVal = conf_session_get_value( ptrConf, SESSION_LPR , LPR_KEY_RUN_CHO_PLATE );
	if ( ptrVal )
	{
		g_lpr_conf.LPR_DBG_RUN_CHOICE_PLATE = (BYTE)atoi( ptrVal );
	}

	ptrVal = conf_session_get_value( ptrConf, SESSION_LPR , LPR_KEY_RUN_CHAR_SEGMENT );
	if ( ptrVal )
	{
		g_lpr_conf.LPR_DBG_RUN_CHAR_SEGMENT = (BYTE)atoi( ptrVal );
	}
	
	
	ptrVal = conf_session_get_value( ptrConf, SESSION_LPR , LPR_KEY_RUN_CHAR_RECOG );
	if ( ptrVal )
	{
		g_lpr_conf.LPR_DBG_RUN_CHAR_RECOGNITION = (BYTE)atoi( ptrVal );
	}

	/* =====================================================================================
	sub process 
	========================================================================================*/ 
	ptrVal = conf_session_get_value( ptrConf, SESSION_LPR , LPR_KEY_RUN_UF_PREDIP );
	if ( ptrVal )
	{
		g_lpr_conf.LPR_DBG_RUN_UF_PREDIP = (BYTE)atoi( ptrVal );
	}

	ptrVal = conf_session_get_value( ptrConf, SESSION_LPR , LPR_KEY_RUN_UF_DEL_THICK );
	if ( ptrVal )
	{
		g_lpr_conf.LPR_DBG_RUN_UF_DEL_THICK =(BYTE) atoi( ptrVal );
	}

	ptrVal = conf_session_get_value( ptrConf, SESSION_LPR , LPR_KEY_RUN_UF_GROUP_LABEL );
	if ( ptrVal )
	{
		g_lpr_conf.LPR_DBG_RUN_UF_LABELING = (BYTE)atoi( ptrVal );
	}

	ptrVal = conf_session_get_value( ptrConf, SESSION_LPR , LPR_KEY_RUN_UF_GROUP_MERGE );
	if ( ptrVal )
	{
		g_lpr_conf.LPR_DBG_RUN_UF_MERGE =(BYTE) atoi( ptrVal );
	}

	ptrVal = conf_session_get_value( ptrConf, SESSION_LPR , LPR_KEY_RUN_UF_GROUP_EXTEN );
	if ( ptrVal )
	{
		g_lpr_conf.LPR_DBG_RUN_UF_EXTENSION = (BYTE)atoi( ptrVal );
	}

	ptrVal = conf_session_get_value( ptrConf, SESSION_LPR , LPR_KEY_RUN_BIG4_PREDIP );
	if ( ptrVal )
	{
		g_lpr_conf.LPR_DBG_RUN_BIG4_PREDIP = (BYTE)atoi( ptrVal );
	}

	ptrVal = conf_session_get_value( ptrConf, SESSION_LPR , LPR_KEY_RUN_BIG4_DEL_W_LINE );
	if ( ptrVal )
	{
		g_lpr_conf.LPR_DBG_RUN_BIG4_DEL_W_LINE = (BYTE)atoi( ptrVal );
	}

	ptrVal = conf_session_get_value( ptrConf, SESSION_LPR , LPR_KEY_RUN_BIG4_DEL_W_BLT );
	if ( ptrVal )
	{
		g_lpr_conf.LPR_DBG_RUN_BIG4_DEL_W_BLT = (BYTE)atoi( ptrVal );
	}

	ptrVal = conf_session_get_value( ptrConf, SESSION_LPR , LPR_KEY_RUN_BIG4_DEL_W_CUVE );
	if ( ptrVal )
	{
		g_lpr_conf.LPR_DBG_RUN_BIG4_DEL_W_CUVE = (BYTE)atoi( ptrVal );
	}
	
	ptrVal = conf_session_get_value( ptrConf, SESSION_LPR , LPR_KEY_RUN_BIG4_DEL_H_LINE );
	if ( ptrVal )
	{
		g_lpr_conf.LPR_DBG_RUN_BIG4_DEL_H_LINE = (BYTE)atoi( ptrVal );
	}

	ptrVal = conf_session_get_value( ptrConf, SESSION_LPR , LPR_KEY_RUN_BIG4_BILINEAR );
	if ( ptrVal )
	{
		g_lpr_conf.LPR_DBG_RUN_BIG4_BILINEAR = (BYTE)atoi( ptrVal );
	}




	ptrVal = conf_session_get_value( ptrConf, SESSION_LPR , LPR_KEY_RUN_PROJECTION_PREDIP );
	if ( ptrVal )
	{
		g_lpr_conf.LPR_DBG_RUN_PRJ_PREDIP = (BYTE)atoi( ptrVal );
	}

	ptrVal = conf_session_get_value( ptrConf, SESSION_LPR , LPR_KEY_RUN_PROJECTION_DEL_W_LINE );
	if ( ptrVal )
	{
		g_lpr_conf.LPR_DBG_RUN_PRJ_DEL_W_LINE = (BYTE)atoi( ptrVal );
	}

	ptrVal = conf_session_get_value( ptrConf, SESSION_LPR , LPR_KEY_RUN_PROJECTION_DEL_W_BLT );
	if ( ptrVal )
	{
		g_lpr_conf.LPR_DBG_RUN_PRJ_DEL_W_BLT = (BYTE)atoi( ptrVal );
	}

	ptrVal = conf_session_get_value( ptrConf, SESSION_LPR , LPR_KEY_RUN_PROJECTION_DEL_W_CUVE );
	if ( ptrVal )
	{
		g_lpr_conf.LPR_DBG_RUN_PRJ_DEL_W_CUVE = (BYTE)atoi( ptrVal );
	}	

	ptrVal = conf_session_get_value( ptrConf, SESSION_LPR , LPR_KEY_RUN_PROJECTION_DEL_H_LINE );
	if ( ptrVal )
	{
		g_lpr_conf.LPR_DBG_RUN_PRJ_DEL_H_LINE = (BYTE)atoi( ptrVal );
	}

	ptrVal = conf_session_get_value( ptrConf, SESSION_LPR , LPR_KEY_RUN_PROJECTION_DEL_NOISE );
	if ( ptrVal )
	{
		g_lpr_conf.LPR_DBG_RUN_PRJ_DEL_NOISE = (BYTE)atoi( ptrVal );
	}


	ptrVal = conf_session_get_value( ptrConf, SESSION_LPR , LPR_KEY_RUN_PROJECTION_DETAIL_RECT );
	if ( ptrVal )
	{
		g_lpr_conf.LPR_DBG_RUN_PRJ_DETAIL_RECT = (BYTE)atoi( ptrVal );
	}
	
	

	ptrVal = conf_session_get_value( ptrConf, SESSION_LPR , LPR_KEY_RUN_CHAR_SAMPLING );
	if ( ptrVal )
	{
		g_lpr_conf.LPR_DBG_RUN_CHAR_SAMPLING = (BYTE)atoi( ptrVal );
	}

	ptrVal = conf_session_get_value( ptrConf, SESSION_LPR , LPR_KEY_RUN_CHAR_RESIZE );
	if ( ptrVal )
	{
		g_lpr_conf.LPR_DBG_RUN_CHAR_RESIZE = (BYTE)atoi( ptrVal );
	}

	ptrVal = conf_session_get_value( ptrConf, SESSION_LPR , LPR_KEY_RUN_CHAR_THINNING );
	if ( ptrVal )
	{
		g_lpr_conf.LPR_DBG_RUN_CHAR_THINNING = (BYTE)atoi( ptrVal );
	}

	

	/*======================================================================================
	Debug
	========================================================================================*/
	ptrVal = conf_session_get_value( ptrConf, SESSION_LPR , LPR_KEY_SHOW_CPY_PRC_IMG );
	if ( ptrVal )
	{
		g_lpr_conf.LPR_DBG_COPY_TRANSFER_IMAGE = (BYTE)atoi( ptrVal );
	}

	ptrVal = conf_session_get_value( ptrConf, SESSION_LPR , LPR_KEY_SHOW_UF_DIP );
	if ( ptrVal )
	{
		g_lpr_conf.LPR_DBG_SHOW_UF_DIP = (BYTE)atoi( ptrVal );
	}

	ptrVal = conf_session_get_value( ptrConf, SESSION_LPR , LPR_KEY_SHOW_UF_LINE );
	if ( ptrVal )
	{
		g_lpr_conf.LPR_DBG_SHOW_UF_LINE = (BYTE)atoi( ptrVal );
	}

	ptrVal = conf_session_get_value( ptrConf, SESSION_LPR , LPR_KEY_SHOW_UF_LABEL_BOX );
	if ( ptrVal )
	{
		g_lpr_conf.LPR_DBG_SHOW_UF_VALID_BOX = (BYTE)atoi( ptrVal );
	}


	ptrVal = conf_session_get_value( ptrConf, SESSION_LPR , LPR_KEY_SHOW_BIG4_DIP );
	if ( ptrVal )
	{
		g_lpr_conf.LPR_DBG_SHOW_BIG4_DIP = (BYTE)atoi( ptrVal );
	}

	ptrVal = conf_session_get_value( ptrConf, SESSION_LPR , LPR_KEY_SHOW_BIG4_BOX );
	if ( ptrVal )
	{
		g_lpr_conf.LPR_DBG_SHOW_BIG4_BOX = (BYTE)atoi( ptrVal );
	}


	ptrVal = conf_session_get_value( ptrConf, SESSION_LPR , LPR_KEY_SHOW_PROJECTION_DIP );
	if ( ptrVal )
	{
		g_lpr_conf.LPR_DBG_SHOW_PRJ_DIP = (BYTE)atoi( ptrVal );
	}
	

	ptrVal = conf_session_get_value( ptrConf, SESSION_LPR , LPR_KEY_SHOW_PROJECTION_BOX );
	if ( ptrVal )
	{
		g_lpr_conf.LPR_DBG_SHOW_PRJ_VALID_BOX = (BYTE)atoi( ptrVal );
	}

	
	ptrVal = conf_session_get_value( ptrConf, SESSION_LPR , LPR_KEY_SHOW_LAST_PLATE_BOX );
	if ( ptrVal )
	{
		g_lpr_conf.LPR_DBG_SHOW_LAST_PLATE_BOX = (BYTE)atoi( ptrVal );
	}	

	ptrVal = conf_session_get_value( ptrConf, SESSION_LPR , LPR_KEY_SHOW_LAST_CHAR_BOX );
	if ( ptrVal )
	{
		g_lpr_conf.LPR_DBG_SHOW_LAST_CHAR_BOX = (BYTE)atoi( ptrVal );
	}	

	ptrVal = conf_session_get_value( ptrConf, SESSION_LPR , LPR_KEY_SHOW_CHAR_SEG );
	if ( ptrVal )
	{
		g_lpr_conf.LPR_DBG_SHOW_CHAR_SEG = (BYTE)atoi( ptrVal );
	}	

	ptrVal = conf_session_get_value( ptrConf, SESSION_LPR , LPR_KEY_SHOW_W_LINE_SEG );
	if ( ptrVal )
	{
		g_lpr_conf.LPR_DBG_SHOW_W_LINE_SEG = (BYTE)atoi( ptrVal );
	}	

	ptrVal = conf_session_get_value( ptrConf, SESSION_LPR , LPR_KEY_SHOW_H_LINE_SEG );
	if ( ptrVal )
	{
		g_lpr_conf.LPR_DBG_SHOW_H_LINE_SEG = (BYTE)atoi( ptrVal );
	}	


	release_conf( ptrConf);
	ptrConf = NULL;


	SetReferVal();

	return ERR_SUCCESS;
}

pLPR_CONF GetLPRConf( void )
{
	return &g_lpr_conf;
}


int SetLPRConf( pLPR_CONF ptrLPRConf )
{
	conf_t * ptrConf = NULL;
	char szval[ CONF_BUF_SIZE ];
	int ret = ERR_SUCCESS;

	ptrConf = load_conf ( TK_LPR_CONF_DATA, STR_DELIM );

	if ( ptrConf == NULL || ptrLPRConf == NULL )
	{
		return ERR_NO_LPR_CONF;
	}

	
	memset( szval, 0, sizeof( szval ));
	sprintf( szval, "%d", ptrLPRConf->LPR_TYPE);
	conf_session_set_value( ptrConf, SESSION_LPR, LPR_KEY_TYPE, szval );

	memset( szval, 0, sizeof( szval ));
	sprintf( szval, "%d", ptrLPRConf->LPR_AREACNT);
	conf_session_set_value( ptrConf, SESSION_LPR, LPR_KEY_AREA_CNT, szval );

	memset( szval, 0, sizeof( szval ));
	sprintf( szval, "%d", ptrLPRConf->LPR_RESIZE_W);
	conf_session_set_value( ptrConf, SESSION_LPR, LPR_KEY_RESIZE_W, szval );

	memset( szval, 0, sizeof( szval ));
	sprintf( szval, "%d", ptrLPRConf->LPR_UF_H);
	conf_session_set_value( ptrConf, SESSION_LPR, LPR_KEY_UF_H, szval );

	memset( szval, 0, sizeof( szval ));
	sprintf( szval, "%d", ptrLPRConf->LPR_UF_W);
	conf_session_set_value( ptrConf, SESSION_LPR, LPR_KEY_UF_W, szval );
	

	memset( szval, 0, sizeof( szval ));
	sprintf( szval, "%d", ptrLPRConf->LPR_MIN_CHAR_H);
	conf_session_set_value( ptrConf, SESSION_LPR, LPR_KEY_MIN_CHAR_H, szval );

	memset( szval, 0, sizeof( szval ));
	sprintf( szval, "%d", ptrLPRConf->LPR_MIN_CHAR_W);
	conf_session_set_value( ptrConf, SESSION_LPR, LPR_KEY_MIN_CHAR_W, szval );
	
	memset( szval, 0, sizeof( szval ));
	sprintf( szval, "%d", ptrLPRConf->LPR_MIN_CHAR_CNT);
	conf_session_set_value( ptrConf, SESSION_LPR, LPR_KEY_MIN_CHAR_CNT, szval );

	memset( szval, 0, sizeof( szval ));
	sprintf( szval, "%d", ptrLPRConf->LPR_MAX_CHAR_CNT);
	conf_session_set_value( ptrConf, SESSION_LPR, LPR_KEY_MAX_CHAR_CNT, szval );

	memset( szval, 0, sizeof( szval ));
	sprintf( szval, "%d", (int)(ptrLPRConf->LPR_MIN_PLATE_RATIO * 10) );
	conf_session_set_value( ptrConf, SESSION_LPR, LPR_KEY_MIN_PLATE_RATIO, szval );

	memset( szval, 0, sizeof( szval ));
	sprintf( szval, "%d", (int)(ptrLPRConf->LPR_MAX_PLATE_RATIO * 10) );
	conf_session_set_value( ptrConf, SESSION_LPR, LPR_KEY_MAX_PLATE_RATIO, szval );

	memset( szval, 0, sizeof( szval ));
	sprintf( szval, "%d", (int)(ptrLPRConf->LPR_NEW_PLATE_RATIO * 10) );
	conf_session_set_value( ptrConf, SESSION_LPR, LPR_KEY_NEW_PLATE_RATIO, szval );

	/* ==========================================================================================
	Main process 
	============================================================================================*/
	memset( szval, 0, sizeof( szval ));
	sprintf( szval, "%d", ptrLPRConf->LPR_DBG_RUN_BASIC_STUDY );
	conf_session_set_value( ptrConf, SESSION_LPR, LPR_KEY_RUN_BASIC_STUDY, szval );

	memset( szval, 0, sizeof( szval ));
	sprintf( szval, "%d", ptrLPRConf->LPR_DBG_RUN_RESIZE );
	conf_session_set_value( ptrConf, SESSION_LPR, LPR_KEY_RUN_RESIZE, szval );

	memset( szval, 0, sizeof( szval ));
	sprintf( szval, "%d", ptrLPRConf->LPR_DBG_RUN_FIND_UF );
	conf_session_set_value( ptrConf, SESSION_LPR, LPR_KEY_RUN_FIND_UF, szval );

	memset( szval, 0, sizeof( szval ));
	sprintf( szval, "%d", ptrLPRConf->LPR_DBG_RUN_FIND_BIG4 );
	conf_session_set_value( ptrConf, SESSION_LPR, LPR_KEY_RUN_FIND_BIG4, szval );

	memset( szval, 0, sizeof( szval ));
	sprintf( szval, "%d", ptrLPRConf->LPR_DBG_RUN_PROJECTION );
	conf_session_set_value( ptrConf, SESSION_LPR, LPR_KEY_RUN_PROJECTION, szval );

	memset( szval, 0, sizeof( szval ));
	sprintf( szval, "%d", ptrLPRConf->LPR_DBG_RUN_PRIORITY );
	conf_session_set_value( ptrConf, SESSION_LPR, LPR_KEY_RUN_PRIORITY, szval );

	memset( szval, 0, sizeof( szval ));
	sprintf( szval, "%d", ptrLPRConf->LPR_DBG_RUN_CHOICE_PLATE );
	conf_session_set_value( ptrConf, SESSION_LPR, LPR_KEY_RUN_CHO_PLATE, szval );


	memset( szval, 0, sizeof( szval ));
	sprintf( szval, "%d", ptrLPRConf->LPR_DBG_RUN_CHAR_SEGMENT );
	conf_session_set_value( ptrConf, SESSION_LPR, LPR_KEY_RUN_CHAR_SEGMENT, szval );	

	memset( szval, 0, sizeof( szval ));
	sprintf( szval, "%d", ptrLPRConf->LPR_DBG_RUN_CHAR_RECOGNITION );
	conf_session_set_value( ptrConf, SESSION_LPR, LPR_KEY_RUN_CHAR_RECOG, szval );


	/* ===================================================================================
	sub process 
	======================================================================================*/
	memset( szval, 0, sizeof( szval ));
	sprintf( szval, "%d", ptrLPRConf->LPR_DBG_RUN_UF_PREDIP );
	conf_session_set_value( ptrConf, SESSION_LPR, LPR_KEY_RUN_UF_PREDIP, szval );

	memset( szval, 0, sizeof( szval ));
	sprintf( szval, "%d", ptrLPRConf->LPR_DBG_RUN_UF_DEL_THICK );
	conf_session_set_value( ptrConf, SESSION_LPR, LPR_KEY_RUN_UF_DEL_THICK, szval );

	memset( szval, 0, sizeof( szval ));
	sprintf( szval, "%d", ptrLPRConf->LPR_DBG_RUN_UF_LABELING );
	conf_session_set_value( ptrConf, SESSION_LPR, LPR_KEY_RUN_UF_GROUP_LABEL, szval );

	memset( szval, 0, sizeof( szval ));
	sprintf( szval, "%d", ptrLPRConf->LPR_DBG_RUN_UF_MERGE );
	conf_session_set_value( ptrConf, SESSION_LPR, LPR_KEY_RUN_UF_GROUP_MERGE, szval );

	memset( szval, 0, sizeof( szval ));
	sprintf( szval, "%d", ptrLPRConf->LPR_DBG_RUN_UF_EXTENSION );
	conf_session_set_value( ptrConf, SESSION_LPR, LPR_KEY_RUN_UF_GROUP_EXTEN, szval );


	memset( szval, 0, sizeof( szval ));
	sprintf( szval, "%d", ptrLPRConf->LPR_DBG_RUN_BIG4_PREDIP );
	conf_session_set_value( ptrConf, SESSION_LPR, LPR_KEY_RUN_BIG4_PREDIP, szval );

	memset( szval, 0, sizeof( szval ));
	sprintf( szval, "%d", ptrLPRConf->LPR_DBG_RUN_BIG4_DEL_W_LINE );
	conf_session_set_value( ptrConf, SESSION_LPR, LPR_KEY_RUN_BIG4_DEL_W_LINE, szval );

	memset( szval, 0, sizeof( szval ));
	sprintf( szval, "%d", ptrLPRConf->LPR_DBG_RUN_BIG4_DEL_W_BLT );
	conf_session_set_value( ptrConf, SESSION_LPR, LPR_KEY_RUN_BIG4_DEL_W_BLT, szval );

	memset( szval, 0, sizeof( szval ));
	sprintf( szval, "%d", ptrLPRConf->LPR_DBG_RUN_BIG4_DEL_W_CUVE );
	conf_session_set_value( ptrConf, SESSION_LPR, LPR_KEY_RUN_BIG4_DEL_W_CUVE, szval );

	memset( szval, 0, sizeof( szval ));
	sprintf( szval, "%d", ptrLPRConf->LPR_DBG_RUN_BIG4_DEL_H_LINE );
	conf_session_set_value( ptrConf, SESSION_LPR, LPR_KEY_RUN_BIG4_DEL_H_LINE, szval );

	memset( szval, 0, sizeof( szval ));
	sprintf( szval, "%d", ptrLPRConf->LPR_DBG_RUN_BIG4_BILINEAR );
	conf_session_set_value( ptrConf, SESSION_LPR, LPR_KEY_RUN_BIG4_BILINEAR, szval );


	memset( szval, 0, sizeof( szval ));
	sprintf( szval, "%d", ptrLPRConf->LPR_DBG_RUN_PRJ_PREDIP );
	conf_session_set_value( ptrConf, SESSION_LPR, LPR_KEY_RUN_PROJECTION_PREDIP, szval );

	memset( szval, 0, sizeof( szval ));
	sprintf( szval, "%d", ptrLPRConf->LPR_DBG_RUN_PRJ_DEL_W_LINE );
	conf_session_set_value( ptrConf, SESSION_LPR, LPR_KEY_RUN_PROJECTION_DEL_W_LINE, szval );

	memset( szval, 0, sizeof( szval ));
	sprintf( szval, "%d", ptrLPRConf->LPR_DBG_RUN_PRJ_DEL_W_BLT );
	conf_session_set_value( ptrConf, SESSION_LPR, LPR_KEY_RUN_PROJECTION_DEL_W_BLT, szval );

	memset( szval, 0, sizeof( szval ));
	sprintf( szval, "%d", ptrLPRConf->LPR_DBG_RUN_PRJ_DEL_W_CUVE );
	conf_session_set_value( ptrConf, SESSION_LPR, LPR_KEY_RUN_PROJECTION_DEL_W_CUVE, szval );

	memset( szval, 0, sizeof( szval ));
	sprintf( szval, "%d", ptrLPRConf->LPR_DBG_RUN_PRJ_DEL_H_LINE );
	conf_session_set_value( ptrConf, SESSION_LPR, LPR_KEY_RUN_PROJECTION_DEL_H_LINE, szval );

	memset( szval, 0, sizeof( szval ));
	sprintf( szval, "%d", ptrLPRConf->LPR_DBG_RUN_PRJ_DEL_NOISE );
	conf_session_set_value( ptrConf, SESSION_LPR, LPR_KEY_RUN_PROJECTION_DEL_NOISE, szval );


	memset( szval, 0, sizeof( szval ));
	sprintf( szval, "%d", ptrLPRConf->LPR_DBG_RUN_PRJ_DETAIL_RECT );
	conf_session_set_value( ptrConf, SESSION_LPR, LPR_KEY_RUN_PROJECTION_DETAIL_RECT, szval );
		
	memset( szval, 0, sizeof( szval ));
	sprintf( szval, "%d", ptrLPRConf->LPR_DBG_RUN_CHAR_SAMPLING );
	conf_session_set_value( ptrConf, SESSION_LPR, LPR_KEY_RUN_CHAR_SAMPLING, szval );

	memset( szval, 0, sizeof( szval ));
	sprintf( szval, "%d", ptrLPRConf->LPR_DBG_RUN_CHAR_RESIZE );
	conf_session_set_value( ptrConf, SESSION_LPR, LPR_KEY_RUN_CHAR_RESIZE, szval );

	memset( szval, 0, sizeof( szval ));
	sprintf( szval, "%d", ptrLPRConf->LPR_DBG_RUN_CHAR_THINNING );
	conf_session_set_value( ptrConf, SESSION_LPR, LPR_KEY_RUN_CHAR_THINNING, szval );

	
	/* ==========================================================================================
	debug 
	============================================================================================*/
	memset( szval, 0, sizeof( szval ));
	sprintf( szval, "%d", ptrLPRConf->LPR_DBG_SHOW_LOG);
	conf_session_set_value( ptrConf, SESSION_LPR, LPR_KEY_SHOW_LOG, szval );

	memset( szval, 0, sizeof( szval ));
	sprintf( szval, "%d", ptrLPRConf->LPR_DBG_COPY_TRANSFER_IMAGE );
	conf_session_set_value( ptrConf, SESSION_LPR, LPR_KEY_SHOW_CPY_PRC_IMG, szval );

	memset( szval, 0, sizeof( szval ));
	sprintf( szval, "%d", ptrLPRConf->LPR_DBG_SHOW_UF_DIP );
	conf_session_set_value( ptrConf, SESSION_LPR, LPR_KEY_SHOW_UF_DIP, szval );

	memset( szval, 0, sizeof( szval ));
	sprintf( szval, "%d", ptrLPRConf->LPR_DBG_SHOW_UF_LINE );
	conf_session_set_value( ptrConf, SESSION_LPR, LPR_KEY_SHOW_UF_LINE, szval );

	memset( szval, 0, sizeof( szval ));
	sprintf( szval, "%d", ptrLPRConf->LPR_DBG_SHOW_UF_VALID_BOX );
	conf_session_set_value( ptrConf, SESSION_LPR, LPR_KEY_SHOW_UF_LABEL_BOX, szval );


	memset( szval, 0, sizeof( szval ));
	sprintf( szval, "%d", ptrLPRConf->LPR_DBG_SHOW_BIG4_DIP );
	conf_session_set_value( ptrConf, SESSION_LPR, LPR_KEY_SHOW_BIG4_DIP, szval );

	memset( szval, 0, sizeof( szval ));
	sprintf( szval, "%d", ptrLPRConf->LPR_DBG_SHOW_BIG4_BOX );
	conf_session_set_value( ptrConf, SESSION_LPR, LPR_KEY_SHOW_BIG4_BOX, szval );

	memset( szval, 0, sizeof( szval ));
	sprintf( szval, "%d", ptrLPRConf->LPR_DBG_SHOW_PRJ_DIP );
	conf_session_set_value( ptrConf, SESSION_LPR, LPR_KEY_SHOW_PROJECTION_DIP, szval );

	memset( szval, 0, sizeof( szval ));
	sprintf( szval, "%d", ptrLPRConf->LPR_DBG_SHOW_PRJ_VALID_BOX );
	conf_session_set_value( ptrConf, SESSION_LPR, LPR_KEY_SHOW_PROJECTION_BOX, szval );
	
	memset( szval, 0, sizeof( szval ));
	sprintf( szval, "%d", ptrLPRConf->LPR_DBG_SHOW_LAST_PLATE_BOX );	                                  
	conf_session_set_value( ptrConf, SESSION_LPR, LPR_KEY_SHOW_LAST_PLATE_BOX, szval );

	memset( szval, 0, sizeof( szval ));
	sprintf( szval, "%d", ptrLPRConf->LPR_DBG_SHOW_LAST_CHAR_BOX );	                                  
	conf_session_set_value( ptrConf, SESSION_LPR, LPR_KEY_SHOW_LAST_CHAR_BOX, szval );

	memset( szval, 0, sizeof( szval ));
	sprintf( szval, "%d", ptrLPRConf->LPR_DBG_SHOW_CHAR_SEG );
	conf_session_set_value( ptrConf, SESSION_LPR, LPR_KEY_SHOW_CHAR_SEG, szval );

	memset( szval, 0, sizeof( szval ));
	sprintf( szval, "%d", ptrLPRConf->LPR_DBG_SHOW_W_LINE_SEG );	                                  
	conf_session_set_value( ptrConf, SESSION_LPR, LPR_KEY_SHOW_W_LINE_SEG, szval );

	memset( szval, 0, sizeof( szval ));
	sprintf( szval, "%d", ptrLPRConf->LPR_DBG_SHOW_H_LINE_SEG );	                                  
	conf_session_set_value( ptrConf, SESSION_LPR, LPR_KEY_SHOW_H_LINE_SEG, szval );


	ret = save_and_release_conf( ptrConf );
	
	if( ret== ERR_SUCCESS )
	{
		memset( &g_lpr_conf, 0, sizeof( LPR_CONF_t ));
		memcpy( &g_lpr_conf, ptrLPRConf,sizeof( LPR_CONF_t )); 
	}

	SetReferVal();

	return ret;
}

/*====================================================================================
OCR configuration
=====================================================================================*/
int loadOCRConf( void)
{
	conf_t * ptrConf = NULL;
	char * ptrVal = NULL;

	memset( &g_ocr_conf, 0, sizeof( OCR_CONF_t));

	/* Default Value */
	g_ocr_conf.OCR_DBG_SHOW_LOG		= DEFAULT_SHOW_LOG;
	g_ocr_conf.OCR_NN_TYPE			= DEFAULT_NN_TYPE;
	g_ocr_conf.OCR_LEARNING_CNT		= DEFAULT_LEARNING_CNT;
	g_ocr_conf.OCR_CHAR_H			= DEFAULT_OCR_CHAR_H;
	g_ocr_conf.OCR_CHAR_W			= DEFAULT_OCR_CHAR_W ;
	g_ocr_conf.OCR_RESIZE_THRESHOLD = ( DEFFAULT_RESIZE_THRESHOLD );

	ptrConf = load_conf ( TK_OCR_CONF_DATA, STR_DELIM );

	if ( ptrConf == NULL )
	{
		return ERR_NO_LPR_CONF;
	}

	ptrVal = conf_session_get_value( ptrConf, SESSION_OCR , OCR_KEY_SHOW_LOG );
	if ( ptrVal )
	{
		g_ocr_conf.OCR_DBG_SHOW_LOG = (BYTE)atoi( ptrVal );
	}

	ptrVal = conf_session_get_value( ptrConf, SESSION_OCR , OCR_KEY_NN_TYPE );
	if ( ptrVal )
	{
		g_ocr_conf.OCR_NN_TYPE = (BYTE)atoi( ptrVal );
	}

	ptrVal = conf_session_get_value( ptrConf, SESSION_OCR , OCR_KEY_LEARNING_CNT );
	if ( ptrVal )
	{
		g_ocr_conf.OCR_LEARNING_CNT = atoi( ptrVal );
	}

	ptrVal = conf_session_get_value( ptrConf, SESSION_OCR , OCR_KEY_RESIZE_THS );
	if ( ptrVal )
	{
		g_ocr_conf.OCR_RESIZE_THRESHOLD = (BYTE)atoi( ptrVal );		
	}

	ptrVal = conf_session_get_value( ptrConf, SESSION_OCR , OCR_KEY_CHAR_H );
	if ( ptrVal )
	{
		g_ocr_conf.OCR_CHAR_H = (short)atoi( ptrVal );
		g_ocr_conf.OCR_CHAR_W = (short)(g_ocr_conf.OCR_CHAR_H / 1.3 );
	}
	
	release_conf( ptrConf);
	ptrConf = NULL;

	return ERR_SUCCESS;

}

pOCR_CONF GetOCRConf( void )
{
	return &g_ocr_conf;
}

int SetOCRConf( pOCR_CONF ptrOCRConf )
{
	conf_t * ptrConf = NULL;
	char szval[ CONF_BUF_SIZE ];
	int ret = ERR_SUCCESS;

	ptrConf = load_conf ( TK_OCR_CONF_DATA, STR_DELIM );

	if ( ptrConf == NULL || ptrOCRConf == NULL )
	{
		return ERR_NO_OCR_CONF;
	}

	memset( szval, 0, sizeof( szval ));
	sprintf( szval, "%d", ptrOCRConf->OCR_DBG_SHOW_LOG );
	conf_session_set_value( ptrConf, SESSION_OCR, OCR_KEY_SHOW_LOG, szval );

	memset( szval, 0, sizeof( szval ));
	sprintf( szval, "%d", ptrOCRConf->OCR_NN_TYPE );
	conf_session_set_value( ptrConf, SESSION_OCR, OCR_KEY_NN_TYPE, szval );

	memset( szval, 0, sizeof( szval ));
	sprintf( szval, "%d", ptrOCRConf->OCR_LEARNING_CNT );
	conf_session_set_value( ptrConf, SESSION_OCR, OCR_KEY_LEARNING_CNT, szval );

	memset( szval, 0, sizeof( szval ));
	sprintf( szval, "%d", ptrOCRConf->OCR_RESIZE_THRESHOLD );
	conf_session_set_value( ptrConf, SESSION_OCR, OCR_KEY_RESIZE_THS, szval );

	memset( szval, 0, sizeof( szval ));
	sprintf( szval, "%d", ptrOCRConf->OCR_CHAR_H );
	conf_session_set_value( ptrConf, SESSION_OCR, OCR_KEY_CHAR_H, szval );

	ret = save_and_release_conf( ptrConf );
	
	if( ret== ERR_SUCCESS )
	{
		memset( &g_ocr_conf, 0, sizeof( OCR_CONF_t ));
		memcpy( &g_ocr_conf, ptrOCRConf, sizeof( OCR_CONF_t )); 
	}
	
	return ret;
}

/*====================================================================================
DIP configuration
=====================================================================================*/
int loadDIPConf( void)
{
	conf_t * ptrConf = NULL;
	char * ptrVal = NULL;

	memset( &g_dip_conf, 0, sizeof(DIP_CONF_t));	
	/* Default Value */
	g_dip_conf.DIP_DBG_SHOW_LOG		= DEFAULT_SHOW_LOG;
	
	ptrConf = load_conf ( TK_DIP_CONF_DATA, STR_DELIM );

	if ( ptrConf == NULL )
	{
		return ERR_NO_LPR_CONF;
	}

	ptrVal = conf_session_get_value( ptrConf, SESSION_DIP , DIP_KEY_SHOW_LOG );
	if ( ptrVal )
	{
		g_dip_conf.DIP_DBG_SHOW_LOG = (BYTE)atoi( ptrVal );
	}

	release_conf( ptrConf);
	ptrConf = NULL;

	return ERR_SUCCESS;
}

pDIP_CONF GetDIPConf( void )
{
	return &g_dip_conf;
}

int SetDIPConf( pDIP_CONF ptrDIPConf )
{
	conf_t * ptrConf = NULL;
	char szval[ CONF_BUF_SIZE ];
	int ret = ERR_SUCCESS;

	ptrConf = load_conf ( TK_DIP_CONF_DATA, STR_DELIM );

	if ( ptrConf == NULL || ptrDIPConf == NULL )
	{
		return ERR_NO_OCR_CONF;
	}

	memset( szval, 0, sizeof( szval ));
	sprintf( szval, "%d", ptrDIPConf->DIP_DBG_SHOW_LOG );
	conf_session_set_value( ptrConf, SESSION_DIP, DIP_KEY_SHOW_LOG, szval );
	
	ret = save_and_release_conf( ptrConf );

	if( ret== ERR_SUCCESS )
	{
		memset( &g_dip_conf, 0, sizeof( DIP_CONF_t ));
		memcpy( &g_dip_conf, ptrDIPConf, sizeof( DIP_CONF_t )); 
	}

	return ERR_SUCCESS;
}

/*====================================================================================
Application configuration
=====================================================================================*/
int loadAPPConf( void )
{

//WINAPPCONF에서 처리 함
	return ERR_SUCCESS;
}

pAPP_CONF GetAPPConf( void )
{

	return &g_app_conf;
}

int SetAPPConf( pAPP_CONF ptrAPPConf )
{
	//WINAPPCONF에서 처리 함
	( void )ptrAPPConf;
	return ERR_SUCCESS;
}

/*====================================================================================
Window 사용시 설정파일 경로처리 
=====================================================================================*/
int SetWinCurPath( char * ptrpath )
{
	memset(g_wincurpath, 0, sizeof( g_wincurpath ));
	if ( ptrpath != NULL && strlen(ptrpath) > 1)
	{
		memcpy( g_wincurpath, ptrpath, strlen(ptrpath));
	}

	return ERR_SUCCESS;
}

int GetWinCurPath( char * ptrpath, char * ptrfilename )
{
	if( ptrpath != NULL && ptrfilename != NULL )
	{
		if ( strlen(g_wincurpath) > 0 )
		{
			sprintf( ptrpath, "%s\\%s", g_wincurpath, ptrfilename );
		}
		else
		{
			sprintf( ptrpath, "%s", ptrfilename );
		}
	}

	return ERR_SUCCESS;
}
