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
            game->stamina -= 0.1 * game->deltaTime;
            if (game->getQuantityOf(AXE) >= 1) {
                for (int i = 0; i < game->monsters.size() && wellCounter > 1; i++) {
                    if (glm::distance(game->monsters[i].position, game->camera.position) <= 7.0f) {
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
        {
            if (chestId == 4) {
                game->notifications.push_back(Notification("Fails", "This chest won't open.", true, 10.0f));
                game->interactingObject = nullptr;
                break;
            }
            bool stealing = false;
            for (int i = 0; i < game->monsters.size() && chestId != 4; i++) {
                // That's a very large distance!
                if (glm::distance(game->camera.position, game->monsters[i].position) <= 10.0f) {
//                    game->jail("You were found stealing.");
                    stealing = true;
                    break;
                }
            }
            if (stealing) { // Won't even let you see the UI!
//                game->interactingObject = nullptr;
            }
            if (chestId == 3) {
                syncItemCount(STEAK, game->steaks);
            } else if (chestId == 5) {
                syncItemCount(TACO, game->steaks);
            } else if (chestId == 12) {
                syncItemCount(EGG, game->eggCount);
            }
            game->escape(game->escaping = true);
#ifdef __APPLE__
            ImGui::SetNextWindowPos(ImVec2{ game->windowSize.x * 0.5f - 620.0f, game->windowSize.y * 0.5f - 210.0f }, ImGuiCond_FirstUseEver);
#else
            ImGui::SetNextWindowPos(ImVec2{ game->windowSize.x - 620.0f, game->windowSize.y - 210.0f }, ImGuiCond_FirstUseEver);
#endif
            ImGui::SetNextWindowSize(ImVec2{ 300.0f, 200.0f }, ImGuiCond_FirstUseEver);
            ImGui::Begin("Stuffs");
            if (ImGui::Button("Close", ImVec2(ImGui::GetWindowSize().x * 0.5f, 0.0f))) {
                game->interactingObject = nullptr;
            }
            for (int i = 0; i < storage.size(); i++) {
                if (ImGui::Button(storage[i].getItemName(true).c_str())) {
                    game->addItem(storage[i]);
                    storage.erase(storage.begin() + i, storage.begin() + i + 1);
                    // Shoplifting steaks; how is that possible?
                    if (chestId == 3 || chestId == 5) {
                        game->steaks = 0;
                    } else if (chestId == 12) {
                        game->eggCount = 0;
                    }
                }
                if (i % 3 < 2) {
                    ImGui::SameLine();
                }
            }
            ImGui::End();
            break;
        }

        default:
            game->interactingObject = nullptr;
            break;
    }
}

void Object::addItem(Item item) { 
    for (int i = 0; i < storage.size(); i++) {
        if (storage[i].type == item.type) {
            storage[i].quantity += item.quantity;
            if (storage[i].quantity <= 0) {
                storage.erase(storage.begin() + i, storage.begin() + i + 1);
            }
            return;
        }
    }
    if (item.quantity > 0) {
        storage.push_back(item);
    }
}

int Object::getQuantityOf(ItemType type) { 
    for (int i = 0; i < storage.size(); i++) {
        if (storage[i].type == type) {
            return storage[i].quantity;
        }
    }
    return 0;
}

void Object::syncItemCount(ItemType type, int count) {
    for (int i = 0; i < storage.size(); i++) {
        if (storage[i].type == type) {
            storage.erase(storage.begin() + i, storage.begin() + i + 1);
        }
    }
    addItem(Item(type, count));
}



