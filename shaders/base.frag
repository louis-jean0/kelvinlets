#version 330 core

in vec3 Normal;
in vec3 FragPos;

out vec4 FragColor;

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    vec3 emissive;
    float shininess;
};

uniform Material u_material;
uniform vec3 u_viewPos;

void main() {
    vec3 lightPosition = vec3(0.0, 10.0, 0.0);
    vec3 lightDir = normalize(lightPosition - FragPos);
    vec3 reflectDir = reflect(-lightDir, Normal);
    vec3 ambient = u_material.ambient;
    float diff = max(dot(Normal, lightDir), 0.0);
    vec3 diffuse = diff * u_material.diffuse;
    vec3 viewDir = normalize(FragPos - u_viewPos);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), u_material.shininess);
    vec3 specular = spec * u_material.specular;
    vec3 result = ambient + diffuse + specular; 
    FragColor = vec4(result, 1.0); 
}