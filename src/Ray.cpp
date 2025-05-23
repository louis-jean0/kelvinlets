#include <glad/glad.h>
#include <iostream>
#include <Ray.hpp>

Ray::Ray() {
	setupOpenGL();
}

Ray::Ray(glm::vec3 origin) : m_origin(origin), m_direction(glm::vec3(0.0f)) {
	setupOpenGL();
}

Ray::Ray(glm::vec3 origin, glm::vec3 direction) : m_origin(origin), m_direction(direction) {
	setupOpenGL();
}

Ray::~Ray() {
	if (m_vbo) glDeleteBuffers(1, &m_vbo);
	if (m_vao) glDeleteBuffers(1, &m_vao);
}

void Ray::setupOpenGL() {
	glGenVertexArrays(1, &m_vao);
	glGenBuffers(1, &m_vbo);

	glBindVertexArray(m_vao);
	
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glBufferData(GL_ARRAY_BUFFER, 2 * sizeof(glm::vec3), NULL, GL_DYNAMIC_DRAW);
	
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
	
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void Ray::updateRay() {
	if (glm::any(glm::isnan(m_hitPosition))) return;
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	std::cout << "Ray origin : " << m_origin.x << " " << m_origin.y << " " << m_origin.z << std::endl;
	std::cout << "Ray hit position : " << m_hitPosition.x << " " << m_hitPosition.y << " " << m_hitPosition.z << std::endl;
	glm::vec3 vertices[2] = { m_origin, m_hitPosition };
	glBufferSubData(GL_ARRAY_BUFFER, 0, 2 * sizeof(glm::vec3), vertices);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Ray::drawRay() const {
	glBindVertexArray(m_vao);
	glDrawArrays(GL_LINES, 0, 2);
	glBindVertexArray(0);
}