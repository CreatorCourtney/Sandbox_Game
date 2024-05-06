#ifndef GAME_MATH_HPP
#define GAME_MATH_HPP

#include <ctime>
#include <cmath>

namespace Math 
{
    struct Vector2 {
        float x;
        float y;
        Vector2(float X, float Y);
        Vector2();
        float length();
        void normalise();
        Vector2 normalised();
        Vector2 operator+(Vector2 const& other);
        Vector2 operator-(Vector2 const& other);
        Vector2 operator*(float const& k);
        Vector2 operator/(float const& k);
        float operator*(Vector2 const& other);
        bool operator==(Vector2 const& other);
        bool operator!=(Vector2 const& other);
    } Zero2(0.0f,0.0f), One2(1.0f,1.0f);

    struct Point2 {
        int x;
        int y;
        Point2(int X, int Y);
        Point2();
    };

    Vector2 getUnitVector(Vector2 v0, Vector2 v1);

    float DeltaTime(clock_t * begin_time);

    float minf(float a, float b);
    float maxf(float a, float b);
    float absf(float a);
    float clampf(float min, float max, float x);
}

#endif