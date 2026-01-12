#pragma once

#include"lve_window.h"
#include"lve_pipeline.h"
#include"lve_device.h"
#include"lve_swap_chain.h"
#include"lve_model.h"

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

		void loadModels();
		void createPipelineLayout();
		void createPipeline();
		void createCommandBuffers();
		void freeCommandBuffers();
		void drawFrame();
		void recreateSwapChain();
		void recordCommandBuffer(int imageIndex);

			//Remember that variables are initialized from Top-Bottom and destroyed Bottom-Top
		LveWindow lveWindow{ WIDTH, HEIGHT, "Le Vulkan"};
		LveDevice lveDevice{ lveWindow };
		std::unique_ptr<LveSwapChain> lveSwapChain;
		std::unique_ptr<LvePipeline> lvePipeline;	
		VkPipelineLayout pipelineLayout;
		std::vector<VkCommandBuffer> commandBuffers;
		std::unique_ptr<LveModel> lveModel;

	};

} //namespace lve