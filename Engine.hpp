#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

class Engine {

public:
	Engine();
	~Engine();

private:

	bool debug_mode = true;

	int width = 640;
	int height = 480;
	GLFWwindow* window = nullptr;

	void buildGlfwWindow();


};