#ifndef _NET_HTTP_REQUEST_H
#define _NET_HTTP_REQUEST_H

#pragma once

#include <net/http/message.h>

namespace net::http {

namespace detail {

template<bool UseSSL, typename CompletionHandler>
class Session;

template<typename CompletionHandler>
class Session<false, CompletionHandler> :
	public std::enable_shared_from_this<Session<false, CompletionHandler>>
{
	boost::asio::io_context& _ctx;
	CompletionHandler _completion_handler;
	tcp::resolver _resolver;
	boost::beast::tcp_stream _stream;
	boost::beast::flat_buffer _buffer;
	Request _req;
	Response _res;

public:
	explicit Session(boost::asio::io_context& ctx, CompletionHandler&& handler) :
		_ctx(ctx),
		_completion_handler(std::move(handler)),
		_resolver(ctx),
		_stream(ctx)
	{

	}

	void Run(Request& req)
	{
		_req = std::move(req);
		_resolver.async_resolve(_req.url.host(), _req.url.port(),
			std::bind(&Session::OnResolve, this->shared_from_this(), std::placeholders::_1, std::placeholders::_2)
		);
	}

	void OnResolve(boost::system::error_code ec, tcp::resolver::results_type results)
	{
		if (ec)
		{
			return Fail(ec, "OnResolve");
		}
		_stream.expires_after(std::chrono::seconds(_req.timeout));
		_stream.async_connect(results, std::bind(&Session::OnConnect, this->shared_from_this(), std::placeholders::_1));
	}

	void OnConnect(boost::system::error_code ec)
	{
		if (ec)
		{
			return Fail(ec, "OnConnect");
		}

		_stream.expires_after(std::chrono::seconds(_req.timeout));
		http::async_write(
			_stream, _req.data,
			std::bind(&Session::OnWrite, this->shared_from_this(), std::placeholders::_1, std::placeholders::_2)
		);
	}

	void OnWrite(boost::system::error_code ec, std::size_t)
	{
		if (ec)
		{
			return Fail(ec, "OnWrite");
		}

		// Receive the HTTP response
		http::async_read(_stream, _buffer, _res,
			std::bind(&Session::OnRead, this->shared_from_this(), std::placeholders::_1, std::placeholders::_2)
		);
	}

	void OnRead(boost::system::error_code ec, std::size_t)
	{
		if (ec)
		{
			return Fail(ec, "OnRead");
		}
		Done();

		_stream.socket().shutdown(tcp::socket::shutdown_both, ec);
	}

	void Fail(boost::system::error_code ec, std::string_view what)
	{
		auto executor = boost::asio::get_associated_executor(_completion_handler);
		boost::asio::dispatch(
			executor,
			[completion_handler = std::move(_completion_handler), ec]() mutable
			{
				completion_handler(ec, Response());
			}
		);
	}

	void Done()
	{
		auto executor = boost::asio::get_associated_executor(_completion_handler);
		boost::asio::dispatch(
			executor,
			[completion_handler = std::move(_completion_handler), res = std::move(_res)]() mutable
			{
				completion_handler(boost::system::error_code(), res);
			}
		);
	}
};

template<typename CompletionHandler>
class Session<true, CompletionHandler> :
	public std::enable_shared_from_this<Session<true, CompletionHandler>>
{
	boost::asio::io_context& _ctx;
	CompletionHandler _completion_handler;
	tcp::resolver _resolver;
	boost::asio::ssl::context _ssl_ctx{ boost::asio::ssl::context::sslv23_client };
	boost::beast::ssl_stream<boost::beast::tcp_stream> _stream;
	//boost::asio::ssl::stream<tcp::socket> _stream;
	boost::beast::flat_buffer _buffer;
	Request _req;
	Response _res;

public:
	explicit Session(boost::asio::io_context& ctx, CompletionHandler&& handler) :
		_ctx(ctx),
		_completion_handler(std::move(handler)),
		_resolver(ctx),
		_stream(ctx, _ssl_ctx)
	{

	}

	void Run(Request& req)
	{
		_req = std::move(req);

		if (!SSL_set_tlsext_host_name(_stream.native_handle(), _req.url.host().c_str()))
		{
			boost::system::error_code ec{ static_cast<int>(::ERR_get_error()), boost::asio::error::get_ssl_category() };
			return Fail(ec, "ssl");
		}
		return _resolver.async_resolve(
			_req.url.host(), _req.url.port(),
			std::bind(&Session::OnResolve, this->shared_from_this(), std::placeholders::_1, std::placeholders::_2)
		);
	}

	void OnResolve(boost::system::error_code ec, tcp::resolver::results_type results)
	{
		if (ec)
		{
			return Fail(ec, "OnResolve");
		}

		_stream.next_layer().async_connect(results, std::bind(&Session::OnConnect, this->shared_from_this(), std::placeholders::_1));
		//boost::asio::async_connect(
		//	_stream.next_layer(),
		//	results.begin(), results.end(),
		//	std::bind(&Session::OnConnect, this->shared_from_this(), std::placeholders::_1)
		//);
	}

	void OnConnect(boost::system::error_code ec)
	{
		if (ec)
		{
			return Fail(ec, "OnConnect");
		}

		_stream.async_handshake(
			boost::asio::ssl::stream_base::client,
			std::bind(&Session::OnHandshake, this->shared_from_this(), std::placeholders::_1)
		);
	}

	void OnHandshake(boost::system::error_code ec)
	{
		if (ec)
		{
			return Fail(ec, "OnHandshake");
		}

		http::async_write(_stream, _req.data,
			std::bind(&Session::OnWrite, this->shared_from_this(), std::placeholders::_1, std::placeholders::_2)
		);
	}

	void OnWrite(boost::system::error_code ec, std::size_t)
	{
		if (ec)
		{
			return Fail(ec, "OnWrite");
		}

		http::async_read(_stream, _buffer, _res,
			std::bind(&Session::OnRead, this->shared_from_this(), std::placeholders::_1, std::placeholders::_2)
		);
	}

	void OnRead(boost::system::error_code ec, std::size_t)
	{
		if (ec)
		{
			return Fail(ec, "OnRead");
		}

		Done();

		_stream.async_shutdown(std::bind(&Session::OnShutdown, this->shared_from_this(), std::placeholders::_1));
	}

	void OnShutdown(boost::system::error_code ec)
	{
		if (ec == boost::asio::error::eof)
		{
			ec.assign(0, ec.category());
		}
	}

	void Fail(boost::system::error_code ec, std::string_view what)
	{
		auto executor = boost::asio::get_associated_executor(_completion_handler);
		boost::asio::dispatch(
			executor,
			[completion_handler = std::move(_completion_handler), ec]() mutable
			{
				completion_handler(ec, Response());
			}
		);
	}

	void Done()
	{
		auto executor = boost::asio::get_associated_executor(_completion_handler);
		boost::asio::dispatch(
			executor,
			[completion_handler = std::move(_completion_handler), res = std::move(_res)]() mutable
			{
				completion_handler(boost::system::error_code(), res);
			}
		);
	}
};

template<bool UseSSL, typename CompletionToken>
auto MakeRequest(boost::asio::io_context& ctx, Request& req, CompletionToken&& token)
{
	return boost::asio::async_initiate<CompletionToken, void(boost::system::error_code, Response)>(
		[&ctx, &req](auto&& completion_handler)
		{
			std::make_shared<Session<UseSSL, std::decay_t<decltype(completion_handler)>>>(
				ctx, std::forward<decltype(completion_handler)>(completion_handler))->Run(req);
		}, std::forward<CompletionToken>(token));
}

} // namespace detail

using detail::MakeRequest;

} // namespace net::http


#endif // !_NET_HTTP_REQUEST_H
