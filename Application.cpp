#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include "Camera.h"
#include "Shader.h"
#include "Raycaster.h"
#include "meshGenerator.h"
#include "Light.h"
#include "Skybox.h"
#include "Model.h"

// Create a Camera object
Camera camera(glm::vec3(0.0f, 1.0f, 3.0f), glm::vec3(0.0f, 1.0f, 0.0f), -90.0f, 0.0f);

// Time tracking variables for smooth movement
float deltaTime = 0.0f;
float lastFrame = 0.0f;

//gravity
float gravity = 0.98f;
bool onGround = true;

// Mouse tracking variables
float lastX = 400, lastY = 300;
bool firstMouse = true;

// Light properties
glm::vec3 lightPos(2.0f, 1.0f, 1.0f); // Initial light position
float lightIntensity = 0.0002f;          // Light brightness

// Raycaster instance
Raycaster raycaster;

// VAO for the ray line
unsigned int rayVAO, rayVBO;

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    static float lastX = 400.0f, lastY = 300.0f;
    static bool firstMouse = true;

    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xOffset = xpos - lastX;
    float yOffset = lastY - ypos; // Reversed since y-coordinates go bottom to top

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xOffset, yOffset);
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


    // Jump
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS && onGround) {
        camera.Position.y += 0.5f; // Initial jump velocity
        onGround = false;
    }
}

void applyGravity() {
    if (!onGround) {
        camera.Position.y -= gravity * deltaTime;
        if (camera.Position.y <= 0.99f) { // Assume ground plane is y = -1
            camera.Position.y = 1.1f;
            onGround = true;
        }
    }
}


// Callback for mouse clicks to calculate and visualize the ray
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        double xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos);
        raycaster.shootFromCamera(camera);

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
    if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
    {
        std::cout << "Right mouse button clicked!" << std::endl;
    }
}

// In your render loop
void renderScene(unsigned int shaderProgram, Model myModel) {
    // Create transformation matrix
    glm::mat4 model = glm::mat4(1.0f);

    // Apply transformations
    myModel.setTransform(glm::mat4(1.0f)); // Reset transform
    myModel.translate(glm::vec3(0.0f, 0.0f, 0.0f));
    myModel.rotate(45.0f, glm::vec3(0.0f, 1.0f, 0.0f));
    myModel.scale(glm::vec3(1.0f, 1.0f, 1.0f));

    // Send model matrix to shader
    unsigned int modelLoc = glGetUniformLocation(shaderProgram, "model");
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(myModel.getModelMatrix()));

    // Draw the model
    myModel.Draw(shaderProgram);
}

void setLightingUniforms(unsigned int shaderProgram) {
    // Simulate sun position (high up and slightly angled)
    glm::vec3 lightPos(-1000.0f, 1000.0f, -250.0f); // Far away to simulate directional light

    // Sunlight color (slightly warm white)
    glm::vec3 sunColor(1.0f, 0.98f, 0.95f);

    // Light components
    glm::vec3 lightAmbient = sunColor * glm::vec3(0.1f);  // Ambient is low, simulating indirect sky light
    glm::vec3 lightDiffuse = sunColor * glm::vec3(1.0f);  // Strong direct sunlight
    glm::vec3 lightSpecular = sunColor * glm::vec3(0.4); // Slightly reduced specular to avoid too much glare

    glUniform3fv(glGetUniformLocation(shaderProgram, "light.position"), 1, glm::value_ptr(lightPos));
    glUniform3fv(glGetUniformLocation(shaderProgram, "light.ambient"), 1, glm::value_ptr(lightAmbient));
    glUniform3fv(glGetUniformLocation(shaderProgram, "light.diffuse"), 1, glm::value_ptr(lightDiffuse));
    glUniform3fv(glGetUniformLocation(shaderProgram, "light.specular"), 1, glm::value_ptr(lightSpecular));

    // Material properties
    float shininess = 40.0f; // Higher shininess for sharper highlights, like you'd see in daylight
    glUniform1f(glGetUniformLocation(shaderProgram, "material.shininess"), shininess);

    // View position (camera position)
    glUniform3fv(glGetUniformLocation(shaderProgram, "viewPos"), 1, glm::value_ptr(camera.Position));
}

