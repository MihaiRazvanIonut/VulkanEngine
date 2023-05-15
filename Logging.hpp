#pragma once

#include "Config.hpp"

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
				vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose | vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning | vk::DebugUtilsMessageSeverityFlagBitsEXT::eError,
				vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral | vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation | vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance,
				debugCallback,
				nullptr
			);


		return instance.createDebugUtilsMessengerEXT(create_debug_utils_messenger_callback_data_flags,
			nullptr, dispatch_loader);

	}


}// Namespace vkInit