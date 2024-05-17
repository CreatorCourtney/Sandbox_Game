#ifndef PLAYER_INPUT_HPP
#define PLAYER_INPUT_HPP

#include "GameMath.hpp"
#include "FrameCreation.hpp"

namespace Input 
{
    // place an object into a cell in the global grid
    void PlaceObjectInCell(Math::Point2 cell, int objType, bool updateBkgBrush);
    void PlaceObjectInCell(Math::Point2 cell, int objType); // for convenience

    // determines if the point, p, is within the rect region
    bool isInRegion(Math::Point2 p, Gdiplus::Rect rect);
}

#endif