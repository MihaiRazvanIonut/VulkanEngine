#include "Engine.hpp"
#include <iostream>


Engine::Engine() {

	buildGlfwWindow();

}

void Engine::buildGlfwWindow() {

	glfwInit();

	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

	if (!(window = glfwCreateWindow(width, height, "Vulkan Engine", nullptr, nullptr))) {

		std::cerr << "GLFW window failed to create!" << '\n';

	}


}

Engine::~Engine() {

	
	glfwTerminate();

}