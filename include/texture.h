#ifndef _TEXTURE_H_
#define _TEXTURE_H_

#include <stdbool.h>
#include <stdint.h>

#include <GLFW/glfw3.h>
#include <GL/glew.h>

class Texture {
public:
    GLuint m_texture;
public:

    /**
     * Creates a new Texture object. 
     */
    Texture();

    /**
     * Destroy the Texture object
     * 
     */
    ~Texture();

    /**
     * Loads a texture from an image.
     *
     * @param file      File path of the image
     */
    int load(const char* file);

    /**
     * Sets the sampling parameters of the texture.
     *
     * @param wrap      Wrapping settings
     * @param filter    Filter settings
     */
    void set_sampling(GLenum wrap, GLenum filter);

    /**
     * Binds the texture to a specific texture slot.
     *
     * @param id        Number of the texture slot.
     */
    void bind(unsigned int slot);

};

#endif