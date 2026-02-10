#pragma once

#include "vulkEngModel.hpp"

//libs
#include <glm/gtc/matrix_transform.hpp>

//std
#include <memory>

namespace VulkanEngine {

	struct TransformComponent{
		glm::vec3 translation{};
		glm::vec3 scale{ 1.f, 1.f, 1.f };
		glm::vec3 rotationRadians; // In radians

        // Matrix corrsponds to Translate * Ry * Rx * Rz * Scale
        // Rotations correspond to Tait-bryan angles of Y(1), X(2), Z(3)
        // https://en.wikipedia.org/wiki/Euler_angles#Rotation_matrix
        glm::mat4 mat4() {
            const float c3 = glm::cos(rotationRadians.z);
            const float s3 = glm::sin(rotationRadians.z);
            const float c2 = glm::cos(rotationRadians.x);
            const float s2 = glm::sin(rotationRadians.x);
            const float c1 = glm::cos(rotationRadians.y);
            const float s1 = glm::sin(rotationRadians.y);
            return glm::mat4{
                {
                    scale.x * (c1 * c3 + s1 * s2 * s3),
                    scale.x * (c2 * s3),
                    scale.x * (c1 * s2 * s3 - c3 * s1),
                    0.0f,
                },
                {
                    scale.y * (c3 * s1 * s2 - c1 * s3),
                    scale.y * (c2 * c3),
                    scale.y * (c1 * c3 * s2 + s1 * s3),
                    0.0f,
                },
                {
                    scale.z * (c2 * s1),
                    scale.z * (-s2),
                    scale.z * (c1 * c2),
                    0.0f,
                },
                {translation.x, translation.y, translation.z, 1.0f} };
        }
	};

	class VulkEngGameObj {
	public:
		using id_t = unsigned int;

		static VulkEngGameObj createGameObject() {
			static id_t currentId = 0;
			return VulkEngGameObj{ currentId++ };
		}

		VulkEngGameObj(const VulkEngGameObj&) = delete;
		VulkEngGameObj& operator=(const VulkEngGameObj&) = delete;
		VulkEngGameObj(VulkEngGameObj&&) = default;
		VulkEngGameObj& operator=(VulkEngGameObj&&) = default;

		id_t getId() const { return id; }

		std::shared_ptr<VulkEngModel> model{};
		glm::vec3 color{};
		TransformComponent transform{};

	private:
		VulkEngGameObj(id_t objId) : id{ objId } {}

		id_t id;
	};
} // namespace VulkanEngine