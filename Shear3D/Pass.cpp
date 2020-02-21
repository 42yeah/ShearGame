//
//  Pass.cpp
//  Shear3D
//
//  Created by 周昊 on 2020/2/20.
//  Copyright © 2020 aiofwa. All rights reserved.
//

#include "Pass.hpp"


Pass::Pass(glm::ivec2 size, PassType passType) {
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    switch (passType) {
        case HDR:
            glGenRenderbuffers(1, &rbo);
            glBindRenderbuffer(GL_RENDERBUFFER, rbo);
            glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, size.x, size.y);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, size.x, size.y, 0, GL_RGB, GL_FLOAT, nullptr);
            break;
            
        case SHADOW:
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, size.x, size.y, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
            break;
    }
    
    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    switch (passType) {
        case HDR:
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);
            glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);
            break;
            
        case SHADOW:
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, texture, 0);
            glDrawBuffer(GL_NONE);
            glReadBuffer(GL_NONE);
            break;
    }
    
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
}

void Pass::pass(GLuint loc, int tex) {
    glActiveTexture(GL_TEXTURE0 + tex);
    glBindTexture(GL_TEXTURE_2D, texture);
    glUniform1i(loc, tex);
}

void Pass::use() { 
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
}

void Pass::unuse() { 
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
