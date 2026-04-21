cmake_minimum_required (VERSION 3.11) # FetchContent needs 3.11+

project ("VOID-STACK")

# --- Assimp FetchContent Section ---
include(FetchContent)
FetchContent_Declare(
    assimp
    GIT_REPOSITORY https://github.com/assimp/assimp.git
    GIT_TAG v5.3.1 
)
# Disable unnecessary Assimp features to speed up build time
set(ASSIMP_BUILD_ASSIMP_TOOLS OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(ASSIMP_INSTALL OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_ZLIB ON CACHE BOOL "" FORCE) # Assimp needs Zlib to unzip formats like .fbx
FetchContent_MakeAvailable(assimp)
# -----------------------------------

# 1. Find the other packages
find_package(glfw3 CONFIG REQUIRED)
find_package(glad CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)

# 2. Create the executable
add_executable(VOID-STACK 
    "VOID-STACK.cpp" 
    "VOID-STACK.h" 
    "Shader.h" 
    "Shader.cpp" 
    "AssetManager.h" 
    "Model.h"
 "Mesh.h" "Model.cpp" "AssetManager.cpp" "stb_image.h")

# Shader folder path
target_compile_definitions(VOID-STACK PRIVATE 
    SHADER_DIR=\"${CMAKE_CURRENT_SOURCE_DIR}/shaders/\"
    MODEL_DIR=\"${CMAKE_CURRENT_SOURCE_DIR}/models/\"
)

# Set C++ Standard
set_property(TARGET VOID-STACK PROPERTY CXX_STANDARD 20)

# 3. Link the libraries (Added assimp here)
target_link_libraries(VOID-STACK PRIVATE 
    glfw 
    glad::glad 
    glm::glm
    assimp::assimp  # <--- Added this
)

add_custom_command(TARGET VOID-STACK POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    "$<TARGET_FILE:assimp::assimp>"        # Source (Assimp's DLL location)
    "$<TARGET_FILE_DIR:VOID-STACK>"        # Destination (Where your .exe is)
)