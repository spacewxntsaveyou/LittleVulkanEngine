#pragma once

#include"lve_window.h"
#include"lve_pipeline.h"
#include"lve_device.h"
#include"lve_renderer.h"
#include"lve_game_object.h"

//std
#include<memory>
#include <vector>

namespace lve {

	class FirstApp {

	public:

		static constexpr int WIDTH = 800;
		static constexpr int HEIGHT = 600;

		FirstApp();
		~FirstApp();

		FirstApp(const FirstApp&) = delete;
		FirstApp& operator=(const FirstApp&) = delete;

		void run();

	private: 

		void loadGameObjects();
		void createPipelineLayout();
		void createPipeline();
		void renderGameObjects(VkCommandBuffer commandBuffer);

			//Remember that variables are initialized from Top-Bottom and destroyed Bottom-Top
		LveWindow lveWindow{ WIDTH, HEIGHT, "Le Vulkan"};
		LveDevice lveDevice{ lveWindow };
		LveRenderer lveRenderer{ lveWindow, lveDevice };
		std::unique_ptr<LvePipeline> lvePipeline;	
		VkPipelineLayout pipelineLayout;
		std::vector<LveGameObject> gameObjects;

	};

} //namespace lve