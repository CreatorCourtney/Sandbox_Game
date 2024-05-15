#include "FrameCreation.hpp"

using namespace Globals;

namespace Frame 
{
    Gdiplus::GdiplusStartupInput gdiplusStartupInput;
    ULONG_PTR gdiplusToken;

    void InitialiseFrameCreation()
    {
        // initialise Gdiplus
        Gdiplus::GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

        Globals::LoadImages();
    }

    void ShutDownFrameCreation()
    {
        // shut down gdiplus
        Gdiplus::GdiplusShutdown(gdiplusToken);
    }

    void CreateBufferFrame(HWND hwnd, HDC hdc)
    {
        Gdiplus::Graphics graphics(hOffscreenDC);

        DrawBackgroundSection(graphics);

        for (int i = 0; i < gameObjects.size(); i++) {
            gameObjects[i]->draw(graphics);
        }
        
        graphics.FillRectangle(overlayBrush, 0, 0, wndWidth, wndHeight);

        Inventory::drawHotbar(graphics);

        if (gameIsPaused) {
            Gdiplus::SolidBrush pauseBrush(Gdiplus::Color(180,0,0,0));
            Gdiplus::Rect rect(0,0,wndWidth,wndHeight);
            graphics.FillRectangle(&pauseBrush, rect);
        }

        // developer debug tools
        int fps = 1.0f/deltaTime;
        if (fps<0) fps = prevFPS; // dTime = 0
        else prevFPS = fps;
        std::wstring fpsText = std::to_wstring(fps)+L" FPS";
        placeText(10, 10, fpsText, white, 10, graphics);
        placeText(10, 25, L"press F9 to toggle debug menu", white, 10, graphics);
        if (debuggingTools&1) ShowDebugInfo(graphics);
    }

    void DrawBackgroundSection(Gdiplus::Graphics& graphics)
    {
        Gdiplus::SolidBrush blackBrush(Gdiplus::Color(0,0,0));
        int x = 0, y = 0;
        
        switch (bkgState) {
            case 0: // full centre
                x = wndWidth/2-player->pos.x;
                y = wndHeight/2-player->pos.y;
                break;
            case 1: // centre y, left x
                if (bkgWidth<wndWidth) {
                    graphics.FillRectangle(&blackBrush, 0, 0, 500, 500);
                    x = (wndWidth-bkgWidth)/2;
                }
                y = wndHeight/2-player->pos.y;
                break;
            case 2: // centre y, right x
                x = wndWidth-bkgWidth;
                y = wndHeight/2-player->pos.y;
                break;
            case 4: // top y, centre x
                x = wndWidth/2-player->pos.x;
                if (bkgHeight<wndHeight) {
                    graphics.FillRectangle(&blackBrush, 0, 0, 500, 500);
                    y = (wndHeight-bkgHeight)/2;
                }
                break;
            case 5: // top y, left x
                if (bkgWidth<wndWidth) {
                    graphics.FillRectangle(&blackBrush, 0, 0, 500, 500);
                    x = (wndWidth-bkgWidth)/2;
                } if (bkgHeight<wndHeight) {
                    graphics.FillRectangle(&blackBrush, 0, 0, 500, 500);
                    y = (wndHeight-bkgHeight)/2;
                }
                break;
            case 6: // top y, right x
                x = wndWidth-bkgWidth;
                if (bkgHeight<wndHeight) {
                    graphics.FillRectangle(&blackBrush, 0, 0, 500, 500);
                    y = (wndHeight-bkgHeight)/2;
                }
                break;
            case 8: // bottom y, centre x
                x = wndWidth/2-player->pos.x;
                y = wndHeight-bkgHeight;
                break;
            case 9: // bottom y, left x
                if (bkgWidth<wndWidth) {
                    graphics.FillRectangle(&blackBrush, 0, 0, 500, 500);
                    x = (wndWidth-bkgWidth)/2;
                }
                y = wndHeight-bkgHeight;
                break;
            case 10: // bottom y, right x
                x = wndWidth-bkgWidth;
                y = wndHeight-bkgHeight;
                break;
        }
        // set the clipping region of the brush to the portion of the backgorund image
        Gdiplus::Matrix matrix(1.0f,0.0f,0.0f,1.0f,x,y);
        // bkgBrush->ResetTransform();
        bkgBrush->SetTransform(&matrix);
        overlayBrush->SetTransform(&matrix);

        graphics.FillRectangle(bkgBrush, 0, 0, wndWidth, wndHeight);
    }

    // for convenience
    void PlaceObjectInCell(Math::Point2 cell, int objType) {
        PlaceObjectInCell(cell, objType, true);
    }

