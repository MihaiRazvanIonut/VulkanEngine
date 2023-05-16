#pragma once

#include <vulkan/vulkan.hpp>
#include <iostream>
#include <set>
#include <optional>
#include "Logging.hpp"
#include "QueueFamilies.hpp"
#include "Frame.hpp"

namespace vkInit {

	struct SwapChainSupportDetails {

		vk::SurfaceCapabilitiesKHR capabilities;
		std::vector<vk::SurfaceFormatKHR> formats;
		std::vector<vk::PresentModeKHR> present_modes;

	};

	struct SwapChainBundle {

		vk::SwapchainKHR swapchain;
		std::vector<vkUtil::SwapChainFrame> frames;
		vk::Format format;
		vk::Extent2D extent;

	};

	SwapChainSupportDetails querySwapChainSupport(const bool& debug, vk::PhysicalDevice physical_device, vk::SurfaceKHR surface) {


		SwapChainSupportDetails support;


		support.capabilities = physical_device.getSurfaceCapabilitiesKHR(surface);

		if (debug) {

			std::cout << "Swapchain can support the following surface capabilities: \n";

			std::cout << "\tminimum image count: " << support.capabilities.minImageCount << '\n';
			std::cout << "\tmaximimum image count: " << support.capabilities.maxImageCount << '\n';

			std::cout << "\tCurrent extent: \n";
			std::cout << "\t\twidth: " << support.capabilities.currentExtent.width << '\n';
			std::cout << "\t\theight: " << support.capabilities.currentExtent.height << '\n';

			std::cout << "\tMinimum supported extent: \n";
			std::cout << "\t\twidth: " << support.capabilities.minImageExtent.width << '\n';
			std::cout << "\t\theight: " << support.capabilities.minImageExtent.height << '\n';


			std::cout << "\tMaximum supported extent: \n";
			std::cout << "\t\twidth: " << support.capabilities.maxImageExtent.width << '\n';
			std::cout << "\t\theight: " << support.capabilities.maxImageExtent.height << '\n';

			std::cout << "\tMaximum image layers: " << support.capabilities.maxImageArrayLayers << '\n';

			std::cout << "\tSupported transforms:\n";
			std::vector<std::string> string_list = logTransformBits(support.capabilities.supportedTransforms);

			for (auto& line : string_list) {

				std::cout << "\t\t" << line << '\n';

			}

			std::cout << "\tCurrent transform:\n";
			string_list = logTransformBits(support.capabilities.currentTransform);

			for (auto& line : string_list) {

				std::cout << "\t\t" << line << '\n';

			}

			std::cout << "\tSupported alpha operations: \n";
			string_list = logAlphaCompositeBits(support.capabilities.supportedCompositeAlpha);
			for (auto& line : string_list) {

				std::cout << "\t\t" << line << '\n';

			}


		}

		support.formats = physical_device.getSurfaceFormatsKHR(surface);

		if (debug) {

			for (auto& supported_format : support.formats) {

				std::cout << "Supported pixel format: " << vk::to_string(supported_format.format) << '\n';
				std::cout << "Supported color space: " << vk::to_string(supported_format.colorSpace) << '\n';

			}

		}

		support.present_modes = physical_device.getSurfacePresentModesKHR(surface);

		for (auto& present_mode : support.present_modes) {

			std::cout << '\t' << logPresentMode(present_mode) << '\n';

		}


		return support;


	}


	vk::SurfaceFormatKHR chooseSwapChainSurfaceFormat(std::vector<vk::SurfaceFormatKHR> formats) {

		for (auto& format : formats) {

			if (format.format == vk::Format::eB8G8R8A8Unorm && format.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear) {

				return format;

			}

		}

		return formats[0];

	}

	vk::PresentModeKHR chooseSwapChainPresentMode(std::vector<vk::PresentModeKHR> present_modes) {

		for (auto& present_mode : present_modes) {

			if (present_mode == vk::PresentModeKHR::eMailbox)
				return present_mode;

		}

		return vk::PresentModeKHR::eFifo;

	}

