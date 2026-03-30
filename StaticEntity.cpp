#include "StaticEntity.hpp"

namespace gps {
	StaticEntity::StaticEntity(std::shared_ptr<Model3DOBJ> model, Transform staticEntityTransform, int objectType, AABB aabb):model(model) {
		this->objectType = objectType;
		this->staticEntityTransform = std::move(staticEntityTransform);
		this->aabb = std::move(aabb);
		this->bvh = std::make_unique<BVH>(model->meshes, this->staticEntityTransform);
	}
	StaticEntity::StaticEntity(Transform staticEntityTransform, int objectType,AABB aabb, std::filesystem::path modelFile){
		this->objectType=objectType;
		this->staticEntityTransform = std::move(staticEntityTransform);
		this->aabb = std::move(aabb);
		this->model = std::make_shared<Model3DOBJ>();
		this->model->LoadModel(modelFile);
		this->bvh=std::make_unique<BVH>(model->meshes,this->staticEntityTransform);
	}
	std::string StaticEntity::getModelFile() const{
		return model->getModelFile();
	}
	const std::shared_ptr<Model3DOBJ> StaticEntity::getModel() const{
		return model;
	}
	const BVH& StaticEntity::getBVH() const{
		return *bvh;
	}
	const Transform& StaticEntity::getGlobalTransform() const {
		return staticEntityTransform;
	}
	glm::vec3 StaticEntity::getPosition() const{
		return staticEntityTransform.position;
	}
	glm::vec3 StaticEntity::getRotation() const{
		return staticEntityTransform.rotation;
	}
	float StaticEntity::getScale() {
		return staticEntityTransform.scale;
	}
	int StaticEntity::getObjectType() {
		return objectType;
	}
	int StaticEntity::getFirstChild() {
		return firstChild;
	}
	int StaticEntity::getChildCount() {
		return childCount;
	}
	void StaticEntity::setFirstChild(int firstChild) {
		this->firstChild = firstChild;
	}
	void StaticEntity::setChildCount(int childCount) {
		this->childCount = childCount;
	}
	const AABB& StaticEntity::getGlobalAABB() const{
		return aabb;
	}
}