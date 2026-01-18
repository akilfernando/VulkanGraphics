#include "vulkanEnginePipeline.hpp"

// std
#include <fstream>
#include <stdexcept>

namespace VulkanEngine {

	VulkanEnginePipeline::VulkanEnginePipeline(
		VulkanEngineDevice& device,
		const std::string& vertFilepath,
		const std::string& fragFilepath,
		const PipelineConfigInfo& configInfo
	) : vulkanDevice{device} {
		createGraphicsPipeline(vertFilepath, fragFilepath, configInfo);
	}

	VulkanEnginePipeline::~VulkanEnginePipeline() {
		if (vulkanDevice.device() != VK_NULL_HANDLE) {
			if (graphicsPipeline != VK_NULL_HANDLE) {
				vkDestroyPipeline(vulkanDevice.device(), graphicsPipeline, nullptr);
			}
			if (vertShaderModule != VK_NULL_HANDLE) {
				vkDestroyShaderModule(vulkanDevice.device(), vertShaderModule, nullptr);
			}
			if (fragShaderModule != VK_NULL_HANDLE) {
				vkDestroyShaderModule(vulkanDevice.device(), fragShaderModule, nullptr);
			}
		}
	}

	std::vector<char> VulkanEnginePipeline::readFile(const std::string& filepath) {

		std::ifstream file{ filepath, std::ios::ate | std::ios::binary };

		if (!file.is_open()) {
			throw std::runtime_error("failed to open file: " + filepath);
		}

		size_t fileSize = static_cast<size_t>(file.tellg());
		std::vector<char> buffer(fileSize);

		file.seekg(0);
		file.read(buffer.data(), fileSize);

		file.close();
		return buffer;
	}

	void VulkanEnginePipeline::createGraphicsPipeline(
		const std::string& vertFilepath,
		const std::string& fragFilepath,
		const PipelineConfigInfo& configInfo
	) {
		// Read shader binaries here. Actual Vulkan setup is handled elsewhere.
		auto vertShaderCode = readFile(vertFilepath);
		auto fragShaderCode = readFile(fragFilepath);

		// No debug output here; pipeline creation code should process shader data.
		(void)vertShaderCode;
		(void)fragShaderCode;
	}

	void VulkanEnginePipeline::createShaderModule(
		const std::vector<char>& code,
		VkShaderModule* shaderModule
	) {
		VkShaderModuleCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		createInfo.codeSize = code.size();
		createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

		if (vkCreateShaderModule(
			vulkanDevice.device(),
			&createInfo,
			nullptr,
			shaderModule
		) != VK_SUCCESS) {
			throw std::runtime_error("failed to create shader module!");
		}
	}

	PipelineConfigInfo VulkanEnginePipeline::defaultPipelineConfigInfo(uint32_t width, uint32_t height) {
		PipelineConfigInfo configInfo{};
		// Default configuration setup can be added here.
		(void)width;
		(void)height;
		return configInfo;
	}

} // namespace VulkanEngine