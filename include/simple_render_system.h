#pragma once

#include "lve_pipeline.h"
#include "lve_device.h"
#include "lve_game_object.h"
#include "lve_camera.h"
#include "lve_frame_info.h"

//std
#include<memory>
#include <vector>

namespace lve {

	class SimpleRenderSystem {

	public:

		SimpleRenderSystem(LveDevice &device, VkRenderPass renderPass);
		~SimpleRenderSystem();

		SimpleRenderSystem(const SimpleRenderSystem&) = delete;
		SimpleRenderSystem& operator=(const SimpleRenderSystem&) = delete;

		void renderGameObjects(FrameInfo &frameInfo, std::vector<LveGameObject>& gameObjects);

	private:

		void createPipelineLayout();
		void createPipeline(VkRenderPass renderPass);

		//Remember that variables are initialized from Top-Bottom and destroyed Bottom-Top
		LveDevice& lveDevice;
		std::unique_ptr<LvePipeline> lvePipeline;
		VkPipelineLayout pipelineLayout;

	};

} //namespace lve