//
//  Game.cpp
//  Shear3D
//
//  Created by 周昊 on 2020/2/20.
//  Copyright © 2020 aiofwa. All rights reserved.
//

#include "Game.hpp"
#include <iostream>
#include <GLFW/glfw3.h>
#include <glm/gtc/type_ptr.hpp>
#include <fstream>
#include "tests.hpp"


Game::Game(GLFWwindow *window, ImGuiIO *io) : nativeWindow(window), reloadKeyPressed(false), firstMouse(true), tabPressed(false), escaping(false), io(io), hunger(1.0f), stamina(1.0f) {
    init();
}

void Game::updateWindowSize() { 
    int w, h;
    glfwGetWindowSize(nativeWindow, &w, &h);
    windowSize = glm::ivec2(w, h);
#ifdef __APPLE__
    windowSize *= 2;
#endif
    aspect = (float) w / h;
}

void Game::init() {
    updateWindowSize();
    ground = genereateGround();
    rect = generateTestRect();
    monster = generateMonsterRect();
    renderPass = Pass(windowSize);
    renderProgram = Program("Assets/default.vertex.glsl", "Assets/default.fragment.glsl");
    monsterProgram = Program("Assets/monster.vertex.glsl", "Assets/monster.fragment.glsl");
    postEffectProgram = Program("Assets/posteffect.vertex.glsl", "Assets/posteffect.fragment.glsl");
    camera = Camera(glm::vec3(4.0, 1.45f, 9.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, 1.0f, 0.0f));

    time = glfwGetTime();
    deltaTime = 0.0f;
    day = 0;
    dayLock = false;
    additiveTime = 30.0f;
    
    models = loadModels("Assets/Model/shear.1.obj", "Assets/Model");
    loadMap("Assets/map");
    monsterTexture = Texture("Assets/Monsters/Monsters.png", RGBA);
    loadMonsters("Assets/monsterlist");
    
    notifications.push_back(Notification{ "Story", "Your father just commited suicide after losing a bet worth of 1000 eggs with the local rich man.\nIt is now up to you to pay the debt.\nLuckily, you have a well which spurts infinite amount of gold coins at your backyard.\nWhen you get enough eggs, find the rich man,\nand fullfill your side of the deal!", false, -1.0f });
}

