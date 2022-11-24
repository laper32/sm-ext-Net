#ifndef _INCLUDE_SOURCEMOD_EXTENSION_PROPER_H_
#define _INCLUDE_SOURCEMOD_EXTENSION_PROPER_H_

#include "smsdk_ext.h"

#undef min
#undef max
#undef clamp
#undef schema

class Extension : public SDKExtension
{
public:
	virtual bool SDK_OnLoad(char *error, size_t maxlen, bool late);
	virtual void SDK_OnUnload();
};

#endif // _INCLUDE_SOURCEMOD_EXTENSION_PROPER_H_