#include "Inventory.hpp"

using namespace Globals;

namespace Inventory {
    HotbarSlot::HotbarSlot(Gdiplus::Rect Rect, int Type, int Count) : 
    rect(Rect), objType(Type), count(Count) {
        switch (Type)
        {
            case LOG:
                img = logImg; break;
            case BRIDGE:
                img = bridgeImg; break;
            case 0: 
                img = nullptr; break;
            default:
                std::cout << "unknown type\n"; break;
        }
    }

    void drawHotbar(Gdiplus::Graphics& graphics) {
        // place hotbar at the bottom centre of the window
        int width = hotbarImg->GetWidth() * currUIScale,
            height = hotbarImg->GetHeight() * currUIScale;

        int x = (wndWidth-width)/2, y = wndHeight-height;

        graphics.DrawImage(hotbarImg, Gdiplus::Rect(x, y, width, height));

        float slotMargin = 15.0f*currUIScale;
        // place item images in hotbar slots
        for (int i = 0; i < 5; i++) {
            HotbarSlot * button = hotbarButtons[i];
            Gdiplus::Rect r = {
                int(button->rect.X*currUIScale), 
                int(button->rect.Y*currUIScale),
                int(button->rect.Width*currUIScale), 
                int(button->rect.Height*currUIScale)};

            if (button->img==nullptr||!button->count) continue;

            if (button->img->GetLastStatus()==Gdiplus::Ok) {
                graphics.DrawImage(hotbarButtons[i]->img, Gdiplus::Rect(
                    x+r.X+slotMargin, y+r.Y+slotMargin,
                    r.Width - 2.0f*slotMargin, r.Height - 2.0f*slotMargin
                ));
            }

            int fontSize = 20*currUIScale;
            int disp = 0;
            for (int n = button->count; n; n /= 10) disp++;
            disp = disp*(fontSize/2) + 2.0f*slotMargin;

            graphics.DrawString(std::to_wstring(hotbarButtons[i]->count).c_str(), -1,
                    new Gdiplus::Font(L"Arial", fontSize, Gdiplus::FontStyleRegular), 
                    Gdiplus::PointF(x+r.X+r.Width-disp, 
                    y+r.Y+r.Height-fontSize/2-2.0f*slotMargin),
                    new Gdiplus::SolidBrush(Gdiplus::Color(255,255,255)));
        }
    }

    int buttonPress(int x, int y) {
        int width = hotbarImg->GetWidth() * currUIScale,
            height = hotbarImg->GetHeight() * currUIScale;

        int hotbarX = (wndWidth-width)/2, hotbarY = wndHeight-height;

        // if x,y is on the hotbar:
        int dHotbarX= x-hotbarX, dHotbarY = y-hotbarY;
        if (dHotbarX>=0&&dHotbarX<=width&&dHotbarY>=0&&dHotbarY<=height) {
            x -= hotbarX; y -= hotbarY;
            // hotbar buttons
            for (int i = 0; i < 6; i++) {
                // x,y needs to be in the right x and y range
                int dx = x-hotbarButtons[i]->rect.X*currUIScale, 
                    dy = y-hotbarButtons[i]->rect.Y*currUIScale;
                if (dx>=0&&dx<=hotbarButtons[i]->rect.Width*currUIScale &&
                    dy>=0&&dy<=hotbarButtons[i]->rect.Height*currUIScale)
                    // HOTBAR_i macros map directly to their value, just return i+1
                    return i+1;
            }
            return 7;
        }
        return 0;
    }
}