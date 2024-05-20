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
    GameObject::GameObject(Math::Vector2 Pos, Math::Vector2 Velocity, EntityType Type, int Hp, int Idx)
    : pos(Pos), velocity(Velocity), type(Type), hp(Hp), idx(Idx) 
    {
        switch (type)
        {
            case Player:
                // assign texture variables
                animations = playerAnimations;
                srcDimensions = {25, 25};
                size = Math::Point2(50*g_scale, 50*g_scale);
                // animation script
                animationScript = Func::playerAnimationScript;

                // attributes
                hasAnimation = true;
                cellularDimensions = {1, 1, size.x, size.y};
                hasCollision = true;
                radius = Math::Max(size.x/2, size.y/2);
                centrePos = pos + Math::Vector2(size.x/2, size.y/2);
                maxHP = 5;
                speed = 100.0f*g_scale;
                
                // behaviour functions
                velocityFunc = Func::playerVelocityFunc;
                positionFunc = Func::defaultPositionFunc;
                collisionFunc = Collisions::defaultCollisionFunction;

                // misc
                // set the interaction radius
                interactRange = interactRangeCells * sideLen;
                cell = findCell(centrePos);
                break;

            case Wolf:
                // assign texture variables
                brush = wolfBrush;
                srcDimensions = {50, 50};
                size = Math::Point2(75*g_scale, 75*g_scale);
                // animation script
                animationScript = Func::noAnimation;

                // attributes
                hasCollision = true;
                radius = Math::Max(size.x/2, size.y/2);
                centrePos = pos + Math::Vector2(size.x/2, size.y/2);
                maxHP = 3;
                speed = 80.0f*g_scale;
                
                // behaviour functions
                velocityFunc = Func::wolfVelocityFunc;
                positionFunc = Func::defaultPositionFunc;
                collisionFunc = Collisions::defaultCollisionFunction;

                // misc
                cell = findCell(centrePos);
                break;

            case Falling_Tree:
                // assign texture variables
                srcDimensions = {250, 250};
                brush = falling_treeBrush;
                size = Math::Point2(10*sideLen, 10*sideLen);
                // animation script
                animationScript = Func::treeFallingAnimations;

                // attributes
                hasAnimation = true;
                hasCollision = false;
                centrePos = pos;
                timer = 1.0f; // amount of time it will exist
                maxHP = 1;
                speed = 0.0f;

                // behaviour functions
                velocityFunc = Func::defaultVelocityFunc;
                positionFunc = Func::fallingTreePositionFunc;
                collisionFunc = Collisions::defaultCollisionFunction;

                // misc
                cell = Math::Point2((pos.x/sideLen)+1, (pos.y/sideLen)+9);
                break;

            case Log_Item:
                // assgign texture variables
                srcDimensions = {50, 50};
                brush = logBrush;
                size = Math::Point2(40*g_scale, 40*g_scale);
                // animation script
                animationScript = Func::noAnimation;

                // attributes
                centrePos = pos + Math::Vector2(size.x/2, size.y/2);
                maxHP = 15; // up to 15 logs can stack together
                hasCollision = true;
                radius = Math::Max(size.x/2, size.y/2);
                // items use speed as a way to scale their damage when thrown
                speed = 1.0f;

                // behaviour functions
                velocityFunc = Func::defaultVelocityFunc;
                positionFunc = Func::thrownItemPositionFunc;
                collisionFunc = Collisions::stationaryItemCollisionFunction;

                // misc
                cell = findCell(centrePos);
                break;

            case Pine_Cone_Item:
                // assign texture variables
                srcDimensions = {25, 25};
                brush = Pine_ConeBrush;
                size = Math::Point2(40*g_scale, 40*g_scale);
                // animation script
                animationScript = Func::noAnimation;

                // attributes
                centrePos = pos + Math::Vector2(size.x/2, size.y/2);
                maxHP = 5; // up to 5 pine cones can stack
                radius = Math::Max(size.x/2, size.y/2);
                hasCollision = true;
                // items use speed as a way to scale their damage when thrown
                speed = 0.5f;

                // behaviour functions
                velocityFunc = Func::defaultVelocityFunc;
                positionFunc = Func::thrownItemPositionFunc;
                collisionFunc = Collisions::stationaryItemCollisionFunction;

                // misc
                cell = findCell(centrePos);
                break;
        }

        // brush mult, for drawing the entity's texture to scale
        float   scaleX = (float)size.x/(float)srcDimensions.x, 
                scaleY = (float)size.y/(float)srcDimensions.y;
        brushScaleMatrix = new Gdiplus::Matrix(scaleX, 0.0f, 0.0f, scaleY, 0.0f, 0.0f);

        
        // assigning the number of cells, and space between collision points
        if (Type != Player) {
            int numX = size.x/sideLen+1, numY = size.y/sideLen+1,
                stepX = (float)size.x/(float)numX, stepY = (float)size.y/(float)numY;
            cellularDimensions = {numX, numY, stepX, stepY};
        }


        // preemptively set the brushes so that they don't wrap, thus drawing the image only once
        for (int i = 0; i < animations.front.size(); i++) {
            if (animations.front[i]->GetLastStatus() == Gdiplus::Ok)
            animations.front[i]->SetWrapMode(Gdiplus::WrapModeClamp);
        }
        for (int i = 0; i < animations.back.size(); i++) {
            if (animations.back[i]->GetLastStatus() == Gdiplus::Ok)
            animations.back[i]->SetWrapMode(Gdiplus::WrapModeClamp);
        }
        for (int i = 0; i < animations.left.size(); i++) {
            if (animations.left[i]->GetLastStatus() == Gdiplus::Ok)
            animations.left[i]->SetWrapMode(Gdiplus::WrapModeClamp);
        }
        for (int i = 0; i < animations.right.size(); i++) {
            if (animations.right[i]->GetLastStatus() == Gdiplus::Ok)
            animations.right[i]->SetWrapMode(Gdiplus::WrapModeClamp);
        }
        if (!hasAnimation && brush->GetLastStatus() == Gdiplus::Ok) 
            brush->SetWrapMode(Gdiplus::WrapModeClamp);
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
        Gdiplus::TextureBrush * imgBrush = animationScript(this, &screenPos);

        // draw the image
        graphics.FillRectangle(imgBrush, screenPos.x, screenPos.y, size.x, size.y);

        // debugging tools
        if ((debuggingTools&5) == 5) DrawHitbox(graphics, 6, screenPos);
    }

    // calls the behaviour functions, updating member variables
    void GameObject::update() 
    {
        if (gameIsPaused) return; // do nothing when the game is paused

        // kill objects with 0 health
        if (hp <= 0) {
            Destroy(this);
            return;
        } else hp = Math::Min(hp, maxHP);

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

        // find the real position of the mouse in world space
        mousePosREAL = getWorldPosition(mousePos);
    }


    // finds the cell coordinates of a position in world space
    Math::Point2 findCell(Math::Vector2 pos) 
    {
        // since each cell is 'sideLen' units wide and tall, dividing a coordinate
        // by 'sideLen' will yield the cell indices

        // if sideLen == 0, or if grid doesn't exist, there will be an error,
        // just return (0, 0) to avoid a crash

        if (sideLen == 0.0f || grid.size() == 0) return Math::Point2(0, 0);

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




    // public drawing functions

    // prepares a texturebrush to be drawn to the screen by setting the matrix displacement and scale
    void GameObject::setBrushMatrix(Gdiplus::TextureBrush *b, Math::Point2 disp)
    {
        Gdiplus::Matrix matrix(1.0f, 0.0f, 0.0f, 1.0f, disp.x, disp.y);
        b->SetTransform(&matrix);
        b->MultiplyTransform(brushScaleMatrix);
    }




    // for instantiating game objects. returns a reference to the object created
    GameObject* Instantiate(EntityType type, Math::Vector2 pos, int hp)
    {
        // create the object
        GameObject *obj = new GameObject(pos, Math::Vector2(0.0f, 0.0f), type, hp, gameObjects.size());
        // add it to the game objects vector
        gameObjects.push_back(obj);

        // return the reference to the game object just created
        return obj;
    }

    // deltes the specified object
    void Destroy(GameObject * obj)
    {
        // object doesn't exist, don't try to destroy it
        if (obj == nullptr) return;

        int n = obj->idx; // index of the object in the global vector
        // decrement the indices of all following game objects,
        // as the size of the vector will decrease
        for (int i = n+1; i < gameObjects.size(); i++) gameObjects[i]->idx--;

        // erase the object's entry in the global vector
        gameObjects.erase(gameObjects.begin()+n);

        // remove itself from the owner's owned objects
        if (obj->owner != nullptr)
        {
            // if the player is holding obj, stop that
            if (obj->owner == player || obj == heldObject) {
                heldObject = nullptr;
                buildingType = EMPTY;
            }

            // look through the owned objects vector
            n = obj->owner->ownedObjects.size();
            for (int i = 0; i < n; i++) {
                // object found
                if (obj->owner->ownedObjects[i] == obj) {
                    // remove the object from the vector
                    obj->owner->ownedObjects.erase(obj->owner->ownedObjects.begin()+i);
                    // exit the loop
                    break;
                }
            }
        }

        // remove itself as the owner of all owned objects
        n = obj->ownedObjects.size();
        for (int i = 0; i < n; i++) {
            GameObject *ownedObj = obj->ownedObjects[i];

            // ensure obj is the owner of the object
            if (obj == ownedObj->owner) {
                // remove obj as the owner
                ownedObj->owner = nullptr;
            }
        }

        // cleanup object's resources
        obj->deallocateResources();
    }

    // cleanup function
    void GameObject::deallocateResources()
    {
        // currently, all of the images used for drawing are stored in global
        // variables which wil be reused by other iterations of the object. don't delete
        // these until the application closes, as other objects will need to
        // use these resources

        // free the matrix for brush scaling
        delete brushScaleMatrix;

        // delete the object itself
        delete this;
    }

    // spawns an item stack with random velocity. return a reference to the item created
    GameObject* spawnItemStack(EntityType type, Math::Vector2 pos, int count)
    {
        if (type<Log_Item) return nullptr; // not an item, don't do anything

        // generate a vector with two random integers
        // rand() returns a value in the range [0, RAND_MAX)
        // to include both positive and negative numbers, subtract RAND_MAX/2
        // from rand(), so there's a 50% chance of being negative
        int num = RAND_MAX/2;
        Math::Vector2 dir(rand()-num, rand()-num);

        // normalise the vector to get a unit vector direction
        dir.normalise();

        // create an item object, and save the reference to it
        GameObject* obj = Instantiate(type, pos, count);

        // set the object's velocity and acceleration
        obj->velocity = dir * 35.0f;
        obj->acceleration = dir * -35.0f;

        // return a reference to the item stack created
        return obj;
    }
}