#ifndef RAYCASTER_H
#define RAYCASTER_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Raycaster {
public:
    glm::vec3 origin;
    glm::vec3 direction;

    Raycaster() : origin(glm::vec3(0.0f)), direction(glm::vec3(0.0f)) {}

    void calculateRay(const glm::vec2& screenCoords, const glm::mat4& projection, const glm::mat4& view, const glm::vec2& viewportSize) {
        // Convert screen coordinates to normalized device coordinates
        float x = (2.0f * screenCoords.x) / viewportSize.x - 1.0f;
        float y = 1.0f - (2.0f * screenCoords.y) / viewportSize.y; // Y is flipped
        glm::vec4 rayNDC(x, y, -1.0f, 1.0f);

        // Convert to clip space
        glm::vec4 rayClip(rayNDC.x, rayNDC.y, -1.0f, 1.0f);

        // Convert to view space
        glm::vec4 rayView = glm::inverse(projection) * rayClip;
        rayView = glm::vec4(rayView.x, rayView.y, -1.0f, 0.0f);

        // Convert to world space
        glm::vec4 rayWorld = glm::inverse(view) * rayView;
        direction = glm::normalize(glm::vec3(rayWorld));
        origin = glm::vec3(glm::inverse(view)[3]);
    }
};

#endif
