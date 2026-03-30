#pragma once
#include "GLFW/glfw3.h"
namespace gps {
    class Mouse
    {
    public:
        static double getDWheel();
        static bool isButtonDown(int button);
        static double getDY();
        static double getDX();
        static void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
        static void mouseMovementCallback(GLFWwindow* window, double xpos, double ypos);
        static void scrollCallback(GLFWwindow* window, double xoffset, double yoffset);
    };

}
