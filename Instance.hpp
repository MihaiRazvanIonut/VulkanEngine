#pragma once

#include "Config.hpp"

namespace vkInit {

	bool supported(const bool debug, std::vector<const char*>& extentions, std::vector<const char*>& layers) {

		std::vector<vk::ExtensionProperties> supported_extentions =
			vk::enumerateInstanceExtensionProperties();

		if (debug) {

			std::cout << "Device can support the following extentions:\n";
			for (auto &supported_extention : supported_extentions) {

				std::cout << '\t' << supported_extention.extensionName << '\n';

			}

		}

		bool found;
		for (auto extension : extentions) {

			found = false;
			for (auto& supported_extension : supported_extentions) {

				if (strcmp(extension, supported_extension.extensionName) == 0) {

					found = true;
					if (debug) {

						std::cout << "Extension \"" << extension << "\" is supported!\n";


					}

				}

			}

			if (!found) {

				if (debug) {

					std::cout << "Extension \"" << extension << "\" is NOT supported!\n";

				}
				return false;
			}

		}

		std::vector<vk::LayerProperties> supported_layers = vk::enumerateInstanceLayerProperties();

		if (debug) {

			std::cout << "Device can support the following layers:\n";
			for (auto& supported_layer : supported_layers) {

				std::cout << '\t' << supported_layer.layerName << '\n';

			}

		}

		for (auto layer : layers) {

			found = false;
			for (auto& supported_layer : supported_layers) {

				if (strcmp(layer, supported_layer.layerName) == 0) {

					found = true;
					if (debug) {

						std::cout << "Layer \"" << layer << "\" is supported!\n";


					}

				}

			}

			if (!found) {

				if (debug) {

					std::cout << "Layer \"" << layer << "\" is NOT supported!\n";

				}
				return false;
			}

		}



		return true;
	}

	vk::Instance makeInstance(const bool debug, const char* application_name) {


		uint32_t version = 0;
		vkEnumerateInstanceVersion(&version);

		if (debug) {

			std::cout << "System can support vulkan variant: " << VK_API_VERSION_VARIANT(version)
				<< ", Major: " << VK_API_VERSION_MAJOR(version)
				<< ", Minor: " << VK_API_VERSION_MINOR(version)
				<< ", Patch: " << VK_API_VERSION_PATCH(version)<<'\n';

		}

		version &= ~(0xFFFU);
		version = VK_MAKE_API_VERSION(0, 1, 0, 0);

		vk::ApplicationInfo application_info = vk::ApplicationInfo( application_name, version,
															"Cyan Crate", version, version);
		uint32_t glfw_extension_count = 0;
		const char** glfw_extensions;
		glfw_extensions = glfwGetRequiredInstanceExtensions(&glfw_extension_count);


		std::vector<const char*> extensions(glfw_extensions, glfw_extensions + glfw_extension_count);

		if (debug) {

			extensions.push_back("VK_EXT_debug_utils");

			std::cout << "Extensions to be requested:\n";
			for (auto extention_name : extensions) {

				std::cout << "\t\"" << extention_name << "\"\n";

			}

		}

		std::vector<const char*> layers;

		if (debug) {

			layers.push_back("VK_LAYER_KHRONOS_validation");

		}

		if (!supported(debug, extensions, layers)) {

			return nullptr;

		}


		vk::InstanceCreateInfo instance_create_info = 
			vk::InstanceCreateInfo(vk::InstanceCreateFlags(),
								   &application_info,
								   static_cast<uint32_t>(layers.size()), layers.data(),
								   static_cast<uint32_t>(extensions.size()), extensions.data());

		try {

			return vk::createInstance(instance_create_info, nullptr);

		}
		catch (vk::SystemError err) {

			if (debug) {

				std::cout << "Failed to create Instance!\n";

			}

			return nullptr;

		}

	}

} // Namespace vkInit