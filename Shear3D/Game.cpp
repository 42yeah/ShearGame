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


Game::Game(GLFWwindow *window) : nativeWindow(window), reloadKeyPressed(false), firstMouse(true) {
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
    renderPass = Pass(windowSize);
    renderProgram = Program("Assets/default.vertex.glsl", "Assets/default.fragment.glsl");
    postEffectProgram = Program("Assets/posteffect.vertex.glsl", "Assets/posteffect.fragment.glsl");
    camera = Camera(glm::vec3(0.0f, 1.63f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    
    time = glfwGetTime();
    deltaTime = 0.0f;
    
    models = loadModels("Assets/Model/shear.1.obj", "Assets/Model");
    loadMap("Assets/map");
}

void Game::clear() {
    glClearColor(sunColor.r, sunColor.g, sunColor.b, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Game::render() {
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
    
    float s = additiveTime * 0.05f;
    sunDirection = glm::vec3(-cosf(s), -sin(s), 0.0f);
    sunColor = glm::mix(glm::vec3(0.01f, 0.01f, 0.01f), glm::vec3(0.9f, 0.9f, 0.99f), sin(s) * 0.5f + 0.5f);
//    sunColor += glm::vec3(1.0f, 0.5f, 0.0f) * glm::max(0.0f, cosf(s));
//    sunColor = glm::normalize(sunColor);
    
    if (glfwGetKey(nativeWindow, GLFW_KEY_R)) {
        if (!reloadKeyPressed) {
            reloadKeyPressed = true;
            renderProgram.reload();
            postEffectProgram.reload();
            loadMap("Assets/map");
        }
    } else {
        reloadKeyPressed = false;
    }
    glm::vec3 f(camera.front.x, camera.front.y, camera.front.z);
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

void Game::mouseEvent(glm::vec2 mousePos) { 
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
    Object object(pos, &models[id], off);
    objects.push_back(object);
}

