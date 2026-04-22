// VOID-STACK.cpp : Defines the entry point for the application.

#include "VOID-STACK.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include "Shader.h"
#include "AssetManager.h"

// Function prototypes (We keep main clean)
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

// Handle mouse movment
void mouse_callback(GLFWwindow* window, double xpos, double ypos);

void update_ship_rotation(float deltaTime);

// Camera globals
glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

// Timing globals
float deltaTime = 0.0f;
float lastFrame = 0.0f;


bool allow_loops = false;

// Mouse globals
float lastX = 600, lastY = 400;
bool firstMouse = true;

float yaw = 0.0f;
float pitch = 0.0f;
float roll = 0.0f;

// Flight Control
float mouseX = 0.0f, mouseY = 0.0f;
float leashRange = 250.0f;
float deadzone = 0.05f;
float autoCenterSpeed = 3.0f;

bool free_Cam = false;

int main() {
    // 1. Initialize GLFW
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }

    // 2. Configure GLFW (Setting OpenGL to version 3.3, Core Profile)
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // 3. Create the Window
    GLFWwindow* window = glfwCreateWindow(1200, 800, "VOID-STACK ENGINE", NULL, NULL);
    if (window == NULL) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    // 4. Initialize GLAD (Load all OpenGL function pointers)
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    glEnable(GL_DEPTH_TEST);

    // 5. Tell OpenGL the size of the rendering window
    glViewport(0, 0, 1200, 800);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);


    // 6. Shaders
    Shader ourShader(SHADER_DIR "shaderVS.txt", SHADER_DIR "shaderFS.txt");
    Shader ourLightShader(SHADER_DIR "lightShaderVS.txt", SHADER_DIR "lightShaderFS.txt");
    ourShader.use();
    ourLightShader.use();

    // 7. Geometry

    float vertices[] = {
        // positions          // normals
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
         0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,

        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
         0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,

        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,

         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,

        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f
    };

    unsigned int VAO;
    unsigned int VBO;

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    // 1. bind Vertex Array Object
    glBindVertexArray(VAO);

    // 2. copy our vertices array in a buffer for OpenGL to use
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // 3. then set our vertex attributes pointers
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    unsigned int lightVAO;
    glGenVertexArrays(1, &lightVAO);

    glBindVertexArray(lightVAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, mouse_callback);

    glm::vec3 lightPos(10.0f, 1.0f, 10.0f);

    AssetManager::LoadModel("planet", MODEL_DIR "low-poly-sphere.obj");
    AssetManager::LoadModel("ship", MODEL_DIR "/Ship/Ship.obj");

    // 8. The RENDER LOOP (The Heartbeat)
    while (!glfwWindowShouldClose(window)) {
        // Calculate per - frame time logic
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // Input
        processInput(window);

        // Rendering Commands
        // Deep Space Navy Color (0.05, 0.05, 0.1)
        glClearColor(0.05f, 0.05f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        update_ship_rotation(deltaTime);

        ourShader.use();

        ourShader.setVec3("viewPos", cameraPos);

        // Define light properties
        ourShader.setVec3("lightPos", lightPos);
        ourShader.setVec3("lightColor", 1.0f, 1.0f, 1.0f);
        ourShader.setVec3("objectColor", 1.0f, 0.5f, 0.31f);
        ourShader.setFloat("ambientStrength", 0.1f);

        //lightPos.x = 1.0f + sin(glfwGetTime()) * 2.0f;
        //lightPos.y = sin(glfwGetTime() / 2.0f);


        // Cam
        glm::vec3 cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f);

        // Reverse Direction!
        glm::vec3 cameraDirection = glm::normalize(cameraPos - cameraTarget);

        glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
        glm::vec3 cameraRight = glm::normalize(glm::cross(up, cameraDirection));


        // Matrixes
        glm::mat4 model = glm::mat4(1.0f);
        glm::mat4 projection;
        glm::mat4 view = glm::mat4(1.0f);

        view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
        projection = glm::perspective(glm::radians(45.0f), 1200.0f / 800.0f, 0.1f, 100.0f);
        
        // CUBE
        model = glm::translate(model, glm::vec3(2.0f, 0.0f, 0.0f));
        model = glm::rotate(model, (float)glfwGetTime() * glm::radians(50.0f), glm::vec3(0.5f, 1.0f, 0.0f));

        ourShader.setMat4("model", model);
        ourShader.setMat4("view", view);
        ourShader.setMat4("projection", projection);

        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        // SUN
        ourLightShader.use();
        ourLightShader.setMat4("view", view);
        ourLightShader.setMat4("projection", projection);

        model = glm::mat4(1.0f);
        model = glm::translate(model, lightPos);
        model = glm::scale(model, glm::vec3(5.0f));

        ourLightShader.setMat4("model", model);

        AssetManager::GetModel("planet").Draw(ourLightShader);

        // SHIP
        ourShader.use();
        ourShader.setMat4("projection", projection);
        ourShader.setMat4("view", view);

        glm::mat4 shipModel = glm::mat4(1.0f);

        if (!free_Cam) {
            // Camera position
            shipModel = glm::translate(shipModel, cameraPos);

            // Rotate the ship 
            shipModel = glm::rotate(shipModel, glm::radians(-yaw - 90.0f), glm::vec3(0, 1, 0));
            shipModel = glm::rotate(shipModel, glm::radians(pitch), glm::vec3(1, 0, 0));
            shipModel = glm::rotate(shipModel, glm::radians(roll), glm::vec3(0, 0, 1));

            // Adjust slightly down/forward so the camera is in the cockpit
            shipModel = glm::translate(shipModel, glm::vec3(0.0f, -2.0f, 1.8f));
        }
        else {
            shipModel = glm::translate(shipModel, glm::vec3(0.0f, 6.0f, 5.0f));
        }

        ourShader.setMat4("model", shipModel);


        AssetManager::GetModel("ship").Draw(ourShader);

        // Swap buffers and poll IO events
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // 9. Cleanup
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glfwTerminate();
    return 0;
}

