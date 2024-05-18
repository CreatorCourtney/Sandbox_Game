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
    }

    // shut down GDI+
    void ShutDownFrameCreation()
    {
        // shut down gdiplus
        Gdiplus::GdiplusShutdown(gdiplusToken);
    }


    // create a buffer frame onto an offscreen DC. once created, this 
    // frame will be placed onto the screen for the player to actually see
    void CreateBufferFrame(HWND hwnd, HDC hdc)
    {
        // create a graphics object associated with the offscreen DC
        Gdiplus::Graphics graphics(hOffscreenDC);

        // background layer: things drawn below the game objects, like the floor, walls, etc
        // the whole background is drawn as one image to improve performance
        DrawBackgroundSection(graphics);

        // game object layer: game objects that move and/or have animation
        // the cannot be drawn into one image since, they usually don't just sit still
        for (int i = 0; i < gameObjects.size(); i++) {
            gameObjects[i]->draw(graphics);
        }
        
        // overlay layer: for static objects that will be drawn ABOVE the game objects
        // this includes things like trees, this layer gives the effect that game objects
        // are moving under it
        graphics.FillRectangle(overlayBrush, 0, 0, wndWidth, wndHeight);


        // pause menu
        if (gameIsPaused) 
        {
            // cover the window with a translucent black cover, to indicate pause
            Gdiplus::SolidBrush *pauseBrush = new Gdiplus::SolidBrush(Gdiplus::Color(180,0,0,0));
            Gdiplus::Rect rect(0,0,wndWidth,wndHeight); // cover the whole window
            graphics.FillRectangle(pauseBrush, rect);

            // cleanup
            delete pauseBrush;
        }


        // developer debug tools
        // show fps
        int fps = 1.0f/deltaTime;
        if (fps<0) fps = prevFPS; // dTime = 0
        else prevFPS = fps;
        std::wstring fpsText = std::to_wstring(fps)+L" FPS";
        placeText(10, 10, fpsText, white, 10, graphics);

        // other debug tools
        placeText(10, 25, L"press F9 to toggle debug menu", white, 10, graphics);
        if (debuggingTools&1) ShowDebugInfo(graphics);


        // cleanup
        graphics.ReleaseHDC(hOffscreenDC);
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
        graphics.DrawImage(img, (Gdiplus::REAL)x, (Gdiplus::REAL)y, sideLen, sideLen);
    }

    // draws a region of an image onto the bitmap
    void DrawImageToBitmap(Gdiplus::Bitmap* bitmap, Gdiplus::Image* img, int x, int y, int srcX, int srcY, int srcWidth, int srcHeight) 
    {
        // create a graphics object from the bitmap
        Gdiplus::Graphics graphics(bitmap);
        graphics.DrawImage(img, x, y, srcX, srcY, srcWidth, srcHeight, Gdiplus::UnitPixel);
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
        bkgBrush = new Gdiplus::TextureBrush(background);
        bkgBrush->SetWrapMode(Gdiplus::WrapModeClamp);
        overlayBrush = new Gdiplus::TextureBrush(overlay);
        overlayBrush->SetWrapMode(Gdiplus::WrapModeClamp);
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
}
