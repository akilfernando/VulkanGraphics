#include "vulkanGraphicsApp.hpp"

//std
#include <stdexcept>

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
		}
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
		// Reserve space for command buffers (one per swap chain image).
		// Real allocation and recording logic should be implemented here.
		commandBuffers.resize(vulkanSwapChain.imageCount());
	}

	void VulkanGraphicsApp::drawFrame() {
		// Frame acquisition, command submission and presentation logic goes here.
		// Left as a stub for now to satisfy the linker.
	}

} // namespace VulkanEngine