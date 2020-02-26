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
            
        case PARTIALLY_EATEN_LAVISH_MEAL:
            ret = "partially eaten lavish meal";
            break;
            
        case AXE:
            ret = "axe";
            break;
            
        case TACO:
            ret = "taco";
            break;
            
        case ROTTEN_TACO:
            ret = "rotten taco";
            break;
            
        case LOG:
            ret = "log";
            break;
            
        case ROD:
            ret = "fishing rod";
            break;
            
        case GOLDEN_FISHING_TROPHY:
            ret = "+golden fishing trophy+";
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
            
        case PARTIALLY_EATEN_LAVISH_MEAL:
            quantity--;
            game->hunger += 1.5f;
            game->stamina -= 1.0f;
            game->notifications.push_back(Notification("Item used", "You ate the partially eaten lavish meal.\nMan, is this good! Even with all those saliva around.\nYou swallowed it down in disgust.", true, 10.0f));
            break;
            
        case AXE:
            quantity--;
            game->stamina -= 10.0f;
            game->notifications.push_back(Notification("Item used", "You ate the axe. IT HURTS LIKE HELL!\nOh no! The axe head is poking out of your stomach!", true, 10.0f));
            break;
            
        case TACO:
            quantity--;
            game->hunger += 1.0f;
            game->stamina += 0.5f;
            msg = "You ate the taco. It's pretty standard.";
            if (game->state != SITTING) {
                msg += "\nHowever, you ate without chair. That's VERY uncomfortable.";
                game->stamina -= 0.8f;
                game->hunger -= 0.3f;
            }
            game->notifications.push_back(Notification("Item used", msg, true, 10.0f));
            break;
            
        case ROTTEN_TACO:
            quantity--;
            game->hunger += 0.03f;
            game->stamina -= 3.0f;
            msg = "You ate the rotten taco.\nYou feel a strong urge to throw up.";
            if (game->state == SITTING) {
                msg += "\nBut you managed to hold them back.";
            } else {
                game->hunger -= 2.0f;
                msg += "\nYou threw up big time!";
            }
            game->notifications.push_back(Notification("Item used", msg, true, 10.0f));
            break;
            
        case LOG:
            quantity--;
            game->hunger += 0.01f;
            game->stamina -= 3.5f;
            game->additiveTime += 20.0f;
            msg = "You ate the log. Chewing on it is extremely hard, and it cost you much time.";
            game->notifications.push_back(Notification("Item used", msg, true, 10.0f));
            break;
            
        case ROD:
            quantity--;
            game->hunger += 0.5f;
            game->stamina -= 2.5f;
            msg = "You ate the fishing rod. It was long, and it latched into your throat.";
            game->notifications.push_back(Notification("Item used", msg, true, 10.0f));
            game->latched = true;
            break;
            
        case GOLDEN_FISHING_TROPHY:
            quantity--;
            game->hunger += 0.5f;
            game->stamina -= 4.0f;
            msg = "You ate the golden fishing trophy. It's too big! You can't swallow! Feck!";
            game->notifications.push_back(Notification("Item used", msg, true, 10.0f));
            game->goldenLatched = 1000;
            break;
    }
    if (game->latched) {
        distrib = std::uniform_int_distribution<>(1, 2);
        outcome = distrib(dev);
        switch (outcome) {
            case 2:
                game->hunger -= 2.0f;
                game->notifications.push_back(Notification("The latching rod", "The latching fishing rod makes food come back!\nYou throw up.", true, 10.0f));
                break;
                
            default:
                break;
        }
    }
    if (game->goldenLatched > 0 && type != GOLDEN_FISHING_TROPHY) {
        distrib = std::uniform_int_distribution<>(0, game->goldenLatched);
        outcome = distrib(dev);
        if (outcome > 0) {
            game->hunger -= 1.0f;
            game->notifications.push_back(Notification("The latching golden trophy", "You gag and vomit.\nSome golden stuffs mixed with vomits came back up.\nApparently, your body cannot digest gold.\nAt least you can treat them like coins!", true, 10.0f));
            game->goldenLatched -= outcome;
            game->addItem(Item(COIN, outcome));
        }
    }
}
