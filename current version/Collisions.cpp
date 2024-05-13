#include "Collisions.hpp"

using namespace Globals;

namespace Collisions
{
    // detects collisions with world borders, walls, and other game objects
    void defaultCollisionFunction(Object::GameObject *o)
    {
        // do nothing if the object doesn't have collision
        if (!o->hasCollision) return;

        // collide with other game objects
        defaultHandleCollisionsWithGameObjects(o);
        // handle everything else AFTER other entities, to avoid being pushed into walls/oob

        // world borders
        defaultCollideWithWorldBorders(o);

        // walls
        defaultHandleSideCollisionsWithWalls(o);
        defaultHandleCornerCollisionsWithWalls(o);
    }

    // pushes apart gameobjects that are too close to itself
    void defaultHandleCollisionsWithGameObjects(Object::GameObject *o)
    {
        // iterate through all the other game objects too find the distance between them
        for (int i = 0; i < gameObjects.size(); i++) 
        {
            // doesn't collide with itself or objects that have no collision
            if (gameObjects[i]==o || !gameObjects[i]->hasCollision) continue;

            // find the displacement vector between the two objects
            Math::Vector2 disp = gameObjects[i]->centrePos - o->centrePos;
            // break up objects directly on each other
            if (disp == Math::Zero2) disp = Math::One2;

            // d is the magnitude of the displacement,
            // r is the sum of the radii of the two objects
            float d = disp.length(), r = o->radius+gameObjects[i]->radius;
            // if d < r, the objects are too close to each other and need to be pushed apart

            if (d < r) 
            {
                // r - d represents how big the overlap is. to move both objects evenly,
                // they need to move (r-d)/2 in opposite direction
                // normalise the displacement to find the direction the objects need to move
                disp.normalise();
                // multiply the unit vector by the distance they need to move
                disp = disp * (r-d)/2;

                // update the position of both objects
                // use - for o itself and + for the other object, so they move APART
                o->pos = o->pos - disp;
                o->centrePos = o->centrePos - disp;
                gameObjects[i]->pos = gameObjects[i]->pos + disp;
            } 
        }
    }

    // makes sure the object cannot walk out of bounds
    void defaultCollideWithWorldBorders(Object::GameObject *o)
    {
        if (o->pos.x<0.0f) o->pos.x = 0.0f;
        else if (o->pos.x>bkgWidth-o->size.x) o->pos.x = bkgWidth-o->size.x-1.0f;
        if (o->pos.y<0.0f) o->pos.y = 0.0f;
        else if (o->pos.y>bkgHeight-o->size.y) o->pos.y = bkgHeight-o->size.y-1.0f;
    }

    // checks if the hitbox is within a wall, and pushes it out
    void defaultHandleSideCollisionsWithWalls(Object::GameObject *o)
    {
        // for collision points on the side of the hitbox, check if they are within
        // a barrier cell, and push them out, colely along the appropriate axis.
        // for instance, if a collision point on the hitbox's left edge is within a barrier,
        // push the entity RIGHT, along the x axis to move it out

        // top side, move down
        for (int i = 1; i < o->cellularDimensions.X; i++) {
            // position of the current collision node
            Math::Vector2 node(o->pos.x+o->cellularDimensions.Width*i, o->pos.y);
            // cell of the collision node
            Math::Point2 p = Object::findCell(node);
            // if it's a barrier, set the position to just outside the barrier cell
            if (grid[p.x][p.y]&BARRIER) o->pos.y = (p.y+1)*sideLen;
        }
        // repeat process for other sides

        // bottom side, move up
        for (int i = 1; i < o->cellularDimensions.X; i++) {
            Math::Vector2 node(o->pos.x+o->cellularDimensions.Width*i, o->pos.y+o->size.y);
            Math::Point2 p = Object::findCell(node);
            if (grid[p.x][p.y]&BARRIER) o->pos.y = p.y*sideLen - o->size.y;
        }
        // left side, move right
        for (int i = 1; i < o->cellularDimensions.X; i++) {
            Math::Vector2 node(o->pos.x, o->pos.y+o->cellularDimensions.Height*i);
            Math::Point2 p = Object::findCell(node);
            if (grid[p.x][p.y]&BARRIER) o->pos.x = (p.x+1)*sideLen;
        }
        // right side, move left
        for (int i = 1; i < o->cellularDimensions.X; i++) {
            Math::Vector2 node(o->pos.x+o->size.y, o->pos.y+o->cellularDimensions.Height*i);
            Math::Point2 p = Object::findCell(node);
            if (grid[p.x][p.y]&BARRIER) o->pos.x = p.x*sideLen - o->size.x;
        }
    }

