#pragma once

#include "Application.h"
#include "Log\Log.h"

extern Warp::Application* CreateApplication();

int main(int argc, char** argv) {

	auto app = CreateApplication();

	delete app;
}