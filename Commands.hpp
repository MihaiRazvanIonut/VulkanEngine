#pragma once
#include <vulkan/vulkan.hpp>
#include "QueueFamilies.hpp"


namespace vkInit {


	struct CommandBufferInputChunk {

		vk::Device logical_device;
		vk::CommandPool command_pool;
		std::vector<vkUtil::SwapChainFrame>& frames;

	};

	vk::CommandPool makeCommandPool(const bool& debug, vk::Device logical_device, vk::PhysicalDevice physical_device, vk::SurfaceKHR surface) {

		vkUtil::QueueFamilyIndices queue_family_indices = vkUtil::findQueueFamilies(debug, physical_device, surface);
		
		vk::CommandPoolCreateInfo command_pool_info = {};
		command_pool_info.flags = vk::CommandPoolCreateFlags() | vk::CommandPoolCreateFlagBits::eResetCommandBuffer;
		command_pool_info.queueFamilyIndex = queue_family_indices.graphics_family.value();

		try {

			return logical_device.createCommandPool(command_pool_info);

		}
		catch(vk::SystemError err){

			if (debug) {

				std::cout << "Failed to create Command Pool" << std::endl;


			}

			return nullptr;

		}

	}

	vk::CommandBuffer makeCommandBuffers(const bool& debug, CommandBufferInputChunk command_buffer_input_chunk) {

		vk::CommandBufferAllocateInfo command_buffer_allocate_info = {};
		command_buffer_allocate_info.commandPool = command_buffer_input_chunk.command_pool;
		command_buffer_allocate_info.level = vk::CommandBufferLevel::ePrimary;
		command_buffer_allocate_info.commandBufferCount = 1;

		for (int i = 0; i < command_buffer_input_chunk.frames.size(); ++i) {

			try {

				command_buffer_input_chunk.frames[i].commandbuffer = command_buffer_input_chunk.logical_device.allocateCommandBuffers(command_buffer_allocate_info)[0];

				if (debug) {

					std::cout << "Allocated command buffer for frame " << i << std::endl;

				}
			}
			catch (vk::SystemError err) {

				if (debug) {

					std::cout << "Failed to create command buffer for frame " << i << std::endl;

				}

			}

		}

		try {

			vk::CommandBuffer commandbuffer = command_buffer_input_chunk.logical_device.allocateCommandBuffers(command_buffer_allocate_info)[0];

			if (debug) {

				std::cout << "Allocated main command buffer" << std::endl;

			}

			return commandbuffer;
		}
		catch (vk::SystemError err) {

			if (debug) {

				std::cout << "Failed to allocate main command buffer" << std::endl;

			}

			return nullptr;

		}

	}

}