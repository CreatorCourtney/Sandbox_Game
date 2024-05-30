// include source code
#include "WndMain.cpp"
#include "UserInput.cpp"
#include "FrameCreation.cpp"
#include "Globals.cpp"
#include "GameMath.cpp"
#include "Buttons.cpp"
#include "Storage.cpp"

// open the exe
int main() {
    int ext = WndMain(
        GetModuleHandle(NULL),
        NULL,
        GetCommandLine(),
        SW_SHOWDEFAULT
    );
    return ext;
}

/*
    KNOWN ISSUES:

    - shorelines not drawing properly

    - the bottoms of trees not being rendered at greater y values

    - trees being drawn in the opposite order they're placed, sometimes causing unnatural overlap issues

    - lack of arguably necesary features, entity/tile health, in fact a complete inability
      to modify entity attributes aside from position and type.

    - inability to change file name/destination

    - lack of many regular QoL features, such as the ability to load a level from file

    - general lack of polish and aesthetic
*/