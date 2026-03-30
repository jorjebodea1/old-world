#pragma once
#include "Models/Model3DGLB.hpp"
#include "Utilities/Transform.hpp"
#include "Animation/Animator.hpp"
#include "Entities/AABB.hpp"
namespace gps {
	class Entity
	{
		public:
			Entity(Transform entityTransform, AABB entityAABB);
			void increasePosition(glm::vec3 dPosition);
			void increaseRotation(glm::vec3 dRotation);
			float getRotY() const;
			glm::vec3 getPosition() const;
			const Transform& getTransform() const;
			const Model3DGLB& getModel() const;
			const AABB& getAABB() const;
			void setPosition(const glm::vec3& pos);
			void playAnimation(std::string animationName);
			std::vector<glm::mat4> updateAnimation(float delta);
		private:
			void loadEntity();
			Animator animator;
			gps::Model3DGLB model;
			Transform entityTransform;
			AABB aabb;
	};
}