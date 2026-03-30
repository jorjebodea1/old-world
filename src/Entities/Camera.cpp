#include "Camera.hpp"

namespace gps {

    //Camera constructor
    Camera::Camera(glm::vec3 cameraUp,const Entity& player):player(player){
        this->cameraUpDirection = cameraUp;
    }
    //return the view matrix, using the glm::lookAt() function
    glm::mat4 Camera::getViewMatrix() const{
        return glm::lookAt(cameraPosition, player.getPosition(), cameraUpDirection);
    }
    float Camera::calculateHorizontalDistance() const{
        return (float)(distanceFromPlayer * glm::cos(glm::radians(pitch)));
    }
    float Camera::calculateVerticalDistance() const{
        return (float)(distanceFromPlayer * glm::sin(glm::radians(pitch)));
    }
    void Camera::calculateZoom() {
        float zoomLevel = Mouse::getDWheel();
        distanceFromPlayer -= zoomLevel;
    }
    void Camera::calculatePitch() {
        if (Mouse::isButtonDown(GLFW_MOUSE_BUTTON_RIGHT) || Mouse::isButtonDown(GLFW_MOUSE_BUTTON_LEFT)) {
            float pitchChange = Mouse::getDY();
            pitch += pitchChange;
        }
    }
    void Camera::calculateangleAroundPlayer() {
        if (Mouse::isButtonDown(GLFW_MOUSE_BUTTON_LEFT)) {
            float angleChange = Mouse::getDX();
            angleAroundPlayer -= angleChange;
        }
    }
    void Camera::update() {
        calculateZoom();
        calculatePitch();
        calculateangleAroundPlayer();
        float horizontalDistance = calculateHorizontalDistance();
        float verticalDistance = calculateVerticalDistance();
        calculateCameraPosition(horizontalDistance, verticalDistance);
        yaw = 180 - (player.getRotY() + angleAroundPlayer);
    }
    void Camera::calculateCameraPosition(float horizontalDistance, float verticalDistance) {
        float theta = player.getRotY()+angleAroundPlayer;
        float offsetX = horizontalDistance * glm::sin(glm::radians(theta));
        float offsetZ= horizontalDistance * glm::cos(glm::radians(theta));
        cameraPosition.x = player.getPosition().x - offsetX;
        cameraPosition.z = player.getPosition().z - offsetZ;
        cameraPosition.y = player.getPosition().y + verticalDistance;
    }
}