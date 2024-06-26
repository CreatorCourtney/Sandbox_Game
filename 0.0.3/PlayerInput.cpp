#include "PlayerInput.hpp"

using namespace Globals;

namespace Input
{
    // place an object into a cell in the global grid
    int PlaceObjectInCell(Math::Point2 cell, int objType, bool updateBkgBrush) 
    {
        // cell is out of bounds and doesn't exist
        if (cell.x<0||cell.x>=grid.size()||cell.y<0||cell.y>=grid[0].size()) return -1;

        // update background image for drawing,
        // img is the image that will be placed into the cell
        Gdiplus::Image* img = nullptr;
        int x = cell.x*sideLen, y = cell.y*sideLen;

        switch (objType)
        {
            case TREE:
                // empty grass tile drawn where the base is
                img = emptyImg;
                
                // add a tree image to the overlay layer
                Frame::AddTreeToOverlay(cell, updateBkgBrush);

                // set the cell space to be a tree
                grid[cell.x][cell.y] |= TREE;
                break;
                

            case LOG: 
                // cell is occupied
                if (grid[cell.x][cell.y]&0x4000) return 1;

                img = logImg; // image drawn will be a log
                grid[cell.x][cell.y] |= LOG;
                break;


            case BRIDGE:
                // cell is occupied
                if (grid[cell.x][cell.y]&0x4000) return 1;
                img = bridgeImg;
                grid[cell.x][cell.y] |= BRIDGE;
                grid[cell.x][cell.y] &= ~0x2000; // turn off barrier bit
                break;


            case STUMP:
                // draw the empty tile first, in case of loading a map with a stump, but not a tree
                Frame::DrawImageToBitmap(background, emptyImg, x, y);

                // only occurs when a tree gets removed, thus the cell could only have been TREE prior.
                // so, there is no issue just settig the cell to STUMP, no need for |=
                img = stumpImg;
                grid[cell.x][cell.y] = STUMP;
                break;


            case EMPTY: 
            {
                // remove building in the cell
                // if it IS indestructible or NOT occupied, return
                if (grid[cell.x][cell.y]&0x8000||!grid[cell.x][cell.y]&0x4000) return 2;
                // this number can reenable specified bits, like in case 3
                int num = 0;
                
                // first byte represents info about the cell, this is the object being removed
                switch (grid[cell.x][cell.y]&255)
                {
                    case 1: { // log
                        // spawn a log
                        Math::Vector2 pos(cell.x*sideLen, cell.y*sideLen);
                        Object::spawnItemStack(Object::Log_Item, pos, 1);
                        break;
                    }

                    case 2: // bridge
                        break;

                    case 3: { // tree

                        // remove tree image from the overlay layer
                        Frame::RemoveTreeFromOverlay(cell);

                        // spawn a falling tree object in its place
                        Object::Instantiate(Object::Falling_Tree, 
                            Math::Vector2((cell.x-1)*sideLen, (cell.y-9)*sideLen), 1);

                        // place a stump where the tree once stood
                        // everything else handled in the STUMP case. don't break, just return
                        return PlaceObjectInCell(cell, STUMP);
                    }

                    case 4: { // stump
                        // spawn a log
                        Math::Vector2 pos(cell.x*sideLen, cell.y*sideLen);
                        Object::spawnItemStack(Object::Log_Item, pos, 1);
                        break;
                    }

                    default: break;
                }

                // if the cell should be water by default, cover the cell with the water image
                // otherwise, cover it with the empty grass image
                if (grid[cell.x][cell.y]&WATER) img = waterImg;
                else img = emptyImg;

                // preserve bits 16 (indestructible), and 13, 17-24 (water), 
                // all others to 0
                // bit 14: on if water (bit 13), off otherwise
                grid[cell.x][cell.y] &= 0xFF9000;
                if (grid[cell.x][cell.y]&WATER) grid[cell.x][cell.y] |= 0x2000;
                else grid[cell.x][cell.y] &= ~0x2000;
                break;
            }

            default: 
                // if the cell should be water, make it so, otherwise make it grass
                if (grid[cell.x][cell.y]&WATER) img = waterImg;
                else img = emptyImg;
                break;
        }

        // draw the object placed onto the image bitmap
        if (img != nullptr) Frame::DrawImageToBitmap(background, img, x, y);

        // update the background brush, so that the change is obervable
        if (updateBkgBrush) {
            // create a new brush
            bkgBrush = new Gdiplus::TextureBrush(background);
            // set the wrap mode to only draw the background once
            bkgBrush->SetWrapMode(Gdiplus::WrapModeClamp);
        }

        // 0 for successful placement
        return 0; 
    }

