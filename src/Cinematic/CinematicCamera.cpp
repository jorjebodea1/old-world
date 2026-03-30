#include "CinematicCamera.hpp"

namespace gps {
	const int MAX_NODES = 2;
	void CinematicCamera::playCinematic(std::string cinematicName) {
		auto it = cinematics.find(cinematicName);
		if (it == cinematics.end()) {
			std::cout << "Cinematic not found\n";
			currentCinematic = NULL;
		}
		else if (&it->second != currentCinematic) {
			currentTime = 0;
			isActive = true;
			currentCinematic = &it->second;
		}
	}
	glm::mat4 CinematicCamera::getViewMatrix(float delta) {
		glm::mat4 finalMatrix = glm::mat4(1.0f);
		if (!isActive)
			return finalMatrix;
		glm::vec3 T = cameraPosition;
		glm::quat R = cameraRotation;
		if (currentCinematic!=NULL &&
			currentCinematic->animationChannels[0].isInitialized()) {//nod care are animatie
			AnimationChannel& channel = currentCinematic->animationChannels[0];
			channel.updatePreviousKeyFrameIndex(currentTime);
			if (channel.hasLocalRotation())
				R = channel.getLocalRotation(currentTime);
			if (channel.hasLocalTranslation())
				T = channel.getLocalTranslation(currentTime);
		}
		if (glm::length(R) > 0) R = glm::normalize(R);
		finalMatrix = glm::translate(glm::mat4(1.0f), T) * glm::toMat4(R);
		if (currentCinematic != NULL) {
			currentTime += delta;
			if (currentTime > currentCinematic->duration) {
				currentTime = 0.0f;
				isActive = false;
			}
		}
		return glm::inverse(finalMatrix);
	}
	void CinematicCamera::LoadCinematicCamera() {
		std::string filename = "objects/camera.glb";
		std::cout << "Loading : " << filename << std::endl;
		cgltf_data* data = NULL;
		cgltf_options options = {};
		cgltf_result result = cgltf_parse_file(&options, filename.c_str(), &data);
		if (result == cgltf_result_success) {
			cgltf_load_buffers(&options, data, filename.c_str());
		}

		cgltf_node* node = data->scene->nodes[0];
		cameraPosition = glm::vec3(node->translation[0], node->translation[1], node->translation[2]);
		cameraRotation = glm::quat(node->rotation[3], node->rotation[0], node->rotation[1], node->rotation[2]);

		for (int i = 0; i < data->animations_count; i++) {
			cgltf_animation* animation = &data->animations[i];
			std::vector<AnimationChannel> animationChannels(MAX_NODES, AnimationChannel());
			float duration = 0;
			for (int j = 0; j < animation->channels_count; j++) {
				cgltf_animation_channel* channel = &animation->channels[j];
				cgltf_animation_sampler* sampler = channel->sampler;
				cgltf_node* node = channel->target_node;
				int currentNodeIndex = (int)(node - data->nodes);
				std::vector<float> timeKeyframes;
				std::vector<glm::vec3> translationKeyframes;
				std::vector<glm::quat> rotationKeyframes;
				std::vector<glm::vec3> scaleKeyframes;

				if (!animationChannels[currentNodeIndex].isInitialized()) {
					cgltf_accessor* timeAccessor = sampler->input;
					for (int k = 0; k < timeAccessor->count; k++) {
						float time;
						bool f = cgltf_accessor_read_float(timeAccessor, k, &time, 1);
						timeKeyframes.push_back(time);
					}
					if (duration < timeAccessor->max[0]) {
						duration = timeAccessor->max[0];
					}
					animationChannels[currentNodeIndex] = AnimationChannel(timeKeyframes);
				}
				cgltf_accessor* transformAccessor = sampler->output;
				if (channel->target_path == cgltf_animation_path_type_translation) {
					for (int k = 0; k < transformAccessor->count; k++) {
						float translation[3];
						cgltf_accessor_read_float(transformAccessor, k, translation, 3);
						translationKeyframes.push_back(glm::vec3(translation[0], translation[1], translation[2]));
					}
					animationChannels[currentNodeIndex].setTranslationKeyframes(translationKeyframes);
				}
				else if (channel->target_path == cgltf_animation_path_type_rotation) {
					for (int k = 0; k < transformAccessor->count; k++) {
						float rotation[4];
						cgltf_accessor_read_float(transformAccessor, k, rotation, 4);
						rotationKeyframes.push_back(
							glm::quat(rotation[3], rotation[0], rotation[1], rotation[2]));
					}
					animationChannels[currentNodeIndex].setRotationKeyframes(rotationKeyframes);
				}
				else if (channel->target_path == cgltf_animation_path_type_scale) {
					for (int k = 0; k < transformAccessor->count; k++) {
						float scale[3];
						cgltf_accessor_read_float(transformAccessor, k, scale, 3);
						translationKeyframes.push_back(
							glm::vec3(scale[0], scale[1], scale[2]));
					}
					animationChannels[currentNodeIndex].setScaleKeyframes(scaleKeyframes);
				}
			}
			AnimationClip animationClip(std::string(animation->name), duration, animationChannels);
			cinematics.insert({ std::string(animation->name), animationClip });
		}
	}
}