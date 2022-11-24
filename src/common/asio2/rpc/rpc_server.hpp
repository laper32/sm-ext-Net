/*
 * COPYRIGHT (C) 2017-2021, zhllxt
 *
 * author   : zhllxt
 * email    : 37792738@qq.com
 * 
 * Distributed under the GNU GENERAL PUBLIC LICENSE Version 3, 29 June 2007
 * (See accompanying file LICENSE or see <http://www.gnu.org/licenses/>)
 */

#ifndef __ASIO2_RPC_SERVER_HPP__
#define __ASIO2_RPC_SERVER_HPP__

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
#pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#if __has_include(<cereal/cereal.hpp>)

#include <asio2/base/detail/push_options.hpp>

#include <asio2/config.hpp>

#include <asio2/tcp/tcp_server.hpp>
#include <asio2/tcp/tcps_server.hpp>
#include <asio2/http/ws_server.hpp>
#include <asio2/http/wss_server.hpp>

#include <asio2/rpc/rpc_session.hpp>

namespace asio2::detail
{
	ASIO2_CLASS_FORWARD_DECLARE_BASE;
	ASIO2_CLASS_FORWARD_DECLARE_TCP_BASE;
	ASIO2_CLASS_FORWARD_DECLARE_TCP_SERVER;

	template<class derived_t, class executor_t>
	class rpc_server_impl_t
		: public executor_t
		, public rpc_invoker_t<typename executor_t::session_type, typename executor_t::session_type::args_type>
	{
		friend executor_t;

		ASIO2_CLASS_FRIEND_DECLARE_BASE;
		ASIO2_CLASS_FRIEND_DECLARE_TCP_BASE;
		ASIO2_CLASS_FRIEND_DECLARE_TCP_SERVER;

	public:
		using super = executor_t;
		using self  = rpc_server_impl_t<derived_t, executor_t>;

		using executor_type = executor_t;
		using session_type  = typename super::session_type;

	protected:
		using super::async_send;
	
	public:
		/**
		 * @brief constructor
		 */
		template<class ...Args>
		explicit rpc_server_impl_t(
			Args&&... args
		)
			: super(std::forward<Args>(args)...)
			, rpc_invoker_t<typename executor_t::session_type, typename executor_t::session_type::args_type>()
		{
		}

		/**
		 * @brief destructor
		 */
		~rpc_server_impl_t()
		{
			this->stop();
		}

		/**
		 * @brief start the server
		 * @param host - A string identifying a location. May be a descriptive name or
		 * a numeric address string.
		 * @param service - A string identifying the requested service. This may be a
		 * descriptive name or a numeric string corresponding to a port number.
		 */
		template<typename String, typename StrOrInt, typename... Args>
		inline bool start(String&& host, StrOrInt&& service, Args&&... args)
		{
			if constexpr (is_websocket_server<executor_t>::value)
			{
				return executor_t::template start(
					std::forward<String>(host), std::forward<StrOrInt>(service),
					std::forward<Args>(args)...);
			}
			else
			{
				return executor_t::template start(
					std::forward<String>(host), std::forward<StrOrInt>(service),
					asio2::use_dgram, std::forward<Args>(args)...);
			}
		}

	public:
		/**
		 * @brief call a rpc function for each session
		 */
		template<class return_t, class Rep, class Period, class ...Args>
		inline void call(std::chrono::duration<Rep, Period> timeout, const std::string& name, const Args&... args)
		{
			this->sessions_.for_each([&](std::shared_ptr<session_type>& session_ptr) mutable
			{
				session_ptr->template call<return_t>(timeout, name, args...);
			});
		}

		/**
		 * @brief call a rpc function for each session
		 */
		template<class return_t, class Rep, class Period, class ...Args>
		inline void call(error_code& ec, std::chrono::duration<Rep, Period> timeout,
			const std::string& name, const Args&... args)
		{
			this->sessions_.for_each([&](std::shared_ptr<session_type>& session_ptr) mutable
			{
				session_ptr->template call<return_t>(ec, timeout, name, args...);
			});
		}

		/**
		 * @brief call a rpc function for each session
		 */
		template<class return_t, class ...Args>
		inline void call(const std::string& name, const Args&... args)
		{
			this->sessions_.for_each([&](std::shared_ptr<session_type>& session_ptr) mutable
			{
				session_ptr->template call<return_t>(name, args...);
			});
		}

		/**
		 * @brief call a rpc function for each session
		 */
		template<class return_t, class ...Args>
		inline void call(error_code& ec, const std::string& name, const Args&... args)
		{
			this->sessions_.for_each([&](std::shared_ptr<session_type>& session_ptr) mutable
			{
				session_ptr->template call<return_t>(ec, name, args...);
			});
		}

		/**
		 * @brief asynchronous call a rpc function for each session
		 * Callback signature : void(asio::error_code ec, int result)
		 * if result type is void, the Callback signature is : void(asio::error_code ec)
		 * Because the result value type is not specified in the first template parameter,
		 * so the result value type must be specified in the Callback lambda.
		 */
		template<class Callback, class ...Args>
		inline typename std::enable_if_t<is_callable_v<Callback>, void>
		async_call(const Callback& fn, const std::string& name, const Args&... args)
		{
			this->sessions_.for_each([&](std::shared_ptr<session_type>& session_ptr) mutable
			{
				session_ptr->async_call(fn, name, args...);
			});
		}

		/**
		 * @brief asynchronous call a rpc function for each session
		 * Callback signature : void(asio::error_code ec, int result)
		 * if result type is void, the Callback signature is : void(asio::error_code ec)
		 * Because the result value type is not specified in the first template parameter,
		 * so the result value type must be specified in the Callback lambda
		 */
		template<class Callback, class Rep, class Period, class ...Args>
		inline typename std::enable_if_t<is_callable_v<Callback>, void>
		async_call(const Callback& fn, std::chrono::duration<Rep, Period> timeout,
			const std::string& name, const Args&... args)
		{
			this->sessions_.for_each([&](std::shared_ptr<session_type>& session_ptr) mutable
			{
				session_ptr->async_call(fn, timeout, name, args...);
			});
		}

		/**
		 * @brief asynchronous call a rpc function for each session
		 * Callback signature : void(asio::error_code ec, return_t result) the return_t
		 * is the first template parameter.
		 * if result type is void, the Callback signature is : void(asio::error_code ec)
		 */
		template<class return_t, class Callback, class ...Args>
		inline void async_call(const Callback& fn, const std::string& name, const Args&... args)
		{
			this->sessions_.for_each([&](std::shared_ptr<session_type>& session_ptr) mutable
			{
				session_ptr->template async_call<return_t>(fn, name, args...);
			});
		}

		/**
		 * @brief asynchronous call a rpc function for each session
		 * Callback signature : void(asio::error_code ec, return_t result) the return_t
		 * is the first template parameter.
		 * if result type is void, the Callback signature is : void(asio::error_code ec)
		 */
		template<class return_t, class Callback, class Rep, class Period, class ...Args>
		inline void async_call(const Callback& fn, std::chrono::duration<Rep, Period> timeout,
			const std::string& name, const Args&... args)
		{
			this->sessions_.for_each([&](std::shared_ptr<session_type>& session_ptr) mutable
			{
				session_ptr->template async_call<return_t>(fn, timeout, name, args...);
			});
		}

		/**
		 * @brief asynchronous call a rpc function for each session
		 * Don't care whether the call succeeds
		 */
		template<class ...Args>
		inline void async_call(const std::string& name, const Args&... args)
		{
			this->sessions_.for_each([&](std::shared_ptr<session_type>& session_ptr) mutable
			{
				session_ptr->async_call(name, args...);
			});
		}

	protected:
		template<typename... Args>
		inline std::shared_ptr<session_type> _make_session(Args&&... args)
		{
			return super::_make_session(std::forward<Args>(args)..., *this);
		}

	protected:
	};
}

