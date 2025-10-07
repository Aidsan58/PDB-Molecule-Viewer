#ifndef SPHERE_H
#define SPHERE_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
// GLM math library
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>
#include <cmath>

struct Atom {
    std::string element;
    glm::vec3 position;

    Atom(const std::string& element, const glm::vec3& position)
        : element(element), position(position) {}

    float getAtomicRadius(const std::string& element, std::unordered_map<std::string, float> vanDerWaalsRadii);
    glm::vec3 getAtomColor(const std::string& element, std::unordered_map<std::string, glm::vec3> atomColors);
};

struct SphereInstance {
    glm::vec3 position;
    float radius;
    glm::vec3 color;

    SphereInstance(const glm::vec3& position, float radius, const glm::vec3& color)
        : position(position), radius(radius), color(color) {}

    SphereInstance MakeSphere(const Atom& atom);
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

    void draw(); // For single drawing (non-instanced)
    // Add drawInstances(...) later for instancing

private:
    void generateMesh(unsigned int sectorCount, unsigned int stackCount);
    void setupMesh();

    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;

    GLuint VAO, VBO, EBO;
};

#endif
