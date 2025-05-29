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

bool Ray::rayIntersectsTriangle(const glm::vec3& v0, const glm::vec3& v1, const glm::vec3& v2, float& outT) {
	const float EPSILON = 1e-8f;
	glm::vec3 edge1 = v1 - v0;
	glm::vec3 edge2 = v2 - v0;
	glm::vec3 h = glm::cross(m_direction, edge2);
	float a = glm::dot(edge1, h);
	if (a > -EPSILON && a < EPSILON)
		return false; // Ray is parallel to triangle

	float f = 1.0f / a;
	glm::vec3 s = m_origin - v0;
	float u = f * glm::dot(s, h);
	if (u < 0.0f || u > 1.0f)
		return false;

	glm::vec3 q = glm::cross(s, edge1);
	float v = f * glm::dot(m_direction, q);
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

void Ray::getRaycastHitPosition(float mouseX, float mouseY, const glm::vec3& rayOrigin, Model* model) {
	m_origin = rayOrigin;
	glm::vec3 hitPosition;
	float closestT = std::numeric_limits<float>::max();
	bool hit = false;
	for (const auto& entries : model->entries) {
		auto& mesh = entries.mesh;
		for (size_t i = 0; i < mesh->indices.size(); i += 3) {
			glm::vec3 v0 = mesh->vertices[mesh->indices[i]].position;
			glm::vec3 v1 = mesh->vertices[mesh->indices[i + 1]].position;
			glm::vec3 v2 = mesh->vertices[mesh->indices[i + 2]].position;
			float t;
			if (rayIntersectsTriangle(v0, v1, v2, t)) {
				if (t < closestT) {
					closestT = t;
					hitPosition = m_origin + t * m_direction;
					hit = true;
				}
			}
		}
	}
	if (hit) m_hitPosition = hitPosition;
	else m_hitPosition = glm::vec3(std::numeric_limits<float>::quiet_NaN());
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
	/*glEnable(GL_LINE_WIDTH);
	glLineWidth(10.0f);*/
	glBindVertexArray(m_vao);
	glDrawArrays(GL_LINES, 0, 2);
	glBindVertexArray(0);
	//glDisable(GL_LINE_WIDTH);
}