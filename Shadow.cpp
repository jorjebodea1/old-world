#include "Shadow.hpp"

const unsigned int SHADOW_WIDTH = 16384;
const unsigned int SHADOW_HEIGHT = 16384;


namespace gps {
	Shadow::Shadow() {
		shadowMapFBO = 0;
		depthMapTexture = 0;
		initFBO();
	}
	void Shadow::initFBO() {
		glGenFramebuffers(1, &shadowMapFBO);
		glGenTextures(1, &depthMapTexture);
		glBindTexture(GL_TEXTURE_2D, depthMapTexture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
			SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
		glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
		//generate FBO ID
		glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMapTexture,
			0);
		glDrawBuffer(GL_NONE);
		glReadBuffer(GL_NONE);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
	glm::mat4 Shadow::computeLightSpaceTrMatrix(glm::vec3 targetPosition) {
		//glm::mat4 lightModel = glm::rotate(glm::mat4(1.0f), glm::radians(-lightAngle), glm::vec3(0.0f, 1.0f, 0.0f));;
		glm::mat4 lightView = glm::lookAt(glm::vec3(3676.6f, 271.114f+200.f, 899.373f+200.f),glm::vec3(3676.6f, 271.114f, 899.373f), glm::vec3(0.0f, 1.0f, 0.0f));
		const GLfloat near_plane = 0.1f, far_plane = 10000.0f;
		glm::mat4 lightProjection = glm::ortho(-2000.0f,2000.0f,-2000.0f,2000.0f, near_plane, far_plane);

		return lightProjection * lightView;
	}

}