#ifndef GAME_OBJECTS_HPP
#define GAME_OBJECTS_HPP

#include <Windows.h>
#include <gdiplus.h>
#include <iostream>
#include <vector>
#include <functional>

#include "GameMath.hpp"
#include "Globals.hpp"
#include "FrameCreation.hpp"

// cell types
/* BYTE DECOMPOSITION:
byte 1:
    1-8 = cell info (wall type, etc)
byte 2:
9-12 = ...
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
#define TREE     0b0110000000000011

typedef int CellObject;


namespace Object
{
    // entity types
    enum EntityType
    {
        Player, // 0
        Wolf    // 1
    };

    class Animations {
    public:
        std::vector<Gdiplus::TextureBrush*> front;
        std::vector<Gdiplus::TextureBrush*> back;
        std::vector<Gdiplus::TextureBrush*> left;
        std::vector<Gdiplus::TextureBrush*> right;
        // which set should be used,
        // front, back, left, right = 0, 1, 2, 3
        int facing = 0;
        float stage = 0;
        float frontInter, backInter, leftInter, rightInter;
        // constructors
        Animations(std::vector<Gdiplus::TextureBrush*> Front,
            std::vector<Gdiplus::TextureBrush*> Back,
            std::vector<Gdiplus::TextureBrush*> Left,
            std::vector<Gdiplus::TextureBrush*> Right,
            float FrontT, float BackT, float LeftT, float RightT);
        Animations();
    };

    class GameObject {
    public:
        Math::Vector2 pos;
        Math::Vector2 centrePos;
        Math::Vector2 velocity;
        EntityType type;
        // object's health
        // currently, trees use HP to represent their index in trees
        int hp;
        float speed;
        Math::Point2 size;
        bool hasCollision;
        GameObject(Math::Vector2 Pos, Math::Vector2 Velocity,
            EntityType Type, Math::Point2 Size, float Speed, int Hp);
        void draw(Gdiplus::Graphics& graphics);
        void update();
        Animations animations;

    private:
        void updateVelocity();
        void updatePosition();
        // collision detection
        void handleCollisions();
        void HandleSideCollisionsWithWalls();
        void HandleCornerCollisionsWithWalls();
        void DrawHitbox(Gdiplus::Graphics& graphics, int dotSize, Math::Point2 screenPos);

        void updateBkg();
        Gdiplus::TextureBrush * chooseAnimationStage();
        bool hasAnimation = true;
        Gdiplus::TextureBrush* brush;
        Gdiplus::Matrix * brushMultMatrix;
        Math::Point2 srcDimensions;
        // X,Y = number of cells, Width/Height = step for collision detection
        Gdiplus::Rect cellularDimensions;
    };

    Math::Point2 findCell(Math::Vector2 pos);
    Math::Vector2 getWorldPosition(Math::Point2 wndPos);
    Math::Point2 getScreenPosition(Math::Vector2 worldPos);
}

#endif