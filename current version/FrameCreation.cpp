#include "FrameCreation.hpp"

using namespace Globals;

namespace Frame 
{
    Gdiplus::GdiplusStartupInput gdiplusStartupInput;
    ULONG_PTR gdiplusToken;

    // initialise GDI+ and load all the images
    void InitialiseFrameCreation()
    {
        // initialise Gdiplus
        Gdiplus::GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

        // load images
        Globals::LoadImages();

        // set up some of the global solid brushes
        pauseBrush = new Gdiplus::SolidBrush(Gdiplus::Color(180,0,0,0));
        nightBrush = new Gdiplus::SolidBrush(Gdiplus::Color(180,0,0,0));
    }

    // shut down GDI+
    void ShutDownFrameCreation()
    {
        // shut down gdiplus
        Gdiplus::GdiplusShutdown(gdiplusToken);

        // clean up the global solid brushes
        delete pauseBrush; delete nightBrush;
    }


    // create a buffer frame onto an offscreen DC. once created, this 
    // frame will be placed onto the screen for the player to actually see
    void CreateBufferFrame(HWND hwnd, HDC hdc)
    {

        // cover the screen in black whenever there's a scene switch
        if (sceneSwitch) {
            Gdiplus::SolidBrush *screenCover = new Gdiplus::SolidBrush(Gdiplus::Color(black));
            g_graphics->FillRectangle(screenCover, viewRect);
            delete screenCover; // cleanup
        }

        // background layer: things drawn below the game objects, like the floor, walls, etc
        // the whole background is drawn as one image to improve performance
        DrawBackgroundSection(*g_graphics);

        // game object layer: game objects that move and/or have animation
        // the cannot be drawn into one image since, they usually don't just sit still
        for (int i = 0; i < gameObjects.size(); i++) {
            gameObjects[i]->draw(*g_graphics);
        }
        
        // overlay layer: for static objects that will be drawn ABOVE the game objects
        // this includes things like trees, this layer gives the effect that game objects
        // are moving under it
        g_graphics->FillRectangle(overlayBrush, 0, 0, wndWidth, wndHeight);


        if (g_time < 10.0f) {
            // find the new alpha value based on linear interpolation
            float t = g_time/10.0f;
            int alpha = (isNightTime)? 100.0f * t : 100.0f * (1.0f-t);
            
            // clean up the old night brush, and mae a new one with the alpha value
            delete nightBrush;
            nightBrush = new Gdiplus::SolidBrush(Gdiplus::Color(alpha,0,0,0));
            // cover the screen in darkness, for night time
            g_graphics->FillRectangle(nightBrush, viewRect);
        
        } else if (isNightTime) g_graphics->FillRectangle(nightBrush, viewRect);


        g_graphics->FillRectangle(CRT_brush, viewRect);


        // pause menu
        if (gameIsPaused) g_graphics->FillRectangle(pauseBrush, viewRect);


        // developer debug tools
        // show fps
        int fps = 1.0f/deltaTime;
        if (fps<0) fps = prevFPS; // dTime = 0
        else prevFPS = fps;
        std::wstring fpsText = std::to_wstring(fps)+L" FPS";
        placeText(10, 10, fpsText, white, 10, *g_graphics);

        // other debug tools
        placeText(10, 25, L"press F9 to toggle debug menu", white, 10, *g_graphics);
        if (debuggingTools&1) ShowDebugInfo(*g_graphics);
    }

