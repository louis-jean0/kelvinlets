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
    m_projectionMatrix = glm::perspective(glm::radians(45.0f), (float)Config::WINDOW_WIDTH / (float)Config::WINDOW_HEIGHT, 0.1f, 1000.0f);
    m_baseShader = std::make_unique<Shader>(Config::SHADER_PATH + "kelvinlets.vert", Config::SHADER_PATH + "base.frag");
    m_lineShader = std::make_unique<Shader>(Config::SHADER_PATH + "line.vert", Config::SHADER_PATH + "line.frag");
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
    m_ray = std::make_unique<Ray>();
}

void Application::renderUI() {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    ImGui::Begin("Kelvinlets app");
    ImGui::Checkbox("Display ray picking", &m_hasRayToDraw);
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
    //m_pointGrid->drawGrid();
    m_loadedModel->draw();
    if (m_hasRayToDraw) {
        m_lineShader->use();
        m_lineShader->setMat4("u_viewMatrix", m_viewMatrix);
        m_lineShader->setMat4("u_projectionMatrix", m_projectionMatrix);
        m_ray->updateRay();
        m_ray->drawRay();
    }
    renderUI();
}

glm::vec3 Application::screenPosToWorldRayDir(float mouseX, float mouseY) {
    // Clip space
    float x  = ((2.0f * mouseX) / Config::WINDOW_WIDTH) - 1.0f;
    float y = 1.0f - ((2.0f * mouseY) / Config::WINDOW_HEIGHT);
    float z = 1.0f;
    glm::vec3 rayNdc = glm::vec3(x,y,z);
    glm::vec4 rayClipSpace = glm::vec4(rayNdc.x, rayNdc.y, -1.0f, 1.0f);
    // View space
    glm::mat4 invProjection = glm::inverse(m_projectionMatrix);
    glm::vec4 rayViewSpace = invProjection * rayClipSpace;
    rayViewSpace = glm::vec4(rayViewSpace.x, rayViewSpace.y, -1.0f, 0.0f);
    // World space
    glm::mat4 invView = glm::inverse(m_viewMatrix);
    glm::vec4 rayWorld = invView * rayViewSpace;
    glm::vec3 rayDir = glm::normalize(rayWorld);

    return rayDir;
}

// Returns true if ray intersects triangle, and sets 'outT' to distance along ray
bool Application::rayIntersectsTriangle(const glm::vec3& rayOrigin, const glm::vec3& rayDir, const glm::vec3& v0, const glm::vec3& v1, const glm::vec3& v2, float& outT) {
    const float EPSILON = 1e-8f;
    glm::vec3 edge1 = v1 - v0;
    glm::vec3 edge2 = v2 - v0;
    glm::vec3 h = glm::cross(rayDir, edge2);
    float a = glm::dot(edge1, h);
    if (a > -EPSILON && a < EPSILON)
        return false; // Ray is parallel to triangle

    float f = 1.0f / a;
    glm::vec3 s = rayOrigin - v0;
    float u = f * glm::dot(s, h);
    if (u < 0.0f || u > 1.0f)
        return false;

    glm::vec3 q = glm::cross(s, edge1);
    float v = f * glm::dot(rayDir, q);
    if (v < 0.0f || u + v > 1.0f)
        return false;

    // At this stage we can compute t to find out where the intersection point is on the line
    float t = f * glm::dot(edge2, q);
    if (t > EPSILON) // Ray intersection
    {
        outT = t;
        return true;
    }
    else // No ray intersection
        return false;
}

glm::vec3 Application::getRaycastHitPosition(float mouseX, float mouseY, const glm::vec3& rayOrigin) {
    glm::vec3 rayDir = screenPosToWorldRayDir(mouseX, mouseY);
    m_ray->m_origin = rayOrigin;
    m_ray->m_direction = rayDir;
    glm::vec3 hitPosition;
    float closestT = std::numeric_limits<float>::max();
    bool hit = false;
    for (const auto& entries : m_loadedModel->entries) {
        auto& mesh = entries.mesh;
        for (size_t i = 0; i < mesh->indices.size(); i += 3) {
            glm::vec3 v0 = mesh->vertices[mesh->indices[i]].position;
            glm::vec3 v1 = mesh->vertices[mesh->indices[i+1]].position;
            glm::vec3 v2 = mesh->vertices[mesh->indices[i+2]].position;
            float t;
            if (rayIntersectsTriangle(rayOrigin, rayDir, v0, v1, v2, t)) {
                if (t < closestT) {
                    closestT = t;
                    hitPosition = rayOrigin + t * rayDir;
                    hit = true;
                }
            }
        }
    }
    if (hit) return hitPosition;
    else return glm::vec3(std::numeric_limits<float>::quiet_NaN());
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
        if (!app->m_camera->m_hasMouse) {
            double mouseX, mouseY;
            glfwGetCursorPos(window, &mouseX, &mouseY);
            app->m_ray->m_hitPosition = app->getRaycastHitPosition(mouseX, mouseY, app->m_camera->getPosition());
            std::cout << glm::to_string(app->m_ray->m_hitPosition) << std::endl;
        }
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
    if(key == GLFW_KEY_Z && action == GLFW_PRESS) {
        if(app->m_wireframe) {
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            app->m_wireframe = false;
        }
        else {
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            app->m_wireframe = true;
        }
    }
}