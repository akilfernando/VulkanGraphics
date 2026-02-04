#pragma once

#include "vulkanEngineWindow.hpp"
#include "vulkanEnginePipeline.hpp"
#include "vulkanEngineDevice.hpp"
#include "vulkanEngineSwapChain.hpp"
#include "vulkanEngineGameObject.hpp"

//std
#include <memory>
#include <vector>

namespace VulkanEngine {
	
	class VulkanGraphicsApp {

	public:
		static constexpr int WIDTH = 800;
		static constexpr int HEIGHT = 600;

		VulkanGraphicsApp();
		~VulkanGraphicsApp();

		VulkanGraphicsApp(const VulkanGraphicsApp&) = delete;
		VulkanGraphicsApp& operator=(const VulkanGraphicsApp&) = delete;

		void run();

	private:
		void loadGameObjects();
		void createPipelineLayout();
		void createPipeline();
		void createCommandBuffers();
		void freeCommandBuffers();
		void drawFrame();
		void recreateSwapChain();
		void recordCommandBuffer(int index);
		void renderGameObjects(VkCommandBuffer commandBuffer);

		VulkanEngineWindow vulkanWindow{ WIDTH, HEIGHT, "Vulkan Engine Window" };
		VulkanEngineDevice vulkanDevice{ vulkanWindow };
		std::unique_ptr<VulkanEngineSwapChain> vulkanSwapChain;
		std::unique_ptr<VulkanEnginePipeline> vulkanEnginePipeline;
		VkPipelineLayout pipelineLayout;
		std::vector<VkCommandBuffer> commandBuffers;
		std::vector<VulkanEngineGameObject> gameObjects;

	}; // class VulkanGraphicsApp

} // namespace VulkanEngine