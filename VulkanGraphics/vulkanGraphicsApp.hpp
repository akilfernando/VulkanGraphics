#pragma once

#include "vulkanEngineWindow.hpp"
#include "vulkanEnginePipeline.hpp"
#include "vulkanEngineDevice.hpp"
#include "vulkanEngineSwapChain.hpp"

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
		void createPipelineLayout();
		void createPipeline();
		void createCommandBuffers();
		void drawFrame();

		VulkanEngineWindow vulkanWindow{ WIDTH, HEIGHT, "Vulkan Engine Window" };
		VulkanEngineDevice vulkanDevice{ vulkanWindow };
		VulkanEngineSwapChain vulkanSwapChain{ vulkanDevice, vulkanWindow.getExtent() };
		std::unique_ptr<VulkanEnginePipeline> vulkanEnginePipeline;
		VkPipelineLayout pipelineLayout;
		std::vector<VkCommandBuffer> commandBuffers;

	}; // class VulkanGraphicsApp

} // namespace VulkanEngine