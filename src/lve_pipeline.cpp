#include "lve_pipeline.h"

//std
#include <fstream>
#include <stdexcept>
#include <iostream>

namespace lve {

	LvePipeline::LvePipeline(const std::string& vertFilepath, const std::string& fragFilepath) {

		createGraphicsPipeline(vertFilepath, fragFilepath);

	}

	//readfile function
	std::vector<char> LvePipeline::readFile(const std::string& filepath) {

		std::ifstream file{ filepath, std::ios::ate | std::ios::binary };	//"ate" means when the file opens we seek the end immediately, and read as "binary"
		if (!file.is_open()) {
			throw std::runtime_error("failed to open file: " + filepath);
		}

		size_t fileSize = static_cast<size_t>(file.tellg());	//"tellg" = last position = filesize
		std::vector<char> buffer(fileSize);

		file.seekg(0);
		file.read(buffer.data(), fileSize);

		file.close();
		return buffer;

	}

	void LvePipeline::createGraphicsPipeline(const std::string& vertFilepath, const std::string& fragFilepath) {

		auto vertCode = readFile(vertFilepath);
		auto fragCode = readFile(fragFilepath);

		std::cout << "Vertex Shader Code Size: " << vertCode.size() << '\n';
		std::cout << "Fragment Shader Code Size: " << fragCode.size() << '\n';

	}

} //namespace lve