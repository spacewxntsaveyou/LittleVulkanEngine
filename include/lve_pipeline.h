#pragma once

#include "lve_device.h"

//std
#include <string>
#include <vector>

namespace lve {

	struct PipelineConfigInfo {

		VkViewport viewport;
		VkRect2D scissor;
		VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo;
		VkPipelineRasterizationStateCreateInfo rasterizationInfo;
		VkPipelineMultisampleStateCreateInfo multisampleInfo;
		VkPipelineColorBlendAttachmentState colorBlendAttachment;
		VkPipelineColorBlendStateCreateInfo colorBlendInfo;
		VkPipelineDepthStencilStateCreateInfo depthStencilInfo;
		VkPipelineLayout pipelineLayout = nullptr;
		VkRenderPass renderPass = nullptr;
		uint32_t subpass = 0;
	};

	class LvePipeline {

	public:
			
		LvePipeline(LveDevice &device, const std::string& vertFilepath, const std::string& fragFilepath, const PipelineConfigInfo &configInfo);
		~LvePipeline();

		LvePipeline(const LvePipeline&) = delete;
		void operator = (const LvePipeline&) = delete;

		static PipelineConfigInfo defaultPipelineConfigInfo(uint32_t width, uint32_t height); 

	private:

		static std::vector<char> readFile(const std::string& filepath);

		void createGraphicsPipeline(const std::string& vertFilepath, const std::string& fragFilepath, const PipelineConfigInfo& configInfo);

		void createShaderModule(const std::vector<char>& code, VkShaderModule* shaderModule);

		LveDevice& lveDevice; //dereferencing this dangling pointer COULD crash the program, the device will outlast any variable of the class (The gpu doesnt stop existing)
		VkPipeline graphicsPipeline; //handle to "VkPipeline" Object
		VkShaderModule vertShaderModule;
		VkShaderModule fragShaderModule;
	};

} //namespace lve