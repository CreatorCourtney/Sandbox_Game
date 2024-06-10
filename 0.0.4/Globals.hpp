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
#include "Storage.hpp"
#include "PlayerInput.hpp"
// had to include here to avoid compiler issues, NOT included in GameObjects.hpp
#include "EntityFunctions.hpp"
#include "Collisions.hpp"

namespace Globals {
    float g_scale = 1.0f;

    int wndWidth, wndHeight;
    int bkgWidth, bkgHeight;
    Gdiplus::Rect viewRect(0, 0, 1400, 788);
    HDC hOffscreenDC, g_hdc;
    HBITMAP hOffscreenBitmap;
    clock_t begin_time = clock();
    Gdiplus::Graphics * g_graphics; // graphics object for drawing

    float g_time = 0.0f; // the current time in game, saved as a float
    bool isNightTime = false; // whether or not it is currently night time

    float deltaTime = 0.0f;
    int prevFPS = 0;
    bool gameIsPaused = 0;

    int interactRangeCells = 3; // player can only interact with things within X cells
    float interactRange = 0.0f; // actual interaction distance

    // how frequently the sapling timer gets updated
    int saplingTime = 1;


    // the position of the player's mouse on the screen
    Math::Point2 mousePos(0, 0); 
    // relative position of the player's mouse in the world
    Math::Vector2 mousePosREAL(0.0f, 0.0f);

    unsigned char debuggingTools = 4; // . . . . . hitboxes, speed, showInfo
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
    Gdiplus::TextureBrush * bkgBrush;

    Gdiplus::Bitmap * overlay; // bitmap for the unmoving objects to be drawn above gameObjects
    Gdiplus::TextureBrush * overlayBrush;

    Gdiplus::Image *logImg, *bridgeImg, *waterImg, *emptyImg, *treeImg, *wolfImg,
    *falling_treeImg, *stumpImg, *Pine_ConeImg, *saplingImg, *shoreline0Img, *shoreline1Img,
    *shoreline2Img, *shoreline3Img, *shoreline4Img, *plankImg;
    Gdiplus::TextureBrush *logBrush, *bridgeBrush, *waterBrush, *grassBrush, *treeBrush,
    *wolfBrush, *falling_treeBrush, *stumpBrush, *Pine_ConeBrush, *saplingBrush, *shoreline0Brush,
    *shoreline1Brush, *shoreline2Brush, *shoreline3Brush, *shoreline4Brush, *plankBrush;

    Object::Animations playerAnimations;


    Object::GameObject * player;
    // object the player is holding
    Object::GameObject * heldObject = nullptr;
    // type of object player tries to put in a cell
    int buildingType = EMPTY;



    // all of the levels in the game
    Storage::Level *currLevel;
    Storage::Level mainBaseArea;
    Storage::Level testLevel;
    bool sceneSwitch = false;



    // a vector containing all cells with time dependent functions (such as saplings)
    std::vector<Math::Point2> timedCells;
    // a vector containing pointers to all game objects
    std::vector<Object::GameObject*> gameObjects;
    int numObjectsinOverlay = 0;


    float sideLen = 52.0f;
    std::vector<std::vector<int>> grid;

    int treeHeight = 10, treeWidth = 3; // dimensions of trees (in grid cells)

    // text colours
    Gdiplus::Color white(255,255,255), red(255,0,0), green(0,255,0), blue(0,0,255), black(0,0,0);
    // some more global brushes
    Gdiplus::SolidBrush *pauseBrush, *nightBrush;

    Gdiplus::Bitmap * createEmptyBitmap(int width, int height);
    int LoadImages();
    void SetGrid();
    void loadAllLevels();
    void saveAllLevels();


    // finds the index of a point in a vector of points,
    // returns -1 if the point is not in the vector
    int findPointIndexInVector(Math::Point2 p, std::vector<Math::Point2> vec);

    // updates all the cells with time elements (like sapling growth)
    void updateTimedCells();


    // for loading/unloading a scene
    // loads the attributes of a level object into the global variables
    void LoadSceneFromLevelObject(Storage::Level * level);
    // stores the global variables into a level object
    void SaveSceneToLevelObject(Storage::Level *level);

    // loads the user's settings into global variables
    void LoadUserSettingsToObject(Storage::UserSettings settings);
    // saves settings globals into a UserSettings object
    Storage::UserSettings saveUserSettingsToObject();

    // switches from the current scene to the specified one
    void switchScene(Storage::Level * level);
}

#endif