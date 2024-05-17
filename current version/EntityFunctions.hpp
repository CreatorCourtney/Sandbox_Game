#ifndef ENTITY_FUNCTIONS_HPP
#define ENTITY_FUNCTIONS_HPP

#include "GameObjects.hpp"
#include "Globals.hpp"

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


    // animation scripts

    // chooses an animation set depending on which direction the player is facing,
    // chooses one image from the selected set based on how much the animation has progressed
    Gdiplus::TextureBrush * playerAnimationScript(Object::GameObject *p, Math::Point2 *screenPos);
    // rotates the tree image, using linear interpolation
    Gdiplus::TextureBrush * treeFallingAnimations(Object::GameObject *t, Math::Point2 *screenPos);

    // just returns the object's brush member. for objects with no animation
    Gdiplus::TextureBrush * noAnimation(Object::GameObject *o, Math::Point2 *screenPos);
}

#endif