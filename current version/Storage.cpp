#include "Storage.hpp"

namespace Storage
{
    // constructor
    Level::Level(Object::GameObject *Player, Object::GameObject *HeldObject, int BuildingType,
        std::vector<Object::GameObject*> GameObjects, float SideLen, 
        std::vector<std::vector<int>> Grid, float Time) : 
    player(Player), heldObject(HeldObject), buildingType(BuildingType), 
    gameObjects(GameObjects), sideLen(SideLen), grid(Grid), time(Time) 
    {
        // assign the grid dimensions with the .size() function
        gridX = grid.size();
        // before checking the size of grid[0], make sure grid[0] actually exists, to avoid a crash
        gridY = (gridX>0)? grid[0].size() : 0;
    }


    // constructor
    UserSettings::UserSettings(float G_scale, unsigned char DebugTools) :
    g_scale(G_scale), debugTools(DebugTools) {}



    // save data from an object into a file
    int SaveSceneToFile(Level scene)
    {
        // open the specified file for writing
        std::fstream file;
        file.open("data/"+scene.lvlName+".txt", std::ios::out);
        if (!file) return -1; // validate the file

        int ext = 0;
        
        // print the name of the scene into one line
        file << scene.lvlName <<'\n';

        // since the player and held object are stored with the rest of the game objects,
        // just store their indices here, and they can be found later. the held objects index is
        // -1 if it doesn't exist
        int idxPlayer = scene.player->idx, 
            idxHeld = (scene.heldObject == nullptr)? -1 : scene.heldObject->idx;
        // put the indices, building type, and time on one line
        file << idxPlayer <<'\t'<< idxHeld <<'\t' 
             <<std::hex<<scene.buildingType <<'\t'<< std::dec<<scene.time<<'\n';

        // in one line, save the number of gameObjects in the scene.
        // in the next num lines, save all the game objects in the scene
        int num = scene.gameObjects.size();

        file << num <<'\n';
        for (int i = 0; i < num; i++) {
            ext = SaveGameObjectToFile(&file, scene.gameObjects[i]);
            if (ext != 0) return ext;
        }

        // dimensions of the grid. write to one line, along with sideLen
        int nx = scene.grid.size(), ny = (nx>0)? scene.grid[0].size() : 0;
        file << nx <<'\t'<< ny <<'\t'<< scene.sideLen <<'\n';

        // the next nx lines will contain ny hexdex entries
        for (int i = 0; i < nx; i++) {
            for (int j = 0; j < ny; j++) {
                file << std::hex << scene.grid[i][j] <<'\t';
            }
            file << '\n';
        }

        // close the file
        file.close();

        return ext; 
    }

    // save attributes of a game object into a file
    int SaveGameObjectToFile(std::fstream *file, Object::GameObject *obj)
    {
        // validate the existence of the file and the game object
        if (!file) {
            return -1;

        } else if (obj != nullptr) { // don't try to save unless the object actually exists

            // to one line, write all information about the object
            *file << std::dec <<obj->idx <<'\t'    // write the object's index first, as a sort of ID 
                  << obj->pos.x             <<' '<< obj->pos.y                  <<'\t'  // pos
                  << obj->centrePos.x       <<' '<< obj->centrePos.y            <<'\t'  // centrePos
                  << obj->velocity.x        <<' '<< obj->velocity.y             <<'\t'  // velocity
                  << obj->acceleration.x    <<' '<< obj->acceleration.y         <<'\t'  // acceleration
                  << obj->size.x            <<' '<< obj->size.y                 <<'\t'  // size
                  << obj->cell.x            <<' '<< obj->cell.y                 <<'\t'  // cell

                  << obj->type     <<'\t'<<    obj->hp    <<' '<<   obj->maxHP  <<'\t'  // type, hp, maxHP
                  << obj->speed  <<'\t'<< obj->timer <<'\t'<< obj->hasCollision <<'\t'  // speed, timer, collision

                  << obj->cellularDimensions.X    <<' '<<obj->cellularDimensions.Y     <<' ' // cellular dimensions (x/y)
                  << obj->cellularDimensions.Width<<' '<<obj->cellularDimensions.Height<<'\t'// cellular dimensions (wid/height)

                  << obj->radius <<'\n'; // radius, endline
        }
        return 0;
    }



