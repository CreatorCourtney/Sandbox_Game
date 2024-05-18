#ifndef GLOBALS
#define GLOBALS

#include "Globals.hpp"

namespace Globals {
    int LoadImages() {
        logImg = Gdiplus::Image::FromFile(L"images/Log.png");
        if (logImg->GetLastStatus() != Gdiplus::Ok) return 2;
        logBrush = new Gdiplus::TextureBrush(logImg);
        logBrush->SetWrapMode(Gdiplus::WrapModeClamp);

        bridgeImg = Gdiplus::Image::FromFile(L"images/Bridge.png");
        if (bridgeImg->GetLastStatus() != Gdiplus::Ok) return 2;
        bridgeBrush = new Gdiplus::TextureBrush(bridgeImg);
        bridgeBrush->SetWrapMode(Gdiplus::WrapModeClamp);

        waterImg = Gdiplus::Image::FromFile(L"images/Water.png");
        if (waterImg->GetLastStatus() != Gdiplus::Ok) return 2;
        waterBrush = new Gdiplus::TextureBrush(waterImg);
        waterBrush->SetWrapMode(Gdiplus::WrapModeClamp);

        emptyImg = Gdiplus::Image::FromFile(L"images/EmptyTile.png");
        if (emptyImg->GetLastStatus() != Gdiplus::Ok) return 2;
        grassBrush = new Gdiplus::TextureBrush(emptyImg);
        grassBrush->SetWrapMode(Gdiplus::WrapModeClamp);

        hotbarImg = Gdiplus::Image::FromFile(L"images/Hotbar.png");
        if (hotbarImg->GetLastStatus() != Gdiplus::Ok) return 2;
        hotbarBrush = new Gdiplus::TextureBrush(hotbarImg);
        hotbarBrush->SetWrapMode(Gdiplus::WrapModeClamp);

        treeImg = Gdiplus::Image::FromFile(L"images/Tree/Tree.png");
        if (treeImg->GetLastStatus() != Gdiplus::Ok) return 2;
        treeBrush = new Gdiplus::TextureBrush(treeImg);
        treeBrush->SetWrapMode(Gdiplus::WrapModeClamp);

        wolfImg = Gdiplus::Image::FromFile(L"images/Wolf.png");
        if (wolfImg->GetLastStatus() != Gdiplus::Ok) return 2;
        wolfBrush = new Gdiplus::TextureBrush(wolfImg);
        wolfBrush->SetWrapMode(Gdiplus::WrapModeClamp);
        
        falling_treeImg = Gdiplus::Image::FromFile(L"images/Tree/Falling_Tree.png");
        if (falling_treeImg->GetLastStatus() != Gdiplus::Ok) return 2;
        falling_treeBrush = new Gdiplus::TextureBrush(falling_treeImg);
        falling_treeBrush->SetWrapMode(Gdiplus::WrapModeClamp);

        stumpImg = Gdiplus::Image::FromFile(L"images/Tree/Stump.png");
        if (stumpImg->GetLastStatus() != Gdiplus::Ok) return 2;
        stumpBrush = new Gdiplus::TextureBrush(stumpImg);
        stumpBrush->SetWrapMode(Gdiplus::WrapModeClamp);

        // load player animations
        {
            std::vector<Gdiplus::TextureBrush*> front, back, left, right;
            float Ft = 1.0f, Bt = 1.0f, Lt = 1.0f, Rt = 1.0f;
            for (int i = 0; i < 12; i++) {
                std::wstring txt = L"images/Player/front/" + std::to_wstring(i) + L".png";
                Gdiplus::Image * img = Gdiplus::Image::FromFile(txt.c_str());
                Gdiplus::TextureBrush *brush = new Gdiplus::TextureBrush(img);
                front.push_back(brush);
            }
            for (int i = 0; i < 12; i++) {
                std::wstring txt = L"images/Player/back/" + std::to_wstring(i) + L".png";
                Gdiplus::Image * img = Gdiplus::Image::FromFile(txt.c_str());
                Gdiplus::TextureBrush *brush = new Gdiplus::TextureBrush(img);
                back.push_back(brush);
            }
            for (int i = 0; i < 12; i++) {
                std::wstring txt = L"images/Player/right/" + std::to_wstring(i) + L".png";
                Gdiplus::Image * img = Gdiplus::Image::FromFile(txt.c_str());
                Gdiplus::TextureBrush *brush = new Gdiplus::TextureBrush(img);
                right.push_back(brush);
            }
            for (int i = 0; i < 12; i++) {
                std::wstring txt = L"images/Player/left/" + std::to_wstring(i) + L".png";
                Gdiplus::Image * img = Gdiplus::Image::FromFile(txt.c_str());
                Gdiplus::TextureBrush *brush = new Gdiplus::TextureBrush(img);
                left.push_back(brush);
            }
            playerAnimations = Object::Animations(front, back, left, right, Ft, Bt, Lt, Rt);
        }
        
        return 0;
    }

