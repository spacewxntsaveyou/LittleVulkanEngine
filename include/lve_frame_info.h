#pragma once

#include "lve_camera.h"
#include "lve_game_object.h"

//lib
#include <vulkan/vulkan.h>

namespace lve {

	struct GlobalUbo {
		glm::mat4 projection{ 1.f };
		glm::mat4 view{ 1.f };
		glm::vec4 ambientLightColor{ 1.f, 1.f, 1.f, .02f };	//4th is intensity
		glm::vec3 lightPosition{ -1.f };
		alignas (16) glm::vec4 lightColor{ 1.f };	//vec4 = (r, g, b, intensity)
	};

	struct FrameInfo {

		int frameIndex;
		float frameTime;
		VkCommandBuffer commandBuffer;
		LveCamera& camera;
		VkDescriptorSet globalDescriptorSet;
		LveGameObject::Map& gameObjects;

	};

}	//namespace lve