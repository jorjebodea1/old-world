#include "AnimationChannel.hpp"

namespace gps {
	AnimationChannel::AnimationChannel(std::vector<float> timeKeyframes) {
		this->timeKeyframes = timeKeyframes;
	}
	glm::vec3 AnimationChannel::getLocalTranslation(float time) {
		float previousTime = timeKeyframes[previousKeyframeIndex];
		float nextTime = timeKeyframes[previousKeyframeIndex + 1];

		if (nextTime < previousTime) {
			return translationKeyframes[previousKeyframeIndex];
		}

		float duration = nextTime - previousTime;

		glm::vec3 previousTranslation = translationKeyframes[previousKeyframeIndex];
		glm::vec3 nextTranslation = translationKeyframes[previousKeyframeIndex + 1];

		float interpolationValue = (time - previousTime) / duration;

		interpolationValue = glm::clamp(interpolationValue, 0.0f, 1.0f);

		return glm::mix(previousTranslation, nextTranslation, interpolationValue);
	}
	glm::quat AnimationChannel::getLocalRotation(float time) {
		float previousTime = timeKeyframes[previousKeyframeIndex];
		float nextTime = timeKeyframes[previousKeyframeIndex + 1];

		if (nextTime < previousTime) {
			return rotationKeyframes[previousKeyframeIndex];
		}

		float duration = nextTime - previousTime;

		glm::quat previousRotation= rotationKeyframes[previousKeyframeIndex];
		glm::quat nextRotation = rotationKeyframes[previousKeyframeIndex + 1];


		float interpolationValue = (time - previousTime) / duration;
		interpolationValue = glm::clamp(interpolationValue, 0.0f, 1.0f);
		return glm::slerp(previousRotation, nextRotation, interpolationValue);
	}
	glm::vec3 AnimationChannel::getLocalScale(float time) {
		float previousTime = timeKeyframes[previousKeyframeIndex];
		float nextTime = timeKeyframes[previousKeyframeIndex + 1];

		if (nextTime < previousTime) {
			return scaleKeyframes[previousKeyframeIndex];
		}

		float duration = nextTime - previousTime;

		glm::vec3 previousScale = scaleKeyframes[previousKeyframeIndex];
		glm::vec3 nextScale = scaleKeyframes[previousKeyframeIndex + 1];
		float interpolationValue = (time - previousTime) / duration;
		interpolationValue = glm::clamp(interpolationValue, 0.0f, 1.0f);
		return glm::mix(previousScale,nextScale,interpolationValue);
	}
	void AnimationChannel::updatePreviousKeyFrameIndex(float time) {
		if (timeKeyframes.empty()) return;

		if (timeKeyframes[previousKeyframeIndex] > time) {
			previousKeyframeIndex = 0;
		}

		while (previousKeyframeIndex + 1 < timeKeyframes.size()) {
			float nextTime = timeKeyframes[previousKeyframeIndex + 1];

			if (nextTime < timeKeyframes[previousKeyframeIndex]) break; // Found the loop point
			if (time < nextTime) break; // Next frame is in the future

			previousKeyframeIndex++;
		}
	}
	bool AnimationChannel::hasLocalTranslation() {
		return !translationKeyframes.empty();
	}
	bool AnimationChannel::hasLocalRotation() {
		return !rotationKeyframes.empty();
	}
	bool AnimationChannel::hasLocalScale() {
		return !scaleKeyframes.empty();
	}
	bool AnimationChannel::isInitialized() {
		if (timeKeyframes.size() > 0)
			return true;
		else return false;
	}
	void AnimationChannel::setTranslationKeyframes(const std::vector<glm::vec3>& translationKeyframes) {
		this->translationKeyframes = translationKeyframes;
	}
	void AnimationChannel::setRotationKeyframes(const std::vector<glm::quat>& rotationKeyframes) {
		this->rotationKeyframes = rotationKeyframes;
	}
	void AnimationChannel::setScaleKeyframes(const std::vector<glm::vec3>& scaleKeyframes) {
		this->scaleKeyframes = scaleKeyframes;
	}
}