#include <Shader.hpp>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <glad/glad.h>

Shader::Shader() {}

Shader::Shader(const std::string& vertexPath, const std::string& fragmentPath) {
    initFromPaths(vertexPath.c_str(), fragmentPath.c_str());
}

Shader::Shader(const char* vertexPath, const char* fragmentPath) {
    initFromPaths(vertexPath, fragmentPath);
}

void Shader::initFromPaths(const char* vertexPath, const char* fragmentPath) {
    // Reading shaders
    std::string vertexCode;
    std::string fragmentCode;
    std::ifstream vertexShaderFile;
    std::ifstream fragmentShaderFile;
    vertexShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    fragmentShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    try {
        vertexShaderFile.open(vertexPath);
        fragmentShaderFile.open(fragmentPath);
        std::stringstream vertex_shader_stream;
        std::stringstream fragment_shader_stream;
        vertex_shader_stream << vertexShaderFile.rdbuf();
        fragment_shader_stream << fragmentShaderFile.rdbuf();
        vertexShaderFile.close();
        fragmentShaderFile.close();
        vertexCode = vertex_shader_stream.str();
        fragmentCode = fragment_shader_stream.str();
    }
    catch(std::ifstream::failure &e) {
        std::cout<<"ERROR::SHADER::"<<vertexPath<<" OR "<<fragmentPath<<"::FILE_NOT_SUCCESFULLY READ"<<std::endl;
    }
    const char* vertex_shader_code = vertexCode.c_str();
    const char* fragment_shader_code = fragmentCode.c_str();
    
    // Shaders compilation
    unsigned int vertex;
    unsigned int fragment;
    int success;
    char infoLog[512];

    // Vertex shader
    vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vertex_shader_code, NULL);
    glCompileShader(vertex);
    glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
    if(!success) {
        glGetShaderInfoLog(vertex, 512, NULL, infoLog);
        std::cout<<"ERROR::SHADER::VERTEX::"<<vertexPath<<"::COMPILATION_FAILED \n{}"<<infoLog<<std::endl;
    }
    else {
        std::cout<<"SHADER::VERTEX::"<<vertexPath<<"::COMPILATION_COMPLETED"<<std::endl;
    }

    // Fragment shader
    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fragment_shader_code, NULL);
    glCompileShader(fragment);
    glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
    if(!success) {
        glGetShaderInfoLog(fragment, 512, NULL, infoLog);

        std::cout<<"ERROR::SHADER::FRAGMENT::"<<fragmentPath<<"::COMPILATION_FAILED \n{}"<<infoLog<<std::endl;
    }
    else {
        std::cout<<"SHADER::FRAGMENT::"<<fragmentPath<<"::COMPILATION_COMPLETED"<<std::endl;
    }

    // Shader program
    m_id = glCreateProgram();
    glAttachShader(m_id, vertex);
    glAttachShader(m_id, fragment);
    glLinkProgram(m_id);
    glGetProgramiv(m_id, GL_LINK_STATUS, &success);
    if(!success) {
        glGetProgramInfoLog(m_id, 512, NULL, infoLog);
        std::cout<<"ERROR::SHADER::PROGRAM::FROM::"<<vertexPath<<" OR "<<fragmentPath<<"::LINKING_FAILED\n{}"<<infoLog<<std::endl;
    }
    else {
        std::cout<<"SHADER::PROGRAM::FROM::"<<vertexPath<<" AND "<<fragmentPath<<"::LINKING_COMPLETED"<<std::endl;
    }
    initialized = true;
    glDeleteShader(vertex);
    glDeleteShader(fragment);
    std::cout<<"==========================================="<<std::endl;
}

Shader::~Shader() {
    if(initialized) {
        std::cout<<"Deleting shader program"<<std::endl;
        glDeleteProgram(m_id);
        initialized = false;
    }
}

void Shader::use() {
    if(initialized) {
        glUseProgram(m_id);
    }
}

void Shader::setVec2(const char* name, const float x, const float y) {
    glUniform2f(glGetUniformLocation(m_id, name), x, y);
}

void Shader::setVec2(const char* name, const glm::vec2 v) {
    glUniform2f(glGetUniformLocation(m_id, name), v.x, v.y);
}

void Shader::setVec3(const char* name, const glm::vec3 v) {
    glUniform3f(glad_glGetUniformLocation(m_id, name), v.x, v.y, v.z);
}

void Shader::setVec4(const char* name, const glm::vec4 v) {
    glUniform4f(glGetUniformLocation(m_id, name), v.x, v.y, v.z, v.w);
}

void Shader::setTexture2D(const char* name, const GLint textureUnit) {
    glUniform1i(glGetUniformLocation(m_id, name), textureUnit);
}

void Shader::setInt(const char* name, const int val) {
    glUniform1i(glGetUniformLocation(m_id, name), val);
}

void Shader::setFloat(const char* name, const float val) {
    glUniform1f(glGetUniformLocation(m_id, name), val);
}

void Shader::setBool(const char* name, const bool val) {
    glUniform1i(glGetUniformLocation(m_id, name), val);
}

void Shader::setMat4(const char* name, const glm::mat4& mat) {
    glUniformMatrix4fv(glGetUniformLocation(m_id, name), 1, GL_FALSE, &mat[0][0]);
}