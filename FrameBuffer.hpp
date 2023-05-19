#pragma once

#include <vulkan/vulkan.hpp>
#include "Frame.hpp"
#include <vector>

namespace vkInit {

	struct FrameBufferInput {

		vk::Device logical_device;
		vk::RenderPass render_pass;
		vk::Extent2D swapchain_extent;

	};

	void makeFramebuffers(FrameBufferInput input_chunk, std::vector<vkUtil::SwapChainFrame>& frames) {

		for (int i = 0; i < frames.size(); ++i) {

			std::vector<vk::ImageView> attachements = { frames[i].image_view };
			vk::FramebufferCreateInfo framebuffer_info = {};

			framebuffer_info.flags = vk::FramebufferCreateFlags();
			framebuffer_info.renderPass = input_chunk.render_pass;
			framebuffer_info.attachmentCount = attachements.size();
			framebuffer_info.pAttachments = attachements.data();
			framebuffer_info.width = input_chunk.swapchain_extent.width;
			framebuffer_info.height = input_chunk.swapchain_extent.height;
			framebuffer_info.layers = 1;

			try {

				frames[i].framebuffer = input_chunk.logical_device.createFramebuffer(framebuffer_info);

			}
			catch(vk::SystemError err){

				std::cerr << "Failed to create framebuffer for frame: " << i << std::endl;

			}

		}

	}


}