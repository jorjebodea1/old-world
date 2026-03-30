#pragma once

#if defined (__APPLE__)
	#include <OpenGL/gl3.h>
#else
	#define GLEW_STATIC
	#include <GL/glew.h>
#endif

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace gps {
	class Shadow
	{
	public:
		Shadow();
		glm::mat4 computeLightSpaceTrMatrix(glm::vec3 targePosition);
		GLuint shadowMapFBO;
		GLuint depthMapTexture;
	private:
		
		void initFBO();
	};
}