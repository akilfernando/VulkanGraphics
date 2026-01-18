#include "vulkanGraphicsApp.hpp"
#include "vulkanEnginePipeline.hpp"

#include <filesystem>
#include <iostream>
#include <thread>
#include <chrono>

namespace VulkanEngine
{

	void VulkanGraphicsApp::run()
	{
		// Diagnostic: print current working directory to help resolve relative shader paths
		std::cout << "CWD: " << std::filesystem::current_path().string() << '\n';

		// Instantiate pipeline (adjust paths if necessary). This will call readFile() and print sizes.
		try {
			VulkanEnginePipeline pipeline{ "shaders/simpleShader.vert", "shaders/simpleShader.frag" };
		}
		catch (const std::exception& e) {
			std::cerr << "Pipeline error: " << e.what() << '\n';
			return; // stop if shader files not found or other pipeline error
		}

		// Main loop: poll events through the window wrapper
		while (!vulkanWindow.shouldClose())
		{
			vulkanWindow.pollEvents();
			// keep the loop from spinning too hot
			std::this_thread::sleep_for(std::chrono::milliseconds(8));
		}
	}

} // namespace VulkanEngine