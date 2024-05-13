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

    // objects with zero velocity do nothing when updating
    void zeroVelocityFunc(Object::GameObject *o);


    // position functions

    // adds velocity to current position
    void defaultPositionFunc(Object::GameObject *o);
    // trees don't move, snap their position to the position they spawned
    void fallingTreePositionFunc(Object::GameObject *t);
}

#endif