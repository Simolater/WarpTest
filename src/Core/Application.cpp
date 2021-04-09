#include "wppch.h"
#include "Application.h"
#include "Window.h"

namespace Warp {

	void Application::init() {
		m_window = CreateCountedRef<Window>();
	}

	void Application::run() {
		while (!m_window->shouldClose()) {
			m_window->update();
		}
	}
}