    // draws a part of the background onto the graphics object. the part drawn determines
    // on the position of the player, since the camera follows the player
    // this function also sets the transform of the overlay image, so the two are synced
    void DrawBackgroundSection(Gdiplus::Graphics& graphics)
    {
        /*
            i won't comment out every switch case because a lot of it is repeated. 
            essentially, 

            - when the camera is in the centre of the map along the x axis,
              the x displacement will be wndWidth - player->pos.x.
            - when the camera is at the left of the map, the x displacement will
              be zero
            - when the camera is at the right of the map, the x displacement will
              be wndWidth - bkgWidth

            the same is true for the y axis, but replace left with top, and right with bottom

            by moving the background, it gives the illusion that the camera is
            following the player 

            the only other thing: when the background image is smaller than the window, fill
            a little region with a black background, so text can be drawn onto it
        */
        
        // black brush, for when the background is smaller than the window
        Gdiplus::SolidBrush *blackBrush = new Gdiplus::SolidBrush(Gdiplus::Color(0,0,0));
        // background image offsets
        int x = 0, y = 0;


        // bkg state determined by player position in GameObject::updateBkg
        switch (bkgState) {
            case 0: // full centre
                x = wndWidth/2-player->pos.x;
                y = wndHeight/2-player->pos.y;
                break;
            case 1: // centre y, left x
                if (bkgWidth<wndWidth) {
                    graphics.FillRectangle(blackBrush, 0, 0, 500, 500);
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
                    graphics.FillRectangle(blackBrush, 0, 0, 500, 500);
                    y = (wndHeight-bkgHeight)/2;
                }
                break;
            case 5: // top y, left x
                if (bkgWidth<wndWidth) {
                    graphics.FillRectangle(blackBrush, 0, 0, 500, 500);
                    x = (wndWidth-bkgWidth)/2;
                } if (bkgHeight<wndHeight) {
                    graphics.FillRectangle(blackBrush, 0, 0, 500, 500);
                    y = (wndHeight-bkgHeight)/2;
                }
                break;
            case 6: // top y, right x
                x = wndWidth-bkgWidth;
                if (bkgHeight<wndHeight) {
                    graphics.FillRectangle(blackBrush, 0, 0, 500, 500);
                    y = (wndHeight-bkgHeight)/2;
                }
                break;
            case 8: // bottom y, centre x
                x = wndWidth/2-player->pos.x;
                y = wndHeight-bkgHeight;
                break;
            case 9: // bottom y, left x
                if (bkgWidth<wndWidth) {
                    graphics.FillRectangle(blackBrush, 0, 0, 500, 500);
                    x = (wndWidth-bkgWidth)/2;
                }
                y = wndHeight-bkgHeight;
                break;
            case 10: // bottom y, right x
                x = wndWidth-bkgWidth;
                y = wndHeight-bkgHeight;
                break;
        }
        
        // set the transform matrix of the brush to stard at the desired location
        Gdiplus::Matrix *matrix = new Gdiplus::Matrix(1.0f,0.0f,0.0f,1.0f,x,y);
        bkgBrush->SetTransform(matrix);
        // set the transform of the overlay layer, too, so the layers are synced
        overlayBrush->SetTransform(matrix);

        // draw the background layer to the graphics object
        graphics.FillRectangle(bkgBrush, 0, 0, wndWidth, wndHeight);

        // cleanup
        delete blackBrush;
        delete matrix;
    }


    // places a string of text on the graphics object at (x, y)
    void placeText(int x, int y, std::wstring text, 
    Gdiplus::Color colour, int size, Gdiplus::Graphics& graphics) {
        // create the font and colour
        Gdiplus::Font *font = new Gdiplus::Font(L"Arial", size);
        Gdiplus::SolidBrush *brush = new Gdiplus::SolidBrush(colour);

        // place the text
        graphics.DrawString(text.c_str(), -1, font, Gdiplus::PointF(x,y), brush);

        // cleanup
        delete font;
        delete brush;
    }

    // draws an image to the bitmap at (x, y)
    void DrawImageToBitmap(Gdiplus::Bitmap* bitmap, Gdiplus::Image* img, int x, int y)
    {
        // create a graphics object from the bitmap
        Gdiplus::Graphics graphics(bitmap);
        graphics.DrawImage(img, (Gdiplus::REAL)x, (Gdiplus::REAL)y, sideLen+3.0f, sideLen+3.0f);
    }

    // draws a region of an image onto the bitmap
    void DrawImageToBitmap(Gdiplus::Bitmap* bitmap, Gdiplus::Image* img, int x, int y, int srcX, int srcY, int srcWidth, int srcHeight) 
    {
        // create a graphics object from the bitmap
        Gdiplus::Graphics graphics(bitmap);
        graphics.DrawImage(img, x, y, srcX, srcY, srcWidth, srcHeight, Gdiplus::UnitPixel);
    }


