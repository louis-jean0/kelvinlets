#version 330 core

layout(location = 0) in vec3 aPos;

uniform mat4 u_viewMatrix;
uniform mat4 u_projectionMatrix;

void main() {
	gl_Position = u_projectionMatrix * u_viewMatrix * vec4(aPos, 1.0);
}