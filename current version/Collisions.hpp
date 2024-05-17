#ifndef COLLISIONS_HPP
#define COLLISIONS_HPP

#include "GameObjects.hpp"

namespace Collisions
{
    // detects collisions with world borders, walls, and other game objects
    void defaultCollisionFunction(Object::GameObject *o);

    
    // for an item sitting still, collide with non-items normally, but when colliding
    // with another item, combine stacks, if possible
    void stationaryItemCollisionFunction(Object::GameObject *item);



    // pushes apart gameobjects that are too close to itself
    void defaultHandleCollisionsWithGameObjects(Object::GameObject *o);
    // pushes apart objects that are too close to itself,
    // or combine stacks with other items
    void itemsHandleCOllisionsWithGameObjects(Object::GameObject *item);




    // makes sure the object cannot walk out of bounds
    void defaultCollideWithWorldBorders(Object::GameObject *o);

    // checks if the hitbox is within a wall, and pushes it out
    void defaultHandleSideCollisionsWithWalls(Object::GameObject *o);
    void defaultHandleCornerCollisionsWithWalls(Object::GameObject *o);
}

#endif