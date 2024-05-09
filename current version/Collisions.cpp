#include "Collisions.hpp"

using namespace Globals;

namespace Object {
    void GameObject::handleCollisions() {
        if (!hasCollision) return;
        // world borders
        if (pos.x<0.0f) pos.x = 0.0f;
        else if (pos.x>bkgWidth-size.x) pos.x = bkgWidth-size.x-1.0f;
        if (pos.y<0.0f) pos.y = 0.0f;
        else if (pos.y>bkgHeight-size.y) pos.y = bkgHeight-size.y-1.0f;

        // walls        
        HandleSideCollisionsWithWalls();
        HandleCornerCollisionsWithWalls();

        // other entities
        handleCollisionsWithGameObjects();
    }

    void GameObject::HandleSideCollisionsWithWalls() {
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
    }

    void GameObject::HandleCornerCollisionsWithWalls() {
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

    void GameObject::DrawHitbox(Gdiplus::Graphics& graphics, int dotSize, Math::Point2 screenPos) {
        int d = dotSize/2;
        Math::Point2 centreScreenPos = Math::Point2(screenPos.x+size.x/2, screenPos.y+size.y/2);
        Gdiplus::SolidBrush redBrush(red), blueBrush(blue);
        Gdiplus::Pen pen(blue), pen2(green), pen3(red);

        graphics.DrawRectangle(&pen, screenPos.x, screenPos.y, size.x, size.y);
        graphics.DrawEllipse(&pen2, centreScreenPos.x-radius, centreScreenPos.y-radius, 2*radius, 2*radius);

        // top side
        for (int i = 0; i <= cellularDimensions.X; i++) {
            int x = screenPos.x+cellularDimensions.Width*i-d,
                y = screenPos.y-d;
            graphics.FillEllipse(&redBrush, x, y, dotSize, dotSize);
        }
        // bottom side
        for (int i = 0; i <= cellularDimensions.X; i++) {
            int x = screenPos.x+cellularDimensions.Width*i-d,
                y = screenPos.y+size.y-d;
            graphics.FillEllipse(&redBrush, x, y, dotSize, dotSize);
        }
        // left side
        for (int i = 1; i <= cellularDimensions.Y; i++) {
            int y = screenPos.y+cellularDimensions.Height*i-d,
                x = screenPos.x-d;
            graphics.FillEllipse(&redBrush, x, y, dotSize, dotSize);
        }
        // right side
        for (int i = 1; i <= cellularDimensions.Y; i++) {
            int y = screenPos.y+cellularDimensions.Height*i-d,
                x = screenPos.x+size.x-d;
            graphics.FillEllipse(&redBrush, x, y, dotSize, dotSize);
        }
        graphics.FillEllipse(&blueBrush, centreScreenPos.x-d, centreScreenPos.y-d, dotSize, dotSize);

        // draw arrow in direction of velocity
        Math::Vector2 centrePosVec(centreScreenPos.x, centreScreenPos.y);
        Math::Vector2 p = centrePosVec + velocity/2.5f;

        graphics.DrawLine(&pen2, Gdiplus::Point(centreScreenPos.x, centreScreenPos.y), 
        Gdiplus::Point(p.x, p.y));
    }

    void GameObject::handleCollisionsWithGameObjects() {
        // iterate through all the other game objects
        for (int i = 0; i < gameObjects.size(); i++) {
            // doesn't collide with itself or objects that have no collision
            if (gameObjects[i]==this || !gameObjects[i]->hasCollision) continue;
            Math::Vector2 disp = gameObjects[i]->centrePos - centrePos;
            // break up objects directly on each other
            if (disp == Math::Zero2) disp = Math::One2;
            float d = Math::absf(disp.length()), r = radius+gameObjects[i]->radius;
            if (d < r) {
                // d is the total distance it needs to move
                disp.normalise(); disp = disp * (r-d)/2;
                pos = pos - disp;
                centrePos = centrePos - disp;
                gameObjects[i]->pos = gameObjects[i]->pos + disp;
                gameObjects[i]->centrePos = gameObjects[i]->centrePos + disp;
            } 
        }
    }
}