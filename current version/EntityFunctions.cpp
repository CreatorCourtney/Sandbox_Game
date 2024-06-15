#include "EntityFunctions.hpp"

using namespace Globals;

namespace Func 
{
    // velocity functions


    // gives wolf velocity in the direction of the player
    void wolfVelocityFunc(Object::GameObject *w) 
    {
        w->damage = 0;
        // when the level loads, or the end of pathfinding has been reached
        // wolves will update their pathfinding every 0.5 seconds
        if (w->timer <= 0.0f) {
            wolfUpdatePathfinding(w);
        }
        
        // if the wolf is close enough to the player, attempt to perform an attack
        if (w->pathfindLength < 5) {

            Math::Vector2 distToPlayer = player->centrePos - w->centrePos;

            // set the pathfinding to a nullptr, so it's not stuck in this case forever
            // if the wolf is super close, just go straight for them
            if (distToPlayer.length() <= 2.0f * sideLen || w->pathfindLength <= 1) {
                w->pathNode = nullptr;
                w->damage = 1;
                moveDirectlyToPlayer(w);

            } else if (w->timer <= 0.5f) {
                w->pathNode = nullptr;
                wolfEnterJumpAttack(w);
            }
        }
        // decrease the timer
        w->timer -= deltaTime;

        // walk to the first cell in it's pathfinding
        if (w->pathNode != nullptr) wolfWalkToNextCell(w);
    }

    // a jumping attack for the wolf
    void wolfJumpAttack(Object::GameObject *w)
    {
        // spend 0.5 seconds charging up
        if (w->acceleration == Math::Zero2 && w->timer <= 2.0f && w->timer >= 1.9f) {

            Math::Vector2 dir = Math::getUnitVector(w->centrePos, player->centrePos);
            // give the wolf a bunch of velocity in that direction,
            // with some negative acceleration
            w->velocity = dir * 450.0f;
            w->acceleration = dir * -300.0f;
            
            // wolf deals increased damage during this attack
            w->damage = 2;

        // when the wolf comes to a stop, go back to the regular pathfinding
        } else if (w->velocity * w->acceleration > 0.0f) {
            // reset any velocity + acceleration
            w->acceleration = w->velocity = Math::Zero2;

        // spend half a second cooling down
        } else if (w->timer <= 0.0f) {
            // reset the timer
            w->timer = 1.0f;
            w->damage = 0;
            // set the velocity function back to regular pathfinding
            wolfUpdatePathfinding(w);
            w->velocityFunc = wolfVelocityFunc;
            return;
        }
        // add acceleration to velocity
        w->velocity = w->velocity + w->acceleration * deltaTime;
        // decrease the timer
        w->timer -= deltaTime;
    }



    // moves to the centre of the map without collision until it gets into the map
    void wolfSpawningFunc(Object::GameObject *w)
    {
        Math::Point2 cell = w->cell;
        if (grid[cell.x][cell.y]&BARRIER) {
            // still inside the world border, move towards the centre of the map
            Math::Vector2 dest(currLevel->gridX*sideLen/2, currLevel->gridY*sideLen/2);
            Math::Vector2 dir = Math::getUnitVector(w->centrePos, dest);
            // don't use the entity's speed, just go with a constant spawning speed
            w->velocity = dir * 80.0f;
        
        } else {
            // entity is now in the map, transition into the normal pathfinding algorithm
            w->hasCollision = true;
            w->velocityFunc = wolfVelocityFunc;
        }
    }





    // adds object's acceleration to their velocity
    void defaultVelocityFunc(Object::GameObject *o) {
        o->velocity = o->velocity + (o->acceleration * deltaTime);
    }

    // adds acceleration to velocity, until the object comes to a stop
    void deccelerateVelocityFunc(Object::GameObject *o) {
        /*
            when two vectors point in the same direction, their dot product is positive.
            when pointing in different (non orthogonal) directions, it is negative

            while the acceleration is pointing opposite the velocity, the entity is slowing down
            if the dot product of the acceleration and velocity is positive, the entity has
            already stopped, and is now speeding up. 

            to stop this, cancel the acceleration once the dot product is non-negative
        */
        if (o->acceleration * o->velocity >= 0.0f) {
            o->acceleration = o->velocity = Math::Zero2;
        }
        o->velocity += o->acceleration * deltaTime;
    }


    // draws a unit vector to the player and moves towards it
    void moveDirectlyToPlayer(Object::GameObject *o)
    {
        Math::Vector2 dir = Math::getUnitVector(o->centrePos, player->centrePos);
        if (o->velocity * o->acceleration > 0.0f) o->acceleration = Math::Zero2;
        o->velocity = (dir * o->speed) + (o->acceleration * deltaTime);
    }


    // position functions


