#include <net/http/native.h>
#include <net/http/sm_handler.h>
#include <net/http/request.h>
#include <common/interop.h>

#include <fstream>

sp_nativeinfo_t net::http::g_HTTPNative[] = {
	{ "HTTPRequest.HTTPRequest", api::CreateRequest },
	{ "HTTPRequest.SetURL", api::SetURL },
	{ "HTTPRequest.SetMethod", api::SetMethod },
	{ "HTTPRequest.SetHeader", api::SetHeader },
	{ "HTTPRequest.SetBodyText", api::SetRequestBodyText },
	{ "HTTPRequest.SetBodyJSON", api::SetRequestBodyJSON },
	{ "HTTPRequest.AppendFileToBody", api::AppendFileToRequestBody },
	{ "HTTPRequest.Prepare", api::PrepareRequest },
	{ "HTTPRequest.Send", api::SendRequest },
	{ "HTTPRequest.SendAsync", api::SendRequestAsync },
	{ "HTTPRequest.HTTPVersion.set", api::SetVersion },

	{ "HTTPResponse.StatusCode.get", api::GetStatusCode },
	{ "HTTPResponse.Length.get", api::GetResponseLength },
	{ "HTTPResponse.GetBodyAsText", api::GetResponseBodyText },
	{ "HTTPResponse.GetBodyAsJSON", api::GetResponseBodyJSON },
	{ "HTTPResponse.GetBodyAsFile", api::GetResponseBodyFile },

	{ nullptr, nullptr }
};

cell_t net::http::api::CreateRequest(IPluginContext* pContext, const cell_t* params)
{
	auto req = new Request();
	HandleError ec = HandleError_None;
	auto hndl = handlesys->CreateHandle(hndl::hRequest, req, pContext->GetIdentity(), myself->GetIdentity(), &ec);
	if (ec != HandleError_None)
	{
		delete req;
		pContext->ReportError("Error occured when creating handle. Error code: #%d", ec);
		return BAD_HANDLE;
	}
	return hndl;
}

cell_t net::http::api::SetURL(IPluginContext* pContext, const cell_t* params)
{
	Handle_t hndl = static_cast<Handle_t>(params[1]);
	HandleError ec = HandleError_None;
	HandleSecurity sec = HandleSecurity{ nullptr, myself->GetIdentity() };
	Request* raw = nullptr;
	if (ec = handlesys->ReadHandle(hndl, hndl::hRequest, &sec, (void**)&raw); ec != HandleError_None)
	{
		pContext->ReportError("Invalid HTTPRequest handle %x (error code %d)", hndl, ec);
		return -1;
	}
	std::string __url;
	sm::interop::cell2native(pContext, params[2], __url);
	raw->url = __url;
	if (raw->url.path().empty()) {
		raw->url = raw->url.str().append("/");
	}
	raw->data.target(raw->url.path());
	raw->data.set(boost::beast::http::field::host, raw->url.host());
	return 0;
}

cell_t net::http::api::SetMethod(IPluginContext* pContext, const cell_t* params)
{
	Handle_t hndl = static_cast<Handle_t>(params[1]);
	HandleError ec = HandleError_None;
	HandleSecurity sec = HandleSecurity{ nullptr, myself->GetIdentity() };
	Request* raw = nullptr;
	if (ec = handlesys->ReadHandle(hndl, hndl::hRequest, &sec, (void**)&raw); ec != HandleError_None)
	{
		pContext->ReportError("Invalid HTTPRequest handle %x (error code %d)", hndl, ec);
		return -1;
	}

	std::string method;
	sm::interop::cell2native(pContext, params[2], method);
	raw->data.method_string(method);

	return 0;
}

cell_t net::http::api::SetHeader(IPluginContext* pContext, const cell_t* params)
{
	Handle_t hndl = static_cast<Handle_t>(params[1]);
	HandleError ec = HandleError_None;
	HandleSecurity sec = HandleSecurity{ nullptr, myself->GetIdentity() };
	Request* raw = nullptr;
	if (ec = handlesys->ReadHandle(hndl, hndl::hRequest, &sec, (void**)&raw); ec != HandleError_None)
	{
		pContext->ReportError("Invalid HTTPRequest handle %x (error code %d)", hndl, ec);
		return -1;
	}
	std::string key, value;
	sm::interop::cell2native(pContext, params[2], key);
	sm::interop::cell2native(pContext, params[3], value);
	raw->data.set(boost::beast::http::string_to_field(key), value);

	return 0;
}

