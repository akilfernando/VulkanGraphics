#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <string>

namespace VulkanEngine {
	class VulkanEngineWindow {

	public:
		VulkanEngineWindow(int w, int h, std::string name);
		~VulkanEngineWindow();

		bool shouldClose() const { return window && glfwWindowShouldClose(window); }

		void pollEvents() { if (window) glfwPollEvents(); }

		VkExtent2D getExtent() { return { static_cast<uint32_t>(width), static_cast<uint32_t>(height) }; }

		void createWindowSurface(VkInstance instance, VkSurfaceKHR* surface);

	private:
		void initWindow();

		const int width;
		const int height;

		std::string windowName;

		GLFWwindow* window = nullptr;
	};
} // namespace VulkanEngine