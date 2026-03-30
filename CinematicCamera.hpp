#pragma once
#include "cgltf.h";
#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "AnimationClip.hpp"
#include <unordered_map>
namespace gps {
	class CinematicCamera {
	public:
		void playCinematic(std::string cinematicName);
		glm::mat4 getViewMatrix(float delta);
		void LoadCinematicCamera();
		bool isActive=false;
	private:
		glm::vec3 cameraPosition;
		glm::quat cameraRotation;
		float currentTime = 0.0f;
		AnimationClip* currentCinematic = NULL;
		std::unordered_map<std::string, AnimationClip> cinematics;
	};

}