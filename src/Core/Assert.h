#pragma once
#include "Base.h"

#if defined(WP_ASSERT_ENABLE)

namespace Warp {
	void log_assert(char* expr, char* message, char* file, int line);
}

#define WP_ASSERTM(expr, message) if (!(expr)) { Warp::log_assert(#expr, message, __FILE__, __LINE__); WP_DEBUG_BREAK(); }

#else

#define WP_ASSERTM(expr, message) ((void)0)

#endif

#define WP_ASSERT(expr) WP_ASSERTM(expr, nullptr)