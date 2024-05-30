#include "Buttons.hpp"

namespace Buttons
{
    // returns true if x and y are in the button
    bool Button::pressed(int x, int y)
    {
        int right = rect.X+rect.Width, bottom = rect.Y+rect.Height;
        // check if the coordinates are within bounds of the button, AND the button is active
        return x>=rect.X && x<=right && y>=rect.Y && y<=bottom && isActive;
    }

    // button constructor
    Button::Button(Gdiplus::Rect Rect, void (*leftClickFunc)(Button *), 
        void (*RightClickFunc)(Button *), void (*HoverOverFunc)(Button *),
        Gdiplus::TextureBrush *Brush, std::wstring Text) : 
        rect(Rect), brush(Brush), text(Text), 
        leftClickFunc(leftClickFunc), rightClickFunc(RightClickFunc),
        hoverOverFunc(HoverOverFunc) 
        {
            // set the transform of the texture brush
            if (brush != nullptr)
                brush->TranslateTransform(rect.X, rect.Y);
        }


    // draw the button onto the graphics object
    void Button::show(Gdiplus::Graphics& graphics)
    {
        // don't show an inactive button
        if (isActive) {
            // draw the texture brush
            if (brush != nullptr) {
                graphics.FillRectangle(brush, rect);
            }

            // draw the text, centred on the button
            int x = rect.X+(rect.Width/2)-(6*text.size()), y = rect.Y+(rect.Height/2)-9;
            Gdiplus::Font *font = new Gdiplus::Font(L"Arial", 12);
            Gdiplus::SolidBrush *txtColour = new Gdiplus::SolidBrush(Gdiplus::Color(255,255,255));
            
            graphics.DrawString(text.c_str(), -1, font, Gdiplus::PointF(x,y), txtColour);
            // cleanup
            delete font; delete txtColour;
        }
    }



    // does nothing when pressed
    void doNothing(Button *b) {}
}