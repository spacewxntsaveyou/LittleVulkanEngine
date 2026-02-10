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

	class PointLightSystem {

	public:

		PointLightSystem(LveDevice &device, VkRenderPass renderPass, VkDescriptorSetLayout globalsetLayout);
		~PointLightSystem();

		PointLightSystem(const PointLightSystem&) = delete;
		PointLightSystem& operator=(const PointLightSystem&) = delete;

		void render(FrameInfo &frameInfo);

	private:

		void createPipelineLayout(VkDescriptorSetLayout globalsetLayout);
		void createPipeline(VkRenderPass renderPass);

		//Remember that variables are initialized from Top-Bottom and destroyed Bottom-Top
		LveDevice& lveDevice;
		std::unique_ptr<LvePipeline> lvePipeline;
		VkPipelineLayout pipelineLayout;

	};

} //namespace lve