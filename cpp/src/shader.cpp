#include "shader.h"

Shader::Shader(const char* vertexShaderPath, const char* fragmentShaderPath){
    program_id = 0;
    vertex_shader_path = vertexShaderPath;
    fragment_shader_path = fragmentShaderPath;
    reload();
}

void Shader::reload(){
    if(program_id != 0) {
        glDeleteProgram(program_id);
    }

    std::string vertexCode;
    std::string fragmentCode;
    std::ifstream vShaderFile;
    std::ifstream fShaderFile;
    has_compilation_succeeded = true;
    // ensure ifstream objects can throw exceptions:
    vShaderFile.exceptions (std::ifstream::failbit | std::ifstream::badbit);
    fShaderFile.exceptions (std::ifstream::failbit | std::ifstream::badbit);
    try 
    {
        // open files
        vShaderFile.open(vertex_shader_path);
        fShaderFile.open(fragment_shader_path);
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
        has_compilation_succeeded = false;
        return;
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
        has_compilation_succeeded = false;
    }

    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fShaderCode, NULL);
    glCompileShader(fragmentShader);
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);

    if(!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
        has_compilation_succeeded = false;
    }

    GLuint shaderProgram = glCreateProgram();

    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if(!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::_LINKING_FAILED\n" << infoLog << std::endl;
        has_compilation_succeeded = false;
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    program_id = shaderProgram;
}

void Shader::use(){
    if (has_compilation_succeeded) {
        glUseProgram(program_id);
    }
}

void Shader::setIntUniform(const std::string name, int value) {
    if (!has_compilation_succeeded) {
        return;
    }

    if(uniform_cache.find(name) == uniform_cache.end()){
        GLint location = glGetUniformLocation(program_id, name.c_str());
        if (location == -1) {
            std::cout << "uniform " << name.c_str() << " not found!" << std::endl;
        }
        uniform_cache[name] = location;
    }
    glUniform1i(uniform_cache[name], value);
}

void Shader::setFloatUniform(const std::string name, float value) {
    if (!has_compilation_succeeded) {
        return;
    }

    if(uniform_cache.find(name) == uniform_cache.end()){
        GLint location = glGetUniformLocation(program_id, name.c_str());
        if (location == -1) {
            std::cout << "uniform " << name.c_str() << " not found!" << std::endl;
        }
        uniform_cache[name] = location;
    }
    glUniform1f(uniform_cache[name], value);
}

void Shader::setVec2Uniform(const std::string name, float x, float y) {
    if (!has_compilation_succeeded) {
        return;
    }

    if(uniform_cache.find(name) == uniform_cache.end()){
        GLint location = glGetUniformLocation(program_id, name.c_str());
        if (location == -1) {
            std::cout << "uniform " << name.c_str() << " not found!" << std::endl;
        }
        uniform_cache[name] = location;
    }
    glUniform2f(uniform_cache[name], x, y);
}

void Shader::setVec3Uniform(const std::string name, float x, float y, float z) {
    if (!has_compilation_succeeded) {
        return;
    }

    if(uniform_cache.find(name) == uniform_cache.end()){
        GLint location = glGetUniformLocation(program_id, name.c_str());
        if (location == -1) {
            std::cout << "uniform " << name.c_str() << " not found!" << std::endl;
        }
        uniform_cache[name] = location;
    }
    glUniform3f(uniform_cache[name], x, y, z);

}

void Shader::setVec4Uniform(const std::string name, float x, float y, float z, float w) {
    if (!has_compilation_succeeded) {
        return;
    }

    if(uniform_cache.find(name) == uniform_cache.end()){
        GLint location = glGetUniformLocation(program_id, name.c_str());
        if (location == -1) {
            std::cout << "uniform " << name.c_str() << " not found!" << std::endl;
        }
        uniform_cache[name] = location;
    }
    glUniform4f(uniform_cache[name], x, y, z, w);

}

void Shader::setMat4Uniform(const std::string name, glm::mat4 value) {
    if (!has_compilation_succeeded) {
        return;
    }

    if(uniform_cache.find(name) == uniform_cache.end()){
        GLint location = glGetUniformLocation(program_id, name.c_str());
        if (location == -1) {
            std::cout << "uniform " << name.c_str() << " not found!" << std::endl;
        }
        uniform_cache[name] = location;
    }
    glUniformMatrix4fv(uniform_cache[name], 1, false, glm::value_ptr(value));

}

bool Shader::CompilationSucceeded(){
    return has_compilation_succeeded;
}

GLuint Shader::programID(){
    return program_id;
}