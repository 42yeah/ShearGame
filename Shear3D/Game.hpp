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
    
private:
    void updateWindowSize();
    GLuint genereateGround();

    Program renderProgram;
    Pass renderPass;
    Program postEffectProgram;
    Camera camera;

    glm::ivec2 windowSize;
    float aspect;
    
    bool reloadKeyPressed;
    GLuint ground, rect;
    GLFWwindow *nativeWindow;
};

#endif /* Game_hpp */
