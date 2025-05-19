#pragma once
#include <glm/glm.hpp>

class OrbitalCamera {
public:
    OrbitalCamera();
    OrbitalCamera(const glm::vec3& target);
    
    void setTarget(const glm::vec3& t);
    void processDrag(float xpos, float ypos);
    void processPan(float xpos, float ypos);
    void processScroll(float yoffset);

    glm::mat4 getViewMatrix() const;
    glm::vec3 getPosition() const;

public:
    bool m_isDragging;
    bool m_isPanning;
    double m_lastX;
    double m_lastY;

private:
    glm::vec3 m_target;
    float m_yaw;
    float m_pitch;
    float m_radius;
};
