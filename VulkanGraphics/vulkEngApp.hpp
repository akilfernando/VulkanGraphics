#pragma once

#include "vulkEngWindow.hpp"
#include "vulkEngPipeline.hpp"
#include "vulkEngDevice.hpp"
#include "vulkEngRenderer.hpp"
#include "vulkEngGameObj.hpp"

//std
#include <memory>
#include <vector>

namespace VulkanEngine {
	
	class VulkEngApp {

	public:
		static constexpr int WIDTH = 800;
		static constexpr int HEIGHT = 600;

		VulkEngApp();
		~VulkEngApp();

		VulkEngApp(const VulkEngApp&) = delete;
		VulkEngApp& operator=(const VulkEngApp&) = delete;

		void run();

	private:
		void loadGameObjects();
		void createPipelineLayout();
		void createPipeline();
		void renderGameObjects(VkCommandBuffer commandBuffer);

		VulkEngWindow vulkanWindow{ WIDTH, HEIGHT, "Vulkan Engine Window" };
		VulkEngDevice vulkanDevice{ vulkanWindow };
		VulkEngRenderer vulkEngRenderer{ vulkanWindow, vulkanDevice };

		std::unique_ptr<VulkEngPipeline> vulkEngPipeline;
		VkPipelineLayout pipelineLayout;
		std::vector<VulkEngGameObj> gameObjects;

	};

} // namespace VulkanEngine