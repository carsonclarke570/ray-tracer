/**
 * @file    main.cpp
 * @author  Carson Clarke-Magrab (ctc7359@rit.edu)
 * @date    2020-04-25
 */

#include <cstdlib>
#include <ctime>
#include <iostream>
#include <chrono>
#include <thread>

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
Texture t_gather, t_render, t_mercury;

// Callbacks
void glfwError(int code, const char *desc) {
    std::cerr << "GLFW error " << code << ": " << desc << std::endl;
    exit( 2 );
}

void init(int samples, int depth) {

    // Texture
    t_render = Texture();
    t_render.set_sampling(GL_REPEAT, GL_LINEAR);
    t_render.bind(0);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, w_width, w_height, 0, GL_RGBA, GL_FLOAT, NULL);
    glBindImageTexture(0, t_render.m_texture, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA8);

    t_gather = Texture();
    t_gather.set_sampling(GL_REPEAT, GL_LINEAR);
    t_gather.bind(1);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, w_width, w_height, 0, GL_RGBA, GL_FLOAT, NULL);
    glBindImageTexture(1, t_gather.m_texture, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);

    t_mercury = Texture();
    t_mercury.bind(2);
    t_mercury.load("earth.jpg");

    // Camera
    float aspect = float(w_width) / float(w_height);
    vec3 target = vec3(278, 278, 0);
    vec3 pos = vec3(278, 278, -800);
    float focus = 10.0f;
    c_camera = new Camera(pos, target, vec3(0, 1, 0), 40.0f, aspect, 0.0f, focus);

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
    srand(time(NULL));

    s_compute.bind();
    t_gather.bind(0);
    s_compute.uniform_int("dest", 0);
    t_render.bind(1);
    s_compute.uniform_int("src", 1);
    t_mercury.bind(2);
    s_compute.uniform_int("mercury_tex", 2);

    s_compute.uniform_int("i_seed", rand());
    s_compute.uniform_int("samples", samples);
    s_compute.uniform_int("depth", depth);
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

void render(int n) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Compute Shader
    s_compute.bind();
    t_render.bind(0);
    t_gather.bind(1);
    s_compute.uniform_int("i_seed", rand());
    s_compute.uniform_int("n_render", n);
    glDispatchCompute(w_width / 32, w_height / 32, 1);

    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

    // Draw to screen
    s_quad.bind();
    t_render.bind(0);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    glfwSwapBuffers(window);
}

int gather(int samples, int depth) {

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

    init(samples, depth);

    time_t start = time(NULL);
    time_t last_update = time(NULL);
    time_t now;
    double diff;
    double delta;
    double last_fps = 0;
    int frames = 0;
    int n = 0;
    while( !glfwWindowShouldClose(window) ) {
        now = time(NULL);
        diff = difftime(now, last_update);
        last_update = now;
        
        last_fps += diff;
        frames++;

        if (last_fps >= 1.0f) {
            printf("[FPS] - %d\n", frames);
            last_fps = 0.0f;
            frames = 0;
        }
        n++;
        render(n);

        glfwPollEvents(); 

        int us = (int) ((difftime(last_update, time(NULL)) + (1.0f / FPS_CAP)) * 1000000);
        std::this_thread::sleep_for(std::chrono::microseconds(us));
    }

    delete c_camera;

    glfwDestroyWindow( window );
    glfwTerminate();

    return 0;
}

int main(int argc, char **argv) {
    int samples = 25;
    int depth = 20;
    if (argc >= 3) {
        depth = atoi(argv[2]);
    }
    if (argc >= 2) {
        samples = atoi(argv[1]);
    }

    return gather(samples, depth);
}
