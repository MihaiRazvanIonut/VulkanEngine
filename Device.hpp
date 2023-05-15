#pragma once

#include "Config.hpp"
#include <set>

namespace vkInit {


	void logDeviceProperties(const vk::PhysicalDevice& device) {

		vk::PhysicalDeviceProperties device_properties = device.getProperties();

		std::cout << "Device name: " << device_properties.deviceName << '\n';
		std::cout << "Device type: ";

		switch (device_properties.deviceType) {

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
									 const bool					    debug,
									 const vk::PhysicalDevice&       device,
									 const std::vector<const char*>& requested_extensions) {

		std::set<std::string> required_extensions(requested_extensions.begin(), requested_extensions.end());
		if (debug) {

			std::cout << "Device can support extentions: \n";

		}

		for (auto& extension : device.enumerateDeviceExtensionProperties()) {

			if (debug) {

				std::cout << extension.extensionName << '\n';

			}

			required_extensions.erase(extension.extensionName);

		}

		return required_extensions.empty();


	}

	bool isDeviceSuitable(const bool debug, const vk::PhysicalDevice& device) {

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

		if (bool extentions_supported = checkDeviceExtensionSupport(debug, device, requested_extentions)) {

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

		std::vector<vk::PhysicalDevice> available_devices = instance.enumeratePhysicalDevices();

		if (debug) {

			std::cout << "There are " << available_devices.size() << " physical devices available on this system!\n";

		}

		for (auto& device : available_devices) {

			if (debug) {

				logDeviceProperties(device);

			}
			if (isDeviceSuitable(debug, device)) {
			
				return device;

			}

		}

		return nullptr;

	}


} // Namespace vkInit