#include "Application.hpp"

Application::Application(const bool& debug, int width, int height) {

	buildGlfwWindow(debug, width, height);

	graphics_engine = new Engine(debug, width, height, window);

}

void Application::buildGlfwWindow(const bool& debug, int width, int height) {

	glfwInit();

	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

	if (!(window = glfwCreateWindow(width, height, "Cyan Crate: A Vulkan Engine", nullptr, nullptr))) {

		std::cerr << "GLFW window failed to create!" << '\n';

	}

}

void Application::runApplication() {

	while (!glfwWindowShouldClose(window)) {

		glfwPollEvents();
		graphics_engine->render();
		calculateFrameRate();
	}


}

void Application::calculateFrameRate() {

	current_time = glfwGetTime();
	double delta = current_time - last_time;

	if (delta >= 1) {
		int framerate{ std::max(1, int(num_frames / delta)) };
		std::stringstream title;
		title << "Running at " << framerate << " fps.";

		glfwSetWindowTitle(window, title.str().c_str());
		last_time = current_time;
		num_frames = -1;
		frame_time = float(1000.0 / framerate);
	}

	++num_frames;

}

Application::~Application() {

	delete graphics_engine;

}