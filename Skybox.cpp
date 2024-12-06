#include "skybox.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <iostream>

Skybox::Skybox(const std::vector<std::string>& faces) {
    setupSkybox();
    createShader();
    textureID = loadCubemap(faces);
}

Skybox::~Skybox() {
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(shaderProgram);
    glDeleteTextures(1, &textureID);
}

void Skybox::setupSkybox() {
    float skyboxVertices[] = {
        // Positions          
        // Front face
        -1.0f,  1.0f, -1.0f, // Top-left
        -1.0f, -1.0f, -1.0f, // Bottom-left
         1.0f, -1.0f, -1.0f, // Bottom-right
         1.0f, -1.0f, -1.0f, // Bottom-right
         1.0f,  1.0f, -1.0f, // Top-right
        -1.0f,  1.0f, -1.0f, // Top-left

        // Back face
        -1.0f, -1.0f,  1.0f, // Bottom-left
        -1.0f,  1.0f,  1.0f, // Top-left
         1.0f,  1.0f,  1.0f, // Top-right
         1.0f,  1.0f,  1.0f, // Top-right
         1.0f, -1.0f,  1.0f, // Bottom-right
        -1.0f, -1.0f,  1.0f, // Bottom-left

        // Left face
        -1.0f,  1.0f,  1.0f, // Top-right
        -1.0f,  1.0f, -1.0f, // Top-left
        -1.0f, -1.0f, -1.0f, // Bottom-left
        -1.0f, -1.0f, -1.0f, // Bottom-left
        -1.0f, -1.0f,  1.0f, // Bottom-right
        -1.0f,  1.0f,  1.0f, // Top-right

        // Right face
         1.0f,  1.0f,  1.0f, // Top-left
         1.0f, -1.0f, -1.0f, // Bottom-right
         1.0f,  1.0f, -1.0f, // Top-right
         1.0f, -1.0f, -1.0f, // Bottom-right
         1.0f,  1.0f,  1.0f, // Top-left
         1.0f, -1.0f,  1.0f, // Bottom-left

         // Bottom face
         -1.0f, -1.0f, -1.0f, // Top-right
          1.0f, -1.0f, -1.0f, // Top-left
          1.0f, -1.0f,  1.0f, // Bottom-left
          1.0f, -1.0f,  1.0f, // Bottom-left
         -1.0f, -1.0f,  1.0f, // Bottom-right
         -1.0f, -1.0f, -1.0f, // Top-right

       -1.0f,  1.0f, -1.0f, // Top-left
        1.0f,  1.0f, -1.0f, // Top-right
        1.0f,  1.0f,  1.0f, // Bottom-right
        1.0f,  1.0f,  1.0f, // Bottom-right
       -1.0f,  1.0f,  1.0f, // Bottom-left
       -1.0f,  1.0f, -1.0f  // Top-left
    };

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glBindVertexArray(0);
}

void Skybox::createShader() {
    // Vertex Shader
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    const char* vertexShaderSource = "#version 330 core\n"
        "layout (location = 0) in vec3 aPos;\n"
        "out vec3 TexCoords;\n"
        "uniform mat4 projection;\n"
        "uniform mat4 view;\n"
        "void main()\n"
        "{\n"
        "   TexCoords = aPos;\n"
        "   vec4 pos = projection * view * vec4(aPos, 1.0);\n"
        "   gl_Position = pos.xyww;\n"
        "}\0";
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    // Fragment Shader
    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    const char* fragmentShaderSource = "#version 330 core\n"
        "in vec3 TexCoords;\n"
        "out vec4 FragColor;\n"
        "uniform samplerCube skybox;\n"
        "void main()\n"
        "{\n"
        "   FragColor = texture(skybox, TexCoords);\n"
        "}\0";
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    // Shader Program
    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    // Error checking
    int success;
    char infoLog[512];
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }

    // Clean up shaders
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
}

unsigned int Skybox::loadCubemap(const std::vector<std::string>& faces) {
    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    int width, height, nrChannels;
    for (unsigned int i = 0; i < faces.size(); i++) {
        unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
        if (data) {
            GLenum format = GL_RGB;
            if (nrChannels == 1)
                format = GL_RED;
            else if (nrChannels == 3)
                format = GL_RGB;
            else if (nrChannels == 4)
                format = GL_RGBA;

            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
            stbi_image_free(data);
        }
        else {
            std::cout << "Cubemap tex failed to load at path: " << faces[i] << std::endl;
        }
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    return textureID;
}

void Skybox::Draw(const glm::mat4& view, const glm::mat4& projection) {
    // Disable depth writing
    glDepthMask(GL_FALSE);
    glDepthFunc(GL_LEQUAL);

    // Use skybox shader
    glUseProgram(shaderProgram);

    // Remove translation from view matrix
    glm::mat4 viewNoTranslation = glm::mat4(glm::mat3(view));

    // Set uniforms
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(viewNoTranslation));
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

    // Bind cubemap
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
    glUniform1i(glGetUniformLocation(shaderProgram, "skybox"), 0);

    // Render skybox
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);

    // Restore depth settings
    glDepthMask(GL_TRUE);
    glDepthFunc(GL_LESS);
}