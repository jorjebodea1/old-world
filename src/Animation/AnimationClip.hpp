#pragma once
#include <iostream>
#include <vector>
#include "AnimationChannel.hpp"

namespace gps {

	class AnimationClip{
	public:
		AnimationClip(std::string animationName, float duration, std::vector<AnimationChannel> animationsChannels);
		std::string animationName;
		float duration;
		std::vector<AnimationChannel> animationChannels;
	private:
	};
}