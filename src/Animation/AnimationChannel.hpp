#pragma once
#include <iostream>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
namespace gps {
	class AnimationChannel {
	public:
		AnimationChannel()=default;
		AnimationChannel(std::vector<float> timeKeyframes);
		glm::vec3 getLocalTranslation(float time);
		glm::quat getLocalRotation(float time);
		glm::vec3 getLocalScale(float time);
		bool hasLocalTranslation();
		bool hasLocalRotation();
		bool hasLocalScale();
		void updatePreviousKeyFrameIndex(float time);
		bool isInitialized();
		void setTranslationKeyframes(const std::vector<glm::vec3>& translationKeyframes);
		void setRotationKeyframes(const std::vector<glm::quat>& rotationKeyframes);
		void setScaleKeyframes(const std::vector<glm::vec3>& scaleKeyframes);
	private:
		std::vector<float> timeKeyframes;
		std::vector<glm::vec3> translationKeyframes;
		std::vector<glm::quat> rotationKeyframes;
		std::vector<glm::vec3> scaleKeyframes;
		int previousKeyframeIndex=0;
	};
}