#include "GameObjects.hpp"

using namespace Globals;

namespace Object
{
    GameObject::GameObject(Math::Vector2 Pos, Math::Vector2 Velocity,
    int Type, float Scale, float Speed)
    : pos(Pos), velocity(Velocity), type(Type), scale(Scale), speed(Speed) {
        switch (type)
        {
            case PLAYER: img = playerImg; break;
            case LOG: img = logImg; break;
            default: std::cout << "unknown entity type\n"; return;
        }
        size = Math::Point2((float)img->GetWidth()*scale, (float)img->GetHeight()*scale);
    }

    void GameObject::draw(Gdiplus::Graphics& graphics)
    {
        if (img->GetLastStatus() == Gdiplus::Ok) {
            int x = pos.x, y = pos.y;

            switch (bkgState&3) { // first two bits
                case 0: // centre
                    if (type == PLAYER) x = wndWidth/2;
                    else x += (wndWidth/2)-player->pos.x;
                    break;
                case 1: // left
                    if (bkgWidth<wndWidth) x += (wndWidth-bkgWidth)/2;
                    break;
                case 2: // right
                    x -= bkgWidth-wndWidth;
                    break;
            }
            if (x<-size.x||x>wndWidth) return; // offscreen, dont render

            switch (bkgState&12) { // bits 3 and 4
                case 0: // centre
                    if (type == PLAYER) y = wndHeight/2;
                    else y += (wndHeight/2)-player->pos.y;
                    break;
                case 4: // top
                    if (bkgHeight<wndHeight) y += (wndHeight-bkgHeight)/2;
                    break;
                case 8: // bottom
                    y -= bkgHeight-wndHeight;
                    break;
            }
            if (y<-size.y||y>wndHeight) return; // offscreen, dont render

            graphics.DrawImage(img, x, y, size.x, size.y);
        }
    }

    void GameObject::update() {
        if (gameIsPaused) return;
        updateVelocity();
        updatePosition();
        handleCollisions();
        if (this == player) updateBkg();
    }

    void GameObject::updateVelocity() {
        switch (type)
        {
            case PLAYER: {
                float s = speed + speed*bool(inputKeys&16);
                velocity = Math::Vector2(
                    s*(bool(inputKeys&1)-bool(inputKeys&4)),
                    s*(bool(inputKeys&2)-bool(inputKeys&8))
                ); break;
            }
            case LOG: break;

            default: std::cout << "unknown entity\n"; break;
        }
    }

    void GameObject::updatePosition() {
        pos = pos + (velocity * deltaTime);
    }

    void GameObject::handleCollisions() {
        // world borders
        if (pos.x<0.0f) pos.x = 0.0f;
        else if (pos.x>bkgWidth-size.x) pos.x = bkgWidth-size.x-1.0f;
        if (pos.y<0.0f) pos.y = 0.0f;
        else if (pos.y>bkgHeight-size.y) pos.y = bkgHeight-size.y-1.0f;
        
        // walls
        Math::Vector2 disp = Math::Zero2;
        Math::Point2 p = findCell(pos); // top left
        if (grid[p.x][p.y]&BARRIER) 
            disp = ((Math::Vector2(p.x,p.y)+Math::One2)*sideLen)-pos;
        if (Math::absf(disp.x)<Math::absf(disp.y)) pos.x += disp.x;
        else pos.y += disp.y;
  
        p = findCell(pos+Math::Vector2(size.x,0.0f)); // top right
        disp = Math::Zero2;
        if (grid[p.x][p.y]&BARRIER)
            disp =  (Math::Vector2(p.x,p.y+1.0f)*sideLen)-
                    (pos+Math::Vector2(size.x,0.0f));
        if (Math::absf(disp.x)<Math::absf(disp.y)) pos.x += disp.x;
        else pos.y += disp.y;
 
        p = findCell(pos+Math::Vector2(0.0f,size.y)); // bottom left
        disp = Math::Zero2;
        if (grid[p.x][p.y]&BARRIER)
            disp =  (Math::Vector2(p.x+1.0f,p.y)*sideLen)-
                    (pos+Math::Vector2(0.0f,size.y));
        if (Math::absf(disp.x)<Math::absf(disp.y)) pos.x += disp.x;
        else pos.y += disp.y;
        
        p = findCell(pos+Math::Vector2(size.x, size.y)); // bottom right
        disp = Math::Zero2;
        if (grid[p.x][p.y]&BARRIER)
            disp =  (Math::Vector2(p.x,p.y)*sideLen)-
                    (pos+Math::Vector2(size.x,size.y));
        if (Math::absf(disp.x)<Math::absf(disp.y)) pos.x += disp.x;
        else pos.y += disp.y;
    }

    void GameObject::updateBkg() {
        // 1st bit = <wndwidth/2||width<wndWidth
        int left = bkgWidth<wndWidth || pos.x<wndWidth/2;
        // 2nd bit = >bkgWidth-wndWidth/2
        int right = (left)? 0 : (pos.x>bkgWidth-wndWidth/2)<<1;
        // 3rd bit = <wndHeight/2 || height<wndHeight
        int top = (bkgHeight<wndHeight || pos.y<wndHeight/2)<<2;
        // 4th bit = >bkgHeight-wndHeight/2
        int bottom = (top)? 0 : (pos.y>bkgHeight-wndHeight/2)<<3;

        bkgState = bottom|top|right|left;
    }

    Math::Point2 findCell(Math::Vector2 pos) {
        int x = int(pos.x/sideLen), y = int(pos.y/sideLen);
        if (x>=grid.size()) x = grid.size()-1;
        if (y>=grid[0].size()) y = grid[0].size()-1;
        return Math::Point2(x, y);
    }

    Math::Vector2 getWorldPosition(Math::Point2 wndPos)
    {
        float x = wndPos.x, y = wndPos.y;

        switch (bkgState&3) { // first two bits
            case 0: // centre
                x += player->pos.x-(wndWidth/2);
                break;
            case 1: // left
                if (bkgWidth<wndWidth) x -= (wndWidth-bkgWidth)/2;
                break;
            case 2: // right
                x += bkgWidth-wndWidth;
                break;
        }

        switch (bkgState&12) { // bits 3 and 4
            case 0: // centre
                y += player->pos.y-(wndHeight/2);
                break;
            case 4: // top
                if (bkgHeight<wndHeight) y -= (wndHeight-bkgHeight)/2;
                break;
            case 8: // bottom
                y += bkgHeight-wndHeight;
                break;
        }
        return Math::Vector2(x, y);
    }

    Math::Point2 getScreenPosition(Math::Vector2 worldPos)
    {
        int x = worldPos.x, y = worldPos.y;

        switch (bkgState&3) { // first two bits
            case 0: // centre
                x += (wndWidth/2)-player->pos.x;
                break;
            case 1: // left
                if (bkgWidth<wndWidth) x += (wndWidth-bkgWidth)/2;
                break;
            case 2: // right
                x += wndWidth-bkgWidth;
                break;
        }

        switch (bkgState&12) { // bits 3 and 4
            case 0: // centre
                y += (wndHeight/2)-player->pos.y;
                break;
            case 4: // top
                if (bkgHeight<wndHeight) y += (wndHeight-bkgHeight)/2;
                break;
            case 8: // bottom
                y += wndHeight-bkgHeight;
                break;
        }
        return Math::Point2(x, y);
    }
}