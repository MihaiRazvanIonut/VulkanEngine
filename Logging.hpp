#pragma once

#include <vulkan/vulkan.hpp>
#include <iostream>

namespace vkInit {

	VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
		VkDebugUtilsMessageSeverityFlagBitsEXT			message_severity,
		VkDebugUtilsMessageTypeFlagsEXT					message_type,
		const VkDebugUtilsMessengerCallbackDataEXT* p_callback_data,
		void* p_user_data) {

		std::cerr << "Validation layer: " << p_callback_data->pMessage << '\n';
		return VK_FALSE;

	}

	vk::DebugUtilsMessengerEXT makeDebugMessenger(vk::Instance& instance, vk::DispatchLoaderDynamic& dispatch_loader) {

		vk::DebugUtilsMessengerCreateInfoEXT create_debug_utils_messenger_callback_data_flags =
			vk::DebugUtilsMessengerCreateInfoEXT(
				vk::DebugUtilsMessengerCreateFlagsEXT(),
				vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning | vk::DebugUtilsMessageSeverityFlagBitsEXT::eError,
				vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral | vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation | vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance,
				debugCallback,
				nullptr
			);


		return instance.createDebugUtilsMessengerEXT(create_debug_utils_messenger_callback_data_flags,
			nullptr, dispatch_loader);

	}

	std::vector<std::string> logTransformBits(vk::SurfaceTransformFlagsKHR bits) {

		std::vector<std::string> result;

		if (bits & vk::SurfaceTransformFlagBitsKHR::eIdentity) {

			result.push_back("Identity");

		}

		if (bits & vk::SurfaceTransformFlagBitsKHR::eRotate90) {

			result.push_back("90 degrees rotation");

		}

		if (bits & vk::SurfaceTransformFlagBitsKHR::eRotate180) {

			result.push_back("180 degrees rotation");

		}


		if (bits & vk::SurfaceTransformFlagBitsKHR::eRotate270) {

			result.push_back("270 degrees rotation");

		}

		if (bits & vk::SurfaceTransformFlagBitsKHR::eHorizontalMirror) {

			result.push_back("Horizontal mirror");

		}

		if (bits & vk::SurfaceTransformFlagBitsKHR::eHorizontalMirrorRotate90) {

			result.push_back("Horizontal mirror, then 90 degrees rotation");

		}

		if (bits & vk::SurfaceTransformFlagBitsKHR::eHorizontalMirrorRotate180) {

			result.push_back("Horizontal mirror, then 180 degrees rotation");

		}

		if (bits & vk::SurfaceTransformFlagBitsKHR::eHorizontalMirrorRotate270) {

			result.push_back("Horizontal mirror, then 270 degrees rotation");

		}

		if (bits & vk::SurfaceTransformFlagBitsKHR::eInherit) {

			result.push_back("Inherited");

		}

		return result;

	}


	std::vector<std::string> logAlphaCompositeBits(vk::CompositeAlphaFlagsKHR bits) {

		std::vector<std::string> result;

		if (bits & vk::CompositeAlphaFlagBitsKHR::eOpaque) {

			result.push_back("Opaque (alpha is ignored)");

		}

		if (bits & vk::CompositeAlphaFlagBitsKHR::ePreMultiplied) {

			result.push_back("Pre Multiplied");

		}

		if (bits & vk::CompositeAlphaFlagBitsKHR::ePostMultiplied) {

			result.push_back("Post Multiplied");

		}

		if (bits & vk::CompositeAlphaFlagBitsKHR::eInherit) {

			result.push_back("Inherited");

		}

		return result;

	}

	std::string logPresentMode(vk::PresentModeKHR present_mode) {


		if (present_mode == vk::PresentModeKHR::eImmediate) {

			return "Immediate";

		}
		if (present_mode == vk::PresentModeKHR::eMailbox) {

			return "Mailbox";

		}

		if (present_mode == vk::PresentModeKHR::eFifo) {

			return "Fifo";

		}
		if (present_mode == vk::PresentModeKHR::eFifoRelaxed) {

			return "Fifo Relaxed";

		}

		if (present_mode == vk::PresentModeKHR::eSharedDemandRefresh) {

			return "Shared Demand Refresh";

		}


		if (present_mode == vk::PresentModeKHR::eSharedContinuousRefresh) {

			return "Shared Continuous Refresh";

		}

	}

}// Namespace vkInit