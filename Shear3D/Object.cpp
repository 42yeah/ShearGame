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


Object::Object(glm::vec3 pos, Model *model, glm::mat4 modelMat, ObjectType type, ObstacleType obsType) : pos(pos), model(model), modelMat(modelMat), prev(nullptr), type(type), obstacleType(obsType), wellCounter(1), destroyed(false) {
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
    if (obstacleType == TREE) {
        glUniform1f(program.loc("destroyness"), (wellCounter - 1) / 3.0f);
    } else {
        glUniform1f(program.loc("destroyness"), 0.0f);
    }
    
    glUniformMatrix4fv(program.loc("model"), 1, GL_FALSE, glm::value_ptr(modelMat));
    model->render(program);
}

void Object::interact(Game *game) {
    switch (obstacleType) {
        case WELL:
            game->flipper = 2.5f;
            game->stamina -= 0.02 * game->deltaTime;
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
            
        case TREE:
            if (game->getQuantityOf(AXE) >= 1) {
                for (int i = 0; i < game->monsters.size(); i++) {
                    if (glm::distance(game->monsters[i].position, game->camera.position) <= 8.0f) {
                        game->jail("You were found destroying the environment of the village.");
                        game->interactingObject = nullptr;
                        break;
                    }
                }
                game->flipper = 5.0f;
                game->escape(game->escaping = true);
                ImGui::SetNextWindowSize(ImVec2{ 300.0f, 100.0f }, ImGuiCond_FirstUseEver);
                ImGui::Begin("Chop");
                if (wellCounter <= 3) {
                    ImGui::Text("You started chopping the tree...");
                    if (ImGui::Button("Continue chopping")) {
                        wellCounter++;
                        game->addItem(Item(LOG, 1));
                        std::random_device dev;
                        std::uniform_int_distribution<> distrib(1, 100);
                        falls = distrib(dev);
                        int breaks = distrib(dev);
                        falls -= breaks;
                    }
                } else {
                    std::string msg = "The tree fells!\nBird eggs from top of it rains down!";
                    if (falls <= 0) {
                        msg += "\nBut all of them breaks...";
                    }
                    ImGui::Text("%s", msg.c_str());
                    if (falls > 0 && ImGui::Button("Get the eggs")) {
                        game->addItem(Item(EGG, falls));
                        game->interactingObject = nullptr;
                        destroyed = true;
                    }
                    std::string leave = "Just leave.";
                    if (falls > 0) {
                        leave += " I hate eggs!";
                    }
                    if (ImGui::Button(leave.c_str())) {
                        game->interactingObject = nullptr;
                        destroyed = true;
                    }
                }
                ImGui::End();
            } else {
                game->interactingObject = nullptr;
            }
            break;
            
        case CHEST:
            // TODO:
            game->interactingObject = nullptr;
            break;
            
        default:
            game->interactingObject = nullptr;
            break;
    }
}
