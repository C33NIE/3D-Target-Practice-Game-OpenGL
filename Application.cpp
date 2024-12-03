#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include "Camera.h"
#include "Shader.h"
#include "Raycaster.h"
#include "Model.h"
#include <vector>


// Window settings
const unsigned int WIDTH = 800;
const unsigned int HEIGHT = 600;

// Create a Camera object
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f), glm::vec3(0.0f, 1.0f, 0.0f), -90.0f, 0.0f);

// Time tracking variables for smooth movement
float deltaTime = 0.0f;
float lastFrame = 0.0f;

// Mouse tracking variables
float lastX = 400, lastY = 300;
bool firstMouse = true;

// Light properties
glm::vec3 lightPos(2.0f, 1.0f, 1.0f); // Initial light position
float lightIntensity = 1.0f;          // Light brightness

// Raycaster instance
Raycaster raycaster;

// VAO for the ray line
unsigned int rayVAO, rayVBO;

// Callback for mouse movement
void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;  // Reversed: y-coordinates range bottom to top
    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

// Process keyboard input
void processInput(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    // Camera movement
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
        camera.ProcessKeyboard(UP, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
        camera.ProcessKeyboard(DOWN, deltaTime);

    // Light position tweaking
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
        lightPos.z -= 0.1f;
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
        lightPos.z += 0.1f;
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
        lightPos.x -= 0.1f;
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
        lightPos.x += 0.1f;
    if (glfwGetKey(window, GLFW_KEY_PAGE_UP) == GLFW_PRESS)
        lightPos.y += 0.1f;
    if (glfwGetKey(window, GLFW_KEY_PAGE_DOWN) == GLFW_PRESS)
        lightPos.y -= 0.1f;

    // Adjust light intensity
    if (glfwGetKey(window, GLFW_KEY_KP_ADD) == GLFW_PRESS) // '+'
        lightIntensity = glm::min(lightIntensity + 0.05f, 10.0f);
    if (glfwGetKey(window, GLFW_KEY_KP_SUBTRACT) == GLFW_PRESS) // '-'
        lightIntensity = glm::max(lightIntensity - 0.05f, 0.0f);
}

// Callback for mouse clicks to calculate and visualize the ray
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        double xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos);

        // Calculate the ray
        raycaster.calculateRay(
            glm::vec2(xpos, ypos),
            glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f),
            camera.GetViewMatrix(),
            glm::vec2(800, 600)
        );

        // Debug output
        std::cout << "Ray Origin: " << raycaster.origin.x << ", " << raycaster.origin.y << ", " << raycaster.origin.z << std::endl;
        std::cout << "Ray Direction: " << raycaster.direction.x << ", " << raycaster.direction.y << ", " << raycaster.direction.z << std::endl;

        // Update the ray's vertex buffer for visualization
        glm::vec3 rayEnd = raycaster.origin + raycaster.direction * 10.0f; // Extend the ray

        float rayVertices[] = {
            raycaster.origin.x, raycaster.origin.y, raycaster.origin.z,
            rayEnd.x, rayEnd.y, rayEnd.z
        };

        glBindBuffer(GL_ARRAY_BUFFER, rayVBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(rayVertices), rayVertices);
    }
}

int main() {
    // Initialize GLFW
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Lighting and Cube with Raycaster", NULL, NULL);
    if (window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    glViewport(0, 0, WIDTH, HEIGHT);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);

    glEnable(GL_DEPTH_TEST);

    Shader lightingShader("vertex_shader.glsl", "fragment_shader.glsl");
    Shader rayShader("ray_vertex_shader.glsl", "ray_fragment_shader.glsl"); // Shader for ray rendering

    
        
    float vertices[] = {

        // Back face
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
         0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,

        // Front face
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
         0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,

        // Left face
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,

        // Right face
         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,

         // Bottom face
         -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
          0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
          0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
          0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
         -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
         -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,

         // Top face
         -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
          0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
          0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
          0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
         -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
         -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f
    };

    unsigned int VAO, VBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // Ray VAO/VBO setup
    glGenVertexArrays(1, &rayVAO);
    glGenBuffers(1, &rayVBO);

    glBindVertexArray(rayVAO);
    glBindBuffer(GL_ARRAY_BUFFER, rayVBO);
    glBufferData(GL_ARRAY_BUFFER, 6 * sizeof(float), NULL, GL_DYNAMIC_DRAW); // Dynamic as ray vertices change
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glEnable(GL_DEPTH_TEST);

    // Load 3D model
    Model myModel("Resources/Models/Chicken/source/Chicken.glb");


    // Transformations for the model
    glm::vec3 modelPosition(2.0f, -1.0f, 0.0f);
    glm::vec3 modelScale(0.5f, 0.5f, 0.5f);
    glm::vec3 modelRotation(0.0f, 0.0f, 0.0f);

    // Transformations for the cube
    glm::vec3 cubePosition(0.0f, -1.0f, 0.0f);
    glm::vec3 cubeScale(1.0f, 1.0f, 1.0f);
    glm::vec3 cubeRotation(0.0f, 0.0f, 0.0f);

    while (!glfwWindowShouldClose(window)) {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window);

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Transformation for the cube
        glm::mat4 cubeModel = glm::mat4(1.0f); // Identity matrix for the cube (no transformation)
        cubeModel = glm::translate(cubeModel, cubePosition);
        lightingShader.setMat4("model", cubeModel);  // Pass cube transformation to shader
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        // Render ray
        rayShader.use();
        glm::mat4 view = camera.GetViewMatrix();
        glm::mat4 projection = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f);
        rayShader.setMat4("view", view);
        rayShader.setMat4("projection", projection);
        rayShader.setVec3("color", glm::vec3(1.0f, 0.0f, 0.0f)); // Red ray


        lightingShader.use();
        lightingShader.setVec3("lightPos", lightPos);
        lightingShader.setVec3("viewPos", camera.Position);
        lightingShader.setVec3("lightColor", glm::vec3(lightIntensity));
        lightingShader.setVec3("objectColor", glm::vec3(1.0f, 0.5f, 0.31f));

        glm::mat4 lightModel = glm::mat4(1.0f);


        lightingShader.setMat4("model", lightModel);
        lightingShader.setMat4("view", view);
        lightingShader.setMat4("projection", projection);

        glBindVertexArray(rayVAO);
        glDrawArrays(GL_LINES, 0, 2);


        // Transformation for the chicken model
        modelRotation.y =  50.0f; // Rotate model over time
        //modelRotation.y += deltaTime * 50.0f; // Rotate model over time
        glm::mat4 chickenModel = glm::mat4(1.0f); // Identity matrix for the chicken
        chickenModel = glm::translate(chickenModel, modelPosition); // Apply translation
        chickenModel = glm::scale(chickenModel, modelScale); // Apply scaling
        chickenModel = glm::rotate(chickenModel, glm::radians(modelRotation.y), glm::vec3(0.0f, 1.0f, 0.0f)); // Apply rotation

        lightingShader.setMat4("model", chickenModel); // Pass chicken transformation to shader
        myModel.Draw(lightingShader, chickenModel); // Draw the chicken




        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    //myModel.cleanUp();
    glDeleteVertexArrays(1, &rayVAO);
    glDeleteBuffers(1, &rayVBO);

    glfwTerminate();
    return 0;
}