namespace asio2
{
	template<class session_t, asio2::net_protocol np> class rpc_server_t;

	template<class session_t>
	class rpc_server_t<session_t, asio2::net_protocol::tcp> : public detail::rpc_server_impl_t<
		rpc_server_t<session_t, asio2::net_protocol::tcp>, detail::tcp_server_impl_t<
		rpc_server_t<session_t, asio2::net_protocol::tcp>, session_t>>
	{
	public:
		using detail::rpc_server_impl_t<
			rpc_server_t<session_t, asio2::net_protocol::tcp>, detail::tcp_server_impl_t<
			rpc_server_t<session_t, asio2::net_protocol::tcp>, session_t>>::rpc_server_impl_t;
	};

	template<class session_t>
	class rpc_server_t<session_t, asio2::net_protocol::ws> : public detail::rpc_server_impl_t<
		rpc_server_t<session_t, asio2::net_protocol::ws>, detail::ws_server_impl_t<
		rpc_server_t<session_t, asio2::net_protocol::ws>, session_t>>
	{
	public:
		using detail::rpc_server_impl_t<
			rpc_server_t<session_t, asio2::net_protocol::ws>, detail::ws_server_impl_t<
			rpc_server_t<session_t, asio2::net_protocol::ws>, session_t>>::rpc_server_impl_t;
	};

