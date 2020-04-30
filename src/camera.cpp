#include "camera.h"

Camera::Camera(vec3 position, vec3 lookat, vec3 up, float fov, float aspect, float aperature, float focus) {
    this->fov = fov;
    this->aspect = aspect;
    this->aperature = aperature;
    this->focus = focus;
    
    this->update(position, lookat, up);
}

void Camera::update(vec3 position, vec3 lookat, vec3 up) {
    float theta = (this->fov * MATH_PI) / 180.0f;
    float hh = tan(theta / 2);
    float hw = this->aspect * hh;

    this->w = (position - lookat).normalize();
    this->u = up.cross(w).normalize();
    this->v = w.cross(u);

    this->origin = position;
    this->lens = this->aperature / 2;
    this->lower_left = position - (hw * focus * u) - (hh * focus * v) - (focus * w);
    this->right = 2 * hw * focus * u;
    this->up = 2 * hh * focus * v;
}

void Camera::update_shader(Shader &shader) {
    shader.uniform_vec3("cam.lower_left", this->lower_left);
    shader.uniform_vec3("cam.origin", this->origin);
    shader.uniform_vec3("cam.right", this->right);
    shader.uniform_vec3("cam.up", this->up);
    shader.uniform_vec3("cam.u", this->u);
    shader.uniform_vec3("cam.v", this->v);
    shader.uniform_float("cam.lens", this->lens);
}