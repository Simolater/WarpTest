#pragma once
#include "Base.h"
#include "Window.h"

namespace Warp {

	class Application {

	public:
		void init();

		void run();
	
	private:
		CountedRef<Window> m_window;
	};
}