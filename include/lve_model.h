#pragma once

#include"lve_device.h"

//lib
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE		//Uses correct orientation for vulkan (-1 -> 1)
#include <glm/glm.hpp>

//std
#include <vector>


namespace lve {

	//Class takes vertex data created/read on cpu, then copies/allocate to gpu
	class LveModel {

	public:

		struct Vertex {

			glm::vec3 position;
			glm::vec3 color;

			static std::vector<VkVertexInputBindingDescription> getBindingDescriptions();
			static std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions();

		};

		LveModel(LveDevice &device, const std::vector<Vertex>& vertices);
		~LveModel();

		LveModel(const LveModel&) = delete;
		LveModel& operator=(const LveModel&) = delete;

		void bind(VkCommandBuffer commandBuffer);
		void draw(VkCommandBuffer commandBuffer);

	private:

		void createVertexBuffers(const std::vector<Vertex>& vertices);

		LveDevice& lveDevice;
		VkBuffer vertexBuffer;
		VkDeviceMemory vertexBufferMemory;
		uint32_t vertexCount;
	};

}//namespace lve