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
        bool flag = defaultCollideWithWorldBorders(o);
        // return if the object was moved to another scene
        if (flag) return;

        // walls
        defaultHandleSideCollisionsWithWalls(o);
        defaultHandleCornerCollisionsWithWalls(o);
    }

    // similar to default, but lets the player load different areas when walking off the map
    void playerCollisionFunction(Object::GameObject *p)
    {
        // collide with other game objects
        defaultHandleCollisionsWithGameObjects(p);
        // handle everything else AFTER other entities, to avoid being pushed into walls
        if (!p->hasCollision) return;

        // world borders
        playerCollideWithWorldBorders(p);

        // walls
        defaultHandleSideCollisionsWithWalls(p);
        defaultHandleCornerCollisionsWithWalls(p);
    }

    // pushes apart gameobjects that are too close to itself
    void defaultHandleCollisionsWithGameObjects(Object::GameObject *o)
    {
        // iterate through all the other game objects too find the distance between them
        for (int i = 0; i < gameObjects.size(); i++) 
        {
            Object::GameObject *obj = gameObjects[i];
            // doesn't collide with itself or objects that have no collision
            if (obj==o || !obj->hasCollision) continue;

            // find the displacement vector between the two objects
            Math::Vector2 disp = obj->centrePos - o->centrePos;
            // break up objects directly on each other
            if (disp == Math::Zero2) disp = Math::One2;

            // d is the magnitude of the displacement,
            // r is the sum of the radii of the two objects
            float d = disp.length(), r = o->radius+obj->radius;
            // if d < r, the objects are too close to each other and need to be pushed apart

            if (d < r) 
            {
                // r - d represents how big the overlap is. to move both objects evenly,
                // they need to move (r-d)/2 in opposite direction
                // normalise the displacement to find the direction the objects need to move
                disp.normalise();

                // knock objects apart
                o->velocity += disp * -15.0f;
                o->acceleration = o->velocity * -1.5f;

                obj->velocity += disp * 15.0f;
                obj->acceleration = obj->velocity * -1.5f;

                // objects damage each other
                if (obj->type < Object::Wolf || obj->type > Object::Wolf) {
                    o->hp -= obj->damage;
                    obj->hp -= o->damage;

                }
            } 
        }
    }

    // makes sure the object cannot walk out of bounds
    bool defaultCollideWithWorldBorders(Object::GameObject *o)
    {
        if (o->pos.x<0.0f) {
            if (currLevel->left != nullptr) {
                Game::MoveEntityToLevel(o, currLevel->left, currLevel->posLeft);
                return true; // true for moving to another scene
            } else o->pos.x = 0.0f;
        }
        else if (o->pos.x>bkgWidth-o->size.x) {
            if (currLevel->right != nullptr) {
                Game::MoveEntityToLevel(o, currLevel->right, currLevel->posRight);
                return true; // true for moving to another scene
            } else o->pos.x = bkgWidth-o->size.x-1.0f;
        }
        if (o->pos.y<0.0f) {
            if (currLevel->above != nullptr) {
                Game::MoveEntityToLevel(o, currLevel->above, currLevel->posAbove);
                return true; // true for moving to another scene
            } else o->pos.y = 0.0f;
        }
        else if (o->pos.y>bkgHeight-o->size.y) {
            if (currLevel->below != nullptr) {
                Game::MoveEntityToLevel(o, currLevel->below, currLevel->posBelow);
                return true; // true for moving to another scene
            } else o->pos.y = bkgHeight-o->size.y-1.0f;
        }
        return false; // did not move to another scene
    }

    // loads different areas based on where the player walks
    void playerCollideWithWorldBorders(Object::GameObject *p)
    {
        // find the active level, and assign globals into that level object
        // i wish i could use a switch statement here, but strings cannot be used
        // in switch statements. else if chain it is :(

        if (p->pos.x<0.0f) {
            if (currLevel->left != nullptr) {
                Game::movePlayerToNewLevel(currLevel->left, currLevel->posLeft);
                return;
            } else p->pos.x = 0.0f;
        } else if (p->pos.x>bkgWidth-p->size.x) {
            if (currLevel->right != nullptr) {
                Game::movePlayerToNewLevel(currLevel->right, currLevel->posRight);
                return;
            } else p->pos.x = bkgWidth-p->size.x-1.0f;
        }
        if (p->pos.y<0.0f) {
            if (currLevel->above != nullptr) {
                Game::movePlayerToNewLevel(currLevel->above, currLevel->posAbove);
                return;
            } else p->pos.y = 0.0f;
        } else if (p->pos.y>bkgHeight-p->size.y) {
            if (currLevel->below != nullptr) {
                Game::movePlayerToNewLevel(currLevel->below, currLevel->posBelow);
                return;
            } else p->pos.y = bkgHeight-p->size.y-1.0f;
        }
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
            if (grid[p.x][p.y]&BARRIER) {
                o->pos.y = (p.y+1)*sideLen;
                // if the enetity does damage, damage the barrier it hit
                if (o->velocity.y != 0.0f) {
                    Game::damageCell(p, o->damage);
                    o->acceleration.y = o->velocity.y = 0.0f;
                }
            }
        }
        // repeat process for other sides

        // bottom side, move up
        for (int i = 1; i < o->cellularDimensions.X; i++) {
            Math::Vector2 node(o->pos.x+o->cellularDimensions.Width*i, o->pos.y+o->size.y);
            Math::Point2 p = Object::findCell(node);
            if (grid[p.x][p.y]&BARRIER) {
                o->pos.y = p.y*sideLen - o->size.y;
                if (o->velocity.y != 0.0f) {
                    Game::damageCell(p, o->damage);
                    o->acceleration.y = o->velocity.y = 0.0f;
                }
            }
        }
        // left side, move right
        for (int i = 1; i < o->cellularDimensions.X; i++) {
            Math::Vector2 node(o->pos.x, o->pos.y+o->cellularDimensions.Height*i);
            Math::Point2 p = Object::findCell(node);
            if (grid[p.x][p.y]&BARRIER) {
                o->pos.x = (p.x+1)*sideLen;
                if (o->velocity.x != 0.0f) {
                    Game::damageCell(p, o->damage);
                    o->acceleration.x = o->velocity.x = 0.0f;
                }
            }
        }
        // right side, move left
        for (int i = 1; i < o->cellularDimensions.X; i++) {
            Math::Vector2 node(o->pos.x+o->size.y, o->pos.y+o->cellularDimensions.Height*i);
            Math::Point2 p = Object::findCell(node);
            if (grid[p.x][p.y]&BARRIER) {
                o->pos.x = p.x*sideLen - o->size.x;
                if (o->velocity.x != 0.0f) {
                    Game::damageCell(p, o->damage);
                    o->acceleration.x = o->velocity.x = 0.0f;
                }
            }
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
        
        if (disp != Math::Zero2) {
            bool flag = false;
            if (Math::absf(disp.x)<Math::absf(disp.y)) {
                o->pos.x += disp.x + Math::signf(disp.x);
                if (o->velocity.x != 0.0f) {
                    flag = true;
                    o->velocity.x = o->acceleration.x = 0.0f;
                }
            } else {
                o->pos.y += disp.y + Math::signf(disp.y);
                if (o->velocity.y != 0.0f) {
                    flag = true;
                    o->velocity.y = o->acceleration.y = 0.0f;
                }
            }
            if (flag) Game::damageCell(p, o->damage);
        }
  

        // top right
        p = Object::findCell(o->pos+Math::Vector2(o->size.x,0.0f));
        disp = Math::Zero2;
        if (grid[p.x][p.y]&BARRIER)
        // displacement between TOP RIGHT corner of hitbox and BOTTOM LEFT corner of cell
            disp =  (Math::Vector2(p.x,p.y+1.0f)*sideLen)-
                    (o->pos+Math::Vector2(o->size.x,0.0f));

        if (disp != Math::Zero2) {
            bool flag = false;
            if (Math::absf(disp.x)<Math::absf(disp.y)) {
                o->pos.x += disp.x + Math::signf(disp.x);
                if (o->velocity.x != 0.0f) {
                    flag = true;
                    o->velocity.x = o->acceleration.x = 0.0f;
                }
            } else {
                o->pos.y += disp.y + Math::signf(disp.y);
                if (o->velocity.y != 0.0f) {
                    flag = true;
                    o->velocity.y = o->acceleration.y = 0.0f;
                }
            }
            if (flag) Game::damageCell(p, o->damage);
        }
 

        // bottom left
        p = Object::findCell(o->pos+Math::Vector2(0.0f,o->size.y));
        disp = Math::Zero2;
        if (grid[p.x][p.y]&BARRIER)
        // displacement between BOTTOM LEFT corner of hitbox and TOP RIGHT corner of cell
            disp =  (Math::Vector2(p.x+1.0f,p.y)*sideLen)-
                    (o->pos+Math::Vector2(0.0f,o->size.y));

        if (disp != Math::Zero2) {
            bool flag = false;
            if (Math::absf(disp.x)<Math::absf(disp.y)) {
                o->pos.x += disp.x + Math::signf(disp.x);
                if (o->velocity.x != 0.0f) {
                    flag = true;
                    o->velocity.x = o->acceleration.x = 0.0f;
                }
            } else {
                o->pos.y += disp.y + Math::signf(disp.y);
                if (o->velocity.y != 0.0f) {
                    flag = true;
                    o->velocity.y = o->acceleration.y = 0.0f;
                }
            }
            if (flag) Game::damageCell(p, o->damage);
        }


        // bottom right
        p = Object::findCell(o->pos+Math::Vector2(o->size.x, o->size.y));
        disp = Math::Zero2;
        if (grid[p.x][p.y]&BARRIER)
        // displacement between BOTTOM RIGHT of hitbox and TOP LEFT of cell
            disp =  (Math::Vector2(p.x,p.y)*sideLen)-
                    (o->pos+Math::Vector2(o->size.x,o->size.y));
        if (disp != Math::Zero2) {
            bool flag = false;
            if (Math::absf(disp.x)<Math::absf(disp.y)) {
                o->pos.x += disp.x + Math::signf(disp.x);
                if (o->velocity.x != 0.0f) {
                    flag = true;
                    o->velocity.x = o->acceleration.x = 0.0f;
                }
            } else {
                o->pos.y += disp.y + Math::signf(disp.y);
                if (o->velocity.y != 0.0f) {
                    flag = true;
                    o->velocity.y = o->acceleration.y = 0.0f;
                }
            }
            if (flag) Game::damageCell(p, o->damage);
        }
    }



    // for an item sitting still, collide with non-items normally, but when colliding
    // with another item, combine stacks, if possible
    void stationaryItemCollisionFunction(Object::GameObject *item)
    {
        // do nothing if the object doesn't have collision, such as when it's being held
        if (!item->hasCollision) return;

        itemsHandleCOllisionsWithGameObjects(item);

        // collide with other game objects
        // defaultHandleCollisionsWithGameObjects(item);
        // handle everything else AFTER other entities, to avoid being pushed into walls/oob

        // world borders
        bool flag = defaultCollideWithWorldBorders(item);
        // return if the object was moved to another scene
        if (flag) return;

        // walls
        defaultHandleSideCollisionsWithWalls(item);
        defaultHandleCornerCollisionsWithWalls(item);
    }


    void itemsHandleCOllisionsWithGameObjects(Object::GameObject *item)
    {
        for (int i = 0; i < gameObjects.size(); i++)
        {
            Object::GameObject *other = gameObjects[i];
            // doesn't collide with itself or objects that have no collision
            if (other==item || !other->hasCollision) continue;

            // find the displacement vector between the two objects
            Math::Vector2 disp = other->centrePos - item->centrePos;
            // break up objects directly on each other
            if (disp == Math::Zero2) disp = Math::One2;

            // d is the magnitude of the displacement,
            // r is the sum of the radii of the two objects
            float d = disp.length(), r = item->radius+other->radius;
            // if d < r, the objects are too close to each other and need to be pushed apart

            if (d < r) 
            {
                // collided with the same item, attempt to combine
                if (other->type >= Object::Log_Item) {
                    if (other->type == item->type) {
                        // add the other stack's item to this stack's, as far as is possible
                        // items store their count in their hp variable

                        int spaceAvailable = item->maxHP-item->hp; // how many more items can be held
                        
                        // number of items being transfered
                        int transfer = Math::Min(other->hp, spaceAvailable);

                        // transfer the items
                        item->hp += transfer;
                        other->hp -= transfer;
                    
                    // not the same item type, attempt to craft with the other item stack
                    } else Object::craftTwoItems(item, other);
                }

                // r - d represents how big the overlap is. to move both objects evenly,
                // they need to move (r-d)/2 in opposite direction
                // normalise the displacement to find the direction the objects need to move
                disp.normalise();

                // if the other item is an enemy
                if (other->type >= Object::Wolf && other->type <= Object::Wolf) {
                    // if the item is moving, damage the enemy it hit
                    // make sure that the item is moving fast enough
                    if (item->velocity.length() > 35.0f && item->timer <= 0.0f) {
                        // calculate the damage by multiplying the speed by the number of items in the stack
                        int damage = Math::ceilToInt(item->hp * item->speed);
                        // damage the enemy
                        other->hp -= damage;
                    } 
                    item->timer = 0.75f;
                }

                // knock objects apart
                item->velocity += disp * -15.0f;
                item->acceleration = item->velocity * -2.0f;

                other->velocity += disp * 15.0f;
                other->acceleration = other->velocity * -2.0f;
            }
        }
    }


}

