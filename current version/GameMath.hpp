#ifndef GAME_MATH_HPP
#define GAME_MATH_HPP

#include <ctime>
#include <cmath>

// mathematican constants
#define PI 3.141592654f
#define ROOT2 1.414213562f
#define SIN45 0.7071067812f

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
        void operator+=(Vector2 const& other);
        void operator-=(Vector2 const& other);
        void operator/=(float const& k);
        void operator*=(float const& k);

    // global 2D vectors, Zero2 = zero magnitude vector, One2 = (1, 1)
    } Zero2(0.0f,0.0f), One2(1.0f,1.0f);

    // two integers
    struct Point2 {
        int x;
        int y;
        // contructors
        Point2(int X, int Y);
        Point2();

        // overload operators
        bool operator==(Point2 const& other);
        bool operator!=(Point2 const& other);
    };



    // finds the unit vector pointing from p0 to p1
    Vector2 getUnitVector(Vector2 p0, Vector2 p1);

    // finds the hadamard product of two vectors
    Vector2 hadamard(Vector2 v0, Vector2 v1);

    // time passed since begin_time, updates begin_time to current time
    float DeltaTime(clock_t * begin_time);

    float minf(float a, float b); // min for floats
    float maxf(float a, float b); // max for floats
    int Max(int a, int b); // max for ints
    int Min(int a, int b); // min for ints
    float absf(float a); // abs for floats
    int Abs(int a); // abs for ints
    float clampf(float min, float max, float x); // clamps x between 2 values, for floats
    int Clamp(int min, int max, int x); // clamp for ints
    int ceilToInt(float a);
    float signf(float a); // finds the sign of a float
    int Sign(int a); // sign for ints

    // counts the number of active bits in a variable
    int numBits(int var);
}

#endif