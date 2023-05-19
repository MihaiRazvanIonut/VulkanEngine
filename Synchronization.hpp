#pragma once

#include <vulkan/vulkan.hpp>

namespace vkInit {

	vk::Semaphore makeSemaphore(const bool& debug, vk::Device logical_device) {

		vk::SemaphoreCreateInfo semaphore_info = {};
		semaphore_info.flags = vk::SemaphoreCreateFlags();

		try {

			return logical_device.createSemaphore(semaphore_info);

		}
		catch (vk::SystemError err) {

			if (debug) {

				std::cout << "Failed to create semaphore" << std::endl;

			}

			return nullptr;

		}

	}

	vk::Fence makeFence(const bool& debug, vk::Device logical_device) {

		vk::FenceCreateInfo fence_info = {};
		fence_info.flags = vk::FenceCreateFlags() | vk::FenceCreateFlagBits::eSignaled;

		try {

			return logical_device.createFence(fence_info);

		}
		catch (vk::SystemError err) {
			if (debug) {
				std::cout << "Failed to create fence" << std::endl;
			}
			return nullptr;
		}

	}


}