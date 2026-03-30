#ifndef Player_hpp
#define Player_hpp

#include "Entity.hpp"
#include "Models/Model3DGLB.hpp"
#include "GLFW/glfw3.h"
#include "InputHandling/Keyboard.hpp";
#include "Tile.hpp"
#include "CollisionSystem/Collision.hpp"
namespace gps {
	class Player
	{
		public:
			Player(Transform playerTransform,AABB playerAABB,const Collision& collisionSystem);
			void update(float delta,bool isChatActive);
			void rotate(float delta);
			float getUpwardsSpeed();
			void setUpwardsSpeed(float upwardsSpeed);
			void setIsInAir(bool isInAir);
			Entity& getEntity() const;
	private:
			std::unique_ptr<Entity> playerEntity;
			const Collision& collisionSystem;
			float currentSpeed=0;
			float currentTurnSpeed=0;
			float upwardsSpeed=0;
			
			bool isInAir = false;
			void move(float delta);
			void handleGravity(float delta);
			void jump();
	};
}
#endif /* Player_hpp */