#ifndef FRAME_CREATION_HPP
#define FRAME_CREATION_HPP

#include <Windows.h>
#include <gdiplus.h>
#include <string>

#include "Globals.hpp"
#include "GameObjects.hpp"
#include "Inventory.hpp"

namespace Frame 
{
    void InitialiseFrameCreation();
    void ShutDownFrameCreation();
    void CreateBufferFrame(HWND hwnd, HDC hdc);

    void ShowDebugInfo(Gdiplus::Graphics& graphics);

    void DrawBackgroundSection(Gdiplus::Graphics& graphics);
    void PlaceObjectInCell(Math::Point2 cell, int objType);
    void PlaceObjectInCell(Math::Point2 cell, int objType, bool updateBkgBrush);
    void DrawCell(Math::Point2 cell, Gdiplus::Graphics& graphics);
    void shadeCell(Gdiplus::Graphics& graphics, Math::Point2 cell);

    void DrawImageToBitmap(Gdiplus::Bitmap* bitmap, Gdiplus::Image* img, int x, int y);
    void DrawImageToBitmap(Gdiplus::Bitmap* bitmap, Gdiplus::Image* img, int x, int y, int srcX, int srcY, int srcWidth, int srcHeight);
    void AddTreeToOverlay(Math::Point2 cell);
    void AddTreeToOverlay(Math::Point2 cell, bool updateOverlayBrush);
    void RemoveTreeFromOverlay(Math::Point2 cell);

    void DrawWholeGrid();

    void placeText(int x, int y, std::wstring text, 
    Gdiplus::Color colour, int size, Gdiplus::Graphics& graphics);
}

#endif