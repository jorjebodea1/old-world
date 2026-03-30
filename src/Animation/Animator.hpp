#pragma once

#include "../Utilities/cgltf.h";
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "AnimationClip.hpp"
#include <unordered_map>
namespace gps {
	struct Joint {
		int skinnedMeshJointIndex;
		int parentIndex;
		glm::mat4 inverseBindMatrix=glm::mat4(1.0f);
		glm::vec3 defaultTranslation;
		glm::quat defaultRotation;
		glm::vec3 defaultScale;
	};
	class Animator
	{
		public:
			void playAnimation(std::string animationName);
			std::vector<glm::mat4> updateAnimation(float delta);
			void LoadModel(cgltf_data* data);
		private:
			std::vector<Joint> joints;
			float currentTime = 0.0f;
			std::vector<glm::mat4> globalMatrices;
			std::unordered_map<std::string, AnimationClip> animations;
			AnimationClip* currentAnimation=NULL;
			void sceneHierarchyTraversal(cgltf_node* node, cgltf_data* data,int parentIndex, std::vector<int>& nodeIndices);
			
	};
}