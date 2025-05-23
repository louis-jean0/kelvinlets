#pragma once

#include <glm/glm.hpp>
#include <glfw/glfw3.h>

class Ray {
	public:
		glm::vec3 m_origin = glm::vec3(0.0f);
		glm::vec3 m_direction = glm::vec3(0.0f);
		glm::vec3 m_hitPosition = glm::vec3(0.0f);
		GLuint m_vao = 0;
		GLuint m_vbo = 0;

		Ray();
		Ray(glm::vec3 origin);
		Ray(glm::vec3 origin, glm::vec3 direction);
		~Ray();

		void setupOpenGL();
		void updateRay();
		void drawRay() const;
};