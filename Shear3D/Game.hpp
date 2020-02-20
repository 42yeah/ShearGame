//
//  Game.hpp
//  Shear3D
//
//  Created by 周昊 on 2020/2/20.
//  Copyright © 2020 aiofwa. All rights reserved.
//

#ifndef Game_hpp
#define Game_hpp

#include "../Includes/glad/glad.h"
#include <glm/glm.hpp>
#include <GLFW/glfw3.h>
#include "Pass.hpp"
#include "Program.hpp"
#include "Camera.hpp"
#include "Model.hpp"


/**
 A game instance that is deeply coupled with GLFW,
 becaused I am too lazy.
 */
class Game {
public:
    Game() {}
    Game(GLFWwindow *window);
    
    void init();
    void clear();
    void update();
    void render();
    void mouseEvent(glm::vec2 mousePos);
    
private:
    void updateWindowSize();
    GLuint genereateGround();

    // === PASSES === //
    Program renderProgram;
    Pass renderPass;
    Program postEffectProgram;
    Camera camera;

    // === HIGH VALUES === //
    glm::ivec2 windowSize;
    float aspect, deltaTime;
    std::vector<Model> models;
    
    // === LOW VALUES === //
    double time;
    bool reloadKeyPressed;
    GLuint ground, rect;
    GLFWwindow *nativeWindow;
    glm::vec2 prevMousePos;
    bool firstMouse;
};

#endif /* Game_hpp */
