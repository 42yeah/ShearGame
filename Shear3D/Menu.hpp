//
//  Menu.hpp
//  Shear3D
//
//  Created by 周昊 on 2020/2/28.
//  Copyright © 2020 aiofwa. All rights reserved.
//

#ifndef Menu_hpp
#define Menu_hpp

#include "../Includes/glad/glad.h"
#include <glm/glm.hpp>
#include <GLFW/glfw3.h>
#include "../Includes/imgui/imgui.h"


enum MenuChoice {
    MENU, NEW, LOAD, USED
};

class Menu {
public:
    Menu() {}
    Menu(GLFWwindow *window, ImGuiIO *io);

    void render();
    
    MenuChoice choice;
    
private:
    GLFWwindow *window;
};

#endif /* Menu_hpp */