cell_t net::http::api::SetVersion(IPluginContext* pContext, const cell_t* params)
{
	Handle_t hndl = static_cast<Handle_t>(params[1]);
	HandleError ec = HandleError_None;
	HandleSecurity sec = HandleSecurity{ nullptr, myself->GetIdentity() };
	Request* raw = nullptr;
	if (ec = handlesys->ReadHandle(hndl, hndl::hRequest, &sec, (void**)&raw); ec != HandleError_None)
	{
		pContext->ReportError("Invalid HTTPRequest handle %x (error code %d)", hndl, ec);
		return -1;
	}
	int version = 0;
	sm::interop::cell2native(pContext, params[2], version);
	raw->data.version(version);

	return 0;
}

cell_t net::http::api::SetRequestBodyText(IPluginContext* pContext, const cell_t* params)
{
	Handle_t hndl = static_cast<Handle_t>(params[1]);
	HandleError ec = HandleError_None;
	HandleSecurity sec = HandleSecurity{ nullptr, myself->GetIdentity() };
	Request* raw = nullptr;
	if (ec = handlesys->ReadHandle(hndl, hndl::hRequest, &sec, (void**)&raw); ec != HandleError_None)
	{
		pContext->ReportError("Invalid HTTPRequest handle %x (error code %d)", hndl, ec);
		return -1;
	}
	std::string body;
	sm::interop::cell2native(pContext, params[2], body);
	raw->data.body() = body;
	return 0;
}

cell_t net::http::api::SetRequestBodyJSON(IPluginContext* pContext, const cell_t* params)
{
	Handle_t hndl = static_cast<Handle_t>(params[1]);
	HandleError ec = HandleError_None;
	HandleSecurity sec = HandleSecurity{ nullptr, myself->GetIdentity() };
	Request* raw = nullptr;
	if (ec = handlesys->ReadHandle(hndl, hndl::hRequest, &sec, (void**)&raw); ec != HandleError_None)
	{
		pContext->ReportError("Invalid HTTPRequest handle %x (error code %d)", hndl, ec);
		return -1;
	}
	IBaseJSON* obj = json->GetJSON(static_cast<Handle_t>(params[2]));
	raw->data.body() = obj->Dump();
	return 0;
}

cell_t net::http::api::AppendFileToRequestBody(IPluginContext* pContext, const cell_t* params)
{
	Handle_t hndl = static_cast<Handle_t>(params[1]);
	HandleError ec = HandleError_None;
	HandleSecurity sec = HandleSecurity{ nullptr, myself->GetIdentity() };
	Request* raw = nullptr;
	if (ec = handlesys->ReadHandle(hndl, hndl::hRequest, &sec, (void**)&raw); ec != HandleError_None)
	{
		pContext->ReportError("Invalid HTTPRequest handle %x (error code %d)", hndl, ec);
		return -1;
	}

	return 0;
}

cell_t net::http::api::PrepareRequest(IPluginContext* pContext, const cell_t* params)
{
	Handle_t hndl = static_cast<Handle_t>(params[1]);
	HandleError ec = HandleError_None;
	HandleSecurity sec = HandleSecurity{ nullptr, myself->GetIdentity() };
	Request* raw = nullptr;
	if (ec = handlesys->ReadHandle(hndl, hndl::hRequest, &sec, (void**)&raw); ec != HandleError_None)
	{
		pContext->ReportError("Invalid HTTPRequest handle %x (error code %d)", hndl, ec);
		return -1;
	}
	raw->data.set(detail::http::field::accept, "*/*");
	raw->data.set(detail::http::field::user_agent, SMEXT_CONF_VERSION_STRING);
	raw->data.prepare_payload();

	return 0;
}

