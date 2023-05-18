#pragma once

#include <fstream>
#include <iostream>
#include <vector>
#include <vulkan/vulkan.hpp>

namespace vkUtil {

	std::vector<char> readFile(bool debug, std::string file_path) {

		std::ifstream file(file_path, std::ios::ate | std::ios::binary);

		if (debug && !file.is_open()) {

			std::cout << "Failed to load \"" << file_path << "\"" << std::endl;

		}

		size_t file_size = static_cast<size_t> (file.tellg());

		std::vector<char> buffer(file_size);

		file.seekg(0);
		file.read(buffer.data(), file_size);

		file.close();

		return buffer;

	}

	vk::ShaderModule createShaderModule(bool debug, std::string file_path, vk::Device logical_device) {

		std::vector<char> source_code = readFile(debug, file_path);

		vk::ShaderModuleCreateInfo shader_module_info = {};
		shader_module_info.flags = vk::ShaderModuleCreateFlags();
		shader_module_info.codeSize = source_code.size();
		shader_module_info.pCode = reinterpret_cast<const uint32_t*>(source_code.data());

		try {

			return logical_device.createShaderModule(shader_module_info);

		}
		catch (vk::SystemError err) {

			if (debug) {

				std::cerr << "Failed to create shader module for \"" << file_path << "\"" << std::endl;

			}

		}


	}


}
