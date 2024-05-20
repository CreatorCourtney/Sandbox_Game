#ifndef STORAGE_HPP
#define STORAGE_HPP

#include <iostream>
#include <fstream>
#include <sstream>

#include "GameObjects.hpp"

namespace Storage
{
    // a struct to store all of the information about a level
    struct Level
    {
        // name of the level
        std::string lvlName = "Default Level";

        // game objects
        int buildingType = EMPTY; // type of object the player tries to put in a cell
        Object::GameObject * player; // pointer to the player
        Object::GameObject * heldObject = nullptr; // object the player is holding

        std::vector<Object::GameObject*> gameObjects; // vector of all the game objects

        // information about the level's map
        float sideLen; // dimensions of each cell

        int gridX, gridY; // dimensions of grid
        std::vector<std::vector<int>> grid; // 2d array of all cells in the map

        // constructor
        Level(Object::GameObject *Player, Object::GameObject *HeldObject, int BuildingType,
        std::vector<Object::GameObject*> GameObjects, float SideLen, 
        std::vector<std::vector<int>> Grid);
    };

    // a struct to hold information about the user's settings
    struct UserSettings
    {
        float g_scale; // global scale, kind of dertermines how far the camera appears from the map
        unsigned char debugTools; // which developer tools the user has enabled    

        // constructor
        UserSettings(float G_scale, unsigned char DebugTools);
    };


    // saving 

    // save data from an object into a file
    int SaveSceneToFile(Level scene);

    // save attributes of a game object into a file
    int SaveGameObjectToFile(std::fstream *file, Object::GameObject *obj);


    // loading

    // takes the information stored in a file and loads it into a level object
    Level LoadLevelObjectFromFile(std::string filename);

    // creates a game object from the line of a file, and returns a reference to it
    Object::GameObject * CreateGameObjectFromFile(std::istringstream *iss);
}

#endif