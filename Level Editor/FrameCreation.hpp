#ifndef FRAME_CREATION_HPP
#define FRAME_CREATION_HPP

#include "Globals.hpp"
#include "UserInput.hpp"

#include <Windows.h>
#include <gdiplus.h>
#include <string>

namespace Frame
{
    void InitialiseFrameCreation(); // initialise GDI+ and load all the images
    void ShutDownFrameCreation(); // shut down GDI+

    // create a buffer frame onto an offscreen DC. once created, this 
    // frame will be placed onto the screen for the player to actually see
    void CreateBufferFrame(HWND hwnd, HDC hdc);

    // places a string of text on the graphics object at (x, y)
    void placeText(int x, int y, std::wstring text, 
    Gdiplus::Color colour, int size, Gdiplus::Graphics& graphics);


    // draws the grey background and the divider lines
    void drawEditorBackground(Gdiplus::Graphics& graphics);
    // draws the side panel
    void drawEditorSidePanel(Gdiplus::Graphics& graphics);



    // draws every cell onto the background image, and every tree onto the overlay
    void DrawWholeGrid();


    // draws an image to the bitmap at (x, y)
    void DrawImageToBitmap(Gdiplus::Bitmap* bitmap, Gdiplus::Image* img, int x, int y);


    // draws a tree image onto the overay image. this is done to avoid drawing all the 
    // trees to the frame individually, which dramatically improves performance
    void AddTreeToOverlay(Math::Point2 cell, bool updateOverlayBrush);
    // removes a tree image from the overlay image
    void RemoveTreeFromOverlay(Math::Point2 cell);


    // fills a cell with the water image, as well as any shoreline
    /*
        this function works and is reasonably fast, but the code is so, SO hideous, please
        do not read this lest i be embarrased
    */
    void DrawWaterToCell(Math::Point2 cell, int x, int y);
}

#endif