    // takes the information stored in a file and loads it into a level object
    Level LoadLevelObjectFromFile(std::string filename)
    {
        // std::cout << "here\n";
        std::ifstream file(filename);
        if (!file) {
            std::cout << "Couldn't open " <<filename <<'\n';
        }

        // get the level's name from the first line
        std::string lvlName;
        std::getline(file, lvlName);
        

        // get the three numbers on line 2
        // get the line of the file as a string
        std::string line;
        std::getline(file, line);
        // convert the string to istringstream
        std::istringstream iss(line);


        // the four numbers held on this line
        int idxPlayer, idxHeld, buildType;
        float time;
        // take in these values, remember that the building type is stored in hexdec
        iss >> std::dec>>idxPlayer >> idxHeld 
            >> std::hex>>buildType >>std::dec>>time;


        // get the next line of the file (should just be one int)
        std::getline(file, line);
        // convert the line to istringstream
        iss = std::istringstream(line);

        int n; // represents the number of game objects that follow this line
        iss >> std::dec>>n;
        
        // create the game objects vector of size n
        std::vector<Object::GameObject*> lvlObjects(n);
        // pointers to the player and held objects; they will be found with the rest of the objects
        Object::GameObject *player = nullptr, *held = nullptr;


        // go through the next n lines and add all the objects to the lvlObjects vector
        for (int i = 0; i < n; i++) 
        {
            std::getline(file, line);
            iss = std::istringstream(line);

            // create the object
            Object::GameObject *obj = CreateGameObjectFromFile(&iss);


            // obj will already have an index, so insert it at that index in lvlObjects
            int id = obj->idx;
            lvlObjects[id] = obj;

            if (id == idxPlayer) {
                // assign obj to the player pointer
                player = obj;

                // if possible, assign the player as the held object's owner,
                // and the held object as being owned by the player
                if (held != nullptr) {
                    held->owner = player;
                    player->ownedObjects.push_back(held);
                }

            } else if (id == idxHeld) {
                // assign obj to the held object pointer
                held = obj;

                // update the objects position script to actually be held
                held->positionFunc = Func::heldItemPositionFunc;

                // if possible, assign the player as the held object's owner,
                // and the held object as being owned by the player
                if (player != nullptr) {
                    held->owner = player;
                    player->ownedObjects.push_back(held);
                }
            }
        }
        


        // the next line has the x and y dimensions of the map (in cells), and the length of each cell
        std::getline(file, line);
        // make line into an istringstream
        iss = std::istringstream(line);

        int nx, ny, sideLen;
        iss >> nx >> ny >> sideLen;


        // the grid is nx across, and ny tall
        std::vector<std::vector<int>> grid(nx);

        // the next nx lines will have ny hexdec integers

        for (int i = 0; i < nx; i++) {
            // create the column of cell, which is ny cells tall
            std::vector<int> vec(ny);

            // read in the next line of the file and make it into an istringstream
            std::getline(file, line);
            iss = std::istringstream(line);

            // read the next ny hex integers into vec
            for (int j = 0; j < ny; j++) {
                iss >> std::hex >> vec[j];
            }

            // add vec into the grid
            grid[i] = vec;
        }

        // create a level object will all the collected attributes
        Level lvl(player, held, buildType, lvlObjects, sideLen, grid, time);

        // close the file
        file.close();

        return lvl;
    }



    // creates a game object from the line of a file, and returns a reference to it
    Object::GameObject * CreateGameObjectFromFile(std::istringstream *iss)
    {
        // read all all attributes about the object

        // all object attributes saved in iss
        int idx, hp, maxHP, type;
        float speed, timer, radius;
        Math::Vector2 pos, centrePos, velocity, acceleration;
        Math::Point2 size, cell;
        bool hasCollision;
        Gdiplus::Rect cellularDimensions;


        // read in values from iss
        *iss >> idx 
             >> pos.x            >> pos.y
             >> centrePos.x      >> centrePos.y
             >> velocity.x       >> velocity.y
             >> acceleration.x   >> acceleration.y
 
             >> size.x       >> size.y
             >> cell.x       >> cell.y
             >> type         >> hp       >> maxHP
             >> speed        >> timer    >> hasCollision
 
             >> cellularDimensions.X     >> cellularDimensions.Y 
             >> cellularDimensions.Width >> cellularDimensions.Height
 
             >> radius;
            

        Object::GameObject *obj = new Object::GameObject(pos, velocity, (Object::EntityType)type, hp, idx);
        

        // manually reassign other variables
        obj->centrePos = centrePos;
        obj->acceleration = acceleration;
        obj->maxHP = maxHP;
        obj->speed = speed;
        obj->timer = timer;
        obj->radius = radius;

        obj->size = size;
        obj->cell = cell;
        obj->hasCollision = hasCollision;

        obj->cellularDimensions = cellularDimensions;
        obj->radius = radius;

        return obj;
    }
}