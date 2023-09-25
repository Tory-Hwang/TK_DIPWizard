#pragma once

#include "../include/TK_TYPE.h"
#include "WizardHelper.h"

class CWINConf
{
public:
	CWINConf(void);
	~CWINConf(void);

	pAPP_CONF GetWinAPPCONF( void );
	int SetWinAPPCONF( pAPP_CONF ptrAppConf );
private:
	void WcsToMcs( TCHAR * Org, char * Dest, int OrgSize);
	void McsToWcs( char * Org, TCHAR * Dest, int OrgSize);

	CWizardHelper m_WizardHelper;
	APP_CONF_t m_app_conf;
};
