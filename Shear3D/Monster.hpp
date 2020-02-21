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


struct Ramp {
    float time;
    glm::vec3 destination;
};

class Monster {
public:
    Monster() {}
    Monster(Texture *tex, int id, glm::vec3 position, GLuint VAO);
    
    void update(float dt, float time, int day);

    void render(Program &program);

    std::vector<Ramp> ramps;
    glm::vec3 position;
    glm::vec3 destination;
    float speed;
    GLuint VAO;
    Texture *texture;
    int id;
};

#endif /* Monster_hpp */
