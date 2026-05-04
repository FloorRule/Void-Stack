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
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

// Function prototypes (We keep main clean)
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

// Handle mouse movment
void mouse_callback(GLFWwindow* window, double xpos, double ypos);

void update_ship_rotation(float deltaTime);

// Camera globals
glm::vec3 cameraPos = glm::vec3(30.0f, 0.0f, 30.0f);
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

// Orientation
glm::quat shipOrientation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
float visualBank = 0.0f;

// Flight Control
float mouseX = 0.0f, mouseY = 0.0f;
float leashRange = 250.0f;
float deadzone = 0.05f;
float autoCenterSpeed = 3.0f;

bool free_Cam = false;

float fov = 45.0f;

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

    // ImGui setup
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    // Setup Style
    ImGui::StyleColorsDark();

    glEnable(GL_DEPTH_TEST);

    // 5. Tell OpenGL the size of the rendering window
    glViewport(0, 0, 1200, 800);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);


    // 6. Shaders
    Shader ourShader(SHADER_DIR "shaderVS.txt", SHADER_DIR "shaderFS.txt");
    Shader ourLightShader(SHADER_DIR "lightShaderVS.txt", SHADER_DIR "lightShaderFS.txt");
    Shader skyboxShader(SHADER_DIR "skyboxVS.txt", SHADER_DIR "skyboxFS.txt");

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

    float skyboxVertices[] = {
        // positions          
        -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

        -1.0f,  1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f,  1.0f
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


    unsigned int skyboxVAO, skyboxVBO;
    glGenVertexArrays(1, &skyboxVAO);

    glGenBuffers(1, &skyboxVBO);
    glBindVertexArray(skyboxVAO);

    glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

    // Load the textures
    std::vector<std::string> faces{
        MODEL_DIR "skybox/right.png", MODEL_DIR "skybox/left.png", MODEL_DIR "skybox/top.png", MODEL_DIR "skybox/bottom.png", MODEL_DIR "skybox/front.png", MODEL_DIR "skybox/back.png"
    };
    unsigned int cubemapTexture = AssetManager::loadCubemap(faces);
    


    glBindVertexArray(0);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, mouse_callback);

    glm::vec3 lightPos(0.0f, -10.0f, 0.0f);

    AssetManager::LoadModel("sun", MODEL_DIR "Sphere.obj");
    AssetManager::LoadModel("planet", MODEL_DIR "Sphere.obj");
    AssetManager::LoadModel("ship", MODEL_DIR "/Ship/Ship.obj");

    // 8. The RENDER LOOP (The Heartbeat)
    while (!glfwWindowShouldClose(window)) {
        // Calculate per - frame time logic
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        glm::vec3 lastCameraPos = cameraPos;
        lastFrame = currentFrame;

        // ImGUI init
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // Input
        processInput(window);

        // UI
        ImGui::Begin("Engine Stats");
        ImGui::Text("Ship Speed: %.2f", glm::distance(lastCameraPos, cameraPos)/ deltaTime);
        ImGui::Text("FOV: %.2f", fov);
        ImGui::Text("FPS: %.1f", io.Framerate);
        ImGui::End();


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
        projection = glm::perspective(glm::radians(fov), 1200.0f / 800.0f, 0.1f, 1000.0f);
        
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
        model = glm::scale(model, glm::vec3(20.0f));

        ourLightShader.setMat4("model", model);

        AssetManager::GetModel("sun").Draw(ourLightShader);

        // Planet
        ourShader.use();
        ourShader.setMat4("view", view);
        ourShader.setMat4("projection", projection);

        model = glm::mat4(1.0f);
        model = glm::rotate(model, (float)glfwGetTime() * 0.2f, glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::translate(model, glm::vec3(50.0f, 0.0f, 0.0f));
        model = glm::rotate(model, (float)glfwGetTime() * 1.0f, glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::scale(model, glm::vec3(5.0f));

        ourShader.setMat4("model", model);

        AssetManager::GetModel("planet").Draw(ourShader);

        // SHIP
        ourShader.use();
        ourShader.setMat4("projection", projection);
        ourShader.setMat4("view", view);

        glm::mat4 shipModel = glm::mat4(1.0f);

        if (!free_Cam) {
            // Camera position
            shipModel = glm::translate(shipModel, cameraPos);

            // Apply the physical orientation of the ship
            shipModel *= glm::mat4_cast(shipOrientation);

            // Apply the visual bank
            shipModel = glm::rotate(shipModel, glm::radians(visualBank), glm::vec3(0, 0, 1));

            // Adjust for model offset
            shipModel = glm::translate(shipModel, glm::vec3(0.0f, -2.0f, 1.8f));
        }
        else {
            shipModel = glm::translate(shipModel, glm::vec3(0.0f, 6.0f, 5.0f));
        }

        ourShader.setMat4("model", shipModel);


        AssetManager::GetModel("ship").Draw(ourShader);


        glDepthFunc(GL_LEQUAL);  // Change depth function
        skyboxShader.use();

        // Remove translation
        view = glm::mat4(glm::mat3(glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp)));
        skyboxShader.setMat4("view", view);
        skyboxShader.setMat4("projection", projection);

        glBindVertexArray(skyboxVAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glDepthFunc(GL_LESS);

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        // Swap buffers and poll IO events
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // 9. Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

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
    const float baseSpeed = 1.5f;
    const float maxBoostSpeed = 25.0f;
    const float maxSpeed = 10.0f;
    const float accelRate = 2.0f;

    const float rollSpeed = 2.0f * deltaTime;
    static float currentForwardSpeed = baseSpeed;

    float targetSpeed = baseSpeed;
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
        targetSpeed = maxBoostSpeed;
    }
    else if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        targetSpeed = maxSpeed;

    currentForwardSpeed = glm::mix(currentForwardSpeed, targetSpeed, deltaTime * accelRate);
    fov = glm::mix(45.0f, 50.0f * (currentForwardSpeed / baseSpeed), deltaTime);


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
    {
        cameraPos += (currentForwardSpeed * deltaTime) * cameraFront;
    }
        

    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    {
        cameraPos -= (currentForwardSpeed * deltaTime) * cameraFront;
    }
        

    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
        glm::quat rollQuat = glm::angleAxis(rollSpeed * deltaTime, glm::vec3(0, 0, 1));
        shipOrientation = shipOrientation * rollQuat;
    }

    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
        glm::quat rollQuat = glm::angleAxis(rollSpeed * deltaTime, glm::vec3(0, 0, 1));
        shipOrientation = shipOrientation * rollQuat;
    }
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

        // Limits the mouse position
        float distance = sqrt(dirX * dirX + dirY * dirY);
        if (distance > leashRange) {
            dirX = (dirX / distance) * leashRange;
            dirY = (dirY / distance) * leashRange;
            // Cursor back to the leash edge
            glfwSetCursorPos(window, centerX + dirX, centerY + dirY);
        }
        mouseX = dirX / leashRange;
        mouseY = dirY / leashRange; // Up negative 
    }

}

