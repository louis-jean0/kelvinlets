#version 330 core

layout(location = 0) in vec3 aPos;

struct Brush {
    float epsilon;
    float f;
};

struct Kelvinlet {
    Brush brush;
    float a;
    float b;
};

uniform mat4 u_viewMatrix;
uniform mat4 u_projectionMatrix;
uniform vec3 x0;
uniform Kelvinlet kelvinlet;

mat3 outerProduct(vec3 u, vec3 v) {
    return mat3(
        u.x * v.x, u.x * v.y, u.x * v.z,
        u.y * v.x, u.y * v.y, u.y * v.z,
        u.z * v.x, u.z * v.y, u.z * v.z
    );
}

void main() {
    vec3 r = aPos - x0;
    float rEpsilon = sqrt(dot(r, r) + pow(kelvinlet.brush.epsilon, 2));
    if (rEpsilon < 0.0001) rEpsilon = 0.0001;

    vec3 identity = vec3(1.0);
    vec3 term1 = (kelvinlet.a - kelvinlet.b) / rEpsilon * identity;
    mat3 rrT = outerProduct(r, r);
    vec3 term2 = (kelvinlet.b / pow(rEpsilon, 3)) * rrT * identity;
    vec3 term3 = (kelvinlet.a / 2.0) * (pow(kelvinlet.brush.epsilon, 2) / pow(rEpsilon, 3)) * identity;
    vec3 displacement = (term1 + term2 + term3) * kelvinlet.brush.f; 

    vec3 newPos = aPos + displacement;
    gl_Position = u_projectionMatrix * u_viewMatrix * vec4(newPos, 1.0);
}