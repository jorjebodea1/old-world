#pragma once

#if defined (__APPLE__)
#include <OpenGL/gl3.h>
#else
#define GLEW_STATIC
#include <GL/glew.h>
#endif

#include <glm/glm.hpp>

#include "Shader.hpp"
#include "Transform.hpp"
#include "glm/gtx/euler_angles.hpp"
#include "Mesh.hpp"
namespace gps {
	class AABB
	{
	public:
		AABB();
		AABB(glm::vec3 min, glm::vec3 max);
		glm::vec3 min;
		glm::vec3 max;
		const AABB getGlobalAABB(Transform globalTransform) const;
		const AABB getGlobalAABB(glm::mat4 model) const;
		void grow(glm::vec3 p);
		void grow(AABB& b);
		bool isColliding(const AABB& other) const;
		bool checkSAT(Triangle& triangle) const;
		glm::vec3 getCenter() const;
		std::vector<glm::vec3> getVertices() const;
		float area() const;
	private:

	};
}