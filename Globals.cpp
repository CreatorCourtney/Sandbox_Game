#ifndef GLOBALS
#define GLOBALS

#include "Globals.hpp"

namespace Globals {
    int LoadImages() {
        playerImg = Gdiplus::Image::FromFile(L"images/Player.png");
        if (playerImg->GetLastStatus() != Gdiplus::Ok) return 2;
        logImg = Gdiplus::Image::FromFile(L"images/Log.png");
        if (logImg->GetLastStatus() != Gdiplus::Ok) return 2;
        bridgeImg = Gdiplus::Image::FromFile(L"images/Bridge.png");
        if (bridgeImg->GetLastStatus() != Gdiplus::Ok) return 2;
        waterImg = Gdiplus::Image::FromFile(L"images/Water.png");
        if (waterImg->GetLastStatus() != Gdiplus::Ok) return 2;
        emptyImg = Gdiplus::Image::FromFile(L"images/EmptyTile.png");
        if (emptyImg->GetLastStatus() != Gdiplus::Ok) return 2;
        hotbarImg = Gdiplus::Image::FromFile(L"images/Hotbar.png");
        if (hotbarImg->GetLastStatus() != Gdiplus::Ok) return 2;

        // hotbar images
        hotbarButtons[0] = new Inventory::HotbarSlot(Gdiplus::Rect(10, 25,100,100), LOG, 100);
        hotbarButtons[1] = new Inventory::HotbarSlot(Gdiplus::Rect(120,25,100,100), BRIDGE, 100);
        hotbarButtons[2] = new Inventory::HotbarSlot(Gdiplus::Rect(230,25,100,100), 0, 20);
        hotbarButtons[3] = new Inventory::HotbarSlot(Gdiplus::Rect(340,25,100,100), 0, 20);
        hotbarButtons[4] = new Inventory::HotbarSlot(Gdiplus::Rect(450,25,100,100), 0, 20);
        hotbarButtons[5] = new Inventory::HotbarSlot(Gdiplus::Rect(560,58, 30, 67), 0, 20);

        return 0;
    }

