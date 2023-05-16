#pragma once

#include<vulkan/vulkan.hpp>
#include<iostream>

namespace vkUtil {

	struct SwapChainFrame {

		vk::Image image;
		vk::ImageView image_view;


	};

}