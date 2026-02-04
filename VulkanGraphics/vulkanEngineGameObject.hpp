#pragma once

#include "vulkanEngineModel.hpp"

//std
#include <memory>

namespace VulkanEngine {

	struct Transform2dComponent{
		glm::vec2 translation{};
		glm::vec2 scale{ 1.f, 1.f };
		float rotation; // In radians

		glm::mat2 mat2() { 
			const float s = glm::sin(rotation);
			const float c = glm::cos(rotation);
			glm::mat2 rotMat{ {c, s}, {-s, c} };

			glm::mat2 scaleMat{ {scale.x, .0f}, { .0f, scale.y } };
			return rotMat * scaleMat;
		}
	};

	class VulkanEngineGameObject {
	public:
		using id_t = unsigned int;

		static VulkanEngineGameObject createGameObject() {
			static id_t currentId = 0;
			return VulkanEngineGameObject{ currentId++ };
		}

		VulkanEngineGameObject(const VulkanEngineGameObject&) = delete;
		VulkanEngineGameObject& operator=(const VulkanEngineGameObject&) = delete;
		VulkanEngineGameObject(VulkanEngineGameObject&&) = default;
		VulkanEngineGameObject& operator=(VulkanEngineGameObject&&) = default;

		id_t getId() const { return id; }

		std::shared_ptr<VulkanEngineModel> model{};
		glm::vec3 color{};
		Transform2dComponent transform2d{};

	private:
		VulkanEngineGameObject(id_t objId) : id{ objId } {}

		id_t id;
	};
} // namespace VulkanEngine