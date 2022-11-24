#ifndef _COMMON_SM_FUNC_HPP
#define _COMMON_SM_FUNC_HPP

#pragma once

#include <common/shared.h>

template<typename F, typename...Args>
inline void WrapSMFunction(F&& fn, Args&&...args)
{
	auto lambda = std::bind(std::forward<F>(fn), std::forward<Args>(args)...);
	using lambda_type = decltype(lambda);
	FRAMEACTION wrap_fn = +[](void* data) {
		lambda_type* lambda_ptr = reinterpret_cast<lambda_type*>(data);
		(*lambda_ptr)();
		delete lambda_ptr;
	};
	void* wrap_lambda = new lambda_type(std::move(lambda));
	g_pSM->AddFrameAction(wrap_fn, wrap_lambda);
}

#endif // !_COMMON_SM_FUNC_HPP
