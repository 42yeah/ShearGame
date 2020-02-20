//
//  Model.cpp
//  Shear3D
//
//  Created by 周昊 on 2020/2/20.
//  Copyright © 2020 aiofwa. All rights reserved.
//

#include "Model.hpp"
#include <glm/gtc/type_ptr.hpp>
#define TINYOBJLOADER_IMPLEMENTATION
#include "../Includes/tiny_obj_loader.h"


std::vector<Model> loadModels(std::string path, std::string mtlPath) {
    std::string error, warning;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    tinyobj::attrib_t attribs;
    
    tinyobj::LoadObj(&attribs, &shapes, &materials, &warning, &error, path.c_str(), mtlPath.c_str());
    std::vector<Model> models;
    if (warning != "") {
        std::cout << path << " WARNING: " << warning << std::endl;;
    }
    if (error != "") {
        std::cout << path << " ERR! " << error << std::endl;;
        return models;
    }
    
    // Load the central texture first.
    if (materials.size() < 0) {
        std::cout << "ERR! No texture." << std::endl;
        return models;
    }
    Texture tex(mtlPath + "/" + materials[0].diffuse_texname);
    
    for (int i = 0; i < shapes.size(); i++) {
        tinyobj::shape_t &shape = shapes[i];
        tinyobj::mesh_t &mesh = shape.mesh;
        std::vector<Vertex> vertices;
        for (int j = 0; j < mesh.indices.size(); j++) {
            tinyobj::index_t index = mesh.indices[j];
            Vertex v = {
                glm::vec3(attribs.vertices[index.vertex_index * 3],
                          attribs.vertices[index.vertex_index * 3 + 1],
                          attribs.vertices[index.vertex_index * 3 + 2]
                          ),
                glm::vec3(attribs.normals[index.normal_index * 3],
                          attribs.normals[index.normal_index * 3 + 1],
                          attribs.normals[index.normal_index * 3 + 2]
                          ),
                glm::vec2(attribs.texcoords[index.texcoord_index * 2],
                          attribs.texcoords[index.texcoord_index * 2 + 1]
                          )
            };
            vertices.push_back(v);
        }
        GLuint VAO, VBO;
        glGenVertexArrays(1, &VAO);
        glBindVertexArray(VAO);
        
        glGenBuffers(1, &VBO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 8, nullptr);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 8, (void *) (sizeof(float) * 3));
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 8, (void *) (sizeof(float) * 6));
        
        Model model(VAO, (int) mesh.indices.size(), tex);
        models.push_back(model);
    }
    std::cout << "Model " << path << " loading finished. #vertices: " << attribs.vertices.size() << std::endl;
    return models;
}


Model::Model(GLuint vao, int nVertices, Texture texture) : VAO(vao), numVertices(nVertices), texture(texture) {

}

void Model::render(Program &program) {
    program.use();
    texture.pass(program.loc("tex"), 0);
    glUniformMatrix4fv(program.loc("model"), 1, GL_FALSE, glm::value_ptr(glm::mat4(1.0)));
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, numVertices);
}
