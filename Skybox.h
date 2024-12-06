#ifndef SKYBOX_H
#define SKYBOX_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <vector>
#include <string>

class Skybox {
public:
    Skybox(const std::vector<std::string>& faces);
    ~Skybox();

    void Draw(const glm::mat4& view, const glm::mat4& projection);

private:
    unsigned int VAO, VBO;
    unsigned int textureID;
    unsigned int shaderProgram;

    void setupSkybox();
    void createShader();
    unsigned int loadCubemap(const std::vector<std::string>& faces);
};

#endif