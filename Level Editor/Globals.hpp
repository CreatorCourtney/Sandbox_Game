#ifndef GLOBALS_HPP
#define GLOBALS_HPP

#include <Windows.h>
#include <windowsx.h>
#include <WinUser.h>
#include <gdiplus.h>
#include <ctime>
#include <vector>

#include "GameMath.hpp"
#include "Buttons.hpp"
#include "FrameCreation.hpp"
#include "UserInput.hpp"
#include "Storage.hpp"

// cell type macros
#define EMPTY           0b00000000000000000000000000000000
#define BARRIER         0b00000000000000000010000000000000
#define OCCUPIED        0b00000000000000000100000000000000
#define WATER           0b00000000000000000001000000000000
#define INDESTRUBTIBLE  0b00000000000000001000000000000000
#define CAN_DIE         0b00000001000000000000000000000000
#define HAS_TIMER       0b00000010000000000000000000000000

// macros to easily access certain regions of data
#define TIMER           0b11111100000000000000000000000000
#define HEALTH          0b00000000000000000000111100000000
#define SHORELINE       0b00000000111111110000000000000000

#define LOG             0b00000001000000000110100000000001 // 8 health
#define BRIDGE          0b00000001000000000100001100000010 // 3 health
#define TREE            0b00000000000000000110000000000011 // not destructible
#define STUMP           0b00000001000000000110100000000100 // 8 health
#define SAPLING         0b00000011000000000110001000000101 // 2 health

namespace Globals
{
    // images and brushes for drawing the map onto the screen
    Gdiplus::Bitmap *mapBackground, *mapOverlay;
    Gdiplus::TextureBrush *mapBkgBrush, *mapOverlayBrush;


    // images of objects and entities in the game
    Gdiplus::Image *logImg, *bridgeImg, *waterImg, *emptyImg, *hotbarImg, *treeImg, *wolfImg,
    *falling_treeImg, *stumpImg, *Pine_ConeImg, *saplingImg, *shoreline0Img, *shoreline1Img,
    *shoreline2Img, *shoreline3Img, *shoreline4Img, *playerImg, *ObjectsButtonImg,
    *EntitiesButtonImg;
    Gdiplus::TextureBrush *logBrush, *bridgeBrush, *waterBrush, *grassBrush, *hotbarBrush, *treeBrush,
    *wolfBrush, *falling_treeBrush, *stumpBrush, *Pine_ConeBrush, *saplingBrush, *shoreline0Brush,
    *shoreline1Brush, *shoreline2Brush, *shoreline3Brush, *shoreline4Brush, *playerBrush,
    *ObjectsButtonBrush, *EntitiesButtonBrush;


    // the type of object the player tries to place
    int placementType = EMPTY;



    // current dimensions of the window
    int wndWidth, wndHeight;

    // desired dimensions of the window
    Gdiplus::Rect wndRect(0, 0, 1742, 980);

    // the placement and dimensions of the map in the window
    Gdiplus::Rect mapRect;
    int sideLen = 0, 
        prevSideLen = 0, // side length of one cell of the map
        savedSideLen = 52; // side length of the cell in the actual game


    // location of the mouse in the window
    int mPosX = 0, mPosY = 0;


    // if the user is in the objects panel (false when in the entities panel)
    bool isInObjectsPanel = true;
    int selectedHP = 20; // hp of entities the user creates

    // the file name the level will be saved as
    std::string levelFileName = "Default Level.txt";




    // device contexts for the offscreen buffer frame, and the current window
    HDC hOffscreenDC, g_hdc;
    // bitmap the buffer frame is drawn onto
    HBITMAP hOffscreenBitmap;

    // for measuring the time passed between frames
    clock_t begin_time = clock(); // time at the beggining of frame creation
    float deltaTime = 0.0f; // the amount of time (in seconds) passed between frames
    int g_fps = 0; // the app's framerate

    // graphics constants
    int lineThickness = 2; // how thick divider lines are
    int topMenuHeight = 40; // height of the top menu bar

    // gdi+ solid brushes/pens/colours
    Gdiplus::Color white(255,255,255), background(54,54,54), accent0(94,94,94), accent1(34,34,34);
    Gdiplus::SolidBrush *bkgBrush, *accent0Brush, *accent1Brush;


    // for editing the map's 2d array, grid
    int gridSizeX = 20, gridSizeY = 20; // grid dimensions
    std::vector<std::vector<int>> grid; // 2d int array


    // vector of all entities
    std::vector<Storage::Entity*> entities;
    Storage::Entity *player; // reference to the player entity


    // vector of buttons
    std::vector<Buttons::Button*> buttons;
    Buttons::Button *mapButton;
    std::vector<Buttons::Button*> objectButtons;
    std::vector<Buttons::Button*> entityButtons;


    // dimensions of the tree image (in cells)
    int treeWidth = 3, treeHeight = 10;


    // fucntions

    // loads all images from file
    int loadImages();

    // initialises the map, to be edited
    void initialiseMap();


    
    // resizes the grid vector
    void resizeGrid();

    // returns a pointer to an empty bitmap
    Gdiplus::Bitmap * createEmptyBitmap(int width, int height);

    // decreases the x size of the grid by 1
    void decrementGridXSize(Buttons::Button *b);
    // decreases the y size of the grid by 1
    void decrementGridYSize(Buttons::Button *b);
    // increases the x size of the grid by 1
    void incrementGridXSize(Buttons::Button *b);
    // increases the y size of the grid by 1
    void incrementGridYSize(Buttons::Button *b);

    // enters the building selection panel
    void enterObjectsPanel(Buttons::Button *b);
    // enters the entity selection panel
    void enterEntitiesPanel(Buttons::Button *b);


    // creates all of the button objects
    void createAllButtons();
}

#endif