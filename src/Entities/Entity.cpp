#include "Entity.hpp"
#define CGLTF_IMPLEMENTATION
#include "Utilities/cgltf.h"
namespace gps {
	Entity::Entity(Transform entityTransform, AABB entityAABB){
		this->entityTransform = std::move(entityTransform);
		this->aabb = std::move(entityAABB);
		this->model = Model3DGLB();
		this->animator = Animator();
		loadEntity();
	}
	void Entity::loadEntity() {
		std::string filename = "objects/panda.glb";
		std::cout << "Loading : " << filename << std::endl;
		cgltf_data* data = NULL;
		cgltf_options options = {};
		cgltf_result result = cgltf_parse_file(&options, filename.c_str(), &data);
		if (result == cgltf_result_success) {
			cgltf_load_buffers(&options, data, filename.c_str());
		}
		model.LoadModel(data);
		animator.LoadModel(data);
		cgltf_free(data);
	}
	void Entity::increasePosition(glm::vec3 dPosition) {
		this->entityTransform.position += dPosition;
	}
	void Entity::increaseRotation(glm::vec3 dRotation) {
		this->entityTransform.rotation += dRotation;
	}
	float Entity::getRotY() const{
		return this->entityTransform.rotation.y;
	}
	glm::vec3 Entity::getPosition() const{
		return this->entityTransform.position;
	}
	const Transform& Entity::getTransform() const{
		return this->entityTransform;
	}
	const AABB& Entity::getAABB() const{
		return this->aabb;
	}
	void Entity::setPosition(const glm::vec3& pos) {
		this->entityTransform.position = pos;
	}
	const Model3DGLB& Entity::getModel() const {
		return model;
	}
	std::vector<glm::mat4> Entity::updateAnimation(float delta){
		return animator.updateAnimation(delta);
	}
	void Entity::playAnimation(std::string animationName) {
		return animator.playAnimation(animationName);
	}
}