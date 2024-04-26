#ifndef GLOBALS_HPP
#define GLOBALS_HPP

#include <Windows.h>
#include <windowsx.h>
#include <WinUser.h>
#include <gdiplus.h>
#include <ctime>
#include <vector>

#include "GameMath.hpp"
#include "GameObjects.hpp"
#include "Inventory.hpp"

namespace Globals {
    int g_scale = 2;
    int wndWidth, wndHeight;
    int bkgWidth, bkgHeight;
    HDC hOffscreenDC, g_hdc;
    HBITMAP hOffscreenBitmap;
    clock_t begin_time = clock();
    float deltaTime = 0.0f;
    int prevFPS = 0;
    bool gameIsPaused = 0;

    unsigned char inputKeys = 0; // 000Swasd
    unsigned char bkgState = 0b00000000;
    /* bit decomposition:
    1: left of screen
    2: right of screen (...00 = centre x)
    3: top of screen
    4: bottom of screen
    00000000 (0) = full centre
    00000001 (1) = centre y, left x
    00000010 (2) = centre y, right x
    00000100 (4) = top y, centre x
    00000101 (5) = top y, left x
    00000110 (6) = top y, right x
    00001000 (8) = bottom y, centre x
    00001001 (9) = bottom y, left x
    00001010 (10)= bottom y, right x */


    Gdiplus::Bitmap * background;
    Gdiplus::Image * playerImg;
    Gdiplus::Image * logImg;
    Gdiplus::Image * bridgeImg;
    Gdiplus::Image * waterImg;
    Gdiplus::Image * emptyImg;
    Gdiplus::Image * hotbarImg;


    Inventory::HotbarSlot * hotbarButtons[6];

    Math::Vector2 wndScale;
    float UIScale = 1.0f, currUIScale;

    Object::GameObject * player;
    std::vector<Object::GameObject*> gameObjects;
    int selectedObj = LOG;


    int LoadImages();

    void SetGrid();

    Gdiplus::Bitmap * createEmptyBitmap(int width, int height);

    float sideLen;
    std::vector<std::vector<int>> grid;
}

#endif