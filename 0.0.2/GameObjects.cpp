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
        // brush mult, for drawing the entity's texture to scale
        float   scaleX = (float)size.x/(float)srcDimensions.x, 
                scaleY = (float)size.y/(float)srcDimensions.y;
        brushMultMatrix = new Gdiplus::Matrix(scaleX, 0.0f, 0.0f, scaleY, 0.0f, 0.0f);

        // assigning the number of cells, and space between collision points
        if (Type != Player) {
            int numX = size.x/sideLen+1, numY = size.y/sideLen+1,
                stepX = (float)size.x/(float)numX, stepY = (float)size.y/(float)numY;
            cellularDimensions = {numX, numY, stepX, stepY};
        }

        // set radius, the distance from the centre for entity collisions
        radius = Math::Max(size.x/2, size.y/2);

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
        if ((debuggingTools&5) == 5) DrawHitbox(graphics, 6, screenPos);
    }

    void GameObject::update() {
        if (gameIsPaused) return;
        updateVelocity();
        updatePosition();
        handleCollisions();
        if (this == player) updateBkg();
    }

    void GameObject::updateVelocity() {
        // for some reason intellisense thinks there are errors with these function calls. 
        // do not listen to it, it is lying, they compile just fine okay
        switch (type)
        {
            case Player:
                Func::playerVelocityFunc(this); break;

            case Wolf: // walk directly towards player
                Func::wolfVelocityFunc(this); break;

            default: std::cout << "unknown entity\n"; break;
        }
    }

    void GameObject::updatePosition() {
        pos = pos + (velocity * deltaTime);
        centrePos = pos + Math::Vector2(size.x/2, size.y/2);
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

    // improve the switch statement here when you get the chance :3
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
        switch (bkgState) {
            case 0: // full centre
                x += (wndWidth/2)-player->pos.x;
                y += (wndHeight/2)-player->pos.y;
                break;
            case 1: // centre y, left x
                if (bkgWidth<wndWidth) x += (wndWidth-bkgWidth)/2;
                y += (wndHeight/2)-player->pos.y;
                break;
            case 2: // centre y, right x
                x += wndWidth-bkgWidth;
                y += (wndHeight/2)-player->pos.y;
                break;
            case 4: // top y, centre x
                if (bkgHeight<wndHeight) y += (wndHeight-bkgHeight)/2;
                x += (wndWidth/2)-player->pos.x;
                break;
            case 5: // top y, left x
                if (bkgWidth<wndWidth) x += (wndWidth-bkgWidth)/2;
                if (bkgHeight<wndHeight) y += (wndHeight-bkgHeight)/2;
                break;
            case 6: // top y, right x
                x += wndWidth-bkgWidth;
                if (bkgHeight<wndHeight) y += (wndHeight-bkgHeight)/2;
                break;
            case 8: // bottom y, centre x
                x += (wndWidth/2)-player->pos.x;
                y += wndHeight-bkgHeight;
                break;
            case 9: // bottom y, left x
                if (bkgWidth<wndWidth) x += (wndWidth-bkgWidth)/2;
                y += wndHeight-bkgHeight;
                break;
            case 10: // bottom y, right x
                x += wndWidth-bkgWidth;
                y += wndHeight-bkgHeight;
                break;
        }
        return Math::Point2(x, y);
    }

}