cell_t net::http::api::SendRequest(IPluginContext* pContext, const cell_t* params)
{
	Handle_t hndl = 0;
	HandleError ec = {};
	HandleSecurity sec = HandleSecurity{ nullptr, myself->GetIdentity() };

	try
	{
		hndl = static_cast<Handle_t>(params[1]);
		
		Request* raw = nullptr;
		if (ec = handlesys->ReadHandle(hndl, hndl::hRequest, &sec, (void**)&raw); ec != HandleError_None)
		{
			throw std::runtime_error(fmt::format("Invalid HTTPRequest handle {:#x} (error code: {})", hndl, ec));
		}

		std::ostringstream oss;
		oss << raw->data;
		META_CONPRINTF("RAW DATA: %s\n", oss.str().c_str());

		bool use_ssl = false;
		sm::interop::cell2native(pContext, params[2], use_ssl);
		sm::interop::cell2native(pContext, params[3], raw->timeout);

		std::future<Response> res = {};

		// This is a workaround for isolating local sync wait and global async operation.
		// We can't use global io_context, since this will stuck the whole thread.
		// Thus, depends on this, we then construct an extra local context to handle the running.
		boost::asio::io_context local_ctx;

		if (use_ssl)
		{
			res = MakeRequest<true>(local_ctx, *raw, boost::asio::use_future);
		}
		else
		{
			res = MakeRequest<false>(local_ctx, *raw, boost::asio::use_future);
		}
		local_ctx.run();

		auto pRes = new Response(res.get());
		Handle_t hRes = handlesys->CreateHandle(hndl::hResponse, pRes, pContext->GetIdentity(), myself->GetIdentity(), &ec);
		if (ec != HandleError_None)
		{
			delete pRes;
			throw std::runtime_error(fmt::format("Error occured when creating handle. Error code: {}", ec));
		}

		handlesys->FreeHandle(hndl, &sec);

		return hRes;
	}
	catch (const std::exception& ex)
	{
		handlesys->FreeHandle(hndl, &sec);
		pContext->ReportError("%s", ex.what());
		return 0;
	}
}

cell_t net::http::api::SendRequestAsync(IPluginContext* pContext, const cell_t* params)
{
	boost::asio::co_spawn(ext_ctx,
		[pContext = std::move(pContext), params = std::move(params)]() mutable -> boost::asio::awaitable<void>
		{
			Handle_t hndl;
			HandleSecurity sec = HandleSecurity{ nullptr, myself->GetIdentity() };
			try
			{
				hndl = static_cast<Handle_t>(params[1]);
				HandleError ec = HandleError_None;
				Request* raw = nullptr;
				if (ec = handlesys->ReadHandle(hndl, hndl::hRequest, &sec, (void**)&raw); ec != HandleError_None)
				{
					throw std::runtime_error(fmt::format("Invalid HTTPRequest handle {:#x} (error code: {})", hndl, ec));
				}

				bool use_ssl = false;
				sm::interop::cell2native(pContext, params[3], use_ssl);
				sm::interop::cell2native(pContext, params[4], raw->timeout);

				IPluginFunction* pFn = pContext->GetFunctionById(params[2]);
				if (!pFn)
				{
					throw std::runtime_error("Function is null.");
				}

				Response res = {};

				if (use_ssl)
				{
					res = co_await MakeRequest<true>(ext_ctx, *raw, boost::asio::use_awaitable);
				}
				else
				{
					res = co_await MakeRequest<false>(ext_ctx, *raw, boost::asio::use_awaitable);
				}

				auto pRes = new Response(res);
				Handle_t hRes = handlesys->CreateHandle(hndl::hResponse, pRes, pContext->GetIdentity(), myself->GetIdentity(), &ec);
				if (ec != HandleError_None)
				{
					throw std::runtime_error(fmt::format("Error occured when creating handle. Error code: {}", ec));
				}

				pFn->PushCell(hRes);

				pFn->Execute(nullptr);

				if (ec = handlesys->FreeHandle(hRes, &sec); ec != HandleError_None)
				{
					throw std::runtime_error(fmt::format("Failed to close response handle {:#x} (error code: {})", hndl, ec));
				}

				handlesys->FreeHandle(hndl, &sec);
			}
			catch (const std::exception& ex)
			{
				handlesys->FreeHandle(hndl, &sec);
				pContext->ReportError("%s", ex.what());
			}
		}, boost::asio::detached);
	return 0;
}

cell_t net::http::api::GetStatusCode(IPluginContext* pContext, const cell_t* params)
{
	Handle_t hndl = static_cast<Handle_t>(params[1]);
	HandleError ec = HandleError_None;
	HandleSecurity sec = HandleSecurity{ nullptr, myself->GetIdentity() };
	Response* raw = nullptr;
	if (ec = handlesys->ReadHandle(hndl, hndl::hResponse, &sec, (void**)&raw); ec != HandleError_None)
	{
		pContext->ReportError("Invalid HTTPResponse handle %x (error code %d)", hndl, ec);
		return -1;
	}

	return raw->result_int();
}

