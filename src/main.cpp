#include <iostream>

#include <GLFW/glfw3.h>
#include "OpenGL/GLApp.h"

#include "embedTest.c"

int main() {
        GLApp openGLApp(800, 600);
        while (!glfwWindowShouldClose((GLFWwindow*)openGLApp.window)) {
                glfwPollEvents();
                
                glfwSwapBuffers((GLFWwindow*)openGLApp.window);
        }
        
        glfwTerminate();
        return 0;
}