void Game::clear() {
    glClearColor(sunColor.r, sunColor.g, sunColor.b, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Game::render() {
//    std::cout << camera.position.x << ", " << camera.position.y << ", " << camera.position.z << std::endl;
    glViewport(0, 0, windowSize.x, windowSize.y);
    renderPass.use();
    glEnable(GL_DEPTH_TEST);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    renderProgram.use();
    glUniform3f(renderProgram.loc("sun.dir"), sunDirection.x, sunDirection.y, sunDirection.z);
    glUniform3f(renderProgram.loc("sun.color"), sunColor.x, sunColor.y, sunColor.z);
    camera.pass(aspect, renderProgram.loc("view"), renderProgram.loc("perspective"));
    glUniformMatrix4fv(renderProgram.loc("model"), 1, GL_FALSE, glm::value_ptr(glm::scale(glm::mat4(1.0f), glm::vec3(200.0f, 200.0f, 200.0f))));
    glBindVertexArray(ground);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    // === RENDER MODELS === //
    for (int i = 0; i < objects.size(); i++) {
        if (glm::length(objects[i].pos - camera.position) >= 60.0f) {
            continue; // Clip
        }
        objects[i].render(renderProgram);
    }
    monsterProgram.use();
    glUniform3f(monsterProgram.loc("sun.color"), sunColor.x, sunColor.y, sunColor.z);
    camera.pass(aspect, monsterProgram.loc("view"), monsterProgram.loc("perspective"));
    for (int i = 0; i < monsters.size(); i++) {
        if (glm::length(monsters[i].position - camera.position) >= 60.0f) {
            continue; // Clip
        }
        monsters[i].render(monsterProgram);
    }
    renderPass.unuse();
    
    glDisable(GL_DEPTH_TEST);
    glClear(GL_COLOR_BUFFER_BIT);
    postEffectProgram.use();
    glUniform1f(postEffectProgram.loc("aspect"), aspect);
    renderPass.pass(postEffectProgram.loc("tex"), 0);
    glBindVertexArray(rect);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

void Game::update() { 
    updateWindowSize();
    double now = glfwGetTime();
    deltaTime = (float) now - time;
    time = now;
    additiveTime += deltaTime;
    
    float s = additiveTime * 0.02f;
    float standarized = sinf(s - 3.14159f / 2.0f) * 0.5f + 0.5f;
    
    if (standarized < 0.1f && !dayLock) {
        day++;
        dayLock = true;
        notifications.push_back(Notification{ "A New Day", "A new day has begun. It is now day " + std::to_string(day) + ". ", true, 3.0f });
    }
    if (standarized > 0.1f) {
        dayLock = false;
    }
    
    sunDirection = glm::normalize(glm::vec3(-sinf(s), cosf(s), 0.0f));
    sunColor = glm::pow(glm::mix(glm::vec3(0.0f), glm::vec3(0.9f, 0.9f, 0.99f), standarized), glm::vec3(1.2f));
    
    if (glfwGetKey(nativeWindow, GLFW_KEY_R)) {
        if (!reloadKeyPressed) {
            reloadKeyPressed = true;
            renderProgram.reload();
            postEffectProgram.reload();
            monsterProgram.reload();
            loadMap("Assets/map");
            loadMonsters("Assets/monsterlist");
        }
    } else {
        reloadKeyPressed = false;
    }
    
    if (glfwGetKey(nativeWindow, GLFW_KEY_TAB)) {
        if (!tabPressed) {
            tabPressed = true;
            escaping = !escaping;
            escape(escaping);
        }
    } else {
        tabPressed = false;
    }
    
    glm::vec3 f(camera.front.x, 0.0f, camera.front.z);
    glm::vec3 oldCameraPos = camera.position; // For collision detection
    f = glm::normalize(f);
    if (glfwGetKey(nativeWindow, GLFW_KEY_W)) {
        camera.position += f * deltaTime * 4.0f;
    }
    if (glfwGetKey(nativeWindow, GLFW_KEY_A)) {
        camera.position -= glm::cross(f, camera.up) * deltaTime * 4.0f;
    }
    if (glfwGetKey(nativeWindow, GLFW_KEY_S)) {
        camera.position -= f * deltaTime * 4.0f;
    }
    if (glfwGetKey(nativeWindow, GLFW_KEY_D)) {
        camera.position += glm::cross(f, camera.up) * deltaTime * 4.0f;
    }
    if (glfwGetKey(nativeWindow, GLFW_KEY_K)) {
        std::cout << standarized << std::endl;
    }
    
    // Collision check
    float objWidth = 0.55f;
    glm::vec3 newCameraPos = camera.position;
    for (int i = 0; i < objects.size(); i++) {
        if (objects[i].type != OBSTACLE || objects[i].pos.y != 0 || glm::distance(objects[i].pos, camera.position) > 4.0f) {
//            std::cout << desired << std::endl;
            continue;
        }
        glm::vec3 objPos = objects[i].pos;
        if (collides(camera.position, objPos, objWidth)) {
            camera.position.x = oldCameraPos.x;
            if (collides(camera.position, objPos, objWidth)) {
                camera.position.x = newCameraPos.x;
                camera.position.z = oldCameraPos.z;
                if (collides(camera.position, objPos, objWidth)) {
                    camera.position = oldCameraPos;
                }
            }
        }
    }
    bool stat = false;
    objWidth = 0.5f;
    for (int i = 0; i < objects.size(); i++) {
        if (objects[i].type == PASSABLE || objects[i].type == OBSTACLE || objects[i].pos.y != 0 || glm::distance(objects[i].pos, camera.position) > 4.0f) {
            continue;
        }
        if (collides(oldCameraPos, objects[i].pos, objWidth)) {
            float desired;
            switch (objects[i].type) {
                case SLEEPABLE:
                    desired = 1.75f;
                    break;

                case SITTABLE:
                    desired = 1.95f;
                    break;

                default:
                    desired = 1.45f;
                    break;
            }
            float dy = desired - camera.position.y;
            camera.position.y += dy * 4.0f * deltaTime;
            stat = true;
            break;
        }
    }
    if (!stat) {
        float dy = 1.45f - camera.position.y;
        camera.position.y += dy * 4.0f * deltaTime;
    }

    for (int i = 0; i < monsters.size(); i++) {
        monsters[i].update(deltaTime, standarized, day, objects);
    }
}

GLuint Game::genereateGround() { 
    GLuint VAO, VBO;
    float triangle[] = {
        -1.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f, -1.0f, 0.0f,
        1.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f, -1.0f, 0.0f,
        1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, -1.0f, 0.0f,
        1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, -1.0f, 0.0f,
        -1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, -1.0f, 0.0f,
        -1.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f, -1.0f, 0.0f
    };
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(triangle), triangle, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 8, nullptr);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 8, (void *) (sizeof(float) * 3));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 8, (void *) (sizeof(float) * 6));
    return VAO;
}

