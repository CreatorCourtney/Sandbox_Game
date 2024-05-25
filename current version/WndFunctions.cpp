#include "WndFunctions.hpp"

using namespace Globals;

int WINAPI WndMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
{
    // FreeConsole(); // hides console
    // set the random seed to the current time, so RNG rolls aren't predictable
    srand(time(NULL));

    Frame::InitialiseFrameCreation();

    HBRUSH bkg = CreateSolidBrush(RGB(255,255,255));


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
        L"WIP viddy game :)",   // window text
        WS_OVERLAPPEDWINDOW,    // window style
        // position and size
        (GetSystemMetrics(SM_CXFULLSCREEN)-viewRect.Width)/2, 
        (GetSystemMetrics(SM_CYFULLSCREEN)-viewRect.Height)/2,
        viewRect.Width, viewRect.Height,
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

        // update all current game objects
        for (int i = 0; i < gameObjects.size(); i++) {
            gameObjects[i]->update();
        }

        // update any timed cells
        updateTimedCells();

        // update the global time
        g_time += deltaTime;

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
            // load the world from local storage
            Storage::Level lvl = Storage::LoadLevelObjectFromFile("data/Default Level.txt");
            LoadSceneFromLevelObject(lvl);

            // get device context for the window
            InitialiseOffscreenDC(hwnd);
            // draw the grid initially
            Frame::DrawWholeGrid();
            break;
        }

        case WM_SIZE: {
            // update global window dimensions
            int newWidth = LOWORD(lParam), newHeight = HIWORD(lParam);
            wndWidth = newWidth; wndHeight = newHeight;

            // recreate hOffscreenBitmap to fit new screen size
            HBITMAP hNewBitmap = CreateCompatibleBitmap(g_hdc, newWidth, newHeight);
            if (hNewBitmap != NULL) {
                // create a new DC for the newly sized window
                SelectObject(hOffscreenDC, hNewBitmap);
                DeleteObject(hOffscreenBitmap);
                hOffscreenBitmap = hNewBitmap;
            }
            break;
        }

        case WM_KEYDOWN:
            Input::keyPressFunc(wParam, hwnd);
            break;

        case WM_KEYUP:
            Input::keyReleaseFunc(wParam);
            break;

        case WM_LBUTTONDOWN: {
            if (gameIsPaused) break;
            
            int x = GET_X_LPARAM(lParam), y = GET_Y_LPARAM(lParam);
            Input::leftClickFunc(x, y);
            break;
        }

        case WM_RBUTTONDOWN: {
            if (gameIsPaused) break;

            int x = GET_X_LPARAM(lParam), y = GET_Y_LPARAM(lParam);
            Input::rightClickFunc(x, y);
            break;
        }

        case WM_MOUSEMOVE: {
            // position of the mouse in the window
            int x = GET_X_LPARAM(lParam), y = GET_Y_LPARAM(lParam);
            // update the global variable storing this position
            mousePos = Math::Point2(x, y);
            
            break;
        }

        case WM_PAINT:
            Frame::CreateBufferFrame(hwnd, g_hdc);
            CopyOffscreenToWindow(hwnd, g_hdc);
            break;

        case WM_DESTROY: {
            Storage::Level lvl = SaveSceneToLevelObject();
            int ext = Storage::SaveSceneToFile(lvl); 

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
    HBRUSH bkg = CreateSolidBrush(RGB(255,255,255));
    FillRect(hOffscreenDC, &rect, bkg);
    DeleteObject(bkg); // deallocate memory for bkg brush
}

void CopyOffscreenToWindow(HWND hwnd, HDC hdc)
{
    BitBlt(hdc, 0, 0, wndWidth, wndHeight, hOffscreenDC, 0, 0, SRCCOPY);
}

void ToggleFullscreen(HWND hwnd, int width, int height)
{
    // get current window style
    DWORD dwStyle = GetWindowLong(hwnd, GWL_STYLE);

    // toggle between fullscreen and windowed mode
    if (dwStyle & WS_OVERLAPPEDWINDOW) {
        // switch to fullscreen
        MONITORINFO monitorInfo;
        monitorInfo.cbSize = sizeof(MONITORINFO);
        GetMonitorInfo(MonitorFromWindow(hwnd, MONITOR_DEFAULTTOPRIMARY), &monitorInfo);

        SetWindowLong(hwnd, GWL_STYLE, dwStyle & ~WS_OVERLAPPEDWINDOW);
        SetWindowPos(hwnd, HWND_TOP, 
        monitorInfo.rcMonitor.left, monitorInfo.rcMonitor.top,
        monitorInfo.rcMonitor.right-monitorInfo.rcMonitor.left,
        monitorInfo.rcMonitor.bottom-monitorInfo.rcMonitor.top,
        SWP_NOZORDER | SWP_FRAMECHANGED);
    } else {
        // switch to windowed mode
        int x = (GetSystemMetrics(SM_CXSCREEN)-width)/2,
            y = (GetSystemMetrics(SM_CYSCREEN)-height)/2;
        SetWindowLong(hwnd, GWL_STYLE, dwStyle | WS_OVERLAPPEDWINDOW);
        SetWindowPos(hwnd, HWND_TOP, x, y, width, height, 
        SWP_NOZORDER | SWP_FRAMECHANGED);
    }
}



// deallocates all resources used by the game
void DestroyAllResources(HWND hwnd)
{
    // destroy all the game objects
    while (gameObjects.size() > 0) {
        // destroys the first game object until there are none left
        Destroy(gameObjects[0]);
    }

    // make sure the player and held object globals are destroyed
    Destroy(player); Destroy(heldObject);

    // destroy all the texture brushes in the animations objects
    // playerAnimations
    for (int i = 0; i < playerAnimations.front.size(); i++) {
        delete playerAnimations.front[i];
    }
    for (int i = 0; i < playerAnimations.back.size(); i++) {
        delete playerAnimations.back[i];
    }
    for (int i = 0; i < playerAnimations.left.size(); i++) {
        delete playerAnimations.left[i];
    }
    for (int i = 0; i < playerAnimations.right.size(); i++) {
        delete playerAnimations.right[i];
    }

    // destroy all the image objects and bitmaps
    delete logImg; delete bridgeImg; delete waterImg; delete emptyImg;
    delete hotbarImg; delete treeImg; delete wolfImg; 
    delete falling_treeImg; delete stumpImg; delete Pine_ConeImg;
    delete saplingImg; delete shoreline0Img; delete shoreline1Img;
    delete shoreline2Img; delete shoreline3Img; delete shoreline4Img;

    delete overlay; delete background;

    // destroy all the texture brushes
    delete logBrush; delete bridgeBrush; delete waterBrush; delete grassBrush;
    delete hotbarBrush; delete treeBrush; delete wolfBrush;
    delete falling_treeBrush; delete stumpBrush; delete Pine_ConeBrush;
    delete saplingBrush; delete shoreline0Brush; delete shoreline1Brush;
    delete shoreline2Brush; delete shoreline3Brush; delete shoreline4Brush;

    delete bkgBrush; delete overlayBrush;


    // destroy additional bitmaps and DCs
    DeleteObject(hOffscreenBitmap);
    DeleteDC(hOffscreenDC);
    ReleaseDC(hwnd, g_hdc);
}