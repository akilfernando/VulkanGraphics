#pragma once

#include "vulkEngPipeline.hpp"
#include "vulkEngDevice.hpp"
#include "vulkEngGameObj.hpp"

//std
#include <memory>
#include <vector>

namespace VulkanEngine {

	class VulkEngRenderSystem {

	public:
		VulkEngRenderSystem(VulkEngDevice& device, VkRenderPass renderPass);
		~VulkEngRenderSystem();

		VulkEngRenderSystem(const VulkEngRenderSystem&) = delete;
		VulkEngRenderSystem& operator=(const VulkEngRenderSystem&) = delete;

		void renderGameObjects(VkCommandBuffer commandBuffer, std::vector<VulkEngGameObj> &gameObjects);

	private:
		void createPipelineLayout();
		void createPipeline(VkRenderPass renderPass);

		VulkEngDevice& vulkanDevice;

		std::unique_ptr<VulkEngPipeline> vulkEngPipeline;
		VkPipelineLayout pipelineLayout;
	};

} // namespace VulkanEngine