GLuint Game::generateMonsterRect() {
    GLuint VAO, VBO;
    float triangle[] = {
        -0.5f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, -1.0f, 0.0f,
        0.5f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, -1.0f, 0.0f,
        0.5f, 2.0f, 0.0f, 0.0f, 0.0f, 1.0f, -1.0f, 0.0f,
        0.5f, 2.0f, 0.0f, 0.0f, 0.0f, 1.0f, -1.0f, 0.0f,
        -0.5f, 2.0f, 0.0f, 0.0f, 0.0f, 1.0f, -1.0f, 0.0f,
        -0.5f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, -1.0f, 0.0f
    };
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(triangle), triangle, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 8, nullptr);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 8, (void *) (sizeof(float) * 3));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 8, (void *) (sizeof(float) * 6));
    return VAO;
}

void Game::mouseEvent(glm::vec2 mousePos) {
    if (escaping) {
        firstMouse = true;
        return;
    }
    if (firstMouse) {
        prevMousePos = mousePos;
        firstMouse = false;
    }
    glm::vec2 dPos = mousePos - prevMousePos;
    dPos.y = -dPos.y;
    prevMousePos = mousePos;
    float sensivity = 0.1f;
    dPos *= sensivity;
    camera.yaw += dPos.x;
    camera.pitch += dPos.y;
    camera.pitch = glm::max(glm::min(camera.pitch, 89.9f), -89.9f);
}

