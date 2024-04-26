#ifndef INVENTORY_HPP
#define INVENTORY_HPP

#include <string>
#include "Globals.hpp"

// button press types
#define HOTBAR_1 1
#define HOTBAR_2 2
#define HOTBAR_3 3
#define HOTBAR_4 4
#define HOTBAR_5 5
#define INVENTORY_BUTTON 6

namespace Inventory {
    class HotbarSlot {
    public:
        Gdiplus::Rect rect; // position within hotbar and dimensions
        Gdiplus::Image * img; // texture for drawing
        int objType; // type of object, when selected
        int count; // amount of the item
        // constructors
        HotbarSlot(Gdiplus::Rect Rect, int Type, int Count);
    };

    void drawHotbar(Gdiplus::Graphics& graphics);
    // determine which button, if any, is pressed given an x, y window coordinate
    int buttonPress(int x, int y);
}

#endif