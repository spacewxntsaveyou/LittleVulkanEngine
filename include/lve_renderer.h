#pragma once

#include"lve_window.h"
#include"lve_device.h"
#include"lve_swap_chain.h"

//std
#include<memory>
#include <vector>
#include <cassert>

namespace lve {

	class LveRenderer {

	public:

		LveRenderer(LveWindow& window, LveDevice &device);
		~LveRenderer();

		LveRenderer(const LveRenderer&) = delete;
		LveRenderer& operator=(const LveRenderer&) = delete;

		VkRenderPass getSwapChainRenderPass() const { return lveSwapChain->getRenderPass(); }
		bool isFrameInProgress() const { return isFrameStarted; }

		VkCommandBuffer getCurrentCommandBuffer() const { assert(isFrameStarted && "Cannot get command buffer when frame not in progress") return commandBuffers[currentImageIndex]; }

		VkCommandBuffer beginFrame();
		void endFrame();
		void beginSwapChainRenderPass(VkCommandBuffer commandBuffers);
		void endSwapChainRenderPass(VkCommandBuffer commandBuffers);


	private:

		void createCommandBuffers();
		void freeCommandBuffers();
		void recreateSwapChain();


		//Remember that variables are initialized from Top-Bottom and destroyed Bottom-Top
		LveWindow& lveWindow;
		LveDevice& lveDevice;
		std::unique_ptr<LveSwapChain> lveSwapChain;
		std::vector<VkCommandBuffer> commandBuffers;

		uint32_t currentImageIndex;
		bool isFrameStarted{false};

	};

} //namespace lve