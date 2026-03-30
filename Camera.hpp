#ifndef Camera_hpp
#define Camera_hpp

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include "Player.hpp"
#include "Mouse.hpp"

#include <string>

namespace gps {   
    class Camera
    {
    public:
        //Camera constructor
        Camera(glm::vec3 cameraUp,const Entity& player);
        glm::mat4 getViewMatrix() const;

        void calculateZoom();
        void calculatePitch();
        void calculateangleAroundPlayer();
        void update();
    private:
        glm::vec3 cameraPosition;
        glm::vec3 cameraUpDirection;
        const Entity& player;
        float pitch=46.35;
        float yaw = 0;
        float distanceFromPlayer = 39.96;
        float angleAroundPlayer = 85.0;
        float calculateHorizontalDistance() const;
        float calculateVerticalDistance() const;
        void calculateCameraPosition(float horizontalDistance, float verticalDistance);
    };
    
}

#endif /* Camera_hpp */
