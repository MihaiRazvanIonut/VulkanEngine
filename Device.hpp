#pragma once

#include <vulkan/vulkan.hpp>
#include <iostream>
#include <set>
#include <optional>
#include "QueueFamilies.hpp"

namespace vkInit {

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



	vk::Device createLogicalDevice(const bool& debug, vk::PhysicalDevice& physical_device, vk::SurfaceKHR surface) {

		vkUtil::QueueFamilyIndices indices = vkUtil::findQueueFamilies(debug, physical_device, surface);
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
		 
		vkUtil::QueueFamilyIndices indices = vkUtil::findQueueFamilies(debug, physical_device, surface);
		

		return { {
				
				device.getQueue(indices.graphics_family.value(), 0),
				device.getQueue(indices.present_family.value(), 0)
			
			   } };
	}

} // Namespace vkInit