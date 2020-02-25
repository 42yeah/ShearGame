//
//  Object.cpp
//  Shear3D
//
//  Created by 周昊 on 2020/2/20.
//  Copyright © 2020 aiofwa. All rights reserved.
//

#include "Object.hpp"
#include <glm/gtc/type_ptr.hpp>


Object::Object(glm::vec3 pos, Model *model, glm::mat4 modelMat, ObjectType type) : pos(pos), model(model), modelMat(modelMat), prev(nullptr), type(type) {
    switch (type) {
        case PASSABLE:
            depth = 0.0f;
            break;
            
        case OBSTACLE:
            depth = 1.0f;
            break;
            
        case SITTABLE:
            depth = 0.5f;
            break;
            
        case SLEEPABLE:
            depth = 0.3f;
            break;
    }
}

void Object::render(Program &program) {
    glUniformMatrix4fv(program.loc("model"), 1, GL_FALSE, glm::value_ptr(modelMat));
    model->render(program);
}

void Object::interact(Game *game) { 
    
}


