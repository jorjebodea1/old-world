#include "Collision.hpp"

namespace gps {
	Collision::Collision(const WorldLoader& worldLoader):worldLoader(worldLoader) {
	}
	Tile* Collision::findCurrentTile(Entity* player) const {
		for (int i = 0; i < 4; i++) {
			AABB aabb = worldLoader.entities[i]->getGlobalAABB();
			if (aabb.min.x>player->getPosition().x || aabb.max.x<player->getPosition().x)
				continue;
			if (aabb.min.z > player->getPosition().z || aabb.max.x < player->getPosition().z)
				continue;
			//playerul este in interiorul bounding boxului i
			return static_cast<Tile*>(worldLoader.entities[i].get());
		}
	}


	float Collision::getGroundHeight(Entity*player)  const{
		Tile* currentTile = findCurrentTile(player);
		if (currentTile == NULL) {
			std::cout << "Current Tile is null";
			return 0;
		}
		return currentTile->getHeight(player->getPosition().x, player->getPosition().z);
	}
	void Collision::findCollidingAABBs(Entity* player, StaticEntity* currentEntity,std::vector<StaticEntity*>& collidedEntities)  const {
		if (currentEntity == NULL) {
			currentEntity=findCurrentTile(player);
		}
		for (int i = 0; i < currentEntity->getChildCount(); i++) {
			StaticEntity* childEntity = worldLoader.entities[currentEntity->getFirstChild() + i].get();
			AABB childAABB = childEntity->getGlobalAABB();
			AABB playerAABB = player->getAABB().getGlobalAABB(player->getTransform());
			if (childAABB.isColliding(playerAABB)) {
				collidedEntities.push_back(childEntity);
				if (childEntity->getChildCount() > 0) {
					findCollidingAABBs(player, childEntity,collidedEntities);
				}
				/*float dist = glm::length(playerAABB.getCenter() - hitLeaf->getAABB().getCenter());

				if (dist < bestDistance) {
					bestDistance = dist;
					bestHit = hitLeaf;
				}*/
			}
		}
		/*if (bestHit != NULL) {
			return bestHit;
		}
		else return currentEntity;*/
	}
	float Collision::getCollidingHeight(Entity* player) const {
		//aflam cel mai mic AABB care nu are copil
		AABB playerAABB = player->getAABB().getGlobalAABB(player->getTransform());
		std::vector<StaticEntity*> collidedEntities;
		findCollidingAABBs(player, NULL,collidedEntities);
		if (collidedEntities.size() <= 0) {
			return std::numeric_limits<float>::min();
		}

		float highestY = std::numeric_limits<float>::min();
		for (int i = 0; i < collidedEntities.size(); i++) {
			float currentY= collidedEntities[i]->getBVH().getCollidingHeight(playerAABB);
			highestY = fmaxf(highestY, currentY);
		}
		return highestY;
	}
} 