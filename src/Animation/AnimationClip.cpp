#include "AnimationClip.hpp"

namespace gps {
	
	AnimationClip::AnimationClip(std::string animationName, float duration, std::vector<AnimationChannel> animationsChannels) {
		this->animationName = animationName;
		this->duration = duration;
		this->animationChannels = animationsChannels;
	}
}