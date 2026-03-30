#include "Mouse.hpp"

namespace gps {
	static bool leftMousePressed = false;
	static bool rightMousePressed = false;

	static double lastY = 0.0;
	static double lastX = 0.0;
	static double deltaX;
	static double deltaY;
	static bool firstMouse = true;

	static double scrollOffset = 0.0;

	bool Mouse::isButtonDown(int button) {
		if (button == GLFW_MOUSE_BUTTON_LEFT) {
			return leftMousePressed;
		}
		if (button == GLFW_MOUSE_BUTTON_RIGHT) {
			return rightMousePressed;
		}
		return false;
	}

	void Mouse::mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
		if (button == GLFW_MOUSE_BUTTON_RIGHT) {
			rightMousePressed = (action == GLFW_PRESS);
		}
		if (button == GLFW_MOUSE_BUTTON_LEFT) {
			leftMousePressed = (action == GLFW_PRESS);
		}
	}
	void Mouse::mouseMovementCallback(GLFWwindow* window, double xpos, double ypos) {
		if (firstMouse) {
			lastX = xpos;
			lastY = ypos;
			firstMouse = false;
			return;
		}
		deltaX = xpos - lastX;
		deltaY = ypos - lastY;

		lastX = xpos;
		lastY = ypos;
	}
	double Mouse::getDY() {
		double deltaYCopy = deltaY;
		deltaY = 0.0;
		return deltaYCopy;
	}
	double Mouse::getDX() {
		double deltaXCopy = deltaX;
		deltaX = 0.0;
		return deltaXCopy;
	}
	void Mouse::scrollCallback(GLFWwindow* window, double xoffset, double yoffset){
		scrollOffset = yoffset;
	}
	double Mouse::getDWheel() {
		double scrollOfssetCopy = scrollOffset;
		scrollOffset = 0.0;
		return scrollOfssetCopy;
	}
}