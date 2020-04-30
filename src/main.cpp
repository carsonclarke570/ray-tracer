/**
 * @file    main.cpp
 * @author  Carson Clarke-Magrab (ctc7359@rit.edu)
 * @date    2020-04-25
 */

#include <cstdlib>
#include <ctime>
#include <iostream>

#if defined(_WIN32) || defined(_WIN64)
#include <windows.h>
#endif

#ifndef __APPLE__
#include <GL/glew.h>
#endif

#include <GLFW/glfw3.h>

#include "camera.h"
#include "shader.h"

#define FPS_CAP 60.0f

// Window
GLFWwindow *window;
int w_width  = 1024;
int w_height = 512;

// Materials
Camera* c_camera;
Shader s_quad, s_compute;
Texture t_render;

// Callbacks
void glfwError(int code, const char *desc) {
    std::cerr << "GLFW error " << code << ": " << desc << std::endl;
    exit( 2 );
}

void init() {

    // Texture
    t_render = Texture();
    t_render.set_sampling(GL_REPEAT, GL_LINEAR);
    t_render.bind(0);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, w_width, w_height, 0, GL_RGBA, GL_FLOAT, NULL);
    glBindImageTexture(0, t_render.m_texture, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA8);

    // Camera
    float aspect = float(w_width) / float(w_height);
    vec3 target = vec3(0, 0, 0);
    vec3 pos = vec3(-2, 2, 1);
    float focus = (pos - target).length();
    c_camera = new Camera(pos, target, vec3(0, 1, 0), 90.0f, aspect, 0.1f, focus);

    // Quad rendering
    s_quad = Shader();
    s_quad.load_file(VERTEX, "quad.vert");
    s_quad.load_file(FRAGMENT, "quad.frag");
    s_quad.compile();

    // Compute shader
    s_compute = Shader();
    s_compute.load_file(COMPUTE, "raytracer.comp");
    s_compute.compile();

    // Configure shaders
    s_compute.bind();
    s_compute.uniform_int("dest", 0);
    s_compute.uniform_int("samples", 1000);
    s_compute.uniform_int("depth", 3);
    s_compute.uniform_float("width", (float) w_width);
    s_compute.uniform_float("height", (float) w_height);
    c_camera->update_shader(s_compute);


    s_quad.bind();
    s_quad.uniform_int("render_tex", 0);

    // Quad buffer

    GLuint vao;
    glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	GLuint pos_buf;
	glGenBuffers(1, &pos_buf);
    glBindBuffer(GL_ARRAY_BUFFER, pos_buf);
	float data[] = {
		-1.0f, -1.0f,
		-1.0f, 1.0f,
		1.0f, -1.0f,
		1.0f, 1.0f
	};
    glBufferData(GL_ARRAY_BUFFER, sizeof(float)*8, data, GL_STREAM_DRAW);
	GLint posPtr = glGetAttribLocation(s_quad.program, "pos");
    glVertexAttribPointer(posPtr, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(posPtr);
}

void update(double t) {
    float d = t / 10;
    float x = cos(d);
    float z = sin(d);
    float y = 2 + cos(d);
    
    vec3 target = vec3(0, 0, 0);
    vec3 pos = vec3(x, y, z);

    c_camera->update(pos, target, vec3(0, 1, 0));
}

void render() {
     glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Compute Shader
    s_compute.bind();
    c_camera->update_shader(s_compute);
    glDispatchCompute(w_width / 32, w_height / 32, 1);

    // Draw to screen
    s_quad.bind();
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    glfwSwapBuffers(window);
}

int main( int argc, char **argv ) {

    // Initialize GLFW
    glfwSetErrorCallback( glfwError );
    if(!glfwInit()) {
        std::cerr << "Can't initialize GLFW!" << std::endl;
        exit( 1 );
    }

    // Create window
    window = glfwCreateWindow( w_width, w_height, "Final Project", NULL, NULL);
    if(!window) {
        std::cerr << "GLFW window create failed!" << std::endl;
        glfwTerminate();
        exit(1);
    }
    glfwMakeContextCurrent( window );

    GLenum err = glewInit();
    if( err != GLEW_OK ) {
        std::cerr << "GLEW error: " << glewGetErrorString(err) << std::endl;
        glfwTerminate();
        exit( 1 );
    }
    if( !GLEW_VERSION_3_2 ) {
        std::cerr << "GLEW: OpenGL 3.2 not available" << std::endl;
        if( !GLEW_VERSION_2_1 ) {
            std::cerr << "GLEW: OpenGL 2.1 not available, either!" << std::endl;
            glfwTerminate();
            exit( 1 );
        }
    }

    // Determine whether or not we can use GLSL 1.50
    int maj = glfwGetWindowAttrib( window, GLFW_CONTEXT_VERSION_MAJOR );
    int min = glfwGetWindowAttrib( window, GLFW_CONTEXT_VERSION_MINOR );

    std::cerr << "GLFW: using " << maj << "." << min << " context" << std::endl;
    if( maj < 3 || (maj == 3 && min < 2) ) {
        std::cerr << "*** GLSL 1.50 shaders may not compile" << std::endl;
    }

    // Configure OpenGL
    glFrontFace(GL_CCW);
    glCullFace(GL_BACK);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_TEXTURE_2D);
    
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glDepthFunc(GL_LEQUAL);
    glClearDepth(1.0f);

    init();

    time_t last_render = time(NULL);
    time_t last_second = time(NULL);
    time_t start = time(NULL);
    double diff;
    double t = 0.0f;
    int frames = 0;
    while( !glfwWindowShouldClose(window) ) {

        t = difftime(time(NULL), start);
        update(t);

        diff = difftime(time(NULL), last_render);
        if (diff >= (1.0f / FPS_CAP)) {
            render();

            frames++;

            last_render = time(NULL);
        }

        diff = difftime(time(NULL), last_second);
        if (diff >= 1.0f) {
            printf("[FPS] - %d\n", frames);
            frames = 0;
            last_second = time(NULL);
        }
        // Process events
        glfwPollEvents(); 
    }

    delete c_camera;

    glfwDestroyWindow( window );
    glfwTerminate();


    return 0;
}
