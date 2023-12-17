#pragma once

#include "Renderer/Mesh.h"

#include "Scene/Entity.h"

#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace Chess {

	struct Node
	{
	public:
		Entity entity;
		Node* parent;
		std::vector<Node*> children;

		Node(Entity entity)
			: entity(entity) {}
	private:
		friend class SceneGraph;
	};

}
