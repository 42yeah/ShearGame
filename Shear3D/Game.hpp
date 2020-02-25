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
#include "Item.hpp"
#include "../Includes/imgui/imgui.h"


enum CharacterState {
    NORMAL, SITTING, SLEEPING
};

struct Notification {
    Notification(std::string title, std::string content, bool live, float aliveTime);

    std::string title;
    std::string content;
    bool live;
    float aliveTime;
};

/**
 A game instance that is deeply coupled with GLFW,
 becaused I am too lazy.
 */
class Game {
public:
    Game() {}
    Game(GLFWwindow *window, ImGuiIO *io);
    
    void init();
    void clear();
    void update();
    void render();
    void renderGUI();
    void mouseEvent(glm::vec2 mousePos);
    void interact();
    void escape(bool es);
    
private:
    friend class Item;
    friend class Object;
    friend class Monster;
    void updateWindowSize();
    GLuint genereateGround();
    GLuint generateMonsterRect();
    
    void loadMap(std::string path);
    void loadMonsters(std::string path);
    void addObject(int id, glm::vec3 pos, float rotY = 0.0f);
    bool collides(glm::vec3 pos, glm::vec3 objPos, float objWidth);

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
    
    // === GAME VALUES === //
    float hunger;
    float stamina;
    CharacterState state;
    Object *interactingObject;
    Monster *interactingMonster;
    Ramp *interactingMonsterRamp;
    float bedCounter;
    int jailDays;
    
    // === LOW VALUES === //
    double time;
    bool reloadKeyPressed;
    GLuint ground, rect, monster;
    GLFWwindow *nativeWindow;
    glm::vec2 prevMousePos;
    bool firstMouse;
    bool dayLock;
    bool escaping;
    ImGuiIO *io;
    std::vector<Notification> notifications;
    std::vector<Item> items;

    bool tabPressed;
};

#endif /* Game_hpp */
