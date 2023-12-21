#pragma once

#include "Renderer/VertexBufferView.h"
#include "Renderer/Material.h"

#include <glm/glm.hpp>

namespace Chess {

	struct BoundingBox
	{
		glm::vec3 min = glm::vec3(std::numeric_limits<float>::max());
		glm::vec3 max = glm::vec3(std::numeric_limits<float>::min());

		void Add(const glm::vec3& p)
		{
			min = glm::min(min, p);
			max = glm::max(max, p);
		}
	};

	struct Mesh
	{
		VertexBufferView vboView;
		BoundingBox bbox;

		Mesh(const VertexBufferView& vboView, const BoundingBox& bbox)
			: vboView(vboView), bbox(bbox) {}
	};

}
