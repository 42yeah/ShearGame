//
//  Camera.hpp
//  Shear3D
//
//  Created by 周昊 on 2020/2/20.
//  Copyright © 2020 aiofwa. All rights reserved.
//

#ifndef Camera_hpp
#define Camera_hpp

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "../Includes/glad/glad.h"
#include "Program.hpp"


class Camera {
public:
    Camera() {}
    Camera(glm::vec3 p, glm::vec3 f, glm::vec3 u);
    
    glm::mat4 perspective(float aspect);
    glm::mat4 view();
    void pass(float aspect, GLuint v, GLuint p);

    glm::vec3 position;
    glm::vec3 front;
    glm::vec3 up;
};

#endif /* Camera_hpp */
