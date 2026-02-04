#include "first_app.h"
#include "keyboard_movement_controller.h"
#include "simple_render_system.h"
#include "lve_camera.h"

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

	FirstApp::FirstApp() { loadGameObjects(); }

	FirstApp::~FirstApp() {}


	void FirstApp::run() {

		SimpleRenderSystem simpleRenderSystem{ lveDevice, lveRenderer.getSwapChainRenderPass() };
        LveCamera camera{};

     camera.setViewTarget(glm::vec3(-1.f, -2.f, 2.f), glm::vec3(0.f, 0.f, 2.5f));

     auto viewerObject = LveGameObject::createGameObject(); //Used to store the camera's current state
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
           camera.setPerspectiveProjection(glm::radians(50.f), aspect, 0.1f, 10.f);    //Perspective projection
			
			if (auto commandBuffer = lveRenderer.beginFrame()) {

				//begin offscreen shadow pass
				//render shadow casting objects
				//end offscreen shadow pass
				lveRenderer.beginSwapChainRenderPass(commandBuffer);
				simpleRenderSystem.renderGameObjects(commandBuffer, gameObjects, camera);
				lveRenderer.endSwapChainRenderPass(commandBuffer);
				lveRenderer.endFrame();

			}
		};

		vkDeviceWaitIdle(lveDevice.device());	//CPU will block until GPU operations have completed	//Avoids console spam at termination
	}

   

	void FirstApp::loadGameObjects() {

		std::shared_ptr<LveModel> lveModel = LveModel::createModelFromFile(lveDevice, "ressources/models/smooth_vase.obj");
        auto gameObj = LveGameObject::createGameObject();
		gameObj.model = lveModel;
		gameObj.transform.translation = { .0f, .0f, 2.5f };
		gameObj.transform.scale = glm::vec3(3.f);
        gameObjects.push_back(std::move(gameObj));

	}

} //namespace lve