// Handle window resizing
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

// Handle key presses
void processInput(GLFWwindow* window) {
    const float cameraSpeed = 2.5f * deltaTime;

    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    static bool b_pressed = false;
    if (glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS && !b_pressed) {
        free_Cam = !free_Cam;
        b_pressed = true;
    }
    else if (glfwGetKey(window, GLFW_KEY_B) == GLFW_RELEASE) {
        b_pressed = false;
    }

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        cameraPos += cameraSpeed * cameraFront;

    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        cameraPos -= cameraSpeed * cameraFront;

    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;

    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
}

// Handle mouse movment
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (free_Cam)
    {
        if (firstMouse)
        {
            lastX = xpos;
            lastY = ypos;
            firstMouse = false;
        }

        float xoffset = xpos - lastX;
        float yoffset = lastY - ypos;

        lastX = xpos;
        lastY = ypos;

        float sensitivity = 0.1f;

        xoffset *= sensitivity;
        yoffset *= sensitivity;

        yaw += xoffset;
        pitch += yoffset;

        if (!allow_loops)
        {
            if (pitch > 89.0f) pitch = 89.0f;
            if (pitch < -89.0f) pitch = -89.0f;
        }

        // Front vector
        glm::vec3 direction;
        direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
        direction.y = sin(glm::radians(pitch));
        direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
        cameraFront = glm::normalize(direction);

        // Roll
        glm::vec3 worldUp = glm::vec3(0.0f, 1.0f, 0.0f);
        glm::vec3 right = glm::normalize(glm::cross(cameraFront, worldUp));
        glm::mat4 roll_mat = glm::rotate(glm::mat4(1.0f), glm::radians(roll), cameraFront);

        cameraUp = glm::normalize(glm::vec3(roll_mat * glm::vec4(glm::cross(right, cameraFront), 0.0f)));
    }
    else
    {
        int width, height;
        glfwGetWindowSize(window, &width, &height);

        float centerX = width / 2.0f;
        float centerY = height / 2.0f;

        float dirX = xpos - centerX;
        float dirY = ypos - centerY;

        float sensitivity = 0.3f;

        dirX *= sensitivity;
        dirY *= sensitivity;

        if (pitch > 88.9f) pitch = 88.8f;
        if (pitch < -88.9f) pitch = -88.8f;

        // Limits the mouse position
        float distance = sqrt(dirX * dirX + dirY * dirY);
        if (distance > leashRange) {
            dirX = (dirX / distance) * leashRange;
            dirY = (dirY / distance) * leashRange;
            // Cursor back to the leash edge
            glfwSetCursorPos(window, centerX + dirX, centerY + dirY);
        }
        mouseX = dirX / leashRange;
        mouseY = dirY / leashRange; // Up is negative 
    }

}

void update_ship_rotation(float deltaTime) {
    float inputX = (abs(mouseX) < deadzone) ? 0.0f : mouseX;
    float inputY = (abs(mouseY) < deadzone) ? 0.0f : mouseY;

    float rotationSpeed = 60.0f;
    yaw += inputX * rotationSpeed * deltaTime;
    pitch -= inputY * rotationSpeed * deltaTime;

    // Banking
    float targetRoll = -inputX * 45.0f;
    roll = glm::mix(roll, targetRoll, deltaTime * 2.0f);

    // Auto-center the "leash"
    //mouseX = glm::mix(mouseX, 0.0f, deltaTime * autoCenterSpeed);
    //mouseY = glm::mix(mouseY, 0.0f, deltaTime * autoCenterSpeed);

    // Calculate Front
    glm::vec3 front;
    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y = sin(glm::radians(pitch));
    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    cameraFront = glm::normalize(front);

    // Temporary "Right" based on horizontal yaw to start
    glm::vec3 worldUp = glm::vec3(0, 1, 0);
    glm::vec3 right = glm::normalize(glm::cross(cameraFront, worldUp));

    // Rotate that Right vector by the Roll amount around the Front axis
    glm::mat4 rollMat = glm::rotate(glm::mat4(1.0f), glm::radians(roll), cameraFront);
    cameraUp = glm::normalize(glm::vec3(rollMat * glm::vec4(worldUp, 0.0f)));
}
