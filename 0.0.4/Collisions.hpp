#ifndef COLLISIONS_HPP
#define COLLISIONS_HPP

#include "GameObjects.hpp"
#include "GameFunctions.hpp"

namespace Collisions
{
    // detects collisions with world borders, walls, and other game objects
    void defaultCollisionFunction(Object::GameObject *o);

    // similar to default, but lets the player load different areas when walking off the map
    void playerCollisionFunction(Object::GameObject *p);

    
    // for an item sitting still, collide with non-items normally, but when colliding
    // with another item, combine stacks, if possible
    void stationaryItemCollisionFunction(Object::GameObject *item);



    // pushes apart gameobjects that are too close to itself
    void defaultHandleCollisionsWithGameObjects(Object::GameObject *o);
    // pushes apart objects that are too close to itself,
    // or combine stacks with other items
    void itemsHandleCOllisionsWithGameObjects(Object::GameObject *item);




    // makes sure the object cannot walk out of bounds
    bool defaultCollideWithWorldBorders(Object::GameObject *o);
    // loads different areas based on where the player walks
    void playerCollideWithWorldBorders(Object::GameObject *p);

    // checks if the hitbox is within a wall, and pushes it out
    void defaultHandleSideCollisionsWithWalls(Object::GameObject *o);
    void defaultHandleCornerCollisionsWithWalls(Object::GameObject *o);
}

#endif