// developer tool, draws the collision boundaries for the game object
void Object::GameObject::DrawHitbox(Gdiplus::Graphics& graphics, int dotSize, Math::Point2 screenPos) {
        int d = dotSize/2;
        Math::Point2 centreScreenPos = Math::Point2(screenPos.x+size.x/2, screenPos.y+size.y/2);
        Gdiplus::SolidBrush redBrush(red), blueBrush(blue);
        Gdiplus::Pen pen(blue), pen2(green), pen3(red);

        // outline bounding box
        graphics.DrawRectangle(&pen, screenPos.x, screenPos.y, size.x, size.y);
        // centre of bounding box
        graphics.FillEllipse(&blueBrush, centreScreenPos.x-d, centreScreenPos.y-d, dotSize, dotSize);
        

        // draw arrow in direction of velocity
        Math::Vector2 centrePosVec(centreScreenPos.x, centreScreenPos.y);
        Math::Vector2 p = centrePosVec + velocity*0.4f;
        graphics.DrawLine(&pen2, Gdiplus::Point(centreScreenPos.x, centreScreenPos.y), Gdiplus::Point(p.x, p.y));
        
        // draw arrow in direction of acceleration
        p = centrePosVec + acceleration*0.4f;
        graphics.DrawLine(&pen3, Gdiplus::Point(centreScreenPos.x, centreScreenPos.y), Gdiplus::Point(p.x, p.y));

        // for visualising collision algorithms
        if (!hasCollision) return;
        // green circle represents collision radius for game objects
        graphics.DrawEllipse(&pen2, centreScreenPos.x-radius, centreScreenPos.y-radius, 2*radius, 2*radius);

        // red dots represent points where the game checks for wall collisions
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
    }