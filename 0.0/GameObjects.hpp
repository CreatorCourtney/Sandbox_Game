#ifndef GAME_OBJECTS_HPP
#define GAME_OBJECTS_HPP

#include <Windows.h>
#include <gdiplus.h>
#include <iostream>

#include "GameMath.hpp"
#include "Globals.hpp"

// entity types
#define PLAYER 0
// cell types
/* BYTE DECOMPOSITION:
byte 1:
    1-8 = cell info (wall type, etc)
byte 2:
9-12 = ....
13: water by default?
14: barrier?
15: occupied?
16: indestructible? 
byte3:
17-20 = water corner shorelines
    ....LRlr, 0010 = bottom left, 0100 = top right
21-24 = side shorelines
    lrtb...., 1100 = left&right, 0101 = right&bottom
*/
#define EMPTY    0b0000000000000000
#define BARRIER  0b0010000000000000
#define WATER    0b0001000000000000
#define LOG      0b0110000000000001
#define BRIDGE   0b0100000000000010


namespace Object
{
    class GameObject {
    public:
        Math::Vector2 pos;
        Math::Vector2 velocity;
        int type;
        float scale;
        float speed;
        Math::Point2 size;
        GameObject(Math::Vector2 Pos, Math::Vector2 Velocity,
            int Type, float Scale, float Speed);
        void draw(Gdiplus::Graphics& graphics);
        void update();

    private:
        void updateVelocity();
        void updatePosition();
        void handleCollisions();
        void updateBkg();
        Gdiplus::Image * img;
    };

    Math::Point2 findCell(Math::Vector2 pos);
    Math::Vector2 getWorldPosition(Math::Point2 wndPos);
    Math::Point2 getScreenPosition(Math::Vector2 worldPos);
}

#endif