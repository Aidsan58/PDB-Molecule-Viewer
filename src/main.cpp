#define GLFW_INCLUDE_NONE
#include <glad/glad.h>
#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#include <GLFW/glfw3.h>
// GLM math library
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
// Shader class
#include "shader.h"
// STB image loader
#include "stb_image.h"
// Camera class
#include "flyCamera.h"
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>
#include <stdio.h>
#include <filesystem>
#include "Sphere.h"
#include <unordered_map>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);

unsigned int loadTexture(const char *path);
// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;

// lighting
glm::vec3 lightPos(1.2f, 1.0f, 2.0f);

// cpk coloring map
std::unordered_map<std::string, glm::vec3> elementColors = {
    {"C", glm::vec3(0.784f, 0.784f, 0.784f)},   // gray
    {"O", glm::vec3(0.941f, 0.0f, 0.0f)},   // red
    {"N", glm::vec3(0.561f, 0.561f, 1.0f)},   // light blue
    {"H", glm::vec3(1.0f, 1.0f, 1.0f)},   // white
    {"S", glm::vec3(1.0f, 0.784f, 0.0f)},   // yellow
    {"P", glm::vec3(1.0f, 0.647f, 0.0f)},   // orange
    {"NA", glm::vec3(0.0f, 0.0f, 1.0f)},   // blue
    {"CL", glm::vec3(0.0f, 1.0f, 0.0f)},   // green
    {"FE", glm::vec3(1.0f, 0.647f, 0.0f)},   // orange
    {"CA", glm::vec3(0.502f, 0.502f, 0.565f)},   // dark grey
    {"BR", glm::vec3(0.647f, 0.165f, 0.165f)},   // brown
    {"MG", glm::vec3(0.133f, 0.545f, 0.133f)},   // forest green
};
// cpk atomic radii in Angstroms (divided by 100)
std::unordered_map<std::string, float> elementRadii = {
    {"C", 1.70f},
    {"O", 1.52f},
    {"N", 1.55f},
    {"H", 1.20f},
    {"S", 1.80f},
    {"P", 1.80f},
    {"NA", 2.27f},
    {"CL", 1.75f},
    {"FE", 1.94f},
    {"CA", 1.97f},    
    {"BR", 1.85f},
    {"MG", 1.73f},
};


int main() {
    // Instantiate GLFW window
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Create window object
    GLFWwindow* window = glfwCreateWindow(800, 600, "LearnOpenGL", NULL, NULL);
    if (window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);     // Set viewport resize callback
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // tell GLFW to capture our mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    
    // Initialize GLAD & load all openGL function pointers
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }
    
    stbi_set_flip_vertically_on_load(true);   
    
    // Enable depth buffer
    glEnable(GL_DEPTH_TEST);
   
    // Shader Program
    Shader ourShader("shaders/atomVertexShader.glsl", "shaders/atomFragmentShader.glsl");
    
    // Sphere class
    Sphere sphere;
    
    std::vector<SphereInstance> instances;
   
    // Parse PDB file
    std::ifstream inputFile("examples/2PGH.pdb");
    std::string line;
    if (inputFile.is_open()) { // Check if the file opened successfully
        while (std::getline(inputFile, line)) { // Read line by line    
            std::istringstream iss(line);
            std::string token;
            int number;

            // ATOM    789  CG  LEU A 106      14.830  -9.976   3.271  1.00 10.45           C  
            std::string nameLabel, nameValue, otherLabel, chain, element;
            int index;
            float x, y, z, occupancy, temperatureFactor;
            glm::vec3 position;
            if (line.substr(0, 6) == "ATOM  ") {
                std::string element = line.substr(76, 2);
                float x = std::stof(line.substr(30, 8));
                float y = std::stof(line.substr(38, 8));
                float z = std::stof(line.substr(46, 8));

                glm::vec3 position = glm::vec3(x, y, z);
                
                // Trim spaces from element string
                element.erase(remove_if(element.begin(), element.end(), ::isspace), element.end());

                Atom atom(element, position);
                glm::vec3 color = atom.getAtomColor(atom.element, elementColors);
                float radius = atom.getAtomicRadius(atom.element, elementRadii);
                instances.emplace_back(atom.position, radius, color);
            }      
        }
        inputFile.close(); // Close the file
    } else {
        std::cerr << "Error: Unable to open file." << std::endl;
    }
    
 
    // Render loop
    while (!glfwWindowShouldClose(window))
    {
        // per-frame time logic
        // --------------------
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // input
        // -----
        processInput(window);

        // render
        // ------
        glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // don't forget to enable shader before setting uniforms
            
        ourShader.use();

        // Setup camera matrices
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();

        ourShader.setMat4("view", view);
        ourShader.setMat4("projection", projection);
        ourShader.setVec3("lightPos", lightPos);
        ourShader.setVec3("viewPos", camera.Position);
                
        sphere.drawInstances(instances);
        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}
// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}


// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(static_cast<float>(yoffset));
}

unsigned int loadTexture(char const * path)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);
    
    int width, height, nrComponents;
    unsigned char *data = stbi_load(path, &width, &height, &nrComponents, 0);
    if (data)
    {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else
    {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}
