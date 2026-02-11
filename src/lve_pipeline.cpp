#include "lve_pipeline.h"
#include "lve_model.h"

//std
#include <fstream>
#include <stdexcept>
#include <iostream>
#include <cassert>

namespace lve {
	//Constructer
	LvePipeline::LvePipeline(LveDevice& device, const std::string& vertFilepath, const std::string& fragFilepath, const PipelineConfigInfo& configInfo) :lveDevice{device} {

		createGraphicsPipeline(vertFilepath, fragFilepath, configInfo);

	}
	//Destructer
	LvePipeline::~LvePipeline() {
		//Shaders
		vkDestroyShaderModule(lveDevice.device(), vertShaderModule, nullptr);
		vkDestroyShaderModule(lveDevice.device(), fragShaderModule, nullptr);
		//Pipeline
		vkDestroyPipeline(lveDevice.device(), graphicsPipeline, nullptr);
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

	void LvePipeline::createGraphicsPipeline(const std::string& vertFilepath, const std::string& fragFilepath, const PipelineConfigInfo& configInfo) {

		assert(configInfo.pipelineLayout != VK_NULL_HANDLE && "Cannot create graphics pipeline : no pipelineLayout provided in configInfo");
		assert(configInfo.renderPass != VK_NULL_HANDLE && "Cannot create graphics pipeline : no renderpass provided in configInfo");

		auto vertCode = readFile(vertFilepath);
		auto fragCode = readFile(fragFilepath);

		createShaderModule(vertCode, &vertShaderModule);
		createShaderModule(fragCode, &fragShaderModule);

		VkPipelineShaderStageCreateInfo shaderStages[2];
		//Vertex shader
		shaderStages[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		shaderStages[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
		shaderStages[0].module = vertShaderModule;
		shaderStages[0].pName = "main";
		shaderStages[0].flags = 0;
		shaderStages[0].pNext = nullptr;
		shaderStages[0].pSpecializationInfo = nullptr; //Mechanism to specialize shader functionality

		//Fragment shader
		shaderStages[1].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		shaderStages[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
		shaderStages[1].module = fragShaderModule;
		shaderStages[1].pName = "main";
		shaderStages[1].flags = 0;
		shaderStages[1].pNext = nullptr;
		shaderStages[1].pSpecializationInfo = nullptr; //Mechanism to specialize shader functionality

		//struct explains how to interpret Vertex buffer data	//Initial input in graphics pipeline
		auto& bindingDescriptions = configInfo.bindingDescriptions;
		auto& attributeDescriptions = configInfo.attributeDescriptions;
		VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
		vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
		vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
		vertexInputInfo.vertexBindingDescriptionCount = static_cast<uint32_t>(bindingDescriptions.size());
		vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();
		vertexInputInfo.pVertexBindingDescriptions = bindingDescriptions.data();

		VkGraphicsPipelineCreateInfo pipelineInfo{};

		//Uses the configs to create the graphicsPipeline object
		pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
		pipelineInfo.stageCount = 2;	//Describes how many programmable stages the pipeline uses
		pipelineInfo.pStages = shaderStages;

		//Connect "pipelineCreateInfo" to "configInfo"	//Config is seperated from pipelineCreation
		pipelineInfo.pVertexInputState = &vertexInputInfo;
		pipelineInfo.pInputAssemblyState = &configInfo.inputAssemblyInfo;
		pipelineInfo.pViewportState = &configInfo.viewportInfo;
		pipelineInfo.pRasterizationState = &configInfo.rasterizationInfo;
		pipelineInfo.pMultisampleState = &configInfo.multisampleInfo;
		pipelineInfo.pColorBlendState = &configInfo.colorBlendInfo;
		pipelineInfo.pDepthStencilState = &configInfo.depthStencilInfo;
		pipelineInfo.pDynamicState = &configInfo.dynamicStateInfo;	//Optional setting used to config some pipeline functionality	//"lineWidth", or dynamic viewport without pipeline recreation

		//Adding indexes to createInfo
		pipelineInfo.layout = configInfo.pipelineLayout;
		pipelineInfo.renderPass = configInfo.renderPass;
		pipelineInfo.subpass = configInfo.subpass;

		//Useful for performance	//Creates a new graphics pipeline from an existing one
		pipelineInfo.basePipelineIndex = -1;
		pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;

		if (vkCreateGraphicsPipelines(lveDevice.device(), VK_NULL_HANDLE, 1, & pipelineInfo, nullptr, &graphicsPipeline) != VK_SUCCESS) {
			throw std::runtime_error("failed to create graphics pipeline");
		}

	}

	void LvePipeline::createShaderModule(const std::vector<char>& code, VkShaderModule* shaderModule) {

		VkShaderModuleCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		createInfo.codeSize = code.size();
		createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

		if (vkCreateShaderModule(lveDevice.device(), &createInfo, nullptr, shaderModule) != VK_SUCCESS) {
			throw std::runtime_error("failed to create shader module");
		}
	}

	void LvePipeline::bind(VkCommandBuffer commandBuffer) {

		vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline);

	}

	void LvePipeline::defaultPipelineConfigInfo(PipelineConfigInfo& configInfo) {

			//First stage of pipeline	//takes a bunch of vertexes and groups them as geometry
		configInfo.inputAssemblyInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
		configInfo.inputAssemblyInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;	//tells what the vertexes group = to (triangle)
		configInfo.inputAssemblyInfo.primitiveRestartEnable = VK_FALSE;

		//Combines viewport and scissor
		configInfo.viewportInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
		configInfo.viewportInfo.viewportCount = 1;
		configInfo.viewportInfo.pViewports = nullptr;
		configInfo.viewportInfo.scissorCount = 1;
		configInfo.viewportInfo.pScissors = nullptr;

		//Rasterization stage
		configInfo.rasterizationInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
		configInfo.rasterizationInfo.depthClampEnable = VK_FALSE;	//Forces Z component of gl positions to be between 0 and 1	//Values below -0 = 0 / +1 = 1
		configInfo.rasterizationInfo.rasterizerDiscardEnable = VK_FALSE;
		configInfo.rasterizationInfo.polygonMode = VK_POLYGON_MODE_FILL;
		configInfo.rasterizationInfo.lineWidth = 1.0f;
		configInfo.rasterizationInfo.cullMode = VK_CULL_MODE_NONE;	//If the triangle is facing away from X it can be culled
		configInfo.rasterizationInfo.frontFace = VK_FRONT_FACE_CLOCKWISE;
		configInfo.rasterizationInfo.depthBiasEnable = VK_FALSE;
		configInfo.rasterizationInfo.depthBiasConstantFactor = 0.0f;  // Optional
		configInfo.rasterizationInfo.depthBiasClamp = 0.0f;           // Optional
		configInfo.rasterizationInfo.depthBiasSlopeFactor = 0.0f;     // Optional

		//How the rasterizer handles the Edges of Geometry
		configInfo.multisampleInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
		configInfo.multisampleInfo.sampleShadingEnable = VK_FALSE;
		configInfo.multisampleInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
		configInfo.multisampleInfo.minSampleShading = 1.0f;           // Optional
		configInfo.multisampleInfo.pSampleMask = nullptr;             // Optional
		configInfo.multisampleInfo.alphaToCoverageEnable = VK_FALSE;  // Optional
		configInfo.multisampleInfo.alphaToOneEnable = VK_FALSE;       // Optional

		//ColorBlending controls how colors are combined in the FrameBuffer
		configInfo.colorBlendAttachment.colorWriteMask =
			VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
		configInfo.colorBlendAttachment.blendEnable = VK_FALSE;
		configInfo.colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;   // Optional
		configInfo.colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;  // Optional
		configInfo.colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;              // Optional
		configInfo.colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;   // Optional
		configInfo.colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;  // Optional
		configInfo.colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;              // Optional

		configInfo.colorBlendInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
		configInfo.colorBlendInfo.logicOpEnable = VK_FALSE;
		configInfo.colorBlendInfo.logicOp = VK_LOGIC_OP_COPY;  // Optional
		configInfo.colorBlendInfo.attachmentCount = 1;
		configInfo.colorBlendInfo.pAttachments = &configInfo.colorBlendAttachment;
		configInfo.colorBlendInfo.blendConstants[0] = 0.0f;  // Optional
		configInfo.colorBlendInfo.blendConstants[1] = 0.0f;  // Optional
		configInfo.colorBlendInfo.blendConstants[2] = 0.0f;  // Optional
		configInfo.colorBlendInfo.blendConstants[3] = 0.0f;  // Optional

		//Depth testing, keeps track of what pixel is Close/Far from the camera
		configInfo.depthStencilInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
		configInfo.depthStencilInfo.depthTestEnable = VK_TRUE;
		configInfo.depthStencilInfo.depthWriteEnable = VK_TRUE;
		configInfo.depthStencilInfo.depthCompareOp = VK_COMPARE_OP_LESS;
		configInfo.depthStencilInfo.depthBoundsTestEnable = VK_FALSE;
		configInfo.depthStencilInfo.minDepthBounds = 0.0f;  // Optional
		configInfo.depthStencilInfo.maxDepthBounds = 1.0f;  // Optional
		configInfo.depthStencilInfo.stencilTestEnable = VK_FALSE;
		configInfo.depthStencilInfo.front = {};  // Optional
		configInfo.depthStencilInfo.back = {};   // Optional

		configInfo.dynamicStateEnables = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR };
		configInfo.dynamicStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
		configInfo.dynamicStateInfo.pDynamicStates = configInfo.dynamicStateEnables.data();
		configInfo.dynamicStateInfo.dynamicStateCount = static_cast<uint32_t>(configInfo.dynamicStateEnables.size());
		configInfo.dynamicStateInfo.flags = 0;

		configInfo.bindingDescriptions = LveModel::Vertex::getBindingDescriptions();
		configInfo.attributeDescriptions = LveModel::Vertex::getAttributeDescriptions();

	}

} //namespace lve