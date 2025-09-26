cmake_minimum_required(VERSION 3.14)
project(OpenGL_Project)

# Find and link necessary libraries
find_package(OpenGL REQUIRED)
find_package(GLFW3 REQUIRED)
find_package(glm REQUIRED)
target_include_directories(glad_lib PUBLIC include) # Assuming 'include' contains glad/glad.h

# Add your executable
add_executable(my_opengl_app src/main.cpp src/glad.c) # Include glad.c if using GLAD

# Link libraries to your executable
target_link_libraries(my_opengl_app PRIVATE OpenGL::GL glad_lib GLFW::GLFW glm)
