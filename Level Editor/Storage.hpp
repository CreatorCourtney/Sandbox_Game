#ifndef STORAGE_HPP
#define STORAGE_HPP

#include <Windows.h>
#include <gdiplus.h>
#include <string>

#include <iostream>
#include <fstream>
#include <sstream>

#include "GameMath.hpp"
#include "Globals.hpp"

namespace Storage 
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


    class Entity {
    public:
        Math::Vector2 pos; // position of top left corner of hitbox (origin)
        Math::Vector2 centrePos; // position of entity's centre
        Math::Vector2 velocity;
        Math::Vector2 acceleration;

        int idx; // index in the global vector vector

        Math::Point2 size; // the dimensions of the entity's hitbos
        Math::Point2 cell; // which cell the entity is in

        EntityType type; // what type of entity it is

        int hp, maxHP; // the health and max health of the entity

        float speed; // entity's movement speed
        float timer = 0.0f; // for entity's with functions dependent on time

        bool hasCollision; // whether or not the entity can collide with things
        Gdiplus::Rect cellularDimensions; // the dimensions (in cells) of the entity's hitbox

        float radius; // the distance other entities can be from this one

        // image that will be drawn to the map
        Gdiplus::TextureBrush *brush;


        // constructor
        Entity(Math::Vector2 Pos, Math::Vector2 Velocity, EntityType Type, int Hp, int Idx);

        
        // public drawing functions
        // draws the entity to the graphics object
        void show(Gdiplus::Graphics& graphics);
        // canges the scale of the object according to sideLen
        void resize(int cellSideLen, int previosuSideLen);

        // cleanup function
        void deallocateResources();

    private:
        
        Gdiplus::Matrix *brushScaleMatrix; // for scaling the brush to be drawn to screen
        Math::Point2 srcDimensions; // dimensions (in pixels) of the source image
    };


    // saves the created level to file
    void saveLevelToFile(std::string filename);
    // saves an entity to one line of a file
    void SaveEntityToFile(std::fstream *file, Storage::Entity *obj);
}

#endif