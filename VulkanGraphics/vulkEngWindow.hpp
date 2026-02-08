#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <string>

namespace VulkanEngine {
	class VulkEngWindow {

	public:
		VulkEngWindow(int w, int h, std::string name);
		~VulkEngWindow();

		bool shouldClose() const { return window && glfwWindowShouldClose(window); }

		void pollEvents() { if (window) glfwPollEvents(); }

		VkExtent2D getExtent() { return { static_cast<uint32_t>(width), static_cast<uint32_t>(height) }; }

		bool wasWindowResized() { return framebufferResized; }
		void resetWindowResizedFlag() { framebufferResized = false; }

		void createWindowSurface(VkInstance instance, VkSurfaceKHR* surface);

	private:
		static void framebufferResizeCallback(GLFWwindow* window, int width, int height);

		void initWindow();

		int width;
		int height;
		bool framebufferResized = false;

		std::string windowName;

		GLFWwindow* window = nullptr;
	};
} // namespace VulkanEngine