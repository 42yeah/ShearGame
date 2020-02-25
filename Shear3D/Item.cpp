//
//  Item.cpp
//  Shear3D
//
//  Created by 周昊 on 2020/2/25.
//  Copyright © 2020 aiofwa. All rights reserved.
//

#include "Item.hpp"
#include "Game.hpp"


Item::Item(ItemType type, int quantity) : type(type), quantity(quantity) {
}

std::string Item::getItemName(bool includesQuantity) {
    std::string ret;
    switch (type) {
        case EGG:
            ret = "egg";
            break;
            
        case STEAK:
            ret = "steak";
            break;
            
        case COIN:
            ret = "coin";
            break;
            
        case FISH:
            ret = "fish";
            break;
            
        case URANIUM_BAR:
            ret = "uranium bar";
            break;
            
        case RADIOACTIVE_FISH:
            ret = "radioactive fish";
            break;
    }
    if (includesQuantity) {
        ret += " x" + std::to_string(quantity);
    }
    return ret;
}

void Item::invoke(Game *game) {
    std::random_device dev;
    std::uniform_int_distribution<> distrib;
    int outcome;
    std::string msg;
    switch (type) {
        case STEAK:
            quantity--;
            distrib = std::uniform_int_distribution<>(1, 4);
            outcome = distrib(dev);
            switch (outcome) {
                case 1:
                    msg = "You ate the " + getItemName() + ". It was burnt, and extremely salty.";
                    game->hunger += 0.5f;
                    game->stamina -= 0.15f;
                    break;
                    
                case 2:
                    msg = "You ate the " + getItemName() + ". It was really hard to bite.";
                    game->hunger += 0.85f;
                    game->stamina -= 0.15f;
                    break;
                    
                case 3:
                    msg = "You ate the " + getItemName() + ".";
                    game->hunger += 1.0f;
                    break;
                    
                case 4:
                    msg = "You ate the " + getItemName() + ". It is delicious!";
                    game->hunger += 1.5f;
                    game->stamina += 0.1f;
                    break;
            }
            if (game->state != SITTING) {
                msg += "\nHowever, you ate without chair. That's uncomfortable.";
                game->stamina -= 0.2f;
                game->hunger -= 0.15f;
            }
            game->notifications.push_back(Notification("Itme used", msg.c_str(), true, 10.0f));
            break;
            
        case EGG:
            quantity--;
            game->hunger += 0.1f;
            game->stamina -= 0.01f;
            game->notifications.push_back(Notification("Itme used", "You ate the egg. It is raw, and you felt like you drank an eggshot.", true, 10.0f));
            break;
            
        case COIN:
            quantity--;
            distrib = std::uniform_int_distribution<>(1, 2);
            outcome = distrib(dev);
            game->stamina -= 1.0f;
            switch (outcome) {
                case 1:
                    msg = "You swallowed the coin. It was too big, and it came back.\nYou puke on the ground.";
                    game->hunger -= 1.0f;
                    break;
                    
                case 2:
                    msg = "You swallowed the coin. You are having a hard time swallowing it because the coin is big.";
                    break;
            }
            game->notifications.push_back(Notification("Itme used", msg.c_str(), true, 10.0f));
            break;
    }
    
}
