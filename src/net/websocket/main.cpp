#include <net/websocket/main.h>
#include <net/websocket/native.h>
#include <net/websocket/sm_forward.h>

bool net::websocket::SDK_OnLoad(char* error, size_t maxlen, bool late)
{
	logger::net_websocket = log4sm->GetManager()->CreateLogger(false, "Net.WebSocket");
	sharesys->AddNatives(myself, g_WebSocketNative);

	forward::OnMessageReceived = forwards->CreateForward("OnWebSocketMessageReceived", ET_Ignore, 1, nullptr, Param_Cell);

	if (!forward::OnMessageReceived)
	{
		logger::net_websocket->Error("Failed to create forward OnWebSocketMessageReceived.");
		return false;
	}

	return true;
}

void net::websocket::SDK_OnUnload()
{
	forwards->ReleaseForward(forward::OnMessageReceived);
	delete logger::net_websocket;
}