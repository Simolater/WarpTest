#pragma once

#include <memory>

namespace Warp {
	template<class T>
	using CountedRef = std::shared_ptr<T>;

	template<class T, typename ... args>
	constexpr CountedRef<T> CreateCountedRef(args&&... values) {
		return std::make_shared<T>(std::forward<args>(values)...);
	}
}