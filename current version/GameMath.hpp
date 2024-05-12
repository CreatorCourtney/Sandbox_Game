#ifndef GAME_MATH_HPP
#define GAME_MATH_HPP

#include <ctime>
#include <cmath>

namespace Math 
{
    // a two dimensional vector
    struct Vector2 {
        float x;
        float y;
        // constructors
        Vector2(float X, float Y);
        Vector2();

        // used for finding length and getting unit vectors
        float length();
        void normalise(); // makes itself a unit vector
        Vector2 normalised(); // returns itself as a unit vector

        // overload operators
        Vector2 operator+(Vector2 const& other);
        Vector2 operator-(Vector2 const& other);
        Vector2 operator*(float const& k);
        Vector2 operator/(float const& k);
        float operator*(Vector2 const& other);
        bool operator==(Vector2 const& other);
        bool operator!=(Vector2 const& other);

    // global 2D vectors, Zero2 = zero magnitude vector, One2 = (1, 1)
    } Zero2(0.0f,0.0f), One2(1.0f,1.0f);

    // two integers
    struct Point2 {
        int x;
        int y;
        // contructors
        Point2(int X, int Y);
        Point2();
    };

    // finds the unit vector pointing from v0 to v1
    Vector2 getUnitVector(Vector2 v0, Vector2 v1);

    // time passed since begin_time, updates begin_time to current time
    float DeltaTime(clock_t * begin_time);

    float minf(float a, float b); // min for floats
    float maxf(float a, float b); // max for floats
    int Max(int a, int b); // max for ints
    float absf(float a); // abs for floats
    float clampf(float min, float max, float x); // clamps x between 2 values, for floats
}

#endif