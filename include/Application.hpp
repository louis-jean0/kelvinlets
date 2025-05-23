#pragma once

#include <memory>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <PointGrid.hpp>
#include <Shader.hpp>
#include <OrbitalCamera.hpp>
#include <Model.hpp>
#include <Kelvinlet.hpp>
#include <Ray.hpp>

namespace Config {
    constexpr int WINDOW_WIDTH = 800;
    constexpr int WINDOW_HEIGHT = 800;
    const std::string SHADER_PATH = "shaders/";
    const std::string MODELS_PATH = "data/models/";
};

class Application {
    public:
        Application();
        ~Application();
        void run();

    private:
        // Window and states
        using GLFWwindowPtr = std::unique_ptr<GLFWwindow, void(*)(GLFWwindow*)>;        
        GLFWwindowPtr m_window = GLFWwindowPtr(nullptr, glfwDestroyWindow);
        bool m_wireframe = false;

        // Objects
        std::unique_ptr<PointGrid> m_pointGrid;
        std::unique_ptr<Model> m_loadedModel;
        std::unique_ptr<OrbitalCamera> m_camera;
        std::unique_ptr<Kelvinlet> m_kelvinlet;
        std::unique_ptr<Ray> m_ray;

        // Shaders
        std::unique_ptr<Shader> m_baseShader;
        std::unique_ptr<Shader> m_kelvinletsShader;
        std::unique_ptr<Shader> m_lineShader;

        // Matrices
        glm::mat4 m_viewMatrix;
        glm::mat4 m_projectionMatrix;

        // App setup
        void initGLFW();
        void initOpenGL();
        void initShaders();
        void initImGui();
        void initObjects();

        // Specific
        glm::vec3 m_lastRayStart;
        glm::vec3 m_lastRayEnd;
        bool m_hasRayToDraw = false;
        glm::vec3 screenPosToWorldRayDir(float mouseX, float mouseY);
        bool rayIntersectsTriangle(const glm::vec3& rayOrigin, const glm::vec3& rayDirection, const glm::vec3& v0, const glm::vec3& v1, const glm::vec3& v2, float& outT);
        glm::vec3 getRaycastHitPosition(float mouseX, float mouseY, const glm::vec3& rayOrigin);

        // Rendering
        void sendKelvinletToShader();
        void renderUI();
        void render();
        void cleanup();
        
        // GLFW callbacks
        static void mouseButtonCallback(GLFWwindow* window, int button, int action, [[maybe_unused]] int mods);
        static void scrollCallback(GLFWwindow* window, double xoffset, double yoffset);
        static void cursorPosCallback(GLFWwindow* window, double xpos, double ypos);
        static void keyCallback(GLFWwindow* window, int key, [[maybe_unused]] int scancode, int action, [[maybe_unused]] int mods);
};