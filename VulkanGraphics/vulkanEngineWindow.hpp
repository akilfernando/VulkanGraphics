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

		void createWindowSurface(VkInstance instance, VkSurfaceKHR* surface);

		void pollEvents() { if (window) glfwPollEvents(); }

	private:
		void initWindow();

		const int width;
		const int height;

		std::string windowName;

		GLFWwindow* window = nullptr;
	};
} // namespace VulkanEngine