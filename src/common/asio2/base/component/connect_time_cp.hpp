/*
 * COPYRIGHT (C) 2017-2021, zhllxt
 *
 * author   : zhllxt
 * email    : 37792738@qq.com
 * 
 * Distributed under the GNU GENERAL PUBLIC LICENSE Version 3, 29 June 2007
 * (See accompanying file LICENSE or see <http://www.gnu.org/licenses/>)
 */

#ifndef __ASIO2_CONNECT_TIME_COMPONENT_HPP__
#define __ASIO2_CONNECT_TIME_COMPONENT_HPP__

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
#pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <chrono>

namespace asio2::detail
{
	template<class derived_t, class args_t>
	class connect_time_cp
	{
	public:
		/**
		 * @brief constructor
		 */
		connect_time_cp() = default;

		/**
		 * @brief destructor
		 */
		~connect_time_cp() = default;

		connect_time_cp(connect_time_cp&&) noexcept = default;
		connect_time_cp(connect_time_cp const&) = default;
		connect_time_cp& operator=(connect_time_cp&&) noexcept = default;
		connect_time_cp& operator=(connect_time_cp const&) = default;

	public:
		/**
		 * @brief get build connection time
		 */
		inline std::chrono::time_point<std::chrono::system_clock> get_connect_time() const noexcept
		{
			return this->connect_time_;
		}

		/**
		 * @brief reset build connection time to system_clock::now()
		 */
		inline derived_t & reset_connect_time() noexcept
		{
			this->connect_time_ = std::chrono::system_clock::now();
			return (static_cast<derived_t &>(*this));
		}

		/**
		 * @brief get connection duration of std::chrono::duration
		 */
		inline std::chrono::system_clock::duration get_connect_duration() const noexcept
		{
			return std::chrono::duration_cast<std::chrono::system_clock::duration>(
				std::chrono::system_clock::now() - this->connect_time_);
		}

	protected:
		/// build connection time
		decltype(std::chrono::system_clock::now()) connect_time_ = std::chrono::system_clock::now();
	};
}

#endif // !__ASIO2_CONNECT_TIME_COMPONENT_HPP__
