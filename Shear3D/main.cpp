//
//  main.cpp
//  Shear3D
//
//  Created by 周昊 on 2020/2/20.
//  Copyright © 2020 aiofwa. All rights reserved.
//

#include <iostream>
#include "../Includes/glad/glad.h"
#include <GLFW/glfw3.h>
#include "Game.hpp"
#include "tests.hpp"


Game game;

void cursorCallback(GLFWwindow *window, double x, double y) {
    game.mouseEvent(glm::vec2(x, y));
}

void mouseCallBack(GLFWwindow *window, int button, int action, int mods) {
//    double x, y;
//    glfwGetCursorPos(window, &x, &y);
    if (button == 1 && action == 1) {
        game.interact();
    }
}

int main(int argc, const char * argv[]) {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    GLFWwindow *window = glfwCreateWindow(800, 600, "Shear 3D", nullptr, nullptr);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, cursorCallback);
    glfwSetMouseButtonCallback(window, mouseCallBack);
    glfwMakeContextCurrent(window);
    gladLoadGL();

    game = Game(window);

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        game.clear();
        game.update();
        game.render();
        glfwSwapBuffers(window);
    }
    return 0;
}
