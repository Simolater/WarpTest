#pragma once
#include <memory>

namespace Warp {
	template<class T>
	using CountedRef = std::shared_ptr<T>;

	template<class T, class ... args>
	constexpr CountedRef<T> CreateCountedRef(args&&... values) {
		return std::make_shared<T>(std::forward<args>(values)...);
	}

	template<class T>
	using ScopedRef = std::unique_ptr<T>;

	template<class T, class ... args>
	constexpr ScopedRef<T> CreateScopedRef(args&&... values) {
		return std::make_unique<T>(std::forward<args>(values)...);
	}
}

#if defined(WARP_PLATFORM_WINDOWS)
#define WP_DEBUG_BREAK() __debugbreak();
#else
#error "Debug break not yet supported on this platform"
#endif