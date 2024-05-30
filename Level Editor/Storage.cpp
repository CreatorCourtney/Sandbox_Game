#include "Storage.hpp"

using namespace Globals;

namespace Storage
{
    // constructor
    Entity::Entity(Math::Vector2 Pos, Math::Vector2 Velocity, EntityType Type, int Hp, int Idx) :
    pos(Pos), velocity(Velocity), type(Type), hp(Hp), idx(Idx)
    {
        switch (type)
        {
            case Player:
                // assign texture variables
                brush = playerBrush;
                srcDimensions = {(int)playerImg->GetWidth(), (int)playerImg->GetHeight()};
                size = Math::Point2(sideLen-2, sideLen-2);

                // attributes
                cellularDimensions = {1, 1, size.x, size.y};
                hasCollision = true;
                radius = Math::Max(size.x/2, size.y/2);
                centrePos = pos + Math::Vector2(size.x/2, size.y/2);
                maxHP = 5;
                speed = 100.0f;

                // misc
                cell = Math::Point2(centrePos.x/sideLen, centrePos.y/sideLen);
                break;

            case Wolf:
                // assign texture variables
                brush = wolfBrush;
                srcDimensions = {(int)wolfImg->GetWidth(), (int)wolfImg->GetHeight()};
                size = Math::Point2((sideLen-2)*1.5f, (sideLen-2)*1.5f);
                
                // attributes
                hasCollision = true;
                radius = Math::Max(size.x/2, size.y/2);
                centrePos = pos + Math::Vector2(size.x/2, size.y/2);
                maxHP = 3;
                speed = 80.0f;

                // misc
                cell = Math::Point2(centrePos.x/sideLen, centrePos.y/sideLen);
                break;

            case Log_Item:
                // assgign texture variables
                srcDimensions = {(int)logImg->GetWidth(), (int)logImg->GetHeight()};
                brush = logBrush;
                size = Math::Point2((sideLen-2)*0.8f, (sideLen-2)*0.8f);

                // attributes
                centrePos = pos + Math::Vector2(size.x/2, size.y/2);
                maxHP = 15; // up to 15 logs can stack together
                hasCollision = true;
                radius = Math::Max(size.x/2, size.y/2);
                // items use speed as a way to scale their damage when thrown
                speed = 1.0f;

                // misc
                cell = Math::Point2(centrePos.x/sideLen, centrePos.y/sideLen);
                break;

            case Pine_Cone_Item:
                // assign texture variables
                srcDimensions = {(int)Pine_ConeImg->GetWidth(), (int)Pine_ConeImg->GetHeight()};
                brush = Pine_ConeBrush;
                size = Math::Point2((sideLen-2)*0.8f, (sideLen-2)*0.8f);

                // attributes
                centrePos = pos + Math::Vector2(size.x/2, size.y/2);
                maxHP = 5; // up to 5 pine cones can stack
                radius = Math::Max(size.x/2, size.y/2);
                hasCollision = true;
                // items use speed as a way to scale their damage when thrown
                speed = 0.5f;

                // misc
                cell = Math::Point2(centrePos.x/sideLen, centrePos.y/sideLen);
                break;

            default: break;
        }
        // if the desired hp is greater than the max hp, fix that
        hp = Math::Min(hp, maxHP);

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

        // set the brush to not wrap
        if (brush != nullptr) if (brush->GetLastStatus() == Gdiplus::Ok)
            brush->SetWrapMode(Gdiplus::WrapModeClamp);
    }

    // public drawing functions
    // draws the entity to the graphics object
    void Entity::show(Gdiplus::Graphics& graphics)
    {
        // not in the map region, don't render
        if (pos.x<mapRect.X-size.x || pos.x>mapRect.X+mapRect.Width ||
            pos.y<mapRect.Y-size.y || pos.x>mapRect.Y+mapRect.Height) return;

        // set the transform of the brush to draw correctly
        brush->ResetTransform();
        brush->TranslateTransform(pos.x, pos.y);
        brush->MultiplyTransform(brushScaleMatrix);

        // draw the brush to the window
        graphics.FillRectangle(brush, pos.x, pos.y, (Gdiplus::REAL)size.x, (Gdiplus::REAL)size.y);
    }

