cmake_minimum_required (VERSION 3.8)

# Enable Hot Reload for MSVC
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
endif()

project ("VOID-STACK")

# 1. Find the packages FIRST
find_package(glfw3 CONFIG REQUIRED)
find_package(glad CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)

# 2. Create the executable ONCE (Using your actual files)
add_executable(VOID-STACK "VOID-STACK.cpp" "VOID-STACK.h" "Shader.h" "Shader.cpp")

# Shader folder path
target_compile_definitions(VOID-STACK PRIVATE SHADER_DIR="${CMAKE_CURRENT_SOURCE_DIR}/shaders/")

# Set C++ Standard
set_property(TARGET VOID-STACK PROPERTY CXX_STANDARD 20)

# 3. Link the libraries to your executable
target_link_libraries(VOID-STACK PRIVATE 
    glfw 
    glad::glad 
    glm::glm
)