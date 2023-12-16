#pragma once

#include "Renderer/Mesh.h"

#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace Chess {

	struct Node
	{
	public:
		Mesh mesh;
		Node* parent;
		std::vector<Node*> children;

		glm::vec3 position = { 0.0f, 0.0f, 0.0f };
		glm::quat rotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
		glm::vec3 scale = { 1.0f, 1.0f, 1.0f };

		Node(const Mesh& mesh)
			: mesh(mesh) {}
	private:
		glm::mat4 CalculateLocalTransform() const
		{
			return
				glm::translate(glm::mat4(1.0f), position) *
				glm::toMat4(rotation) *
				glm::scale(glm::mat4(1.0f), scale);
		}
	private:
		glm::mat4 m_GlobalTransform = glm::mat4(1.0f);
	private:
		friend class SceneGraph;
	};

}