    void SetGrid()
    {
        // cell size is relative to the size of the player's hitbox
        // but add 2 so they can more easily walk through 1 wide gaps
        sideLen = Math::maxf(player->size.x, player->size.y)+2.0f;

        grid = {
            {0,0,0,0,0,0,0,WATER|BARRIER,WATER|BARRIER,WATER|BARRIER,WATER|BARRIER,WATER|BARRIER,WATER|BARRIER,0,0,0,0,0,0,0},
            {0,0,0,0,0,0,0,WATER|BARRIER,WATER|BARRIER,WATER|BARRIER,WATER|BARRIER,WATER|BARRIER,WATER|BARRIER,0,0,0,0,0,0,0},
            {0,0,0,0,0,0,0,WATER|BARRIER,WATER|BARRIER,WATER|BARRIER,WATER|BARRIER,WATER|BARRIER,WATER|BARRIER,0,0,0,0,0,0,0},
            {0,0,0,0,0,0,0,WATER|BARRIER,WATER|BARRIER,WATER|BARRIER,WATER|BARRIER,WATER|BARRIER,WATER|BARRIER,0,0,0,0,0,0,0},
            {0,0,0,0,0,0,0,WATER|BARRIER,WATER|BARRIER,WATER|BARRIER,WATER|BARRIER,WATER|BARRIER,WATER|BARRIER,0,0,0,0,0,0,0},
            {0,0,0,0,0,0,0,WATER|BARRIER,WATER|BARRIER,WATER|BARRIER,WATER|BARRIER,WATER|BARRIER,WATER|BARRIER,0,0,0,0,0,0,0},
            {0,0,0,0,0,0,0,WATER|BARRIER,WATER|BARRIER,WATER|BARRIER,WATER|BARRIER,WATER|BARRIER,WATER|BARRIER,0,0,0,0,0,0,0},
            {0,0,0,0,0,0,0,WATER|BARRIER,WATER|BARRIER,WATER|BARRIER,WATER|BARRIER,WATER|BARRIER,WATER|BARRIER,0,0,0,0,0,0,0},
            {0,0,0,0,0,0,0,WATER|BARRIER,WATER|BARRIER,WATER|BARRIER,WATER|BARRIER,WATER|BARRIER,WATER|BARRIER,0,0,0,0,0,0,0},
            {0,0,0,0,0,0,0,WATER|BARRIER,WATER|BARRIER,WATER|BARRIER,WATER|BARRIER,WATER|BARRIER,WATER|BARRIER,0,0,0,0,0,0,0},
            {0,0,0,0,0,0,0,WATER|BARRIER,WATER|BARRIER,WATER|BARRIER,WATER|BARRIER,WATER|BARRIER,WATER|BARRIER,0,0,0,0,0,0,0},
            {0,0,0,0,0,0,0,WATER|BARRIER,WATER|BARRIER,WATER|BARRIER,WATER|BARRIER,WATER|BARRIER,WATER|BARRIER,0,0,0,0,0,0,0},
            {0,0,0,0,0,0,0,WATER|BARRIER,WATER|BARRIER,WATER|BARRIER,WATER|BARRIER,WATER|BARRIER,WATER|BARRIER,0,0,0,0,0,0,0},
            {0,0,0,0,0,0,0,WATER|BARRIER,WATER|BARRIER,WATER|BARRIER,WATER|BARRIER,WATER|BARRIER,WATER|BARRIER,0,0,0,0,0,0,0},
            {0,0,0,0,0,0,0,WATER|BARRIER,WATER|BARRIER,WATER|BARRIER,WATER|BARRIER,WATER|BARRIER,WATER|BARRIER,0,0,0,0,0,0,0},
            {0,0,0,0,0,0,0,WATER|BARRIER,WATER|BARRIER,WATER|BARRIER,WATER|BARRIER,WATER|BARRIER,WATER|BARRIER,0,0,0,0,0,0,0},
            {0,0,0,0,0,0,0,WATER|BARRIER,WATER|BARRIER,WATER|BARRIER,WATER|BARRIER,WATER|BARRIER,WATER|BARRIER,0,0,0,0,0,0,0},
            {0,0,0,0,0,0,0,WATER|BARRIER,WATER|BARRIER,WATER|BARRIER,WATER|BARRIER,WATER|BARRIER,WATER|BARRIER,0,0,0,0,0,0,0},
            {0,0,0,0,0,0,0,WATER|BARRIER,WATER|BARRIER,WATER|BARRIER,WATER|BARRIER,WATER|BARRIER,WATER|BARRIER,0,0,0,0,0,0,0},
            {0,0,0,0,0,0,0,WATER|BARRIER,WATER|BARRIER,WATER|BARRIER,WATER|BARRIER,WATER|BARRIER,WATER|BARRIER,0,0,0,0,0,0,0},
            {0,0,0,0,0,0,0,WATER|BARRIER,WATER|BARRIER,WATER|BARRIER,WATER|BARRIER,WATER|BARRIER,WATER|BARRIER,0,0,0,0,0,0,0},
            {0,0,0,0,0,0,0,WATER|BARRIER,WATER|BARRIER,WATER|BARRIER,WATER|BARRIER,WATER|BARRIER,WATER|BARRIER,0,0,0,0,0,0,0},
            {0,0,0,0,0,0,0,WATER|BARRIER,WATER|BARRIER,WATER|BARRIER,WATER|BARRIER,WATER|BARRIER,WATER|BARRIER,0,0,0,0,0,0,0},
            {0,0,0,0,0,0,0,WATER|BARRIER,WATER|BARRIER,WATER|BARRIER,WATER|BARRIER,WATER|BARRIER,WATER|BARRIER,0,0,0,0,0,0,0},
            {0,0,0,0,0,0,0,WATER|BARRIER,WATER|BARRIER,WATER|BARRIER,WATER|BARRIER,WATER|BARRIER,WATER|BARRIER,0,0,0,0,0,0,0},
            {0,0,0,0,0,0,0,WATER|BARRIER,WATER|BARRIER,WATER|BARRIER,WATER|BARRIER,WATER|BARRIER,WATER|BARRIER,0,0,0,0,0,0,0},
            {0,0,0,0,0,0,0,WATER|BARRIER,WATER|BARRIER,WATER|BARRIER,WATER|BARRIER,WATER|BARRIER,WATER|BARRIER,0,0,0,0,0,0,0},
            {0,0,0,0,0,0,0,WATER|BARRIER,WATER|BARRIER,WATER|BARRIER,WATER|BARRIER,WATER|BARRIER,WATER|BARRIER,0,0,0,0,0,0,0},
            {0,0,0,0,0,0,0,WATER|BARRIER,WATER|BARRIER,WATER|BARRIER,WATER|BARRIER,WATER|BARRIER,WATER|BARRIER,0,0,0,0,0,0,0},
            {0,0,0,0,0,0,0,WATER|BARRIER,WATER|BARRIER,WATER|BARRIER,WATER|BARRIER,WATER|BARRIER,WATER|BARRIER,0,0,0,0,0,0,0},
            {0,0,0,0,0,0,0,WATER|BARRIER,WATER|BARRIER,WATER|BARRIER,WATER|BARRIER,WATER|BARRIER,WATER|BARRIER,0,0,0,0,0,0,0},
            {0,0,0,0,0,0,0,WATER|BARRIER,WATER|BARRIER,WATER|BARRIER,WATER|BARRIER,WATER|BARRIER,WATER|BARRIER,0,0,0,0,0,0,0},
            {0,0,0,0,0,0,0,WATER|BARRIER,WATER|BARRIER,WATER|BARRIER,WATER|BARRIER,WATER|BARRIER,WATER|BARRIER,0,0,0,0,0,0,0},
            {0,0,0,0,0,0,0,WATER|BARRIER,WATER|BARRIER,WATER|BARRIER,WATER|BARRIER,WATER|BARRIER,WATER|BARRIER,0,0,0,0,0,0,0},
            {0,0,0,0,0,0,0,WATER|BARRIER,WATER|BARRIER,WATER|BARRIER,WATER|BARRIER,WATER|BARRIER,WATER|BARRIER,0,0,0,0,0,0,0},
            {0,0,0,0,0,0,0,WATER|BARRIER,WATER|BARRIER,WATER|BARRIER,WATER|BARRIER,WATER|BARRIER,WATER|BARRIER,0,0,0,0,0,0,0},
            {0,0,0,0,0,0,0,WATER|BARRIER,WATER|BARRIER,WATER|BARRIER,WATER|BARRIER,WATER|BARRIER,WATER|BARRIER,0,0,0,0,0,0,0},
            {0,0,0,0,0,0,0,WATER|BARRIER,WATER|BARRIER,WATER|BARRIER,WATER|BARRIER,WATER|BARRIER,WATER|BARRIER,0,0,0,0,0,0,0},
            {0,0,0,0,0,0,0,WATER|BARRIER,WATER|BARRIER,WATER|BARRIER,WATER|BARRIER,WATER|BARRIER,WATER|BARRIER,0,0,0,0,0,0,0},
            {0,0,0,0,0,0,0,WATER|BARRIER,WATER|BARRIER,WATER|BARRIER,WATER|BARRIER,WATER|BARRIER,WATER|BARRIER,0,0,0,0,0,0,0}
        };
        bkgWidth = sideLen * grid.size(); bkgHeight = sideLen * grid[0].size();
        background = createEmptyBitmap(bkgWidth, bkgHeight);
    }

    Gdiplus::Bitmap * createEmptyBitmap(int width, int height)
    {
        // create an empty bitmap of size bkgWidth x bkgHeight
        Gdiplus::Bitmap * bmp = new Gdiplus::Bitmap(width, height, PixelFormat32bppARGB);
        if (bmp->GetLastStatus() != Gdiplus::Ok) {
            std::cout << "failed to create bitmap\n";
            delete bmp;
            return nullptr;
        }
        return bmp;
    }
}

#endif