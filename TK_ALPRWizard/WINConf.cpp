#include "StdAfx.h"

#include "WINConf.h"
#include "../include/TK_CONF.h"

CWINConf::CWINConf(void)
{
	memset( &m_app_conf, 0, sizeof( APP_CONF_t));
}

CWINConf::~CWINConf(void)
{
}


void CWINConf::McsToWcs( char * Org, TCHAR * Dest, int OrgSize)
{
	USES_CONVERSION;	
	wcscpy_s( Dest, OrgSize + 1 , A2W( Org) );	
}

void CWINConf::WcsToMcs( TCHAR * Org, char * Dest, int OrgSize)
{
	USES_CONVERSION;
	strcpy_s( Dest, OrgSize + 1, W2A( Org) );
}

pAPP_CONF CWINConf::GetWinAPPCONF( void )
{
	TCHAR tcCurPath[ MAX_PATH ];
	CString strConf;
	TCHAR strPath[ MAX_PATH ];
	TCHAR strAppShow[ MAX_PATH ];
	char szTemp[ MAX_PATH ];
	
	memset( tcCurPath, 0, sizeof( tcCurPath));
	memset( strPath, 0, sizeof( strPath));
	memset( strAppShow, 0, sizeof( strAppShow));


	m_WizardHelper.GetCurrentPath( tcCurPath );

	strConf.Format(L"%s\\%s",tcCurPath, _T(TK_APP_CONF_DATA ));

	GetPrivateProfileString(_T("APP"),_T( APP_KEY_SHOW_LOG ),_T(""),strAppShow, sizeof( strAppShow) ,strConf);
	GetPrivateProfileString(_T("APP"),_T( APP_KEY_IMG_PATH),_T(""),strPath, sizeof( strPath ) ,strConf);

	if (wcslen( strAppShow ) == 0)
	{
		WritePrivateProfileString(_T("APP"),_T(APP_KEY_SHOW_LOG),_T("0"),strConf);
		m_app_conf.APP_DBG_SHOW_LOG = 0;
	}
	else
	{
		memset( szTemp, 0, sizeof( szTemp));
		WcsToMcs( strAppShow, szTemp, (int)wcslen( strAppShow));
		m_app_conf.APP_DBG_SHOW_LOG = ( BYTE )atoi( szTemp );		
	}

	if (wcslen( strPath ) != 0)
	{
		memset( szTemp, 0, sizeof( szTemp));
		WcsToMcs( strPath, szTemp, (int)wcslen( strPath));
		memset( m_app_conf.IMG_PATH, 0, MAX_PATH);
		memcpy( m_app_conf.IMG_PATH, szTemp, strlen( szTemp ));		
	}
	
	return &m_app_conf;
}

int CWINConf::SetWinAPPCONF( pAPP_CONF ptrAppConf )
{
	TCHAR tcCurPath[ MAX_PATH ];
	CString strConf;
	TCHAR strPath[ MAX_PATH ];
	TCHAR strAppShow[ MAX_PATH ];
	char szTemp[ MAX_PATH ];

	memset( tcCurPath, 0, sizeof( tcCurPath));
	memset( strPath, 0, sizeof( strPath));
	memset( strAppShow, 0, sizeof( strAppShow));


	m_WizardHelper.GetCurrentPath( tcCurPath );
	strConf.Format(L"%s\\%s",tcCurPath, _T(TK_APP_CONF_DATA ));
	

	if ( ptrAppConf->APP_DBG_SHOW_LOG != m_app_conf.APP_DBG_SHOW_LOG )
	{
		memset( szTemp, 0, sizeof( szTemp));
		sprintf_s( szTemp, "%d", ptrAppConf->APP_DBG_SHOW_LOG );
		McsToWcs( szTemp, strAppShow, (int)strlen( szTemp ));
		WritePrivateProfileString(_T("APP"),_T(APP_KEY_SHOW_LOG), strAppShow, strConf);
	}

	if( strcmp(ptrAppConf->IMG_PATH , m_app_conf.IMG_PATH ) != 0 )
	{
		memset( szTemp, 0, sizeof( szTemp));
		sprintf_s( szTemp, "%s", ptrAppConf->IMG_PATH );
		McsToWcs( szTemp, strPath, (int)strlen( szTemp ));
		WritePrivateProfileString(_T("APP"),_T(APP_KEY_IMG_PATH),strPath ,strConf);
	}

	return ERR_SUCCESS;
}