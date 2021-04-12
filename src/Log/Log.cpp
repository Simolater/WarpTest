#include "wppch.h"
#include "Log.h"

#include <spdlog/sinks/stdout_color_sinks.h>

#ifdef WP_LOG_ENABLE

namespace Warp {

	CountedRef<spdlog::logger> Log::s_Logger;

	void Log::init() {
		std::array<spdlog::sink_ptr, 1> logSinks = {
			CreateCountedRef<spdlog::sinks::stdout_color_sink_mt>()
		};
		logSinks[0]->set_pattern("[%H:%M:%S] [%n] %v%$");

		s_Logger = CreateCountedRef<spdlog::logger>("WARP", logSinks.begin(), logSinks.end());
		spdlog::register_logger(s_Logger);

		s_Logger->set_level(spdlog::level::trace);
		s_Logger->flush_on(spdlog::level::trace);
	}
}

#endif