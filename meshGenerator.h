#pragma once

#ifndef  meshGenerator_H
#define meshGenerator_H

#include <vector>
#include <glm/glm.hpp>
#include <glad/glad.h>

class MeshGen {
public:
    unsigned int VAO, VBO, EBO;
    std::vector<float> vertices;
    std::vector<unsigned int> indices;

    MeshGen(const std::vector<float>& vertices, const std::vector<unsigned int>& indices);
    ~MeshGen();

    void render() const;

    glm::vec3 getMinBounds() const;
    glm::vec3 getMaxBounds() const;

private:
    void setupMesh();
};

class MeshGenerator {
public:
    static MeshGen generatePlane(float width, float height, const glm::vec3& position);
    static MeshGen generateCube(float width, float height, float depth, const glm::vec3& position);


};

#endif