void update_ship_rotation(float deltaTime) {
    float inputX = (abs(mouseX) < deadzone) ? 0.0f : mouseX;
    float inputY = (abs(mouseY) < deadzone) ? 0.0f : mouseY;

    float rotationSpeed = 2.0f; // Radians

    // Calculate local rotatio
    glm::quat pitchQuat = glm::angleAxis(-inputY * rotationSpeed * deltaTime, glm::vec3(1, 0, 0));
    glm::quat yawQuat = glm::angleAxis(-inputX * rotationSpeed * deltaTime, glm::vec3(0, 1, 0));

    // Combine with existing orientation
    shipOrientation = shipOrientation * pitchQuat * yawQuat;
    shipOrientation = glm::normalize(shipOrientation);

    // Visual Banking
    float targetBank = -inputX * 35.0f;
    visualBank = glm::mix(visualBank, targetBank, deltaTime * 5.0f);

    // Update Camera Vectors
    cameraFront = shipOrientation * glm::vec3(0, 0, -1);
    glm::vec3 shipUp = shipOrientation * glm::vec3(0, 1, 0);

    // Bank tilt to the camera Up vector
    glm::mat4 bankMat = glm::rotate(glm::mat4(1.0f), glm::radians(visualBank), cameraFront);
    cameraUp = glm::vec3(bankMat * glm::vec4(shipUp, 0.0f));
}
