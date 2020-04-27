#include "texture.h"

#include <math.h>
#include <stdio.h>

#include <SOIL.h>

Texture::Texture() {
    glGenTextures(1, &this->m_texture);
}

Texture::~Texture() {
    glDeleteTextures(1, &this->m_texture);
}

int Texture::load(const char* file) {
    this->m_texture = SOIL_load_OGL_texture(
        file,
        SOIL_LOAD_AUTO,
        SOIL_CREATE_NEW_ID,
        SOIL_FLAG_MIPMAPS | SOIL_FLAG_TEXTURE_REPEATS | SOIL_FLAG_INVERT_Y
    );

    if(!this->m_texture) {
	    printf("[Texture Error]\t%s\n", SOIL_last_result());
        return -1;
    }
    return 0;
}

void Texture::set_sampling(GLenum wrap, GLenum filter) {
    glBindTexture(GL_TEXTURE_2D, this->m_texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture::bind(unsigned int slot) {
    glActiveTexture(GL_TEXTURE0 + slot);
    glBindTexture(GL_TEXTURE_2D, this->m_texture);
}