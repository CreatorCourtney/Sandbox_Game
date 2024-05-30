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

        // create the solid brushes
        bkgBrush = new Gdiplus::SolidBrush(background);
        accent0Brush = new Gdiplus::SolidBrush(accent0);
        accent1Brush = new Gdiplus::SolidBrush(accent1);
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
        

        // draw the mapBackground
        drawEditorBackground(graphics);

        // create the side panel
        drawEditorSidePanel(graphics);

        // draw the map image
        graphics.FillRectangle(mapBkgBrush, mapRect);

        // show all entities
        int n = entities.size();
        for (int i = 0; i < n; i++) {
            entities[i]->show(graphics);
        }

        // draw the tree overlay
        graphics.FillRectangle(mapOverlayBrush, mapRect);


        // show all the buttons
        n = buttons.size();
        for (int i = 0; i < n; i++) {
            buttons[i]->show(graphics);
        }


        // framerate
        int fps = (deltaTime <= 0.0f)? g_fps : 1.0f/deltaTime;
        g_fps = fps;
        std::wstring fpsText = std::to_wstring(fps)+L" FPS";
        placeText (10, topMenuHeight+10, fpsText, white, 10, graphics);

        // cleanup
        graphics.ReleaseHDC(hOffscreenDC);
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



    // draws the grey mapBackground and the divider lines
    void drawEditorBackground(Gdiplus::Graphics& graphics)
    {
        int x = 0, y = topMenuHeight;

        // fill the top bar
        graphics.FillRectangle(accent1Brush, 0, 0, wndWidth, topMenuHeight);
        // draw the divider between the top menu and the rest of the editor
        graphics.FillRectangle(accent0Brush, 0, y, wndWidth, lineThickness);

        // fill the mapBackground of the main editor area
        graphics.FillRectangle(bkgBrush, 0, y, wndWidth, wndHeight-topMenuHeight);

        // the second divider line will be placed 5/6 of the way across the screen
        int d = 5*wndWidth/6;
        graphics.FillRectangle(accent0Brush, d, y, lineThickness, wndHeight-topMenuHeight);
    
        // the next divider line
        // distance between the vertical divider and the window edge
        int dWidth = wndWidth-d; y += 88;
        graphics.FillRectangle(accent0Brush, d, y, dWidth, lineThickness);


        // lines seperating panel buttons from panel
        if (isInObjectsPanel) {
            graphics.FillRectangle(accent0Brush, d+22, y+100, 23, lineThickness);
            graphics.FillRectangle(accent0Brush, d+95, y+100, 151, lineThickness);
        } else {
            graphics.FillRectangle(accent0Brush, d+22, y+100, 73, lineThickness);
            graphics.FillRectangle(accent0Brush, d+145, y+100, 101, lineThickness);
        }
    }

    // draws the side panel
    void drawEditorSidePanel(Gdiplus::Graphics& graphics)
    {
        // the second divider line will be placed 5/6 of the way across the screen
        int d = 5*wndWidth/6, x = d+10, y = topMenuHeight+54, 
            width = wndWidth-d, step = (width+10)/8;

        // dimensions
        placeText(x, topMenuHeight+20, L"Map Dimensions: ", white, 14, graphics);

        // draw textbox mapBackgrounds for dimensions
        graphics.FillRectangle(accent1Brush, x, y, step*3, 24);
        graphics.FillRectangle(accent1Brush, x+4*step, y, 3*step, 24);


        // write the dimensions
        std::wstring    gridXtxt = std::to_wstring(gridSizeX), 
                        gridYtxt = std::to_wstring(gridSizeY);

        placeText(x-12*(gridXtxt.size()-1)+(3*step/2), y+4, gridXtxt, white, 12, graphics);
        placeText(x-12*(gridYtxt.size()-1)+(11*step/2), y+4, gridYtxt, white, 12, graphics);
    }


    // draws every cell onto the background image, and every tree onto the overlay
    void DrawWholeGrid()
    {
        // create new bitmaps for the overlay and background
        mapBackground = createEmptyBitmap(mapRect.Width, mapRect.Height);
        mapOverlay = createEmptyBitmap(mapRect.Width, mapRect.Height);

        // iterate over every entry in the grid
        for (int x = 0; x < gridSizeX; x++) {
            for (int y = 0; y < gridSizeY; y++) 
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
        Gdiplus::TextureBrush *newBrush = new Gdiplus::TextureBrush(mapBackground);
        if (newBrush->GetLastStatus() == Gdiplus::Ok) {
            // cleanup old brush 
            delete mapBkgBrush;
            mapBkgBrush = newBrush;
            // set the transform and scale matrix properly
            mapBkgBrush->TranslateTransform(mapRect.X, mapRect.Y);

            mapBkgBrush->SetWrapMode(Gdiplus::WrapModeClamp);

        } else delete newBrush;


        newBrush = new Gdiplus::TextureBrush(mapOverlay);
        if (newBrush->GetLastStatus() == Gdiplus::Ok) {
            // clean up old brush
            delete mapOverlayBrush;
            mapOverlayBrush = newBrush;
            // set the transform and scale matrix properly
            mapOverlayBrush->TranslateTransform(mapRect.X, mapRect.Y);

            mapOverlayBrush->SetWrapMode(Gdiplus::WrapModeClamp);

        } else delete newBrush;
    }



    // draws an image to the bitmap at (x, y)
    void DrawImageToBitmap(Gdiplus::Bitmap* bitmap, Gdiplus::Image* img, int x, int y)
    {
        // create a graphics object from the bitmap
        Gdiplus::Graphics graphics(bitmap);
        graphics.DrawImage(img, x, y, sideLen, sideLen);
    }


    // draws a tree image onto the overay image. this is done to avoid drawing all the 
    // trees to the frame individually, which dramatically improves performance
    void AddTreeToOverlay(Math::Point2 cell, bool updateOverlayBrush) 
    {
        // to place the base of the tree at 'cell', the image needs to be drawn
        // above and to the left of cell
        cell.x -= treeWidth/2; cell.y -= treeHeight-1;

        // actual position the tree image will be drawn at
        int x = (cell.x*sideLen)+mapRect.X, y = (cell.y*sideLen)+mapRect.Y;

        // create a graphics object for image drawing
        Gdiplus::Graphics graphics(mapOverlay);

        // draw the tree onto the overlay image
        graphics.DrawImage(treeImg, x, y, sideLen*treeWidth, sideLen*treeHeight);

        // update the overlay brush, so that the addition is observable
        if (updateOverlayBrush) 
        {
            // clean up the old overlay brush
            delete mapOverlayBrush;

            // create a new overlay brush
            mapOverlayBrush = new Gdiplus::TextureBrush(mapOverlay);
            // set the wrap mode to only draw the overlay once
            mapOverlayBrush->SetWrapMode(Gdiplus::WrapModeClamp);
        }
    }

    // removes a tree image from the overlay image
    void RemoveTreeFromOverlay(Math::Point2 cell) 
    {
        /*  this funtion is currently O(n*m), where
            n = number of trees and m = size of each tree. with the current tree size, 
            25 trees seems to be a reasonable limit for how many trees should be in the level
        */

        // remove tree cell type from grid
        if (cell.x>0 && cell.x<grid.size() && cell.y>0 && cell.y<grid[0].size())
            grid[cell.x][cell.y] &= 0xFF9000;
        
        // create a graphics object for editing the overlay
        Gdiplus::Graphics graphics(mapOverlay);

        // clear the existing overlay
        graphics.Clear(Gdiplus::Color::Transparent);

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
        delete mapOverlayBrush;

        // create a new overlay brush
        mapOverlayBrush = new Gdiplus::TextureBrush(mapOverlay);
        // set the wrap mode to only draw the overlay once
        mapOverlayBrush->SetWrapMode(Gdiplus::WrapModeClamp);
    }




    // fills a cell with the water image, as well as any shoreline
    /*
        this function works and is reasonably fast, but the code is so, SO hideous, please
        do not read this lest i be embarrased
    */
    void DrawWaterToCell(Math::Point2 cell, int x, int y)
    {
        // ensure the cell is in bounds
        if (cell.x<0 || cell.x>=grid.size() || cell.y<0 || cell.y>=grid[0].size()) return;
        // ensure the cell is actually water
        if (!(grid[cell.x][cell.y]&WATER)) return;

        // draw the base water image to the cell
        DrawImageToBitmap(mapBackground, waterImg, x, y);

        // sample the four adjacent cells to see where shoreline should be drawn

        unsigned char shoreLocations = 0b00000000; // lrLR TBLR
        /*
            METHOD:
            
            the eight bits of shoreLocations represent the following:

            1: there is a shoreline on the RIGHT side of the cell
            2: there is a shoreline on the LEFT side of the cell
            3: there is a shoreline on the TOP of the cell
            4: there is a shoreline on the BOTTOM of the cell

            5: there is shore in the BOTTOM RIGHT corner of the cell
            6: there is shore in the BOTTOM LEFT corner of the cell
            7: there is shore in the TOP RIGHT corner of the cell
            8: there is shore in the TOP LEFT corner of the cell

            run checks on the surrounding cells to see where there is and isn't land, in order to determine
            where there should and shouldn't be shore, setting these bits

            once shoreLocations is created, run a switch statement to determine which shoreline types
            should be drawn, and where
        */
        // dimensions of grid - 1
        int nx = grid.size()-1, ny = (nx)? grid[0].size()-1 : 0;

        // check the cell to the right, x+1. if x>=grid.size(), don't do anything, assume there is no shoreline
        if (cell.x < nx) {
            // if the cell to the right is NOT water, set bit 1;
            if (!(grid[cell.x + 1][cell.y]&WATER)) shoreLocations |= 1;
            else {
                // if applicable, set the right corner bits
                if (cell.y<ny) { // bottom right
                    if (!(grid[cell.x+1][cell.y+1]&WATER)) shoreLocations |= 0x10;
                }
                if (cell.y>0) { // top right
                    if (!(grid[cell.x+1][cell.y-1]&WATER)) shoreLocations |= 0x40;
                }
            }
        }

        // check the cell to the left, x-1. if x <= 0, don't do anything, assume there is no shoreline
        if (cell.x > 0) {
            // if the cell to the left is NOT water, set bit 2
            if (!(grid[cell.x - 1][cell.y]&WATER)) shoreLocations |= 2;
            else {
                // if applicable, set the left corner bits
                if (cell.y<ny) { // bottom left
                    if (!(grid[cell.x-1][cell.y+1]&WATER)) shoreLocations |= 0x20;
                }
                if (cell.y>0) { // top left
                    if (!(grid[cell.x-1][cell.y-1]&WATER)) shoreLocations |= 0x80;
                }
            }
        }

        // check the cell above, y-1. if y <= 0, don't do anything, assume there is no shoreline
        if (cell.y > 0) {
            // if the cell above is NOT water, set bit 3
            if (!(grid[cell.x][cell.y - 1]&WATER)) {
                shoreLocations |= 4;
                // reset top corners, in case they were set
                shoreLocations &= ~0xC0;
            }
            else {
                // if applicable, set the top corner bits
                if (cell.x<ny) { // top right
                    if (!(grid[cell.x+1][cell.y-1]&WATER) && !(shoreLocations&1)) 
                        shoreLocations |= 0x40;
                }
                if (cell.x>0) { // top left
                    if (!(grid[cell.x-1][cell.y-1]&WATER) && !(shoreLocations&2)) 
                        shoreLocations |= 0x80;
                }
            }
        }

        // check the cell below, y+1. if y>=grid[0].size(), don't do anything, assume there is no shoreline
        if (cell.y < ny) {
            // if the cell below is NOT water, set bit 4
            if (!(grid[cell.x][cell.y + 1]&WATER)) {
                shoreLocations |= 8;
                // reset bottom corners, in case they were set
                shoreLocations &= ~0x30;
            }
            else {
                // if applicable, set the bottom corner bits
                if (cell.x<ny) { // bottom right
                    if (!(grid[cell.x+1][cell.y+1]&WATER) && !(shoreLocations&1)) 
                        shoreLocations |= 0x10;
                }
                if (cell.x>0) { // bottom left
                    if (!(grid[cell.x-1][cell.y+1]&WATER) && !(shoreLocations&2)) 
                        shoreLocations |= 0x20;
                }
            }
        }

        // now, based off shore locations, draw the shorelines
        switch (shoreLocations)
        {
            case 0: // no shorelines, do nothing
                break;

            case 1: // only shore on the right
                // rotate shoreline0 then draw it
                shoreline0Img->RotateFlip(Gdiplus::Rotate90FlipNone);
                DrawImageToBitmap(mapBackground, shoreline0Img, x, y);
                // reset transform
                shoreline0Img->RotateFlip(Gdiplus::Rotate270FlipNone);
                break;

            case 2: // only shore on the left
                // rotate shoreline 0 then draw it
                shoreline0Img->RotateFlip(Gdiplus::Rotate270FlipNone);
                DrawImageToBitmap(mapBackground, shoreline0Img, x, y);
                // reset transform
                shoreline0Img->RotateFlip(Gdiplus::Rotate90FlipNone);
                break;

            case 3: // shore on left and right
                // rotate shoreline0 then draw it
                shoreline0Img->RotateFlip(Gdiplus::Rotate90FlipNone);
                DrawImageToBitmap(mapBackground, shoreline0Img, x, y);
                // rotate again for the left side
                shoreline0Img->RotateFlip(Gdiplus::Rotate180FlipNone);
                DrawImageToBitmap(mapBackground, shoreline0Img, x, y);
                // reset transform
                shoreline0Img->RotateFlip(Gdiplus::Rotate90FlipNone);
                break;

            case 4: // only shore on top
                // no rotation necesary
                DrawImageToBitmap(mapBackground, shoreline0Img, x, y);
                break;

            case 5: // shore on top and right
                // rotate shoreline 1 then draw it
                shoreline1Img->RotateFlip(Gdiplus::Rotate90FlipNone);
                DrawImageToBitmap(mapBackground, shoreline1Img, x, y);
                // reset transform
                shoreline1Img->RotateFlip(Gdiplus::Rotate270FlipNone);
                break;

            case 6: // shore on top and left
                // no rotation necesary
                DrawImageToBitmap(mapBackground, shoreline1Img, x, y);
                break;

            case 7: // shore on top left and right
                // no rotation necesary
                DrawImageToBitmap(mapBackground, shoreline2Img, x, y);
                break;

            case 8: // shore on bottom
                // flip shoreline 0 then draw it
                shoreline0Img->RotateFlip(Gdiplus::RotateNoneFlipY);
                DrawImageToBitmap(mapBackground, shoreline0Img, x, y);
                // reset transform
                shoreline0Img->RotateFlip(Gdiplus::RotateNoneFlipY);
                break;

            case 9: // shore on bottom and right
                // rotate shoreline 1 then draw it
                shoreline1Img->RotateFlip(Gdiplus::Rotate180FlipNone);
                DrawImageToBitmap(mapBackground, shoreline1Img, x, y);
                // reset transform
                shoreline1Img->RotateFlip(Gdiplus::Rotate180FlipNone);
                break;

            case 10: // shore on bottom and left
                // flip shoreline 1 then draw it
                shoreline1Img->RotateFlip(Gdiplus::RotateNoneFlipY);
                DrawImageToBitmap(mapBackground, shoreline1Img, x, y);
                // reset transform
                shoreline1Img->RotateFlip(Gdiplus::RotateNoneFlipY);
                break;

            case 11: // shore on bottom left and right
                // flip shoreline 2 then draw it
                shoreline2Img->RotateFlip(Gdiplus::RotateNoneFlipY);
                DrawImageToBitmap(mapBackground, shoreline2Img, x, y);
                // reset transform
                shoreline2Img->RotateFlip(Gdiplus::RotateNoneFlipY);
                break;

            case 12: // shore on top and bottom
                // draw top, no transformation necesary
                DrawImageToBitmap(mapBackground, shoreline0Img, x, y);
                // flip shoreline 0 then draw it
                shoreline0Img->RotateFlip(Gdiplus::RotateNoneFlipY);
                DrawImageToBitmap(mapBackground, shoreline0Img, x, y);
                // reset transform
                shoreline0Img->RotateFlip(Gdiplus::RotateNoneFlipY);
                break;

            case 13: // shore on top, bottom and right
                // rotate shoreline 2 then draw it
                shoreline2Img->RotateFlip(Gdiplus::Rotate90FlipNone);
                DrawImageToBitmap(mapBackground, shoreline2Img, x, y);
                // reset transform
                shoreline2Img->RotateFlip(Gdiplus::Rotate270FlipNone);
                break;

            case 14: // shore on top, bottom, and left
                // rotate shoreline 2 then draw it
                shoreline2Img->RotateFlip(Gdiplus::Rotate270FlipNone);
                DrawImageToBitmap(mapBackground, shoreline2Img, x, y);
                // reset transform
                shoreline2Img->RotateFlip(Gdiplus::Rotate90FlipNone);
                break;

            case 15: // shore on all four sides
                // draw shoreline 3
                DrawImageToBitmap(mapBackground, shoreline3Img, x, y);
                break;

            case 16: // only bottom right corner
                // rotate shoreline 4 then draw it
                shoreline4Img->RotateFlip(Gdiplus::Rotate180FlipNone);
                DrawImageToBitmap(mapBackground, shoreline4Img, x, y);
                // reset transform
                shoreline4Img->RotateFlip(Gdiplus::Rotate180FlipNone);
                break;

            case 17: // should never occur
                break;

            case 18: // bottom right corner, left shore
                // rotate shore 0 and 4
                shoreline0Img->RotateFlip(Gdiplus::Rotate270FlipNone);
                shoreline4Img->RotateFlip(Gdiplus::Rotate180FlipNone);
                // draw the shores
                DrawImageToBitmap(mapBackground, shoreline0Img, x, y);
                DrawImageToBitmap(mapBackground, shoreline4Img, x, y);
                // reset transforms
                shoreline0Img->RotateFlip(Gdiplus::Rotate90FlipNone);
                shoreline4Img->RotateFlip(Gdiplus::Rotate180FlipNone);
                break;

            case 19: // should never occur
                break;

            case 20: // bottom right corner, top side
                // rotate shore 4
                shoreline4Img->RotateFlip(Gdiplus::Rotate180FlipNone);
                // draw the shores
                DrawImageToBitmap(mapBackground, shoreline0Img, x, y);
                DrawImageToBitmap(mapBackground, shoreline4Img, x, y);
                // reset transform
                shoreline4Img->RotateFlip(Gdiplus::Rotate180FlipNone);
                break;

            case 21: // should never occur
                break;

            case 22: // bottom right corner, top and left side
                // rotate shore 4
                shoreline4Img->RotateFlip(Gdiplus::Rotate180FlipNone);
                // draw the shores
                DrawImageToBitmap(mapBackground, shoreline1Img, x, y);
                DrawImageToBitmap(mapBackground, shoreline4Img, x, y);
                // reset transform
                shoreline4Img->RotateFlip(Gdiplus::Rotate180FlipNone);
                break;

            case 23: break; // should never occur

            case 24: break; // should never occur

            // jump to case 32
            case 32: // only bottom left corner
                // rotate shoreline 4 then draw it
                shoreline4Img->RotateFlip(Gdiplus::Rotate270FlipNone);
                DrawImageToBitmap(mapBackground, shoreline4Img, x, y);
                // reset transform
                shoreline4Img->RotateFlip(Gdiplus::Rotate270FlipNone);
                break;

            case 0x21: // bottom left corner, right side
                // rotate shore 0 and 4
                shoreline0Img->RotateFlip(Gdiplus::Rotate90FlipNone);
                shoreline4Img->RotateFlip(Gdiplus::Rotate270FlipNone);
                // draw the shores
                DrawImageToBitmap(mapBackground, shoreline0Img, x, y);
                DrawImageToBitmap(mapBackground, shoreline4Img, x, y);
                // reset transforms
                shoreline0Img->RotateFlip(Gdiplus::Rotate270FlipNone);
                shoreline4Img->RotateFlip(Gdiplus::Rotate90FlipNone);
                break;

            case 0x22: break;

            case 0x23: break;

            case 0x24: // bottom left corner, top side
                // rotate shore 4
                shoreline4Img->RotateFlip(Gdiplus::Rotate270FlipNone);
                // draw the shores
                DrawImageToBitmap(mapBackground, shoreline0Img, x, y);
                DrawImageToBitmap(mapBackground, shoreline4Img, x, y);
                // reset transform
                shoreline4Img->RotateFlip(Gdiplus::Rotate90FlipNone);
                break;

            case 0x25: // bottom left corner, top and right sides
                // rotate shores 1 and 4
                shoreline1Img->RotateFlip(Gdiplus::Rotate90FlipNone);
                shoreline4Img->RotateFlip(Gdiplus::Rotate270FlipNone);
                // draw the shores
                DrawImageToBitmap(mapBackground, shoreline1Img, x, y);
                DrawImageToBitmap(mapBackground, shoreline4Img, x, y);
                // reset transforms
                shoreline1Img->RotateFlip(Gdiplus::Rotate270FlipNone);
                shoreline4Img->RotateFlip(Gdiplus::Rotate90FlipNone);
                break;

            case 0x26: break;

            case 0x30: // bottom left and right corners
                // rotate and draw shore 4
                shoreline4Img->RotateFlip(Gdiplus::Rotate180FlipNone);
                DrawImageToBitmap(mapBackground, shoreline4Img, x, y);
                // rotate and draw
                shoreline4Img->RotateFlip(Gdiplus::Rotate90FlipNone);
                DrawImageToBitmap(mapBackground, shoreline4Img, x, y);
                // reset transform
                shoreline4Img->RotateFlip(Gdiplus::Rotate90FlipNone);
                break;

            case 0x34: // bottom corners top side
                // rotate and draw shore 4
                shoreline4Img->RotateFlip(Gdiplus::Rotate180FlipNone);
                DrawImageToBitmap(mapBackground, shoreline4Img, x, y);
                // rotate and draw
                shoreline4Img->RotateFlip(Gdiplus::Rotate90FlipNone);
                DrawImageToBitmap(mapBackground, shoreline4Img, x, y);
                // draw shore 0
                DrawImageToBitmap(mapBackground, shoreline0Img, x, y);
                // reset transform
                shoreline4Img->RotateFlip(Gdiplus::Rotate90FlipNone);
                break;

            case 0x40: // just top right
                // rotate and draw shore 4
                shoreline4Img->RotateFlip(Gdiplus::Rotate90FlipNone);
                DrawImageToBitmap(mapBackground, shoreline4Img, x, y);
                // reset transform
                shoreline4Img->RotateFlip(Gdiplus::Rotate270FlipNone);
                break;

            case 0x42: // top right, left side
                // rotate and draw shore 4
                shoreline4Img->RotateFlip(Gdiplus::Rotate90FlipNone);
                DrawImageToBitmap(mapBackground, shoreline4Img, x, y);
                // rotate and draw shore 0
                shoreline0Img->RotateFlip(Gdiplus::Rotate270FlipNone);
                DrawImageToBitmap(mapBackground, shoreline0Img, x, y);
                // reset transform
                shoreline4Img->RotateFlip(Gdiplus::Rotate270FlipNone);
                shoreline0Img->RotateFlip(Gdiplus::Rotate90FlipNone);
                break;

            case 0x48: // top right, bottom side
                // rotate and draw shore 4
                shoreline4Img->RotateFlip(Gdiplus::Rotate90FlipNone);
                DrawImageToBitmap(mapBackground, shoreline4Img, x, y);
                // flip and draw shore 0
                shoreline0Img->RotateFlip(Gdiplus::RotateNoneFlipY);
                DrawImageToBitmap(mapBackground, shoreline0Img, x, y);
                // reset transform
                shoreline4Img->RotateFlip(Gdiplus::Rotate270FlipNone);
                shoreline0Img->RotateFlip(Gdiplus::RotateNoneFlipY);
                break;

            case 0x4A: // top right, bottom and left
                // rotate and draw shore 4
                shoreline4Img->RotateFlip(Gdiplus::Rotate90FlipNone);
                DrawImageToBitmap(mapBackground, shoreline4Img, x, y);
                // flip and draw shore 1
                shoreline1Img->RotateFlip(Gdiplus::RotateNoneFlipY);
                DrawImageToBitmap(mapBackground, shoreline1Img, x, y);
                // reset transform
                shoreline4Img->RotateFlip(Gdiplus::Rotate270FlipNone);
                shoreline1Img->RotateFlip(Gdiplus::RotateNoneFlipY);
                break;

            case 0x50: // right corners
                // rotate and draw shore 4
                shoreline4Img->RotateFlip(Gdiplus::Rotate90FlipNone);
                DrawImageToBitmap(mapBackground, shoreline4Img, x, y);
                // rotate and draw shore 4
                shoreline4Img->RotateFlip(Gdiplus::Rotate90FlipNone);
                DrawImageToBitmap(mapBackground, shoreline4Img, x, y);
                // reset transform
                shoreline4Img->RotateFlip(Gdiplus::Rotate180FlipNone);
                break;

            case 0x52: // right corners, left side
                // rotate and draw shore 4
                shoreline4Img->RotateFlip(Gdiplus::Rotate90FlipNone);
                DrawImageToBitmap(mapBackground, shoreline4Img, x, y);
                // rotate and draw shore 4
                shoreline4Img->RotateFlip(Gdiplus::Rotate90FlipNone);
                DrawImageToBitmap(mapBackground, shoreline4Img, x, y);
                // rotate and draw shore 0
                shoreline0Img->RotateFlip(Gdiplus::Rotate270FlipNone);
                DrawImageToBitmap(mapBackground, shoreline0Img, x, y);
                // reset transform
                shoreline4Img->RotateFlip(Gdiplus::Rotate180FlipNone);
                shoreline0Img->RotateFlip(Gdiplus::Rotate90FlipNone);
                break;

            case 0x60: // top right and bottom left corners
                // rotate and draw shore 4
                shoreline4Img->RotateFlip(Gdiplus::Rotate90FlipNone);
                DrawImageToBitmap(mapBackground, shoreline4Img, x, y);
                // rotate and draw shore 4
                shoreline4Img->RotateFlip(Gdiplus::Rotate180FlipNone);
                DrawImageToBitmap(mapBackground, shoreline4Img, x, y);
                // reset transform
                shoreline4Img->RotateFlip(Gdiplus::Rotate90FlipNone);
                break;

            case 0x70: // bottom and right corners
                // rotate and draw shore 4
                shoreline4Img->RotateFlip(Gdiplus::Rotate90FlipNone);
                DrawImageToBitmap(mapBackground, shoreline4Img, x, y);
                // rotate and draw shore 4
                shoreline4Img->RotateFlip(Gdiplus::Rotate90FlipNone);
                DrawImageToBitmap(mapBackground, shoreline4Img, x, y);
                // rotate and draw shore 4
                shoreline4Img->RotateFlip(Gdiplus::Rotate90FlipNone);
                DrawImageToBitmap(mapBackground, shoreline4Img, x, y);
                // reset transform
                shoreline4Img->RotateFlip(Gdiplus::Rotate90FlipNone);
                break;

            case 0x80: // top left corner
                // draw shore 4
                DrawImageToBitmap(mapBackground, shoreline4Img, x, y);
                break;

            case 0x81: // top left, right side
                // rotate shore 0
                shoreline0Img->RotateFlip(Gdiplus::Rotate90FlipNone);
                // draw shores
                DrawImageToBitmap(mapBackground, shoreline4Img, x, y);
                DrawImageToBitmap(mapBackground, shoreline0Img, x, y);
                // reset transform
                shoreline0Img->RotateFlip(Gdiplus::Rotate270FlipNone);
                break;

            case 0x88: // top left, bottom side
                // flip shore 0
                shoreline0Img->RotateFlip(Gdiplus::RotateNoneFlipY);
                // draw shores
                DrawImageToBitmap(mapBackground, shoreline4Img, x, y);
                DrawImageToBitmap(mapBackground, shoreline0Img, x, y);
                // reset transform
                shoreline0Img->RotateFlip(Gdiplus::RotateNoneFlipY);
                break;

            case 0x89: // top left, bottom and right sides
                // rotate shore 1
                shoreline1Img->RotateFlip(Gdiplus::Rotate180FlipNone);
                // draw shores
                DrawImageToBitmap(mapBackground, shoreline4Img, x, y);
                DrawImageToBitmap(mapBackground, shoreline1Img, x, y);
                // reset transform
                shoreline1Img->RotateFlip(Gdiplus::Rotate180FlipNone);
                break;

            case 0x90: // top left and bottom right
                // draw shore 4
                DrawImageToBitmap(mapBackground, shoreline4Img, x, y);
                // rotate and draw shore 4
                shoreline4Img->RotateFlip(Gdiplus::Rotate180FlipNone);
                DrawImageToBitmap(mapBackground, shoreline4Img, x, y);
                // reset transform
                shoreline4Img->RotateFlip(Gdiplus::Rotate180FlipNone);
                break;

            case 0xA0: // left corners
                // draw shore 4
                DrawImageToBitmap(mapBackground, shoreline4Img, x, y);
                // rotate and draw shore 4
                shoreline4Img->RotateFlip(Gdiplus::Rotate270FlipNone);
                DrawImageToBitmap(mapBackground, shoreline4Img, x, y);
                // reset transform
                shoreline4Img->RotateFlip(Gdiplus::Rotate90FlipNone);
                break;

            case 0xA1: // left corners, right side
                // draw shore 4
                DrawImageToBitmap(mapBackground, shoreline4Img, x, y);
                // rotate and draw shore 4
                shoreline4Img->RotateFlip(Gdiplus::Rotate270FlipNone);
                DrawImageToBitmap(mapBackground, shoreline4Img, x, y);
                // rotate and draw shore 0
                shoreline0Img->RotateFlip(Gdiplus::Rotate90FlipNone);
                DrawImageToBitmap(mapBackground, shoreline0Img, x, y);
                // reset transform
                shoreline4Img->RotateFlip(Gdiplus::Rotate90FlipNone);
                shoreline0Img->RotateFlip(Gdiplus::Rotate270FlipNone);
                break;

            case 0xB0: // top left, bottom right, bottom left
                // draw shore 4
                DrawImageToBitmap(mapBackground, shoreline4Img, x, y);
                // rotate and draw shore 4
                shoreline4Img->RotateFlip(Gdiplus::Rotate180FlipNone);
                DrawImageToBitmap(mapBackground, shoreline4Img, x, y);
                // rotate and draw shore 4
                shoreline4Img->RotateFlip(Gdiplus::Rotate90FlipNone);
                DrawImageToBitmap(mapBackground, shoreline4Img, x, y);
                // rotate transform
                shoreline4Img->RotateFlip(Gdiplus::Rotate90FlipNone);
                break;

            case 0xC0: // top corners
                // draw shore 4
                DrawImageToBitmap(mapBackground, shoreline4Img, x, y);
                // flip and draw shore 4
                shoreline4Img->RotateFlip(Gdiplus::RotateNoneFlipY);
                DrawImageToBitmap(mapBackground, shoreline4Img, x, y);
                // reset transform
                shoreline4Img->RotateFlip(Gdiplus::RotateNoneFlipY);
                break;

            case 0xC8: // top corners, bottom side
                // draw shore 4
                DrawImageToBitmap(mapBackground, shoreline4Img, x, y);
                // flip and draw shore 4
                shoreline4Img->RotateFlip(Gdiplus::RotateNoneFlipY);
                DrawImageToBitmap(mapBackground, shoreline4Img, x, y);
                // flip and draw shore 0
                shoreline0Img->RotateFlip(Gdiplus::RotateNoneFlipY);
                DrawImageToBitmap(mapBackground, shoreline0Img, x, y);
                // reset transform
                shoreline4Img->RotateFlip(Gdiplus::RotateNoneFlipY);
                shoreline0Img->RotateFlip(Gdiplus::RotateNoneFlipY);
                break;
            
            case 0xD0: // top corners, bottom right
                // draw shore 4
                DrawImageToBitmap(mapBackground, shoreline4Img, x, y);
                // rotate and draw shore 4
                shoreline4Img->RotateFlip(Gdiplus::Rotate90FlipNone);
                DrawImageToBitmap(mapBackground, shoreline4Img, x, y);
                // rotate and draw shore 4
                shoreline4Img->RotateFlip(Gdiplus::Rotate90FlipNone);
                DrawImageToBitmap(mapBackground, shoreline4Img, x, y);
                // reset transform
                shoreline4Img->RotateFlip(Gdiplus::Rotate180FlipNone);
                break;

            case 0xE0: // top corners, bottom left
                // draw shore 4
                DrawImageToBitmap(mapBackground, shoreline4Img, x, y);
                // rotate and draw shore 4
                shoreline4Img->RotateFlip(Gdiplus::Rotate90FlipNone);
                DrawImageToBitmap(mapBackground, shoreline4Img, x, y);
                // rotate and draw shore 4
                shoreline4Img->RotateFlip(Gdiplus::Rotate180FlipNone);
                DrawImageToBitmap(mapBackground, shoreline4Img, x, y);
                // reset transform
                shoreline4Img->RotateFlip(Gdiplus::Rotate90FlipNone);
                break;

            case 0xF0: // all corners
                // draw and rotate four timed
                for (int i = 0; i < 4; i++) {
                    DrawImageToBitmap(mapBackground, shoreline4Img, x, y);
                    shoreline4Img->RotateFlip(Gdiplus::Rotate90FlipNone);
                }
                break;

            default: 
                std::cout << std::hex<<(int)shoreLocations <<'\n';
                break;
        }  
    }
}