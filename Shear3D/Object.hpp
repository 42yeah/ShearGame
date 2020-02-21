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


class Object {
public:
    Object() {}
    Object(Model *model, glm::mat4 modelMat);
    
    void render(Program &program);

    glm::mat4 modelMat;
    Model *model;
};

#endif /* Object_hpp */
