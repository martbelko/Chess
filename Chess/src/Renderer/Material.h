#pragma once

#include <glm/glm.hpp>

#include <string>

namespace Chess {

	struct Material
	{
		glm::vec4 ambient;
		glm::vec4 diffuse;
		glm::vec4 specular;

		Material(const glm::vec4& ambient, const glm::vec4& diffuse, const glm::vec4& specular)
			: ambient(ambient), diffuse(diffuse), specular(specular) {}
	};

}
