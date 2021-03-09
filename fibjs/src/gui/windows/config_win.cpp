/*
 * config_win.cpp
 *
 *  Created on: Feb 26, 2021
 *      Author: lion
 */

#include "object.h"
#include "EventInfo.h"
#include "ifs/WebView.h"

#ifdef WIN32
#include <commctrl.h>

namespace fibjs {

void GetDPI(HWND hWndParent, int* dpix, int* dpiy)
{
    HDC hdc = ::GetDC(hWndParent);
    *dpix = ::GetDeviceCaps(hdc, LOGPIXELSX);
    *dpiy = ::GetDeviceCaps(hdc, LOGPIXELSY);
    ::ReleaseDC(hWndParent, hdc);
}

LRESULT CALLBACK mySubClassProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData)
{
    WebView_base* webView1 = (WebView_base*)dwRefData;
    switch (uMsg) {
    case WM_MOVE: {
        RECT rcWin;
        GetWindowRect(hWnd, &rcWin);

        int dpix = 0, dpiy = 0;
        GetDPI(hWnd, &dpix, &dpiy);

        obj_ptr<EventInfo> ei = new EventInfo(webView1, "move");
        ei->add("left", (int32_t)rcWin.left * 96 / dpix);
        ei->add("top", (int32_t)rcWin.top * 96 / dpiy);

        webView1->_emit("move", ei);
        break;
    }
    case WM_SIZE: {
        RECT rcWin;
        GetWindowRect(hWnd, &rcWin);

        int dpix = 0, dpiy = 0;
        GetDPI(hWnd, &dpix, &dpiy);

        obj_ptr<EventInfo> ei = new EventInfo(webView1, "resize");
        ei->add("width", (int32_t)(rcWin.right - rcWin.left) * 96 / dpix);
        ei->add("height", (int32_t)(rcWin.bottom - rcWin.top) * 96 / dpiy);

        webView1->_emit("resize", ei);
        break;
    }
    case WM_CLOSE:
        if (webView1->close(NULL))
            DestroyWindow(hWnd);
        return 0;
    case WM_DESTROY:
        webView1->_emit("closed");
        webView1->holder()->Unref();
        webView1->Unref();
        break;
    }

    return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}

void os_config_window(WebView_base* webview, void* _window, NObject* opt)
{
    HWND hWndParent = (HWND)_window;
    DWORD dwStyle = WS_OVERLAPPED;
    int x = CW_USEDEFAULT;
    int y = CW_USEDEFAULT;
    int nWidth = CW_USEDEFAULT;
    int nHeight = CW_USEDEFAULT;
    bool _fullscreen = false;

    if (opt) {
        Variant v;

        if (opt->get("left", v) == 0)
            x = v.intVal();
        if (opt->get("top", v) == 0)
            y = v.intVal();
        if (opt->get("width", v) == 0)
            nWidth = v.intVal();
        if (opt->get("height", v) == 0)
            nHeight = v.intVal();

        if (!(opt->get("border", v) == 0 && !v.boolVal())) {
            dwStyle |= WS_BORDER | WS_THICKFRAME;

            if (!(opt->get("caption", v) == 0 && !v.boolVal()))
                dwStyle |= WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX;

            if (opt->get("resizable", v) == 0 && !v.boolVal()) {
                dwStyle &= ~(WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX);

                HMENU hSysMenu = GetSystemMenu(hWndParent, FALSE);
                RemoveMenu(hSysMenu, SC_RESTORE, MF_BYCOMMAND);
                RemoveMenu(hSysMenu, SC_MAXIMIZE, MF_BYCOMMAND);
                RemoveMenu(hSysMenu, SC_MINIMIZE, MF_BYCOMMAND);
                RemoveMenu(hSysMenu, SC_SIZE, MF_BYCOMMAND);
            }
        }

        if (opt->get("maximize", v) == 0 && v.boolVal())
            dwStyle |= WS_MAXIMIZE;

        if (opt->get("fullscreen", v) == 0 && v.boolVal())
            _fullscreen = true;
    } else
        dwStyle = WS_OVERLAPPEDWINDOW;

    int dpix = 0, dpiy = 0;
    GetDPI(hWndParent, &dpix, &dpiy);

    RECT actualDesktop;
    GetWindowRect(GetDesktopWindow(), &actualDesktop);

    if (nWidth != CW_USEDEFAULT)
        nWidth = nWidth * dpix / 96;
    else
        nWidth = actualDesktop.right * 3 / 4;

    if (nHeight != CW_USEDEFAULT)
        nHeight = nHeight * dpix / 96;
    else
        nHeight = actualDesktop.bottom * 3 / 4;

    if (x != CW_USEDEFAULT)
        x = x * dpix / 96;
    else
        x = (actualDesktop.right - nWidth) / 2;

    if (y != CW_USEDEFAULT)
        y = y * dpix / 96;
    else
        y = (actualDesktop.bottom - nHeight) / 2;

    if (_fullscreen) {
        dwStyle = WS_OVERLAPPED;
        SetWindowLong(hWndParent, GWL_STYLE, dwStyle);
        SetWindowPos(hWndParent, HWND_TOP, 0, 0, actualDesktop.right, actualDesktop.bottom, 0);
    } else {
        SetWindowLong(hWndParent, GWL_STYLE, dwStyle);
        SetWindowPos(hWndParent, HWND_TOP, x, y, nWidth, nHeight, 0);
    }

    dwStyle |= WS_VISIBLE;
    SetWindowLong(hWndParent, GWL_STYLE, dwStyle);

    SetWindowSubclass(hWndParent, &mySubClassProc, 1, (DWORD_PTR)webview);
    webview->Ref();
    webview->_emit("open");
}
}

#endif