/*
 * Tray.cpp
 *
 *  Created on: Oct 12, 2024
 *      Author: lion
 */

#ifdef _WIN32

#include "object.h"
#include "ifs/gui.h"
#include "Tray.h"
#include "EventInfo.h"

namespace fibjs {

static int32_t s_uid = 1;

const wchar_t* szWndClassTray = L"fibjs_tray";
#define WM_TRAY_EVENT (WM_USER + 1001)

void on_click_menu(uint32_t id);
HICON LoadPngIcon(const BYTE* pngData, size_t pngSize);

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    if (uMsg == WM_TRAY_EVENT) {
        if (lParam == WM_LBUTTONUP) {
            Tray* _tray = reinterpret_cast<Tray*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
            if (_tray) {
                if (_tray->m_options->menu.has_value()) {
                    POINT pt;
                    GetCursorPos(&pt);
                    SetForegroundWindow(hwnd);

                    HMENU menu = (HMENU)_tray->m_options->menu.value()->create_os_menu();
                    TrackPopupMenu(menu, TPM_RIGHTBUTTON, pt.x, pt.y, 0, hwnd, NULL);
                }
            }
        }
    } else if (uMsg == WM_COMMAND) {
        on_click_menu(wParam);
    }

    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

void Tray::createTray()
{
    static bool s_inited = false;

    HINSTANCE hInstance = GetModuleHandle(NULL);

    if (!s_inited) {
        WNDCLASSW wc = {};
        wc.cbWndExtra = sizeof(void*);
        wc.hbrBackground = (HBRUSH)COLOR_WINDOW;
        wc.hCursor = LoadCursor(NULL, IDC_ARROW);
        wc.hIcon = nullptr;
        wc.hInstance = hInstance;
        wc.lpfnWndProc = WindowProc;
        wc.lpszClassName = szWndClassTray;

        RegisterClassW(&wc);
    }

    HWND hWnd = CreateWindowExW(0, szWndClassTray, L"", 0,
        CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);
    SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));

    NOTIFYICONDATA* nid = new NOTIFYICONDATA();
    m_tray = nid;

    nid->cbSize = sizeof(NOTIFYICONDATA);
    nid->hWnd = hWnd;
    nid->uID = s_uid++;
    nid->uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
    nid->uCallbackMessage = WM_TRAY_EVENT;
    nid->hIcon = LoadPngIcon(m_icon->data(), m_icon->length());
    wcscpy_s(nid->szTip, L"fibjs");

    Shell_NotifyIcon(NIM_ADD, nid);
}

result_t Tray::close(AsyncEvent* ac)
{
    result_t hr = check_status(ac);
    if (hr < 0)
        return hr;

    NOTIFYICONDATA* _tray = (NOTIFYICONDATA*)m_tray;
    CloseWindow(_tray->hWnd);
    Shell_NotifyIcon(NIM_DELETE, _tray);

    release();

    return 0;
}

} // namespace fibjs

#endif // _WIN32