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
    
    models = loadModels("Assets/Model/shear.obj", "Assets/Model");
}

void Game::clear() {
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Game::render() {
    renderPass.use();
    glEnable(GL_DEPTH_TEST);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    renderProgram.use();
    camera.pass(aspect, renderProgram.loc("view"), renderProgram.loc("perspective"));
    glUniformMatrix4fv(renderProgram.loc("model"), 1, GL_FALSE, glm::value_ptr(glm::scale(glm::mat4(1.0f), glm::vec3(100.0f, 100.0f, 100.0f))));
    glBindVertexArray(ground);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    // === RENDER MODELS === //
    models[2].render(renderProgram);
    renderPass.unuse();
    
    glDisable(GL_DEPTH_TEST);
    glClear(GL_COLOR_BUFFER_BIT);
    postEffectProgram.use();
    renderPass.pass(postEffectProgram.loc("tex"), 0);
    glBindVertexArray(rect);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

void Game::update() { 
    updateWindowSize();
    double now = glfwGetTime();
    deltaTime = (float) now - time;
    time = now;
    
    if (glfwGetKey(nativeWindow, GLFW_KEY_R)) {
        if (!reloadKeyPressed) {
            reloadKeyPressed = true;
            renderProgram.reload();
            postEffectProgram.reload();
        }
    } else {
        reloadKeyPressed = false;
    }
    if (glfwGetKey(nativeWindow, GLFW_KEY_W)) {
        camera.position += camera.front * deltaTime * 4.0f;
    }
    if (glfwGetKey(nativeWindow, GLFW_KEY_A)) {
        camera.position -= glm::cross(camera.front, camera.up) * deltaTime * 4.0f;
    }
    if (glfwGetKey(nativeWindow, GLFW_KEY_S)) {
        camera.position -= camera.front * deltaTime * 4.0f;
    }
    if (glfwGetKey(nativeWindow, GLFW_KEY_D)) {
        camera.position += glm::cross(camera.front, camera.up) * deltaTime * 4.0f;
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