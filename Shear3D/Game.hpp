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
#include "Object.hpp"
#include "Monster.hpp"


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
    GLuint generateMonsterRect();
    
    void loadMap(std::string path);
    void loadMonsters(std::string path);
    void addObject(int id, glm::vec3 pos, float rotY = 0.0f);

    // === PASSES === //
    Program renderProgram;
    Program monsterProgram;
    Pass renderPass;
    Program postEffectProgram;
    Camera camera;

    // === HIGH VALUES === //
    glm::ivec2 windowSize;
    float aspect, deltaTime;
    double additiveTime;
    Texture monsterTexture;
    std::vector<Model> models;
    std::vector<Object> objects;
    std::vector<Monster> monsters;
    glm::vec3 sunDirection;
    glm::vec3 sunColor;
    int day;
    
    // === LOW VALUES === //
    double time;
    bool reloadKeyPressed;
    GLuint ground, rect, monster;
    GLFWwindow *nativeWindow;
    glm::vec2 prevMousePos;
    bool firstMouse;
    bool dayLock;
    
    std::vector<Ramp> ramps;
    bool waypointKeyPressed;
};

#endif /* Game_hpp */
