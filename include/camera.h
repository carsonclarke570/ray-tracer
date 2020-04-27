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

    public:

        /**
         * Creates a new Camera object.
         *
         * @param   position    Position of camera
         * @param   lookat      Target to look at
         * @param   up          Camera's up vector
         * @param   fov         Field of view from top to bottom
         * @param   aspect      Aspect ratio (width / height)
         */
        Camera(vec3 position, vec3 lookat, vec3 up, float fov, float aspect, float aperture, float focus);


        /**
         *  Updates a shader with the camera data.
         * 
         *  @param  shader  Shader to update unforms for
         */
        void update_shader(Shader &shader);
};

#endif