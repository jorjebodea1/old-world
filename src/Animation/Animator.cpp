#include "Animator.hpp"

namespace gps {
	const int MAX_JOINTS = 429;

	std::vector<glm::mat4> Animator::updateAnimation(float delta){
		std::vector<glm::mat4> finalMatrices(MAX_JOINTS,glm::mat4(1.0f));
		for (int jointID = 0; jointID < joints.size();jointID++) {
			glm::vec3 T = joints[jointID].defaultTranslation;
			glm::quat R = joints[jointID].defaultRotation;
			glm::vec3 S = joints[jointID].defaultScale;
			if (currentAnimation!=NULL && 
				currentAnimation->animationChannels[jointID].isInitialized()) {//nod care are animatie
				AnimationChannel& channel = currentAnimation->animationChannels[jointID];
				channel.updatePreviousKeyFrameIndex(currentTime);
				if(channel.hasLocalRotation())
					R = channel.getLocalRotation(currentTime);
				if(channel.hasLocalTranslation())
					T = channel.getLocalTranslation(currentTime);
				if(channel.hasLocalScale())
					S = channel.getLocalScale(currentTime);
			}
			if (glm::length(R) > 0) R = glm::normalize(R);
			glm::mat4 localMatrix = glm::translate(glm::mat4(1.0f), T) * glm::toMat4(R) * glm::scale(glm::mat4(1.0f), S);
			int parentID = joints[jointID].parentIndex;
			if (parentID != -1) {
				globalMatrices[jointID] = globalMatrices[parentID] * localMatrix;
			}
			else globalMatrices[jointID] =localMatrix;
			int skinIndex = joints[jointID].skinnedMeshJointIndex;
			if(skinIndex!=-1)
				finalMatrices[skinIndex] = globalMatrices[jointID] * joints[jointID].inverseBindMatrix;
		}
		if (currentAnimation != NULL) {
			currentTime += delta;
			if (currentTime > currentAnimation->duration) {
				currentTime = 0.0f;
			}
		}
		
		return finalMatrices;
	}
	void Animator::playAnimation(std::string animationName) {
		if (currentAnimation!=NULL && (currentAnimation->animationName != "Run"  
			&& currentAnimation->animationName != "Walkbackwards") && animationName == "Stand")
			return;
		auto it=animations.find(animationName);
		if (it == animations.end()) {
			std::cout << "Animation not found\n";
			currentAnimation = NULL;
		}
		else if (&it->second != currentAnimation) {
			currentTime = 0;
			currentAnimation = &it->second;
		}
	}
	void Animator::sceneHierarchyTraversal(cgltf_node* node,cgltf_data *data,int parentIndex,std::vector<int>& nodeIndices) {
		for (int i = 0; i < node->children_count; i++) {
			cgltf_node* child = node->children[i];
			int thisNodeIndex = (int)joints.size();

			Joint j;
			nodeIndices.push_back(child - data->nodes);
			j.defaultTranslation = glm::vec3(child->translation[0], child->translation[1], child->translation[2]);
			j.defaultRotation = glm::quat(child->rotation[3], child->rotation[0], child->rotation[1], child->rotation[2]);
			j.defaultScale = glm::vec3(child->scale[0], child->scale[1], child->scale[2]);
			
			j.parentIndex = parentIndex;
			j.skinnedMeshJointIndex = -1;
			joints.push_back(j);
			sceneHierarchyTraversal(child,data,thisNodeIndex,nodeIndices);
		}
	}
	void Animator::LoadModel(cgltf_data* data) {
		globalMatrices.resize(MAX_JOINTS);
		std::vector<int> nodeIndices;
		cgltf_node* node = data->scene->nodes[0];
		Joint j;
		nodeIndices.push_back((int)(node - data->nodes));
		j.defaultTranslation = glm::vec3(node->translation[0], node->translation[1], node->translation[2]);
		j.defaultRotation = glm::quat(node->rotation[3], node->rotation[0], node->rotation[1], node->rotation[2]);
		j.defaultScale = glm::vec3(node->scale[0], node->scale[1], node->scale[2]);
		j.skinnedMeshJointIndex = -1;
		j.parentIndex = -1;
		joints.push_back(j);
		sceneHierarchyTraversal(node,data,0,nodeIndices);

		cgltf_accessor* bindMatrices_accesor = data->skins[0].inverse_bind_matrices;
		for (size_t i = 0; i < data->skins[0].joints_count; i++) {
			glm::mat4 bindMatrix=glm::mat4();
			if (bindMatrices_accesor != NULL) {
				float bindMatrixArray[16];
				cgltf_accessor_read_float(bindMatrices_accesor, i, bindMatrixArray, 16);
				bindMatrix = glm::mat4(
					bindMatrixArray[0], bindMatrixArray[1], bindMatrixArray[2], bindMatrixArray[3],
					bindMatrixArray[4], bindMatrixArray[5], bindMatrixArray[6], bindMatrixArray[7],
					bindMatrixArray[8], bindMatrixArray[9], bindMatrixArray[10], bindMatrixArray[11],
					bindMatrixArray[12], bindMatrixArray[13], bindMatrixArray[14], bindMatrixArray[15]
				);
			}
			int currentNodeIndex =(int)(data->skins[0].joints[i]-data->nodes);
			for (int j = 0; j < joints.size(); j++) {
				if (currentNodeIndex == nodeIndices[j]) {
					joints[j].inverseBindMatrix = bindMatrix;
					joints[j].skinnedMeshJointIndex = i;
					break;
				}
			}
		}


		for (int i = 0; i < data->animations_count; i++) {
			cgltf_animation* animation = &data->animations[i];
			std::vector<AnimationChannel> animationChannels(MAX_JOINTS, AnimationChannel());
			float duration = 0;
			for (int j = 0; j < animation->channels_count; j++) {
				cgltf_animation_channel* channel = &animation->channels[j];
				cgltf_animation_sampler* sampler = channel->sampler;
				cgltf_node* node = channel->target_node;
				int currentNodeIndex =(int) (node - data->nodes);
				int jointID = -1;
				std::vector<float> timeKeyframes;
				std::vector<glm::vec3> translationKeyframes;
				std::vector<glm::quat> rotationKeyframes;
				std::vector<glm::vec3> scaleKeyframes;
				for (int k = 0; k < joints.size(); k++) {
					if (currentNodeIndex == nodeIndices[k]) {
						jointID = k;
						break;
					}
				}
				if (!animationChannels[jointID].isInitialized()) {
					cgltf_accessor* timeAccessor = sampler->input;
					for (int k = 0; k < timeAccessor->count; k++) {
						float time;
						bool f=cgltf_accessor_read_float(timeAccessor, k, &time, 1);
						timeKeyframes.push_back(time);
					}
					if (duration < timeAccessor->max[0]) {
						duration = timeAccessor->max[0];
					}
					animationChannels[jointID] = AnimationChannel(timeKeyframes);
				}
				cgltf_accessor* transformAccessor = sampler->output;
				if (channel->target_path == cgltf_animation_path_type_translation) {
					for (int k = 0; k < transformAccessor->count; k++) {
						float translation[3];
						cgltf_accessor_read_float(transformAccessor, k, translation, 3);
						translationKeyframes.push_back(glm::vec3(translation[0], translation[1], translation[2]));
					}
					animationChannels[jointID].setTranslationKeyframes(translationKeyframes);
				}
				else if (channel->target_path == cgltf_animation_path_type_rotation) {
					for (int k = 0; k < transformAccessor->count; k++) {
						float rotation[4];
						cgltf_accessor_read_float(transformAccessor, k, rotation, 4);
						rotationKeyframes.push_back(
							glm::quat(rotation[3], rotation[0], rotation[1],rotation[2]));
					}
					animationChannels[jointID].setRotationKeyframes(rotationKeyframes);
				}
				else if (channel->target_path == cgltf_animation_path_type_scale) {
					for (int k = 0; k < transformAccessor->count; k++) {
						float scale[3];
						cgltf_accessor_read_float(transformAccessor, k, scale, 3);
						translationKeyframes.push_back(
							glm::vec3(scale[0], scale[1], scale[2]));
					}
					animationChannels[jointID].setScaleKeyframes(scaleKeyframes);
				}
			}
			std::string animationName = std::string(animation->name);
			auto pos = animationName.find(" ");
			animationName = animationName.substr(0, pos);
			AnimationClip animationClip(animationName, duration, animationChannels);
			animations.insert({ animationName, animationClip });
		}
	}
}