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

	struct SimplePushConstantData {
		glm::mat4 transform{ 1.f };
		alignas(16) glm::vec3 color;
	};

	VulkEngRenderSystem::VulkEngRenderSystem(VulkEngDevice& device, VkRenderPass renderPass) : vulkanDevice{ device }
	{
		createPipelineLayout();
		createPipeline(renderPass);
	}

	VulkEngRenderSystem::~VulkEngRenderSystem()
	{
		vkDestroyPipelineLayout(vulkanDevice.device(), pipelineLayout, nullptr);
	}

	void VulkEngRenderSystem::createPipelineLayout() {

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

	void VulkEngRenderSystem::createPipeline(VkRenderPass renderPass) {
		assert(pipelineLayout != nullptr && "Cannot create pipeline before pipeline layout");

		PipelineConfigInfo pipelineConfig{};
		VulkEngPipeline::defaultPipelineConfigInfo(
			pipelineConfig
		);
		pipelineConfig.renderPass = renderPass;
		pipelineConfig.pipelineLayout = pipelineLayout;
		vulkEngPipeline = std::make_unique<VulkEngPipeline>(
			vulkanDevice,
			"shaders/simpleShader.vert.spv",
			"shaders/simpleShader.frag.spv",
			pipelineConfig
		);
	}

	void VulkEngRenderSystem::renderGameObjects(VkCommandBuffer commandBuffer, std::vector<VulkEngGameObj>& gameObjects) {
		int i = 0;
		for (auto& obj : gameObjects) {
			i += 1;
			obj.transform.rotationRadians.y =
				glm::mod<float>(obj.transform.rotationRadians.y + 0.001f * i, 2.f * glm::pi<float>());
			obj.transform.rotationRadians.x =
				glm::mod<float>(obj.transform.rotationRadians.x + 0.001f * i, 2.f * glm::pi<float>());
		}

		vulkEngPipeline->bind(commandBuffer);
		for (auto& obj : gameObjects) {
			SimplePushConstantData push{};
			push.color = obj.color;
			push.transform = obj.transform.mat4();

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
} // namespace VulkanEngine