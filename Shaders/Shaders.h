#pragma once

#include <fstream>
#include <iostream>
#include <vector>

namespace vkUtil {

	std::vector<char> read_file(bool debug, std::string file_path) {

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


}
