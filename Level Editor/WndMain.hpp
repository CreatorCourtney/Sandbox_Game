#ifndef UNICODE
#define UNICODE
#endif

#include "UserInput.hpp"
#include "FrameCreation.hpp"

#include <Windows.h>
#include <windowsx.h>
#include <WinUser.h>

// main window display function
int WINAPI WndMain(
    HINSTANCE hInstance,
    HINSTANCE hPrevInstance,
    PWSTR pCmdLine,
    int nCmdShow
);
// window procedure
LRESULT CALLBACK WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
// offscreen
void InitialiseOffscreenDC(HWND hwnd);
void CopyOffscreenToWindow(HWND hwnd, HDC hdc);

// deallocates all resources used by the app
void DestroyAllResources(HWND hwnd);