int main() {
    // Initialize GLFW
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);


    GLFWwindow* window = glfwCreateWindow(1350, 1080, "Target Practice", NULL, NULL);
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

    glViewport(0, 0, 1350, 1080);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);

    // Prepare skybox
    std::vector<std::string> skyboxFaces{
        "Assets/skybox/px.png",
        "Assets/skybox/nx.png",
        "Assets/skybox/py.png",
        "Assets/skybox/ny.png",
        "Assets/skybox/pz.png",
        "Assets/skybox/nz.png"
    };
    Skybox skybox(skyboxFaces);

    glEnable(GL_DEPTH_TEST);

    Shader lightingShader("vertex_shader.glsl", "lighting.glsl");
    Shader rayShader("ray_vertex_shader.glsl", "ray_fragment_shader.glsl", "ray_geometry_shader.glsl"); // Include geometry shader
    Shader ObjectShader("vertex_shader.glsl", "fragment_shader.glsl");
    Shader modelShader("model_vertex.glsl", "model_fragment.glsl");

    // After creating shader program
    GLint isLinked;
    glGetProgramiv(modelShader.ID, GL_LINK_STATUS, &isLinked);
    if (!isLinked) {
        GLint maxLength;
        glGetProgramiv(modelShader.ID, GL_INFO_LOG_LENGTH, &maxLength);
        std::vector<GLchar> infoLog(maxLength);
        glGetProgramInfoLog(modelShader.ID, maxLength, &maxLength, &infoLog[0]);
        std::cout << "Shader linking error: " << std::string(infoLog.begin(), infoLog.end()) << std::endl;
    }

    // Print all active uniforms
    GLint numUniforms;
    glGetProgramiv(modelShader.ID, GL_ACTIVE_UNIFORMS, &numUniforms);
    for (GLint i = 0; i < numUniforms; ++i) {
        GLint size; GLenum type;
        GLchar name[128];
        glGetActiveUniform(modelShader.ID, i, sizeof(name) - 1, nullptr, &size, &type, name);
        std::cout << "Active uniform " << i << ": " << name << std::endl;
    }


    // Generate a plane at position (2.0f, 0.0f, 0.0f)
    MeshGen plane = MeshGenerator::generatePlane(20.f, 20.f, glm::vec3(.0f, -1.5f, 0.0f));

    // Generate a cube at position (-2.0f, 1.0f, 0.0f)
    MeshGen cube = MeshGenerator::generateCube(2.0f, 2.0f, 2.0f, glm::vec3(-2.0f, 1.0f, 0.0f));


    // Load 3D model
    Model Guns("Resources/Models/Guns/scene.gltf");
    Model Ground("Resources/Models/Ground/scene.gltf");
    Model Plants("Resources/Models/Plants1/scene.gltf");
    Model Targets("Resources/Models/Target/scene.gltf");
    Model Tower("Resources/Models/Tower/scene.gltf");
    Model Hut("Resources/Models/Hut/scene.gltf");
    //Model Desert("Resources/Models/Desert/scene.gltf");


    
    // Aim Position
    glm::vec3 aimPos(0.0f, 0.0f, 0.0f);

    // Model rotation speed
    float rotationSpeed = 50.0f;




    // Ray VAO/VBO setup
    glGenVertexArrays(1, &rayVAO);
    glGenBuffers(1, &rayVBO);

    glBindVertexArray(rayVAO);
    glBindBuffer(GL_ARRAY_BUFFER, rayVBO);
    glBufferData(GL_ARRAY_BUFFER, 6 * sizeof(float), NULL, GL_DYNAMIC_DRAW); // Dynamic as ray vertices change
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    while (!glfwWindowShouldClose(window)) {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window);
        applyGravity();


        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::mat4 view = camera.GetViewMatrix();
        glm::mat4 projection = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f);


        glm::mat4 model = glm::mat4(1.0f);
        ObjectShader.use();
        ObjectShader.setVec3("objectColor", glm::vec3(0.0f, 0.0f, 1.0f));  // Blue color
        ObjectShader.setMat4("model", model);
        ObjectShader.setMat4("view", view);
        ObjectShader.setMat4("projection", projection);

        //plane.render();
        //cube.render();


        skybox.Draw(view, projection);
        DirectionalLight pointLight(glm::vec3(0.f, .0f, .0f), glm::vec3(0.5f, 1.0f, 1.0f), lightIntensity);

        // Apply lights in the render loop
        lightingShader.use();
        pointLight.apply(lightingShader, "pointLight");



        // Render ray
        rayShader.use();
        rayShader.setMat4("view", camera.GetViewMatrix());
        rayShader.setMat4("projection", glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f));
        rayShader.setFloat("thickness", 0.5f); // Adjust thickness as needed
        glBindVertexArray(rayVAO);
        glDrawArrays(GL_LINES, 0, 2);

        modelShader.use();
        modelShader.setMat4("projection", projection);
        modelShader.setMat4("view", camera.GetViewMatrix());
        modelShader.setVec3("viewPos", camera.Position);

        // Set lighting uniforms
        modelShader.setVec3("light.position", lightPos);
        modelShader.setVec3("light.ambient", glm::vec3(0.2f));
        modelShader.setVec3("light.diffuse", glm::vec3(0.5f));
        modelShader.setVec3("light.specular", glm::vec3(1.0f));
        setLightingUniforms(modelShader.ID);
        renderScene(modelShader.ID, Guns);
        renderScene(modelShader.ID, Ground);
        renderScene(modelShader.ID, Plants);
        renderScene(modelShader.ID, Targets);
        renderScene(modelShader.ID, Tower);
        renderScene(modelShader.ID, Hut);
        //renderScene(modelShader.ID, Desert);
        


        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &rayVAO);
    glDeleteBuffers(1, &rayVBO);

    glfwTerminate();
    return 0;
}
