#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <string>

class Shader {
    public:
        Shader();
        Shader(const std::string& vertexPath, const std::string& fragmentPath);
        Shader(const char* vertexPath, const char* fragmentPath);
        ~Shader();
        void use();
        void initFromPaths(const char* vertexPath, const char* fragmentPath);
        void setVec2(const char* name, const float x, const float y);
        void setVec2(const char* name, const glm::vec2 v);
        void setVec3(const char* name, const glm::vec3 v);
        void setVec4(const char* name, const glm::vec4 v);
        void setTexture2D(const char* name, const GLint textureUnit);
        void setInt(const char* name, const int val);
        void setFloat(const char* name, const float val);
        void setBool(const char* name, const bool val);
        void setMat4(const char* name, const glm::mat4& mat);

    private:
        GLuint m_id = 0;
        bool initialized = false;
};