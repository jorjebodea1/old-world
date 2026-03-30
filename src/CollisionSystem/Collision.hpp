#pragma once

#include "Entities/Entity.hpp"
#include "Entities/StaticEntity.hpp"
#include "Entities/Tile.hpp"
#include "Utilities/WorldLoader.hpp"
#include "glm/gtx/matrix_decompose.hpp"
namespace gps {
	class Collision {
		public:
			Collision(const WorldLoader& worldLoader);
			float getGroundHeight(Entity* player) const;
			float getCollidingHeight(Entity* player) const;
		private:
			const WorldLoader& worldLoader;
			void findCollidingAABBs(Entity* player, StaticEntity* currentEntity, std::vector<StaticEntity*>& collidedEntities) const;
			Tile*  findCurrentTile(Entity* player) const;
	};
}