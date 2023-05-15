#pragma once

#include <GLFW/glfw3.h>
#include "Config.hpp"

class Engine {

public:

	Engine();
	~Engine();

private:

	const bool debug_mode = true;

	int width = 640;
	int height = 480;
	GLFWwindow* window = nullptr;

	vk::Instance instance = nullptr;

	vk::DispatchLoaderDynamic dispatch_loader;

	vk::DebugUtilsMessengerEXT debug_messenger = nullptr;

	vk::PhysicalDevice physical_device = nullptr;

	void buildGlfwWindow();

	void makeInstance();

	void makeDebugMessenger();

	void makeDevice();


};