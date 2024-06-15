#ifndef ENTITY_FUNCTIONS_HPP
#define ENTITY_FUNCTIONS_HPP

#include "GameObjects.hpp"
#include "Globals.hpp"

#include "AStarPathfinding.hpp"

namespace Func {
    // velocity functions

    // gives wolf velocity in the direction of the player
    void wolfVelocityFunc(Object::GameObject *w);
    // a jumping attack for the wolf
    void wolfJumpAttack(Object::GameObject *w);
    // moves to the centre of the map without collision until it gets into the map
    void wolfSpawningFunc(Object::GameObject *w);


    // adds object's acceleration to their velocity
    void defaultVelocityFunc(Object::GameObject *o);
    // adds acceleration to velocity, until the object comes to a stop
    void deccelerateVelocityFunc(Object::GameObject *o);

    // draws a unit vector to the player and moves towards it
    void moveDirectlyToPlayer(Object::GameObject *o);




    // position functions

    void playerPositionFunc(Object::GameObject *p);

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
    
    // finds the best path to the player
    void pathfindToPlayer(Object::GameObject *o, int searchDepth);

    void wolfUpdatePathfinding(Object::GameObject *w);
    void wolfWalkToNextCell(Object::GameObject *w);



    // functions that make switching between these functions easier
    
    // switches a wolf into it's jump attack
    void wolfEnterJumpAttack(Object::GameObject *w);
}

#endif