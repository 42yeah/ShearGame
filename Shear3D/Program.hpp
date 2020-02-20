//
//  Program.hpp
//  Shear3D
//
//  Created by 周昊 on 2020/2/20.
//  Copyright © 2020 aiofwa. All rights reserved.
//

#ifndef Program_hpp
#define Program_hpp

#include "../Includes/glad/glad.h"
#include <string>
#include <map>


class Program {
public:
    Program() {}
    Program(std::string vPath, std::string fPath);
    
    void link(std::string vPath, std::string fPath, bool report = true);
    void report();
    void use();
    void reload();
    
    GLuint loc(std::string uniformName);

private:
    GLuint compile(GLuint type, std::string path);
    
    void setLog(GLuint type, std::string log);

    std::string vertexPath, fragmentPath;
    std::string vertexLog, fragmentLog, programLog;
    
    // Lazy init the locations
    std::map<std::string, GLuint> locationCache;
    GLuint program;
};

#endif /* Program_hpp */