    // fills a cell with the water image, as well as any shoreline
    void DrawWaterToCell(Math::Point2 cell, int x, int y)
    {
        // dimensions of grid - 1
        int nx = grid.size()-1, ny = (nx)? grid[0].size()-1 : 0;
        // ensure the cell is in bounds
        if (cell.x<0 || cell.x>nx || cell.y<0 || cell.y>ny) return;
        // ensure the cell is actually water
        if (!(grid[cell.x][cell.y]&WATER)) return;

        // draw the base water image to the cell
        DrawImageToBitmap(background, waterImg, x, y);

        // sample the four adjacent cells to see where shoreline should be drawn

        unsigned char shoreLocations = 0b00000000; // lrLR TBLR
        /*
            METHOD:
            
            the eight bits of shoreLocations represent the following:

            1: there is a shoreline on the TOP side of the cell
            2: there is a shoreline on the RIGHT side of the cell
            3: there is a shoreline on the BOTTOM of the cell
            4: there is a shoreline on the LEFT of the cell

            5: there is shore in the TOP LEFT corner of the cell
            6: there is shore in the TOP RIGHT corner of the cell
            7: there is shore in the BOTTOM RIGHT corner of the cell
            8: there is shore in the BOTTOM LEFT corner of the cell

            run checks on the surrounding cells to see where there is and isn't land, in order to determine
            where there should and shouldn't be shore, setting these bits
        */

        // check the cell above
        if (cell.y > 1) {
            int num = grid[cell.x][cell.y-1];
            if (!(num&WATER || num==BORDER)) shoreLocations |= 0x01;
        }
        // check the cell to the right
        if (cell.x < nx) {
            int num = grid[cell.x+1][cell.y];
            if (!(num&WATER || num==BORDER)) shoreLocations |= 0x02;
        }
        // check the cell below
        if (cell.y < ny) {
            int num = grid[cell.x][cell.y+1];
            if (!(num&WATER || num==BORDER)) shoreLocations |= 0x04;
        }
        // check the cell to the left
        if (cell.x > 1) {
            int num = grid[cell.x-1][cell.y];
            if (!(num&WATER || num==BORDER)) shoreLocations |= 0x08;
        }

        // check the corners
        if (!(shoreLocations&0x09)) { // no top or left shoreline
            // check cell to the top left
            if (cell.x > 1 && cell.y > 1) {
                int num = grid[cell.x-1][cell.y-1];
                if (!(num&WATER || num==BORDER)) shoreLocations |= 0x10;
            }
        }
        if (!(shoreLocations&0x03)) { // no top or right shoreline
            // check the cell to the top right
            if (cell.x < nx && cell.y > 1) {
                int num = grid[cell.x+1][cell.y-1];
                if (!(num&WATER || num==BORDER)) shoreLocations |= 0x20;
            }
        }
        if (!(shoreLocations&0x06)) { // no bottom or right shoreline
            // check the cell to the bottom right
            if (cell.x < nx && cell.y < ny) {
                int num = grid[cell.x+1][cell.y+1];
                if (!(num&WATER || num==BORDER)) shoreLocations |= 0x40;
            }
        }
        if (!(shoreLocations&0x0C)) { // no bottom or left shoreline
            // check the cell to the bottom left
            if (cell.x > 1 && cell.y < ny) {
                int num = grid[cell.x-1][cell.y+1];
                if (!(num&WATER || num==BORDER)) shoreLocations |= 0x80;
            }
        }


        // now, based off shore locations, draw the shorelines
        if (shoreLocations & 0xF0) { // there are corner shorelines
            for (int i = 0; i < 4; i++) {
                // if there is a corner for the specified bit, draw the corresponding corner
                unsigned char bit = shoreLocations & (0x10<<i);
                if (bit) DrawImageToBitmap(background, shoreline4Img, x, y);
                // rotate the image, for the next corner
                shoreline4Img->RotateFlip(Gdiplus::Rotate90FlipNone);
            }
        }

        // count the number of surrounding shores by counting the number of the first four bits activated
        int numShores = Math::numBits(static_cast<int>(shoreLocations&0x0F));

        switch (numShores) 
        {
            case 1:
                // only one shoreline exsists, find it, and draw it
                for (int i = 0; i < 4; i++) {
                    unsigned char bit = shoreLocations & (0x01<<i);
                    if (bit) DrawImageToBitmap(background, shoreline0Img, x, y);
                    shoreline0Img->RotateFlip(Gdiplus::Rotate90FlipNone);
                }
                break;

            case 2: {
                // if the two shores are opposite each other (top/bottom, or left/right), then shoreline0
                // will be drawn twice. if they are next to each other, shoreline1 will be drawn
                int val = shoreLocations&0x0F;

                if ((shoreLocations&0x05)==val || (shoreLocations&0x0A)==val) { // top/bottom OR left/right
                    // draw shoreline0 at any place a bit is active
                    for (int i = 0; i < 4; i++) {
                        unsigned char bit = shoreLocations & (0x01<<i);
                        if (bit) DrawImageToBitmap(background, shoreline0Img, x, y);
                        shoreline0Img->RotateFlip(Gdiplus::Rotate90FlipNone);
                    }
                } else {
                    // draw shoreline1 in the appropriate location
                    // check if the top AND left corners have a shore
                    if ((shoreLocations&0x09)==val) DrawImageToBitmap(background, shoreline1Img, x, y);
                    else {
                        // check all the other side combinations, in order
                        shoreline1Img->RotateFlip(Gdiplus::Rotate90FlipNone);
                        for (int i = 0; i < 3; i++) {
                            unsigned char ref = (0x03<<i), bits = shoreLocations & ref;
                            if (bits == ref) DrawImageToBitmap(background, shoreline1Img, x, y);
                            shoreline1Img->RotateFlip(Gdiplus::Rotate90FlipNone);
                        }
                    }
                }
                break;
            }

            case 3:
                // flip the image vertically first
                shoreline2Img->RotateFlip(Gdiplus::RotateNoneFlipY);

                // find the side that DOESN'T have a shore
                for (int i = 0; i < 4; i++) {
                    unsigned char bit = shoreLocations & 0x01<<i;
                    if (!bit) DrawImageToBitmap(background, shoreline2Img, x, y);
                    shoreline2Img->RotateFlip(Gdiplus::Rotate90FlipNone);
                }
                // reset the vertical flip
                shoreline2Img->RotateFlip(Gdiplus::RotateNoneFlipY);
                break;

            case 4:
                // all four sides occupied, just draw shoreline 3 to the cell
                DrawImageToBitmap(background, shoreline3Img, x, y);
                break;

            default: break; // no shores, nothing more needs to be done
        } 
    }


