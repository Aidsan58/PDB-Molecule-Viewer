#version 330 core

layout(location = 0) in vec3 aPos;            // sphere vertex
layout(location = 1) in vec3 instancePos;     // atom position
layout(location = 2) in float instanceScale;  // radius
layout(location = 3) in vec3 instanceColor;   // color

out vec3 fragColor;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
    vec3 worldPosition = instancePos + aPos * instanceScale;
    gl_Position = projection * view * model * vec4(worldPosition, 1.0);
    fragColor = instanceColor;
}

