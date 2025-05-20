#include <stdexcept>
#include <memory>
#include <iostream>
#include <Application.hpp>
#include <glm/ext/matrix_clip_space.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

Application::Application() {
    initGLFW();
    initOpenGL();
    initShaders();
    initImGui();
    initObjects();
}

Application::~Application() {
    cleanup();
}

void Application::run() {
    while (!glfwWindowShouldClose(m_window.get())) {
        render();
        glfwSwapBuffers(m_window.get());
        glfwPollEvents();
    }
}

void Application::initGLFW() {
    if (!glfwInit()) {
        throw std::runtime_error("Failed to initialize GLFW");
    }
    #ifdef __linux__
        glfwWindowHintString(GLFW_WAYLAND_APP_ID, "FloatingApp");
    #endif
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    m_window = GLFWwindowPtr(glfwCreateWindow(Config::WINDOW_WIDTH, Config::WINDOW_HEIGHT, "PointGrid", nullptr, nullptr), glfwDestroyWindow);
    if (!m_window) {
        glfwTerminate();
        throw std::runtime_error("Failed to create GLFW window");
    }
    glfwMakeContextCurrent(m_window.get());
    glfwSetWindowUserPointer(m_window.get(), this);
    glfwSetMouseButtonCallback(m_window.get(), mouseButtonCallback);
    glfwSetCursorPosCallback(m_window.get(), cursorPosCallback);
    glfwSetScrollCallback(m_window.get(), scrollCallback);
    glfwSetKeyCallback(m_window.get(), keyCallback);
}

void Application::initOpenGL() {
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        throw std::runtime_error("Failed to initialize GLAD");
    }
    glEnable(GL_DEPTH_TEST);
    glViewport(0, 0, Config::WINDOW_WIDTH, Config::WINDOW_HEIGHT);
    glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
}

void Application::initShaders() {
    m_baseShader = std::make_unique<Shader>(Config::SHADER_PATH + "kelvinlets.vert", Config::SHADER_PATH + "base.frag");
    m_projectionMatrix = glm::perspective(glm::radians(45.0f), (float)Config::WINDOW_WIDTH / (float)Config::WINDOW_HEIGHT, 0.1f, 1000.0f);
    m_baseShader->setMat4("u_projectionMatrix", m_projectionMatrix);
}

void Application::initImGui() {
    ImGui::CreateContext();
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(m_window.get(), true);
    ImGui_ImplOpenGL3_Init("#version 330");
}

void Application::initObjects() {
    m_pointGrid = std::make_unique<PointGrid>();
    m_loadedModel = std::make_unique<Model>(Config::MODELS_PATH + "capsule/capsule.gltf");
    m_camera = std::make_unique<OrbitalCamera>();
    m_kelvinlet = std::make_unique<Kelvinlet>();
}

void Application::renderUI() {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    ImGui::Begin("Kelvinlets app");
    ImGui::End();

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void Application::sendKelvinletToShader() {
    m_kelvinletsShader->use();
    m_kelvinletsShader->setFloat("kelvinlet.brush.epsilon", m_kelvinlet->m_brush.epsilon);
    m_kelvinletsShader->setFloat("kelvinlet.brush.f", m_kelvinlet->m_brush.f);
    m_kelvinletsShader->setFloat("kelvinlet.a", m_kelvinlet->m_a);
    m_kelvinletsShader->setFloat("kelvinlet.b", m_kelvinlet->m_b);
    m_kelvinletsShader->setVec3("x0", glm::vec3(0.0f));
}

void Application::render() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    m_baseShader->use();
    m_viewMatrix = m_camera->getViewMatrix();
    m_baseShader->setMat4("u_viewMatrix", m_viewMatrix);
    m_baseShader->setMat4("u_projectionMatrix", m_projectionMatrix);
    m_baseShader->setFloat("kelvinlet.brush.epsilon", m_kelvinlet->m_brush.epsilon);
    m_baseShader->setFloat("kelvinlet.brush.f", m_kelvinlet->m_brush.f);
    m_baseShader->setFloat("kelvinlet.a", m_kelvinlet->m_a);
    m_baseShader->setFloat("kelvinlet.b", m_kelvinlet->m_b);
    m_baseShader->setVec3("x0", glm::vec3(0.0f));
    m_pointGrid->drawGrid();
    m_loadedModel->draw();
    renderUI();
}

void Application::cleanup() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    // No need for glfwDestroyWindow (using custom deleter with smart ptr)
    glfwTerminate();
}

void Application::mouseButtonCallback(GLFWwindow* window, int button, int action, [[maybe_unused]] int mods) {
    auto app = static_cast<Application*>(glfwGetWindowUserPointer(window));
    if(button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS && !ImGui::GetIO().WantCaptureMouse) {
        app->m_camera->m_isDragging = true;
        glfwGetCursorPos(window, &app->m_camera->m_lastX, &app->m_camera->m_lastY);
    }
    if(button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE) {
        app->m_camera->m_isDragging = false;
    }
    if(button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS && !ImGui::GetIO().WantCaptureMouse) {
        app->m_camera->m_isPanning = true;
        glfwGetCursorPos(window, &app->m_camera->m_lastX, &app->m_camera->m_lastY);
    }
    if(button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_RELEASE) {
        app->m_camera->m_isPanning = false;
    }
}

void Application::cursorPosCallback(GLFWwindow* window, double xpos, double ypos) {
    auto app = static_cast<Application*>(glfwGetWindowUserPointer(window));
    app->m_camera->processDrag(xpos, ypos);
    app->m_camera->processPan(xpos, ypos);
}

void Application::scrollCallback(GLFWwindow* window, [[maybe_unused]] double xoffset, double yoffset) {
    auto app = static_cast<Application*>(glfwGetWindowUserPointer(window));
    if(!ImGui::GetIO().WantCaptureMouse) {
        app->m_camera->processScroll(yoffset);
    }
}

void Application::keyCallback(GLFWwindow* window, int key, [[maybe_unused]] int scancode, int action, [[maybe_unused]] int mods) {
    auto app = static_cast<Application*>(glfwGetWindowUserPointer(window));
    if(key == GLFW_KEY_E && action == GLFW_PRESS) {
        app->m_camera->m_hasMouse = !app->m_camera->m_hasMouse;
    }
}