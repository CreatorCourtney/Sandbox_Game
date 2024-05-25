#include "Globals.hpp"

namespace Globals 
{
    int LoadImages() 
    {
        logImg = Gdiplus::Image::FromFile(L"images/Log.png");
        if (logImg->GetLastStatus() != Gdiplus::Ok) return 2;
        logBrush = new Gdiplus::TextureBrush(logImg);
        logBrush->SetWrapMode(Gdiplus::WrapModeClamp);

        bridgeImg = Gdiplus::Image::FromFile(L"images/Bridge.png");
        if (bridgeImg->GetLastStatus() != Gdiplus::Ok) return 2;
        bridgeBrush = new Gdiplus::TextureBrush(bridgeImg);
        bridgeBrush->SetWrapMode(Gdiplus::WrapModeClamp);

        waterImg = Gdiplus::Image::FromFile(L"images/Ground/Water.png");
        if (waterImg->GetLastStatus() != Gdiplus::Ok) return 2;
        waterBrush = new Gdiplus::TextureBrush(waterImg);
        waterBrush->SetWrapMode(Gdiplus::WrapModeClamp);

        emptyImg = Gdiplus::Image::FromFile(L"images/Ground/Grass.png");
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

        Pine_ConeImg = Gdiplus::Image::FromFile(L"images/Tree/Pine_Cone.png");
        if (Pine_ConeImg->GetLastStatus() != Gdiplus::Ok) return 2;
        Pine_ConeBrush = new Gdiplus::TextureBrush(Pine_ConeImg);
        Pine_ConeBrush->SetWrapMode(Gdiplus::WrapModeClamp);

        saplingImg = Gdiplus::Image::FromFile(L"images/Tree/Sapling.png");
        if (saplingImg->GetLastStatus() != Gdiplus::Ok) return 2;
        saplingBrush = new Gdiplus::TextureBrush(saplingImg);
        saplingBrush->SetWrapMode(Gdiplus::WrapModeClamp);

        shoreline0Img = Gdiplus::Image::FromFile(L"images/Ground/Shoreline0.png");
        if (shoreline0Img->GetLastStatus() != Gdiplus::Ok) return 2;
        shoreline0Brush = new Gdiplus::TextureBrush(shoreline0Img);
        shoreline0Brush->SetWrapMode(Gdiplus::WrapModeClamp);

        shoreline1Img = Gdiplus::Image::FromFile(L"images/Ground/Shoreline1.png");
        if (shoreline1Img->GetLastStatus() != Gdiplus::Ok) return 2;
        shoreline1Brush = new Gdiplus::TextureBrush(shoreline1Img);
        shoreline1Brush->SetWrapMode(Gdiplus::WrapModeClamp);

        shoreline2Img = Gdiplus::Image::FromFile(L"images/Ground/Shoreline2.png");
        if (shoreline2Img->GetLastStatus() != Gdiplus::Ok) return 2;
        shoreline2Brush = new Gdiplus::TextureBrush(shoreline2Img);
        shoreline2Brush->SetWrapMode(Gdiplus::WrapModeClamp);

        shoreline3Img = Gdiplus::Image::FromFile(L"images/Ground/Shoreline3.png");
        if (shoreline3Img->GetLastStatus() != Gdiplus::Ok) return 2;
        shoreline3Brush = new Gdiplus::TextureBrush(shoreline3Img);
        shoreline3Brush->SetWrapMode(Gdiplus::WrapModeClamp);

        shoreline4Img = Gdiplus::Image::FromFile(L"images/Ground/Shoreline4.png");
        if (shoreline4Img->GetLastStatus() != Gdiplus::Ok) return 2;
        shoreline4Brush = new Gdiplus::TextureBrush(shoreline4Img);
        shoreline4Brush->SetWrapMode(Gdiplus::WrapModeClamp);

        // load player animations
        {
            std::vector<Gdiplus::TextureBrush*> front, back, left, right;
            float Ft = 1.0f, Bt = 1.0f, Lt = 1.0f, Rt = 1.0f;
            for (int i = 0; i < 12; i++) {
                std::wstring txt = L"images/Player/front/" + std::to_wstring(i) + L".png";
                Gdiplus::Image * img = Gdiplus::Image::FromFile(txt.c_str());
                Gdiplus::TextureBrush *brush = new Gdiplus::TextureBrush(img);
                front.push_back(brush);
                // cleanup
                delete img;
            }
            for (int i = 0; i < 12; i++) {
                std::wstring txt = L"images/Player/back/" + std::to_wstring(i) + L".png";
                Gdiplus::Image * img = Gdiplus::Image::FromFile(txt.c_str());
                Gdiplus::TextureBrush *brush = new Gdiplus::TextureBrush(img);
                back.push_back(brush);
                // cleanup
                delete img;
            }
            for (int i = 0; i < 12; i++) {
                std::wstring txt = L"images/Player/right/" + std::to_wstring(i) + L".png";
                Gdiplus::Image * img = Gdiplus::Image::FromFile(txt.c_str());
                Gdiplus::TextureBrush *brush = new Gdiplus::TextureBrush(img);
                right.push_back(brush);
                // cleanup
                delete img;
            }
            for (int i = 0; i < 12; i++) {
                std::wstring txt = L"images/Player/left/" + std::to_wstring(i) + L".png";
                Gdiplus::Image * img = Gdiplus::Image::FromFile(txt.c_str());
                Gdiplus::TextureBrush *brush = new Gdiplus::TextureBrush(img);
                left.push_back(brush);
                // cleanup
                delete img;
            }
            playerAnimations = Object::Animations(front, back, left, right, Ft, Bt, Lt, Rt);
        }
        
        return 0;
    }

