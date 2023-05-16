#pragma once

#include <optional>
#include <vulkan/vulkan.hpp>
#include <iostream>
#include <vector>

namespace vkUtil {


	struct QueueFamilyIndices {

		std::optional<uint32_t> graphics_family;
		std::optional<uint32_t> present_family;

		bool isComplete() {

			return graphics_family.has_value() && present_family.has_value();

		}

	};

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


}