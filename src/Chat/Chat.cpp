#include "Chat.hpp"

namespace gps {
	void Chat::update() {
		if (Keyboard::isKeyDown(GLFW_KEY_ENTER)) {
			isActive = !isActive;
		}
		if (isActive) {
			char pressedChar = Keyboard::getPressedChar();
			if (pressedChar != 0) {
				chatText.append(1, pressedChar);
			}
		}
	}


}