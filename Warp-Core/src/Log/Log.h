#pragma once

#include <memory>
#include <spdlog\spdlog.h>

#ifdef WP_LOG_ENABLE

namespace Warp {

	class Log {

	public:
		static void init();

		static std::shared_ptr<spdlog::logger>& getLogger() { return s_Logger; }

	private:
		static std::shared_ptr<spdlog::logger> s_Logger;
	};
}

#define WP_LOG_INIT() ::Warp::Log::init();

#define WP_LOG_CRITICAL(...)	::Warp::Log::getLogger()->critical(__VA_ARGS__);
#define WP_LOG_ERROR(...)	::Warp::Log::getLogger()->error(__VA_ARGS__);
#define WP_LOG_WRN(...)		::Warp::Log::getLogger()->warn(__VA_ARGS__);
#define WP_LOG_DBG(...)		::Warp::Log::getLogger()->debug(__VA_ARGS__);
#define WP_LOG_INFO(...)	::Warp::Log::getLogger()->info(__VA_ARGS__);
#define WP_LOG_TRACE(...)	::Warp::Log::getLogger()->trace(__VA_ARGS__);

#else

#define WP_LOG_INIT()

#define WP_LOG_FATAL(...)
#define WP_LOG_ERROR(...)
#define WP_LOG_WRN(...)
#define WP_LOG_DBG(...)
#define WP_LOG_INFO(...)
#define WP_LOG_TRACE(...)
#define WP_LOG_ALL(...)

#endif