    // draws every cell onto the background image, and every tree onto the overlay
    void DrawWholeGrid()
    {
        int nx = grid.size(), ny = grid[0].size();
        // iterate over every entry in the grid
        for (int x = 0; x < nx; x++) {
            for (int y = 0; y < ny; y++) 
            {
                Math::Point2 cell(x, y);
                // get the cell type from the existing cell data
                int type = grid[cell.x][cell.y];

                // place 'type' into the cell, this won't change 'grid' at all, but
                // will draw the background and overlay images

                // add the false parameter to avoid updating the brushes every time
                // doing so isn't necesary in this context, and can be done after this loop
                Input::PlaceObjectInCell(cell, type, false);
            }
        }

        // set the background and overlay brushes to represent the background and overlay images
        // set the wrap mode to clamp so they are only drawn once
        Gdiplus::TextureBrush   *newBkgBrush = new Gdiplus::TextureBrush(background),
                                *newOverlayBrush = new Gdiplus::TextureBrush(overlay);

        if (newBkgBrush->GetLastStatus() == Gdiplus::Ok) {
            delete bkgBrush; // clean up the old bakcground brush
            bkgBrush = newBkgBrush;
            bkgBrush->SetWrapMode(Gdiplus::WrapModeClamp);
        }
        if (newOverlayBrush->GetLastStatus() == Gdiplus::Ok) {
            delete overlayBrush; // clean up the old overlay brush
            overlayBrush = newOverlayBrush;
            overlayBrush->SetWrapMode(Gdiplus::WrapModeClamp);
        }
    }

    // draws a tree image onto the overay image. this is done to avoid drawing all the 
    // trees to the frame individually, which dramatically improves performance
    void AddTreeToOverlay(Math::Point2 cell, bool updateOverlayBrush) 
    {
        // to place the base of the tree at 'cell', the image needs to be drawn
        // above and to the left of cell
        cell.x -= treeWidth/2; cell.y -= treeHeight-1;

        // actual position the tree image will be drawn at
        int x = cell.x*sideLen, y = cell.y*sideLen;

        // create a graphics object for image drawing
        Gdiplus::Graphics graphics(overlay);

        // draw the tree onto the overlay image
        graphics.DrawImage(treeImg, (Gdiplus::REAL)x, (Gdiplus::REAL)y, 
            sideLen*treeWidth, sideLen*treeHeight);

        // update the overlay brush, so that the addition is observable
        if (updateOverlayBrush) 
        {
            // clean up the old overlay brush
            delete overlayBrush;

            // create a new overlay brush
            overlayBrush = new Gdiplus::TextureBrush(overlay);
            // set the wrap mode to only draw the overlay once
            overlayBrush->SetWrapMode(Gdiplus::WrapModeClamp);
        }
        // increment the number of objects currently drawn to the overlay
        numObjectsinOverlay++;
    }

    // for convenience
    void AddTreeToOverylay(Math::Point2 cell) {
        AddTreeToOverlay(cell, true);
    }


