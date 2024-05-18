#ifndef PLAYER_INPUT_HPP
#define PLAYER_INPUT_HPP

#include "GameMath.hpp"
#include "FrameCreation.hpp"

namespace Input 
{
    // place an object into a cell in the global grid
    int PlaceObjectInCell(Math::Point2 cell, int objType, bool updateBkgBrush);
    int PlaceObjectInCell(Math::Point2 cell, int objType); // for convenience

    // determines if the point, p, is within the rect region
    bool isInRegion(Math::Vector2 p, Gdiplus::Rect rect);


    // if the player is holding an object, throw it, otherwise try to pick one up
    void leftClickFunc(int x, int y);
    // if the player IS holding an item, attempt to place it, otherwise, do nothing (for now c:)
    void rightClickFunc(int x, int y);
}

#endif