#pragma once

#include "lve_camera.h"

//lib
#include <vulkan.h>

namespace lve {

	struct FrameInfo {

		int FrameIndex;
		float frameTime;
		VkCommandBuffer commandBuffer;
		LveCamera &camera

	};

}	//namespace lve