#pragma once

#include "vulkEngWindow.hpp"
#include "vulkEngDevice.hpp"
#include "vulkEngSwapChain.hpp"

//std
#include <memory>
#include <vector>
#include <cassert>

namespace VulkanEngine {

	class VulkEngRenderer {

	public:
		VulkEngRenderer(VulkEngWindow& window, VulkEngDevice& device);
		~VulkEngRenderer();

		VulkEngRenderer(const VulkEngRenderer&) = delete;
		VulkEngRenderer& operator=(const VulkEngRenderer&) = delete;

		VkRenderPass getSwapChainRenderPass() const { return vulkSwapChain->getRenderPass(); }
		bool isFrameInProgress() const { return isFrameStarted; }

		VkCommandBuffer getCurrentCommandBuffer() const {
			assert(isFrameStarted && "Cannot get command buffer when frame not in progress");
			return commandBuffers[currentFrameIndex];
		}

		int getFrameIndex() const {
			assert(isFrameStarted && "Cannot get frame index when frame not in progress");
			return currentFrameIndex;
		}

		VkCommandBuffer beginFrame();
		void endFrame();
		void beginSwapChainRenderPass(VkCommandBuffer commandBuffer);
		void endSwapChainRenderPass(VkCommandBuffer commandBuffer);

	private:
		void createCommandBuffers();
		void freeCommandBuffers();
		void recreateSwapChain();

		VulkEngWindow& vulkanWindow;
		VulkEngDevice& vulkanDevice;
		std::unique_ptr<VulkEngSwapChain> vulkSwapChain;
		std::vector<VkCommandBuffer> commandBuffers;

		uint32_t currentImageIndex;
		int currentFrameIndex;
		bool isFrameStarted;
	};

} // namespace VulkanEngine