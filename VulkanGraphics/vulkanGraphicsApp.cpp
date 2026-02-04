#include "vulkanGraphicsApp.hpp"

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

	struct SimplePushConstantData {
		glm::mat2 transform{1.f};
		glm::vec2 offset;
		alignas(16) glm::vec3 color;
	};

	VulkanGraphicsApp::VulkanGraphicsApp()
	{
		loadGameObjects();
		createPipelineLayout();
		recreateSwapChain();
		// createCommandBuffers() is called inside recreateSwapChain() after swapchain is created,
		// so don't call it again here to avoid double-free / double-allocation.
	}

	VulkanGraphicsApp::~VulkanGraphicsApp()
	{
		vkDestroyPipelineLayout(vulkanDevice.device(), pipelineLayout, nullptr);
	}

	void VulkanGraphicsApp::run()
	{
		vkDeviceWaitIdle(vulkanDevice.device());
		while (!vulkanWindow.shouldClose())
		{
			vulkanWindow.pollEvents();
			drawFrame();
		}
	}

	void VulkanGraphicsApp::loadGameObjects() {
		std::vector<VulkanEngineModel::Vertex> vertices{
			{{0.0f, -0.5f}, {1.0f, 0.0f, 0.0f}},
			{{0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},
			{{-0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}} };
		auto vulkanEngineModel = std::make_shared<VulkanEngineModel>(vulkanDevice, vertices);

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
			auto triangle = VulkanEngineGameObject::createGameObject();
			triangle.model = vulkanEngineModel;
			triangle.transform2d.scale = glm::vec2(.5f) + i * 0.025f;
			triangle.transform2d.rotation = i * glm::pi<float>() * .025f;
			triangle.color = colors[i % colors.size()];
			gameObjects.push_back(std::move(triangle));
		}
	}

	void VulkanGraphicsApp::createPipelineLayout() {

		VkPushConstantRange pushConstantRange{};
		pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
		pushConstantRange.offset = 0;
		pushConstantRange.size = sizeof(SimplePushConstantData);

		VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
		pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutInfo.setLayoutCount = 0;
		pipelineLayoutInfo.pSetLayouts = nullptr;
		pipelineLayoutInfo.pushConstantRangeCount = 1;
		pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;
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
		assert(vulkanSwapChain != nullptr && "Cannot create pipeline before swap chain");
		assert(pipelineLayout != nullptr && "Cannot create pipeline before pipeline layout");

		PipelineConfigInfo pipelineConfig{};
		VulkanEnginePipeline::defaultPipelineConfigInfo(
			pipelineConfig
		);
		pipelineConfig.renderPass = vulkanSwapChain->getRenderPass();
		pipelineConfig.pipelineLayout = pipelineLayout;
		vulkanEnginePipeline = std::make_unique<VulkanEnginePipeline>(
			vulkanDevice,
			"shaders/simpleShader.vert.spv",
			"shaders/simpleShader.frag.spv",
			pipelineConfig
		);
	}

	void VulkanGraphicsApp::recreateSwapChain() {
		auto extent = vulkanWindow.getExtent();
		while (extent.width == 0 || extent.height == 0) {
			extent = vulkanWindow.getExtent();
			glfwWaitEvents();
		}

		vkDeviceWaitIdle(vulkanDevice.device());

		// If an old swapchain exists, destroy it so we don't end up with a null pointer later.
		// Resetting unique_ptr ensures old resources are destroyed before creating the new chain.
		if (vulkanSwapChain) {
			vulkanSwapChain.reset();
		}

		// Create a new swapchain for the current extent.
		vulkanSwapChain = std::make_unique<VulkanEngineSwapChain>(vulkanDevice, extent);

		// Create or recreate pipeline which depends on the swapchain's render pass.
		createPipeline();

		// Ensure command buffers match the new swapchain image count.
		// freeCommandBuffers() will noop if commandBuffers is empty.
		if (commandBuffers.size() != vulkanSwapChain->imageCount()) {
			if (!commandBuffers.empty()) {
				freeCommandBuffers();
			}
			createCommandBuffers();
		} else if (commandBuffers.empty()) {
			createCommandBuffers();
		}
	}

	void VulkanGraphicsApp::createCommandBuffers() {

		// If we already have command buffers, free them from the command pool first.
		if (!commandBuffers.empty()) {
			vkFreeCommandBuffers(
				vulkanDevice.device(),
				vulkanDevice.getCommandPool(),
				static_cast<uint32_t>(commandBuffers.size()),
				commandBuffers.data()
			);
		}

		commandBuffers.resize(vulkanSwapChain->imageCount());

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

	void VulkanGraphicsApp::freeCommandBuffers() {
		vkFreeCommandBuffers(
			vulkanDevice.device(),
			vulkanDevice.getCommandPool(),
			static_cast<uint32_t>(commandBuffers.size()),
			commandBuffers.data()
		);
		commandBuffers.clear();
	}

	void VulkanGraphicsApp::recordCommandBuffer(int imageIndex) {
		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags = 0;
		beginInfo.pInheritanceInfo = nullptr;
		if (vkBeginCommandBuffer(commandBuffers[imageIndex], &beginInfo) != VK_SUCCESS) {
			throw std::runtime_error("failed to begin recording command buffer!");
		}

		VkRenderPassBeginInfo renderPassInfo{};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass = vulkanSwapChain->getRenderPass();
		renderPassInfo.framebuffer = vulkanSwapChain->getFrameBuffer(imageIndex);

		renderPassInfo.renderArea.offset = { 0, 0 };
		renderPassInfo.renderArea.extent = vulkanSwapChain->getSwapChainExtent();

		std::array<VkClearValue, 2> clearValues{};
		clearValues[0].color = { {0.01f, 0.01f, 0.01f, 1.0f} };
		clearValues[1].depthStencil = { 1.0f, 0 };
		renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
		renderPassInfo.pClearValues = clearValues.data();

		vkCmdBeginRenderPass(commandBuffers[imageIndex], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

		VkViewport viewport{};
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = static_cast<float>(vulkanSwapChain->getSwapChainExtent().width);
		viewport.height = static_cast<float>(vulkanSwapChain->getSwapChainExtent().height);
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;
		VkRect2D scissor{ {0, 0}, vulkanSwapChain->getSwapChainExtent() };
		vkCmdSetViewport(commandBuffers[imageIndex], 0, 1, &viewport);
		vkCmdSetScissor(commandBuffers[imageIndex], 0, 1, &scissor);

		renderGameObjects(commandBuffers[imageIndex]);

		vkCmdEndRenderPass(commandBuffers[imageIndex]);
		if (vkEndCommandBuffer(commandBuffers[imageIndex]) != VK_SUCCESS) {
			throw std::runtime_error("failed to record command buffer!");
		}
	}

	void VulkanGraphicsApp::renderGameObjects(VkCommandBuffer commandBuffer) {
		// update
		int i = 0;
		for (auto& obj : gameObjects) {
			i += 1;
			obj.transform2d.rotation =
				glm::mod<float>(obj.transform2d.rotation + 0.001f * i, 2.f * glm::pi<float>());
		}

		// render
		vulkanEnginePipeline->bind(commandBuffer);
		for (auto& obj : gameObjects) {
			SimplePushConstantData push{};
			push.offset = obj.transform2d.translation;
			push.color = obj.color;
			push.transform = obj.transform2d.mat2();

			vkCmdPushConstants(
				commandBuffer,
				pipelineLayout,
				VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
				0,
				sizeof(SimplePushConstantData),
				&push);
			obj.model->bind(commandBuffer);
			obj.model->draw(commandBuffer);
		}
	}

	void VulkanGraphicsApp::drawFrame() {
		uint32_t imageIndex;
		VkResult result = vulkanSwapChain->acquireNextImage(&imageIndex);

		if (result == VK_ERROR_OUT_OF_DATE_KHR) {
			recreateSwapChain();
			return;
		}
		
		if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
			throw std::runtime_error("failed to acquire swap chain image!");
		}

		recordCommandBuffer(imageIndex);
		result = vulkanSwapChain->submitCommandBuffers(&commandBuffers[imageIndex], &imageIndex);
		if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || vulkanWindow.wasWindowResized()) {
			vulkanWindow.resetWindowResizedFlag();
			recreateSwapChain();
			return;
		}
		if (result != VK_SUCCESS) {
			throw std::runtime_error("failed to present swap chain image!");
		}
	}

} // namespace VulkanEngine