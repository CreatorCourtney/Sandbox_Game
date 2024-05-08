#include "GameObjects.hpp"

using namespace Globals;

namespace Object
{
    Animations::Animations(std::vector<Gdiplus::TextureBrush*> Front,
            std::vector<Gdiplus::TextureBrush*> Back,
            std::vector<Gdiplus::TextureBrush*> Left,
            std::vector<Gdiplus::TextureBrush*> Right,
            float FrontT, float BackT, float LeftT, float RightT) :
    front(Front), back(Back), left(Left), right(Right) {
        frontInter = float(Front.size()-1) / FrontT;
        backInter = float(Back.size()-1) / BackT;
        leftInter = float(Left.size()-1) / LeftT;
        rightInter = float(Right.size()-1) / RightT;
    }

    Animations::Animations() {}

    GameObject::GameObject(Math::Vector2 Pos, Math::Vector2 Velocity,
    EntityType Type, Math::Point2 Size, float Speed, int Hp)
    : pos(Pos), velocity(Velocity), type(Type), size(Size), speed(Speed), hp(Hp) {
        centrePos = pos + Math::Vector2(size.x/2, size.y/2);

        switch (type)
        {
            case Player:
                animations = playerAnimations;
                hasCollision = true;
                srcDimensions = {25, 25};

                cellularDimensions = {1, 1, size.x, size.y};
                break;

            case Wolf:
                hasAnimation = false;
                hasCollision = true;
                brush = wolfBrush;
                srcDimensions = {50, 50};
                break;
        }
        float   scaleX = (float)size.x/(float)srcDimensions.x, 
                scaleY = (float)size.y/(float)srcDimensions.y;
        brushMultMatrix = new Gdiplus::Matrix(scaleX, 0.0f, 0.0f, scaleY, 0.0f, 0.0f);

        if (Type != Player) {
            int numX = size.x/sideLen+1, numY = size.y/sideLen+1,
                stepX = (float)size.x/(float)numX, stepY = (float)size.y/(float)numY;

            cellularDimensions = {numX, numY, stepX, stepY};
        }

        // preemptively set the brushes so that they don't wrap, thus drawing the image only once
        if (hasAnimation) {
            for (int i = 0; i < animations.front.size(); i++) {
                animations.front[i]->SetWrapMode(Gdiplus::WrapModeClamp);
            }
            for (int i = 0; i < animations.back.size(); i++) {
                animations.back[i]->SetWrapMode(Gdiplus::WrapModeClamp);
            }
            for (int i = 0; i < animations.left.size(); i++) {
                animations.left[i]->SetWrapMode(Gdiplus::WrapModeClamp);
            }
            for (int i = 0; i < animations.right.size(); i++) {
                animations.right[i]->SetWrapMode(Gdiplus::WrapModeClamp);
            }
        } else {
            brush->SetWrapMode(Gdiplus::WrapModeClamp);
        }
    }

    void GameObject::draw(Gdiplus::Graphics& graphics)
    {
        Math::Point2 screenPos = getScreenPosition(pos);
        // offscreen, dont render
        if (screenPos.x<-size.x || screenPos.x>wndWidth ||
            screenPos.y<-size.y || screenPos.y>wndHeight) return;

        Gdiplus::TextureBrush * imgBrush = (hasAnimation)? chooseAnimationStage() : brush;
        Gdiplus::Matrix matrix(1.0f, 0.0f, 0.0f, 1.0f, screenPos.x, screenPos.y);
        imgBrush->SetTransform(&matrix);
        imgBrush->MultiplyTransform(brushMultMatrix);
        graphics.FillRectangle(imgBrush, screenPos.x, screenPos.y, size.x, size.y);

        // debugging tools
        if (showDebugInfo && showHitboxes) {
            int dotSize = 6;
            Math::Point2 centreScreenPos = Math::Point2(screenPos.x+size.x/2, screenPos.y+size.y/2);
            Gdiplus::SolidBrush redBrush(red), blueBrush(blue);

            graphics.DrawRectangle(new Gdiplus::Pen(blue), screenPos.x, screenPos.y, size.x, size.y);

            // top side
            for (int i = 1; i < cellularDimensions.X; i++) {
                int x = screenPos.x+cellularDimensions.Width*i-dotSize/2,
                    y = screenPos.y-dotSize/2;
                graphics.FillEllipse(&redBrush, x, y, dotSize, dotSize);
            }
            // bottom side
            for (int i = 0; i <= cellularDimensions.X; i++) {
                int x = screenPos.x+cellularDimensions.Width*i-dotSize/2,
                    y = screenPos.y+size.y-dotSize/2;
                graphics.FillEllipse(&redBrush, x, y, dotSize, dotSize);
            }
            // left side
            for (int i = 0; i < cellularDimensions.Y; i++) {
                int y = screenPos.y+cellularDimensions.Height*i-dotSize/2,
                    x = screenPos.x-dotSize/2;
                graphics.FillEllipse(&redBrush, x, y, dotSize, dotSize);
            }
            // right side
            for (int i = 0; i < cellularDimensions.Y; i++) {
                int y = screenPos.y+cellularDimensions.Height*i-dotSize/2,
                    x = screenPos.x+size.x-dotSize/2;
                graphics.FillEllipse(&redBrush, x, y, dotSize, dotSize);
            }
            graphics.FillEllipse(&blueBrush, centreScreenPos.x-dotSize/2, centreScreenPos.y-dotSize/2, dotSize, dotSize);
            
            // std::wstring txt = L"cells: "+std::to_wstring(cellularDimensions.X)+
            // L", "+std::to_wstring(cellularDimensions.Y);
            // Frame::placeText(screenPos.x, screenPos.y, txt, white, 9, graphics);
            // txt = std::to_wstring(cellularDimensions.Width)+L", "+std::to_wstring(cellularDimensions.Height);
            // Frame::placeText(screenPos.x, screenPos.y+10, txt, white, 9, graphics);
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
            case Player: {
                float s = speed + speed*bool(inputKeys&16);
                if (debugMoveSpeedBoost) s *= 2.0f;
                velocity = Math::Vector2(
                    s*(bool(inputKeys&1)-bool(inputKeys&4)),
                    s*(bool(inputKeys&2)-bool(inputKeys&8))
                ); 
                // update the direction you're facing based on velocity
                if (velocity.y > 0.0f) animations.facing = 0;
                else if (velocity.y < 0.0f) animations.facing = 1;
                else if (velocity.x > 0.0f) animations.facing = 3;
                else if (velocity.x < 0.0f) animations.facing = 2;
                else animations.stage = 0.0f;
                break;
            }

            case Wolf: { // walk directly towards player
                Math::Vector2 dir = Math::getUnitVector(centrePos, player->centrePos);
                velocity = dir * speed;
                break;
            }

            default: std::cout << "unknown entity\n"; break;
        }
    }

