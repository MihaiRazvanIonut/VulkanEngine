#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vulkan/vulkan.hpp>
#include <iostream>
#include "Frame.hpp"
#include "Scene.hpp"


class Engine {

public:

	Engine(const bool& debug, int width, int height, GLFWwindow* window);
	~Engine();

	void render(Scene* scene);

private:

	bool debug_mode;

	int width;
	int height;
	GLFWwindow* window;

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

	vk::CommandPool command_pool;
	vk::CommandBuffer main_command_buffer;

	int max_frames_in_flight, frame_number;

	void makeInstance();

	void makeDebugMessenger();

	void makeDevice();

	void makePipeline();

	void finalizeSetup();

	void recordDrawCommands(vk::CommandBuffer command_buffer, uint32_t image_index, Scene* scene);


};