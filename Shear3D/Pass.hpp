//
//  Pass.hpp
//  Shear3D
//
//  Created by 周昊 on 2020/2/20.
//  Copyright © 2020 aiofwa. All rights reserved.
//

#ifndef Pass_hpp
#define Pass_hpp

#include "../Includes/glad/glad.h"
#include <glm/glm.hpp>
#include "Program.hpp"


enum PassType {
    HDR, SHADOW
};

class Pass {
public:
    Pass() {}
    Pass(glm::ivec2 size, PassType passType = HDR);
    
    void pass(GLuint loc, int tex);
    void use();
    void unuse();
    
private:
    GLuint fbo, rbo;
    GLuint texture;
    PassType type;
};

#endif /* Pass_hpp */