    // checks if the hitbox is within a wall, and pushes it out
    void defaultHandleCornerCollisionsWithWalls(Object::GameObject *o)
    {
        /*
        for each of the four corners: find the cell that the corner lies in
        if that cell is a barrier, then find the displacement
        between the hitbox corner itself, and the corner of the cell nearest to the 
        entity's centre. 

        this displacement vector represents how far into the barrier the entity is. 
        instead of just pushing the game object out with this vector directly, 
        find the smaller of the two terms, then push the object out ONLY by that term.

        so, if the x component of the displacement is 0.5, and the y component is -3,
        |0.5| < |-3|, so ONLY move the entity along the x axis by 0.5
        this will get the entity out of the wall hitbox, without causing it to interact
        with corners in an unintuitive way
        */

        /*
        Object::findCell returns indices of said cell. 
        findCell * sideLen is equal to the coordinates of the top left corner of the cell
        (findCell + (1, 1)) * sideLen is the coordinates of the bottom right corner

        'pos' member represents the position of the hitbox's top left corner. 'size' 
        represents the dimensions of the hitbox. pos + (size.x, size.y) is the position
        of the bottom right corner of the hitbox
        */


        // top left
        Math::Vector2 disp = Math::Zero2;
        Math::Point2 p = Object::findCell(o->pos);
        if (grid[p.x][p.y]&BARRIER) 
            // displacement bettwen TOP LEFT corner of hitbox and BOTTOM RIGHT corner of cell
            disp = ((Math::Vector2(p.x,p.y)+Math::One2)*sideLen)-o->pos;
        
        if (Math::absf(disp.x)<Math::absf(disp.y)) o->pos.x += disp.x;
        else o->pos.y += disp.y;
  

        // top right
        p = Object::findCell(o->pos+Math::Vector2(o->size.x,0.0f));
        disp = Math::Zero2;
        if (grid[p.x][p.y]&BARRIER)
        // displacement between TOP RIGHT corner of hitbox and BOTTOM LEFT corner of cell
            disp =  (Math::Vector2(p.x,p.y+1.0f)*sideLen)-
                    (o->pos+Math::Vector2(o->size.x,0.0f));

        if (Math::absf(disp.x)<Math::absf(disp.y)) o->pos.x += disp.x;
        else o->pos.y += disp.y;
 

        // bottom left
        p = Object::findCell(o->pos+Math::Vector2(0.0f,o->size.y));
        disp = Math::Zero2;
        if (grid[p.x][p.y]&BARRIER)
        // displacement between BOTTOM LEFT corner of hitbox and TOP RIGHT corner of cell
            disp =  (Math::Vector2(p.x+1.0f,p.y)*sideLen)-
                    (o->pos+Math::Vector2(0.0f,o->size.y));

        if (Math::absf(disp.x)<Math::absf(disp.y)) o->pos.x += disp.x;
        else o->pos.y += disp.y;
        

        // bottom right
        p = Object::findCell(o->pos+Math::Vector2(o->size.x, o->size.y));
        disp = Math::Zero2;
        if (grid[p.x][p.y]&BARRIER)
        // displacement between BOTTOM RIGHT of hitbox and TOP LEFT of cell
            disp =  (Math::Vector2(p.x,p.y)*sideLen)-
                    (o->pos+Math::Vector2(o->size.x,o->size.y));
        if (Math::absf(disp.x)<Math::absf(disp.y)) o->pos.x += disp.x;
        else o->pos.y += disp.y;
    }
}

// developer tool, draws the collision boundaries for the game object
void Object::GameObject::DrawHitbox(Gdiplus::Graphics& graphics, int dotSize, Math::Point2 screenPos) {
        int d = dotSize/2;
        Math::Point2 centreScreenPos = Math::Point2(screenPos.x+size.x/2, screenPos.y+size.y/2);
        Gdiplus::SolidBrush redBrush(red), blueBrush(blue);
        Gdiplus::Pen pen(blue), pen2(green), pen3(red);

        graphics.DrawRectangle(&pen, screenPos.x, screenPos.y, size.x, size.y);
        graphics.DrawEllipse(&pen2, centreScreenPos.x-radius, centreScreenPos.y-radius, 2*radius, 2*radius);

        // top side
        for (int i = 0; i <= cellularDimensions.X; i++) {
            int x = screenPos.x+cellularDimensions.Width*i-d,
                y = screenPos.y-d;
            graphics.FillEllipse(&redBrush, x, y, dotSize, dotSize);
        }
        // bottom side
        for (int i = 0; i <= cellularDimensions.X; i++) {
            int x = screenPos.x+cellularDimensions.Width*i-d,
                y = screenPos.y+size.y-d;
            graphics.FillEllipse(&redBrush, x, y, dotSize, dotSize);
        }
        // left side
        for (int i = 1; i <= cellularDimensions.Y; i++) {
            int y = screenPos.y+cellularDimensions.Height*i-d,
                x = screenPos.x-d;
            graphics.FillEllipse(&redBrush, x, y, dotSize, dotSize);
        }
        // right side
        for (int i = 1; i <= cellularDimensions.Y; i++) {
            int y = screenPos.y+cellularDimensions.Height*i-d,
                x = screenPos.x+size.x-d;
            graphics.FillEllipse(&redBrush, x, y, dotSize, dotSize);
        }
        graphics.FillEllipse(&blueBrush, centreScreenPos.x-d, centreScreenPos.y-d, dotSize, dotSize);

        // draw arrow in direction of velocity
        Math::Vector2 centrePosVec(centreScreenPos.x, centreScreenPos.y);
        Math::Vector2 p = centrePosVec + velocity/2.5f;

        graphics.DrawLine(&pen2, Gdiplus::Point(centreScreenPos.x, centreScreenPos.y), 
        Gdiplus::Point(p.x, p.y));
    }