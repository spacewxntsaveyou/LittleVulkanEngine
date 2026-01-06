#pragma once

#include"lve_device.h"

namespace lve {

	//Class takes vertex data created/read on cpu, then copies/allocate to gpu
	class LveModel {

	public:

		LveModel();
		~LveModel();

		LveModel(const LveModel&) = delete;
		LveModel& operator=(const LveModel&) = delete;

		void bind(VkCommandBuffer commandBuffer);
		void draw(VkCommandBuffer commandBuffer);

	private:

		LveDevice& lveDevice;
		VkBuffer vertexBuffer;
		VkDeviceMemory vertexBufferMemory;
		uint32_t vertexCount;
	};

}//namespace lve