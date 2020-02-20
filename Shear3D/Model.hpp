//
//  Model.hpp
//  Shear3D
//
//  Created by 周昊 on 2020/2/20.
//  Copyright © 2020 aiofwa. All rights reserved.
//

#ifndef Model_hpp
#define Model_hpp

#include <vector>
#include <iostream>
#include <glm/glm.hpp>
#include "../Includes/glad/glad.h"
#include "Texture.hpp"
#include "Program.hpp"


struct Vertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 texCoord;
};

class Model {
public:
    Model() {}
    Model(GLuint vao, int nVertices, Texture texture);
    
    void render(Program &program);
    GLuint VAO;

private:
    Texture texture;
    int numVertices;
};


std::vector<Model> loadModels(std::string path, std::string mtlPath);

#endif /* Model_hpp */
