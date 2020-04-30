#ifndef _CAMERA_H_
#define _CAMERA_H_

#include "shader.h"
#include "vector.h"

class Camera {
    private:
        vec3 origin;
        vec3 lower_left;
        vec3 right;
        vec3 up;
        vec3 u, v ,w;
        float lens;
        float fov; 
        float aspect; 
        float aperature; 
        float focus;

    public:

        /**
         * Creates a new Camera object.
         *
         * @param   position    Position of camera
         * @param   lookat      Target to look at
         * @param   up          Camera's up vector
         * @param   fov         Field of view from top to bottom
         * @param   aspect      Aspect ratio (width / height)
         * @param   aperature   Camera aperature
         * @param   focus       Focus distance
         */
        Camera(vec3 position, vec3 lookat, vec3 up, float fov, float aspect, float aperture, float focus);


        /**
         *  Updates the camera data
         *
         * @param   position    Position of camera
         * @param   lookat      Target to look at
         * @param   up          Camera's up vector
         */ 
        void update(vec3 position, vec3 lookat, vec3 up);

        /**
         *  Updates a shader with the camera data.
         * 
         *  @param  shader  Shader to update unforms for
         */
        void update_shader(Shader &shader);
};

#endif