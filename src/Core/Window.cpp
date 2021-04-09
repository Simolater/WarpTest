#include "wppch.h"
#include "Window.h"
#include <GLFW/glfw3.h>

constexpr uint32_t WIDTH = 800, HEIGHT = 600;

namespace Warp {
	Window::Window() {
		glfwInit();

		glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

		m_window = glfwCreateWindow(WIDTH, HEIGHT, "Warp", nullptr, nullptr);
	}

	bool Window::shouldClose() {
		return glfwWindowShouldClose(m_window);
	}

	void Window::update() {
		glfwPollEvents();
	}

	Window::~Window() {
		glfwDestroyWindow(m_window);

		glfwTerminate();
	}
}