#include "vulkEngApp.hpp"
#include "vulkEngRenderSystem.hpp"

//libs
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

//std
#include <stdexcept>
#include <array>
#include <vector>

namespace VulkanEngine
{
	VulkEngApp::VulkEngApp()
	{
		loadGameObjects();
	}

	VulkEngApp::~VulkEngApp() {}

	void VulkEngApp::run()
	{
		VulkEngRenderSystem vulkEngRenderSystem{ vulkanDevice, vulkEngRenderer.getSwapChainRenderPass() };

		vkDeviceWaitIdle(vulkanDevice.device());
		while (!vulkanWindow.shouldClose())
		{
			vulkanWindow.pollEvents();
			
			if (auto commandBuffer = vulkEngRenderer.beginFrame()) {
				vulkEngRenderer.beginSwapChainRenderPass(commandBuffer);
				vulkEngRenderSystem.renderGameObjects(commandBuffer, gameObjects);
				vulkEngRenderer.endSwapChainRenderPass(commandBuffer);
				vulkEngRenderer.endFrame();
			}
		}
	}

	void VulkEngApp::loadGameObjects() {
		std::vector<VulkEngModel::Vertex> vertices{
			{{0.0f, -0.5f}, {1.0f, 0.0f, 0.0f}},
			{{0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},
			{{-0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}} };
		auto vulkEngModel = std::make_shared<VulkEngModel>(vulkanDevice, vertices);

		// https://www.color-hex.com/color-palette/5361
		std::vector<glm::vec3> colors{
			{1.f, .7f, .73f},
			{1.f, .87f, .73f},
			{1.f, 1.f, .73f},
			{.73f, 1.f, .8f},
			{.73, .88f, 1.f}  //
		};
		for (auto& color : colors) {
			color = glm::pow(color, glm::vec3{ 2.2f });
		}
		for (int i = 0; i < 40; i++) {
			auto triangle = VulkEngGameObj::createGameObject();
			triangle.model = vulkEngModel;
			triangle.transform2d.scale = glm::vec2(.5f) + i * 0.025f;
			triangle.transform2d.rotation = i * glm::pi<float>() * .025f;
			triangle.color = colors[i % colors.size()];
			gameObjects.push_back(std::move(triangle));
		}
	}
} // namespace VulkanEngine