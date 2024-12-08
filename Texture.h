#pragma once
#include <glad/glad.h>
#include <string>
#include "stb_image.h"

class Texture {
public:
    unsigned int id;
    std::string type;
    std::string path;
    
    Texture(const char* texPath, const std::string& directory, std::string typeName) {
        type = typeName;
        path = texPath;
        
        glGenTextures(1, &id);
        glBindTexture(GL_TEXTURE_2D, id);
        
        // Set texture parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        
        std::string filename = directory + '/' + std::string(texPath);
        
        int width, height, nrChannels;
        stbi_set_flip_vertically_on_load(true);
        unsigned char* data = stbi_load(filename.c_str(), &width, &height, &nrChannels, 0);
        
        if (data) {
            GLenum format;
            if (nrChannels == 1)
                format = GL_RED;
            else if (nrChannels == 3)
                format = GL_RGB;
            else if (nrChannels == 4)
                format = GL_RGBA;
            
            glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(GL_TEXTURE_2D);
        }
        else {
            std::cout << "Failed to load texture: " << filename << std::endl;
        }
        stbi_image_free(data);
    }
};