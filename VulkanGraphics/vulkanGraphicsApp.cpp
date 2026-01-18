#include "vulkanGraphicsApp.hpp"

namespace VulkanEngine
{

	void VulkanGraphicsApp::run()
	{
		while (!vulkanWindow.shouldClose())
		{
			vulkanWindow.pollEvents();
		}
	}

} // namespace VulkanEngine