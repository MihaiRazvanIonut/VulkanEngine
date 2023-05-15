#pragma once

#include "Config.hpp"
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

	QueueFamilyIndices findQueueFamilies(const bool& debug, vk::PhysicalDevice& physical_device) {

		QueueFamilyIndices indices;

		std::vector<vk::QueueFamilyProperties> queue_families = physical_device.getQueueFamilyProperties();

		if (debug) {

			std::cout << "System can support: " << queue_families.size() << " queue families.\n";

		}

		int i = 0;

		for (auto& queue_family : queue_families) {

			if (queue_family.queueFlags & vk::QueueFlagBits::eGraphics) {

				indices.graphics_family = i;
				indices.present_family = i;

				if (debug) {

					std::cout << "Queue family " << i << " is suitable for graphics and presenting. \n";

				}

			}

			if (indices.isComplete()) {

				break;

			}

			i++;

		}

		return indices;

	}


	vk::Device createLogicalDevice(const bool& debug, vk::PhysicalDevice& physical_device) {

		QueueFamilyIndices indices = findQueueFamilies(debug, physical_device);
		float queue_priority = 1.0f;
		vk::DeviceQueueCreateInfo queue_create_info = 
			vk::DeviceQueueCreateInfo(vk::DeviceQueueCreateFlags(),
									  indices.graphics_family.value(),
									  1, &queue_priority);

		vk::PhysicalDeviceFeatures physical_device_features = vk::PhysicalDeviceFeatures();
		

		std::vector<const char*> enabled_layers;
		if (debug) {

			enabled_layers.push_back("VK_LAYER_KHRONOS_validation");

		}

		vk::DeviceCreateInfo device_info = vk::DeviceCreateInfo(

			vk::DeviceCreateFlags(), 1, &queue_create_info, 
			static_cast<uint32_t>(enabled_layers.size()), enabled_layers.data(),
			0, nullptr, &physical_device_features
		
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

	vk::Queue getQueue(const bool& debug, vk::PhysicalDevice physical_device, vk::Device device) {

		QueueFamilyIndices indices = findQueueFamilies(debug, physical_device);


		return device.getQueue(indices.graphics_family.value(), 0);
	}


} // Namespace vkInit