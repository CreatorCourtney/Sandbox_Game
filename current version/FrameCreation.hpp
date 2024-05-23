#ifndef FRAME_CREATION_HPP
#define FRAME_CREATION_HPP

#include <Windows.h>
#include <gdiplus.h>
#include <string>

#include "Globals.hpp"
#include "GameObjects.hpp"
#include "PlayerInput.hpp"

namespace Frame 
{
    void InitialiseFrameCreation(); // initialise GDI+ and load all the images
    void ShutDownFrameCreation(); // shut down GDI+

    // create a buffer frame onto an offscreen DC. once created, this 
    // frame will be placed onto the screen for the player to actually see
    void CreateBufferFrame(HWND hwnd, HDC hdc);


    // display's information like the player's position, the number of entities, debug commands, and more! :)
    void ShowDebugInfo(Gdiplus::Graphics& graphics);


    // draws a part of the background onto the graphics object. the part drawn determines
    // on the position of the player, since the camera follows the player
    // this function also sets the transform of the overlay image, so the two are synced
    void DrawBackgroundSection(Gdiplus::Graphics& graphics);
    
    // debugging tool, covers the cell with a translucent square
    void shadeCell(Gdiplus::Graphics& graphics, Math::Point2 cell);



    // draws an image to the bitmap at (x, y)
    void DrawImageToBitmap(Gdiplus::Bitmap* bitmap, Gdiplus::Image* img, int x, int y);
    // draws a region of an image onto the bitmap
    void DrawImageToBitmap(Gdiplus::Bitmap* bitmap, Gdiplus::Image* img, int x, int y, int srcX, int srcY, int srcWidth, int srcHeight);
    
    // draws a tree image onto the overay image. this is done to avoid drawing all the 
    // trees to the frame individually, which dramatically improves performance
    void AddTreeToOverlay(Math::Point2 cell, bool updateOverlayBrush);
    // for convenience
    void AddTreeToOverlay(Math::Point2 cell);

    // removes a tree image from the overlay image
    void RemoveTreeFromOverlay(Math::Point2 cell);



    // draws every cell onto the background image, and every tree onto the overlay
    void DrawWholeGrid();

    // places a string of text on the graphics object at (x, y)
    void placeText(int x, int y, std::wstring text, 
    Gdiplus::Color colour, int size, Gdiplus::Graphics& graphics);
}

#endif