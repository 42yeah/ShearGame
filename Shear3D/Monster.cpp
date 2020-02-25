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
#include "../Includes/imgui/imgui.h"
#include "Game.hpp"
#include "Item.hpp"


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
                rampIndex = -1;
                break;
            }
            rampIndex = i - 1;
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
        if (destinationRamp && pathIndex == path.size() - 1) {
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
        if (length(o->pos - destination) < 0.01f) {
            // We are here!
            result = o;
            break;
        }
        glm::vec3 dPos = destination - o->pos;
        for (int z = -1; z <= 1; z++) {
            for (int x = -1; x <= 1; x++) {
                if (abs(x) == abs(z)) { continue; }
                Object *no = lookup(glm::vec3(o->pos.x + x, o->pos.y, o->pos.z + z), objects);
                if (no && no->type != OBSTACLE && no->prev == nullptr && no != origin) {
                    no->prev = o;
                    if (dPos.x * x > 0 || dPos.z * z > 0) {
                        // General direction is right; append
                        frontier.insert(frontier.begin(), no);
                    } else {
                        frontier.push_back(no);
                    }
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
        if (glm::length(objects[i].pos - pos) < 0.01f) {
            return &objects[i];
        }
    }
    return nullptr;
}

void Monster::interact(Game *game) {
    game->escape(game->escaping = true);
    ImGui::SetNextWindowSize(ImVec2{ 400.0f, 300.0f }, ImGuiCond_FirstUseEver);
    ImGui::Begin("Dialogue");
    switch (id) {
        case 0:
            switch (rampIndex) {
                case 0:
                    ImGui::Text("Zzz...");
                    break;
                    
                case 1:
                    switch (conversationId) {
                        case 0:
                            ImGui::Text("Just going out for a walk here.");
                            if (ImGui::Button("Punch him in the face")) {
                                std::random_device dev;
                                std::uniform_int_distribution<> distrib(1, 2);
                                conversationId = distrib(dev);
                            }
                            break;
                            
                        case 1:
                            ImGui::Text("What the hell? Up yours, too!");
                            break;
                        
                        case 2:
                            ImGui::Text("POLICE! There's a maniac here punching people!");
                            break;
                    }
                    break;
                    
                case 2:
                    switch (conversationId) {
                        case 0:
                            ImGui::Text("Look at the store. Isn't it pretty?");
                            if (ImGui::Button("Punch him in the face")) {
                                std::random_device dev;
                                std::uniform_int_distribution<> distrib(1, 2);
                                conversationId = distrib(dev);
                            }
                            break;
                            
                        case 1:
                            ImGui::Text("What the hell? Up yours, too!");
                            break;
                        
                        case 2:
                            ImGui::Text("POLICE! There's a maniac here punching people!");
                            break;
                    }
                    break;
                    
                case 3:
                    ImGui::Text("Hi there. I am preparing for the shop.");
                    break;
                    
                case 4:
                    if (game->steaks > 0) {
                        bool canBuy = game->getQuantityOf(COIN) >= game->steakPrice;
                        std::string msg = "Hello! We've got %d steaks now,\n and it costs %d each. Wanna have one?";
                        if (!canBuy) {
                            msg += "\nOh, but looks like you don't have enough coins...";
                        }
                        ImGui::Text(msg.c_str(), game->steaks, (int) game->steakPrice);
                        if (canBuy && ImGui::Button("OK")) {
                            game->addItem(Item(COIN, (int) -game->steakPrice));
                            game->addItem(Item(STEAK, 1));
                            game->steaks--;
                        }
                    } else {
                        ImGui::Text("Sorry, we've got no steaks now. But we are cooking!");
                    }
                    
                    break;
            }
            break;
            
        case 1:
            switch (rampIndex) {
                case 0:
                    switch (conversationId) {
                        case 0:
                            ImGui::Text("What the hell are you doing in my room?");
                            if (ImGui::Button("Just visiting around.")) {
                                conversationId = 1;
                            }
                            break;

                        case 1:
                            ImGui::Text("POLICE!");
                            break;
                    }
                    
                    break;
                    
                case 1:
                    ImGui::Text("Hello, this is my home and we are eating! Get out!");
                    break;
                    
                case 2:
                    ImGui::Text("Could you get out of my way?");
                    break;
                    
                case 3:
                    ImGui::Text("Where I am going to is none of your business.");
                    break;
                    
                case 4:
                    ImGui::Text("Go away.");
                    break;
                    
                case 5:
                    ImGui::Text("Could you get out of my way? Going to work here.");
                    break;
                    
                case 6:
                    switch (conversationId) {
                        case 0:
                        {
                            if (game->eggCount > 0) {
                                std::string say = "Got %d eggs today. Buy them all for just %d coins.\nTake it or leave it.";
                                bool canBuy = game->getQuantityOf(COIN) >= game->eggPrice;
                                if (!canBuy) {
                                    say += "\nYou don't even have that much money!";
                                }
                                ImGui::Text(say.c_str(), game->eggCount, (int) game->eggPrice);
                                if (canBuy && ImGui::Button("Purchase")) {
                                    conversationId = 2;
                                    game->addItem(Item(EGG, game->eggCount));
                                    game->addItem(Item(COIN, -game->eggPrice));
                                    game->eggCount = 0;
                                }
                            } else {
                                ImGui::Text("No more eggs today. Bye!");
                            }
                            
                            break;
                        }
                            
                        case 2:
                            ImGui::Text("OK. Now pack your shite and leave it!");
                            break;
                    }
                    
            }
            break;
            
        default:
            break;
    }
    if (ImGui::Button("Try to leave") || destinationRamp != game->interactingMonsterRamp) {
        if (id == 1 && conversationId == 1) {
            game->jail("You were found breaking into someone's house.");
        }
        if (id == 0 && conversationId == 1) {
            game->hospital("You got punched straight in the face and lose conciousness.");
        }
        if (id == 0 && conversationId == 2) {
            game->jail("You were found beating the chef up.");
        }
        game->interactingMonster = nullptr;
    }
    ImGui::End();
}





