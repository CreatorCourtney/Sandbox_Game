#include "WndFunctions.cpp"
#include "FrameCreation.cpp"
#include "Globals.cpp"
#include "GameObjects.cpp"
#include "GameMath.cpp"
#include "Inventory.cpp"

int main() {
    int ext = WndMain(
        GetModuleHandle(NULL),
        NULL,
        GetCommandLine(),
        SW_SHOWDEFAULT
    );
    return ext;
}