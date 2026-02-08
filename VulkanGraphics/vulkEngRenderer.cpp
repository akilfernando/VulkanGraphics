#include "vulkEngRenderer.hpp"

//std
#include <stdexcept>
#include <array>
#include <vector>

namespace VulkanEngine
{
	VulkEngRenderer::VulkEngRenderer(VulkEngWindow& window, VulkEngDevice& device) 
		: vulkanWindow{ window }, vulkanDevice{ device }
	{
		recreateSwapChain();
		createCommandBuffers();
	}

	VulkEngRenderer::~VulkEngRenderer() { freeCommandBuffers(); }

	void VulkEngRenderer::recreateSwapChain() {
		auto extent = vulkanWindow.getExtent();
		while (extent.width == 0 || extent.height == 0) {
			extent = vulkanWindow.getExtent();
			glfwWaitEvents();
		}

		vkDeviceWaitIdle(vulkanDevice.device());

		if (vulkSwapChain) {
			vulkSwapChain.reset();
		}

		vulkSwapChain = std::make_unique<VulkEngSwapChain>(vulkanDevice, extent);

		if (commandBuffers.size() != vulkSwapChain->imageCount()) {
			if (!commandBuffers.empty()) {
				freeCommandBuffers();
			}
			createCommandBuffers();
		}
		else if (commandBuffers.empty()) {
			createCommandBuffers();
		}
	}

	void VulkEngRenderer::createCommandBuffers() {

		if (!commandBuffers.empty()) {
			vkFreeCommandBuffers(
				vulkanDevice.device(),
				vulkanDevice.getCommandPool(),
				static_cast<uint32_t>(commandBuffers.size()),
				commandBuffers.data()
			);
		}

		commandBuffers.resize(vulkSwapChain->imageCount());

		VkCommandBufferAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandPool = vulkanDevice.getCommandPool();
		allocInfo.commandBufferCount = static_cast<uint32_t>(commandBuffers.size());
		if (vkAllocateCommandBuffers(
			vulkanDevice.device(),
			&allocInfo,
			commandBuffers.data()
		) != VK_SUCCESS) {
			throw std::runtime_error("failed to allocate command buffers!");
		}
	}

	void VulkEngRenderer::freeCommandBuffers() {
		vkFreeCommandBuffers(
			vulkanDevice.device(),
			vulkanDevice.getCommandPool(),
			static_cast<uint32_t>(commandBuffers.size()),
			commandBuffers.data()
		);
		commandBuffers.clear();
	}

	VkCommandBuffer VulkEngRenderer::beginFrame() {
		assert(!isFrameStarted && "Cannot call beginFrame while already in progress");

		VkResult result = vulkSwapChain->acquireNextImage(&currentImageIndex);
		if (result == VK_ERROR_OUT_OF_DATE_KHR) {
			recreateSwapChain();
			return nullptr;
		}

		if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
			throw std::runtime_error("failed to acquire swap chain image!");
		}

		isFrameStarted = true;

		VkCommandBuffer commandBuffer = getCurrentCommandBuffer();
		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags = 0;
		beginInfo.pInheritanceInfo = nullptr;
		if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) {
			throw std::runtime_error("failed to begin recording command buffer!");
		}
		return commandBuffer;
	}
	void VulkEngRenderer::endFrame() {
		assert(isFrameStarted && "Cannot call endFrame while frame not in progress");
		VkCommandBuffer commandBuffer = getCurrentCommandBuffer();
		if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
			throw std::runtime_error("failed to record command buffer!");
		}

		auto result = vulkSwapChain->submitCommandBuffers(&commandBuffer, &currentImageIndex);
		if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || vulkanWindow.wasWindowResized()) {
			vulkanWindow.resetWindowResizedFlag();
			recreateSwapChain();
		}
		else if (result != VK_SUCCESS) {
			throw std::runtime_error("failed to present swap chain image!");
		}
		
		isFrameStarted = false;
	}
	void VulkEngRenderer::beginSwapChainRenderPass(VkCommandBuffer commandBuffer) {
		assert(isFrameStarted && "Cannot call beginSwapChainRenderPass if frame not in progress");
		assert(commandBuffer == getCurrentCommandBuffer() && "Cannot begin render pass on command buffer from a different frame");

		VkRenderPassBeginInfo renderPassInfo{};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass = vulkSwapChain->getRenderPass();
		renderPassInfo.framebuffer = vulkSwapChain->getFrameBuffer(currentImageIndex);

		renderPassInfo.renderArea.offset = { 0, 0 };
		renderPassInfo.renderArea.extent = vulkSwapChain->getSwapChainExtent();

		std::array<VkClearValue, 2> clearValues{};
		clearValues[0].color = { {0.01f, 0.01f, 0.01f, 1.0f} };
		clearValues[1].depthStencil = { 1.0f, 0 };
		renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
		renderPassInfo.pClearValues = clearValues.data();

		vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

		VkViewport viewport{};
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = static_cast<float>(vulkSwapChain->getSwapChainExtent().width);
		viewport.height = static_cast<float>(vulkSwapChain->getSwapChainExtent().height);
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;
		VkRect2D scissor{ {0, 0}, vulkSwapChain->getSwapChainExtent() };
		vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
		vkCmdSetScissor(commandBuffer, 0, 1, &scissor);
	}


	void VulkEngRenderer::endSwapChainRenderPass(VkCommandBuffer commandBuffer) {
		assert(isFrameStarted && "Cannot call endSwapChainRenderPass if frame not in progress");
		assert(commandBuffer == getCurrentCommandBuffer() && "Cannot end render pass on command buffer from a different frame");

		vkCmdEndRenderPass(commandBuffer);
	}

} // namespace VulkanEngine