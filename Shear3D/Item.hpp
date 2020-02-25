//
//  Item.hpp
//  Shear3D
//
//  Created by 周昊 on 2020/2/25.
//  Copyright © 2020 aiofwa. All rights reserved.
//

#ifndef Item_hpp
#define Item_hpp

#include <iostream>
#include <random>


class Game;

enum ItemType {
    STEAK, EGG, COIN, FISH, URANIUM_BAR, RADIOACTIVE_FISH
};

class Item {
public:
    Item() {}
    Item(ItemType type, int quantity);

    std::string getItemName(bool includesQuantity = false);
    
    void invoke(Game *game);
    
    ItemType type;
    int quantity;
};

#endif /* Item_hpp */