    void GameObject::updatePosition() {
        pos = pos + (velocity * deltaTime);
        centrePos = pos + Math::Vector2(size.x/2, size.y/2);
    }

    void GameObject::handleCollisions() {
        if (!hasCollision) return;
        // world borders
        if (pos.x<0.0f) pos.x = 0.0f;
        else if (pos.x>bkgWidth-size.x) pos.x = bkgWidth-size.x-1.0f;
        if (pos.y<0.0f) pos.y = 0.0f;
        else if (pos.y>bkgHeight-size.y) pos.y = bkgHeight-size.y-1.0f;

        // walls        
        // top side, move down
        for (int i = 1; i < cellularDimensions.X; i++) {
            Math::Vector2 node(pos.x+cellularDimensions.Width*i, pos.y);
            Math::Point2 p = findCell(node);
            if (grid[p.x][p.y]&BARRIER) pos.y = (p.y+1)*sideLen;
        }
        // bottom side, move up
        for (int i = 1; i < cellularDimensions.X; i++) {
            Math::Vector2 node(pos.x+cellularDimensions.Width*i, pos.y+size.y);
            Math::Point2 p = findCell(node);
            if (grid[p.x][p.y]&BARRIER) pos.y = p.y*sideLen - size.y;
        }
        // left side, move right
        for (int i = 1; i < cellularDimensions.X; i++) {
            Math::Vector2 node(pos.x, pos.y+cellularDimensions.Height*i);
            Math::Point2 p = findCell(node);
            if (grid[p.x][p.y]&BARRIER) pos.x = (p.x+1)*sideLen;
        }
        // right side, move left
        for (int i = 1; i < cellularDimensions.X; i++) {
            Math::Vector2 node(pos.x+size.y, pos.y+cellularDimensions.Height*i);
            Math::Point2 p = findCell(node);
            if (grid[p.x][p.y]&BARRIER) pos.x = p.x*sideLen - size.x;
        }

        // corners
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

    Gdiplus::TextureBrush * GameObject::chooseAnimationStage() {
        if (this != player) std::cout << "tree\n";
        Gdiplus::Image * image;
        switch (animations.facing)
        {
            case 0: { // forwards
                int idx = animations.stage*animations.frontInter;
                if (idx >= animations.front.size()) idx = animations.stage = 0;
                else if (!gameIsPaused) animations.stage += deltaTime;
                return animations.front[idx];
            }
            case 1: { // back
                int idx = animations.stage*animations.backInter;
                if (idx >= animations.back.size()) idx = animations.stage = 0;
                else if (!gameIsPaused) animations.stage += deltaTime;
                return animations.back[idx];
            }
            case 2: { // left
                int idx = animations.stage*animations.leftInter;
                if (idx >= animations.left.size()) idx = animations.stage = 0;
                else if (!gameIsPaused) animations.stage += deltaTime;
                return animations.left[idx];
            }
            case 3: { // right
                int idx = animations.stage*animations.rightInter;
                if (idx >= animations.right.size()) idx = animations.stage = 0;
                else if (!gameIsPaused) animations.stage += deltaTime;
                return animations.right[idx];
            }
            default:
                return nullptr;
        }
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