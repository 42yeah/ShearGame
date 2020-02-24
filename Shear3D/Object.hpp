//
//  Object.hpp
//  Shear3D
//
//  Created by 周昊 on 2020/2/20.
//  Copyright © 2020 aiofwa. All rights reserved.
//

#ifndef Object_hpp
#define Object_hpp

#include <glm/glm.hpp>
#include "Model.hpp"


enum ObjectType {
    PASSABLE, OBSTACLE, SITTABLE, SLEEPABLE
};

class Object {
public:
    Object() {}
    Object(glm::vec3 pos, Model *model, glm::mat4 modelMat, ObjectType type);
    
    void render(Program &program);

    glm::vec3 pos;
    glm::mat4 modelMat;
    Model *model;
    Object *prev;
    ObjectType type;
    float depth;
};

#endif /* Object_hpp */
