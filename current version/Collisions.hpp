#ifndef COLLISIONS_HPP
#define COLLISIONS_HPP

#include "GameObjects.hpp"

namespace Collisions
{
    // detects collisions with world borders, walls, and other game objects
    void defaultCollisionFunction(Object::GameObject *o);

    // pushes apart gameobjects that are too close to itself
    void defaultHandleCollisionsWithGameObjects(Object::GameObject *o);

    // makes sure the object cannot walk out of bounds
    void defaultCollideWithWorldBorders(Object::GameObject *o);

    // checks if the hitbox is within a wall, and pushes it out
    void defaultHandleSideCollisionsWithWalls(Object::GameObject *o);
    void defaultHandleCornerCollisionsWithWalls(Object::GameObject *o);
}

#endif