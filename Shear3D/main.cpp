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
#define IMGUI_IMPL_OPENGL_LOADER_GLAD
#include "../Includes/imgui/imgui.h"
#include "../Includes/imgui/examples/imgui_impl_glfw.h"
#include "../Includes/imgui/examples/imgui_impl_opengl3.h"
#include "Game.hpp"
#include "Menu.hpp"
#include "tests.hpp"


Game game;
Menu menu;

void cursorCallback(GLFWwindow *window, double x, double y) {
    if (menu.choice == USED) {
        game.mouseEvent(glm::vec2(x, y));
    }
}

void mouseCallback(GLFWwindow *window, int button, int action, int mods) {
//    double x, y;
//    glfwGetCursorPos(window, &x, &y);
    if (menu.choice == USED && button == 1 && action == 1) {
        game.interact();
    }
}

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (menu.choice == USED && key == GLFW_KEY_F5 && action == 1) {
        game.save();
    }
}

int main(int argc, const char * argv[]) {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    GLFWwindow *window = glfwCreateWindow(800, 600, "Shear 3D", nullptr, nullptr);
    glfwSetCursorPosCallback(window, cursorCallback);
    glfwSetMouseButtonCallback(window, mouseCallback);
    glfwSetKeyCallback(window, keyCallback);
    glfwMakeContextCurrent(window);
    gladLoadGL();
    
    // === SETUP IMGUI === //
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    
    io.WantSaveIniSettings = false;
    io.IniFilename = nullptr;
    
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330 core");
    game = Game(window, &io);
    menu = Menu(window, &io);

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        switch (menu.choice) {
            case MENU:
                glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
                ImGui_ImplOpenGL3_NewFrame();
                ImGui_ImplGlfw_NewFrame();
                ImGui::NewFrame();
                menu.render();
                ImGui::Render();
                ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
                glfwSwapBuffers(window);
                break;
                
            case LOAD:
                menu.choice = USED;
                game.init();
                game.load();
                // Load
                break;

            case NEW:
                menu.choice = USED;
                game.init();
                break;
                
            case USED:
                game.clear();
                game.update();
                game.render();
                
                ImGui_ImplOpenGL3_NewFrame();
                ImGui_ImplGlfw_NewFrame();
                ImGui::NewFrame();
                game.renderGUI();
                ImGui::Render();
                ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
                glfwSwapBuffers(window);
                break;
        }
    }
    return 0;
}
