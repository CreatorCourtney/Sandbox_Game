#ifndef ENTITY_FUNCTIONS_HPP
#define ENTITY_FUNCTIONS_HPP

#include "GameObjects.hpp"
#include "Globals.hpp"

#include "AStarPathfinding.hpp"

namespace Func {
    // velocity functions

    // update's player's velocity based on input keys
    void playerVelocityFunc(Object::GameObject *p);
    // gives wolf velocity in the direction of the player
    void wolfVelocityFunc(Object::GameObject *w);

    // adds object's acceleration to their velocity
    void defaultVelocityFunc(Object::GameObject *o);


    // position functions

    // adds velocity to current position
    void defaultPositionFunc(Object::GameObject *o);
    // trees don't move, snap their position to the position they spawned
    void fallingTreePositionFunc(Object::GameObject *t);

    // holds the item stack a constant distance away from the player's centre
    void heldItemPositionFunc(Object::GameObject *item);
    // adds velocity to the position. when the velocity reaches zero, set acceleration to zero.
    void thrownItemPositionFunc(Object::GameObject *item);


    // animation scripts

    // chooses an animation set depending on which direction the player is facing,
    // chooses one image from the selected set based on how much the animation has progressed
    Gdiplus::TextureBrush * playerAnimationScript(Object::GameObject *p, Math::Point2 *screenPos);
    // rotates the tree image, using linear interpolation
    Gdiplus::TextureBrush * treeFallingAnimations(Object::GameObject *t, Math::Point2 *screenPos);

    // just returns the object's brush member. for objects with no animation
    Gdiplus::TextureBrush * noAnimation(Object::GameObject *o, Math::Point2 *screenPos);


    // pathfinding!!!!
    
    // finds the best path to the player, search depth of 50
    void pathfindToPlayer(Object::GameObject *o);
}

#endif