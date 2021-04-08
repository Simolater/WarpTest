#include "wppch.h"
#include "Application.h"

namespace Warp {

	void Application::run() {
		WP_LOG_INIT();
		WP_LOG_CRITICAL("Test");
	}
}