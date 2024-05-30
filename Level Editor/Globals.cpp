#include "Globals.hpp"

namespace Globals
{
    // loads all images from file
    int loadImages()
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

        playerImg = Gdiplus::Image::FromFile(L"images/Player.png");
        if (playerImg->GetLastStatus() != Gdiplus::Ok) return 2;
        playerBrush = new Gdiplus::TextureBrush(playerImg);
        playerBrush->SetWrapMode(Gdiplus::WrapModeClamp);

        ObjectsButtonImg = Gdiplus::Image::FromFile(L"images/Buttons/ObjectsButton.png");
        if (ObjectsButtonImg->GetLastStatus() != Gdiplus::Ok) return 2;
        ObjectsButtonBrush = new Gdiplus::TextureBrush(ObjectsButtonImg);
        ObjectsButtonBrush->SetWrapMode(Gdiplus::WrapModeClamp);

        EntitiesButtonImg = Gdiplus::Image::FromFile(L"images/Buttons/EntitiesButton.png");
        if (EntitiesButtonImg->GetLastStatus() != Gdiplus::Ok) return 2;
        EntitiesButtonBrush = new Gdiplus::TextureBrush(EntitiesButtonImg);
        EntitiesButtonBrush->SetWrapMode(Gdiplus::WrapModeClamp);

