#pragma once

#include "vulkEngDevice.hpp"

// lib
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

// std
#include <vector>

namespace VulkanEngine
{
	class VulkEngModel
	{
	public:

		struct Vertex {
			glm::vec3 position;
			glm::vec3 color;

			static std::vector<VkVertexInputBindingDescription> getBindingDescriptions();
			static std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions();
		};

		VulkEngModel(VulkEngDevice &device, const std::vector<Vertex> &vertices);
		~VulkEngModel();

		VulkEngModel(const VulkEngModel&) = delete;
		VulkEngModel& operator=(const VulkEngModel&) = delete;

		void bind(VkCommandBuffer commandBuffer);
		void draw(VkCommandBuffer commandBuffer);


	private:

		void createVertexBuffers(const std::vector<Vertex> &vertices);
		
		VulkEngDevice& vulkanDevice;
		VkBuffer vertexBuffer;
		VkDeviceMemory vertexBufferMemory;
		uint32_t vertexCount;
	};

} // namespace VulkanEngine