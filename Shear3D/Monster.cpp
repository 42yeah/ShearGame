//
//  Monster.cpp
//  Shear3D
//
//  Created by 周昊 on 2020/2/21.
//  Copyright © 2020 aiofwa. All rights reserved.
//

#include "Monster.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


Monster::Monster(Texture *tex, int id, glm::vec3 position, GLuint VAO) : texture(tex), id(id), position(position), destination(position), VAO(VAO) {
}

void Monster::render(Program &program) { 
    glm::mat4 model(1.0f);
    model = glm::translate(model, position);
    glUniform1i(program.loc("id"), id);
    glUniformMatrix4fv(program.loc("model"), 1, GL_FALSE, glm::value_ptr(model));
    texture->pass(program.loc("tex"), 0);
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

void Monster::update(float dt, float time, int day) {
    bool ramped = false;
    Ramp prevRamp, currentRamp;
    for (int i = 0; i < ramps.size(); i++) {
        if (ramps[i].time >= time) {
            currentRamp = ramps[i];
            if (i <= 0) {
                break;
            }
            ramped = true;
            prevRamp = ramps[i - 1];
            break;
        }
    }
    // interpolate
    if (ramped) {
        float t = (time - prevRamp.time) / (currentRamp.time - prevRamp.time);
        destination = glm::mix(prevRamp.destination, currentRamp.destination, t);
    }
    if (!ramped && ramps.size() > 0) {
        destination = ramps[ramps.size() - 1].destination;
    }
    float speed = 4.0f;
    glm::vec3 dPos = destination - position;
    float distance = glm::length(dPos);
    position += dPos * glm::min(speed * dt, distance);
}


