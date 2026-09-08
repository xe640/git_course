#include "shader.h"

Shader::Shader(const char* vertexShaderPath, const char* fragmentShaderPath){
    std::string vertexCode;
    std::string fragmentCode;
    std::ifstream vShaderFile;
    std::ifstream fShaderFile;
    // ensure ifstream objects can throw exceptions:
    vShaderFile.exceptions (std::ifstream::failbit | std::ifstream::badbit);
    fShaderFile.exceptions (std::ifstream::failbit | std::ifstream::badbit);
    try 
    {
        // open files
        vShaderFile.open(vertexShaderPath);
        fShaderFile.open(fragmentShaderPath);
        std::stringstream vShaderStream, fShaderStream;
        // read file's buffer contents into streams
        vShaderStream << vShaderFile.rdbuf();
        fShaderStream << fShaderFile.rdbuf();
        // close file handlers
        vShaderFile.close();
        fShaderFile.close();
        // convert stream into string
        vertexCode   = vShaderStream.str();
        fragmentCode = fShaderStream.str();
    }
    catch(std::ifstream::failure e)
    {
        std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
    }
    const char* vShaderCode = vertexCode.c_str();
    const char* fShaderCode = fragmentCode.c_str();

    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vShaderCode, NULL);
    glCompileShader(vertexShader);

    int  success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);

    if(!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fShaderCode, NULL);
    glCompileShader(fragmentShader);
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);

    if(!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    GLuint shaderProgram = glCreateProgram();

    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if(!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::_LINKING_FAILED\n" << infoLog << std::endl;
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    programID = shaderProgram;
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
