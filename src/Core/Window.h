#pragma once
#include <GLFW/glfw3.h>
#include "Base.h"

namespace Warp {

	class Window {
	public:
		Window();
		~Window();

		bool shouldClose();

		void update();
	private:
		GLFWwindow* m_window;
	};
}

