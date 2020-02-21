//
//  Texture.hpp
//  Shear3D
//
//  Created by 周昊 on 2020/2/20.
//  Copyright © 2020 aiofwa. All rights reserved.
//

#ifndef Texture_hpp
#define Texture_hpp

#include <iostream>
#include "../Includes/glad/glad.h"


enum TextureInternalFormat {
    RGB, RGBA
};

class Texture {
public:
    Texture() {}
    Texture(std::string path, TextureInternalFormat format = RGB);
    
    void pass(GLuint loc, int tex);

    GLuint tex;
    int width, height;
};

#endif /* Texture_hpp */
