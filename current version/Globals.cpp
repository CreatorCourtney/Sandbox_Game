#include "Globals.hpp"

namespace Globals 
{
    int LoadImages() 
    {
        logImg = Gdiplus::Image::FromFile(L"images/Buildings/Log.png");
        if (logImg->GetLastStatus() != Gdiplus::Ok) return 2;
        logBrush = new Gdiplus::TextureBrush(logImg);
        logBrush->SetWrapMode(Gdiplus::WrapModeClamp);

        bridgeImg = Gdiplus::Image::FromFile(L"images/Buildings/Bridge.png");
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

        treeImg = Gdiplus::Image::FromFile(L"images/Tree/Tree.png");
        if (treeImg->GetLastStatus() != Gdiplus::Ok) return 2;
        treeBrush = new Gdiplus::TextureBrush(treeImg);
        treeBrush->SetWrapMode(Gdiplus::WrapModeClamp);

        wolfImg = Gdiplus::Image::FromFile(L"images/Entities/Wolf.png");
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

        Pine_ConeImg = Gdiplus::Image::FromFile(L"images/Items/Pine_Cone.png");
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

        plankImg = Gdiplus::Image::FromFile(L"images/Items/Plank.png");
        if (plankImg->GetLastStatus() != Gdiplus::Ok) return 2;
        plankBrush = new Gdiplus::TextureBrush(plankImg);
        plankBrush->SetWrapMode(Gdiplus::WrapModeClamp);

        doorImg = Gdiplus::Image::FromFile(L"images/Buildings/Door.png");
        if (doorImg->GetLastStatus() != Gdiplus::Ok) return 2;
        doorBrush = new Gdiplus::TextureBrush(doorImg);
        doorBrush->SetWrapMode(Gdiplus::WrapModeClamp);

        openDoorImg = Gdiplus::Image::FromFile(L"images/Buildings/Open_Door.png");
        if (openDoorImg->GetLastStatus() != Gdiplus::Ok) return 2;
        openDoorBrush = new Gdiplus::TextureBrush(openDoorImg);
        openDoorBrush->SetWrapMode(Gdiplus::WrapModeClamp);

        CRT_baseImg = Gdiplus::Image::FromFile(L"images/CRT_Base_Texture.png");
        if (CRT_baseImg->GetLastStatus() != Gdiplus::Ok) return 2;
        int x = CRT_baseImg->GetWidth(), y = CRT_baseImg->GetHeight();
        CRT = createEmptyBitmap(x, y);
        Gdiplus::Graphics graphics(CRT);
        graphics.DrawImage(CRT_baseImg, 0, 0, x, y);
        Frame::setBitmapAlpha(CRT, 20);
        CRT_brush = new Gdiplus::TextureBrush(CRT);

        // load player animations
        {
            std::vector<Gdiplus::TextureBrush*> front, back, left, right;
            float Ft = 1.0f, Bt = 1.0f, Lt = 1.0f, Rt = 1.0f;
            for (int i = 0; i < 4; i++) {
                std::wstring txt = L"images/Entities/Player/front/" + std::to_wstring(i) + L".png";
                Gdiplus::Image * img = Gdiplus::Image::FromFile(txt.c_str());
                Gdiplus::TextureBrush *brush = new Gdiplus::TextureBrush(img);
                brush->SetWrapMode(Gdiplus::WrapModeClamp);
                front.push_back(brush);
                // cleanup
                delete img;
            }
            for (int i = 0; i < 4; i++) {
                std::wstring txt = L"images/Entities/Player/back/" + std::to_wstring(i) + L".png";
                Gdiplus::Image * img = Gdiplus::Image::FromFile(txt.c_str());
                Gdiplus::TextureBrush *brush = new Gdiplus::TextureBrush(img);
                brush->SetWrapMode(Gdiplus::WrapModeClamp);
                back.push_back(brush);
                // cleanup
                delete img;
            }
            for (int i = 0; i < 4; i++) {
                std::wstring txt = L"images/Entities/Player/right/" + std::to_wstring(i) + L".png";
                Gdiplus::Image * img = Gdiplus::Image::FromFile(txt.c_str());
                Gdiplus::TextureBrush *brush = new Gdiplus::TextureBrush(img);
                brush->SetWrapMode(Gdiplus::WrapModeClamp);
                right.push_back(brush);
                // cleanup
                delete img;
            }
            for (int i = 0; i < 4; i++) {
                std::wstring txt = L"images/Entities/Player/left/" + std::to_wstring(i) + L".png";
                Gdiplus::Image * img = Gdiplus::Image::FromFile(txt.c_str());
                Gdiplus::TextureBrush *brush = new Gdiplus::TextureBrush(img);
                brush->SetWrapMode(Gdiplus::WrapModeClamp);
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
        Gdiplus::Bitmap *newBackground = createEmptyBitmap(bkgWidth, bkgHeight),
                        *newOverlay = createEmptyBitmap(bkgWidth, bkgHeight);

        if (newBackground->GetLastStatus() == Gdiplus::Ok) {
            delete background; // clean up the old background image
            background = newBackground;
        }
        if (newOverlay->GetLastStatus() == Gdiplus::Ok) {
            delete overlay; // clean up the old overlay image
            overlay = newOverlay;
        }
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


    void loadAllLevels()
    {
        mainBaseArea = Storage::LoadLevelObjectFromFile("saves/Main Base.txt");
        testLevel = Storage::LoadLevelObjectFromFile("saves/Test Level.txt");

        // set the level object's pointers, so you can walk from level to level
        mainBaseArea.below = &testLevel;
        testLevel.above = &mainBaseArea;

        mainBaseArea.setExitPos(Math::Zero2, {0.5f, 0.05f}, Math::Zero2, Math::Zero2);
        testLevel.setExitPos({0.166667f, 0.95f}, Math::Zero2, Math::Zero2, Math::Zero2);


        // search for the level that has a loaded player; this will be the starting scene
        Storage::Level *loadLevel = nullptr;
        if (mainBaseArea.player != nullptr) loadLevel = &mainBaseArea;
        else if (testLevel.player != nullptr) loadLevel = &testLevel;

        LoadSceneFromLevelObject(loadLevel);
    }

    void saveAllLevels()
    {
        // find the active level, and assign globals into that level object
        // i wish i could use a switch statement here, but strings cannot be used
        // in switch statements. else if chain it is :(
        if (currLevel == &mainBaseArea) SaveSceneToLevelObject(&mainBaseArea);
        else if (currLevel == &testLevel) SaveSceneToLevelObject(&testLevel);

        // save all levels into local storage
        Storage::SaveSceneToFile(mainBaseArea);
        Storage::SaveSceneToFile(testLevel);
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

                    // saplings grow at the beginning of each day
                    if (g_time == 0.0f && !isNightTime) {
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

        // do the same process for the level objects
        if (currLevel != &mainBaseArea) Game::updateTimedCells(&mainBaseArea);
        if (currLevel != &testLevel) Game::updateTimedCells(&testLevel);
    }


    // loads the attributes of a level object into the global variables
    void LoadSceneFromLevelObject(Storage::Level * level)
    {
        // load information about the player
        player = level->player; // load the player itself
        heldObject = level->heldObject; // load the object the player is holding
        buildingType = level->buildingType; // load what the player is building with
        if (heldObject != nullptr) Input::setHeldObject(heldObject);

        // load information about the rest of the scene
        gameObjects = level->gameObjects; // load all of the game objects

        // load information about the map
        sideLen = level->sideLen; // load the dimensions of each cell
        interactRange = interactRangeCells * sideLen; // set the interaction radius
        grid = level->grid; // load the array of all cells

        // update the global pointer letting the game know which level is loaded
        currLevel = level;


        // initialise the background/overlay images
        SetGrid();
        Frame::DrawWholeGrid();
    }
    
    // stores the global variables into a level object
    void SaveSceneToLevelObject(Storage::Level *level)
    {
        // save all the individual attributes
        level->player = player; level->heldObject = heldObject;
        level->buildingType = buildingType;
        level->gameObjects = gameObjects; level->sideLen = sideLen;
        level->grid = grid; 
        level->lvlName = currLevel->lvlName;
        level->timedCells = timedCells;
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


    // switches from the current scene to the specified one
    void switchScene(Storage::Level * level)
    {
        if (level->player == nullptr) {
            level->player = Object::Instantiate(Object::Player, player->pos, player->hp, &level->gameObjects);
            Object::Destroy(player); player = nullptr;
        }

        // find the active level, and assign globals into that level object
        // i wish i could use a switch statement here, but strings cannot be used
        // in switch statements. else if chain it is :(
        if (currLevel == &mainBaseArea) SaveSceneToLevelObject(&mainBaseArea);
        else if (currLevel == &testLevel) SaveSceneToLevelObject(&testLevel);
        
        // load the specified level into the global scene
        LoadSceneFromLevelObject(level);

        sceneSwitch = true;
    }
}