	template<asio2::net_protocol np> class rpc_server_use;

	template<>
	class rpc_server_use<asio2::net_protocol::tcp>
		: public rpc_server_t<rpc_session_use<asio2::net_protocol::tcp>, asio2::net_protocol::tcp>
	{
	public:
		using rpc_server_t<rpc_session_use<asio2::net_protocol::tcp>, asio2::net_protocol::tcp>::rpc_server_t;
	};

	template<>
	class rpc_server_use<asio2::net_protocol::ws>
		: public rpc_server_t<rpc_session_use<asio2::net_protocol::ws>, asio2::net_protocol::ws>
	{
	public:
		using rpc_server_t<rpc_session_use<asio2::net_protocol::ws>, asio2::net_protocol::ws>::rpc_server_t;
	};

#if defined(ASIO2_USE_SSL)
	template<class session_t, asio2::net_protocol np> class rpcs_server_t;

	template<class session_t>
	class rpcs_server_t<session_t, asio2::net_protocol::tcps> : public detail::rpc_server_impl_t<
		rpcs_server_t<session_t, asio2::net_protocol::tcps>, detail::tcps_server_impl_t<
		rpcs_server_t<session_t, asio2::net_protocol::tcps>, session_t>>
	{
	public:
		using detail::rpc_server_impl_t<
			rpcs_server_t<session_t, asio2::net_protocol::tcps>, detail::tcps_server_impl_t<
			rpcs_server_t<session_t, asio2::net_protocol::tcps>, session_t>>::rpc_server_impl_t;
	};

	template<class session_t>
	class rpcs_server_t<session_t, asio2::net_protocol::wss> : public detail::rpc_server_impl_t<
		rpcs_server_t<session_t, asio2::net_protocol::wss>, detail::wss_server_impl_t<
		rpcs_server_t<session_t, asio2::net_protocol::wss>, session_t>>
	{
	public:
		using detail::rpc_server_impl_t<
			rpcs_server_t<session_t, asio2::net_protocol::wss>, detail::wss_server_impl_t<
			rpcs_server_t<session_t, asio2::net_protocol::wss>, session_t>>::rpc_server_impl_t;
	};

	template<asio2::net_protocol np> class rpcs_server_use;

	template<>
	class rpcs_server_use<asio2::net_protocol::tcps>
		: public rpcs_server_t<rpcs_session_use<asio2::net_protocol::tcps>, asio2::net_protocol::tcps>
	{
	public:
		using rpcs_server_t<rpcs_session_use<asio2::net_protocol::tcps>, asio2::net_protocol::tcps>::rpcs_server_t;
	};

	template<>
	class rpcs_server_use<asio2::net_protocol::wss>
		: public rpcs_server_t<rpcs_session_use<asio2::net_protocol::wss>, asio2::net_protocol::wss>
	{
	public:
		using rpcs_server_t<rpcs_session_use<asio2::net_protocol::wss>, asio2::net_protocol::wss>::rpcs_server_t;
	};
#endif

#if !defined(ASIO2_USE_WEBSOCKET_RPC)
	/// Using tcp dgram mode as the underlying communication support
	using rpc_server = rpc_server_use<asio2::net_protocol::tcp>;
#else
	/// Using websocket as the underlying communication support
	using rpc_server = rpc_server_use<asio2::net_protocol::ws>;
#endif

#if defined(ASIO2_USE_SSL)
#if !defined(ASIO2_USE_WEBSOCKET_RPC)
	/// Using tcp dgram mode as the underlying communication support
	using rpcs_server = rpcs_server_use<asio2::net_protocol::tcps>;
#else
	/// Using websocket as the underlying communication support
	using rpcs_server = rpcs_server_use<asio2::net_protocol::wss>;
#endif
#endif
}

#include <asio2/base/detail/pop_options.hpp>

#endif

#endif // !__ASIO2_RPC_SERVER_HPP__
