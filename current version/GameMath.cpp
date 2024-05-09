#ifndef GAME_MATH_CPP
#define GAME_MATH_CPP

#include "GameMath.hpp"

namespace Math
{
    Vector2::Vector2(float X, float Y) : x(X), y(Y) {}
    Vector2::Vector2() : x(0.0f), y(0.0f) {}
    float Vector2::length() {
        return sqrtf(x*x + y*y);
    }
    // makes itself a unit vector
    void Vector2::normalise() {
        float len = length();
        if (len == 0.0f) *this = Zero2;
        else {x /= len; y /= len;}
    }
    // returns itself as a unit vector
    Vector2 Vector2::normalised() {
        float len = length();
        if (len == 0.0f) return Zero2; 
        return *this / len;
    }

    Vector2 Vector2::operator+(Vector2 const& other) {
        return Vector2(x+other.x, y+other.y);
    }
    Vector2 Vector2::operator-(Vector2 const& other) {
        return Vector2(x-other.x, y-other.y);
    }
    Vector2 Vector2::operator*(float const& k) {
        return Vector2(x*k, y*k); // scalar product
    }
    Vector2 Vector2::operator/(float const& k) {
        return Vector2(x/k, y/k);
    }
    float Vector2::operator*(Vector2 const& other) {
        return (x*other.x) + (y*other.y); // dot product
    }
    bool Vector2::operator==(Vector2 const& other) {
        return x==other.x && y==other.y;
    }
    bool Vector2::operator!=(Vector2 const& other) {
        return x!=other.x || y!=other.y;
    }
    Point2::Point2(int X, int Y) : x(X), y(Y) {}
    Point2::Point2() : x(0), y(0) {}

    // gets unit vector from p0 to p1
    Vector2 getUnitVector(Vector2 p0, Vector2 p1)
    {
        Vector2 disp = p1 - p0;
        return disp.normalised();
    }

    float DeltaTime(clock_t * begin_time)
    {
        clock_t t = clock(); // current time
        float dt = float(t - *begin_time) / CLOCKS_PER_SEC;
        *begin_time = t;
        return dt;
    }

    float minf(float a, float b) {
        return (a<b)? a : b;
    }
    float maxf(float a, float b) {
        return (a>b)? a : b;
    }

    float absf(float a) {
        return (a<0.0f)? -a : a;
    }

    float clampf(float min, float max, float x) {
        if (x<min) return min;
        else return (max<x)? max : x;
    }
}

#endif