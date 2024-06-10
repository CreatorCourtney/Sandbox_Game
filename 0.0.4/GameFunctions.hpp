#ifndef GAME_FUNCTIONS_HPP
#define GAME_FUNCTIONS_HPP

#include "Globals.hpp"

namespace Game
{
    // switches from day to night, or night to day. autosaves the game
    void toggleDayNightCycle();

    // moves an entity into the specified level, from the current level
    Object::GameObject * MoveEntityToLevel(Object::GameObject *obj, Storage::Level *level, Math::Vector2 placement);

    // moves the player to another level by loading the selected scene
    void movePlayerToNewLevel(Storage::Level *lvl, Math::Vector2 placement);
}

#endif