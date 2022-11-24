#include <net/main.h>
#include <net/http/main.h>
#include <net/websocket/main.h>

namespace net {

ILog4SM* log4sm = nullptr;
IJSON* json = nullptr;

namespace logger {

ILogger* net = nullptr;
ILogger* net_http = nullptr;
ILogger* net_websocket = nullptr;

} // namespace logger

} // namespace net

bool net::SDK_OnLoad(char* error, size_t maxlength, bool late)
{
	sharesys->AddDependency(myself, "log4sm.ext", true, true);
	sharesys->AddDependency(myself, "json.ext", true, true);
	SM_GET_IFACE(LOG4SM, log4sm);
	logger::net = log4sm->GetManager()->CreateLogger(false, "Net");
	SM_GET_IFACE(JSON, json);

	http::SDK_OnLoad(error, maxlength, late);
	websocket::SDK_OnLoad(error, maxlength, late);
	return true;
}

void net::SDK_OnUnload()
{
	websocket::SDK_OnUnload();
	http::SDK_OnUnload();
	delete logger::net;
}