    void playerPositionFunc(Object::GameObject *p)
    {
        float s = p->speed; // store in a local variable, to save time loading from memory
        if (inputKeys&16) s *= 2.0f; // bit 5 of inputKeys = shift, double move speed
        if (debuggingTools&2) s *= 2.0f; // bit 2 of debugTools = speed boost, double speed

        Math::Vector2 vel(
            // horizontal velocity: bit 1=d; move right (+x), bit 3=a; move left (-x)
            (bool(inputKeys&1)-bool(inputKeys&4)),
            // vertical velocity: bit 2=s; move down (+y), bit 4=w; move up (-y)
            (bool(inputKeys&2)-bool(inputKeys&8))
        );

        // normalise the vector so that players can't walk faster by moving diagonally
        vel.normalise(); 
        vel *= s; // multiply the unit vector by movement speed

        Math::Vector2 d = vel + p->velocity;
        p->pos += d * deltaTime;
        p->centrePos = p->pos + Math::Vector2(p->size.x/2, p->size.y/2);
        p->cell = Object::findCell(p->centrePos);
    }




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
        item->pos += (item->velocity * deltaTime);
        // update the centre position, which is in the centre of the object's hitbox
        item->centrePos = item->pos + Math::Vector2(item->size.x/2, item->size.y/2);

        // update the timer
        item->timer -= deltaTime;
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
        if (p->velocity == Math::Zero2 && (inputKeys&0x0F) == 0) p->animations.stage = 0.0f;


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
    
    // finds the best path to the player
    void pathfindToPlayer(Object::GameObject *o, int searchDepth)
    {
        // pathfind to the player
        // create a new pathfinding object
        Math::Point2 start = o->cell, end = player->cell, dimensions(grid.size(), grid[0].size());
        AStar::Pathfinding pathfinding(start, end, dimensions, searchDepth);

        // find the best path to the player
        AStar::LinkedCell *node = pathfinding.findPath();
        // reverse the list to be start to end
        o->pathNode = AStar::ReverseLinkedList(node);
        pathfinding.close();

        // count the number of nodes in the linked list
        int count = 0;
        for (AStar::LinkedCell *curr = o->pathNode; curr != nullptr; curr = curr->next) {
            count++;
        }
        o->pathfindLength = count;
    }


    void wolfUpdatePathfinding(Object::GameObject *w)
    {
        if (w->pathNode != nullptr) {
            // save the current pathfinding cell
            Math::Point2 currCell = w->pathNode->cell;

            // clean up all existing pathfinding nodes
            AStar::LinkedCell *curr = w->pathNode;
            while (curr != nullptr) {
                AStar::LinkedCell *next = curr->next;
                delete curr;
                curr = next;
            }

            // update pathfinding
            pathfindToPlayer(w, 50);

            // if the new pathfinding cell is different than the new cell, then the wolf was in
            // the process of moving to the next node, progress the node to represent this.
            // sometimes, two wolves can get each other stuck by trying to move into the same cell
            // since they push each other back and never reach the centre, they are stuck forever.
            // to break this "stalemate", give the wolf a 50% chance progress to the next node
            float random = float(rand()) / RAND_MAX;
            if (w->pathNode->cell != currCell || random < 0.5f) {
                w->pathNode = w->pathNode->next;
            }
            // reset the timer
            w->timer = 0.5f;
        
        } else pathfindToPlayer(w, 50);
    }


    void wolfWalkToNextCell(Object::GameObject *w)
    {
        // try to get to the centre of the first pathfinding cell
        Math::Point2 cell = w->pathNode->cell;
        Math::Vector2 cellPos(((float)cell.x+0.5f)*sideLen, ((float)cell.y+0.5)*sideLen);

        // get the displacement from the wolf's centre to the centre of the next cell
        Math::Vector2 disp = cellPos - w->centrePos;
        Math::Vector2 dir = Math::Zero2;
        float len = disp.length();

        // if it is close enough (within one unit), progress to the next node
        // otherwise, move towards the cell centre
        if (len <= 2.0f) {
            // make the position exact
            w->centrePos = cellPos;
            w->pos = cellPos - Math::Vector2(w->size.x/2, w->size.y/2);
            // progress to the next node
            w->pathNode = w->pathNode->next;
        } else {
            // move towards the cell's centre
            dir = disp / len;
        }
        if (w->velocity * w->acceleration > 0.0f) w->acceleration = Math::Zero2;
        w->velocity = (dir * w->speed) + (w->acceleration * deltaTime);
    }




    // functions that make switching between these functions easier
    
    // switches a wolf into it's jump attack
    void wolfEnterJumpAttack(Object::GameObject *w)
    {
        // the attack lasts 2.75 seconds
        w->timer = 2.75f;
        // give the wolf acceleration so that it will come to a stop in 0.5 seconds
        w->acceleration = w->velocity * -2.0f;
        // switch to the attack function and return
        w->velocityFunc = wolfJumpAttack;
    }
}