void Game::loadMap(std::string path) {
    objects.clear();
    std::ifstream reader(path);
    
    float z = 0.0f;
    while (!reader.eof()) {
        std::string line;
        std::getline(reader, line, '\n');
        for (int x = 0; x < line.size(); x++) {
            switch (line[x]) {
                case '#':
                    addObject(0, glm::vec3(x, 0.0f, z));
                    addObject(0, glm::vec3(x, 1.0f, z));
                    addObject(0, glm::vec3(x, 2.0f, z));
                    break;
                    
                case '^':
                    addObject(0, glm::vec3(x, 0.0f, z));
                    addObject(0, glm::vec3(x, 1.0f, z));
                    addObject(0, glm::vec3(x, 2.0f, z));
                    addObject(0, glm::vec3(x, 3.0f, z));
                    addObject(0, glm::vec3(x, 4.0f, z));
                    break;
                    
                case '+':
                    addObject(3, glm::vec3(x, 0.0f, z));
                    addObject(0, glm::vec3(x, 2.0f, z));
                    addObject(2, glm::vec3(x, 0.0f, z));
                    break;
                    
                case '=':
                    addObject(3, glm::vec3(x, 0.0f, z));
                    addObject(0, glm::vec3(x, 2.0f, z));
                    addObject(0, glm::vec3(x, 3.0f, z));
                    addObject(0, glm::vec3(x, 4.0f, z));
                    addObject(2, glm::vec3(x, 0.0f, z));
                    break;
                    
                case '|':
                    addObject(3, glm::vec3(x, 0.0f, z), glm::radians(90.0f));
                    addObject(0, glm::vec3(x, 2.0f, z));
                    addObject(2, glm::vec3(x, 0.0f, z));
                    break;
                    
                case '-':
                    addObject(3, glm::vec3(x, 0.0f, z), glm::radians(90.0f));
                    addObject(0, glm::vec3(x, 2.0f, z));
                    addObject(0, glm::vec3(x, 3.0f, z));
                    addObject(0, glm::vec3(x, 4.0f, z));
                    addObject(2, glm::vec3(x, 0.0f, z));
                    break;
                    
                case '.':
                    addObject(2, glm::vec3(x, 0.0f, z));
                    addObject(0, glm::vec3(x, 2.0f, z));
                    break;
                    
                case ',':
                    addObject(2, glm::vec3(x, 0.0f, z));
                    addObject(0, glm::vec3(x, 4.0f, z));
                    break;
                    
                case '0':
                    addObject(2, glm::vec3(x, 0.0f, z));
                    addObject(1, glm::vec3(x, 0.0f, z - 0.5f));
                    addObject(0, glm::vec3(x, 2.0f, z));
                    break;
                    
                case '1':
                    addObject(2, glm::vec3(x, 0.0f, z));
                    addObject(1, glm::vec3(x, 0.0f, z - 0.5f));
                    addObject(0, glm::vec3(x, 4.0f, z));
                    break;
                    
                case '~':
                    addObject(6, glm::vec3(x, 0.0f, z));
                    break;
                    
                case '7':
                    addObject(5, glm::vec3(x, 0.0f, z));
                    break;
                    
                case 'O':
                    addObject(4, glm::vec3(x, 0.0f, z));
                    addObject(6, glm::vec3(x, 0.0f, z));
                    break;
                    
                case 'a':
                case 'b':
                case 'c':
                case 'd':
                    addObject(7, glm::vec3(x, 0.0f, z), glm::radians(90.0f * (line[x] - 'a')));
                    addObject(0, glm::vec3(x, 2.0f, z));
                    break;
                    
                case 'A':
                case 'B':
                case 'C':
                case 'D':
                    addObject(7, glm::vec3(x, 0.0f, z), glm::radians(90.0f * (line[x] - 'A')));
                    addObject(0, glm::vec3(x, 4.0f, z));
                    break;
                    
                case '!':
                    addObject(6, glm::vec3(x, 0.0f, z));
                    addObject(8, glm::vec3(x, 0.0f, z));
                    break;
                    
                case 'e':
                case 'f':
                case 'g':
                case 'h':
                    if (line[x] == 'h') {
                        addObject(0, glm::vec3(x, 2.0f, z));
                    }
                    addObject(9, glm::vec3(x, 0.0f, z + 0.01f), glm::radians(90.0f * (line[x] - 'e')));
                    break;
                    
                case 'i':
                case 'j':
                case 'k':
                case 'l':
                    addObject(10, glm::vec3(x, 0.0f, z), glm::radians(90.0f * (line[x] - 'i')));
                    addObject(2, glm::vec3(x, 0.0f, z));
                    addObject(0, glm::vec3(x, 2.0f, z));
                    break;
                    
                case '*':
                    addObject(10, glm::vec3(x, 0.0f, z), glm::radians(90.0f));
                    addObject(6, glm::vec3(x, 0.0f, z));
                    break;
                    
                case 'I':
                case 'J':
                case 'K':
                case 'L':
                    addObject(10, glm::vec3(x, 0.0f, z), glm::radians(90.0f * (line[x] - 'I')));
                    addObject(2, glm::vec3(x, 0.0f, z));
                    addObject(0, glm::vec3(x, 4.0f, z));
                    break;
                    
                case 't':
                    addObject(11, glm::vec3(x, 0.0f, z));
                    addObject(2, glm::vec3(x, 0.0f, z));
                    addObject(0, glm::vec3(x, 2.0f, z));
                    break;
                    
                case 'T':
                    addObject(11, glm::vec3(x, 0.0f, z));
                    addObject(2, glm::vec3(x, 0.0f, z));
                    addObject(0, glm::vec3(x, 4.0f, z));
                    break;
                    
                case '?':
                    addObject(12, glm::vec3(x, 0.0f, z));
                    break;
                    
                case '[':
                    addObject(2, glm::vec3(x, 0.0f, z));
                    addObject(13, glm::vec3(x, 0.0f, z - 0.5f));
                    addObject(0, glm::vec3(x, 2.0f, z));
                    break;
            }
        }
        z++;
    }
}

void Game::addObject(int id, glm::vec3 pos, float rotY) {
    glm::mat4 off(1.0f);
    off = glm::translate(off, pos);
    off = glm::rotate(off, rotY, glm::vec3(0.0f, 1.0f, 0.0f));
    ObjectType type = PASSABLE;
    switch (id) {
        case 0: // Brick
        case 4: // Well
        case 5: // Tree
        case 9: // Stall
        case 7: // Chest
        case 11: // Table
            type = OBSTACLE;
            break;
            
        case 1: // Bed
            type = SLEEPABLE;
            break;
            
        case 10:
            type = SITTABLE;
            break;
            
        default:
            break;
    }
    Object object(pos, &models[id], off, type);
    objects.push_back(object);
}

