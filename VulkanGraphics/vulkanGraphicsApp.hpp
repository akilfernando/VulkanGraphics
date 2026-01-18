#pragma once

#include "vulkanEngineWindow.hpp"
#include "vulkanEnginePipeline.hpp"
#include "vulkanEngineDevice.hpp"

namespace VulkanEngine {
	
	class VulkanGraphicsApp {

	public:
		static constexpr int WIDTH = 800;
		static constexpr int HEIGHT = 600;

		void run();

	private:
		VulkanEngineWindow vulkanWindow{ WIDTH, HEIGHT, "Vulkan Engine Window" };
		VulkanEngineDevice vulkanDevice{ vulkanWindow };
		VulkanEnginePipeline vulkanPipeline{
			vulkanDevice, 
			"shaders/simpleShader.vert.spv", 
			"shaders/simpleShader.frag.spv", 
			VulkanEnginePipeline::defaultPipelineConfigInfo(WIDTH, HEIGHT)};
	};

} // namespace VulkanEngine