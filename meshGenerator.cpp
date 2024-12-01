// mesh_generator.cpp
#include "meshGenerator.h"
#include "glm/glm.hpp"
#include <iostream>

Mesh::Mesh(const std::vector<float>& vertices, const std::vector<unsigned int>& indices)
    : vertices(vertices), indices(indices) {
    setupMesh();
}

Mesh::~Mesh() {
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
}

void Mesh::setupMesh() {
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

    // Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Texture coordinates attribute
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);
}

void Mesh::render() const {
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

Mesh MeshGenerator::generatePlane(float width, float height, const glm::vec3& position) {
    std::vector<float> vertices = {
        // positions              // texCoords
        -width / 2 + position.x, 0.0f + position.y, -height / 2 + position.z, 0.0f, 0.0f,
         width / 2 + position.x, 0.0f + position.y, -height / 2 + position.z, 1.0f, 0.0f,
         width / 2 + position.x, 0.0f + position.y,  height / 2 + position.z, 1.0f, 1.0f,
        -width / 2 + position.x, 0.0f + position.y,  height / 2 + position.z, 0.0f, 1.0f
    };

    std::vector<unsigned int> indices = {
        0, 1, 2,
        2, 3, 0
    };

    return Mesh(vertices, indices);
}

Mesh MeshGenerator::generateCube(float width, float height, float depth, const glm::vec3& position) {
    std::vector<float> vertices = {
        // positions               // texCoords
        -width / 2 + position.x, -height / 2 + position.y, -depth / 2 + position.z, 0.0f, 0.0f,
         width / 2 + position.x, -height / 2 + position.y, -depth / 2 + position.z, 1.0f, 0.0f,
         width / 2 + position.x,  height / 2 + position.y, -depth / 2 + position.z, 1.0f, 1.0f,
        -width / 2 + position.x,  height / 2 + position.y, -depth / 2 + position.z, 0.0f, 1.0f,
        -width / 2 + position.x, -height / 2 + position.y,  depth / 2 + position.z, 0.0f, 0.0f,
         width / 2 + position.x, -height / 2 + position.y,  depth / 2 + position.z, 1.0f, 0.0f,
         width / 2 + position.x,  height / 2 + position.y,  depth / 2 + position.z, 1.0f, 1.0f,
        -width / 2 + position.x,  height / 2 + position.y,  depth / 2 + position.z, 0.0f, 1.0f
    };

    std::vector<unsigned int> indices = {
        // back face
        0, 1, 2, 2, 3, 0,
        // front face
        4, 5, 6, 6, 7, 4,
        // left face
        0, 3, 7, 7, 4, 0,
        // right face
        1, 5, 6, 6, 2, 1,
        // bottom face
        0, 1, 5, 5, 4, 0,
        // top face
        3, 2, 6, 6, 7, 3
    };

    return Mesh(vertices, indices);
}

glm::vec3 Mesh::getMinBounds() const {
    float minX = std::numeric_limits<float>::max();
    float minY = std::numeric_limits<float>::max();
    float minZ = std::numeric_limits<float>::max();

    for (size_t i = 0; i < vertices.size(); i += 5) { // Step by 5 for each vertex
        minX = std::min(minX, vertices[i]);
        minY = std::min(minY, vertices[i + 1]);
        minZ = std::min(minZ, vertices[i + 2]);
    }

    return glm::vec3(minX, minY, minZ);
}

glm::vec3 Mesh::getMaxBounds() const {
    float maxX = std::numeric_limits<float>::lowest();
    float maxY = std::numeric_limits<float>::lowest();
    float maxZ = std::numeric_limits<float>::lowest();

    for (size_t i = 0; i < vertices.size(); i += 5) { // Step by 5 for each vertex
        maxX = std::max(maxX, vertices[i]);
        maxY = std::max(maxY, vertices[i + 1]);
        maxZ = std::max(maxZ, vertices[i + 2]);
    }

    return glm::vec3(maxX, maxY, maxZ);
}

