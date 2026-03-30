#pragma once

#include "Models/Model3DOBJ.hpp"
#include "Entities/AABB.hpp"
#include "Utilities/Transform.hpp"
#include "CollisionSystem/BVH.hpp"

namespace gps {
	class StaticEntity
	{
	public:
		StaticEntity(std::shared_ptr<Model3DOBJ> model, Transform staticEntityTransform, int objectType, AABB aabb);
		StaticEntity(Transform staticEntityTransform, int objectType,AABB aabb, std::filesystem::path modelFile);
		std::string getModelFile() const;
		const std::shared_ptr<Model3DOBJ> getModel() const;
		const AABB& getGlobalAABB() const;
		glm::vec3 getPosition() const;
		glm::vec3 getRotation() const;
		const Transform& getGlobalTransform() const;
		float getScale();
		int getObjectType();
		int getFirstChild();
		int getChildCount();
		const BVH& getBVH() const;
		void setFirstChild(int firstChild);
		void setChildCount(int childCount);
		virtual ~StaticEntity() = default;
	private:
		std::shared_ptr<Model3DOBJ> model;
		/*std::unique_ptr<Transform> staticEntityTransform;
		std::unique_ptr<AABB> aabb;*/
		std::unique_ptr<BVH> bvh;
		//Model3DOBJ model;
		Transform staticEntityTransform;
		AABB aabb;
		int objectType;
		int firstChild;
		int childCount;
	};
}