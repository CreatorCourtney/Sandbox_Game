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

        treeImg = Gdiplus::Image::FromFile(L"images/Tree.png");
        if (treeImg->GetLastStatus() != Gdiplus::Ok) return 2;
        treeBrush = new Gdiplus::TextureBrush(treeImg);
        treeBrush->SetWrapMode(Gdiplus::WrapModeClamp);

        wolfImg = Gdiplus::Image::FromFile(L"images/Wolf.png");
        if (wolfImg->GetLastStatus() != Gdiplus::Ok) return 2;
        wolfBrush = new Gdiplus::TextureBrush(wolfImg);
        wolfBrush->SetWrapMode(Gdiplus::WrapModeClamp);

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

        // falling tree animations
        {
            std::vector<Gdiplus::TextureBrush*> anim;
            for (int i = 0; i < 7; i++) {
                std::wstring txt = L"images/Falling_Tree/"+std::to_wstring(i)+L".png";
                Gdiplus::Image *img = Gdiplus::Image::FromFile(txt.c_str());
                Gdiplus::TextureBrush *brush = new Gdiplus::TextureBrush(img);
                anim.push_back(brush);
            }
            falling_treeAnimations.front = anim;
            falling_treeAnimations.frontInter = float(anim.size()) / 1.0f;
        }

        // hotbar images
        {
            hotbarButtons[0] = new Inventory::HotbarSlot(Gdiplus::Rect(10, 25,100,100), LOG, 20);
            hotbarButtons[1] = new Inventory::HotbarSlot(Gdiplus::Rect(120,25,100,100), BRIDGE, 20);
            hotbarButtons[2] = new Inventory::HotbarSlot(Gdiplus::Rect(230,25,100,100), 0, 20);
            hotbarButtons[3] = new Inventory::HotbarSlot(Gdiplus::Rect(340,25,100,100), 0, 20);
            hotbarButtons[4] = new Inventory::HotbarSlot(Gdiplus::Rect(450,25,100,100), 0, 20);
            hotbarButtons[5] = new Inventory::HotbarSlot(Gdiplus::Rect(560,58, 30, 67), 0, 20);
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