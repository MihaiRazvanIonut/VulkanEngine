#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vulkan/vulkan.hpp>
#include <iostream>


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
	vk::SurfaceKHR surface;

	vk::PhysicalDevice physical_device = nullptr;
	vk::Device device = nullptr;
	vk::Queue graphics_queue = nullptr;
	vk::Queue present_queue = nullptr;


	void buildGlfwWindow();

	void makeInstance();

	void makeDebugMessenger();

	void makeDevice();


};