#pragma once

#include <memory>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <PointGrid.hpp>
#include <Shader.hpp>
#include <OrbitalCamera.hpp>
#include <Model.hpp>

namespace Config {
    constexpr int WINDOW_WIDTH = 1600;
    constexpr int WINDOW_HEIGHT = 1600;
    const std::string SHADER_PATH = "../shaders/";
    const std::string IMAGES_PATH = "../images/";
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

        // Objects
        std::unique_ptr<PointGrid> m_pointGrid;
        std::unique_ptr<Model> m_loadedModel;
        std::unique_ptr<OrbitalCamera> m_camera;

        // Shaders
        std::unique_ptr<Shader> m_baseShader;

        // Matrices
        glm::mat4 m_viewMatrix;
        glm::mat4 m_projectionMatrix;

        // App setup
        void initGLFW();
        void initOpenGL();
        void initShaders();
        void initImGui();
        void initObjects();

        // Rendering
        void renderUI();
        void render();
        void cleanup();
        
        // GLFW callbacks
        static void mouseButtonCallback(GLFWwindow* window, int button, int action, [[maybe_unused]] int mods);
        static void scrollCallback(GLFWwindow* window, double xoffset, double yoffset);
        static void cursorPosCallback(GLFWwindow* window, double xpos, double ypos);
        //static void keyCallback([[maybe_unused]] GLFWwindow* window, int key, int scancode, int action, int mods);
};