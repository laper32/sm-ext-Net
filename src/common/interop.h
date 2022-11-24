#ifndef _INTEROP_H
#define _INTEROP_H

#pragma once

#include <algorithm>
#include <vector>
#include <string>
#include <functional>
#include <array>
#include <iterator>

#include <sp_vm_api.h>
#include <Color.h>
#include <vector.h>

namespace sm {
	namespace interop {
		// decl
		template<class Fn, class PtrType = IForward*> class ForwardCaller;
		template<class Fn, class PtrType = IPluginFunction*> class PluginFunctionCaller;

		// bool => bool
		// char => char
		// int => int / cell_t
		// float => float
		// Handle => Handle_t
		// string => std::string
		// array => std::vector / std::array
		// Function => PluginFunctionCaller / std::function

		template<class T>
		inline auto cell2native(IPluginContext* pContext, cell_t in, T& out)
			-> std::enable_if_t<std::is_integral_v<T> || std::is_enum_v<T>, int>
		{
			out = static_cast<T>(in);
			return 1;
		}
		template<class T>
		inline auto native2cell(IPluginContext* pContext, const T& in, cell_t* out)
			-> std::enable_if_t<std::is_integral_v<T> || std::is_enum_v<T>, int>
		{
			if (out)
				*out = static_cast<cell_t>(in);
			return 1;
		}

		template<class T>
		inline auto cell2native(IPluginContext* pContext, cell_t in, T& out)
			-> std::enable_if_t<std::is_floating_point_v<T>, int>
		{
			out = sp_ctof(in);
			return 1;
		}
		template<class T>
		inline auto native2cell(IPluginContext* pContext, const T& in, cell_t* out)
			-> std::enable_if_t<std::is_floating_point_v<T>, int>
		{
			if (out)
				*out = sp_ftoc(in);
			return 1;
		}

		inline int cell2native(IPluginContext* pContext, cell_t in, std::string& out)
		{
			char* str; pContext->LocalToString(in, &str);
			assert(str != nullptr);
			out = str;
			return 1;
		}
		inline int native2cell(IPluginContext* pContext, const std::string& in, cell_t out, cell_t maxbyte)
		{
			return pContext->StringToLocalUTF8(out, maxbyte, in.c_str(), nullptr);
		}

		inline int cell2native(IPluginContext* pContext, cell_t in, Color& out)
		{
			cell_t* vecParams;
			pContext->LocalToPhysAddr(in, &vecParams);
			out = Color(vecParams[0], vecParams[1], vecParams[2]);
			return 1;
		}
		inline int native2cell(IPluginContext* pContext, const Color& in, cell_t out)
		{
			cell_t* addr;
			pContext->LocalToPhysAddr(out, &addr);
			addr[0] = in.r();
			addr[1] = in.g();
			addr[2] = in.b();
			return 1;
		}

		inline int cell2native(IPluginContext* pContext, cell_t in, Vector& out)
		{
			cell_t* vecParams;
			pContext->LocalToPhysAddr(in, &vecParams);
			out = Vector(sp_ctof(vecParams[0]), sp_ctof(vecParams[1]), sp_ctof(vecParams[2]));
			return 1;
		}

		inline int native2cell(IPluginContext* pContext, const Vector& in, cell_t out)
		{
			cell_t* addr;
			pContext->LocalToPhysAddr(out, &addr);
			addr[0] = sp_ftoc(in.x);
			addr[1] = sp_ftoc(in.y);
			addr[2] = sp_ftoc(in.z);
			return 1;
		}

		inline int cell2native(IPluginContext* pContext, cell_t in, QAngle& out)
		{
			cell_t* angParams;
			pContext->LocalToPhysAddr(in, &angParams);
			out = QAngle(sp_ctof(angParams[0]), sp_ctof(angParams[1]), sp_ctof(angParams[2]));
			return 1;
		}

		inline int native2cell(IPluginContext* pContext, const QAngle& in, cell_t out)
		{
			cell_t* addr;
			pContext->LocalToPhysAddr(out, &addr);
			addr[0] = sp_ftoc(in.x);
			addr[1] = sp_ftoc(in.y);
			addr[2] = sp_ftoc(in.z);
			return 1;
		}

		template<template<class> class FnType, class Ret, class...Args>
		inline auto cell2native(IPluginContext* pContext, cell_t in, FnType<Ret(Args...)>& out)
			-> std::enable_if_t<
			std::is_invocable_r_v<Ret, FnType<Ret(Args...)>, Args...> &&
			std::is_assignable_v<FnType<Ret(Args...)>, PluginFunctionCaller<Ret(Args...)>>,
			int>
		{
			IPluginFunction* pFunction = pContext->GetFunctionById(in);
			if (!pFunction)
			{
				return pContext->ThrowNativeError("Invalid function id (%X)", in);
			}
			out = PluginFunctionCaller<Ret(Args...)>(pFunction);
			return 1;
		}

		inline cell_t n2c(bool in) { return !!in; }
		inline cell_t n2c(cell_t in) { return in; }
		inline cell_t n2c(float in) { return sp_ftoc(in); }

