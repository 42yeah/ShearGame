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


Monster::Monster(Texture *tex, int id, glm::vec3 position, GLuint VAO) : texture(tex), id(id), position(position), destination(position), VAO(VAO), destinationRamp(nullptr), pathIndex(0), mugCounter(-5), texId(id) {
}

void Monster::render(Program &program) { 
    glm::mat4 model(1.0f);
    model = glm::translate(model, position);
    glUniform1i(program.loc("id"), texId);
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
            
        case 2:
            switch (rampIndex) {
                case 0:
                    ImGui::Text("Zzz... OH POLICE!");
                    break;
                    
                case 1:
                    switch (conversationId) {
                        case 0:
                            ImGui::Text("Alright. You got %d days left. You got enough eggs?", 365 - game->day);
                            if (ImGui::Button("Yep")) {
                                conversationId = 1;
                            }
                            if (ImGui::Button("Nope")) {
                                conversationId = 2;
                            }
                            break;
                            
                        case 1:
                        {
                            std::string msg = "Let me see...";
                            bool enough = game->getQuantityOf(EGG) >= 1000;
                            if (!enough) {
                                msg += "\n(you do not have enough egg.)";
                            }
                            ImGui::Text("%s", msg.c_str());
                            if (enough && ImGui::Button("Show him the eggs.")) {
                                game->addItem(Item(EGG, -1000));
                                conversationId = 3;
                            }
                            if (ImGui::Button("Punch him. Now!")) {
                                conversationId = 4;
                            }
                            break;
                        }
                            
                        case 2:
                            ImGui::Text("Well, better go get it then.\nIf you still don't get 1000 eggs before the end of this year,\nyou will know the taste of eternal hell... HARRRR HARRH RHRRRR!");
                            break;
                            
                        case 3:
                            ImGui::Text("I am impressed. Alright, I will withdraw the sue.\n(YOU WIN THE GAME!)");
                            break;
                            
                        case 4:
                            ImGui::Text("Oh, you fecker! You are so gonna be dead! POLICE!");
                            break;
                    }
                    break;
                    
                case 2:
                case 3:
                    ImGui::Text("Ha, it's you. I am not at my office hours. Come back later, when I am at my desk.");
                    break;
                    
                case 4:
                    ImGui::Text("Isn't this nice? I love my life.\nDo you hate yours?");
                    break;
                    
                case 5:
                case 6:
                case 7:
                case 8:
                    ImGui::Text("Hm, It's you. I am not at my office hours. Come back later, when I am at my desk.");
                    break;
                    
                case 9:
                    switch (conversationId) {
                        case 0:
                            ImGui::Text("Om nom nom... What are you doing here?");
                            if (ImGui::Button("Punch him. Now!")) {
                                conversationId = 4;
                            }
                            break;
                            
                        case 4:
                            ImGui::Text("*PSSHHAUARAHJSIF*\n(You've got a partially-eaten lavish meal!)\nYou FECKING FECKER! WHAT THE FECK? POLIZEI!");
                            break;
                    }
                    break;
            }
            break;
            
        case 3:
            switch (rampIndex) {
                case 0:
                    ImGui::Text("Zzz... Oh, it's you. Zzz...");
                    break;
                    
                case 1:
                    ImGui::Text("Having breakfast here. Leave me alone!");
                    break;
                    
                case 2:
                case 4:
                    switch (conversationId) {
                        case 0:
                        {
                            int price = 450;
                            bool affordable = price <= game->getQuantityOf(COIN);
                            if (affordable && !game->axed) {
                                ImGui::Text("Hey, look, I have a spare axe and I am looking to sell it.\nDo you wanna buy? This is just between you and me.");
                            } else {
                                ImGui::Text("I live in this forest. I chop trees down for a livin'.");
                            }
                            if (affordable && ImGui::Button("OK, I'll take it")) {
                                conversationId = 1;
                                game->addItem(Item(COIN, -price));
                                game->addItem(Item(AXE, 1));
                            }
                            if (ImGui::Button("Why do you live here?")) {
                                conversationId = 2;
                            }
                            
                            break;
                        }
                            
                        case 1:
                            ImGui::Text("There you are.\nHave fun using it, it costed me 10 coins!\n... I mean 500!");
                            break;
                            
                        case 2:
                            ImGui::Text("Because I am a forester. Foresters live in forests.");
                            if (ImGui::Button("Oh yeah?")) {
                                conversationId = 3;
                            }
                            break;
                            
                        case 3:
                            ImGui::Text("Of course!\nYou looking for a fight?\nA fight is what you are gonna get!");
                            break;
                    }
                    break;

                case 3:
                    ImGui::Text("Isn't today lovely? I mean look at the sky!");
                    break;
            }
            break;
            
        case 4:
            switch (rampIndex) {
                case 0:
                    switch (conversationId) {
                        case 0:
                        {
                            ImGui::Text("Zzz... Hmm Caterina...");
                            if (ImGui::Button("Impersonate Caterina")) {
                                conversationId = 1;
                            }
                            break;
                        }
                            
                        case 1:
                            ImGui::Text("How are you going to impersonate?");
                            if (ImGui::Button("I love you too, Johnson...")) {
                                conversationId = 2;
                            }
                            if (ImGui::Button("You jump, I jump, remember?")) {
                                conversationId = 2;
                            }
                            if (ImGui::Button("We were on a break!")) {
                                conversationId = 2;
                            }
                            if (ImGui::Button("Feck off.")) {
                                conversationId = 2;
                            }
                            break;
                            
                        case 2:
                            ImGui::Text("What the hell are you talking about? THEIF!");
                            break;
                    }
                    break;
                    
                case 1:
                    ImGui::Text("Good morning.\nGoing to my girlfriend's to have breakfast.");
                    break;
                    
                case 2:
                case 3:
                    ImGui::Text("My girlfriend is a little bit grumpy at times.\nNevermind that!");
                    break;
                    
                case 4:
                    ImGui::Text("Gotta go to work now.");
                    break;
                    
                case 5:
                case 6:
                    ImGui::Text("The commute is long, yes,\nbut it's not like I can do anything to change it!");
                    break;
                    
                case 7:
                case 8:
                    ImGui::Text("See the big mansion over there?\nIt belongs to a very rich man.\nI hope I can live in those houses one day!");
                    break;
                    
                case 9:
                    switch (conversationId) {
                        case 0:
                        {
                            int nightFee = 100;
                            bool affordable = nightFee <= game->getQuantityOf(COIN);
                            std::string msg = "Hello and welcome to the village hotel.";
                            if (affordable && !game->rented) {
                                msg += "\nDo you want to stay here for the night?\nIt costs " + std::to_string(nightFee) + " coins.\nWe also provide unlimited buffet to those who stays.";
                            }
                            if (!affordable && !game->rented) {
                                msg += "\nOh... Sorry, but you didn't seem to bring enough money!";
                            }
                            if (game->rented) {
                                msg += "\nAre you hungry?\nWe provide taco buffet for those who are hungry.\nTaken straight from the village chef!";
                            }
                            ImGui::Text("%s", msg.c_str());
                            if (!game->rented && affordable && ImGui::Button("I'd like to stay here for the night.")) {
                                conversationId = 1;
                                game->rented = true;
                                game->addItem(Item(COIN, -nightFee));
                            }
                            if (ImGui::Button("There are no customers here.")) {
                                conversationId = 2;
                            }
                            if (ImGui::Button("Hands up, prepare to get mugged!")) {
                                conversationId = 3;
                            }
                            if (game->rented && ImGui::Button("The chef makes taco too?")) {
                                conversationId = 4;
                            }
                            if (game->rented && ImGui::Button("I'd love to have one.")) {
                                if (game->steaks > 0) {
                                    conversationId = 5;
                                    game->steaks--;
                                    game->addItem(Item(TACO, 1));
                                } else {
                                    conversationId = 6;
                                }
                            }
                            break;
                        }
                            
                        case 1:
                            ImGui::Text("Thanks! Now take your time to sleep here.\nWe've got all sorts of rooms!");
                            break;
                            
                        case 2:
                            ImGui::Text("Yeah, our villages is actually very secluded.\nYou need to cross a peach blossom forest to get here.");
                            break;
                            
                        case 3:
                            ImGui::Text("OK, chillax now...\n(You hear a soft click.)\n(You hear very loud siren.)");
                            break;
                            
                        case 4:
                            ImGui::Text("Oh yes he does! He never sells it though.\nHe just provides it for the hotel.");
                            break;
                            
                        case 5:
                            ImGui::Text("Here you go. Please eat it while it's hot.");
                            break;
                            
                        case 6:
                            ImGui::Text("Oh sorry. But there are no more tacos now... Come back later!");
                            break;
                    }
                    break;
            }
            
        case 5:
            switch (rampIndex) {
                case 0:
                case 21:
                    switch (conversationId) {
                        case 0:
                            ImGui::Text("Zzz...");
                            if (ImGui::Button("Mug him now!")) {
                                if (mugCounter < 0) {
                                    game->addItem(Item(COIN, -mugCounter));
                                    mugCounter = -mugCounter + 1;
                                    conversationId = 1;
                                } else {
                                    mugCounter = mugCounter * 2;
                                    conversationId = 2;
                                    texId = 12;
                                }
                            }
                            break;
                            
                        case 1:
                            ImGui::Text("Oh, no! I will give you all I have!\nPlease don't hurt me!\n(You get %d coins!)", (mugCounter - 1));
                            break;
                            
                        case 2:
                            ImGui::Text("(Oh feck, the old man came prepared this time!\nThis is actually a policeman under disguise!)\nThere you are! Prepare for JUSTICE!");
                            break;
                    }
                    break;
                    
                case 1:
                case 2:
                case 3:
                case 4:
                case 5:
                case 6:
                case 7:
                case 8:
                case 9:
                case 10:
                case 11:
                case 12:
                case 13:
                case 14:
                case 15:
                case 16:
                case 17:
                case 18:
                case 19:
                case 20:
                    switch (conversationId) {
                        case 0:
                            ImGui::Text("*The old man is talking nonsense.*");
                            if (ImGui::Button("Hey old man!")) {
                                conversationId = 1;
                            }
                            break;
                            
                        case 1:
                            ImGui::Text("The quick brown fox jumps over the lazy dog.");
                            if (ImGui::Button("Are you crazy?")) {
                                conversationId = 2;
                            }
                            break;
                            
                        case 2:
                            ImGui::Text("I used to be an adventure like you...");
                            if (ImGui::Button("Huh?")) {
                                conversationId = 3;
                            }
                            break;
                            
                        case 3:
                            ImGui::Text("In case of emergency, dial 01189998819991197253.");
                            if (ImGui::Button("Uh huh.")) {
                                conversationId = 4;
                            }
                            break;
                            
                        case 4:
                            ImGui::Text("Do you want to listen to my story?");
                            if (ImGui::Button("Finally, some sentences that makes sense!")) {
                                conversationId = 5;
                            }
                            if (ImGui::Button("Yes, please")) {
                                conversationId = 6;
                            }
                            if (ImGui::Button("Feck off, old man")) {
                                conversationId = 7;
                            }
                            break;
                            
                        case 5:
                            ImGui::Text("Don't sleep on my bed. It's my spot.");
                            if (ImGui::Button("Feck off, old man")) {
                                conversationId = 7;
                            }
                            break;
                            
                        case 6:
                        {
                            std::string msg = "Gimme a taco, and I will tell you.";
                            bool haveTaco = game->getQuantityOf(TACO) > 0;
                            if (!haveTaco) {
                                msg += "\n(You don't have a taco.)";
                            }
                            ImGui::Text("%s", msg.c_str());
                            if (haveTaco && ImGui::Button("Give him a taco.")) {
                                game->addItem(Item(TACO, -1));
                                conversationId = 8;
                            }
                            for (int i = 0; i < game->items.size(); i++) {
                                if (game->items[i].type == TACO) {
                                    continue;
                                }
                                if (ImGui::Button(std::string("Give him a " + game->items[i].getItemName() + " instead.").c_str())) {
                                    conversationId = 9;
                                    game->addItem(Item(game->items[i].type, -1));
                                    shove = Item(game->items[i].type, 1);
                                }
                            }
                            break;
                        }
                            
                        case 7:
                            ImGui::Text("Oh, you fecker!\nYou know what I do when I was young?");
                            break;
                            
                        case 8:
                            ImGui::Text("Well, actually I've got no stories.\nI just want to try a taco out.\nCouldn't pay to go to the hotel.\nThanks!");
                            if (ImGui::Button("Punch the old man!")) {
                                conversationId = 7;
                            }
                            break;
                            
                        case 9:
                            ImGui::Text("%s", std::string("I mean a TACO, not a " + shove.getItemName() + "!\n(The old man shoves the food back into your mouth.)").c_str());
                            shove.invoke(game);
                            break;
                    }
                    break;
            }
            break;
            
        case 6:
            switch (rampIndex) {
                case 0:
                    ImGui::Text("Hmm. Fish. Zzz.");
                    break;
                    
                case 1:
                    ImGui::Text("Hey there! How are you today?");
                    break;
                    
                case 2:
                    ImGui::Text("Going to do some fishin'.");
                    break;
                    
                case 3:
                {
                    if (game->rodDay == -1) {
                        switch (conversationId) {
                            case 0:
                                ImGui::Text("Hey! Do you want to learn fishin' too?");
                                if (ImGui::Button("Yeah!")) {
                                    conversationId = 1;
                                    game->rodDay = game->day;
                                    game->addItem(Item(ROD, 1));
                                }
                                if (ImGui::Button("Nope.")) {
                                    conversationId = 2;
                                }
                                break;

                            case 2:
                                ImGui::Text("Well, suit yourself.");
                                break;
                        }
                    } else if (game->rodDay != -1 && game->getQuantityOf(ROD) == 0) {
                        ImGui::Text("WHAT?! You ATE the fecking rod?\nGo away! I don't want to talk to you.");
                    } else if (game->rodDay != -1) {
                        switch (conversationId) {
                            case 1:
                                ImGui::Text("Here, take this rod.\nJust throw it in some sort of water.\nIf you can bring me a fish within a month,\nI will give you a reward!");
                                if (ImGui::Button("I don't think there's water inside this village.")) {
                                    conversationId = 2;
                                }
                                break;
                                
                            case 2:
                                ImGui::Text("Of course there is!\nYou just need to look very carefully.");
                                if (ImGui::Button("No, I seriously don't think there is.")) {
                                    conversationId = 2;
                                }
                                break;
                        }
                    }
                    break;
                }
            }
            break;
            
        default:
            break;
    }
    if (ImGui::Button("Try to leave") || destinationRamp != game->interactingMonsterRamp) {
        if ((id == 1 && conversationId == 1) ||
            (id == 2 && rampIndex == 0) ||
            (id == 4 && rampIndex == 0 && conversationId == 2)) {
            game->jail("You were found breaking into someone's house.");
        }
        if ((id == 0 && conversationId == 1) ||
            (id == 3 && conversationId == 3) ||
            (id == 5 && rampIndex >= 1 && rampIndex <= 20 && conversationId == 7)) {
            game->hospital("You got punched straight in the face and lose conciousness.");
        }
        if (id == 0 && conversationId == 2) {
            game->jail("You were found beating the chef up.");
        }
        if (id == 2 && (rampIndex == 1 || rampIndex == 9) && conversationId == 4) {
            game->jail("You were found beating the rich man up.");
            if (rampIndex == 9) {
                game->addItem(Item(PARTIALLY_EATEN_LAVISH_MEAL, 1));
            }
        }
        if (id == 2 && conversationId == 1) {
            game->jail("You were found lying.");
        }
        if ((id == 4 && rampIndex == 9 && conversationId == 3) ||
            (id == 5 && (rampIndex == 0 || rampIndex == 21) && conversationId == 2)) {
            game->jail("You were found mugging.");
        }
        texId = id;
        game->interactingMonster = nullptr;
    }
    ImGui::End();
}





