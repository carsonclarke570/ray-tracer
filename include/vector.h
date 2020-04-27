#ifndef _VECTOR_H_
#define _VECTOR_H_

#include <math.h>

#define MATH_PI 3.141592653589793238462643383279502884197169399375f

struct vec3 {
    float e[3];

    vec3() : e{0.0f, 0.0f, 0.0f} {}
    vec3(float f) : e{f, f, f} {}
    vec3(float x, float y, float z) : e{x, y, z} {}

    float operator[](int i) const { return e[i]; }
    float& operator[](int i) { return e[i]; }

    vec3 operator-() const { return vec3(-e[0], -e[1], -e[2]); }
    
    vec3& operator+=(const vec3 &v) {
        e[0] += v.e[0];
        e[1] += v.e[1];
        e[2] += v.e[2];
        return *this;
    }

    vec3& operator*=(const vec3 &v) {
        e[0] *= v.e[0];
        e[1] *= v.e[1];
        e[2] *= v.e[2];
        return *this;
    }

    vec3& operator/=(const vec3 &v) {
        e[0] /= v.e[0];
        e[1] /= v.e[1];
        e[2] /= v.e[2];
        return *this;
    }

    friend vec3 operator+(const vec3 &u, const vec3 &v) {
        return vec3(u.e[0] + v.e[0], u.e[1] + v.e[1], u.e[2] + v.e[2]);
    }

    friend vec3 operator-(const vec3 &u, const vec3 &v) {
        return vec3(u.e[0] - v.e[0], u.e[1] - v.e[1], u.e[2] - v.e[2]);
    }

    friend vec3 operator*(const vec3 &u, const vec3 &v) {
        return vec3(u.e[0] * v.e[0], u.e[1] * v.e[1], u.e[2] * v.e[2]);
    }

    friend vec3 operator*(double f, const vec3 &v) {
        return vec3(f * v.e[0], f * v.e[1], f * v.e[2]);
    }

    friend vec3 operator*(const vec3 &u, double f) {
        return f * u;
    }

    friend vec3 operator/(vec3 v, double t) {
        return (1/t) * v;
    }

    float dot(const vec3 &v) {
        return e[0] * v.e[0] + e[1] * v.e[1] + e[2] * v.e[2];
    }

    vec3 cross(const vec3 &v) {
        return vec3(
            e[1] * v.e[2] - e[2] * v.e[1],
            e[2] * v.e[0] - e[0] * v.e[2],
            e[0] * v.e[1] - e[1] * v.e[0]
        );
    }

    float length() {
        return sqrt(e[0]*e[0] + e[1]*e[1] + e[2]*e[2]);
    }

    vec3& normalize() {
        float len = this->length();
        e[0] /= len;
        e[1] /= len;
        e[2] /= len;
        return *this;
    }
};

#endif