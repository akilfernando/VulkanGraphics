#include "vulkanEngineWindow.hpp"

#include <stdexcept>
#include "vulkanEngineDevice.hpp"

namespace VulkanEngine {
	VulkanEngineWindow::VulkanEngineWindow(int w, int h, std::string name) : width{ w }, height{ h }, windowName{ name } {
		initWindow();
	}
	VulkanEngineWindow::~VulkanEngineWindow() {
		if (window) {
			glfwDestroyWindow(window);
		}
		// Terminate GLFW if it was initialized; glfwTerminate is safe to call even if window is null
		glfwTerminate();
	}
	void VulkanEngineWindow::initWindow() {
		if (!glfwInit()) {
			throw std::runtime_error("Failed to initialize GLFW");
		}

		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

		window = glfwCreateWindow(width, height, windowName.c_str(), nullptr, nullptr);
		glfwSetWindowUserPointer(window, this);
		glfwSetFramebufferSizeCallback(window, framebufferResizeCallback);
		if (!window) {
			glfwTerminate();
			throw std::runtime_error("Failed to create GLFW window");
		}
	}

	void VulkanEngineWindow::createWindowSurface(VkInstance instance, VkSurfaceKHR* surface) {
		if (glfwCreateWindowSurface(instance, window, nullptr, surface) != VK_SUCCESS) {
			throw std::runtime_error("Failed to create window surface");
		}
	}

	void VulkanEngineWindow::framebufferResizeCallback(GLFWwindow* window, int width, int height) {
		auto vulkanWindow = reinterpret_cast<VulkanEngineWindow*>(glfwGetWindowUserPointer(window));
		vulkanWindow->framebufferResized = true;
		vulkanWindow->width = width;
		vulkanWindow->height = height;
	}

} // namespace VulkanEngine