#include "PlayerInput.hpp"

using namespace Globals;

namespace Input
{
    // place an object into a cell in the global grid
    void PlaceObjectInCell(Math::Point2 cell, int objType, bool updateBkgBrush) 
    {
        // cell is out of bounds and doesn't exist
        if (cell.x<0||cell.x>=grid.size()||cell.y<0||cell.y>=grid[0].size()) return;

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
                // cell is occupied or out of logs
                if (grid[cell.x][cell.y]&0x4000||!hotbarButtons[0]->count) return;

                img = logImg; // image drawn will be a log
                grid[cell.x][cell.y] |= LOG;
                hotbarButtons[0]->count--;
                break;


            case BRIDGE:
                // cell is occupied or out of bridges
                if (grid[cell.x][cell.y]&0x4000||!hotbarButtons[1]->count) return;
                img = bridgeImg;
                grid[cell.x][cell.y] |= BRIDGE;
                grid[cell.x][cell.y] &= ~0x2000; // turn off barrier bit
                hotbarButtons[1]->count--;
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
                if (grid[cell.x][cell.y]&0x8000||!grid[cell.x][cell.y]&0x4000) return;
                // this number can reenable specified bits, like in case 3
                int num = 0;
                
                // first byte represents info about the cell, this is the object being removed
                switch (grid[cell.x][cell.y]&255)
                {
                    case 1: // log
                        // get wood back
                        hotbarButtons[0]->count++; break;

                    case 2: // bridge
                        // get bridge back
                        hotbarButtons[1]->count++; break;

                    case 3: { // tree
                        hotbarButtons[0]->count += 5; // get wood

                        // remove tree image from the overlay layer
                        Frame::RemoveTreeFromOverlay(cell);

                        // spawn a falling tree object in its place
                        Object::Instantiate(Object::Falling_Tree, 
                            Math::Vector2((cell.x-1)*sideLen, (cell.y-9)*sideLen),
                            Math::Point2(10*sideLen, 10*sideLen), 0.0f, 1);

                        // place a stump where the tree once stood
                        PlaceObjectInCell(cell, STUMP);

                        // everything else handled in the STUMP case. don't break, just return
                        return;
                    }

                    case 4: // stump
                        hotbarButtons[0]->count++; // get wood 
                        break;

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
                // if the cell should be water by default, cover the cell with the water image
                // otherwise, cover it with the empty grass image
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
    }

    // for convenience
    void PlaceObjectInCell(Math::Point2 cell, int objType) {
        PlaceObjectInCell(cell, objType, true);
    }



    // determines if the point, p, is within the rect region
    bool isInRegion(Math::Vector2 p, Gdiplus::RectF rect)
    {
        // right boundary and bottom boundary
        float right = rect.X+rect.Width, bottom = rect.Y+rect.Height;

        // if within the X bound of region AND within Y bound of region
        return p.x>=rect.X && p.x<=right && p.y>=rect.Y && p.y<=bottom;
    }
}