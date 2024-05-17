#ifndef UNICODE
#define UNICODE
#endif

#include <Windows.h>
#include <windowsx.h>
#include <WinUser.h>

#include "PlayerInput.hpp"

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

void ToggleFullscreen(HWND hwnd, int width, int height);