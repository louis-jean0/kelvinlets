#include <OrbitalCamera.hpp>
#include <iostream>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/constants.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>

OrbitalCamera::OrbitalCamera()
    : m_target(0.0f), m_yaw(0.0f), m_pitch(0.0f), m_radius(5.0f) {}

OrbitalCamera::OrbitalCamera(const glm::vec3& target)
    : m_target(target), m_yaw(0.0f), m_pitch(0.0f), m_radius(5.0f) {}

void OrbitalCamera::setTarget(const glm::vec3& t) {
    m_target = t;
}

void OrbitalCamera::processDrag(float xpos, float ypos) {
    if(m_isDragging) {
        float dx = xpos - m_lastX;
        float dy = ypos - m_lastY;
        float sensitivity = 0.005f;
        m_yaw -= dx * sensitivity;
        m_pitch -= dy * sensitivity;
        m_pitch = glm::clamp(m_pitch, -glm::half_pi<float>() + 0.01f, glm::half_pi<float>() - 0.01f);
        m_lastX = xpos;
        m_lastY = ypos;
    }
}

void OrbitalCamera::processPan(float xpos, float ypos) {
    if(m_isPanning) {
        float dx = xpos - m_lastX;
        float dy = ypos - m_lastY;
        float sensitivity = 0.005f;
        glm::vec3 viewDir = glm::normalize(m_target - getPosition());
        glm::vec3 cameraRight = glm::cross(viewDir, glm::vec3(0.0f, 1.0f, 0.0f));
        glm::vec3 cameraUp = glm::cross(viewDir, cameraRight);
        glm::vec3 panning = (dx * sensitivity) * cameraRight + (dy * sensitivity) * cameraUp;
        m_target += panning;
        m_lastX = xpos;
        m_lastY = ypos;
    }
}

void OrbitalCamera::processScroll(float yoffset) {
    m_radius -= yoffset * 0.5f;
    m_radius = glm::clamp(m_radius, 1.0f, 50.0f);
}

glm::vec3 OrbitalCamera::getPosition() const {
    float x = m_radius * cos(m_pitch) * sin(m_yaw);
    float y = m_radius * sin(m_pitch);
    float z = m_radius * cos(m_pitch) * cos(m_yaw);
    return m_target + glm::vec3(x, y, z);
}

glm::mat4 OrbitalCamera::getViewMatrix() const {
    glm::vec3 pos = getPosition();
    return glm::lookAt(pos, m_target, glm::vec3(0.0f, 1.0f, 0.0f));
}
