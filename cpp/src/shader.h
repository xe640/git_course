#ifndef SHADER_H
#define SHADER_H

#include <glad/glad.h> // include glad to get all the required OpenGL headers
  
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <unordered_map>

class Shader {
public : 
    GLuint programID;

    Shader(const char* vertexShaderPath, const char* fragmentShaderPath);

    void use();

    void setIntUniform(const std::string name, int value);
    void setFloatUniform(const std::string name, float value);
    void setVec2Uniform(const std::string name, float x, float y);
    void setVec3Uniform(const std::string name, float x, float y, float z);
    void setVec4Uniform(const std::string name, float x, float y, float z, float w);

private:
    std::unordered_map<std::string, GLint> uniform_cache;
};

#endif