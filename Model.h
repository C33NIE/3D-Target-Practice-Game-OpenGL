
#ifndef MODEL_H
#define MODEL_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <string>
#include <vector>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "Shader.h"
#include "Texture.h"

// Data structure for a single mesh
struct Mesh {
    unsigned int VAO, VBO, EBO;
    std::vector<float> vertices;
    std::vector<unsigned int> indices;
    std::vector<Texture> textures;

    // Clean up OpenGL buffers
    void cleanUp() {
        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);
        glDeleteBuffers(1, &EBO);
    }
};




class Model {
public:
    Model(const std::string& path) {
        loadModel(path);
    }

    void Draw(Shader& shader, const glm::mat4& modelMatrix) {
        for (auto& mesh : meshes) {
            // Bind textures
            unsigned int diffuseCount = 0;
            unsigned int specularCount = 0;
            for (size_t i = 0; i < mesh.textures.size(); ++i) {
                glActiveTexture(GL_TEXTURE0 + i); // Activate the proper texture unit
                std::string name = mesh.textures[i].type;
                std::string number = (name == "texture_diffuse") ? std::to_string(diffuseCount++) : std::to_string(specularCount++);
                shader.setInt(("material." + name + number).c_str(), i); // Bind texture
                glBindTexture(GL_TEXTURE_2D, mesh.textures[i].id);
            }

            // Bind VAO and draw
            glBindVertexArray(mesh.VAO);
            glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(mesh.indices.size()), GL_UNSIGNED_INT, 0);
            glBindVertexArray(0);

            // Reset texture units
            glActiveTexture(GL_TEXTURE0);
        }
    }

    void SetTransform(glm::vec3 position, glm::vec3 scale, glm::vec3 rotation) {
        this->position = position;
        this->scale = scale;
        this->rotation = rotation;
    }

    glm::mat4 GetModelMatrix() const {
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, position);
        model = glm::rotate(model, glm::radians(rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
        model = glm::rotate(model, glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::rotate(model, glm::radians(rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
        model = glm::scale(model, scale);
        return model;
    }


private:
    std::vector<Mesh> meshes;
    glm::vec3 position = glm::vec3(0.0f);
    glm::vec3 scale = glm::vec3(1.0f);
    glm::vec3 rotation = glm::vec3(0.0f);

    void loadModel(const std::string& path) {
        Assimp::Importer importer;
        const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);

        if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
            std::cerr << "ERROR::ASSIMP:: " << importer.GetErrorString() << std::endl;
            return;
        }
        processNode(scene->mRootNode, scene);
    }

    void processNode(aiNode* node, const aiScene* scene) {
        for (unsigned int i = 0; i < node->mNumMeshes; i++) {
            aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
            meshes.push_back(processMesh(mesh, scene));
        }
        for (unsigned int i = 0; i < node->mNumChildren; i++) {
            processNode(node->mChildren[i], scene);
        }
    }

    Mesh processMesh(aiMesh* mesh, const aiScene* scene) {
        Mesh m;

        for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
            // Vertex positions
            m.vertices.push_back(mesh->mVertices[i].x);
            m.vertices.push_back(mesh->mVertices[i].y);
            m.vertices.push_back(mesh->mVertices[i].z);

            // Normals
            m.vertices.push_back(mesh->mNormals[i].x);
            m.vertices.push_back(mesh->mNormals[i].y);
            m.vertices.push_back(mesh->mNormals[i].z);

            // Texture coordinates
            if (mesh->mTextureCoords[0]) {
                m.vertices.push_back(mesh->mTextureCoords[0][i].x);
                m.vertices.push_back(mesh->mTextureCoords[0][i].y);
            }
            else {
                m.vertices.push_back(0.0f);
                m.vertices.push_back(0.0f);
            }
        }

        // Indices
        for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
            aiFace face = mesh->mFaces[i];
            for (unsigned int j = 0; j < face.mNumIndices; j++) {
                m.indices.push_back(face.mIndices[j]);
            }
        }

        // Textures
        if (mesh->mMaterialIndex >= 0) {
            aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
            std::vector<Texture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
            m.textures.insert(m.textures.end(), diffuseMaps.begin(), diffuseMaps.end());
            std::vector<Texture> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
            m.textures.insert(m.textures.end(), specularMaps.begin(), specularMaps.end());
        }

        setupMesh(m);
        return m;
    }

    void setupMesh(Mesh& mesh) {
        glGenVertexArrays(1, &mesh.VAO);
        glGenBuffers(1, &mesh.VBO);
        glGenBuffers(1, &mesh.EBO);

        glBindVertexArray(mesh.VAO);

        glBindBuffer(GL_ARRAY_BUFFER, mesh.VBO);
        glBufferData(GL_ARRAY_BUFFER, mesh.vertices.size() * sizeof(float), &mesh.vertices[0], GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh.indices.size() * sizeof(unsigned int), &mesh.indices[0], GL_STATIC_DRAW);

        // Vertex attribute
        glEnableVertexAttribArray(0); // Position
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1); // Normals
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(2); // Texture coords
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));

        glBindVertexArray(0);
    }

    std::vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, const std::string& typeName) {
        std::vector<Texture> textures;
        for (unsigned int i = 0; i < mat->GetTextureCount(type); i++) {
            aiString str;
            mat->GetTexture(type, i, &str);
            Texture texture(str.C_Str(), typeName);
            textures.push_back(texture);
        }
        return textures;
    }
};

#endif
