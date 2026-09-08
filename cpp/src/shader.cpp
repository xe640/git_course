#include <shader.h>

Shader::Shader(const char* vertexShaderPath, const char* fragmentShaderPath){

}

void Shader::use(){
    glUseProgram(programID);
}

void Shader::setIntUniform(const std::string name, int value) {
    if(uniform_cache.find(name) == uniform_cache.end()){
        uniform_cache[name] = glGetUniformLocation(programID, name.c_str());
    }
    glUniform1i(uniform_cache[name], value);
}

void Shader::setFloatUniform(const std::string name, float value) {
    if(uniform_cache.find(name) == uniform_cache.end()){
        uniform_cache[name] = glGetUniformLocation(programID, name.c_str());
    }
    glUniform1f(uniform_cache[name], value);
}

void Shader::setVec2Uniform(const std::string name, float x, float y) {
    if(uniform_cache.find(name) == uniform_cache.end()){
        uniform_cache[name] = glGetUniformLocation(programID, name.c_str());
    }
    glUniform2f(uniform_cache[name], x, y);
}

void Shader::setVec3Uniform(const std::string name, float x, float y, float z) {
    if(uniform_cache.find(name) == uniform_cache.end()){
        uniform_cache[name] = glGetUniformLocation(programID, name.c_str());
    }
    glUniform3f(uniform_cache[name], x, y, z);

}

void Shader::setVec4Uniform(const std::string name, float x, float y, float z, float w) {
    if(uniform_cache.find(name) == uniform_cache.end()){
        uniform_cache[name] = glGetUniformLocation(programID, name.c_str());
    }
    glUniform4f(uniform_cache[name], x, y, z, w);

}
