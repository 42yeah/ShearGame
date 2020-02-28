//
//  Menu.cpp
//  Shear3D
//
//  Created by 周昊 on 2020/2/28.
//  Copyright © 2020 aiofwa. All rights reserved.
//

#include "Menu.hpp"
#include "../Includes/imgui/imgui.h"


Menu::Menu(GLFWwindow *window, ImGuiIO *io) : window(window), choice(MENU) {
    
}

void Menu::render() {
    ImGui::SetNextWindowPos(ImVec2(100.0f, 100.0f), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(400.0f, 200.0f), ImGuiCond_FirstUseEver);
    ImGui::Begin("Menu");
    ImGui::Text("Shear 3D");
    if (ImGui::Button("New Game")) {
        choice = NEW;
    } else if (ImGui::Button("Load")) {
        choice = LOAD;
    } else if (ImGui::Button("Exit")) {
        exit(0);
    }
    ImGui::End();
}
