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


Monster::Monster(Texture *tex, int id, glm::vec3 position, GLuint VAO) : texture(tex), id(id), position(position), destination(position), VAO(VAO), destinationRamp(nullptr), pathIndex(0) {
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

void Monster::update(float dt, float time, int day, std::vector<Object> &objects) {
    if (ramps.size() <= 0) {
        return;
    }
    Ramp *prevRamp = nullptr, currentRamp;
    for (int i = 0; i < ramps.size(); i++) {
        if (ramps[i].time >= time) {
            currentRamp = ramps[i];
            if (i <= 0) {
                break;
            }
            prevRamp = &ramps[i - 1];
            break;
        }
    }
    // interpolate
//    float t = (time - prevRamp.time) / (currentRamp.time - prevRamp.time);
//    destination = glm::mix(prevRamp.destination, currentRamp.destination, t);
    if (prevRamp && destinationRamp != prevRamp) {
        destination = prevRamp->destination;
        destinationRamp = prevRamp;
        pathfind(destination, objects);
        pathIndex = 0;
    }

    if (path.size() > 0) {
        float speed = 4.0f;
        glm::vec3 dPos = path[pathIndex]->pos - position;
        float distance = glm::length(dPos);
//        float trop = glm::length(destinationRamp->destination - position);
        if (distance <= 0.1f) {
            if (pathIndex < path.size() - 1) {
                pathIndex++;
            }
        }
        if (pathIndex == path.size() - 1) {
            glm::vec3 dPos = destinationRamp->destination - position;
            position += dPos * speed * dt;
            return; // Get up chairs, etc.
        }
        glm::vec3 generalDirection = glm::normalize(dPos);
        if (glm::length(dPos) >= speed * dt) {
            position += generalDirection * glm::min(speed * dt, distance);
        } else {
            position += dPos * speed * dt;
        }
    }
}

void Monster::pathfind(glm::vec3 destination, std::vector<Object> &objects) {
    for (int i = 0; i < objects.size(); i++) {
        objects[i].prev = nullptr;
    }
    destination.y = 0.0f;
    destination = glm::round(destination);
    glm::vec3 u = glm::round(position);
    u.y = 0.0f;
    std::vector<Object *> frontier;
    Object *origin = lookup(u, objects);
    Object *result = nullptr;
    if (!origin) {
        return;
    }
    origin->prev = nullptr;
    frontier.push_back(origin);
    while (!frontier.empty()) {
        Object *o = frontier[0];
        frontier.erase(frontier.begin(), frontier.begin() + 1);
        if (!o || glm::length(o->pos - position) > 60.0f) {
            continue;
        }
//        std::cout << o->pos.x << ", " << o->pos.y << ", " << o->pos.z << " - " << destination.x << ", " << destination.y << ", " << destination.z << std::endl;
//        std::cout << "Destination " << glm::length(o->pos - destination) << std::endl;
        if (length(o->pos - destination) < 0.01f) {
            // We are here!
            result = o;
            break;
        }
        for (int z = -1; z <= 1; z++) {
            for (int x = -1; x <= 1; x++) {
                if (abs(x) == abs(z)) { continue; }
                Object *no = lookup(glm::vec3(o->pos.x + x, o->pos.y, o->pos.z + z), objects);
                if (no && no->type == PASSABLE && no->prev == nullptr && no != origin) {
//                    std::cout << "Pushing back " << no->pos.x << ", " << no->pos.y << ", " << no->pos.z << std::endl;
                    no->prev = o;
                    frontier.push_back(no);
                }
            }
        }
    }
//    std::cout << "Path found" << std::endl;
    path.clear();
    while (result) {
        path.insert(path.begin(), result);
        result = result->prev;
    }
}

Object *Monster::lookup(glm::vec3 pos, std::vector<Object> &objects) {
    for (int i = 0; i < objects.size(); i++) {
//        glm::vec3 dPos = objects[i].pos - pos;
        if (glm::length(objects[i].pos - pos) < 0.01f) {
            return &objects[i];
        }
//        float manhattan = fabs(dPos.x) + fabs(dPos.y);
//        if (manhattan < 0.5) {
//            return &objects[i];
//        }
    }
    return nullptr;
}




