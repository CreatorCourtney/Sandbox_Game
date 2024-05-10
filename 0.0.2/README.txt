VERSION 0.0.2

ADDED:

Wolves!!

- The first NPC type!! currently you can spawn as many as you want by just adding a new pointer to the gameObjects vector
- In their current state, wolves just walk directly towards the player. This is pretty stupid, so we're going to look into the A* pathfinding algorithm for these wolves. 
  - A* is a pathfinding algorithm designed to be applicable to worlds divided into square cells, and this world is already divided into square cells for building, so A* seemed like a natural choice


Entity Collisions!!

- Game Objects will now collide with each other
  - Unlike walls, which detect collisions by seeing if the corners of the entity are within a barrier cell, these collisions work by having each game object store a 'radius' value.
  - Whenever the distance between two game objects is less than the sum of their radii, there is a collision
  - Upon colliding, currently both game objects are moved apart by half the amount that they overlap, but in the future more complicated collision behaviour may be implemented
  - We were worried that using circles for collision detection when walls use sqaured would seem unnatural, but for the time being, this method actually seems natural in practice, as it allows entities to push each other aside as the move in more dynamic ways

- Currently, this collision detection function is O(n^2), and has all entities check collision with all other entities
  - This is inefficient and may be rectified in the future, however, for the time being this system seems fine. 
  - In testing, as many as 20 wolves were spawned with literally zero noticable drop in FPS, so unless we add gameplay with literally hundreds of NPCs, this method will work fine for now


Centre Positions!!

- Game Objects now store a coordinate representing the centre of their hitbox.
  - The original 'pos' members are still used widely in the code, especially for drawing the game object
  - The 'centrePos' member allows for more dynamic behaviour, for instance, before its addition, wolves would chase the player by trying to have their top left corner meet the player's top left corner, as the 'pos' member represent's the position of a game object's top left corner
    The addition of centrePos allows the wolves to chase the player in a more natural way
  - This also makes the collision detection between entities simpler


Debugging tools!!

- In the debugging menu, you can now press F6 to toggle entity hitboxes.
  - The blue box is just an outline of the entity's bounding box
  - The red dots represent points where the game checks if the entity is colliding with a wall (see changes to wall collisions)
  - The blue dot represents the position of the entity's centre
  - The green circle is the radius used for collisions with other game objects
  - The green line visualises the entity's velocity

- The debug menu now shows counters for how many entities are loaded, as well as how many objects are drawn to the overlay bitmap

- This is a currently unused debugging function that will highlight a specified cell. Hopefully this will make debugging entity pathfinding easier, when implemented


Added EntityFunctions.hpp/cpp files, which will store functions for entity behaviour, such as pathfinding algorithms. This will hopefully make the code in GameObjects.cpp more readable



CHANGES/BUG FIXES:

Changed wall collisions + Fixed a bug where entities bigger than one cell could walk through some barriers

- This was caused by the fact that the collision detection only checked if a gameObject's corners intersected a barrier tile. For entities who were wider than one tile, though, a one tile wide wall could fit in between the corners of it's hitbox, allowing it to walk through the wall
- This was fixed by adding wall collision checks along the sides of entities. the number of points that need to be checked along a given side is equal to n+1, where n is the the number of cells in length of that side.
- For instance, if a game object is two cells wide, then for the top and bottom sides, wall collision checks need to be performed at the two corners associated with each side, as well as at one point in the middle of the hitbox edge
- This becomes easier to understand when observing the red points on the debug hitboxes


Patched an oversight where the player's diagonal movement was faster than their orthogonal movement by a factor of sqrt(2).

- This was caused by the player inheriting the full vertical AND horizontal velocity if you held down two movement keys. For instance, holding down S and D would make the player's velocity (1, 1)*speed.
- Patched out by just normalising the velocity vector before multiplying by speed


Fixed a bug where normalising a vector with Zero magnitude caused both terms of the vector to be infinity

- To normalise a vector, all terms are divided by the vector's magnitude. Issues arise then the vector's magnitude is zero.
- When dividing by 0.0f, floating point numbers have a special binary representation for infinity
- This issue led to wolves having infinite velocity if they shared the player's exact position, as they would try to create a unit vector from the zero magnitude diplacement vector
- The bug was fixed by returning a zero vector when trying to normalise a zero vector


The debug menu position now display's the position at the centre of the player's hitbox, rather than their top left corner. The cell display also shows the cell the player's centre is in


KNOWN ISSUES:

- Currently nothing happens when the wolf reaches the player. They just walk towards you to do nothing

- Wolves are very stupid and have no real pathfinding

- Wolves have no animations. We were just lazy, they will be added later

- Wolves are very stupid, as described in the added section

- Fullscreen mode is slow, buggy, and allows the player to see a greater region of the map than windowed mode
