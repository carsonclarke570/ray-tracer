#ifndef _SHADER_H_
#define _SHADER_H_

#include <stdlib.h>

#include "matrix.h"
#include "texture.h"
#include "vector.h"

enum ShaderType {
    VERTEX = 0, 
    FRAGMENT = 1, 
    GEOMETRY = 2, 
    COMPUTE = 3
};

class Shader {
public:
    GLuint program;
private:
    GLuint shaders[4];
public:

    /**
     * Constructs a new Shader object.
     */
    Shader();

    /**
     * Destroys a Shader object
     */
    ~Shader();

    /**
     * Loads and attaches a shader from text.
     *
     * @param type      Type of shader to attach.
     * @param src       The text of the shader.
     * @return  CODE_SUCCESS if success, else a relevant error code.
     */
    int load_text(enum ShaderType type, const char* src);

    /**
     * Loads and attaches a shader from a file.
     *
     * @param type      Type of shader to attach.
     * @param file      File path of the file to open and read.
     * @return  CODE_SUCCESS if success, else a relevant error code.
     */
    int load_file(enum ShaderType type, const char* file);

    /**
     * Attempts to compiles all attached shaders. To prevent memory
     * leakages, make sure to call dispose() when you're done using the
     * shader.
     *
     * @return  CODE_SUCCESS if success, else a relevant error code.
     */
    int compile();

    /**
     * Updates a shader's uniform.
     *
     * @param name      Name of uniform to update.
     * @param data      data to update with.
     */
    // void uniform_mat4(const char* name, const mat4 data);
    // void uniform_vec2(const char* name, const vec2 data);
    void uniform_vec3(const char* name, const vec3 data);
    void uniform_float(const char* name, float data);
    void uniform_int(const char* name, int data);

    /**
     * Binds a uniform buffer object to a Shader by slot number.
     *
     * @param name      Name of uniform buffer
     * @param slot      Slot where the uniform buffer is bound.
     */
    void bind_ubo(const char* name, uint32_t slot);

    /**
     * Binds the shader by making it the active shader
     */
    void bind();

    /**
     * Unbinds shaders
     */
    void unbind();
};

#endif