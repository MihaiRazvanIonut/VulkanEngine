#pragma once

#include<vulkan/vulkan.hpp>
#include<iostream>

namespace vkUtil {

	struct SwapChainFrame {

		vk::Image image;
		vk::ImageView image_view;
		vk::Framebuffer framebuffer;
		vk::CommandBuffer commandbuffer;

		vk::Semaphore image_available, render_finished;
		vk::Fence in_flight;

	};

}