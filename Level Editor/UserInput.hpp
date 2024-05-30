#ifndef USER_INPUT_HPP
#define USER_INPUT_HPP

#include <iostream>
#include <Windows.h>
#include <windowsx.h>
#include <WinUser.h>
#include <gdiplus.h>

#include "GameMath.hpp"
#include "Globals.hpp"
#include "Storage.hpp"

namespace Input
{
    // place an object into a cell in the global grid
    int PlaceObjectInCell(Math::Point2 cell, int objType, bool updateBkgBrush);



    // what happens when the user presses down a key
    void keyPressFunc(WPARAM wParam, HWND hwnd);
    // what happens when the user releases a key
    void keyReleaseFunc(WPARAM wParam);



    // function that gets called when the user left clicks
    void leftClickFunc(int x, int y);
    // function that gets called when the user right clicks
    void rightClickFunc(int x, int y);

    // determines if the point, p, is within the rect region
    bool isInRegion(Math::Vector2 p, Gdiplus::Rect rect);


    // button functions

    // places the buiding type you have held into the map
    void placeCellOnMapButtonFunc(Buttons::Button *b);
    // creates an entity on the map where the player clicks
    void placeEntityOnMapButtonFunc(Buttons::Button *b);
    // removes any entity that you click on
    void removeEntityFromMapButtonFunc(Buttons::Button *b);

    // button functions for setting the building type
    void makeBuildingTypeLog(Buttons::Button *b);
    void makeBuildingTypeBridge(Buttons::Button *b);
    void makeBuildingTypeWater(Buttons::Button *b);
    void makeBuildingTypeSapling(Buttons::Button *b);
    void makeBuildingTypeTree(Buttons::Button *b);
    void makeBuildingTypeStump(Buttons::Button *b);

    // button functions for placing entity types
    void makeBuildingTypePlayer(Buttons::Button *b);
    void makeBuildingTypeWolf(Buttons::Button *b);
    void makeBuildingTypeLog_Item(Buttons::Button *b);
    void makeBuildingTypePine_Cone_Item(Buttons::Button *b);


    // saving the level
    void saveLevelToFileButtonFunc(Buttons::Button *b);
}

#endif