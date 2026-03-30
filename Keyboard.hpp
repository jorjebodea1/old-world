#pragma once
#include "GLFW/glfw3.h"
namespace gps {
    class Keyboard
    {
    public:
        static bool isKeyDown(int key);
        static char getPressedChar();
        static void keyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mode);
        static void characterCallback(GLFWwindow* window, unsigned int codepoint);
    };

}