#ifndef TEXTURE_H
#define TEXTURE_H

#include <glad/glad.h>
#include <string>
#include <iostream>
#define STB_IMAGE_IMPLEMENTATION
#include <STB_Image/stb_image.h>

class Texture {
public:
    unsigned int id;
    std::string type;
    std::string path;

    Texture(const std::string& file, const std::string& textureType) {
        type = textureType;
        path = file;
        id = loadTexture(file);
    }

    void Bind(unsigned int unit) const {
        glActiveTexture(GL_TEXTURE0 + unit);
        glBindTexture(GL_TEXTURE_2D, id);
    }

private:
    unsigned int loadTexture(const std::string& file) {
        unsigned int textureID;
        glGenTextures(1, &textureID);

        int width, height, nrChannels;
        unsigned char* data = stbi_load(file.c_str(), &width, &height, &nrChannels, 0);
        if (data) {
            GLenum format = (nrChannels == 4) ? GL_RGBA : GL_RGB;
            glBindTexture(GL_TEXTURE_2D, textureID);
            glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(GL_TEXTURE_2D);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        }
        else {
            std::cerr << "Failed to load texture: " << file << std::endl;
        }
        stbi_image_free(data);

        return textureID;
    }
};

#endif
