ADDED:

- crafting!!
  - when right clicking an item stack, depending on the type of item, it may be crafted into a different item. alternatively, throwing two item stacks into each other has the potential to produce a stack of a third, craftable item.

  - the current crafting recipes are:
    - 1 log -> 2 planks
    - 1 bridge -> 2 logs + 4 planks
    - 2 logs + 4 planks -> 1 bridge

  - side note, this also marks the addition of plank items, currently the only type that cannot be placed as a building, an item entity for bridges, as well as the reintroduction of the ability to place bridges


- local storage!!
  - when you open the game, it will now load levels form locally stored .txt files, instead of creating the map when compiling
  - every 5 minutes, the game autosaves, and any changes you made will be saved to the same .txt files


- world border building types!!
  - surrounding the map, there are cells filled with a black tile, these are world borders
  - they cannot be removed, build over, or moved through


- level editor!!
  - this isn't really part of the game itself, just something saved in a seperate folder on this github, which allows me to create levels much easier
  - it's pretty unpolished, but I think it's fine for something made from scratch in 2 days. plus, i'd honestly much rather focus on the game itself than polishing the level editor
  - I probably won't post any updates on the level editor, even if I do continue to work on it


- different levels!!
  - now, when you walk off the edge of the map (through intentionally placed gaps in the world border), you will be transported into a different scene
  - currently, the only scenes are a main base area, and a level I'm using for testing things like pathfinding, but i do have a handful of levels planned
  - aside from walking between levels yourself, you may move other entities, such as items across levels, as well.


- pathfinding!!
  - instead of walking directly towards a the player, enemies (currently just wolves), will use the A* pathfinding algorithm to find the best path to the player, avoiding walls

  - the current algorithm differs from standard A* in the following ways:
    - if there is no valid path to the player (such as if they surround themselves with walls on all sides), pathfind to the best cell that can be reached
    - do not consider diagonal movements, as doing so would lead to enemies trying to walk through spaces they cannot, or trying to round corners directly, instead of walking around a wall
      - this works as intended, and prevents entities from trying to move through impossible to pass barriers, and makes them better at rounding corners
      - the main downside to this is that it causes inefficient and awkward movement through unobstructed areas. imagine a rook trying to cross a chessboard diagonally, as a bishop would

  - wolves now move by pathfinding to the player, walking to the next cell in their path, then pathfinding again.
    - occasionally, entities will interfere with each other's pathfinding by pushing each other around. this can sometimes cause entities to walk into a wall while trying to reach the next node in their pathfinding, and never end up reaching it
    - to handle this, wolves will automatically pathfind to the player every three seconds, which stops wolves from soft locking themselves in this way.
      - though, this has the side effect of sometimes causing strange and unintended movement in wolves that would otherwise be moving as intended, every three seconds.


- day/night!!

  - every 5 minutes, when the game autosaves, it will transition from day into night or vice versa.
  - currently, this doesn't really do anything other than make the scene darker during night, but it will be important in the future!


                                               
CHANGES:

- only 5 logs may be added to a stack. Although it isn't really important NOW, in the future, i want to make sure players can't easily spam building and projectiles without thinking, by moving huge 15 stacks of the most abundant resource in the game

- the max stack size for pine cones was raised from 5 to 6. this was done because i anticipate players wanting to plant trees in 2 sapling wide rows, as this formation gives player full access to all trees they plant, while still maintaining high density.
  to encourage intelligent tree placement like this, i decided that players should be able to carry an even number of pine cones; planting trees in this way while only being able to hold an odd number of seeds would just be frustrating



KNOWN ISSUES:

- Memory leaking with pathfinding:
  - when running the pathfinding algorithm, a bunch of pointers to 'Node' objects are created, but not destroyed, causing memory leaks
    - the leak isn't huge, since the node objects just hold five ints and a pointer to another node, (so like 20 bytes total), but it's something that needs to be fixed
    - the pathfinding class DOES have a 'close()' function, but when I call it to clean everything up, it interferes heavily with the wolves pathfinding. 
      - interestingly, when there is only one wolf pathfinding, the close() function may be called without consequence. When more wolves are spawned, their pathfinding gets messed up in a very unpredictable way, but this doesn't affect all pathfinding entities;
        even with multiple wolves, when the close() function is called to clean up unused pointers, one wolf pathfinds as intended, without error, while the rest get messed up

  - i don't quite understand this, nor do I know how to fix it as of right now. That being said, to prevent memory leaking, it is something that NEEDS to be fixed
