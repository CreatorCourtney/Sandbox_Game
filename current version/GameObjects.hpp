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
9-12 = timer, (0-15)

13: water by default?
14: barrier?
15: occupied?
16: indestructible by player? 

byte 3:
17-20 = water corner shorelines
    ....LRlr, 0010 = bottom left, 0100 = top right
21-24 = side shorelines
    lrtb...., 1100 = left&right, 0101 = right&bottom

byte 4:
25: can be destroyed by enemies

16-32: building HP
*/
#define EMPTY           0b00000000000000000000000000000000
#define BARRIER         0b00000000000000000010000000000000
#define OCCUPIED        0b00000000000000000100000000000000
#define WATER           0b00000000000000000001000000000000
#define DESTRUCTIBLE    0b00000001000000000000000000000000

#define LOG      0b00010001000000000110000000000001 // 8 health
#define BRIDGE   0b00000111000000000100000000000010 // 3 health
#define TREE     0b00000000000000000110000000000011 // not destructible
#define STUMP    0b00010001000000000110000000000100 // 8 health
#define SAPLING  0b00000101000000000110000000000101 // 2 health

typedef int CellObject;


namespace Object
{
    // entity types
    enum EntityType
    {
        Player, // 0
        Wolf,    // 1
        Falling_Tree, // 2

        // item stack entities
        Log_Item, // 3
        Pine_Cone_Item // 4
    };

    // object to store images for game object animation
    class Animations {
    public:
        // sets of images, all of the images in one vector 
        // will be shows sequentially to play an animation
        std::vector<Gdiplus::TextureBrush*> front;
        std::vector<Gdiplus::TextureBrush*> back;
        std::vector<Gdiplus::TextureBrush*> left;
        std::vector<Gdiplus::TextureBrush*> right;

        // which set should be used,
        // front, back, left, right = 0, 1, 2, 3
        int facing = 0;
        // float used for vector index; it increases by a subinteger value
        // according to deltaTime, and when typecasted to int, will be
        // truncated to serve as a vector index
        float stage = 0;

        // the amount of time needed to pass between each image in each vector
        float frontInter, backInter, leftInter, rightInter;

        // constructors
        Animations(std::vector<Gdiplus::TextureBrush*> Front,
            std::vector<Gdiplus::TextureBrush*> Back,
            std::vector<Gdiplus::TextureBrush*> Left,
            std::vector<Gdiplus::TextureBrush*> Right,
            float FrontT, float BackT, float LeftT, float RightT);
        Animations();
    };


    // a class used to represent a dynamic entity in the game
    class GameObject {
    public:
        // position of the top left corner of the hitbox
        Math::Vector2 pos;
        // size of the hitbox
        Math::Point2 size;
        // position of the centre of the hitbox
        Math::Vector2 centrePos;
        Math::Point2 cell; // the cell the object lies in

        // object's velocity and acceleration
        Math::Vector2 velocity;
        Math::Vector2 acceleration;

        EntityType type; // which type of entity it is
        Animations animations; // images used for animating
        Gdiplus::TextureBrush* brush; // if the entity has no animations, this constant brush is used for drawing
        

        // for managing associated entities
        GameObject* owner = nullptr;
        std::vector<GameObject*> ownedObjects; 

        
        int hp, maxHP; // object's health/health cap
        float speed; // scalar movement speed
        int idx; // index of the object in the gameObjects vector
        float timer; // for obects with mechanics dependent on time


        bool hasCollision;
        // X,Y = number of cells, Width/Height = space between collision detection points
        Gdiplus::Rect cellularDimensions;
        // radius of circle around the hitbox's centre, representing
        // how close other entities can get to the object
        float radius = 0.0f; 


        // constructor
        GameObject(Math::Vector2 Pos, Math::Vector2 Velocity, EntityType Type, int Hp);


        // calls the behaviour functions, updating member variables
        void update();


        // public drawing functions

        // draws the object to the graphics object
        void draw(Gdiplus::Graphics& graphics);
        // prepares a texturebrush to be drawn to the screen by setting the matrix displacement and scale
        void setBrushMatrix(Gdiplus::TextureBrush *b, Math::Point2 disp);


        // what should happen when updating the entity's velocity
        void (*velocityFunc)(GameObject*);
        // what should happen when updating the entity's position
        void (*positionFunc)(GameObject*);
        // collision behaviour
        void (*collisionFunc)(GameObject*);


        // cleanup function
        void deallocateResources();

    private:
        
        // developer tool, draws the collision boundaries for the game object
        void DrawHitbox(Gdiplus::Graphics& graphics, int dotSize, Math::Point2 screenPos);


        // updates the position of the camera based on the player's position
        void updateBkg();


        // choses one image from the animations object that will be drawn to the screen
        Gdiplus::TextureBrush* (*animationScript)(GameObject*, Math::Point2*);
        
        Gdiplus::Matrix *brushScaleMatrix; // scale for image drawing purposes
        Math::Point2 srcDimensions; // dimensions (in pixels) of the image file
    };



    // finds the cell coordinates of a position in world space
    Math::Point2 findCell(Math::Vector2 pos);

    // finds the position on the map associated with (x,y) coordinates on the screen
    Math::Vector2 getWorldPosition(Math::Point2 wndPos);
    // translates a position on the map to its relative plaement on the screen
    Math::Point2 getScreenPosition(Math::Vector2 worldPos);



    // for instantiating game objects. returns a reference to the object created
    GameObject* Instantiate(EntityType type, Math::Vector2 pos, int hp);

    // deletes the specified object
    void Destroy(GameObject * obj);

    
    // spawns an item stack with random velocity. return a reference to the item created
    GameObject* spawnItemStack(EntityType type, Math::Vector2 pos, int count);
}

#endif