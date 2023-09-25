
#ifndef __TK_IBRIDGE_H__
#define __TK_IBRIDGE_H__

class CIBridge
{
public:

	virtual int NotifySave() = 0;
	virtual int NotifyClose( UINT uDLGID ) = 0;

};

#endif