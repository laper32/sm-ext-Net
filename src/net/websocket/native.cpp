#include <net/websocket/ws.h>
#include <net/websocket/native.h>
#include <net/websocket/sm_forward.h>
#include <common/interop.h>
#include <common/sm_func.hpp>

bool connected = false;

sp_nativeinfo_t net::websocket::g_WebSocketNative[] = {
    { "WebSocket.Init", api::Init },
    { "WebSocket.UseSSL", api::UseSSL },
    { "WebSocket.SetURL", api::SetURL },
    { "WebSocket.Connect", api::Connect },
    { "WebSocket.Connected", api::Connected },
    { "WebSocket.Disconnect", api::Disconnect },
    { "WebSocket.Send", api::Send },
    { nullptr, nullptr }
};

cell_t net::websocket::api::Init(IPluginContext* pContext, const cell_t* params)
{
    cl_ssl.auto_reconnect(true);
    cl_ssl.bind_init(
        [&]()
        {
            cl_ssl.ws_stream().text(true);
            cl_ssl.ws_stream().set_option(
                ::websocket::stream_base::decorator(
                    [](::websocket::request_type& req)
                    {
                        req.set(http::field::authorization, SMEXT_CONF_NAME);
                    }
                )
            );
        }
    ).bind_connect(
        [&]()
        {
            auto& last_error = asio2::get_last_error();
            if (last_error)
            {
                logger::net_websocket->Error(fmt::format("{}: {}", last_error.value(), last_error.message()).c_str());
            }
        }
    ).bind_upgrade(
        [&]()
        {
            auto& last_error = asio2::get_last_error();
            if (last_error)
            {
                logger::net_websocket->Error(fmt::format("{}: {}", last_error.value(), last_error.message()).c_str());
            }
            else
            {
                logger::net_websocket->Info("Connected to the server.");
                connected = true;
            }
        }
    ).bind_disconnect(
        [&]()
        {
            connected = false;
            logger::net_websocket->Info("Disconnected from the backend.");
        }
    ).bind_recv(
        [&](std::string_view data)
        {
            WrapSMFunction(
                [&](auto&& data, auto&& pContext, auto&& params) {
                    std::string result(data);
                    auto type = json->GetClassType(result.c_str());
                    if (type == Type_Object)
                    {
                        IJSONObject* obj = json->CreateObject();
                        obj->ImportFromString(result.c_str());
                        Handle_t hndl = json->MakeHandle(obj);
                        if (hndl != BAD_HANDLE)
                        {
                            forward::OnMessageReceived->PushCell(hndl);
                            forward::OnMessageReceived->Execute(nullptr);
                            json->FreeHandle(hndl);
                        }
                    }
                    else if (type == Type_Array)
                    {
                        IJSONArray* obj = json->CreateArray();
                        obj->ImportFromString(result.c_str());
                        Handle_t hndl = json->MakeHandle(obj);
                        if (hndl != BAD_HANDLE)
                        {
                            forward::OnMessageReceived->PushCell(hndl);
                            forward::OnMessageReceived->Execute(nullptr);
                            json->FreeHandle(hndl);
                        }
                    }
                }, data, pContext, params
            );
        }
    );

    cl.auto_reconnect(true);
    cl.bind_init(
        [&]()
        {
            cl.ws_stream().text(true);
            cl.ws_stream().set_option(
                ::websocket::stream_base::decorator(
                    [](::websocket::request_type& req)
                    {
                        req.set(http::field::authorization, SMEXT_CONF_NAME);
                    }
                )
            );
        }
    ).bind_connect(
        [&]()
        {
            auto& last_error = asio2::get_last_error();
            if (last_error)
            {
                logger::net_websocket->Error(fmt::format("{}: {}", last_error.value(), last_error.message()).c_str());
            }
        }
    ).bind_upgrade(
        [&]()
        {
            auto& last_error = asio2::get_last_error();
            if (last_error)
            {
                logger::net_websocket->Error(fmt::format("{}: {}", last_error.value(), last_error.message()).c_str());
            }
            else
            {
                logger::net_websocket->Info("Connected to the server.");
                connected = true;
            }
        }
    ).bind_disconnect(
        [&]()
        {
            connected = false;
            logger::net_websocket->Info("Disconnected from the backend.");
        }
    ).bind_recv(
        [&](std::string_view data)
        {
            WrapSMFunction(
                [&](auto&& data, auto&& pContext, auto&& params) {
                    std::string result(data);
                    auto type = json->GetClassType(result.c_str());
                    if (type == Type_Object)
                    {
                        IJSONObject* obj = json->CreateObject();
                        obj->ImportFromString(result.c_str());
                        Handle_t hndl = json->MakeHandle(obj);
                        if (hndl != BAD_HANDLE)
                        {
                            forward::OnMessageReceived->PushCell(hndl);
                            forward::OnMessageReceived->Execute(nullptr);
                            json->FreeHandle(hndl);
                        }
                    }
                    else if (type == Type_Array)
                    {
                        IJSONArray* obj = json->CreateArray();
                        obj->ImportFromString(result.c_str());
                        Handle_t hndl = json->MakeHandle(obj);
                        if (hndl != BAD_HANDLE)
                        {
                            forward::OnMessageReceived->PushCell(hndl);
                            forward::OnMessageReceived->Execute(nullptr);
                            json->FreeHandle(hndl);
                        }
                    }
                }, data, pContext, params
            );
        }
    );
    return 0;
}

cell_t net::websocket::api::UseSSL(IPluginContext* pContext, const cell_t* params)
{
    sm::interop::cell2native(pContext, params[1], use_ssl);
    return 0;
}

cell_t net::websocket::api::SetURL(IPluginContext* pContext, const cell_t* params)
{
    std::string __url;
    sm::interop::cell2native(pContext, params[1], __url);
    url = __url;
    if (url.path().empty())
    {
        url = url.str().append("/");
    }
    return 0;
}

cell_t net::websocket::api::Connect(IPluginContext* pContext, const cell_t* params)
{
    if (use_ssl)
    {
        cl_ssl.start(url.host(), url.port(), url.path());
    }
    else
    {
        cl.start(url.host(), url.port(), url.path());
    }
    return 0;
}

cell_t net::websocket::api::Connected(IPluginContext* pContext, const cell_t* params)
{
    return connected;
}

cell_t net::websocket::api::Disconnect(IPluginContext* pContext, const cell_t* params)
{
    if (use_ssl)
    {
        cl_ssl.stop();
    }
    else
    {
        cl.stop();
    }
    return 0;
}

cell_t net::websocket::api::Send(IPluginContext* pContext, const cell_t* params)
{
    Handle_t hndl = static_cast<Handle_t>(params[1]);
    IBaseJSON* raw = json->GetJSON(hndl);
    if (!raw)
    {
        pContext->ReportError("%s", fmt::format("Retrieved JSON is nullptr.").c_str());
        return -1;
    }

    if (use_ssl)
    {
        cl_ssl.async_send(raw->Dump());
    }
    else
    {
        cl.async_send(raw->Dump());
    }
    json->FreeHandle(hndl);

    return 0;
}

