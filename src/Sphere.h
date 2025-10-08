
#ifndef SPHERE_H
#define SPHERE_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>
#include <cmath>
#include <string>
#include <unordered_map>  // <-- Add this!

struct Atom {
    std::string element;
    glm::vec3 position;

    Atom(const std::string& element, const glm::vec3& position)
        : element(element), position(position) {}

    float getAtomicRadius(const std::string& element, const std::unordered_map<std::string, float>& vanDerWaalsRadii);
    glm::vec3 getAtomColor(const std::string& element, const std::unordered_map<std::string, glm::vec3>& atomColors);
};

struct SphereInstance {
    glm::vec3 position;
    float radius;
    glm::vec3 color;

    SphereInstance(const glm::vec3& position, float radius, const glm::vec3& color)
        : position(position), radius(radius), color(color) {}
};

struct Vertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 texCoords;
};

class Sphere {
public:
    Sphere(unsigned int sectorCount = 36, unsigned int stackCount = 18);
    ~Sphere();

    void draw();
    void drawInstances(const std::vector<SphereInstance>& instances);  // <-- Add this!

private:
    void generateMesh(unsigned int sectorCount, unsigned int stackCount);
    void setupMesh();

    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;

    GLuint VAO, VBO, EBO;

    // For instancing
    GLuint instanceVBO;
    bool instanceBufferInitialized = false;
};

#endif

