#ifndef SHADER_H
#define SHADER_H

#include "../include/glad/glad.h"
  
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <unordered_map>

class Shader {
public : 
    Shader(const char* vertexShaderPath, const char* fragmentShaderPath);

    void reload();
    void use();

    void setIntUniform(const std::string name, int value);
    void setFloatUniform(const std::string name, float value);
    void setVec2Uniform(const std::string name, float x, float y);
    void setVec3Uniform(const std::string name, float x, float y, float z);
    void setVec4Uniform(const std::string name, float x, float y, float z, float w);
    bool CompilationSucceeded();
    GLuint programID();

private:
    std::unordered_map<std::string, GLint> uniform_cache;
    bool has_compilation_succeeded;
    const char* vertex_shader_path;
    const char* fragment_shader_path;
    GLuint program_id;
};

#endif