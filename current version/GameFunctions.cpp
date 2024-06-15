#include "GameFunctions.hpp"

using namespace Globals;

namespace Game
{
    // switches from day to night, or night to day. autosaves the game
    void toggleDayNightCycle()
    {
        // toggle night
        isNightTime = !isNightTime;
        // autosave the game
        saveAllLevels();
        // reset g_time to 0
        g_time = 0.0f;
    }


    // moves an entity into the specified level, from the current level
    Object::GameObject * MoveEntityToLevel(Object::GameObject *obj, Storage::Level *level, Math::Vector2 placement)
    {
        // if the object being moved is the held object, do nothing since it should move
        // with the player, not on its own
        if (obj == heldObject) return nullptr;

        // find the position of the entity when it enters the new level
        Math::Vector2   levelSize(level->gridX*level->sideLen, level->gridY*level->sideLen),
                        newPos = Math::hadamard(placement, levelSize);

        // instantiate a new object in the level's gameObjects vector
        Object::GameObject *newObj = Object::Instantiate(obj->type, newPos, obj->hp, &level->gameObjects);

        // delete the object from the current scene
        Object::Destroy(obj);
        return newObj;
    }

    // moves the player to another level by loading the selected scene
    void movePlayerToNewLevel(Storage::Level *lvl, Math::Vector2 placement)
    {
        // if the player is holding something, it should be removed from the current scene
        // and added to the new scene, as the held object
        if (heldObject != nullptr) {
            // get a new pointer to the held object, and make the global pointer a nullptr
            Object::GameObject *newHeldObject = heldObject;
            heldObject = nullptr;
            // move this new object into the selected scene, and make it the global held object in
            // said scene
            delete lvl->heldObject;
            lvl->heldObject = MoveEntityToLevel(newHeldObject, lvl, placement);
        }

        // move the player themself by switching scenes
        switchScene(lvl);

        // find the size of the new map
        Math::Vector2 newMapSize(lvl->gridX*sideLen, lvl->gridY*sideLen);
        // set the player's position with the hadamard product of the new map size and placement
        player->pos = Math::hadamard(placement, newMapSize);
    }


    // attempts to spawn an enemy
    void attemptEnemySpawn() {
        // nothing will happen if the spawning attempt is invalid
        bool validAttempt = false;

        // enemies may only spawn after 10 seconds into the night
        if (isNightTime && g_time >= 10.0f) {
            // spawn an enemy every 10 seconds
            int currTime = (int)g_time, prevTime = int(g_time-deltaTime);
            if (currTime%10 == 0 && prevTime%10 != 0) {
                validAttempt = true;
            }
        }

        if (validAttempt) {
            // choose an enemy type to spawn in
            int range = Object::Wolf - Object::Wolf + 1;
            int type = (rand() % range) + Object::Wolf;

            switch (type)
            {
                case Object::Wolf:
                    spawnWolf();
                    break;
                
                default: break; // invalid spawn type
            }
        }
    }


    // spawns a wolf on a random edge of the map
    void spawnWolf()
    {
        // choose a random world edge to spawn on
        int edge = rand() % 4;
        int len; // length of the map edge
        
        float x, y; // coordinates the wolf will spawn at

        // set to true when a valid spawning location has been chosen
        bool validLocation = false;

        switch (edge)
        {
            case 0: { // top edge of the map
                // -51, to account for the size of the wolf
                len = (currLevel->gridX * sideLen) - 51;
                y = 1.0f;
                // make x a random number between 1 and len
                x = (rand()%len) + 1.0f;

                // check the cell below to make sure it's not water
                Math::Point2 cell(x/sideLen, y/sideLen);
                if (!(grid[cell.x][cell.y+1]&WATER)) validLocation = true;

                break;
            }
            case 1: { // left edge
                len = (currLevel->gridY * sideLen) - 51;
                x = 1.0f;
                y = (rand()%len) + 1.0f;

                // check the cell to the right to make sure it's not water
                Math::Point2 cell(x/sideLen, y/sideLen);
                if (!(grid[cell.x+1][cell.y]&WATER)) validLocation = true;

                break;
            }
            case 2: { // bottom edge
                len = (currLevel->gridX * sideLen) - 51;
                y = (currLevel->gridY * sideLen) - 51;
                x = (rand()%len) + 1.0f;

                // check the cell above to make sure it's not water
                Math::Point2 cell(x/sideLen, y/sideLen);
                if (!(grid[cell.x][cell.y-1]&WATER)) validLocation = true;

                break;
            }
            case 3: { // right edge
                len = (currLevel->gridY * sideLen) - 51;
                x = (currLevel->gridX * sideLen) - 51;
                y = (rand()%len) + 1.0f;

                // check the cell to the left to make sure it's not water
                Math::Point2 cell(x/sideLen, y/sideLen);
                if (!(grid[cell.x-1][cell.y]&WATER)) validLocation = true;

                break;
            }
        }

        // spawn a wolf at the desired location
        if (validLocation) Object::Instantiate(Object::Wolf, Math::Vector2(x, y), 5);
    }




    // deals a specified amount of damage to a cell
    void damageCell(Math::Point2 cell, int damage)
    {
        if (damage == 0) return;
        int num = grid[cell.x][cell.y];
        // nothing in the cell to damage, or it cannot be damaged
        if ((num&255) == 0 || (num&CAN_DIE) == 0) return;

        // find the current amount of health in the cell
        // bit shift it to represent as a regular value
        int health = (num&HEALTH)>>8;

        // subtract the amount of damage dealth
        health -= damage;

        // if the health is now less than zero, destroy the cell
        if (health <= 0) {
            Input::PlaceObjectInCell(cell, EMPTY);
        } else {
            // bit shift c_health to be in the right position
            health <<= 8;
            // reset the health bits of the cell
            num &= ~HEALTH;
            // set the health bits to the new value
            num |= health;
            grid[cell.x][cell.y] = num;
        }
    }




    // updates all the timed cells, such as saplings, in a level
    void updateTimedCells(Storage::Level *lvl)
    {
        if (lvl == nullptr) return; 
        for (int i = 0; i < lvl->timedCells.size(); i++)
        {
            // get the cell info from the grid
            Math::Point2 cell = lvl->timedCells[i];
            int cellInfo = lvl->grid[cell.x][cell.y];

            // get the timer bits of cellInfo as a regular int
            int currTimer = (cellInfo&TIMER)>>26;

            switch (cellInfo & 255) // just the first byte (cell's ID)
            {
                case 5: // sapling

                    // saplings grow at the beginning of each day
                    if (g_time == 0.0f && !isNightTime) {
                        // when the sapling grows, a tree will be placed in the cell
                        lvl->grid[cell.x][cell.y] = TREE;

                        // remove the cell from the vector, and decrement i, accordingly
                        lvl->timedCells.erase(lvl->timedCells.begin() + i--);
                        continue;
                    }
                    break;

                default: continue; // not actually a timed object
            }

            // update the cell's timer info
            currTimer <<= 26; // set the bits of currTimer back in the correct position
            // clear the cellInfo's timer bits
            cellInfo &= ~TIMER;
            // set the timer bits with currTimer
            cellInfo |= currTimer;

            // update the actual grid
            lvl->grid[cell.x][cell.y] = cellInfo;
        }
    }
}