//
//  Camera.cpp
//  Shear3D
//
//  Created by 周昊 on 2020/2/20.
//  Copyright © 2020 aiofwa. All rights reserved.
//

#include "Camera.hpp"
#include <glm/gtc/type_ptr.hpp>


Camera::Camera(glm::vec3 p, glm::vec3 f, glm::vec3 u) : position(p), front(glm::normalize(f)), up(u) {
    pitch = 0.0f;
    yaw = 90.0f;
}


glm::mat4 Camera::perspective(float aspect) {
    return glm::perspective(glm::radians(45.0f), aspect, 0.01f, 100.0f);
}

glm::mat4 Camera::view() {
    front.x = cos(glm::radians(pitch)) * cos(glm::radians(yaw));
    front.z = cos(glm::radians(pitch)) * sin(glm::radians(yaw));
    front.y = sin(glm::radians(pitch));
    front = glm::normalize(front);
    return glm::lookAt(position, position + front, up);
}

void Camera::pass(float aspect, GLuint v, GLuint p) {
    glm::mat4 vue = view();
    glm::mat4 perspec = perspective(aspect);
    glUniformMatrix4fv(v, 1, GL_FALSE, glm::value_ptr(vue));
    glUniformMatrix4fv(p, 1, GL_FALSE, glm::value_ptr(perspec));
}
