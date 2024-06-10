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
    }


    // gives wolf velocity in the direction of the player
    void wolfVelocityFunc(Object::GameObject *w) 
    {
        // whenever the wolf reaches the centre of it's pathfinding cell, pahtfind again
        // if the pathfinding node is a nullptr, then pathfind as well
        
        // because this process can sometimes be interuppted when a wolf is pushed by another
        // entity along it's path, it will also perform a pathfindind check every 3 seconds
        if (w->pathNode == nullptr || w->timer <= 0.0f) {
            // pathfind to the player
            pathfindToPlayer(w);
            // reset the timer
            w->timer = 3.0f;

        } else {
            // decrement the wolf's timer
            w->timer -= deltaTime;

            // try to get to the centre of the first pathfinding cell
            Math::Point2 cell = w->pathNode->cell;
            Math::Vector2 cellPos(((float)cell.x+0.5f)*sideLen, ((float)cell.y+0.5)*sideLen);

            // get the displacement from the wolf's centre to the centre of the next cell
            Math::Vector2 disp = cellPos - w->centrePos;

            // if either component is sufficiently large, move in that direction
            // only move in one direction at a time, as to round corners more effectively
            float absx = Math::absf(disp.x), absy = Math::absf(disp.y);
            if (absx > 2.0f && absx > absy) {
                w->velocity = {w->speed*Math::signf(disp.x), 0.0f};
            } else if (absy > 2.0f) {
                w->velocity = {0.0f, w->speed*Math::signf(disp.y)};
            } else {
                // if the wolf is close enough to the centre of it's foremost pathfinding cell,
                // pathfind to the player again, and progress to the next cell, again
                pathfindToPlayer(w);
                w->pathNode = w->pathNode->parent;
            }
        }


        // pathfind every second
        if (w->timer <= 0.0f) {
            // create a new pathfinding object
            Math::Point2 start = w->cell, end = player->cell, dimensions(grid.size(), grid[0].size());
            AStar::Pathfinding pathfinding(start, end, dimensions, 50);

            // find the best path to the player
            AStar::Node *node = pathfinding.findPath();
            // reverse the list to be start to end
            w->pathNode = AStar::ReverseLinkedList(node);
            w->pathNode = w->pathNode->parent;
            // pathfinding.close();
            w->timer = 1.0f;

        } else w->timer -= deltaTime;
    }


    // adds object's acceleration to their velocity
    void defaultVelocityFunc(Object::GameObject *o) {
        o->velocity = o->velocity + (o->acceleration * deltaTime);
    }


    // position functions

    // adds velocity to current position
    void defaultPositionFunc(Object::GameObject *o) 
    {
        // velocity is the derivative of position; add velocity to the current position every frame
        // multiply the velocity by deltaTime, first, to normalise the movement speed
        o->pos = o->pos + (o->velocity * deltaTime);
        // update the centre position, which is in the centre of the object's hitbox
        o->centrePos = o->pos + Math::Vector2(o->size.x/2, o->size.y/2);

        // update the cell the object resides in
        o->cell = {int(o->centrePos.x/sideLen), int(o->centrePos.y/sideLen)};
    }

    
    // moves the base of the falling tree to remain in place as the tree gets rotated
    void fallingTreePositionFunc(Object::GameObject *t)
    {
        // when the timer reaches 0, destroy the object
        if (t->timer <= 0.0f) 
        {
            // once the tree is gone, spawn some log + pine cone items in where it fell
            // the further log will be the height of the tree (size.x) to the LEFT of pos,
            // the nearest one could be at pos

            // 5 wood will spawn, 0-2 seeds will spawn
            int seeds = rand()%3, n = seeds+5;
            
            // step between each item object, and the y position of each object
            float step = t->size.x / n, y = t->pos.y + t->size.y;

            // spawn n items
            for (int i = 1; i <= n; i++)
            {
                Math::Vector2 pos(t->pos.x - step*i, y);
                // spawn logs first, seeds last
                Object::EntityType item = (i<6)? Object::Log_Item : Object::Pine_Cone_Item;
                Object::spawnItemStack(item, pos, 1);
            }

            // destroy the falling tree object
            Object::Destroy(t);
        }
    }


    // holds the item stack a constant distance away from the player's centre
    void heldItemPositionFunc(Object::GameObject *item)
    {
        // get a unit vector from the player's centre to the mouse position's centre
        Math::Vector2 dir = Math::getUnitVector(player->centrePos, mousePosREAL);

        // place the item a fixed distance from the player's centre, in the direction of the mouse
        float r = player->radius + item->radius + 5.0f;
        item->centrePos = player->centrePos + (dir * r);

        // update the 'pos' member
        item->pos = item->centrePos - Math::Vector2(item->size.x/2, item->size.y/2);
    }

    // adds velocity to the position. when the velocity reaches zero, set acceleration to zero.
    void thrownItemPositionFunc(Object::GameObject *item)
    {
        // velocity is the derivative of position; add velocity to the current position every frame
        // multiply the velocity by deltaTime, first, to normalise the movement speed
        item->pos = item->pos + (item->velocity * deltaTime);
        // update the centre position, which is in the centre of the object's hitbox
        item->centrePos = item->pos + Math::Vector2(item->size.x/2, item->size.y/2);

        /*
            when two vectors point in the same direction, their dot product is positive.
            when pointing in different (non orthogonal) directions, it is negative

            while the acceleration is pointing opposite the velocity, the entity is slowing down
            if the dot product of the acceleration and velocity is positive, the entity has
            already stopped, and is now speeding up. 

            to stop this, cancel the acceleration once the dot product is non-negative
        */
        
        // dot product is non-negative
        if (item->velocity * item->acceleration >= 0.0f)
        {
            // set velocity and acceleration both to zero 
            item->velocity = item->acceleration = Math::Zero2;
            // set the item's position function to default, to avoid
            // making this check every frame
            item->positionFunc = defaultPositionFunc;
        }

        // update the timer
        if (item->timer > 0.0f) item->timer -= deltaTime;
    }





    // animation scripts

    // chooses an animation set depending on which direction the player is facing,
    // chooses one image from the selected set based on how much the animation has progressed
    Gdiplus::TextureBrush * playerAnimationScript(Object::GameObject *p, Math::Point2 *screenPos)
    {
        // choose vector/index based on direction the object is facing

        // to choose index, multiply the 'stage' member with the associated interval member
        // then truncate the decimal by typecasting to int. if the index is bigger
        // than the size of the vector, then set the stage to zero and return the first
        // image in the vector. otherwise, use the index to choose an image.
        // also increment the stage by adding deltaTime to it


        // update the direction you're facing based on direction to the mouse
        Math::Vector2 dir = Math::getUnitVector(p->centrePos, mousePosREAL);

        if (dir.x > SIN45) p->animations.facing = 3; // looking right
        else if (dir.x < -SIN45) p->animations.facing = 2; // looking right
        else if (dir.y <= -SIN45) p->animations.facing = 1; // looking backwards
        else p->animations.facing = 0; // looking forwards

        // when not moving, keep the animation stage at 0
        if (p->velocity == Math::Zero2) p->animations.stage = 0.0f;


        Gdiplus::TextureBrush *res;
        switch (p->animations.facing)
        {
            case 0: { // forwards
                int idx = p->animations.stage*p->animations.frontInter;
                if (idx >= p->animations.front.size()) idx = p->animations.stage = 0;
                else if (!gameIsPaused) p->animations.stage += deltaTime;
                res = p->animations.front[idx];
                break;
            }
            case 1: { // back
                int idx = p->animations.stage*p->animations.backInter;
                if (idx >= p->animations.back.size()) idx = p->animations.stage = 0;
                else if (!gameIsPaused) p->animations.stage += deltaTime;
                res = p->animations.back[idx];
                break;
            }
            case 2: { // left
                int idx = p->animations.stage*p->animations.leftInter;
                if (idx >= p->animations.left.size()) idx = p->animations.stage = 0;
                else if (!gameIsPaused) p->animations.stage += deltaTime;
                res = p->animations.left[idx];
                break;
            }
            case 3: { // right
                int idx = p->animations.stage*p->animations.rightInter;
                if (idx >= p->animations.right.size()) idx = p->animations.stage = 0;
                else if (!gameIsPaused) p->animations.stage += deltaTime;
                res = p->animations.right[idx];
                break;
            }
            default: return nullptr;
        }

        // displacement matric for the brush, so that the image draws correctly
        p->setBrushMatrix(res, *screenPos);
        return res;
    }

    // rotates the tree image, using linear interpolation
    Gdiplus::TextureBrush * treeFallingAnimations(Object::GameObject *t, Math::Point2 *screenPos)
    {
        Gdiplus::TextureBrush *brush = t->brush;

        // find the displacement in x and y of the bottom left of the image to the base of the tree once rotated

        // timer is initialised to 1.0f, use it as an interpolator
        float interp = 1.0f - (t->timer/1.0f);


        /*
            not gonna explain all the trig here, I found that for an angle, theta, between the line parallel to the tree,
            and the vertical y axis, the distance of the bottom left corner of the tree from it's original position will be:

            dX = r * sin(theta),    dY = r * (1 - cos(theta)), where r is the height of the tree

            it's unfortunate this uses TWO trig functions evrery frame... if someone could figure out how to do this with 
            vector arithmetic that would be so cool B)

            *this doesn't fix the fact that trig functions are used, but recall that 
            sin(theta) = sqrt(1 - cos(theta)), which can be used to avoid calculating ONE trig function
        */


        // alpha = angle in degrees (for actual rotation), 
        // theta = angle in radians (for calculations). r = height of the tree
        float alpha = -100.0f * interp, theta = (PI*0.5555556f) * interp, r = t->size.y;

        // recall that sin = sqrt(1 - cos^2)
        float cosTheta = cosf(theta), sinTheta = sqrt(1.0f - cosTheta*cosTheta);

        // distance the corner of the object is from its original position (dx, dy)
        float dx = r*sinTheta, dy = r*(1-cosTheta);

        // adjust the area the brush is being drawn along the x axis, since otherwise the image would rotate OUT of the drawing
        // rectangle. This isn't necesary along the y axis, though. just add the y displacement to the tranform matrix

        // brush->SetWrapMode(Gdiplus::WrapModeTile);
        // t->setBrushMatrix(brush, Math::Point2(screenPos->x, screenPos->y+t->size.y));

        screenPos->x -= dx;
        t->setBrushMatrix(brush, Math::Point2(screenPos->x, screenPos->y+dy));

        // rotate the image to mimic the tree falling
        brush->RotateTransform(alpha);

        t->timer -= deltaTime; // decrease the object's hp

        return brush;
    }


    // just returns the object's brush member. for objects with no animation
    Gdiplus::TextureBrush * noAnimation(Object::GameObject *o, Math::Point2 *screenPos)
    {
        Gdiplus::TextureBrush *res = o->brush;
        o->setBrushMatrix(res, *screenPos);
        return res;
    }



    // pathfinding!!!!
    
    // finds the best path to the player, search depth of 50
    void pathfindToPlayer(Object::GameObject *o)
    {
        // pathfind to the player
        // create a new pathfinding object
        Math::Point2 start = o->cell, end = player->cell, dimensions(grid.size(), grid[0].size());
        AStar::Pathfinding pathfinding(start, end, dimensions, 50);

        // find the best path to the player
        AStar::Node *node = pathfinding.findPath();
        // reverse the list to be start to end
        o->pathNode = AStar::ReverseLinkedList(node);
        // pathfinding.close();
    }
}