    // for convenience
    int PlaceObjectInCell(Math::Point2 cell, int objType) {
        return PlaceObjectInCell(cell, objType, true);
    }



    // determines if the point, p, is within the rect region
    bool isInRegion(Math::Vector2 p, Gdiplus::Rect rect)
    {
        // right boundary and bottom boundary
        float right = rect.X+rect.Width, bottom = rect.Y+rect.Height;

        // if within the X bound of region AND within Y bound of region
        return p.x>=rect.X && p.x<=right && p.y>=rect.Y && p.y<=bottom;
    }


    // if the player is holding an object, throw it, otherwise try to pick one up
    void leftClickFunc(int x, int y)
    {
        // update global mouse pos, in case it somehow got misaligned ¯\_(ツ)_/¯
        mousePos = {x, y};
        mousePosREAL = Object::getWorldPosition(mousePos);

        // if an item is NOT held, the player may pick up an item stack
        if (heldObject == nullptr) 
        {
            // check all the game objects, to see which are items,
            // and if they are clicked
            for (int i = 0; i < gameObjects.size(); i++) 
            {
                Object::GameObject *obj = gameObjects[i];

                // distance between object and player
                float r = (obj->centrePos - player->centrePos).length();
                if (r > interactRange) continue; // cannot interact with an object out of range

                // rect representing the item stack entity
                Gdiplus::Rect rect(obj->pos.x,  obj->pos.y, obj->size.x, obj->size.y);

                // entity was clicked
                if (isInRegion(mousePosREAL, rect))
                {
                    Object::EntityType type = obj->type;

                    // entity is an item stack
                    if (obj->type >= Object::Log_Item)
                    {
                        // reset any existing velocity/acceleration
                        obj->velocity = obj->acceleration = Math::Zero2;

                        // update the behaviour functions, so that the stack will be held by the player
                        obj->positionFunc = Func::heldItemPositionFunc;

                        // update the global held object
                        heldObject = obj;

                        // make the player the object's owner
                        obj->owner = player;
                        player->ownedObjects.push_back(obj);


                        // update the building type the player tries to place
                        switch (type)
                        {
                            case Object::Log_Item:
                                // player is building with logs
                                buildingType = LOG;
                                break;
                        }

                        // exit the function, the item was picked up
                        return;
                    }
                }
            }
        } else { // if an item IS held, throw it :)
        
            // release the held object
            Object::GameObject *obj = heldObject;
            heldObject = nullptr;

            // initiate the velocity in the direction of the mouse
            Math::Vector2 dir = Math::getUnitVector(obj->centrePos, mousePosREAL);
            // add player's velocity, so you can actually throw faster 
            // when throwing in your direction of motion
            obj->velocity = player->velocity + (dir*100.0f);

            // set the acceleration, so it will eventually stop
            // set proportionally to the object's velocity
            obj->acceleration = obj->velocity * -0.7f;

            // set the position function back to normal
            obj->positionFunc = Func::thrownItemPositionFunc;


            // remove obj from the player's owned objects
            // look through the owned objects vector
            int n = obj->owner->ownedObjects.size();
            for (int i = 0; i < n; i++) {
                // object found
                if (obj->owner->ownedObjects[i] == obj) {
                    // remove the object from the vector
                    obj->owner->ownedObjects.erase(obj->owner->ownedObjects.begin()+i);
                    // exit the loop
                    break;
                }
            }
            // remove the player as the objec't owner
            obj->owner = nullptr;
            
            // player can no longer build anything, set the building type back to EMPTY
            buildingType = EMPTY;
        }
    }


    // if the player IS holding an item, attempt to place it, otherwise, remove a building from a cell
    void rightClickFunc(int x, int y)
    {
        // update global mouse pos, in case it somehow got misaligned ¯\_(ツ)_/¯
        mousePos = {x, y};
        mousePosREAL = Object::getWorldPosition(mousePos);

        // distance from the player to the cell they tried to click
        float r = (mousePosREAL - player->centrePos).length();
        if (r > interactRange) return; // clicked too far away, do nothing

        // find the cell the player selected
        Math::Point2 cell = Object::findCell(mousePosREAL);

        // place the held building into the cell
        int status = PlaceObjectInCell(cell, buildingType);

        // upon successful placement, decrease the held item's hp
        if (status == 0 && heldObject != nullptr) heldObject->hp--;
    }
}