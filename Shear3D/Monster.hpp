//
//  Monster.hpp
//  Shear3D
//
//  Created by 周昊 on 2020/2/21.
//  Copyright © 2020 aiofwa. All rights reserved.
//

#ifndef Monster_hpp
#define Monster_hpp

#include <glm/glm.hpp>
#include <vector>
#include "Program.hpp"
#include "Texture.hpp"
#include "Object.hpp"
#include "Item.hpp"


class Game;

struct Ramp {
    float time;
    glm::vec3 destination;
};

class Monster {
public:
    Monster() {}
    Monster(Texture *tex, int id, glm::vec3 position, GLuint VAO);
    
    void update(float dt, float time, int day, std::vector<Object> &objects);
    void pathfind(glm::vec3 destination, std::vector<Object> &objects);
    
    Object *lookup(glm::vec3 pos, std::vector<Object> &objects);

    void render(Program &program);
    
    void interact(Game *game);

    std::vector<Ramp> ramps;
    std::vector<Ramp> festiveRamps;
    glm::vec3 position;
    glm::vec3 destination;
    float speed;
    GLuint VAO;
    Texture *texture;
    int id;
    std::vector<Object *> path;
    Ramp *destinationRamp;
    int rampIndex;
    int pathIndex;
    int conversationId;
    
    // Old man variables
    Item shove;
    int mugCounter;
    int texId;
    
    // Beggar variables
    bool begging;
};

#endif /* Monster_hpp */
