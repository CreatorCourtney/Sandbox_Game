#ifndef GAME_MATH_CPP
#define GAME_MATH_CPP

#include "GameMath.hpp"

namespace Math
{
    // constructors: assign x and y
    Vector2::Vector2(float X, float Y) : x(X), y(Y) {}
    // make x and y both 0 by default
    Vector2::Vector2() : x(0.0f), y(0.0f) {}

    // magnitude of the vector
    // ||v|| = sqrt(x^2 + y^2)
    float Vector2::length() {
        return sqrtf(x*x + y*y);
    }


    // unit vector = vector with magnitude of 1 = v / ||v||
    // just divide x and y by length()
    // when the vector has zero magnitude, x = y = 0, issues arise since both terms are divided by zero
    // when normalising a zero magnitude vector, do nothing

    // makes itself a unit vector
    void Vector2::normalise() {
        float len = length();
        if (len != 0.0f) {
            // divide both terms by the magnitude
            x /= len; 
            y /= len;
        }
    }

    // returns itself as a unit vector
    Vector2 Vector2::normalised() {
        float len = length();
        if (len == 0.0f) return Zero2; // reutrn a zero magnitude vector
        return *this / len; // return itself divided by its magnitude
    }

    // overload operators for vector arithmetic

    // vector addition/subtraction: a + b = (ax + bx, ay + by)
    Vector2 Vector2::operator+(Vector2 const& other) {
        return Vector2(x+other.x, y+other.y);
    }

    Vector2 Vector2::operator-(Vector2 const& other) {
        return Vector2(x-other.x, y-other.y);
    }

    // scalar multiplication/division: k * a = (k * ax, k * ay)
    Vector2 Vector2::operator*(float const& k) {
        return Vector2(x*k, y*k); // scalar product
    }

    Vector2 Vector2::operator/(float const& k) {
        return Vector2(x/k, y/k);
    }

    // dot product (retuns a scalar): a * b = ||a||*||b||*cos(theta)
    // a * b = (ax * bx) + (ay * by)
    float Vector2::operator*(Vector2 const& other) {
        return (x*other.x) + (y*other.y); // dot product
    }

    // boolean comparisons

    // vectors a and b are equal if ax = bx AND ay = by
    bool Vector2::operator==(Vector2 const& other) {
        return x==other.x && y==other.y;
    }
    // vectors a and b are NOT equal if ax != bx OR ay != by
    bool Vector2::operator!=(Vector2 const& other) {
        return x!=other.x || y!=other.y;
    }

    // constructors for Point2; assign x and y
    Point2::Point2(int X, int Y) : x(X), y(Y) {}
    // make x and y both 0 by default
    Point2::Point2() : x(0), y(0) {}


    // overload operators
    bool Point2::operator==(Point2 const& other) {
        return x==other.x && y==other.y;
    }
    bool Point2::operator!=(Point2 const& other) {
        return x!=other.x || y!=other.y;
    }



    // gets unit vector from p0 to p1
    Vector2 getUnitVector(Vector2 p0, Vector2 p1)
    {
        // displacement from p0 to p1 = p1 - p0
        Vector2 disp = p1 - p0;
        // normalise the displacement vector to get a unit vector
        return disp.normalised();
    }

    // time passed since begin_time
    float DeltaTime(clock_t * begin_time)
    {
        clock_t t = clock(); // current time
        // clock_t is an integer number of clock cycles on the processor
        // t - *begin_time is the number of clock cycles passed
        // divide by CLOCKS_PER_SECOND, the number of times your computer's clock cycles every second, to get dt, a value of real time
        float dt = float(t - *begin_time) / CLOCKS_PER_SEC;
        // update begin time to be the current time, so this function can be reused every frame
        *begin_time = t;
        return dt; // reutrn delta time
    }

    
    float minf(float a, float b) { // min for floats
        return (a<b)? a : b;
    }
    float maxf(float a, float b) { // max for floats
        return (a>b)? a : b;
    }
    int Max(int a, int b) { // max for ints
        return (a>b)? a : b;
    }
    int Min(int a, int b) { // min for ints
        return (a<b)? a : b;
    }

    float absf(float a) { // abs for ints
        return (a<0.0f)? -a : a;
    }

    float clampf(float min, float max, float x) { // clamps x between a min and max value
        if (x<min) return min; // return min if x is below the desired range
        return (max<x)? max : x; // return max if x is too large, otherwise don't change x
    }
}

#endif