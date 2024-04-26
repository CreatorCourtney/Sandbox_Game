#include "WndFunctions.hpp"

int WINAPI WndMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
{
    // FreeConsole(); // hides console

    Frame::InitialiseFrameCreation();

    // instantiate the player
    Globals::gameObjects.push_back(new Object::GameObject(
        Math::Vector2(5.0f, 5.0f), Math::Vector2(0.0f, 0.0f),
        PLAYER, 0.25f*Globals::g_scale, 75.0f*Globals::g_scale
    ));
    Globals::player = Globals::gameObjects[0];

    Globals::SetGrid();

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
        (GetSystemMetrics(SM_CXFULLSCREEN)-750)/2, (GetSystemMetrics(SM_CYFULLSCREEN)-500)/2,
        750, 500,
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
        SetCursor(cursor);
        // update DeltaTime
        Globals::deltaTime = Math::DeltaTime(&Globals::begin_time);

        for (int i = 0; i < Globals::gameObjects.size(); i++) {
            Globals::gameObjects[i]->update();
        }

        TranslateMessage(&msg);
        DispatchMessage(&msg);
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
            Globals::wndWidth = newWidth; Globals::wndHeight = newHeight;

            // recreate hOffscreenBitmap
            HBITMAP hNewBitmap = CreateCompatibleBitmap(Globals::g_hdc, newWidth, newHeight);
            if (hNewBitmap != NULL) {
                SelectObject(Globals::hOffscreenDC, hNewBitmap);
                DeleteObject(Globals::hOffscreenBitmap);
                Globals::hOffscreenBitmap = hNewBitmap;

                Globals::wndScale = Math::Vector2(
                    (float)Globals::wndWidth/GetSystemMetrics(SM_CXFULLSCREEN),
                    (float)Globals::wndHeight/GetSystemMetrics(SM_CYFULLSCREEN)
                );

                Globals::currUIScale = 
                    Math::clampf(0.5f, 1.0f, 
                    Math::minf(Globals::wndScale.x, Globals::wndScale.y))*Globals::UIScale;
            }
            break;
        }

        case WM_KEYDOWN:
            switch (wParam)
            {
                case 0x57: // w
                    Globals::inputKeys |= 8; break;
                case 0x41: // a
                    Globals::inputKeys |= 4; break;
                case 0x53: // s
                    Globals::inputKeys |= 2; break;
                case 0x44: // d
                    Globals::inputKeys |= 1; break;
                case VK_SHIFT:
                    Globals::inputKeys |= 16; break;
                case VK_ESCAPE:
                    Globals::gameIsPaused = !Globals::gameIsPaused;
                    break;
                case VK_F11:
                    ToggleFullscreen(hwnd, 750, 500); break;
            } break;

        case WM_KEYUP:
            switch (wParam)
            {
                case 0x57: // w
                    Globals::inputKeys ^= 8; break;
                case 0x41: // a
                    Globals::inputKeys ^= 4; break;
                case 0x53: // s
                    Globals::inputKeys ^= 2; break;
                case 0x44: // d
                    Globals::inputKeys ^= 1; break;
                case VK_SHIFT:
                    Globals::inputKeys ^= 16; break;
            } break;

        case WM_LBUTTONDOWN: {
            if (Globals::gameIsPaused) break;
            int x = GET_X_LPARAM(lParam), y = GET_Y_LPARAM(lParam);

            switch (Inventory::buttonPress(x, y))
            {
                case 0: {// clicked somewhere on the map
                    Math::Vector2 worldPos = Object::getWorldPosition(Math::Point2(x,y));
                    Math::Point2 cell = Object::findCell(worldPos);
                    Frame::PlaceObjectInCell(cell, Globals::selectedObj);
                    break;
                }
                case HOTBAR_1:
                    Globals::selectedObj = LOG; break;
                case HOTBAR_2:
                    Globals::selectedObj = BRIDGE; break;
                case HOTBAR_3:
                    std::cout << "slot 3\n"; break;
                case HOTBAR_4:
                    std::cout << "slot 4\n"; break;
                case HOTBAR_5:
                    std::cout << "slot 5\n"; break;
                case INVENTORY_BUTTON:
                    std::cout << "inventory\n"; break;
                default:
                    std::cout << "UI panel click\n"; break;
            } break;
        }

        case WM_RBUTTONDOWN: {
            if (Globals::gameIsPaused) break;
            int x = GET_X_LPARAM(lParam), y = GET_Y_LPARAM(lParam);

            switch (Inventory::buttonPress(x, y))
            {
                case 0: {// clicked somewhere on the map
                    Math::Vector2 worldPos = Object::getWorldPosition(Math::Point2(x,y));
                    Math::Point2 cell = Object::findCell(worldPos);
                    Frame::PlaceObjectInCell(cell, EMPTY);
                    break;
                }
                case HOTBAR_1:
                    std::cout << "slot 1\n"; break;
                case HOTBAR_2:
                    std::cout << "slot 2\n"; break;
                case HOTBAR_3:
                    std::cout << "slot 3\n"; break;
                case HOTBAR_4:
                    std::cout << "slot 4\n"; break;
                case HOTBAR_5:
                    std::cout << "slot 5\n"; break;
                case INVENTORY_BUTTON:
                    std::cout << "inventory\n"; break;
                default:
                    std::cout << "UI panel click\n"; break;
            } break;
        }

        case WM_PAINT:
            Frame::CreateBufferFrame(hwnd, Globals::g_hdc);
            CopyOffscreenToWindow(hwnd, Globals::g_hdc);
            break;

        case WM_DESTROY:
            DeleteObject(Globals::hOffscreenBitmap);
            DeleteDC(Globals::hOffscreenDC);
            ReleaseDC(hwnd, Globals::g_hdc);
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
    Globals::wndWidth = clientRect.right-clientRect.left;
    Globals::wndHeight = clientRect.bottom-clientRect.top;

    // create a compatible DC for the offscreen bitmap
    Globals::g_hdc = GetDC(hwnd);
    Globals::hOffscreenDC = CreateCompatibleDC(Globals::g_hdc);
    Globals::hOffscreenBitmap = CreateCompatibleBitmap(Globals::g_hdc, Globals::wndWidth, Globals::wndHeight);
    SelectObject(Globals::hOffscreenDC, Globals::hOffscreenBitmap);

    RECT rect = { 0, 0, Globals::wndWidth, Globals::wndHeight };
    HBRUSH bkg = CreateSolidBrush(RGB(255,255,255));
    FillRect(Globals::hOffscreenDC, &rect, bkg);
    DeleteObject(bkg); // deallocate memory for bkg brush

    Globals::wndScale = Math::Vector2(
        (float)Globals::wndWidth/GetSystemMetrics(SM_CXFULLSCREEN),
        (float)Globals::wndHeight/GetSystemMetrics(SM_CYFULLSCREEN)
    );
    Globals::currUIScale = 
        Math::clampf(0.5f, 1.0f, 
        Math::minf(Globals::wndScale.x, Globals::wndScale.y))*Globals::UIScale;
}

void CopyOffscreenToWindow(HWND hwnd, HDC hdc)
{
    BitBlt(hdc, 0, 0, Globals::wndWidth, Globals::wndHeight, Globals::hOffscreenDC, 0, 0, SRCCOPY);
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