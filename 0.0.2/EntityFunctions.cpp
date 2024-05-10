#include "EntityFunctions.hpp"

using namespace Globals;

namespace Func {
    void playerVelocityFunc(Object::GameObject *p) {
        float s = p->speed; 
        if (inputKeys&16) s *= 2.0f;
        if (debuggingTools&2) s *= 2.0f;
        p->velocity = Math::Vector2(
            (bool(inputKeys&1)-bool(inputKeys&4)),
            (bool(inputKeys&2)-bool(inputKeys&8))
        );
        // normalise the vector so that players can't walk faster by moving diagonally
        p->velocity.normalise(); p->velocity = p->velocity*s;
        // update the direction you're facing based on velocity
        if (p->velocity.y > 0.0f) p->animations.facing = 0;
        else if (p->velocity.y < 0.0f) p->animations.facing = 1;
        else if (p->velocity.x > 0.0f) p->animations.facing = 3;
        else if (p->velocity.x < 0.0f) p->animations.facing = 2;
        else p->animations.stage = 0.0f;
    }

    void wolfVelocityFunc(Object::GameObject *w) {
        Math::Vector2 dir = Math::getUnitVector(w->centrePos, player->centrePos);
        w->velocity = dir * w->speed;
    }
}