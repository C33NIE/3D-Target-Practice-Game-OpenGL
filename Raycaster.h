#ifndef RAYCASTER_H
#define RAYCASTER_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Camera.h" // Ensure this is included to access the camera object

class Raycaster {
public:
    glm::vec3 origin;
    glm::vec3 direction;

    Raycaster() : origin(glm::vec3(0.0f)), direction(glm::vec3(0.0f)) {}

    void shootFromCamera(const Camera& camera) {
        // The direction should be based on the camera's front vector
        direction = glm::normalize(camera.Front);  // Camera's forward vector

        // The origin should be the camera's position
        origin = camera.Position;
    }

};

#endif
