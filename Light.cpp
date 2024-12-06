#include "Light.h"

void PointLight::apply(Shader& shader, const std::string& uniformName) {
    shader.setVec3(uniformName + ".position", position);
    shader.setVec3(uniformName + ".color", color * intensity);
    shader.setFloat(uniformName + ".constant", constant);
    shader.setFloat(uniformName + ".linear", linear);
    shader.setFloat(uniformName + ".quadratic", quadratic);
}

void DirectionalLight::apply(Shader& shader, const std::string& uniformName) {
    shader.setVec3(uniformName + ".direction", direction);
    shader.setVec3(uniformName + ".color", color * intensity);
}

void SpotLight::apply(Shader& shader, const std::string& uniformName) {
    shader.setVec3(uniformName + ".position", position);
    shader.setVec3(uniformName + ".direction", direction);
    shader.setVec3(uniformName + ".color", color * intensity);
    shader.setFloat(uniformName + ".cutOff", glm::cos(glm::radians(cutOff)));
    shader.setFloat(uniformName + ".outerCutOff", glm::cos(glm::radians(outerCutOff)));
}
