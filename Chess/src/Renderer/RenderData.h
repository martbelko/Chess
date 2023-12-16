#pragma once

#include <glm/glm.hpp>

namespace Chess {

	struct Vertex3D
	{
		glm::vec3 position;
		glm::vec3 normal;
		glm::vec2 uv;
	};

}