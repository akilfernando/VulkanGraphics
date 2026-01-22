#include "vulkanGraphicsApp.hpp"

//std
#include <stdexcept>
#include <array>

namespace VulkanEngine
{

	VulkanGraphicsApp::VulkanGraphicsApp()
	{
		createPipelineLayout();
		createPipeline();
		createCommandBuffers();
	}

	VulkanGraphicsApp::~VulkanGraphicsApp()
	{
		vkDestroyPipelineLayout(vulkanDevice.device(), pipelineLayout, nullptr);
	}

	void VulkanGraphicsApp::run()
	{
		while (!vulkanWindow.shouldClose())
		{
			vulkanWindow.pollEvents();
			drawFrame();
		}

		vkDeviceWaitIdle(vulkanDevice.device());
	}

	void VulkanGraphicsApp::createPipelineLayout() {
		VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
		pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutInfo.setLayoutCount = 0;
		pipelineLayoutInfo.pSetLayouts = nullptr;
		pipelineLayoutInfo.pushConstantRangeCount = 0;
		pipelineLayoutInfo.pPushConstantRanges = nullptr;
		if (vkCreatePipelineLayout(
			vulkanDevice.device(),
			&pipelineLayoutInfo,
			nullptr,
			&pipelineLayout
		) != VK_SUCCESS) {
			throw std::runtime_error("failed to create pipeline layout!");
		}
	}

	void VulkanGraphicsApp::createPipeline() {
		auto pipelineConfig = VulkanEnginePipeline::defaultPipelineConfigInfo(
			vulkanSwapChain.getSwapChainExtent().width,
			vulkanSwapChain.getSwapChainExtent().height
		);
		pipelineConfig.renderPass = vulkanSwapChain.getRenderPass();
		pipelineConfig.pipelineLayout = pipelineLayout;
		vulkanEnginePipeline = std::make_unique<VulkanEnginePipeline>(
			vulkanDevice,
			"shaders/simpleShader.vert.spv",
			"shaders/simpleShader.frag.spv",
			pipelineConfig
		);
	}

	void VulkanGraphicsApp::createCommandBuffers() {

		commandBuffers.resize(vulkanSwapChain.imageCount());

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

		for (int i = 0; i < commandBuffers.size(); i++) {
			VkCommandBufferBeginInfo beginInfo{};
			beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
			beginInfo.flags = 0;
			beginInfo.pInheritanceInfo = nullptr;
			if (vkBeginCommandBuffer(commandBuffers[i], &beginInfo) != VK_SUCCESS) {
				throw std::runtime_error("failed to begin recording command buffer!");
			}

			VkRenderPassBeginInfo renderPassInfo{};
			renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
			renderPassInfo.renderPass = vulkanSwapChain.getRenderPass();
			renderPassInfo.framebuffer = vulkanSwapChain.getFrameBuffer(i);

			renderPassInfo.renderArea.offset = { 0, 0 };
			renderPassInfo.renderArea.extent = vulkanSwapChain.getSwapChainExtent();

			std::array<VkClearValue, 2> clearValues{};
			clearValues[0].color = { {0.01f, 0.01f, 0.01f, 1.0f} };
			clearValues[1].depthStencil = { 1.0f, 0 };
			renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
			renderPassInfo.pClearValues = clearValues.data();

			vkCmdBeginRenderPass(commandBuffers[i], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

			vulkanEnginePipeline->bind(commandBuffers[i]);
			vkCmdDraw(commandBuffers[i], 3, 1, 0, 0);

			vkCmdEndRenderPass(commandBuffers[i]);
			if (vkEndCommandBuffer(commandBuffers[i]) != VK_SUCCESS) {
				throw std::runtime_error("failed to record command buffer!");
			}
		}
	}

	void VulkanGraphicsApp::drawFrame() {
		uint32_t imageIndex;
		VkResult result = vulkanSwapChain.acquireNextImage(&imageIndex);

		if (result == VK_ERROR_OUT_OF_DATE_KHR) {
			// swapchain no longer compatible with window - recreate before next frame
			// (implement swapchain recreation in your VulkanEngineSwapChain or here)
			return;
		} else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
			throw std::runtime_error("failed to acquire swap chain image!");
		}

		result = vulkanSwapChain.submitCommandBuffers(&commandBuffers[imageIndex], &imageIndex);

		if (result == VK_ERROR_OUT_OF_DATE_KHR) {
			// swapchain resized or out of date while presenting -> recreate swapchain
			return;
		} else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
			throw std::runtime_error("failed to present swap chain image!");
		}
	}

} // namespace VulkanEngine