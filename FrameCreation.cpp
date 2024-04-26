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

        // for (int x = 0; x < grid.size(); x++) {
        //     for (int y = 0; y < grid[0].size(); y++) {
        //         DrawCell(Math::Point2(x, y), graphics);
        //     }
        // }

        for (int i = 0; i < gameObjects.size(); i++) {
            gameObjects[i]->draw(graphics);
        }

        Inventory::drawHotbar(graphics);

        if (gameIsPaused) {
            Gdiplus::SolidBrush pauseBrush(Gdiplus::Color(180,0,0,0));
            Gdiplus::Rect rect(0,0,wndWidth,wndHeight);
            graphics.FillRectangle(&pauseBrush, rect);
        }

        // frame rate
        int fps = 1.0f/deltaTime;
        if (fps<0||fps>500) fps = prevFPS; // dTime = 0
        else prevFPS = fps;
        std::wstring fpsText = std::to_wstring(fps)+L" FPS";
        placeText(10, 10, fpsText, Gdiplus::Color(255,255,255), 12, graphics);
    }

    void DrawBackgroundSection(Gdiplus::Graphics& graphics)
    {
        Gdiplus::SolidBrush blackBrush(Gdiplus::Color(0,0,0));
        int srcx = 0, srcy = 0, bkgx = 0, bkgy = 0;
        
        switch (bkgState&3) { // first two bits
            case 0: // centre
                srcx = player->pos.x-wndWidth/2;
                break;
            case 1: // left
                if (bkgWidth<wndWidth) {
                    graphics.FillRectangle(&blackBrush, 0, 0, 100, 50);
                    bkgx = (wndWidth-bkgWidth)/2;
                }
                break;
            case 2: // right
                srcx = bkgWidth-wndWidth;
                break;
        }

        switch (bkgState&12) { // bits 3 and 4
            case 0: // centre
                srcy = player->pos.y-wndHeight/2;
                break;
            case 4: // top
                if (bkgHeight<wndHeight) {
                    graphics.FillRectangle(&blackBrush, 0, 0, 100, 50);
                    bkgy = (wndHeight-bkgHeight)/2;
                }
                break;
            case 8: // bottom
                srcy = bkgHeight-wndHeight;
                break;
        }
        graphics.DrawImage(background, bkgx, bkgy, srcx, srcy, 
        wndWidth, wndHeight, Gdiplus::UnitPixel);
    }

    void PlaceObjectInCell(Math::Point2 cell, int objType) {
        if (cell.x<0||cell.x>=grid.size()||cell.y<0||cell.y>=grid[0].size()) return;

        // update background image for drawing
        Gdiplus::Image* img;
        int x = cell.x*sideLen, y = cell.y*sideLen;
        switch (objType)
        {
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

            case EMPTY: {
                // remove building in the cell
                // if it IS indestructible or not occupied, return
                if (grid[cell.x][cell.y]&0x8000||!grid[cell.x][cell.y]&0x4000) return;
                
                switch (grid[cell.x][cell.y]&255)
                {
                    case 0: // empty cell, do nothing
                        break;
                    case 1: // log
                        hotbarButtons[0]->count++; break;
                    case 2: // bridge
                        hotbarButtons[1]->count++; break;
                }

                if (grid[cell.x][cell.y]&WATER) img = waterImg;
                else img = emptyImg;
                DrawImageToBitmap(background, img, x, y);
                // preserve bits 16 (indestructible), and 13, 17-24 (water), 
                // all others to 0
                // bit 14: on if water (bit 13), off otherwise
                grid[cell.x][cell.y] &= 0xFF9000;
                if (grid[cell.x][cell.y]&WATER) grid[cell.x][cell.y] |= 0x2000;
                else grid[cell.x][cell.y] &= ~0x2000;
                return;
            }
            default: 
                if (grid[cell.x][cell.y]&WATER) img = waterImg;
                else img = emptyImg;
                break;
        }
        DrawImageToBitmap(background, img, x, y);
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

    void DrawWholeGrid()
    {
        int nx = grid.size(), ny = grid[0].size();
        for (int x = 0; x < nx; x++) {
            for (int y = 0; y < ny; y++) {
                Math::Point2 cell(x, y);
                int type = grid[cell.x][cell.y];
                PlaceObjectInCell(cell, type);
            }
        }
    }
}
