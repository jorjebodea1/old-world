#include "Keyboard.hpp"
namespace gps {
	static bool pressedKeys[1024] = { false };
	static char pressedChar=0;
	static int polygonMode=1;
	static bool enterPressed = false;

	bool  Keyboard::isKeyDown(int key){
		if (key == GLFW_KEY_ENTER) {
			bool enterJustPressed = pressedKeys[key] && !enterPressed;
			enterPressed = pressedKeys[key];
			return enterJustPressed;
		}
		if (key >= 0 && key < 1024) {
			return pressedKeys[key];
		}
		return false;
	}
	char Keyboard::getPressedChar() {
		if (pressedChar !=0) {
			char returnChar = pressedChar;
			pressedChar = 0;
			return returnChar;
		}
		return 0;
	}
	void Keyboard::keyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mode) {
		if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
			glfwSetWindowShouldClose(window, GL_TRUE);
		}
		if (key == GLFW_KEY_T && action == GLFW_PRESS) {
			if (polygonMode != 1) {
				glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
				polygonMode = 1;
			}
		}
		if (key == GLFW_KEY_Y && action == GLFW_PRESS) {
			if (polygonMode != 2) {
				glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
				polygonMode = 2;
			}
		}
		if (key == GLFW_KEY_U && action == GLFW_PRESS) {
			if (polygonMode != 3) {
				glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
				polygonMode = 3;
			}
		}
		if (key >= 0 && key < 1024) {
			if (key==GLFW_KEY_ENTER) {
				pressedKeys[key] = (action != GLFW_RELEASE);
			}
			else pressedKeys[key] = (action == GLFW_PRESS || action == GLFW_REPEAT);
		}
	}
	void Keyboard::characterCallback(GLFWwindow* window, unsigned int codepoint)
	{
		if (codepoint < 128) {
			pressedChar = static_cast<char>(codepoint);
		}
	}
}