    void PlaceObjectInCell(Math::Point2 cell, int objType, bool updateBkgBrush) {
        if (cell.x<0||cell.x>=grid.size()||cell.y<0||cell.y>=grid[0].size()) return;

        // update background image for drawing
        Gdiplus::Image* img = nullptr;
        int x = cell.x*sideLen, y = cell.y*sideLen;

        switch (objType)
        {
            case TREE:
                // empty grass tile drawn where the base is
                img = emptyImg;
                
                AddTreeToOverlay(cell, updateBkgBrush);

                grid[cell.x][cell.y] |= TREE;
                break;
                
            case LOG: 
                // cell is occupied or out of logs
                if (grid[cell.x][cell.y]&0x4000||!hotbarButtons[0]->count) return;
                img = logImg;
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
                DrawImageToBitmap(background, emptyImg, x, y);

                // only occurs when a tree gets removed, thus the cell could only have been TREE prior.
                // so, there is no issue just settig the cell to STUMP, no need for |=
                img = stumpImg;
                grid[cell.x][cell.y] = STUMP;
                break;

            case EMPTY: {
                // remove building in the cell
                // if it IS indestructible or not occupied, return
                if (grid[cell.x][cell.y]&0x8000||!grid[cell.x][cell.y]&0x4000) return;
                // this number can reenable specified bits, like in case 3
                int num = 0;
                
                switch (grid[cell.x][cell.y]&255)
                {
                    case 1: // log
                        hotbarButtons[0]->count++; break;

                    case 2: // bridge
                        hotbarButtons[1]->count++; break;

                    case 3: { // tree
                        hotbarButtons[0]->count += 5; // get wood
                        // remove tree
                        RemoveTreeFromOverlay(cell);

                        // spawn a falling tree object in its place
                        Object::Instantiate(Object::Falling_Tree, 
                            Math::Vector2((cell.x-1)*sideLen, (cell.y-9)*sideLen),
                            Math::Point2(10*sideLen, 10*sideLen), 0.0f, 1.0f);

                        // place a stump where the tree once stood
                        PlaceObjectInCell(cell, STUMP);

                        // everything else handled in the STUMP case. don't break, just return
                        return;
                    }

                    case 4: // stump
                        hotbarButtons[0]->count++; // get wood break;

                    default: break;
                }

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
                if (grid[cell.x][cell.y]&WATER) img = waterImg;
                else img = emptyImg;
                break;
        }

        // draw the object placed onto the image bitmap
        if (img != nullptr) DrawImageToBitmap(background, img, x, y);
        if (updateBkgBrush) {
            bkgBrush = new Gdiplus::TextureBrush(background);
            bkgBrush->SetWrapMode(Gdiplus::WrapModeClamp);
        }
        // need to preserve water bits... (9-13)
    }

    void DrawCell(Math::Point2 cell, Gdiplus::Graphics& graphics) {
        Gdiplus::Image * img;
        Math::Point2 wndPos = 
            Object::getScreenPosition(Math::Vector2(cell.x*sideLen, cell.y*sideLen));
        if (wndPos.x>wndWidth||wndPos.x<-sideLen||
            wndPos.y>wndHeight||wndPos.y<-sideLen) return; // offscreen, dont render
        
        if (grid[cell.x][cell.y]&WATER) img = waterImg;
        else img = emptyImg;
        graphics.DrawImage(img, (Gdiplus::REAL)wndPos.x, (Gdiplus::REAL)wndPos.y, sideLen, sideLen);

        switch (grid[cell.x][cell.y])
        {
            case LOG: img = logImg; break;
            default: return; // empty cell, dont draw
        }
            
        graphics.DrawImage(img, (Gdiplus::REAL)wndPos.x, (Gdiplus::REAL)wndPos.y, sideLen, sideLen);
    }

    void placeText(int x, int y, std::wstring text, 
    Gdiplus::Color colour, int size, Gdiplus::Graphics& graphics) {
        Gdiplus::Font font(L"Arial", size);
        Gdiplus::SolidBrush brush(colour);
        graphics.DrawString(text.c_str(), -1, &font, Gdiplus::PointF(x,y), &brush);
    }

    void DrawImageToBitmap(Gdiplus::Bitmap* bitmap, Gdiplus::Image* img, int x, int y)
    {
        Gdiplus::Graphics graphics(bitmap);
        graphics.DrawImage(img, (Gdiplus::REAL)x, (Gdiplus::REAL)y, sideLen, sideLen);
    }

    void DrawImageToBitmap(Gdiplus::Bitmap* bitmap, Gdiplus::Image* img, int x, int y, int srcX, int srcY, int srcWidth, int srcHeight) {
        Gdiplus::Graphics graphics(bitmap);
        graphics.DrawImage(img, x, y, srcX, srcY, srcWidth, srcHeight, Gdiplus::UnitPixel);
    }

    void DrawWholeGrid()
    {
        int nx = grid.size(), ny = grid[0].size();
        for (int x = 0; x < nx; x++) {
            for (int y = 0; y < ny; y++) {
                Math::Point2 cell(x, y);
                int type = grid[cell.x][cell.y];
                PlaceObjectInCell(cell, type, false);
            }
        }
        bkgBrush = new Gdiplus::TextureBrush(background);
        bkgBrush->SetWrapMode(Gdiplus::WrapModeClamp);
        overlayBrush = new Gdiplus::TextureBrush(overlay);
        overlayBrush->SetWrapMode(Gdiplus::WrapModeClamp);
    }

    void AddTreeToOverlay(Math::Point2 cell, bool updateOverlayBrush) {
        cell.x -= treeWidth/2; cell.y -= treeHeight-1;
        int x = cell.x*sideLen, y = cell.y*sideLen;

        Gdiplus::Graphics graphics(overlay);
        graphics.DrawImage(treeImg, (Gdiplus::REAL)x, (Gdiplus::REAL)y, 
            sideLen*treeWidth, sideLen*treeHeight);

        if (updateOverlayBrush) {
            overlayBrush = new Gdiplus::TextureBrush(overlay);
            overlayBrush->SetWrapMode(Gdiplus::WrapModeClamp);
        }
        numObjectsinOverlay++;
    }

    // for convenience
    void AddTreeToOverylay(Math::Point2 cell) {
        AddTreeToOverlay(cell, true);
    }

    /*  this funtion is currently O(n*m), where
        n = number of trees and m = size of each tree. with the current tree size, 
        25 trees seems to be a reasonable limit for how many trees should be in the level
    */
    void RemoveTreeFromOverlay(Math::Point2 cell) {
        // remove tree cell type from grid
        grid[cell.x][cell.y] &= 0xFF9000;
        // delete the entire region the tree covers
        // origin of the tree
        cell.x -= treeWidth/2; cell.y -= treeHeight-1;
        Gdiplus::Rect rect( cell.x*sideLen, cell.y*sideLen, 
                            treeWidth*sideLen, treeHeight*sideLen);
        
        Gdiplus::Graphics graphics(overlay);
        // clear the existing overlay
        graphics.Clear(Gdiplus::Color::Transparent);
        numObjectsinOverlay = 0;

        // draw remaining trees back onto the overlay
        int nx = grid.size(), ny = grid[0].size();
        for (int x = 0; x < nx; x++) {
            for (int y = 0; y < ny; y++) {
                Math::Point2 cell(x, y);
                if (grid[cell.x][cell.y]==TREE) AddTreeToOverlay(cell, false);
            }
        }

        // update overlay brush
        overlayBrush = new Gdiplus::TextureBrush(overlay);
        overlayBrush->SetWrapMode(Gdiplus::WrapModeClamp);
    }

    void ShowDebugInfo(Gdiplus::Graphics& graphics) {
        int yPos = 40;
        // player position
        std::wstring posTxt = L"position: ("+
            std::to_wstring(player->centrePos.x)+L", "+
            std::to_wstring(player->centrePos.y)+L')';
        placeText(10, yPos, posTxt, white, 10, graphics); yPos += 15;
        // player cell
        posTxt = L"cell: ("+
            std::to_wstring(int(player->centrePos.x/sideLen))+L", "+
            std::to_wstring(int(player->centrePos.y/sideLen))+L')';
        placeText(10, yPos, posTxt, white, 10, graphics); yPos += 15;

        // entity count
        std::wstring entityTxt = L"num entities: "+std::to_wstring(gameObjects.size());
        placeText(10, yPos, entityTxt, white, 10, graphics); yPos += 15;
        // tree count
        entityTxt = L"num objects in overlay: "+std::to_wstring(numObjectsinOverlay);
        placeText(10, yPos, entityTxt, white, 10, graphics); yPos += 15;

        // debug tools
        placeText(10, yPos, L"press F8 to toggle speed boost", 
        (debuggingTools&2)? red : white, 10, graphics); yPos += 15;
        placeText(10, yPos, L"press F7 to toggle noclip", 
        (player->hasCollision)? white : red, 10, graphics); yPos += 15;
        placeText(10, yPos, L"press F6 to toggle hitboxes",
        (debuggingTools&4)? red : white, 10, graphics); yPos += 15;

    }

    void shadeCell(Gdiplus::Graphics& graphics, Math::Point2 cell) {
        Gdiplus::SolidBrush cellBrush(Gdiplus::Color(100, 0, 0, 255));
        Math::Vector2 o(cell.x*sideLen, cell.y*sideLen);
        Math::Point2 p = Object::getScreenPosition(o);
        if (p.x<-sideLen||p.x>wndWidth||p.y<-sideLen||p.y>wndHeight) return;
        graphics.FillRectangle(&cellBrush, p.x, p.y, (INT)sideLen, (INT)sideLen);
    }
}
