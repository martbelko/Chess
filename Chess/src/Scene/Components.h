#pragma once

#include "Core/UUID.h"

#include "Renderer/Mesh.h"

#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>

#include <string>

namespace Chess {

	struct IDComponent
	{
		UUID id;
	};

	struct TagComponent
	{
		std::string tag;
	};

	struct TransformComponent
	{
		glm::vec3 translation = { 0.0f, 0.0f, 0.0f };
		glm::quat rotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
		float scale = 1.0f;

		glm::mat4 worldMatrix = glm::mat4(1.0f);

		TransformComponent(const glm::vec3& translation = glm::vec3(0.0f, 0.0f, 0.0f))
			: translation(translation) {}

		glm::mat4 CalculateLocalTransform() const
		{
			return
				glm::translate(glm::mat4(1.0f), translation) *
				glm::toMat4(rotation) *
				glm::scale(glm::mat4(1.0f), { scale, scale, scale });
		}
	};

	struct MeshComponent
	{
		Ref<Mesh> mesh;
		Ref<Material> material;

		MeshComponent(const Ref<Mesh>& mesh, const Ref<Material> material)
			: mesh(mesh), material(material) {}
	};

}
