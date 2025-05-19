#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <memory>
#include <vector>
#include <Shader.hpp>
#include <Material.hpp>

struct Vertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec3 tangent;
    glm::vec3 bitangent;
    glm::vec2 uv;
};

class Mesh {
    public:
        // Public attributes
        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;
        std::shared_ptr<Material> material = nullptr;
        std::shared_ptr<Shader> shader;
        
        // Constructors
        Mesh() {}
        Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::shared_ptr<Material> material) : vertices(std::move(vertices)), indices(std::move(indices)), material(material) {
            setup_mesh();
        }

        // Destructor
        ~Mesh() {
            glDeleteBuffers(1, &vbo);
            glDeleteVertexArrays(1, &vao);
        }

        void bind_shader(std::shared_ptr<Shader> shader);
        void bind_shader(const GLchar* vertex_path, const GLchar* fragment_path);
        void unbind_shader();
        void draw();
        void setup_mesh();
        void bindVAO();
        void drawElements();
        void add_texture(std::shared_ptr<Texture> texture);
        glm::vec3 getVerticeFromIndice(unsigned int indice);
    
    private:
        // Private attributes
        GLuint vao,vbo,ebo;
};