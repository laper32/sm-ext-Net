#include <net/http/main.h>
#include <net/http/native.h>
#include <net/http/sm_handler.h>

bool net::http::SDK_OnLoad(char* error, size_t maxlen, bool late)
{
	logger::net_http = log4sm->GetManager()->CreateLogger(false, "Net.HTTP");

	sharesys->AddNatives(myself, g_HTTPNative);

	HandleAccess sec;
	handlesys->InitAccessDefaults(nullptr, &sec);
	sec.access[HandleAccess_Delete] = 0;

	hndl::hRequest = handlesys->CreateType("HTTPRequest", &hndl::sRequestHandler, 0, nullptr, &sec, myself->GetIdentity(), nullptr);
	hndl::hResponse = handlesys->CreateType("HTTPResponse", &hndl::sResponseHandler, 0, nullptr, &sec, myself->GetIdentity(), nullptr);

	return true;
}

void net::http::SDK_OnUnload()
{
	handlesys->RemoveType(hndl::hRequest, myself->GetIdentity());
	handlesys->RemoveType(hndl::hResponse, myself->GetIdentity());
	delete logger::net_http;
}
