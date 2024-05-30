#include "UserInput.hpp"

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


        // set the water bit if applicable
        grid[cell.x][cell.y] |= objType&WATER;


        switch (objType & 255) // just the first byte, containing cell ID
        {
            case 3: // tree

                // empty grass tile drawn where the base is
                Frame::DrawImageToBitmap(mapBackground, emptyImg, x, y);
                
                // add a tree image to the overlay layer
                Frame::AddTreeToOverlay(cell, updateBkgBrush);

                // set the cell space to be a tree
                // use = (not |=) because previous data SHOULD be overwritten; if the tree grew
                // from a sapling, the cell should no longer contain any data about the sapling
                
                grid[cell.x][cell.y] = TREE;
                break;
                

            case 1: // log 

                // draw the background of the cell first
                if (grid[cell.x][cell.y]&WATER) Frame::DrawWaterToCell(cell, x, y);
                else Frame::DrawImageToBitmap(mapBackground, emptyImg, x, y);

                // draw a log into the cell
                Frame::DrawImageToBitmap(mapBackground, logImg, x, y);

                // removes a tree if you try to replace it
                if ((grid[cell.x][cell.y]&255)==3) Frame::RemoveTreeFromOverlay(cell);
                // if there was something other than a log before, make sure to remove it,
                // preserving the water bit
                grid[cell.x][cell.y] &= LOG|WATER;
                
                // update the grid value to represent a log
                grid[cell.x][cell.y] |= LOG;
                break;


            case 2: // bridge

                // draw the background of the cell first
                if (grid[cell.x][cell.y]&WATER) Frame::DrawWaterToCell(cell, x, y);
                else Frame::DrawImageToBitmap(mapBackground, emptyImg, x, y);

                // draw a bridge to the cell
                Frame::DrawImageToBitmap(mapBackground, bridgeImg, x, y);

                // removes a tree if you try to replace it
                if ((grid[cell.x][cell.y]&255)==3) Frame::RemoveTreeFromOverlay(cell);
                // if there was something other than a bridge before, make sure to remove it,
                // preserving the water bit
                grid[cell.x][cell.y] &= BRIDGE|WATER;

                // update the grid value to represent a bridge
                grid[cell.x][cell.y] |= BRIDGE;
                grid[cell.x][cell.y] &= ~BARRIER; // turn off barrier bit
                break;


            case 4: // stump

                // draw the empty tile first, in case of loading a map with a stump, but not a tree
                Frame::DrawImageToBitmap(mapBackground, emptyImg, x, y);

                // draw a stump to the cell
                Frame::DrawImageToBitmap(mapBackground, stumpImg, x, y);

                // removes a tree if you try to replace it
                if ((grid[cell.x][cell.y]&255)==3) Frame::RemoveTreeFromOverlay(cell);
                // only occurs when a tree gets removed, thus the cell could only have been TREE prior.
                // so, there is no issue just settig the cell to STUMP, no need for |=
                grid[cell.x][cell.y] = STUMP;
                break;

            case 5: { // sapling

                // draw the empty tile first
                Frame::DrawImageToBitmap(mapBackground, emptyImg, x, y);

                // draw a sapling to the cell
                Frame::DrawImageToBitmap(mapBackground, saplingImg, x, y);

                // removes a tree if you try to replace it
                if ((grid[cell.x][cell.y]&255)==3) Frame::RemoveTreeFromOverlay(cell);
                // if there was something other than a sapling before, make sure to remove it
                grid[cell.x][cell.y] &= SAPLING;

                // update the grid to represent a sapling
                grid[cell.x][cell.y] |= SAPLING;
                break;
            }

            case EMPTY: 
            {   
                // first byte represents info about the cell, this is the object being removed
                switch (grid[cell.x][cell.y]&255)
                {
                    case 1: { // log

                        // if the cell should be water by default, cover the cell with the water image
                        // otherwise, cover it with the empty grass image
                        if (grid[cell.x][cell.y]&WATER) Frame::DrawWaterToCell(cell, x, y);
                        else Frame::DrawImageToBitmap(mapBackground, emptyImg, x, y);

                        break;
                    }

                    case 2: // bridge
                        // if the cell should be water by default, cover the cell with the water image
                        // otherwise, cover it with the empty grass image
                        if (grid[cell.x][cell.y]&WATER) Frame::DrawWaterToCell(cell, x, y);
                        else Frame::DrawImageToBitmap(mapBackground, emptyImg, x, y);
                        break;

                    case 3: { // tree

                        // remove tree image from the overlay layer
                        Frame::RemoveTreeFromOverlay(cell);

                        // if the cell should be water by default, cover the cell with the water image
                        // otherwise, cover it with the empty grass image
                        if (grid[cell.x][cell.y]&WATER) Frame::DrawWaterToCell(cell, x, y);
                        else Frame::DrawImageToBitmap(mapBackground, emptyImg, x, y);
                    }

                    case 4: { // stump

                        // if the cell should be water by default, cover the cell with the water image
                        // otherwise, cover it with the empty grass image
                        if (grid[cell.x][cell.y]&WATER) Frame::DrawWaterToCell(cell, x, y);
                        else Frame::DrawImageToBitmap(mapBackground, emptyImg, x, y);
                        break;
                    }

                    case 5: { // sapling

                        // if the cell should be water by default, cover the cell with the water image
                        // otherwise, cover it with the empty grass image
                        if (grid[cell.x][cell.y]&WATER) Frame::DrawWaterToCell(cell, x, y);
                        else Frame::DrawImageToBitmap(mapBackground, emptyImg, x, y);
                        break;
                    }

                    default: 

                        if (objType == EMPTY) grid[cell.x][cell.y] = EMPTY;
                        // if the cell should be water by default, cover the cell with the water image
                        // otherwise, cover it with the empty grass image
                        if (grid[cell.x][cell.y]&WATER) {
                            Frame::DrawWaterToCell(cell, x, y);
                        }
                        else Frame::DrawImageToBitmap(mapBackground, emptyImg, x, y);
                        
                        // redraw any adjacent water cells
                        Frame::DrawWaterToCell(Math::Point2(cell.x,cell.y-1),x,y-sideLen);
                        Frame::DrawWaterToCell(Math::Point2(cell.x,cell.y+1),x,y+sideLen);
                        Frame::DrawWaterToCell(Math::Point2(cell.x-1,cell.y),x-sideLen,y);
                        Frame::DrawWaterToCell(Math::Point2(cell.x+1,cell.y),x+sideLen,y);
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
                else Frame::DrawImageToBitmap(mapBackground, emptyImg, x, y);
                break;
        }

        // update the background brush, so that the change is observable
        if (updateBkgBrush) {
            Gdiplus::TextureBrush *newBrush = new Gdiplus::TextureBrush(mapBackground);

            // upon successful creation, update bkgBrush
            if (newBrush->GetLastStatus() == Gdiplus::Ok)
            {
                // clean up the old brush
                delete mapBkgBrush;

                // update bkgBrush to the new brush
                mapBkgBrush = newBrush;
                // set the wrap mode to only draw the background once
                mapBkgBrush->SetWrapMode(Gdiplus::WrapModeClamp);

                // set the brush translation so it remains in place
                mapBkgBrush->TranslateTransform(mapRect.X, mapRect.Y);
            } else delete newBrush;
        }
        // 0 for successful placement
        return ext; 
    }




    // what happens when the user presses down a key
    void keyPressFunc(WPARAM wParam, HWND hwnd)
    {

    }
    // what happens when the user releases a key
    void keyReleaseFunc(WPARAM wParam)
    {

    }



    // function that gets called when the user left clicks
    void leftClickFunc(int x, int y)
    {
        // update global mouse position
        mPosX = x; mPosY = y;

        // attempt to press buttons
        int n = buttons.size();
        for (int i = 0; i < n; i++) {
            Buttons::Button *b = buttons[i];
            // if the button is pressed call its left click function
            if (b->pressed(x, y)) b->leftClickFunc(b); 
        }
    }
    
    // function that gets called when the user right clicks
    void rightClickFunc(int x, int y)
    {
        // update global mouse position
        mPosX = x; mPosY = y;

        // store the object type the player had held
        int currType = placementType;
        placementType = EMPTY; // place an empty cell

        // attempt to press buttons
        int n = buttons.size();
        for (int i = 0; i < n; i++) {
            Buttons::Button *b = buttons[i];
            // if the button is pressed call its right click function
            if (b->pressed(x, y)) b->rightClickFunc(b); 
        }

        // reset the placement type to what it was before
        placementType = currType;
    }

    // determines if the point, p, is within the rect region
    bool isInRegion(Math::Vector2 p, Gdiplus::Rect rect)
    {
        // right boundary and bottom boundary
        float right = rect.X+rect.Width, bottom = rect.Y+rect.Height;

        // if within the X bound of region AND within Y bound of region
        return p.x>=rect.X && p.x<=right && p.y>=rect.Y && p.y<=bottom;
    }



    // button functions

    // places the buiding type you have held into the map
    void placeCellOnMapButtonFunc(Buttons::Button *b)
    {
        if (b->isActive) {
            // position of the mouse on to the actual map image
            int x = mPosX - mapRect.X, y = mPosY - mapRect.Y;

            x /= sideLen; y /= sideLen; // coordinates as cell indices

            // cell does not exist, return to avoid errors
            if (x<0 || x>=gridSizeX || y<0 || y>=gridSizeY) return;

            Math::Point2 cell(x, y);
            PlaceObjectInCell(cell, placementType, true);
        }
    }

    // creates an entity on the map where the player clicks
    void placeEntityOnMapButtonFunc(Buttons::Button *b)
    {
        if (b->isActive) {
            // move the player when trying to create a player when one already exists
            if (placementType == Storage::Player && player != nullptr) {
                player->pos = Math::Vector2(mPosX, mPosY);
                player->centrePos = player->pos + Math::Vector2(player->size.x/2, player->size.y/2);
                return;
            }

            // create an entity based on the selected type
            Storage::Entity *e = new Storage::Entity(
                Math::Vector2(mPosX, mPosY), Math::Zero2, (Storage::EntityType)placementType,
                selectedHP, entities.size());

            // make sure the entirety of the entity's hitbox is within the map region
            if (e->pos.x<mapRect.X || e->pos.x>mapRect.X+mapRect.Width-e->size.x ||
                e->pos.y<mapRect.Y || e->pos.y>mapRect.Y+mapRect.Height-e->size.y) {
                    // entity is off the map, destroy it
                    e->deallocateResources();
                }
            else {
                // placement is valid, add the entity to the entities vector
                entities.push_back(e);
                // update the player reference, if necesary
                if (placementType == Storage::Player) player = e;
            }
        }
    }

    // removes any entity that you click on
    void removeEntityFromMapButtonFunc(Buttons::Button *b)
    {
        if (b->isActive) {
            // get mouse position as a vector2
            Math::Vector2 mousePos(mPosX, mPosY);

            // check all the entities to see if the user clicked one
            bool flag = true; // flag set to false once an entity has been found 

            for (int i = 0; i < entities.size(); i++) {
                Storage::Entity *e = entities[i];
                Gdiplus::Rect entityRect(e->pos.x, e->pos.y, e->size.x, e->size.y);
                if (flag) {
                    // entity was clicked
                    if (isInRegion(mousePos, entityRect)) {
                        // when removing the player, update the global reference accoringly
                        if (e == player) player = nullptr;
                        // deallocate the object's resources and erase it from the vector
                        e->deallocateResources();
                        entities.erase(entities.begin() + i--);
                        // set the flag to false so no other entities are deleted
                        flag = false;
                    }
                } else {
                    // decrement the index of all entities following the deleted one
                    e->idx--;
                }
            }
        }
    }


    // button functions for setting the building type
    void makeBuildingTypeLog(Buttons::Button *b) {
        if (b->isActive) {
            // update the map button's building functions
            mapButton->leftClickFunc = mapButton->rightClickFunc = placeCellOnMapButtonFunc;
            placementType = LOG;
        }
    }
    void makeBuildingTypeBridge(Buttons::Button *b) {
        if (b->isActive) {
            // update the map button's building functions
            mapButton->leftClickFunc = mapButton->rightClickFunc = placeCellOnMapButtonFunc;
            placementType = BRIDGE;
        }
    }
    void makeBuildingTypeWater(Buttons::Button *b) {
        if (b->isActive) {
            // update the map button's building functions
            mapButton->leftClickFunc = mapButton->rightClickFunc = placeCellOnMapButtonFunc;
            placementType = WATER|BARRIER; // for now, water will also act as a barrier
        }
    }
    void makeBuildingTypeSapling(Buttons::Button *b) {
        if (b->isActive) {
            // update the map button's building functions
            mapButton->leftClickFunc = mapButton->rightClickFunc = placeCellOnMapButtonFunc;
            placementType = SAPLING;
        }
    }
    void makeBuildingTypeTree(Buttons::Button *b) {
        if (b->isActive) {
            // update the map button's building functions
            mapButton->leftClickFunc = mapButton->rightClickFunc = placeCellOnMapButtonFunc;
            placementType = TREE;
        }
    }
    void makeBuildingTypeStump(Buttons::Button *b) {
        if (b->isActive) {
            // update the map button's building functions
            mapButton->leftClickFunc = mapButton->rightClickFunc = placeCellOnMapButtonFunc;
            placementType = STUMP;
        }
    }

    // button functions for placing entity types
    void makeBuildingTypePlayer(Buttons::Button *b) {
        if (b->isActive) {
            // update the map button's building functions
            mapButton->leftClickFunc = placeEntityOnMapButtonFunc;
            mapButton->rightClickFunc = removeEntityFromMapButtonFunc;
            placementType = (int)Storage::Player;
        }
    }
    void makeBuildingTypeWolf(Buttons::Button *b) {
        if (b->isActive) {
            // update the map button's building functions
            mapButton->leftClickFunc = placeEntityOnMapButtonFunc;
            mapButton->rightClickFunc = removeEntityFromMapButtonFunc;
            placementType = (int)Storage::Wolf;
        }
    }
    void makeBuildingTypeLog_Item(Buttons::Button *b) {
        if (b->isActive) {
            // update the map button's building functions
            mapButton->leftClickFunc = placeEntityOnMapButtonFunc;
            mapButton->rightClickFunc = removeEntityFromMapButtonFunc;
            placementType = (int)Storage::Log_Item;
        }
    }
    void makeBuildingTypePine_Cone_Item(Buttons::Button *b) {
        if (b->isActive) {
            // update the map button's building functions
            mapButton->leftClickFunc = placeEntityOnMapButtonFunc;
            mapButton->rightClickFunc = removeEntityFromMapButtonFunc;
            placementType = (int)Storage::Pine_Cone_Item;
        }
    }


    // saving the level
    void saveLevelToFileButtonFunc(Buttons::Button *b)
    {
        if (b->isActive) {
            Storage::saveLevelToFile(levelFileName);
        }
    }
}