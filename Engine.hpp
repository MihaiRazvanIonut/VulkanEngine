#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vulkan/vulkan.hpp>
#include <iostream>
#include "Frame.hpp"


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
	vk::SwapchainKHR swapchain = nullptr;
	std::vector<vkUtil::SwapChainFrame> swapchain_frames;
	vk::Format swapchain_format;
	vk::Extent2D swapchain_extent;

	vk::PipelineLayout graphics_pipeline_layout;
	vk::RenderPass graphics_pipeline_render_pass;
	vk::Pipeline graphics_pipeline;


	void buildGlfwWindow();

	void makeInstance();

	void makeDebugMessenger();

	void makeDevice();

	void makePipeline();


};