void Game::loadMonsters(std::string path) { 
    monsters.clear();
    std::ifstream reader(path);
    int index = -1;
    while (!reader.eof()) {
        int id;
        char action;
        float time, x, y, z;
        reader >> action;
        switch (action) {
            case 'M':
                if (index != -1 && monsters[index].ramps.size() > 0) {
                    monsters[index].ramps.push_back(Ramp { 1.0f, glm::vec3(0.0f, 0.0f, 0.0f ) });
                }
                reader >> id >> x >> y >> z;
                monsters.push_back(Monster(&monsterTexture, id, glm::vec3(x, y, z), monster));
                index++;
                break;
                
            case 'R':
                reader >> time >> x >> y >> z;
                monsters[index].ramps.push_back(Ramp{ time, glm::vec3(x, y, z) });
                break;
        }
        
    }
}

bool Game::collides(glm::vec3 pos, glm::vec3 objPos, float objWidth) { 
    return (!(pos.x < objPos.x - objWidth ||
              pos.x > objPos.x + objWidth ||
              pos.z < objPos.z - objWidth ||
              pos.z > objPos.z + objWidth));
}

void Game::interact() {
    // Ray march to get the cube intersection
    float depth = 0.0f;
    int index = -1;
    bool isMonster = false;
    float recordedDepth = -1.0f;
    for (int i = 0; i < 25 && index == -1; i++) {
        float closest = 10000.0f;
        glm::vec3 nCamPos = camera.position + depth * camera.front;
        for (int i = 0; i < objects.size(); i++) {
            float dst = glm::distance(nCamPos, objects[i].pos);
            if (dst < closest) {
                closest = dst;
                if (!(nCamPos.x < objects[i].pos.x - 0.5f ||
                      nCamPos.x > objects[i].pos.x + 0.5f ||
                      nCamPos.z < objects[i].pos.z - 0.5f ||
                      nCamPos.z > objects[i].pos.z + 0.5f ||
                      nCamPos.y < objects[i].pos.y ||
                      nCamPos.y > objects[i].pos.y + objects[i].depth)) {
                    if (index == -1 || recordedDepth < depth) {
                        index = i;
                        recordedDepth = depth;
                        isMonster = false;
                    }
                    continue;
                }
            }
        }
        for (int i = 0; i < monsters.size(); i++) {
            float dst = glm::distance(nCamPos, monsters[i].position);
            if (dst < closest) {
                closest = dst;
                if (!(nCamPos.x < monsters[i].position.x - 0.5f ||
                      nCamPos.x > monsters[i].position.x + 0.5f ||
                      nCamPos.z < monsters[i].position.z - 0.5f ||
                      nCamPos.z > monsters[i].position.z + 0.5f ||
                      nCamPos.y < monsters[i].position.y ||
                      nCamPos.y > monsters[i].position.y + 1.6f)) {
                    if (index == -1 || recordedDepth < depth) {
                        index = i;
                        isMonster = true;
                        recordedDepth = depth;
                    }
                    continue;
                }
            }
        }
        depth += closest / 5.0f;
    }
    if (isMonster) {
        std::cout << "Monster selected: " << index << std::endl;
    } else {
        std::cout << "Object selected: " << index << std::endl;
    }
//    std::cout << index << std::endl;
}

void Game::escape(bool es) {
    if (es) {
        glfwSetInputMode(nativeWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    } else {
        glfwSetInputMode(nativeWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    }
}

void Game::renderGUI() {
    float y = 10.0f;
    for (int i = 0; i < notifications.size(); i++) {
        Notification &notification = notifications[i];
        ImGui::SetNextWindowPos(ImVec2{ 10.0f, y });
        ImGui::SetNextItemWidth(windowSize.x * 0.8f);
        bool shouldClose = false;
        if (notification.live) {
            bool open = true;
            notification.aliveTime -= deltaTime;
            ImGui::Begin(notification.title.c_str(), &open, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoResize);
            ImGui::Text("%s", notification.content.c_str());
            shouldClose = notification.aliveTime < 0.0f;
        } else {
            ImGui::Begin(notification.title.c_str());
            ImGui::Text("%s", notification.content.c_str());
            shouldClose = ImGui::Button("Dismiss");
        }
        if (shouldClose) {
            notifications.erase(notifications.begin() + i, notifications.begin() + i + 1);
            i--;
        }
        ImVec2 size = ImGui::GetWindowSize();
        ImGui::End();
        y += size.y + 10.0f;
    }
//    ImGui::ShowDemoWindow();
}
