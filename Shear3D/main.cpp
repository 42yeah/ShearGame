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
#include "Program.hpp"
#include "Pass.hpp"
#include "tests.hpp"


int main(int argc, const char * argv[]) {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    GLFWwindow *window = glfwCreateWindow(800, 600, "Shear 3D", nullptr, nullptr);
    glfwMakeContextCurrent(window);
    gladLoadGL();
    
    int w, h;
    glfwGetWindowSize(window, &w, &h);
    Program program("Assets/default.vertex.glsl", "Assets/default.fragment.glsl");
    program.report();
    Program fboProgram("Assets/fbo.vertex.glsl", "Assets/fbo.fragment.glsl");
    fboProgram.report();
    GLuint triangle = generateTestTriangle();
    GLuint rect = generateTestRect();
    Pass pass(glm::ivec2(w * 2, h * 2));
    
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        
        pass.use();
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glBindVertexArray(triangle);
        program.use();
        glDrawArrays(GL_TRIANGLES, 0, 3);
        pass.unuse();
        
//        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glBindVertexArray(rect);
        fboProgram.use();
        pass.pass(fboProgram.loc("tex"), 0);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        glfwSwapBuffers(window);
    }
    return 0;
}