    void SetGrid()
    {
        grid = {
            {0,0,0,0,0,0,0,WATER|BARRIER,WATER|BARRIER,WATER|BARRIER,WATER|BARRIER,WATER|BARRIER,WATER|BARRIER,0,0,0,0,0,0,0},
            {0,0,0,0,0,0,0,WATER|BARRIER,WATER|BARRIER,WATER|BARRIER,WATER|BARRIER,WATER|BARRIER,WATER|BARRIER,0,0,0,0,0,0,0},
            {0,0,0,0,0,0,0,WATER|BARRIER,WATER|BARRIER,WATER|BARRIER,WATER|BARRIER,WATER|BARRIER,WATER|BARRIER,0,TREE,0,0,0,0,0},
            {0,0,0,TREE,0,0,0,WATER|BARRIER,WATER|BARRIER,WATER|BARRIER,WATER|BARRIER,WATER|BARRIER,WATER|BARRIER,0,0,TREE,0,0,0,0},
            {0,0,0,TREE,0,0,0,WATER|BARRIER,WATER|BARRIER,WATER|BARRIER,WATER|BARRIER,WATER|BARRIER,WATER|BARRIER,0,0,0,0,0,0,0},
            {0,0,0,TREE,0,0,0,WATER|BARRIER,WATER|BARRIER,WATER|BARRIER,WATER|BARRIER,WATER|BARRIER,WATER|BARRIER,0,0,0,TREE,0,0,0},
            {0,0,0,0,0,0,0,WATER|BARRIER,WATER|BARRIER,WATER|BARRIER,WATER|BARRIER,WATER|BARRIER,WATER|BARRIER,0,0,TREE,0,0,0,0},
            {0,0,0,0,0,0,0,WATER|BARRIER,WATER|BARRIER,WATER|BARRIER,WATER|BARRIER,WATER|BARRIER,WATER|BARRIER,0,0,0,0,0,0,0},
            {0,0,0,0,0,0,0,WATER|BARRIER,WATER|BARRIER,WATER|BARRIER,WATER|BARRIER,WATER|BARRIER,WATER|BARRIER,0,0,0,0,0,0,0},
            {0,0,0,0,0,0,0,WATER|BARRIER,WATER|BARRIER,WATER|BARRIER,WATER|BARRIER,WATER|BARRIER,WATER|BARRIER,0,0,0,0,0,0,0},
            {0,TREE,0,TREE,TREE,0,0,WATER|BARRIER,WATER|BARRIER,WATER|BARRIER,WATER|BARRIER,WATER|BARRIER,WATER|BARRIER,0,0,TREE,0,0,0,0},
            {0,TREE,0,TREE,TREE,0,0,WATER|BARRIER,WATER|BARRIER,WATER|BARRIER,WATER|BARRIER,WATER|BARRIER,WATER|BARRIER,0,0,0,0,0,0,0},
            {0,TREE,0,TREE,TREE,0,0,WATER|BARRIER,WATER|BARRIER,WATER|BARRIER,WATER|BARRIER,WATER|BARRIER,WATER|BARRIER,0,0,0,0,0,0,0},
            {0,TREE,0,TREE,TREE,0,0,WATER|BARRIER,WATER|BARRIER,WATER|BARRIER,WATER|BARRIER,WATER|BARRIER,WATER|BARRIER,0,0,0,0,0,0,0},
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
        overlay = createEmptyBitmap(bkgWidth, bkgHeight);
    }

    Gdiplus::Bitmap * createEmptyBitmap(int width, int height)
    {
        // create an empty bitmap of size width x height
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