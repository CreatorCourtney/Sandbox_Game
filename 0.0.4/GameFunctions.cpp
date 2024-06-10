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
        Math::Vector2 newMapSize(currLevel->gridX*sideLen, currLevel->gridY*sideLen);
        // set the player's position with the hadamard product of the new map size and placement
        player->pos = Math::hadamard(placement, newMapSize);
    }
}