		inline void func_push(ICallable* c, const cell_t& x) { c->PushCell(x); }
		inline void func_push(ICallable* c, cell_t& x) { c->PushCellByRef(&x); }
		inline void func_push(ICallable* c, const float& x) { c->PushFloat(x); }
		inline void func_push(ICallable* c, float& x) { c->PushFloatByRef(&x); }
		inline void func_push(ICallable* c, std::vector<cell_t>& in) { c->PushArray(in.data(), in.size(), SM_PARAM_COPYBACK); }
		template<std::size_t N> void func_push(ICallable* c, std::array<cell_t, N>& in) {
			c->PushArray(in.data(), in.size(), SM_PARAM_COPYBACK);
		}
		template<std::size_t N> void func_push(ICallable* c, cell_t(&in)[N]) { c->PushArray(in, N, SM_PARAM_COPYBACK); }
		template<class ArrayType> auto func_push(ICallable* c, const ArrayType& arr)
			-> decltype(std::begin(arr), std::end(arr), void())
		{
			std::vector<cell_t> in;
			std::transform(std::begin(arr), std::end(arr), std::back_inserter(in), [](const auto& x) { return n2c(x); });
			c->PushArray(in.data(), in.size());
		}
		inline void func_push(ICallable* c, const std::string& str) { c->PushString(str.c_str()); }
		inline void func_push(ICallable* c, std::string& str) {
			c->PushStringEx(str.data(), str.size(), SM_PARAM_STRING_UTF8, SM_PARAM_COPYBACK); 
		}
		inline void func_push(ICallable* c, const char* psz) { c->PushString(psz); }
		template<std::size_t N> void func_push(ICallable* c, char(&str)[N]) {
			c->PushStringEx(str, N, SM_PARAM_STRING_UTF8, SM_PARAM_COPYBACK);
		}

		template<class Fn, class PtrType> class ForwardCaller;
		template<class PtrType, class...Args> class ForwardCaller<cell_t(Args...), PtrType>
		{
			PtrType const m_pfn;
		public:
			ForwardCaller(PtrType pf) : m_pfn(std::move(pf)) {}
			cell_t operator()(Args ...args) const
			{
				cell_t result;
				(func_push(&*m_pfn, args), ..., m_pfn->Execute(&result));
				return result;
			}
			friend auto operator<=>(const ForwardCaller& a, const ForwardCaller& b)
			{
				return a.m_pfn->GetFunctionID() <=> b.m_pfn->GetFunctionID();
			}
		};
		template<class PtrType, class...Args> class ForwardCaller<void(Args...), PtrType>
		{
			PtrType const m_pfn;
		public:
			ForwardCaller(PtrType pf) : m_pfn(std::move(pf)) {}
			void operator()(const Args &...args) const
			{
				ForwardCaller<cell_t(Args...)>::operator()(args...);
			}
			friend auto operator<=>(const ForwardCaller& a, const ForwardCaller& b)
			{
				return a.m_pfn->GetFunctionID() <=> b.m_pfn->GetFunctionID();
			}
		};

		template<class Fn, class PtrType> class PluginFunctionCaller;
		template<class PtrType, class Ret, class...Args> class PluginFunctionCaller<Ret(Args...), PtrType>
		{
			PtrType const m_pfn;
		public:
			PluginFunctionCaller(PtrType pf) : m_pfn(std::move(pf)) {}
			Ret operator()(Args...args) const
			{
				cell_t result;
				(func_push(&*m_pfn, args), ..., m_pfn->Execute(&result));
				if constexpr (!std::is_void_v<Ret>)
				{
					Ret ret;
					cell2native(m_pfn->GetParentContext(), &result, ret);
					return ret;
				}
			}
			friend auto operator<=>(const PluginFunctionCaller& a, const PluginFunctionCaller& b)
			{
				return a.m_pfn->GetFunctionID() <=> b.m_pfn->GetFunctionID();
			}
		};

		template<class Fn, class PtrType> class NativeCaller;
		template<class Ret, class...Args> class NativeCaller<Ret(Args...), SPVM_NATIVE_FUNC>
		{
			IPluginContext* const m_pContext;
			SPVM_NATIVE_FUNC const m_pfn;

		public:
			NativeCaller(IPluginContext* pContext, SPVM_NATIVE_FUNC pf) : m_pContext(pContext), m_pfn(pf) {}

			Ret operator()(Args...args) const
			{
				int params_size = (native2cell(m_pContext, args, nullptr) + ...);

				std::vector<cell_t> params_vec(params_size + 1);

				params_vec[0] = sizeof...(Args);
				auto p = params_vec.data() + 1;
				int placeholder_args[] = { (p += native2cell(m_pContext, args, p)) ... };

				cell_t out = (*m_pfn)(m_pContext, params_vec.data());
				if constexpr (!std::is_void_v<Ret>)
				{
					Ret ret;
					cell2native(m_pContext, out, ret);
					return ret;
				}
			}
			friend auto operator<=>(const NativeCaller& a, const NativeCaller& b)
			{
				return std::tie(a.m_pfn, a.m_pContext) <=> std::tie(b.m_pfn, b.m_pContext);
			}
		};

		template<class...Args>
		void params2vars(IPluginContext* pContext, const cell_t* params, Args&...out)
		{
			int N = 1;
			((N += cell2native(pContext, params[N], out)), ...);
		}

		template<class TupleType, std::size_t...I>
		void params2tuple_impl(IPluginContext* pContext, const cell_t* params, TupleType& ret, std::index_sequence<I...>)
		{
			params2vars(pContext, params, std::get<I>(ret)...);
		}

		template<class...Args>
		std::tuple<Args...> params2tuple(IPluginContext* pContext, const cell_t* params)
		{
			std::tuple<Args...> ret;
			params2tuple_impl(pContext, params, ret, std::index_sequence_for<Args...>());
			return ret;
		}
	}
}

#endif // !_INTEROP_H