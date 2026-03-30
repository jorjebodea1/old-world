#pragma once

#include "Keyboard.hpp"
#include <iostream>

namespace gps {
	class Chat {
	public:
		bool isActive = false;
		void update();
		std::string chatText;
	private:

	};


}