    // canges the scale of the object according to sideLen
    void Entity::resize(int cellSideLen, int previosuSideLen)
    {
        // update the position
        // relative position on the map
        Math::Vector2 relativePos = pos - Math::Vector2(mapRect.X, mapRect.Y);

        Math::Vector2 Pos = relativePos / previosuSideLen; // relative position before the size change
        Pos = Pos * cellSideLen; // position with the new side length in mind
        Pos = Pos + Math::Vector2(mapRect.X, mapRect.Y); // actual position on the screen
        // update object's position
        pos = Pos;

        // different entities have different sizes
        switch (type)
        {
            case Player:
                size = Math::Point2(cellSideLen-2, cellSideLen-2);
                break;

            case Wolf:
                size = Math::Point2((cellSideLen-2)*1.5f, (cellSideLen-2)*1.5f);
                break;

            case Log_Item:
                size = Math::Point2((cellSideLen-2)*0.8f, (cellSideLen-2)*0.8f);
                break;

            case Pine_Cone_Item:
                size = Math::Point2((cellSideLen-2)*0.8f, (cellSideLen-2)*0.8f);
                break;

            default: break;
        }

        // update the brush scale matrix
        delete brushScaleMatrix; // clean up the old matrix
        float   scaleX = (float)size.x/(float)srcDimensions.x, 
                scaleY = (float)size.y/(float)srcDimensions.y;
        brushScaleMatrix = new Gdiplus::Matrix(scaleX, 0.0f, 0.0f, scaleY, 0.0f, 0.0f);

        // update radius and cellular dimensions
        int numX = size.x/cellSideLen + 1, numY = size.y/cellSideLen + 1,
            stepX = (float)size.x/(float)numX, stepY = (float)size.y/(float)numY;
        cellularDimensions = {numX, numY, stepX, stepY};
        radius = Math::Max(size.x/2, size.y/2);
    }


    // cleanup function
    void Entity::deallocateResources()
    {
        // currently, just delete the reference to itself and scale matrix
        delete brushScaleMatrix;
        delete this;
    }



    // saves the created level to file
    void saveLevelToFile(std::string filename)
    {
        // open the file for writing
        std::fstream file;
        file.open(filename, std::ios::out);
        if (!file) return; // validate the file


        // print the name of the level into tone line
        file << levelFileName <<'\n';

        // since the player and held object are stored with the rest of the game objects,
        // just store their indices here, and they can be found later. the held objects index is
        // -1 if it doesn't exist. for this editor, the index of the held object will always be -1
        // building type, therefore, will always be 0, and for good measure make time 0.0 as well
        int idxPlayer = (player == nullptr)? -1 : player->idx;
        // put all of these onto one line
        file << idxPlayer <<'\t'<< -1 <<'\t'<< 0 <<'\t'<< 0 <<'\n';

        // in one line, save the number of gameObjects in the scene.
        // in the next num lines, save all the game objects in the scene
        int num = entities.size();
        
        file << num <<'\n';
        for (int i = 0; i < num; i++) {
            SaveEntityToFile(&file, entities[i]);
        }

        // write the map dimensions, as well as the desired cell side length to one line
        file << gridSizeX <<'\t'<< gridSizeY <<'\t'<< savedSideLen <<'\n';

        // the next gridX lines will contain gridY hexdec entries
        for (int i = 0; i < gridSizeX; i++) {
            for (int j = 0; j < gridSizeY; j++) {
                file << std::hex << grid[i][j] << '\t';
            }
            file << '\n';
        }

        // close the file
        file.close();
    }

    // saves an entity to one line of a file
    void SaveEntityToFile(std::fstream *file, Storage::Entity *obj)
    {
        // validate the existence of the file and the game object
        if (!file) {
            return;

        } else if (obj != nullptr) { // don't try to save unless the object actually exists

            // resize the entity with the side length actually saved to file
            obj->resize(savedSideLen, sideLen);
            // update the entity's position relative to the map, not the window
            Math::Vector2 mPos = obj->pos - Math::Vector2(mapRect.X, mapRect.Y);
            mPos = (mPos / sideLen) * savedSideLen;
            Math::Vector2 cPos = mPos + Math::Vector2(obj->size.x, obj->size.y);
            Math::Point2 cell(cPos.x/savedSideLen, cPos.y/savedSideLen);

            // to one line, write all information about the object
            *file << std::dec <<obj->idx <<'\t'    // write the object's index first, as a sort of ID 
                  << mPos.x                 <<' '<< mPos.y                      <<'\t'  // pos
                  << cPos.x                 <<' '<< cPos.y                      <<'\t'  // centrePos
                  << obj->velocity.x        <<' '<< obj->velocity.y             <<'\t'  // velocity
                  << obj->acceleration.x    <<' '<< obj->acceleration.y         <<'\t'  // acceleration
                  << obj->size.x            <<' '<< obj->size.y                 <<'\t'  // size
                  << cell.x                 <<' '<< cell.y                      <<'\t'  // cell

                  << obj->type     <<'\t'<<    obj->hp    <<' '<<   obj->maxHP  <<'\t'  // type, hp, maxHP
                  << obj->speed  <<'\t'<< obj->timer <<'\t'<< obj->hasCollision <<'\t'  // speed, timer, collision

                  << obj->cellularDimensions.X    <<' '<<obj->cellularDimensions.Y     <<' ' // cellular dimensions (x/y)
                  << obj->cellularDimensions.Width<<' '<<obj->cellularDimensions.Height<<'\t'// cellular dimensions (wid/height)

                  << obj->radius <<'\n'; // radius, endline

            // resize the entity back to how it was, to continue drawing
            obj->resize(sideLen, savedSideLen);
        }
    }
}