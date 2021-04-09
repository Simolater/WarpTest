#include "wppch.h"
#include "Core\Application.h"

int main(int argc, char** argv) {

	WP_LOG_INIT();
	WP_LOG_INFO("Creating app");
	auto app = new Warp::Application();

	WP_LOG_INFO("Initalizing app");
	app->init();

	WP_LOG_INFO("Running app");
	app->run();

	WP_LOG_INFO("Shutting down app");
	delete app;
}