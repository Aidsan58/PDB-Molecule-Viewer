
#define GLFW_INCLUDE_NONE
#include <glad/glad.h>
#include <GLFW/glfw3.h>
// GLM math library
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
// Shader class
#include "shader.h"
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>
#include <unordered_map>
#include "Sphere.h"
#include <cmath>

// ---- Atom Static Methods ----
float Atom::getAtomicRadius(const std::string& element, const std::unordered_map<std::string, float>& vanDerWaalsRadii) {
    auto it = vanDerWaalsRadii.find(element);
    return (it != vanDerWaalsRadii.end()) ? it->second : 1.5f;
}

glm::vec3 Atom::getAtomColor(const std::string& element, const std::unordered_map<std::string, glm::vec3>& atomColors) {
    auto it = atomColors.find(element);
    return (it != atomColors.end()) ? it->second : glm::vec3(1.0f);
}

// ---- Sphere ----
Sphere::Sphere(unsigned int sectorCount, unsigned int stackCount) {
    generateMesh(sectorCount, stackCount);
    setupMesh();
}

Sphere::~Sphere() {
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteBuffers(1, &instanceVBO);
}

// Thank you to © 2005-2025, Song Ho Ahn (안성호) for providing the math behind the sphere mesh
// https://www.songho.ca/opengl/gl_sphere.html
void Sphere::generateMesh(unsigned int sectorCount, unsigned int stackCount) {
    const float PI = 3.14159265359f;
    float x, y, z, xy;
    float radius = 1.0f;

    float sectorStep = 2 * PI / sectorCount;
    float stackStep = PI / stackCount;

    for (unsigned int i = 0; i <= stackCount; ++i) {
        float stackAngle = PI / 2 - i * stackStep;
        xy = radius * cosf(stackAngle);
        z = radius * sinf(stackAngle);

        for (unsigned int j = 0; j <= sectorCount; ++j) {
            float sectorAngle = j * sectorStep;

            x = xy * cosf(sectorAngle);
            y = xy * sinf(sectorAngle);

            Vertex vertex;
            vertex.position = glm::vec3(x, y, z);
            vertex.normal = glm::normalize(vertex.position);
            vertex.texCoords = glm::vec2((float)j / sectorCount, (float)i / stackCount);
            vertices.push_back(vertex);
        }
    }

    // Indices
    for (unsigned int i = 0; i < stackCount; ++i) {
        unsigned int k1 = i * (sectorCount + 1);
        unsigned int k2 = k1 + sectorCount + 1;

        for (unsigned int j = 0; j < sectorCount; ++j, ++k1, ++k2) {
            if (i != 0) {
                indices.push_back(k1);
                indices.push_back(k2);
                indices.push_back(k1 + 1);
            }
            if (i != (stackCount - 1)) {
                indices.push_back(k1 + 1);
                indices.push_back(k2);
                indices.push_back(k2 + 1);
            }
        }
    }
}

void Sphere::setupMesh() {
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    // Vertex buffer
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

    // Index buffer
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

    // Vertex attributes
    // position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    glEnableVertexAttribArray(0);

    // normal
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
    glEnableVertexAttribArray(1);

    // texCoords
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoords));
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);
}

void Sphere::draw() {
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void Sphere::drawInstances(const std::vector<SphereInstance>& instances) {
    if (!instanceBufferInitialized) {
        glGenBuffers(1, &instanceVBO);
        instanceBufferInitialized = true;
    }

    glBindVertexArray(VAO);

    // Format: vec3 pos, float scale, vec3 color → total = 7 floats
    std::vector<float> instanceData;
    for (const auto& inst : instances) {
        instanceData.push_back(inst.position.x);
        instanceData.push_back(inst.position.y);
        instanceData.push_back(inst.position.z);
        instanceData.push_back(inst.radius);
        instanceData.push_back(inst.color.r);
        instanceData.push_back(inst.color.g);
        instanceData.push_back(inst.color.b);
    }

    glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
    glBufferData(GL_ARRAY_BUFFER, instanceData.size() * sizeof(float), instanceData.data(), GL_DYNAMIC_DRAW);

    // Position (vec3)
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(3);
    glVertexAttribDivisor(3, 1);

    // Radius (float)
    glVertexAttribPointer(4, 1, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(4);
    glVertexAttribDivisor(4, 1);

    // Color (vec3)
    glVertexAttribPointer(5, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)(4 * sizeof(float)));
    glEnableVertexAttribArray(5);
    glVertexAttribDivisor(5, 1);

    // Draw
    glDrawElementsInstanced(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0, instances.size());

    glBindVertexArray(0);
}

