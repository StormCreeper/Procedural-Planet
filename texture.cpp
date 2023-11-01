/*
    texture.cpp
    author: Telo PHILIPPE

    Implementation of the Texture class.
*/

#include "gl_includes.hpp"

#include "texture.hpp"

#include <iostream>


#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

void Texture::load(const std::string& filename) {

    if(m_textureID)
        glDeleteTextures(1, &m_textureID);

    int width, height, numComponents;
    // Loading the image in CPU memory using stb_image
    unsigned char *data = stbi_load(
        filename.c_str(),
        &width, &height,
        &numComponents,  // 1 for a 8 bit grey-scale image, 3 for 24bits RGB image, 4 for 32bits RGBA image
        0);

    if(!data) {
        std::cout << "Could load file " << filename << "\n";
        exit(-1);
    }

    glGenTextures(1, &m_textureID); // generate an OpenGL texture container
    glBindTexture(GL_TEXTURE_2D, m_textureID); // activate the texture
    // Setup the texture filtering option and repeat mode; check www.opengl.org for details.
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // Fill the GPU texture with the data stored in the CPU image
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);

    // Free useless CPU memory
    stbi_image_free(data);
    glBindTexture(GL_TEXTURE_2D, 0);  // unbind the texture
}

void Texture::bind() const {
    glBindTexture(GL_TEXTURE_2D, m_textureID);
}

void Texture::unbind() const {
    glBindTexture(GL_TEXTURE_2D, 0);
}

Texture::~Texture() {
    if(m_textureID)
        glDeleteTextures(1, &m_textureID);
}
