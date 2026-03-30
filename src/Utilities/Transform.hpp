#pragma once
#include "glm/glm.hpp"
#include <functional>

namespace gps {
	struct Transform
	{
		glm::vec3 position;
		glm::vec3 rotation;
		float scale;
		Transform(glm::vec3 pos, glm::vec3 rot, float scl)
			: position(pos), rotation(rot), scale(scl) {}
		Transform() {};

	};
}