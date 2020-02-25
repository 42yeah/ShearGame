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


class Game;

enum ObjectType {
    PASSABLE, OBSTACLE, SITTABLE, SLEEPABLE
};

enum ObstacleType {
    NOPE, CHEST, WELL
};

class Object {
public:
    Object() {}
    Object(glm::vec3 pos, Model *model, glm::mat4 modelMat, ObjectType type, ObstacleType obsType);
    
    void render(Program &program);
    
    void interact(Game *game);

    glm::vec3 pos;
    glm::mat4 modelMat;
    Model *model;
    Object *prev;
    ObjectType type;
    ObstacleType obstacleType;
    float depth;
    
    int wellCounter;
};

#endif /* Object_hpp */
