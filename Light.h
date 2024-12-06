#ifndef LIGHT_H
#define LIGHT_H

#include <glm/glm.hpp>
#include <string>
#include "Shader.h"

class Light {
public:
    glm::vec3 position;
    glm::vec3 color;
    float intensity;

    Light(const glm::vec3& position, const glm::vec3& color, float intensity)
        : position(position), color(color), intensity(intensity) {}

    virtual void apply(Shader& shader, const std::string& uniformName) = 0;
};

class PointLight : public Light {
public:
    float constant;
    float linear;
    float quadratic;

    PointLight(const glm::vec3& position, const glm::vec3& color, float intensity,
        float constant, float linear, float quadratic)
        : Light(position, color, intensity),
        constant(constant), linear(linear), quadratic(quadratic) {}

    void apply(Shader& shader, const std::string& uniformName) override;
};

class DirectionalLight : public Light {
public:
    glm::vec3 direction;

    DirectionalLight(const glm::vec3& direction, const glm::vec3& color, float intensity)
        : Light(glm::vec3(0.0f), color, intensity), direction(direction) {}

    void apply(Shader& shader, const std::string& uniformName) override;
};

class SpotLight : public Light {
public:
    glm::vec3 direction;
    float cutOff;
    float outerCutOff;

    SpotLight(const glm::vec3& position, const glm::vec3& direction, const glm::vec3& color,
        float intensity, float cutOff, float outerCutOff)
        : Light(position, color, intensity),
        direction(direction), cutOff(cutOff), outerCutOff(outerCutOff) {}

    void apply(Shader& shader, const std::string& uniformName) override;
};

#endif
