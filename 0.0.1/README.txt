ADDED:


Trees!!

Cells can now be initialised to the TREE type. this will create a tile that you cannot walk through, and add a tree that extends beyond the tile, drawn over everything else in the level.
This creates an effect where it looks like the objects in the scene are walking behind the trees.

Clicking on cells covered by the tree texture, but not the tree cell itself, will function normally, placing or removing a building from that cell. The building is drawn behind the tree. 
Right clicking on a tree tile (where the base of the tree is drawn) will destroy the tree, setting the cell to an empty grass tile, removing the tree texture from the screen, and gives the player 5 logs.

The way this tree removal works is currently inefficient. To draw the trees in a way that doesn't cause much lag, instead of drawing every tree to the screen individually, all of the trees are drawn onto an overlay image.
Then, this overlay is drawn onto the frame, in the same way that the background is drawn, just on top of the background and all the game objects. When a tree is removed, though, to my knowledge the Gdiplus library does not
have a fast way to remove the pixel data from one section of a bitmap object, so instead, I have to clear the ENTIRE bitmap using the Gdiplus::Graphics::Clear method, and re-draw every remaining tree back onto the overlay bitmap.
This is O(n*m) complexity, where n is number of trees and m is the size of the tree texture. Removing a tree with this current method causes a noticable lag spike for a single frame after the tree's removal, even with a
relatively small number of trees. Hopefully this won't be as bad in the final version, because in my testing, the lag spike is mostly noticable when the player destroys a tree while moving, but I intend for the player to have to
stop at a tree in order to cut in down in future builds.


Debug Tools!!

You can now activate a number of debug tools, such as info about the player's position, and enable cheats like faster movement speed and no-clip.


Abdul!!

I've recruited some help! All versions of the game after this one (0.0.1), will be the joint effort of myself and my friend :)


CHANGES:

Instead of being drawn with Gdiplus::Graphics::DrawImage, all images are now converted into Gdiplus::TextureBrush objects, and drawn using the Gdiplus::FillRectangle function, along with using Gdiplus::Matrix to make them draw in the correct positions.
For some reason this is faster than just drawing the images directly, but results in no noticable change to the user other than improved frame rate.


TO-DO:

- QOL/BUGS
  - fix low framerate when in fullscreen mode
  - make playing in fullscreen enlarge the size of the image being drawn, without giving the player more visibility of the map

  - fix bugs with object animations

- STORAGE
  - add methods to save and load data from local memory

  - find ways to save data about a level into an object, as well as in local memory.

- GAMEPLAY
  - make a system where the player can carry items around
  - rework inventory 

  - start thinking about a crafting system

  - start working on adding NPCs
    - give them pathfinding, behaviour, etc
