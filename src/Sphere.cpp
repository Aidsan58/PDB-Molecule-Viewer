
#include "Sphere.h"
#include <cmath>


Atom::getAtomicRadius(const std::string& element, std::unordered_map<std::string, float> vanDerWaalsRadii) {
    auto it = vanDerWaalsRadii.find(element);
    if (it != vanDerWaalsRadii.end())
        return it->second * 0.1f; // scale to fit your scene
    else
        return 1.5f * 0.1f; // fallback radius
}

Atom::getAtomColor(const std::string& element, std::unordered_map<std::string, glm::vec3> atomColors) {
    auto it = atomColors.find(element);
    if (it != atomColors.end()) {
        return it->second;
    } else {
        return glm::vec3(1.0f, 1.0f, 1.0f); // default: white
    }
}

SphereInstance::makeSphere(const Atom& atom) {
    float radius = getAtomicRadius(atom.element);
    glm::vec3 color = getAtomColor(atom.element);
    return SphereInstance(atom.position, radius, color);
}

Sphere::Sphere(unsigned int sectorCount, unsigned int stackCount) {
    generateMesh(sectorCount, stackCount);
    setupMesh();
}

Sphere::~Sphere() {
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
}

void Sphere::generateMesh(unsigned int sectorCount, unsigned int stackCount) {
    const float PI = 3.14159265359f;
    float x, y, z, xy;
    float nx, ny, nz;
    float s, t;

    float radius = 1.0f;
    float lengthInv = 1.0f / radius;

    float sectorStep = 2 * PI / sectorCount;
    float stackStep = PI / stackCount;
    float sectorAngle, stackAngle;

    // Generate vertices
    for (unsigned int i = 0; i <= stackCount; ++i) {
        stackAngle = PI / 2 - i * stackStep;   // from pi/2 to -pi/2
        xy = radius * cosf(stackAngle);
        z = radius * sinf(stackAngle);

        for (unsigned int j = 0; j <= sectorCount; ++j) {
            sectorAngle = j * sectorStep;

            x = xy * cosf(sectorAngle);
            y = xy * sinf(sectorAngle);

            Vertex vertex;
            vertex.position = glm::vec3(x, y, z);
            vertex.normal = glm::normalize(vertex.position);
            vertex.texCoords = glm::vec2((float)j / sectorCount, (float)i / stackCount);

            vertices.push_back(vertex);
        }
    }

    // Generate indices
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

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

    // Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    glEnableVertexAttribArray(0);

    // Normal attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
    glEnableVertexAttribArray(1);

    // TexCoord attribute
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoords));
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);
}

void Sphere::draw() {
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}
