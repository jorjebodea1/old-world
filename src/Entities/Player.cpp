#include "Player.hpp"

namespace gps {
	const float TURN_SPEED = 160.f;
	const float RUN_SPEED = 20.f;
	const float GRAVITY = -50.f;
	const float JUMP_POWER = 20.f;

	Player::Player(Transform playerTransform,AABB playerAABB,const Collision& collisionSystem)
		:collisionSystem(collisionSystem){
		this->playerEntity = std::make_unique<Entity>(playerTransform, playerAABB);
	};
	void Player::move(float delta){
		float distance = currentSpeed*delta;

		float dx = (float)(distance * glm::cos(glm::radians(playerEntity->getRotY())));
		playerEntity->increasePosition(glm::vec3(dx, 0, 0));
		float collisionX=collisionSystem.getCollidingHeight(playerEntity.get());
		if (collisionX!=std::numeric_limits<float>::min()) {
			float STEP_HEIGHT = (playerEntity->getAABB().max.y-playerEntity->getAABB().min.y)/2+0.2f; // max step height
			playerEntity->increasePosition(glm::vec3(0, STEP_HEIGHT, 0));

			float newCollision = collisionSystem.getCollidingHeight(playerEntity.get());
			if (newCollision == std::numeric_limits<float>::min() && isInAir==false) {
				// stepped successfully
				glm::vec3 playerPos = playerEntity->getPosition();
				playerPos.y = collisionX; // lift player above obstacle
				playerEntity->setPosition(playerPos);
			}
			else {
				// could not step, go back down
				playerEntity->increasePosition(glm::vec3(-dx, -STEP_HEIGHT, 0));
			}
		}

		float dz = (float)(distance * -glm::sin(glm::radians(playerEntity->getRotY())));
		playerEntity->increasePosition(glm::vec3(0, 0, dz));
		float collisionZ = collisionSystem.getCollidingHeight(playerEntity.get());
		if (collisionZ != std::numeric_limits<float>::min()) {
			float STEP_HEIGHT = (playerEntity->getAABB().max.y - playerEntity->getAABB().min.y) / 2+0.2f;
			playerEntity->increasePosition(glm::vec3(0, STEP_HEIGHT, 0));
			float newCollision = collisionSystem.getCollidingHeight(playerEntity.get());
			if (newCollision == std::numeric_limits<float>::min() && isInAir == false) {
				// stepped successfully
				glm::vec3 playerPos = playerEntity->getPosition();
				playerPos.y = collisionZ; // lift player above obstacle
				playerEntity->setPosition(playerPos);
			}
			else {
				// could not step, go back down
				playerEntity->increasePosition(glm::vec3(0, -STEP_HEIGHT, -dz));
			}
		}
	}
	void Player::rotate(float delta) {
		playerEntity->increaseRotation(glm::vec3(0, currentTurnSpeed*delta, 0));
	}
	void Player::handleGravity(float delta) {
		upwardsSpeed += GRAVITY * delta;
		float dy = upwardsSpeed * delta;
		playerEntity->increasePosition(glm::vec3(0, dy, 0));
		if (collisionSystem.getCollidingHeight(playerEntity.get()) != std::numeric_limits<float>::min()) {
			if (upwardsSpeed < 0) {
				upwardsSpeed = 0;
			}
			isInAir = false;
			playerEntity->increasePosition(glm::vec3(0, -dy, 0));
		}
		float terrainHeight = collisionSystem.getGroundHeight(playerEntity.get());
		if (terrainHeight>playerEntity->getPosition().y) {
			if (upwardsSpeed < 0) {
				upwardsSpeed = 0;
			}
			isInAir = false;
			glm::vec3 playerNewPosition = playerEntity->getPosition();
			playerNewPosition.y = terrainHeight;
			playerEntity->setPosition(playerNewPosition);
		}
		
	}
	void Player::jump() {
		if (!isInAir) {
			upwardsSpeed = JUMP_POWER;
			isInAir = true;
		}
	}
	void Player::update(float delta,bool isChatActive) {
		handleGravity(delta);
		//std::cout << playerEntity->getPosition().x << " " << playerEntity->getPosition().y << " " << playerEntity->getPosition().z << "\n";
		if (Keyboard::isKeyDown(GLFW_KEY_W) && !isChatActive) {
			playerEntity->playAnimation("Run");
			currentSpeed = RUN_SPEED;
			this->move(delta);
		}
		else if (Keyboard::isKeyDown(GLFW_KEY_S) && !isChatActive) {
			playerEntity->playAnimation("Walkbackwards");
			currentSpeed = -RUN_SPEED/2;
			this->move(delta);
		}
		else {
			playerEntity->playAnimation("Stand");
			currentSpeed = 0;
		}
		if (Keyboard::isKeyDown(GLFW_KEY_A) && !isChatActive) {
			currentTurnSpeed = TURN_SPEED;
			this->rotate(delta);
		}
		else if (Keyboard::isKeyDown(GLFW_KEY_D) && !isChatActive) {
			currentTurnSpeed = -TURN_SPEED;
			this->rotate(delta);
		}
		else {
			currentTurnSpeed = 0;
		}
		if (Keyboard::isKeyDown(GLFW_KEY_SPACE) && !isChatActive) {
			jump();
		}
	}
	void Player::setUpwardsSpeed(float upwardsSpeed) {
		this->upwardsSpeed = upwardsSpeed;
	}
	float Player::getUpwardsSpeed() {
		return upwardsSpeed;
	}
	void Player::setIsInAir(bool isInAir) {
		this->isInAir = isInAir;
	}
	Entity& Player::getEntity() const{
		return *playerEntity;
	}
}