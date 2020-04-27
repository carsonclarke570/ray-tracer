#include "shader.h"

#include <fstream>
#include <string>

Shader::Shader() {
    this->program = 0;
    this->shaders[VERTEX] = 0;
    this->shaders[FRAGMENT] = 0;
    this->shaders[GEOMETRY] = 0;
    this->shaders[COMPUTE] = 0;
}

Shader::~Shader() {
    glDeleteProgram(this->program);
    this->program = 0;
}

int Shader::load_text(enum ShaderType type, const char *src) {
    // Create shader
    GLenum gl_type;
    switch(type) {
        case VERTEX:
            gl_type = GL_VERTEX_SHADER;
            break;
        case GEOMETRY:
            gl_type = GL_GEOMETRY_SHADER;
            break;
        case FRAGMENT:
            gl_type = GL_FRAGMENT_SHADER;
            break;
        case COMPUTE:
            gl_type = GL_COMPUTE_SHADER;
            break;
        default:
            fprintf(stderr, "ERROR: Invalid ShaderType!\n");
            return -1;
    }

    GLuint gl_shader = glCreateShader(gl_type);
    glShaderSource(gl_shader, 1, &src, NULL);
    

    // Compile + error check
    int status;
    glCompileShader(gl_shader);
    glGetShaderiv(gl_shader, GL_COMPILE_STATUS, &status);
    if (!status) {
        // Get error and print
        int32_t log_size;
        glGetShaderiv(gl_shader, GL_INFO_LOG_LENGTH, &log_size);
        char* log = (char*) malloc(log_size * sizeof(char));
        glGetShaderInfoLog(gl_shader, log_size, NULL, log);
        fprintf(stderr, "ERROR: Failed to compile shader! Message follows:\n%s", log);

        // Clean up
        free(log);
        glDeleteShader(gl_shader);

        //return code
        return -1;
    }

    this->shaders[type] = gl_shader;
    return 0;
}

int Shader::load_file(enum ShaderType type, const char* file) {
    std::ifstream ifs(file);
    std::string content( 
        (std::istreambuf_iterator<char>(ifs)),
        (std::istreambuf_iterator<char>())
    );

    return this->load_text(type, content.c_str());
}

int Shader::compile() {
    // Create and attach shaders
    this->program = glCreateProgram();
    for (int i = 0; i < 4; i ++) {
        if (this->shaders[i]) {
            glAttachShader(this->program, this->shaders[i]);
        }
    }

    // Link
    int32_t status;
    glLinkProgram(this->program);
    glGetProgramiv(this->program, GL_LINK_STATUS, &status);
    if (!status) {
        // Get error and print
        int32_t log_size;
        glGetProgramiv(this->program, GL_INFO_LOG_LENGTH, &log_size);
        char* log = (char*) malloc(log_size * sizeof(char));
        glGetProgramInfoLog(this->program, log_size, NULL, log);
        fprintf(stderr, "ERROR: Failed to link shader! Message follows:\n%s", log);

        // Clean up
        free(log);
        glDeleteProgram(this->program);

        //return code
        return -1;
    }

    for (int i = 0; i < 4; i ++) {
        if (this->shaders[i]) {
            glDetachShader(this->program, this->shaders[i]);
            glDeleteShader(this->shaders[i]);
        }
    }
    return 0;
}

// void Shader::uniform_mat4(const char* name, const mat4 data) {
//     GLuint loc = glGetUniformLocation(this->program, name);
//     glUniformMatrix4fv(loc, 1, GL_FALSE, data);
// }

// void Shader::uniform_vec2(const char* name, const vec2 data) {
//     GLuint loc = glGetUniformLocation(this->program, name);
//     glUniform2fv(loc, 1, data);
// }

void Shader::uniform_vec3(const char* name, const vec3 data) {
    GLuint loc = glGetUniformLocation(this->program, name);
    glUniform3fv(loc, 1, data.e);
}

void Shader::uniform_float(const char* name, float data) {
    GLuint loc = glGetUniformLocation(this->program, name);
    glUniform1f(loc, data);
}

void Shader::uniform_int(const char* name, int data) {
    GLuint loc = glGetUniformLocation(this->program, name);
    glUniform1i(loc, data);
}

void Shader::bind_ubo(const char* name, uint32_t slot) {
    GLuint loc = glGetUniformBlockIndex(this->program, name);
    glUniformBlockBinding(this->program, loc, slot);
}

void Shader::bind() {
    glUseProgram(this->program);
}

void Shader::unbind() {
    glUseProgram(0);
}