    void SetGrid()
    {
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



    // finds the index of a point in a vector of points,
    // returns -1 if the point is not in the vector
    int findPointIndexInVector(Math::Point2 p, std::vector<Math::Point2> vec)
    {
        // set the result to -1, get the length of the vector
        int idx = -1, n = vec.size();
        // check all items in vec to see if p is there
        for (int i = 0; i < n; i++) {
            if (vec[i] == p) {
                // if p is found, update idx and exit the loop
                idx = i; 
                break;
            }
        }
        // return the index found
        return idx;
    }

    // updates all the cells with time elements (like sapling growth)
    void updateTimedCells()
    {
        for (int i = 0; i < timedCells.size(); i++)
        {
            // get the cell info from the grid
            Math::Point2 cell = timedCells[i];
            int cellInfo = grid[cell.x][cell.y];

            // get the timer bits of cellInfo as a regular int
            int currTimer = (cellInfo&TIMER)>>26;

            switch (cellInfo & 255) // just the first byte (cell's ID)
            {
                case 5: // sapling

                    // growth condition met
                    if (((int)g_time%saplingTime)==0) 
                    {
                        // because the exact amount of time passed won't always be an exact integer,
                        // currTimer will be incremented when g_time is close ENOUGH to an integer
                        
                        // d = amount of time from last INTEGER
                        float d = g_time - (int)g_time;

                        if (d <= 0.01f) // margin of error
                            currTimer++; // increment the time when successful
                    }

                    
                    if (currTimer >= 10) 
                    {
                        // when the sapling grows, a tree will be placed in the cell
                        Input::PlaceObjectInCell(cell, TREE);

                        // remove the cell from the vector, and decrement i, accordingly
                        timedCells.erase(timedCells.begin() + i--);
                        continue;
                    }
                    break;

                default: continue; // not actually a timed object
            }

            // update the cell's timer info
            currTimer <<= 26; // set the bits of currTimer back in the correct position
            // clear the cellInfo's timer bits
            cellInfo &= ~TIMER;
            // set the timer bits with currTimer
            cellInfo |= currTimer;

            // update the actual grid
            grid[cell.x][cell.y] = cellInfo;
        }
    }


    // loads the attributes of a level object into the global variables
    void LoadSceneFromLevelObject(Storage::Level level)
    {
        // load information about the player
        player = level.player; // load the player itself
        heldObject = level.heldObject; // load the object the player is holding
        buildingType = level.buildingType; // load what the player is building with

        // load information about the rest of the scene
        gameObjects = level.gameObjects; // load all of the game objects

        // load information about the map
        sideLen = level.sideLen; // load the dimensions of each cell
        interactRange = interactRangeCells * sideLen; // set the interaction radius
        grid = level.grid; // load the array of all cells

        // put all timed cells in the timedCells vector

        // set the current time
        g_time = level.time;

        // initialise the background/overlay images
        SetGrid();
    }
    
    // stores the global variables into a level object
    Storage::Level SaveSceneToLevelObject()
    {
        // create a level object using global variables
        Storage::Level scene(player, heldObject, buildingType,
            gameObjects, sideLen, grid, g_time);


        // return the level object created
        return scene;
    }

    // loads the user's settings into global variables
    void LoadUserSettingsToObject(Storage::UserSettings settings)
    {
        g_scale = settings.g_scale; // load global scale
        debuggingTools = settings.debugTools; // load which developer tools the user has enabled
    }

    // saves settings globals into a UserSettings object
    Storage::UserSettings saveUserSettingsToObject()
    {
        // create a settings object using settings globals
        Storage::UserSettings settings(g_scale, debuggingTools);

        // reutrn the settings object created
        return settings;
    }
}