    // removes a tree image from the overlay image
    void RemoveTreeFromOverlay(Math::Point2 cell) 
    {
        /*  this funtion is currently O(n*m), where
            n = number of trees and m = size of each tree. with the current tree size, 
            25 trees seems to be a reasonable limit for how many trees should be in the level
        */

        // remove tree cell type from grid
        grid[cell.x][cell.y] &= 0xFF9000;
        
        // create a graphics object for editing the overlay
        Gdiplus::Graphics graphics(overlay);

        // clear the existing overlay
        graphics.Clear(Gdiplus::Color::Transparent);
        // reset the number of objects drawn onto the ovelray
        numObjectsinOverlay = 0;

        // draw remaining trees back onto the overlay
        int nx = grid.size(), ny = grid[0].size();
        for (int x = 0; x < nx; x++) {
            for (int y = 0; y < ny; y++) {
                Math::Point2 cell(x, y);
                if (grid[cell.x][cell.y]==TREE) AddTreeToOverlay(cell, false);
            }
        }

        // update overlay brush, so the removal is observable
        // clean up old overlay brush
        delete overlayBrush;

        // create a new overlay brush
        overlayBrush = new Gdiplus::TextureBrush(overlay);
        // set the wrap mode to only draw the overlay once
        overlayBrush->SetWrapMode(Gdiplus::WrapModeClamp);
    }


    // display's information like the player's position, the number of entities, debug commands, and more! :)
    void ShowDebugInfo(Gdiplus::Graphics& graphics) {
        int yPos = 40;
        // player position
        std::wstring posTxt = L"position: ("+
            std::to_wstring(player->centrePos.x)+L", "+
            std::to_wstring(player->centrePos.y)+L')';
        placeText(10, yPos, posTxt, white, 10, graphics); yPos += 15;
        // player cell
        posTxt = L"cell: ("+
            std::to_wstring(player->cell.x)+L", "+ std::to_wstring(player->cell.y)+L')';
        placeText(10, yPos, posTxt, white, 10, graphics); yPos += 15;

        // entity count
        std::wstring entityTxt = L"num entities: "+std::to_wstring(gameObjects.size());
        placeText(10, yPos, entityTxt, white, 10, graphics); yPos += 15;
        // tree count
        entityTxt = L"num objects in overlay: "+std::to_wstring(numObjectsinOverlay);
        placeText(10, yPos, entityTxt, white, 10, graphics); yPos += 15;
        // timed object count
        entityTxt = L"num timed cells: "+std::to_wstring(timedCells.size());
        placeText(10, yPos, entityTxt, white, 10, graphics); yPos += 15;

        // debug tools
        placeText(10, yPos, L"press F8 to toggle speed boost", 
        (debuggingTools&2)? red : white, 10, graphics); yPos += 15;
        placeText(10, yPos, L"press F7 to toggle noclip", 
        (player->hasCollision)? white : red, 10, graphics); yPos += 15;
        placeText(10, yPos, L"press F6 to toggle hitboxes",
        (debuggingTools&4)? red : white, 10, graphics); yPos += 15;

    }

    // debugging tool, covers the cell with a translucent square
    void shadeCell(Gdiplus::Graphics& graphics, Math::Point2 cell) 
    {
        // create a translucent brush to draw with
        Gdiplus::SolidBrush *cellBrush = new Gdiplus::SolidBrush(Gdiplus::Color(100, 0, 0, 255));
        
        // actual position of the cell
        Math::Vector2 o(cell.x*sideLen, cell.y*sideLen);
        // where on the window should be shaded
        Math::Point2 p = Object::getScreenPosition(o);

        // if the cell isn't visible, don't do anything
        if (p.x<-sideLen||p.x>wndWidth||p.y<-sideLen||p.y>wndHeight) return;

        // shade the cell
        graphics.FillRectangle(cellBrush, p.x, p.y, (INT)sideLen, (INT)sideLen);

        // cleanup
        delete cellBrush;
    }




    // changes the alpha value of all the pixels in a bitmap. used for the CRT effect
    void setBitmapAlpha(Gdiplus::Bitmap *bitmap, BYTE alpha)
    {
        // rect representing the entire bitmap
        int nx = bitmap->GetWidth(), ny = bitmap->GetHeight();
        Gdiplus::Rect rect(0, 0, nx, ny);
        Gdiplus::BitmapData bitmapData;

        // lock the bitmap bits
        bitmap->LockBits(&rect, Gdiplus::ImageLockModeWrite, PixelFormat32bppARGB, &bitmapData);

        // get a pointer to the pixel data
        BYTE *pixels = (BYTE*)bitmapData.Scan0;

        // iterate through each pixel and set the alpha value
        for (int y = 0; y < ny; y++) {
            for (int x = 0; x < nx; x++) {
                BYTE *pixel = pixels + y * bitmapData.Stride + x * 4;
                pixel[3] = alpha; // set the alpha value (4th byte in ARGB format)
            }
        }

        // unlock the bitmap bits
        bitmap->UnlockBits(&bitmapData);
    }
}