	vk::Extent2D chooseSwapChainExtent(uint32_t width, uint32_t height, vk::SurfaceCapabilitiesKHR surface_capabilities) {

		if (surface_capabilities.currentExtent.width != UINT32_MAX) {

			return surface_capabilities.currentExtent;

		}
		else {

			vk::Extent2D extent = { width, height };

			extent.width = std::min(surface_capabilities.maxImageExtent.width, std::max(surface_capabilities.minImageExtent.width, width));
			extent.height = std::min(surface_capabilities.maxImageExtent.height, std::max(surface_capabilities.minImageExtent.height, height));

			return extent;
		}


	}

	SwapChainBundle createSwapChain(const bool& debug, vk::Device logical_device, vk::PhysicalDevice physical_device, vk::SurfaceKHR surface, const int& width, const int& height) {

		SwapChainSupportDetails support = querySwapChainSupport(debug, physical_device, surface);

		vk::SurfaceFormatKHR format = chooseSwapChainSurfaceFormat(support.formats);

		vk::PresentModeKHR present_mode = chooseSwapChainPresentMode(support.present_modes);

		vk::Extent2D extent = chooseSwapChainExtent(width, height, support.capabilities);

		uint32_t image_count = std::min(support.capabilities.maxImageCount, support.capabilities.minImageCount + 1);

		vk::SwapchainCreateInfoKHR create_swapchain_info =
			vk::SwapchainCreateInfoKHR(

				vk::SwapchainCreateFlagsKHR(),
				surface, image_count, format.format, format.colorSpace,
				extent, 1, vk::ImageUsageFlagBits::eColorAttachment

			);
		vkUtil::QueueFamilyIndices indices = vkUtil::findQueueFamilies(debug, physical_device, surface);
		uint32_t queue_family_indices[] = { indices.graphics_family.value(), indices.present_family.value() };

		if (indices.graphics_family.value() != indices.present_family.value()) {

			create_swapchain_info.imageSharingMode = vk::SharingMode::eConcurrent;
			create_swapchain_info.queueFamilyIndexCount = 2;
			create_swapchain_info.pQueueFamilyIndices = queue_family_indices;

		}
		else {

			create_swapchain_info.imageSharingMode = vk::SharingMode::eExclusive;

		}

		create_swapchain_info.preTransform = support.capabilities.currentTransform;
		create_swapchain_info.compositeAlpha = vk::CompositeAlphaFlagBitsKHR::eOpaque;
		create_swapchain_info.presentMode = present_mode;
		create_swapchain_info.clipped = VK_TRUE;

		create_swapchain_info.oldSwapchain = vk::SwapchainKHR(nullptr);

		SwapChainBundle bundle{};

		try {

			bundle.swapchain = logical_device.createSwapchainKHR(create_swapchain_info);

		}
		catch (vk::SystemError err) {

			throw std::runtime_error("Failed to create swapchain!");

		}

		std::vector<vk::Image>images = logical_device.getSwapchainImagesKHR(bundle.swapchain);

		bundle.frames.resize(images.size());

		for (size_t i = 0; i < images.size(); ++i) {


			vk::ImageViewCreateInfo create_image_view_info = {};
			create_image_view_info.image = images[i];
			create_image_view_info.viewType = vk::ImageViewType::e2D;
			create_image_view_info.components.r = vk::ComponentSwizzle::eIdentity;
			create_image_view_info.components.g = vk::ComponentSwizzle::eIdentity;
			create_image_view_info.components.b = vk::ComponentSwizzle::eIdentity;
			create_image_view_info.components.a = vk::ComponentSwizzle::eIdentity;

			create_image_view_info.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
			create_image_view_info.subresourceRange.baseMipLevel = 0;
			create_image_view_info.subresourceRange.levelCount = 1;
			create_image_view_info.subresourceRange.baseArrayLayer = 0;
			create_image_view_info.subresourceRange.layerCount = 1;
			create_image_view_info.format = format.format;

			bundle.frames[i].image = images[i];
			bundle.frames[i].image_view = logical_device.createImageView(create_image_view_info);

		}

		bundle.format = format.format;

		bundle.extent = extent;

		return bundle;

	}



}