#include "WndMain.hpp"

using namespace Globals;

int WINAPI WndMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
{
    // FreeConsole(); // hides console

    Frame::InitialiseFrameCreation();

    // load all the images used
    int ext = loadImages();
    if (ext != 0) return ext;


    // window's background colour
    HBRUSH bkg = CreateSolidBrush(RGB(100,100,100));

    // register the window class
    const wchar_t CLASS_NAME[] = L"Window Class";
    WNDCLASS wc = { };
    wc.lpfnWndProc   = WndProc;
    wc.hInstance     = hInstance;
    wc.lpszClassName = CLASS_NAME;
    wc.hbrBackground = bkg;
    RegisterClass(&wc);

    // create the window
    HWND hwnd = CreateWindowEx(
        0,                      // optional window styles
        CLASS_NAME,             // window class
        L"Level Editor",   // window text
        WS_OVERLAPPEDWINDOW,    // window style
        // position and size
        (GetSystemMetrics(SM_CXFULLSCREEN)-wndRect.Width)/2, 
        (GetSystemMetrics(SM_CYFULLSCREEN)-wndRect.Height)/2,
        wndRect.Width, wndRect.Height,
        NULL,       // parent window
        NULL,       // menu
        hInstance,  // instance handle
        NULL        // additional application data
    );
    if (hwnd == NULL) return 1;

    ShowWindow(hwnd, nCmdShow);

    HCURSOR cursor = LoadCursor(NULL, IDC_ARROW);

    // main message loop
    MSG msg = { };
    while (GetMessage(&msg, NULL, 0, 0) > 0)
    {
        // sets the cursor
        SetCursor(cursor);
        
        // update DeltaTime
        deltaTime = Math::DeltaTime(&begin_time); 


        TranslateMessage(&msg);
        DispatchMessage(&msg);

        // WM_PAINT (in WndProc) occurs every iteration of this loop
    }

    // cleanup
    Frame::ShutDownFrameCreation();
    DeleteObject(bkg);

    return 0;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
        case WM_CREATE: {
            // get device context for the window
            InitialiseOffscreenDC(hwnd);

            // set the placement of the map
            mapRect = {50, topMenuHeight+50, (5*wndWidth/6)-100, wndHeight-topMenuHeight-100};

            // create all the button objects
            createAllButtons();

            // set the size of the grid and draw the map for the first time
            resizeGrid();

            break;
        }

        case WM_KEYDOWN:
            Input::keyPressFunc(wParam, hwnd);
            break;

        case WM_KEYUP:
            Input::keyReleaseFunc(wParam);
            break;

        case WM_LBUTTONDOWN: {
            int x = GET_X_LPARAM(lParam), y = GET_Y_LPARAM(lParam);
            Input::leftClickFunc(x, y);
            break;
        }

        case WM_RBUTTONDOWN: {
            int x = GET_X_LPARAM(lParam), y = GET_Y_LPARAM(lParam);
            Input::rightClickFunc(x, y);
            break;
        }

        case WM_PAINT:
            Frame::CreateBufferFrame(hwnd, g_hdc);
            CopyOffscreenToWindow(hwnd, g_hdc);
            break;

        case WM_DESTROY: {
            // cleanup
            DestroyAllResources(hwnd);
            PostQuitMessage(0);
            break;
        }

        default:
            return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
    return 0;
}



void InitialiseOffscreenDC(HWND hwnd)
{
    // get dimensions of the visible window
    RECT clientRect;
    GetClientRect(hwnd, &clientRect);
    wndWidth = clientRect.right-clientRect.left;
    wndHeight = clientRect.bottom-clientRect.top;

    // create a compatible DC for the offscreen bitmap
    g_hdc = GetDC(hwnd);
    hOffscreenDC = CreateCompatibleDC(g_hdc);
    hOffscreenBitmap = CreateCompatibleBitmap(g_hdc, wndWidth, wndHeight);
    SelectObject(hOffscreenDC, hOffscreenBitmap);

    RECT rect = { 0, 0, wndWidth, wndHeight };
    HBRUSH bkg = CreateSolidBrush(RGB(100,100,100));
    FillRect(hOffscreenDC, &rect, bkg);
    DeleteObject(bkg); // deallocate memory for bkg brush
}

void CopyOffscreenToWindow(HWND hwnd, HDC hdc)
{
    BitBlt(hdc, 0, 0, wndWidth, wndHeight, hOffscreenDC, 0, 0, SRCCOPY);
}

// deallocates all resources used by the game
void DestroyAllResources(HWND hwnd)
{
    // destroy all the image objects and bitmaps
    delete logImg; delete bridgeImg; delete waterImg; delete emptyImg;
    delete hotbarImg; delete treeImg; delete wolfImg; 
    delete falling_treeImg; delete stumpImg; delete Pine_ConeImg;
    delete saplingImg; delete shoreline0Img; delete shoreline1Img;
    delete shoreline2Img; delete shoreline3Img; delete shoreline4Img;
    delete playerImg; delete ObjectsButtonImg;

    delete mapOverlay; delete mapBackground;

    // destroy all the texture brushes
    delete logBrush; delete bridgeBrush; delete waterBrush; delete grassBrush;
    delete hotbarBrush; delete treeBrush; delete wolfBrush;
    delete falling_treeBrush; delete stumpBrush; delete Pine_ConeBrush;
    delete saplingBrush; delete shoreline0Brush; delete shoreline1Brush;
    delete shoreline2Brush; delete shoreline3Brush; delete shoreline4Brush;
    delete playerBrush; delete ObjectsButtonBrush;

    delete bkgBrush; delete accent0Brush; delete accent1Brush;
    delete mapBkgBrush; delete mapOverlayBrush;

    // make sure all the entity/button textures are deleted
    for (int i = 0; i < entities.size(); i++) delete entities[i]->brush;
    for (int i = 0; i < buttons.size(); i++) delete buttons[i]->brush;

    // destroy additional bitmaps and DCs
    DeleteObject(hOffscreenBitmap);
    DeleteDC(hOffscreenDC);
    ReleaseDC(hwnd, g_hdc);
}