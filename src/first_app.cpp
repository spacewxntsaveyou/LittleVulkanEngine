#include "first_app.h"
#include "keyboard_movement_controller.h"
#include "simple_render_system.h"
#include "lve_camera.h"
#include "lve_buffer.h"

//Lib
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE		//Uses correct orientation for vulkan (-1 -> 1)
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

//std
#include <stdexcept>
#include <array>
#include <chrono>
#include <cassert>

namespace lve {

	struct GlobalUbo {
		glm::mat4 projectionView{ 1.f };
		glm::vec4 ambientLightColor{ 1.f, 1.f, 1.f, .02f };	//4th is intensity
		glm::vec3 lightPosition{ -1.f };
		alignas (16) glm::vec4 lightColor{ 1.f };	//vec4 = (r, g, b, intensity)
	};

	FirstApp::FirstApp() { 
		globalPool = LveDescriptorPool::Builder(lveDevice)
			.setMaxSets(LveSwapChain::MAX_FRAMES_IN_FLIGHT)
			.addPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, LveSwapChain::MAX_FRAMES_IN_FLIGHT)
			.build();

		loadGameObjects(); }
	FirstApp::~FirstApp() {}


	void FirstApp::run() {

		std::vector<std::unique_ptr<LveBuffer>> uboBuffers(LveSwapChain::MAX_FRAMES_IN_FLIGHT);

		for (int i = 0; i < uboBuffers.size(); i++) {

			uboBuffers[i] = std::make_unique<LveBuffer>(
				lveDevice,
				sizeof(GlobalUbo),
				1,
				VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
				VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT );
			uboBuffers[i]->map();
		}

		auto globalSetLayout = LveDescriptorSetLayout::Builder(lveDevice)
			.addBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT)
			.build();

		std::vector<VkDescriptorSet> globalDescriptorSets(LveSwapChain::MAX_FRAMES_IN_FLIGHT);
		for (int i = 0; i < globalDescriptorSets.size(); i++) {

			auto bufferInfo = uboBuffers[i]->descriptorInfo();
			LveDescriptorWriter(*globalSetLayout, *globalPool)
				.writeBuffer(0, &bufferInfo)
				.build(globalDescriptorSets[i]);
		}


		SimpleRenderSystem simpleRenderSystem{ lveDevice, lveRenderer.getSwapChainRenderPass(), globalSetLayout->getDescriptorSetLayout()};

        LveCamera camera{};
		camera.setViewTarget(glm::vec3(-1.f, -2.f, 2.f), glm::vec3(0.f, 0.f, 2.5f));
		auto viewerObject = LveGameObject::createGameObject(); //Used to store the camera's current state
		viewerObject.transform.translation.z = -2.5f;
		KeyboardMovementController cameraController{};


	  auto currentTime = std::chrono::high_resolution_clock::now();

		while (!lveWindow.shouldClose()) {

			glfwPollEvents();

            auto newTime = std::chrono::high_resolution_clock::now();
            float frameTime = std::chrono::duration<float, std::chrono::seconds::period>(newTime - currentTime).count();
            currentTime = newTime;

			//Limits camera mouvement while resizing occurs
			float MAX_FRAMES_TIME = 120;
			frameTime = glm::min(frameTime, MAX_FRAMES_TIME);	

            cameraController.moveInPLaneXZ(lveWindow.getGLFWwindow(), frameTime, viewerObject);
            camera.setViewYXZ(viewerObject.transform.translation, viewerObject.transform.rotation);

            float aspect = lveRenderer.getAspectRatio();
           camera.setPerspectiveProjection(glm::radians(50.f), aspect, 0.1f, 100.f);    //Perspective projection
			
			if (auto commandBuffer = lveRenderer.beginFrame()) {

				int frameIndex = lveRenderer.getFrameIndex();
				FrameInfo frameInfo{ frameIndex, frameTime, commandBuffer, camera, globalDescriptorSets[frameIndex]};

				//	update
				GlobalUbo ubo{};
				ubo.projectionView = camera.getProjection() * camera.getView();
				uboBuffers[frameIndex]->writeToBuffer(&ubo);
				uboBuffers[frameIndex]->flush();

				//	render
				lveRenderer.beginSwapChainRenderPass(commandBuffer);
				simpleRenderSystem.renderGameObjects(frameInfo, gameObjects);
				lveRenderer.endSwapChainRenderPass(commandBuffer);
				lveRenderer.endFrame();

			}
		};

		vkDeviceWaitIdle(lveDevice.device());	//CPU will block until GPU operations have completed	//Avoids console spam at termination
	}

   

	void FirstApp::loadGameObjects() {

		//std::shared_ptr<LveModel> lveModel = LveModel::createModelFromFile(lveDevice, "ressources/models/flat_vase.obj");
        //auto gameObj = LveGameObject::createGameObject();
		//gameObj.model = lveModel;
		//gameObj.transform.translation = { .0f, .5f, 2.5f };
		//gameObj.transform.scale = { 3.f, 1.5, 3.f };
        //gameObjects.push_back(std::move(gameObj));

		std::shared_ptr<LveModel> lveModel = LveModel::createModelFromFile(lveDevice, "ressources/models/flat_vase.obj");
		auto flatVase = LveGameObject::createGameObject();
		flatVase.model = lveModel;
		flatVase.transform.translation = { -.5f, .5f, 0.f };
		flatVase.transform.scale = { 3.f, 1.5, 3.f };
		gameObjects.push_back(std::move(flatVase));

		lveModel = LveModel::createModelFromFile(lveDevice, "ressources/models/smooth_vase.obj");
		auto smoothVase = LveGameObject::createGameObject();
		smoothVase.model = lveModel;
		smoothVase.transform.translation = { .5f, .5f, 0.f };
		smoothVase.transform.scale = { 3.f, 1.5, 3.f };
		gameObjects.push_back(std::move(smoothVase));


		lveModel = LveModel::createModelFromFile(lveDevice, "ressources/models/quad.obj");
		auto quad = LveGameObject::createGameObject();
		quad.model = lveModel;
		quad.transform.translation = { .0f, .5f, 0.f };
		quad.transform.scale = { 3.f, 1., 3.f };
		gameObjects.push_back(std::move(quad));

	}

} //namespace lve