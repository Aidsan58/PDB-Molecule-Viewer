cmake_minimum_required(VERSION 3.14)
project(OpenGL_Project)

# Add GLAD
add_library(glad STATIC src/glad.c)
target_include_directories(glad PUBLIC include)

# Option 1: GLFW as subdirectory (recommended if not installed system-wide)
# Replace with correct path to GLFW source
add_subdirectory("/home/aidan/Programs/GLFW/glfw-3.4" "${CMAKE_BINARY_DIR}/glfw-build")

# Add Dear Imgui
set(IMGUI_DIR ${CMAKE_CURRENT_SOURCE_DIR}/imgui/imgui-master)

set(IMGUI_SRC
    ${IMGUI_DIR}/imgui.cpp
    ${IMGUI_DIR}/imgui_draw.cpp
    ${IMGUI_DIR}/imgui_tables.cpp
    ${IMGUI_DIR}/imgui_widgets.cpp
  )

add_library(imgui STATIC ${IMGUI_SRC})
target_include_directories(imgui PUBLIC ${IMGUI_DIR})

# Find GLM (must be installed)
find_package(glm REQUIRED)

# Find OpenGL
find_package(OpenGL REQUIRED)

# Add executable
add_executable(my_opengl_app src/main.cpp)

# Link libraries
target_link_libraries(my_opengl_app
    PRIVATE
    OpenGL::GL
    glad
    glfw  # target name is 'glfw' when using add_subdirectory
    glm::glm
    imgui
)
