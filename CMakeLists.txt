cmake_minimum_required (VERSION 3.11)
project ("VOID-STACK")

include(FetchContent)

# --- Assimp ---
FetchContent_Declare(
    assimp
    GIT_REPOSITORY https://github.com/assimp/assimp.git
    GIT_TAG v5.3.1 
)
set(ASSIMP_BUILD_ASSIMP_TOOLS OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(ASSIMP_INSTALL OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_ZLIB ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(assimp)

# --- ImGui ---
FetchContent_Declare(
    imgui
    GIT_REPOSITORY https://github.com/ocornut/imgui.git
    GIT_TAG v1.91.1
)
FetchContent_MakeAvailable(imgui)

# Create the ImGui library
add_library(imgui STATIC
    ${imgui_SOURCE_DIR}/imgui.cpp
    ${imgui_SOURCE_DIR}/imgui_draw.cpp
    ${imgui_SOURCE_DIR}/imgui_tables.cpp
    ${imgui_SOURCE_DIR}/imgui_widgets.cpp
    ${imgui_SOURCE_DIR}/imgui_demo.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp
)
target_include_directories(imgui PUBLIC ${imgui_SOURCE_DIR} ${imgui_SOURCE_DIR}/backends)
target_link_libraries(imgui PRIVATE glfw glad::glad)

# --- Find packages ---
find_package(glfw3 CONFIG REQUIRED)
find_package(glad CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)

# --- Your Executable ---
add_executable(VOID-STACK 
    "VOID-STACK.cpp" 
    "VOID-STACK.h" 
    "Shader.h" 
    "Shader.cpp" 
    "AssetManager.h" 
    "Model.h"
    "Mesh.h" 
    "Model.cpp" 
    "AssetManager.cpp" 
    "stb_image.h"
)

target_compile_definitions(VOID-STACK PRIVATE 
    SHADER_DIR=\"${CMAKE_CURRENT_SOURCE_DIR}/shaders/\"
    MODEL_DIR=\"${CMAKE_CURRENT_SOURCE_DIR}/models/\"
)

set_property(TARGET VOID-STACK PROPERTY CXX_STANDARD 20)

# --- Link everything ---
target_link_libraries(VOID-STACK PRIVATE 
    glfw 
    glad::glad 
    glm::glm
    assimp::assimp
    imgui # ImGui
)

add_custom_command(TARGET VOID-STACK POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    "$<TARGET_FILE:assimp::assimp>"
    "$<TARGET_FILE_DIR:VOID-STACK>"
)