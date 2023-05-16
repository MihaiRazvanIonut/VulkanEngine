#pragma once

#include <vulkan/vulkan.hpp>
#include <iostream>
#include <set>
#include <optional>

namespace vkInit {

	struct QueueFamilyIndices {

		std::optional<uint32_t> graphics_family;
		std::optional<uint32_t> present_family;

		bool isComplete() {

			return graphics_family.has_value() && present_family.has_value();

		}

	};

	struct SwapChainSupportDetails {

		vk::SurfaceCapabilitiesKHR capabilities;
		std::vector<vk::SurfaceFormatKHR> formats;
		std::vector<vk::PresentModeKHR> present_modes;

	};

	struct SwapChainBundle {

		vk::SwapchainKHR swapchain;
		std::vector<vk::Image> images;
		vk::Format format;
		vk::Extent2D extent;

	};

	void logDeviceProperties(const vk::PhysicalDevice& physical_device) {

		vk::PhysicalDeviceProperties physical_device_properties = physical_device.getProperties();

		std::cout << "Device name: " << physical_device_properties.deviceName << '\n';
		std::cout << "Device type: ";

		switch (physical_device_properties.deviceType) {

		case (vk::PhysicalDeviceType::eCpu):
			std::cout << "CPU\n";
			break;

		case (vk::PhysicalDeviceType::eDiscreteGpu):
			std::cout << "Discrete GPU\n";
			break;

		case (vk::PhysicalDeviceType::eIntegratedGpu):
			std::cout << "Integrated GPU\n";
			break;

		case (vk::PhysicalDeviceType::eVirtualGpu):
			std::cout << "Virtual GPU\n";
			break;

		default:
			std::cout << "Other type of physical device!\n";


		}

	}

	bool checkDeviceExtensionSupport(
									 const bool					     debug,
									 const vk::PhysicalDevice&       physical_device,
									 const std::vector<const char*>& requested_extensions) {

		std::set<std::string> required_extensions(requested_extensions.begin(), requested_extensions.end());
		if (debug) {

			std::cout << "Device can support extentions: \n";

		}

		for (auto& extension : physical_device.enumerateDeviceExtensionProperties()) {

			if (debug) {

				std::cout << extension.extensionName << '\n';

			}

			required_extensions.erase(extension.extensionName);

		}

		return required_extensions.empty();


	}

	bool isDeviceSuitable(const bool debug, const vk::PhysicalDevice& physical_device) {

		if (debug) {

			std::cout << "Checking if device is suitable!\n";

		}

		const std::vector<const char*> requested_extentions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };

		if (debug) {

			std::cout << "We are requesting device extensions: \n";

			for (auto& extension : requested_extentions) {

				std::cout << "\t\"" << extension << "\"\n";

			}

		}

		if (bool extentions_supported = checkDeviceExtensionSupport(debug, physical_device, requested_extentions)) {

			if (debug) {

				std::cout << "Device can support the requested extensions!\n";

			}

		}
		else {

			if (debug) {

				std::cout << "Device can NOT support the requested extensions!\n";

			}

			return false;

		}

		return true;

	}


	vk::PhysicalDevice choosePhysicalDevice(bool debug, vk::Instance& instance) {

		if (debug) {

			std::cout << "Choosing physical device...\n";

		}

		std::vector<vk::PhysicalDevice> available_physical_devices = instance.enumeratePhysicalDevices();

		if (debug) {

			std::cout << "There are " << available_physical_devices.size() << " physical devices available on this system!\n";

		}

		for (auto& physical_device : available_physical_devices) {

			if (debug) {

				logDeviceProperties(physical_device);

			}
			if (isDeviceSuitable(debug, physical_device)) {
			
				return physical_device;

			}

		}

		return nullptr;

	}

	QueueFamilyIndices findQueueFamilies(const bool& debug, vk::PhysicalDevice& physical_device, vk::SurfaceKHR surface) {

		QueueFamilyIndices indices;

		std::vector<vk::QueueFamilyProperties> queue_families = physical_device.getQueueFamilyProperties();

		if (debug) {

			std::cout << "System can support: " << queue_families.size() << " queue families.\n";

		}

		int i = 0;

		for (auto& queue_family : queue_families) {

			if (queue_family.queueFlags & vk::QueueFlagBits::eGraphics) {

				indices.graphics_family = i;

				if (debug) {

					std::cout << "Queue family " << i << " is suitable for graphics and presenting. \n";

				}

			}
			if (physical_device.getSurfaceSupportKHR(i, surface)) {

				indices.present_family = i;
				if (debug) {

					std::cout << "Queue family " << i << " is suitable for presenting. \n";

				}

			}
			if (indices.isComplete()) {

				break;

			}

			i++;

		}

		return indices;

	}


	vk::Device createLogicalDevice(const bool& debug, vk::PhysicalDevice& physical_device, vk::SurfaceKHR surface) {

		QueueFamilyIndices indices = findQueueFamilies(debug, physical_device, surface);
		std::vector<uint32_t> unique_indices;
		unique_indices.push_back(indices.graphics_family.value());

		if (indices.graphics_family.value() != indices.present_family.value()) {

			unique_indices.push_back(indices.present_family.value());

		}

		float queue_priority = 1.0f;
		std::vector<vk::DeviceQueueCreateInfo> queue_create_info;
		
		for (uint32_t queue_family_index : unique_indices) {
			 
			queue_create_info.push_back( 
				vk::DeviceQueueCreateInfo(
										  vk::DeviceQueueCreateFlags(), 
										  queue_family_index,
										  1, &queue_priority
										 )
			);

		}

		std::vector<const char*> device_extension = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };

		vk::PhysicalDeviceFeatures physical_device_features = vk::PhysicalDeviceFeatures();
		

		std::vector<const char*> enabled_layers;
		if (debug) {

			enabled_layers.push_back("VK_LAYER_KHRONOS_validation");

		}

		vk::DeviceCreateInfo device_info = vk::DeviceCreateInfo(

			vk::DeviceCreateFlags(), 
			static_cast<uint32_t>(queue_create_info.size()), queue_create_info.data(),
			static_cast<uint32_t>(enabled_layers.size()), enabled_layers.data(),
			static_cast<uint32_t>(device_extension.size()), device_extension.data(),
			&physical_device_features
		
		);

		try {

			vk::Device device = physical_device.createDevice(device_info);
			if (debug) {

				std::cout << "GPU has been succesfully abstracted!\n";

			}

			return device;

		}
		catch (vk::SystemError error) {

			std::cout << "GPU has NOT been succesfully abstracted!\n";
			return nullptr;

		}

		return nullptr;

	}

	std::array<vk::Queue, 2> getQueue(const bool& debug, vk::PhysicalDevice physical_device, vk::Device device, vk::SurfaceKHR surface) {
		 
		QueueFamilyIndices indices = findQueueFamilies(debug, physical_device, surface);
		

		return { {
				
				device.getQueue(indices.graphics_family.value(), 0),
				device.getQueue(indices.present_family.value(), 0)
			
			   } };
	}


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
		QueueFamilyIndices indices = findQueueFamilies(debug, physical_device, surface);
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
		catch(vk::SystemError err){

			throw std::runtime_error("Failed to create swapchain!");

		}

		bundle.images = logical_device.getSwapchainImagesKHR(bundle.swapchain);

		bundle.format = format.format;

		bundle.extent = extent;

		return bundle;

	}

} // Namespace vkInit