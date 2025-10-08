
#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

// Instance attributes
layout (location = 3) in vec3 instancePos;
layout (location = 4) in float instanceScale;
layout (location = 5) in vec3 instanceColor;

out vec3 FragPos;
out vec3 Normal;
out vec3 Color;

uniform mat4 view;
uniform mat4 projection;

void main()
{
    mat4 model = mat4(1.0);
    model[3].xyz = instancePos; // translate
    model = model * mat4(
        vec4(instanceScale, 0.0, 0.0, 0.0),
        vec4(0.0, instanceScale, 0.0, 0.0),
        vec4(0.0, 0.0, instanceScale, 0.0),
        vec4(0.0, 0.0, 0.0, 1.0)
    ); // scale

    FragPos = vec3(model * vec4(aPos, 1.0));
    Normal = mat3(transpose(inverse(model))) * aNormal;
    Color = instanceColor;

    gl_Position = projection * view * model * vec4(aPos, 1.0);
}

