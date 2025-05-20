#version 330 core

layout(location = 0) in vec3 aPos;

uniform mat4 u_viewMatrix;
uniform mat4 u_projectionMatrix;

void main() {
    gl_PointSize = 10.0;
    vec4 newPos = u_projectionMatrix * u_viewMatrix * mat4(1.0) * vec4(aPos,1.0);
    gl_Position = newPos; //vec4(aPos, 1.0);
}