        return 0;
    }


    // initialises the map, to be edited
    void initialiseMap()
    {
        // set the grid to be of the right dimensions
        resizeGrid();

        // create empty bitmap objects for the background and overlay
        mapBackground = createEmptyBitmap(mapRect.Width, mapRect.Height);
        mapOverlay = createEmptyBitmap(mapRect.Width, mapRect.Height);
    }



    // resizes the grid vector
    void resizeGrid()
    {
        // the map must be at least 1x1 cells
        gridSizeX = Math::Max(gridSizeX, 1); gridSizeY = Math::Max(gridSizeY, 1);

        // resize the x dimensions of the grid

        while (grid.size() > gridSizeX)
        {
            // remove the last element of the vector until it is the right size
            grid.erase(grid.end()-1);
        }

        while (grid.size() < gridSizeX)
        {
            // add rows until the vector is the right size
            std::vector<int> row(gridSizeY);
            grid.push_back(row);
        }

        // resize the y dimensions of the grid
        for (int i = 0; i < gridSizeX; i++)
        {
            while (grid[i].size() > gridSizeY)
            {
                // remove the last element of the vector until it is the right size
                grid[i].erase(grid[i].end()-1);
            }

            while (grid[i].size() < gridSizeY)
            {
                // add entries until the vector is the right size
                grid[i].push_back(0);
            }
        }

        // update sideLen
        sideLen = Math::Min(mapRect.Width/gridSizeX, mapRect.Height/gridSizeY);

        // redraw the whole grid to account for the change in dimensions
        Frame::DrawWholeGrid();

        // update the size of all entities
        int n = entities.size();
        for (int i = 0; i < n; i++) {
            entities[i]->resize(sideLen, prevSideLen);
        }

        // update the positioning of objects in overlay, by removing a tree from an oob cell
        Frame::RemoveTreeFromOverlay(Math::Point2(-1, -1));

        // update the previous side length
        prevSideLen = sideLen;
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



    // decreases the x size of the grid by 1
    void decrementGridXSize(Buttons::Button *b) {
        if (b->isActive) {
            gridSizeX--;
            if (gridSizeX < 1) gridSizeX++;
            // commit the size change to grid
            resizeGrid();
        }
    }
    // decreases the y size of the grid by 1
    void decrementGridYSize(Buttons::Button *b) {
        if (b->isActive) {
            gridSizeY--;
            if (gridSizeY < 1) gridSizeY++;
            // commit the size change to grid
            resizeGrid();
        }
    }
    // increases the x size of the grid by 1
    void incrementGridXSize(Buttons::Button *b) {
        if (b->isActive) {
            gridSizeX++;
            // commit the size change to grid
            resizeGrid();
        }
    }
    // increases the y size of the grid by 1
    void incrementGridYSize(Buttons::Button *b) {
        if (b->isActive) {
            gridSizeY++;
            // commit the size change to grid
            resizeGrid();
        }
    }

    // enters the building selection panel
    void enterObjectsPanel(Buttons::Button *b) {
        if (b->isActive) {
            isInObjectsPanel = true;
            // disable all the entity buttons
            int n = entityButtons.size();
            for (int i = 0; i < n; i++) {
                entityButtons[i]->isActive = false;
            }
            // enable all the object buttons
            n = objectButtons.size();
            for (int i = 0; i < n; i++) {
                objectButtons[i]->isActive = true;
            }
        }
    }
    // enters the entity selection panel
    void enterEntitiesPanel(Buttons::Button *b) {
        if (b->isActive) {
            isInObjectsPanel = false;
            // disable all the object buttons
            int n = objectButtons.size();
            for (int i = 0; i < n; i++) {
                objectButtons[i]->isActive = false;
            }
            // enable all the entity buttons
            n = entityButtons.size();
            for (int i = 0; i < n; i++) {
                entityButtons[i]->isActive = true;
            }
        }
    }




    // creates all of the button objects
    void createAllButtons()
    {
        int d = (5*wndWidth/6), dWidth = wndWidth-d+10, step = dWidth/8;
        int x = d+10, y = topMenuHeight+lineThickness+54;
        Gdiplus::Rect rect(x, y, 24, 24);
        buttons.push_back(new Buttons::Button(
            rect, decrementGridXSize, Buttons::doNothing, Buttons::doNothing, nullptr, L"-"));

        rect = {x-24+3*step, rect.Y, rect.Width, rect.Height};
        buttons.push_back(new Buttons::Button(
            rect, incrementGridXSize, Buttons::doNothing, Buttons::doNothing, nullptr, L"+"));

        rect = {x+4*step, rect.Y, rect.Width, rect.Height};
        buttons.push_back(new Buttons::Button(
            rect, decrementGridYSize, Buttons::doNothing, Buttons::doNothing, nullptr, L"-"));

        rect = {x-24+7*step, rect.Y, rect.Width, rect.Height};
        buttons.push_back(new Buttons::Button(
            rect, incrementGridYSize, Buttons::doNothing, Buttons::doNothing, nullptr, L"+"));

        
        // main map
        mapButton = new Buttons::Button(
            mapRect, Buttons::doNothing, Buttons::doNothing, Buttons::doNothing, nullptr, L"");
        buttons.push_back(mapButton);



        rect = {x+35, topMenuHeight+138, 50, 50};
        Buttons::Button *button = new Buttons::Button(
            rect, enterObjectsPanel, Buttons::doNothing, Buttons::doNothing,
            ObjectsButtonBrush, L"");
        buttons.push_back(button);


        rect = {rect.X+50, rect.Y, 50, 50};
        buttons.push_back(new Buttons::Button(
            rect, enterEntitiesPanel, Buttons::doNothing, Buttons::doNothing, 
            EntitiesButtonBrush, L""));


        // object types
        rect = {x+55, rect.Y+100, 50, 50};
        button = new Buttons::Button(
            rect, Input::makeBuildingTypeLog, Buttons::doNothing, Buttons::doNothing,
            new Gdiplus::TextureBrush(logImg), L"");
        buttons.push_back(button);
        objectButtons.push_back(button);

        rect = {rect.X+80, rect.Y, 50, 50};
        button = new Buttons::Button(
            rect, Input::makeBuildingTypeBridge, Buttons::doNothing, Buttons::doNothing,
            bridgeBrush, L"");
        buttons.push_back(button);
        objectButtons.push_back(button);

        rect = {rect.X-80, rect.Y+100, 50, 50};
        button = new Buttons::Button(
            rect, Input::makeBuildingTypeWater, Buttons::doNothing, Buttons::doNothing,
            waterBrush, L"");
        buttons.push_back(button);
        objectButtons.push_back(button);

        rect = {rect.X+80, rect.Y, 50, 50};
        button = new Buttons::Button(
            rect, Input::makeBuildingTypeSapling, Buttons::doNothing, Buttons::doNothing,
            saplingBrush, L"");
        // brush too small :(
        Gdiplus::Matrix *matrix = new Gdiplus::Matrix(2.0f, 0.0f, 0.0f, 2.0f, 0.0f, 0.0f);
        button->brush->MultiplyTransform(matrix);
        delete matrix; // cleanup
        buttons.push_back(button);
        objectButtons.push_back(button);


        rect = {rect.X-80, rect.Y+100, 50, 50};
        button = new Buttons::Button(
            rect, Input::makeBuildingTypeTree, Buttons::doNothing, Buttons::doNothing,
            treeBrush, L"");
        // brush too big :(
        matrix = new Gdiplus::Matrix(0.666667f, 0.0f, 0.0f, 0.2f, 0.0f, 0.0f);
        button->brush->MultiplyTransform(matrix);
        delete matrix; // cleanup
        buttons.push_back(button);
        objectButtons.push_back(button);


        rect = {rect.X+80, rect.Y, 50, 50};
        button = new Buttons::Button(
            rect, Input::makeBuildingTypeStump, Buttons::doNothing, Buttons::doNothing,
            stumpBrush, L"");
        // brush too small :(
        matrix = new Gdiplus::Matrix(2.0f, 0.0f, 0.0f, 2.0f, 0.0f, 0.0f);
        button->brush->MultiplyTransform(matrix);
        delete matrix; // cleanup
        buttons.push_back(button);
        objectButtons.push_back(button);


        // entity buttons
        rect = {x+55, topMenuHeight+238, 50, 50};
        button = new Buttons::Button(
            rect, Input::makeBuildingTypePlayer, Buttons::doNothing, Buttons::doNothing, 
            new Gdiplus::TextureBrush(playerImg), L"");
        // brush too small :(
        matrix = new Gdiplus::Matrix(2.0f, 0.0f, 0.0f, 2.0f, 0.0f, 0.0f);
        button->brush->MultiplyTransform(matrix);
        delete matrix; // cleanup
        buttons.push_back(button);
        entityButtons.push_back(button);
        button->isActive = false;

        rect = {rect.X+80, rect.Y, 50, 50};
        button = new Buttons::Button(
            rect, Input::makeBuildingTypeWolf, Buttons::doNothing, Buttons::doNothing, 
            new Gdiplus::TextureBrush(wolfImg), L"");
        buttons.push_back(button);
        entityButtons.push_back(button);
        button->isActive = false;

        rect = {rect.X-80, rect.Y+100, 50, 50};
        button = new Buttons::Button(
            rect, Input::makeBuildingTypePine_Cone_Item, Buttons::doNothing, Buttons::doNothing, 
            new Gdiplus::TextureBrush(Pine_ConeImg), L"");
        // brush too small :(
        matrix = new Gdiplus::Matrix(2.0f, 0.0f, 0.0f, 2.0f, 0.0f, 0.0f);
        button->brush->MultiplyTransform(matrix);
        delete matrix; // cleanup
        buttons.push_back(button);
        entityButtons.push_back(button);
        button->isActive = false;

        rect = {rect.X+80, rect.Y, 50, 50};
        button = new Buttons::Button(
            rect, Input::makeBuildingTypeLog_Item, Buttons::doNothing, Buttons::doNothing, 
            new Gdiplus::TextureBrush(logImg), L"");
        buttons.push_back(button);
        entityButtons.push_back(button);
        button->isActive = false;

        // save button
        rect = {0, 0, 100, topMenuHeight};
        button = new Buttons::Button(
            rect, Input::saveLevelToFileButtonFunc, Buttons::doNothing, Buttons::doNothing,
            nullptr, L"Save Map");
        buttons.push_back(button);
    }
}