#include "lve_model.h"

//std
#include <cassert>

namespace lve {

	LveModel::LveModel(LveDevice& device, const std::vector<Vertex>& vertices) : lveDevice{device} {

		createVertexBuffers(vertices);

	}
	
	LveModel::~LveModel() {

		vkDestroyBuffer(lveDevice.device, vertexBuffer, nullptr);
		vkFreeMemory(lveDevice.device(), vertexBufferMemory, nullptr);

	}

	void LveModel::createVertexBuffers(const std::vector<Vertex>& vertices) {

		vertexCount = static_cast<uint32_t>(vertices.size());
		assert(vertexCount >= 3 && "Vertex count must atleast be at 3");
		VkDeviceSize bufferSize = sizeof(vertices[0]) * vertexCount;	
		lveDevice.createBuffer(bufferSize, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, vertexBuffer, vertexBufferMemory);	//Tells vulkan the alloc. memory needs to be accessed by host (cpu)

		void* data;
		vkMapMemory(lveDevice.device(), vertexBufferMemory, 0, bufferSize, 0, &data);	//Creates a region of host memory, mapped to device memory, sends data to point at the beggining of the mapped memory (in gpu)
		memcpy(data, vertices.data(), static_cast<size_t>(bufferSize));
		vkUnmapMemory(lveDevice.device(), vertexBufferMemory);

	}

	void LveModel::draw(VkCommandBuffer commandBuffer) {

		//XXXX

	}

} //namespace lve