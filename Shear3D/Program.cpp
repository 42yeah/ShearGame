//
//  Program.cpp
//  Shear3D
//
//  Created by 周昊 on 2020/2/20.
//  Copyright © 2020 aiofwa. All rights reserved.
//

#include "Program.hpp"
#include <fstream>
#include <iostream>
#include <sstream>


Program::Program(std::string vPath, std::string fPath) { 
    link(vPath, fPath);
}

void Program::link(std::string vPath, std::string fPath, bool report) {
    vertexPath = vPath;
    fragmentPath = fPath;
    GLuint program = glCreateProgram();
    GLuint vs = compile(GL_VERTEX_SHADER, vPath);
    GLuint fs = compile(GL_FRAGMENT_SHADER, fPath);
    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);
    char log[512] = { 0 };
    glGetProgramInfoLog(program, sizeof(log), nullptr, log);
    setLog(0, std::string(log));
    this->program = program;
    if (report) {
        this->report();
    }
}

GLuint Program::compile(GLuint type, std::string path) { 
    GLuint shader = glCreateShader(type);
    std::ifstream reader(path);
    std::stringstream stream;
    stream << reader.rdbuf();
    std::string src = stream.str();
    const char *raw = src.c_str();
    glShaderSource(shader, 1, &raw, nullptr);
    glCompileShader(shader);
    char log[512] = { 0 };
    glGetShaderInfoLog(shader, sizeof(log), nullptr, log);
    setLog(type, std::string(log));
    return shader;
}

void Program::setLog(GLuint type, std::string log) { 
    switch (type) {
        case GL_VERTEX_SHADER:
            vertexLog = log;
            break;
            
        case GL_FRAGMENT_SHADER:
            fragmentLog = log;
            break;
            
        default:
            programLog = log;
            break;
    }
}

void Program::report() {
    bool erred = false;
    if (vertexLog != "") {
        std::cout << vertexPath << ": " << vertexLog << std::endl;
        erred = true;
    }
    if (fragmentLog != "") {
        std::cout << fragmentPath << ": " << fragmentLog << std::endl;
        erred = true;
    }
    if (programLog != "") {
        std::cout << "Program: " << programLog << std::endl;
        erred = true;
    }
    if (!erred) {
        std::cout << "The linking is success without errors." << std::endl;
    }
}

GLuint Program::loc(std::string uniformName) {
    if (locationCache.count(uniformName) <= 0) {
        GLuint loc = glGetUniformLocation(program, uniformName.c_str());
        if (loc == -1) {
            std::cout << "WARNING: Couldn't find " << uniformName << " at " << vertexPath << " and " << fragmentPath << "." << std::endl;
        }
        locationCache[uniformName] = loc;
        return loc;
    } else {
        return locationCache[uniformName];
    }
}

void Program::use() { 
    glUseProgram(program);
}

void Program::reload() {
    glDeleteProgram(program);
    link(vertexPath, fragmentPath);
    locationCache.clear();
}

