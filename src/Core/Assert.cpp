#include "wppch.h"
#include "Assert.h"

namespace Warp {

	void log_assert(char* expr, char* message, char* file, int line) {
		if (message) {
			WP_LOG_CRITICAL("Assertion error ({0}) at {1}:{2} -> {3}", expr, file, line, message);
		}
		else {
			WP_LOG_CRITICAL("Assertion error ({0}) at {1}:{2}", expr, file, line);
		}
	}
}
