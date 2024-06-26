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
        int x = cell.x*sideLen, y = cell.y*sideLen;
        
        // the value that will be returned. negative for unsuccessful execution,
        // zero mean you've place a cell, otherwise, when removing a cell, return the 
        // type removed
        int ext = 0;

        switch (objType & 255) // just the first byte, containing cell ID
        {
            case 3: // tree

                // empty grass tile drawn where the base is
                Frame::DrawImageToBitmap(background, emptyImg, x, y);
                
                // add a tree image to the overlay layer
                Frame::AddTreeToOverlay(cell, updateBkgBrush);

                // set the cell space to be a tree
                // use = (not |=) because previous data SHOULD be overwritten; if the tree grew
                // from a sapling, the cell should no longer contain any data about the sapling
                grid[cell.x][cell.y] = TREE;
                break;
                

            case 1: // log 

                // cell is occupied, AND the placement was made by player, not loading
                if (grid[cell.x][cell.y]&0x4000 && updateBkgBrush) return -2;

                // draw the background of the cell first
                if (grid[cell.x][cell.y]&WATER) Frame::DrawWaterToCell(cell, x, y);
                else Frame::DrawImageToBitmap(background, emptyImg, x, y);

                // draw a log into the cell
                Frame::DrawImageToBitmap(background, logImg, x, y);
                
                // update the grid value to represent a log
                grid[cell.x][cell.y] |= LOG;
                break;


            case 2: // bridge

                // cell is occupied, AND the placement was made by player, not loading
                if (grid[cell.x][cell.y]&0x4000 && updateBkgBrush) return -2;

                // draw the background of the cell first
                if (grid[cell.x][cell.y]&WATER) Frame::DrawWaterToCell(cell, x, y);
                else Frame::DrawImageToBitmap(background, emptyImg, x, y);

                // draw a bridge to the cell
                Frame::DrawImageToBitmap(background, bridgeImg, x, y);

                // update the grid value to represent a bridge
                grid[cell.x][cell.y] |= BRIDGE;
                grid[cell.x][cell.y] &= ~BARRIER; // turn off barrier bit
                break;


            case 4: // stump

                // draw the empty tile first, in case of loading a map with a stump, but not a tree
                Frame::DrawImageToBitmap(background, emptyImg, x, y);

                // draw a stump to the cell
                Frame::DrawImageToBitmap(background, stumpImg, x, y);

                // only occurs when a tree gets removed, thus the cell could only have been TREE prior.
                // so, there is no issue just settig the cell to STUMP, no need for |=
                grid[cell.x][cell.y] = STUMP;
                break;

            case 5: { // sapling

                // cell is occupied or water, AND the placement was made by player, not loading
                if (grid[cell.x][cell.y]&0x5000 && updateBkgBrush) return -2;

                // look to see if the cell is already in the timed cells vector
                int idx = findPointIndexInVector(cell, timedCells);

                // the cell doesn't exist in timed cells
                if (idx == -1) {
                    // add the cell to timed cells
                    timedCells.push_back(cell);
                } else {
                    // clear data about cell timer & health
                    grid[cell.x][cell.y] &= ~(TIMER|HEALTH);
                }

                // draw the empty tile first
                Frame::DrawImageToBitmap(background, emptyImg, x, y);

                // draw a sapling to the cell
                Frame::DrawImageToBitmap(background, saplingImg, x, y);

                // update the grid to represent a sapling
                grid[cell.x][cell.y] |= SAPLING;
                break;
            }

            case 6: { // world border
                Gdiplus::Graphics graphics(background);
                // make the cell empty (black)
                Gdiplus::SolidBrush *blackBrush = new Gdiplus::SolidBrush(black);
                graphics.FillRectangle(blackBrush, x, y, (INT)sideLen, (INT)sideLen);
                // cleanup
                delete blackBrush;

                // update the grid to represent a border
                grid[cell.x][cell.y] = BORDER;
                break;
            }

            case EMPTY: 
            {
                // cannot remove cells while holding an object
                if (heldObject != nullptr && updateBkgBrush) return -4;
                // remove building in the cell
                // if it IS indestructible or NOT occupied, return
                if (grid[cell.x][cell.y]&0x8000||!grid[cell.x][cell.y]&0x4000) return -3;
                
                
                // first byte represents info about the cell, this is the object being removed
                switch (grid[cell.x][cell.y]&255)
                {
                    case 1: { // log
                        // spawn a log
                        Math::Vector2 pos(x, y);
                        Object::spawnItemStack(Object::Log_Item, pos, 1);

                        // if the cell should be water by default, cover the cell with the water image
                        // otherwise, cover it with the empty grass image
                        if (grid[cell.x][cell.y]&WATER) Frame::DrawWaterToCell(cell, x, y);
                        else Frame::DrawImageToBitmap(background, emptyImg, x, y);

                        break;
                    }

                    case 2: { // bridge
                        // spawn a bridge
                        Math::Vector2 pos(x, y);
                        Object::spawnItemStack(Object::Bridge_Item, pos, 1);

                        // if the cell should be water by default, cover the cell with the water image
                        // otherwise, cover it with the empty grass image
                        if (grid[cell.x][cell.y]&WATER) Frame::DrawWaterToCell(cell, x, y);
                        else Frame::DrawImageToBitmap(background, emptyImg, x, y);
                        break;
                    }

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

                        // if the cell should be water by default, cover the cell with the water image
                        // otherwise, cover it with the empty grass image
                        if (grid[cell.x][cell.y]&WATER) Frame::DrawWaterToCell(cell, x, y);
                        else Frame::DrawImageToBitmap(background, emptyImg, x, y);
                        break;
                    }

                    case 5: { // sapling
                        // spawn a pine cone
                        Math::Vector2 pos(cell.x*sideLen, cell.y*sideLen);
                        Object::spawnItemStack(Object::Pine_Cone_Item, pos, 1);

                        // remove the cell from the timed cells vector
                        int idx = findPointIndexInVector(cell, timedCells);
                        if (idx != -1) timedCells.erase(timedCells.begin() + idx);

                        // if the cell should be water by default, cover the cell with the water image
                        // otherwise, cover it with the empty grass image
                        if (grid[cell.x][cell.y]&WATER) Frame::DrawWaterToCell(cell, x, y);
                        else Frame::DrawImageToBitmap(background, emptyImg, x, y);
                        break;
                    }

                    case 6: { // world border 
                        return 0; // cannot be modified, exit the function
                    }

                    default: 
                        // if the cell should be water by default, cover the cell with the water image
                        // otherwise, cover it with the empty grass image
                        if (grid[cell.x][cell.y]&WATER) Frame::DrawWaterToCell(cell, x, y);
                        else Frame::DrawImageToBitmap(background, emptyImg, x, y);
                        break;
                }

                // preserve bits 16 (indestructible), and 13, 17-24 (water), 
                // all others to 0
                // bit 14: on if water (bit 13), off otherwise
                grid[cell.x][cell.y] &= 0xFF9000;
                if (grid[cell.x][cell.y]&WATER) grid[cell.x][cell.y] |= 0x2000;
                else grid[cell.x][cell.y] &= ~0x2000;
                break;
            }

            default: 
                // if the cell should be water by default, cover the cell with the water image
                // otherwise, cover it with the empty grass image
                if (grid[cell.x][cell.y]&WATER) Frame::DrawWaterToCell(cell, x, y);
                else Frame::DrawImageToBitmap(background, emptyImg, x, y);
                break;
        }

        // update the background brush, so that the change is observable
        if (updateBkgBrush) {
            Gdiplus::TextureBrush *newBrush = new Gdiplus::TextureBrush(background);

            // upon successful creation, update bkgBrush
            if (newBrush->GetLastStatus() == Gdiplus::Ok)
            {
                // clean up the old brush
                delete bkgBrush;

                // update bkgBrush to the new brush
                bkgBrush = newBrush;
                // set the wrap mode to only draw the background once
                bkgBrush->SetWrapMode(Gdiplus::WrapModeClamp);
            }
        }

        // 0 for successful placement
        return ext; 
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
                    // make the player pick up the object, if possible
                    setHeldObject(obj);

                    // exit the function, the entity was clicked
                    return;
                }
            }

        } else { 
            // if an item IS held, throw it :)
            throwHeldObject();
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

        if (heldObject == nullptr) 
        {
            // check to see if the player clicked on ANY entities
            for (int i = 0; i < gameObjects.size(); i++)
            {
                Object::GameObject *obj = gameObjects[i];
                // nothing happens when clicking on non items
                if (obj->type < Object::Log_Item) continue;

                Gdiplus::Rect objRect(obj->pos.x, obj->pos.y, obj->size.x, obj->size.y);
                if (isInRegion(mousePosREAL, objRect)) { // right clicked an item stack
                    // attemp to craft the item stack
                    Object::GameObject *crafted = Object::craftItem(obj);
                    // if successful, return
                    if (crafted != nullptr) return;
                }
            }
        }
        // find the cell the player selected
        Math::Point2 cell = Object::findCell(mousePosREAL);

        // place the held building into the cell
        int status = PlaceObjectInCell(cell, buildingType);

        // upon successful placement, decrease the held item's hp
        if (status == 0 && heldObject != nullptr) heldObject->hp--;
    }


    // what happens when the player presses down a key
    void keyPressFunc(WPARAM wParam, HWND hwnd)
    {
        // enable bits
        switch (wParam)
        {
            case 0x57: // w
                inputKeys |= 8; break;
            case 0x41: // a
                inputKeys |= 4; break;
            case 0x53: // s
                inputKeys |= 2; break;
            case 0x44: // d
                inputKeys |= 1; break;
            case VK_SHIFT:
                inputKeys |= 16; break;
            case VK_ESCAPE:
                gameIsPaused = !gameIsPaused;
                break;
            case VK_F11:
                // commented out for now bc bad, will work on this later :3
                // ToggleFullscreen(hwnd, viewRect.Width, viewRect.Height);
                break;
            case VK_F9:
                debuggingTools ^= 1; break;
            case VK_F8:
                debuggingTools ^= 2; break;
            case VK_F7:
                player->hasCollision = !player->hasCollision;
                break;
            case VK_F6:
                debuggingTools ^= 4; break;
        }
    }

    // what happens when the player releases a key
    void keyReleaseFunc(WPARAM wParam)
    {
        // disable bits
        switch (wParam)
        {
            case 0x57: // w
                inputKeys &= ~8; break;
            case 0x41: // a
                inputKeys &= ~4; break;
            case 0x53: // s
                inputKeys &= ~2; break;
            case 0x44: // d
                inputKeys &= ~1; break;
            case VK_SHIFT:
                inputKeys &= ~16; break;
        }
    }





    // sets the selected object to be held by the player
    void setHeldObject(Object::GameObject *obj)
    {
        // update the building type the player tries to place
        switch (obj->type)
        {
            case Object::Log_Item:
                // player is building with logs
                buildingType = LOG;
                break;

            case Object::Pine_Cone_Item:
                // player is planting tree saplings
                buildingType = SAPLING;
                break;

            case Object::Plank_Item:
                // player cannot build anything haha nerd
                buildingType = EMPTY;
                break;

            case Object::Bridge_Item:
                buildingType = BRIDGE;
                break;

            default: return; // not an item, cannot be held
        }

        // reset any existing velocity/acceleration
        obj->velocity = obj->acceleration = Math::Zero2;
        // reset the timer
        obj->timer = 0.0f;

        // update the behaviour functions, so that the stack will be held by the player
        obj->positionFunc = Func::heldItemPositionFunc;

        // update the global held object
        heldObject = obj;

        // make the player the object's owner
        obj->owner = player;
        player->ownedObjects.push_back(obj);
    }

    // throw the object the player is currently holding
    void throwHeldObject()
    {
        // release the held object
        Object::GameObject *obj = heldObject;
        heldObject = nullptr;
        // player can no longer build anything, set the building type back to EMPTY
        buildingType = EMPTY;


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
    }
}