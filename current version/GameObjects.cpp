#include "GameObjects.hpp"

using namespace Globals;

namespace Object
{
    // constructor for animation object
    Animations::Animations(std::vector<Gdiplus::TextureBrush*> Front,
            std::vector<Gdiplus::TextureBrush*> Back,
            std::vector<Gdiplus::TextureBrush*> Left,
            std::vector<Gdiplus::TextureBrush*> Right,
            float FrontT, float BackT, float LeftT, float RightT) :
    front(Front), back(Back), left(Left), right(Right) {
        // assign vectors directly, for the time intervals, the function parameter
        // is a float number of seconds that the animation cycle should last
        // i.e., if the 'FrontT' parameter is 3.0, it should take 3 seconds to
        // cycle through every image in the 'front' vector

        // to find the intervals (time between each image in the vector),
        // divide the number of images (vector size) by the total time
        frontInter = float(Front.size()) / FrontT;
        backInter = float(Back.size()) / BackT;
        leftInter = float(Left.size()) / LeftT;
        rightInter = float(Right.size()) / RightT;
    }

    // default constructor: do nothing
    Animations::Animations() {}


    // constructor for GameObject
    GameObject::GameObject(Math::Vector2 Pos, Math::Vector2 Velocity,
    EntityType Type, Math::Point2 Size, float Speed, float Hp)
    : pos(Pos), velocity(Velocity), type(Type), size(Size), speed(Speed), hp(Hp) 
    {
        centrePos = pos + Math::Vector2(size.x/2, size.y/2);

        // index is the size of the gameObjects vector, which gets incremented after the creation of this
        idx = gameObjects.size();

        switch (type)
        {
            case Player:
                // assign texture variables
                animations = playerAnimations;
                srcDimensions = {25, 25};
                // attributes
                cellularDimensions = {1, 1, size.x, size.y};
                hasCollision = true;
                radius = Math::Max(size.x/2, size.y/2);
                
                // behaviour functions
                velocityFunc = Func::playerVelocityFunc;
                positionFunc = Func::defaultPositionFunc;
                collisionFunc = Collisions::defaultCollisionFunction;

                // misc
                // cell size is relative to the size of the player's hitbox
                // but add 2 so they can more easily walk through 1 wide gaps
                sideLen = Math::maxf(size.x, size.y)+2.0f;
                SetGrid(); // map created relative to the size of the player
                cell = findCell(centrePos);
                break;

            case Wolf:
                // assign texture variables
                brush = wolfBrush;
                srcDimensions = {50, 50};
                // attributes
                hasAnimation = false;
                hasCollision = true;
                radius = Math::Max(size.x/2, size.y/2);
                
                // behaviour functions
                velocityFunc = Func::wolfVelocityFunc;
                positionFunc = Func::defaultPositionFunc;
                collisionFunc = Collisions::defaultCollisionFunction;

                // misc
                cell = findCell(centrePos);
                break;

            case Falling_Tree:
                // assign texture variables
                animations = falling_treeAnimations;
                srcDimensions = {250, 250};
                // attributes
                hasCollision = false;

                // behaviour functions
                velocityFunc = Func::zeroVelocityFunc;
                positionFunc = Func::fallingTreePositionFunc;
                collisionFunc = Collisions::defaultCollisionFunction;

                // misc
                cell = Math::Point2((pos.x/sideLen)+8.26f, (pos.y/sideLen)+8.98f);
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

    // draws the object to the graphics object
    void GameObject::draw(Gdiplus::Graphics& graphics)
    {
        // position of the object on the screen
        Math::Point2 screenPos = getScreenPosition(pos);
        // offscreen, dont render
        if (screenPos.x<-size.x || screenPos.x>wndWidth ||
            screenPos.y<-size.y || screenPos.y>wndHeight) return;

        // select an image (TextureBrush) to be drawn
        // if the object has animation, choose from the 'animations' object, otherwise,
        // use the 'brush' member
        Gdiplus::TextureBrush * imgBrush = (hasAnimation)? chooseAnimationStage() : brush;

        // displacement matric for the brush, so that the image draws correctly
        Gdiplus::Matrix matrix(1.0f, 0.0f, 0.0f, 1.0f, screenPos.x, screenPos.y);
        imgBrush->SetTransform(&matrix);
        // use the 'brushMultMatrix' member to scale the brush, so the image is drawn to scale
        imgBrush->MultiplyTransform(brushMultMatrix);

        // draw the image
        graphics.FillRectangle(imgBrush, screenPos.x, screenPos.y, size.x, size.y);

        // debugging tools
        if ((debuggingTools&5) == 5) DrawHitbox(graphics, 6, screenPos);
    }

    // calls the behaviour functions, updating member variables
    void GameObject::update() 
    {
        if (gameIsPaused) return; // do nothing when the game is paused

        velocityFunc(this); // update velocity
        positionFunc(this); // update position
        collisionFunc(this); // handle collisions

        // when the player gets updated, update the position of
        // the camera based on the player's position
        if (this == player) updateBkg();
    }

    // updates the position of the camera based on the player's position
    void GameObject::updateBkg() 
    {
        // 1st bit = <wndwidth/2||width<wndWidth
        int left = bkgWidth<wndWidth || pos.x<wndWidth/2;
        // 2nd bit = >bkgWidth-wndWidth/2
        int right = (left)? 0 : (pos.x>bkgWidth-wndWidth/2)<<1;
        // 3rd bit = <wndHeight/2 || height<wndHeight
        int top = (bkgHeight<wndHeight || pos.y<wndHeight/2)<<2;
        // 4th bit = >bkgHeight-wndHeight/2
        int bottom = (top)? 0 : (pos.y>bkgHeight-wndHeight/2)<<3;

        // bkgState is used in other functions to determine where the camer is
        bkgState = bottom|top|right|left;
    }

    // choses one image from the animations object that will be drawn to the screen
    Gdiplus::TextureBrush * GameObject::chooseAnimationStage() 
    {   
        // choose vector/index based on direction the object is facing

        // to choose index, multiply the 'stage' member with the associated interval member
        // then truncate the decimal by typecasting to int. if the index is bigger
        // than the size of the vector, then set the stage to zero and return the first
        // image in the vector. otherwise, use the index to choose an image.
        // also increment the stage by adding deltaTime to it

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
            default: return nullptr;
        }
    }

    // finds the cell coordinates of a position in world space
    Math::Point2 findCell(Math::Vector2 pos) 
    {
        // since each cell is 'sideLen' units wide and tall, dividing a coordinate
        // by 'sideLen' will yield the cell indices
        int x = int(pos.x/sideLen), y = int(pos.y/sideLen);

        // since the coordinates should be able to be used as indices in the 'grid'
        // 2d array, they need to be >0 and < the size of the grid
        if (x>=grid.size()) x = grid.size()-1;
        else if (x<0) x = 0;
        if (y>=grid[0].size()) y = grid[0].size()-1;
        else if (y<0) y = 0;

        // return the coordinates
        return Math::Point2(x, y);
    }


    // need to add more comments to these two functions :|

    // improve the switch statement here when you get the chance :3
    // finds the position on the map associated with (x,y) coordinates on the screen
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

    // translates a position on the map to its relative plaement on the screen
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


    // for instantiating game objects
    void Instantiate(EntityType type, Math::Vector2 pos, Math::Point2 size, float speed, float hp)
    {
        // create the object 
        GameObject *obj = new GameObject(pos, Math::Vector2(0.0f, 0.0f), type, size, speed, hp);
        // add it to the game objects vector
        gameObjects.push_back(obj);
    }

    // deltes the specified object
    void Destroy(GameObject * obj)
    {
        int n = obj->idx; // index of the object in the global vector
        // decrement the indices of all following game objects,
        // as the size of the vector will decrease
        for (int i = n+1; i < gameObjects.size(); i++) gameObjects[i]->idx--;

        // erase the object's entry in the global vector
        gameObjects.erase(gameObjects.begin()+n);

        // deallocate the object's resources, to avoid memory leaks
        delete obj;
    }
}