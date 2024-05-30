#ifndef BUTTONS_HPP
#define BUTTONS_HPP

#include <iostream>
#include <Windows.h>
#include <gdiplus.h>
#include <string>

namespace Buttons
{
    class Button {
    public:
        // position and dimensions of the button
        Gdiplus::Rect rect;

        // image the button uses
        Gdiplus::TextureBrush *brush = nullptr;
        // text places in the button
        std::wstring text;

        // whether or not the button can be pressed/shown
        bool isActive = true;

        // returns true if x and y are in the button
        bool pressed(int x, int y);

        // what happens when the button is pressed
        void (*leftClickFunc)(Button *);
        void (*rightClickFunc)(Button *);
        void (*hoverOverFunc)(Button *);

        // constructor
        Button(Gdiplus::Rect Rect, void (*LeftClickFunc)(Button *), 
            void (*RightClickFunc)(Button *), void (*HoverOverFunc)(Button *), 
            Gdiplus::TextureBrush *Brush = nullptr, std::wstring Text = L"");

        // draw the button onto the graphics object
        void show(Gdiplus::Graphics& graphics);
    };

    // does nothing when pressed
    void doNothing(Button *b);
}

#endif