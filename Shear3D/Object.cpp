//
//  Object.cpp
//  Shear3D
//
//  Created by 周昊 on 2020/2/20.
//  Copyright © 2020 aiofwa. All rights reserved.
//

#include "Object.hpp"
#include <glm/gtc/type_ptr.hpp>
#include "../Includes/imgui/imgui.h"
#include "Game.hpp"
#include "Item.hpp"


Object::Object(glm::vec3 pos, Model *model, glm::mat4 modelMat, ObjectType type, ObstacleType obsType) : pos(pos), model(model), modelMat(modelMat), prev(nullptr), type(type), obstacleType(obsType), wellCounter(1) {
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
    switch (obstacleType) {
        case WELL:
            game->flipper = 2.0f;
            game->stamina -= 0.05 * game->deltaTime;
            game->escape(game->escaping = true);
            ImGui::SetNextWindowSize(ImVec2{ 300.0f, 100.0f }, ImGuiCond_FirstUseEver);
            ImGui::Begin("Well");
            if (wellCounter <= 50) {
                ImGui::Text("You found %d coins in the well!", wellCounter);
                if (ImGui::Button("OK")) {
                    wellCounter++;
                    game->addItem(Item(COIN, 1));
                }
            } else {
                ImGui::Text("You can't find anymore coins for now.");
                if (ImGui::Button("OK")) {
                    wellCounter = 1;
                    game->interactingObject = nullptr;
                }
            }
            ImGui::End();
            break;
            
        case CHEST:
            // TODO:
            break;
            
        default:
            break;
    }
}