cell_t net::http::api::GetResponseLength(IPluginContext* pContext, const cell_t* params)
{
	Handle_t hndl = static_cast<Handle_t>(params[1]);
	HandleError ec = HandleError_None;
	HandleSecurity sec = HandleSecurity{ nullptr, myself->GetIdentity() };
	Response* raw = nullptr;
	if (ec = handlesys->ReadHandle(hndl, hndl::hResponse, &sec, (void**)&raw); ec != HandleError_None)
	{
		pContext->ReportError("Invalid HTTPResponse handle %x (error code %d)", hndl, ec);
		return -1;
	}

	return 0;
}

cell_t net::http::api::GetResponseBodyText(IPluginContext* pContext, const cell_t* params)
{
	Handle_t hndl = static_cast<Handle_t>(params[1]);
	HandleError ec = HandleError_None;
	HandleSecurity sec = HandleSecurity{ nullptr, myself->GetIdentity() };
	Response* raw = nullptr;
	if (ec = handlesys->ReadHandle(hndl, hndl::hResponse, &sec, (void**)&raw); ec != HandleError_None)
	{
		pContext->ReportError("Invalid HTTPResponse handle %x (error code %d)", hndl, ec);
		return -1;
	}
	//META_CONPRINTF("%s\n", raw->body().c_str());
	sm::interop::native2cell(pContext, raw->body(), params[2], params[3]);
	return 0;
}

cell_t net::http::api::GetResponseBodyJSON(IPluginContext* pContext, const cell_t* params)
{
	try
	{
		Handle_t hndl = static_cast<Handle_t>(params[1]);
		HandleError ec = HandleError_None;
		HandleSecurity sec = HandleSecurity{ nullptr, myself->GetIdentity() };
		Response* raw = nullptr;
		if (ec = handlesys->ReadHandle(hndl, hndl::hResponse, &sec, (void**)&raw); ec != HandleError_None)
		{
			throw std::runtime_error(fmt::format("Invalid HTTPResponse handle {:#x} (Error code: {})", hndl, ec));
		}
		auto test_result = json->GetClassType(raw->body().c_str());
		if (test_result == Type_Object)
		{
			IJSONObject* obj = json->CreateObject();
			obj->ImportFromString(raw->body().c_str());
			Handle_t ret = json->MakeHandle(obj);
			if (ret == BAD_HANDLE)
			{
				delete obj;
			}
			return ret;
		}
		if (test_result == Type_Array)
		{
			IJSONArray* obj = json->CreateArray();
			obj->ImportFromString(raw->body().c_str());
			Handle_t ret = json->MakeHandle(obj);
			if (ret == BAD_HANDLE)
			{
				delete obj;
			}
			return ret;
		}
		else
		{
			throw std::runtime_error(fmt::format("Cannot convert the body to JSON."));
		}
	}
	catch (const std::exception& ex)
	{
		pContext->ReportError("%s", ex.what());
		return BAD_HANDLE;
	}
}

cell_t net::http::api::GetResponseBodyFile(IPluginContext* pContext, const cell_t* params)
{
	try
	{
		Handle_t hndl = static_cast<Handle_t>(params[1]);
		HandleError ec = HandleError_None;
		HandleSecurity sec = HandleSecurity{ nullptr, myself->GetIdentity() };
		Response* raw = nullptr;
		if (ec = handlesys->ReadHandle(hndl, hndl::hResponse, &sec, (void**)&raw); ec != HandleError_None)
		{
			throw std::runtime_error(fmt::format("Invalid HTTPResponse handle {:#x} (Error code: {})", hndl, ec));
		}

		std::string dir_to_save;
		sm::interop::cell2native(pContext, params[2], dir_to_save);
		char path[256] = {};
		g_pSM->BuildPath(Path_Game, path, sizeof(path), "%s", dir_to_save.c_str());

		std::ofstream ofs(path, std::ios::binary);
		ofs.write(raw->body().c_str(), raw->body().size());
		ofs.close();

		return 0;
	}
	catch (const std::exception& ex)
	{
		pContext->ReportError("%s", ex.what());
		return 0;
	}
}
