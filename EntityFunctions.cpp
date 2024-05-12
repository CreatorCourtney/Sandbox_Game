#include "EntityFunctions.hpp"

using namespace Globals;

namespace Func 
{
    // velocity functions
    // update's player's velocity based on input keys
    void playerVelocityFunc(Object::GameObject *p) {
        float s = p->speed; // store in a local variable, to save time loading from memory
        if (inputKeys&16) s *= 2.0f; // bit 5 of inputKeys = shift, double move speed
        if (debuggingTools&2) s *= 2.0f; // bit 2 of debugTools = speed boost, double speed

        p->velocity = Math::Vector2(
            // horizontal velocity: bit 1=d; move right (+x), bit 3=a; move left (-x)
            (bool(inputKeys&1)-bool(inputKeys&4)),
            // vertical velocity: bit 2=s; move down (+y), bit 4=w; move up (-y)
            (bool(inputKeys&2)-bool(inputKeys&8))
        );

        // normalise the vector so that players can't walk faster by moving diagonally
        p->velocity.normalise(); 
        p->velocity = p->velocity*s; // multiply the unit vector by movement speed

        // update the direction you're facing based on velocity
        if (p->velocity.y > 0.0f) p->animations.facing = 0;
        else if (p->velocity.y < 0.0f) p->animations.facing = 1;
        else if (p->velocity.x > 0.0f) p->animations.facing = 3;
        else if (p->velocity.x < 0.0f) p->animations.facing = 2;
        else p->animations.stage = 0.0f;
    }


    // gives wolf velocity in the direction of the player
    void wolfVelocityFunc(Object::GameObject *w) {
        // a unit vector from the wolf's position to the player's position
        Math::Vector2 dir = Math::getUnitVector(w->centrePos, player->centrePos);
        // moves in the direction of the unit vector, with a magnitude equal to it's speed
        w->velocity = dir * w->speed;
    }


    // position functions
    // adds velocity to current position
    void defaultPositionFunc(Object::GameObject *o) {
        // velocity is the derivative of position; add velocity to the current position every frame
        // multiply the velocity by deltaTime, first, to normalise the movement speed
        o->pos = o->pos + (o->velocity * deltaTime);
        // update the centre position, which is in the centr of the object's hitbox
        o->centrePos = o->pos + Math::Vector2(o->size.x, o->size.y);
    }
}