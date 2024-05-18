#include "WndFunctions.hpp"

using namespace Globals;

int WINAPI WndMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
{
    // FreeConsole(); // hides console

    Frame::InitialiseFrameCreation();

    // instantiate the player
    Object::Instantiate(Object::Player, Math::Vector2(400.0f, 200.0f), 5);
    player = gameObjects[0];


    // spawn wolves for testing
    for (int i = 0; i < 0; i++) {
        Object::Instantiate(Object::Wolf, Math::Vector2(500.0f, 300.0f), 3);
    }

    // spawn items for testing
    Object::Instantiate(Object::Log_Item, Math::Vector2(700.0f, 300.0f), 13);

    Object::Instantiate(Object::Log_Item, Math::Vector2(500.0f, 300.0f), 2);

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
        case WM_CREATE:
            // get device context for the window
            InitialiseOffscreenDC(hwnd);
            // draw the grid initially
            Frame::DrawWholeGrid();
            break;

        case WM_SIZE: {
            int newWidth = LOWORD(lParam), newHeight = HIWORD(lParam);
            wndWidth = newWidth; wndHeight = newHeight;

            // recreate hOffscreenBitmap to fit new screen size
            HBITMAP hNewBitmap = CreateCompatibleBitmap(g_hdc, newWidth, newHeight);
            if (hNewBitmap != NULL) {
                // create a new DC for the newly sized window
                SelectObject(hOffscreenDC, hNewBitmap);
                DeleteObject(hOffscreenBitmap);
                hOffscreenBitmap = hNewBitmap;

                wndScale = Math::Vector2(
                    (float)wndWidth/GetSystemMetrics(SM_CXFULLSCREEN),
                    (float)wndHeight/GetSystemMetrics(SM_CYFULLSCREEN)
                );

                currUIScale = 
                    Math::clampf(0.5f, 1.0f, 
                    Math::minf(wndScale.x, wndScale.y))*UIScale;
            }
            break;
        }

        case WM_KEYDOWN:
            // enable bits
            switch (wParam)
            {
                case 0x57: // w
                    inputKeys |= 8; break;
                case 0x41: // a
                    inputKeys |= 4; break;
                case 0x53: // s
                    inputKeys |= 2; break;
                case 0x44: // d
                    inputKeys |= 1; break;
                case VK_SHIFT:
                    inputKeys |= 16; break;
                case VK_ESCAPE:
                    gameIsPaused = !gameIsPaused;
                    break;
                case VK_F11:
                    ToggleFullscreen(hwnd, viewRect.Width, viewRect.Height); 
                    break;
                case VK_F9:
                    debuggingTools ^= 1; break;
                case VK_F8:
                    debuggingTools ^= 2; break;
                case VK_F7:
                    player->hasCollision = !player->hasCollision;
                    break;
                case VK_F6:
                    debuggingTools ^= 4; break;
            } 
            break;

        case WM_KEYUP:
            // disable bits
            switch (wParam)
            {
                case 0x57: // w
                    inputKeys &= ~8; break;
                case 0x41: // a
                    inputKeys &= ~4; break;
                case 0x53: // s
                    inputKeys &= ~2; break;
                case 0x44: // d
                    inputKeys &= ~1; break;
                case VK_SHIFT:
                    inputKeys &= ~16; break;
            } 
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

        case WM_DESTROY:
            DeleteObject(hOffscreenBitmap);
            DeleteDC(hOffscreenDC);
            ReleaseDC(hwnd, g_hdc);
            PostQuitMessage(0);
            break;

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

    wndScale = Math::Vector2(
        (float)wndWidth/GetSystemMetrics(SM_CXFULLSCREEN),
        (float)wndHeight/GetSystemMetrics(SM_CYFULLSCREEN)
    );
    currUIScale = 
        Math::clampf(0.5f, 1.0f, 
        Math::minf(wndScale.x, wndScale.y))*UIScale;
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