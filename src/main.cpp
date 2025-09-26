#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/gl.h>

if (!glfwInit()) {
    // Initialization failed
}

void error_callback(int error, const char* description) {
    fprintf(stderr, "Error: %s\n", description);
}

glfwSetErrorCallback(error_callback);

glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
GLFWwindow* window = glfwCreateWindow(640, 480, "My Title", NULL, NULL);
if (!window) {
    // Window or OpenGL context creation failed
}

glfwMakeContextCurrent(window);

glfwTerminate(); // Destroys any remaining windows and resources allocated by glfw
