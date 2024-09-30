/*
 * config_win.cpp
 *
 *  Created on: Feb 26, 2021
 *      Author: lion
 */

#ifdef _WIN32

#include <uv/include/uv.h>
#include <windows.h>
#include <wrl.h>
#include "loader/WebView2.h"

#include "object.h"
#include "EventInfo.h"
#include "WebView.h"

#include <commctrl.h>


namespace fibjs {

const wchar_t* szWndClassMain = L"fibjs_window";
ICoreWebView2Environment* g_env = nullptr;

static exlib::LockedList<AsyncEvent> s_uiPool;
static uint32_t s_thread;

void putGuiPool(AsyncEvent* ac)
{
    s_uiPool.putTail(ac);
    PostThreadMessage(s_thread, WM_USER + 1000, 0, 0);
}

static void RegMainClass()
{
    HINSTANCE hInstance = GetModuleHandle(NULL);

    WNDCLASSW wc = {};
    wc.cbWndExtra = sizeof(void*);
    wc.hbrBackground = (HBRUSH)COLOR_WINDOW;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(1));
    wc.hInstance = hInstance;
    wc.lpfnWndProc = DefWindowProc;
    wc.lpszClassName = szWndClassMain;

    RegisterClassW(&wc);
}

void WebView::run_os_gui(exlib::Event& gui_ready)
{
    exlib::OSThread* _thGUI = exlib::OSThread::current();
    s_thread = _thGUI->thread_id;

    SetProcessDPIAware();
    RegMainClass();

    OleInitialize(NULL);

    HRESULT hr = CreateCoreWebView2EnvironmentWithOptions(nullptr, nullptr, nullptr,
        Microsoft::WRL::Callback<ICoreWebView2CreateCoreWebView2EnvironmentCompletedHandler>(
            [](HRESULT result, ICoreWebView2Environment* env) -> HRESULT {
                if (FAILED(result)) {
                    printf("Failed to create WebView2 environment. Error: 0x%08X\n", result);
                    exit(-1);
                }

                g_env = env;
                g_env->AddRef();

                return S_OK;
            })
            .Get());
    if (FAILED(hr)) {
        printf("Failed to create WebView2 environment. Error: 0x%08X\n", hr);
        CoUninitialize();
        exit(-1);
    }

    gui_ready.set();

    while (true) {
        MSG msg;

        while (true) {
            bool has_event = false;

            while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
                has_event = true;
            }

            AsyncEvent* p = s_uiPool.getHead();
            if (p) {
                p->invoke();
                has_event = true;
            }

            if (!has_event)
                break;
        }

        GetMessage(&msg, NULL, 0, 0);

        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
}

void GetDPI(HWND hWndParent, int* dpix, int* dpiy)
{
    HDC hdc = ::GetDC(hWndParent);
    *dpix = ::GetDeviceCaps(hdc, LOGPIXELSX);
    *dpiy = ::GetDeviceCaps(hdc, LOGPIXELSY);
    ::ReleaseDC(hWndParent, hdc);
}

static int dpix = 0, dpiy = 0;
LRESULT CALLBACK mySubClassProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData)
{

    WebView* webview = (WebView*)dwRefData;
    switch (uMsg) {
    case WM_DPICHANGED: {
        GetDPI(hWnd, &dpix, &dpiy);
        break;
    }
    case WM_MOVE: {
        RECT rcWin;
        GetWindowRect(hWnd, &rcWin);

        obj_ptr<EventInfo> ei = new EventInfo(webview, "move");
        ei->add("left", (int32_t)rcWin.left * 96 / dpix);
        ei->add("top", (int32_t)rcWin.top * 96 / dpiy);

        webview->_emit("move", ei);
        break;
    }
    case WM_SIZE: {
        RECT rcWin;
        GetWindowRect(hWnd, &rcWin);

        obj_ptr<EventInfo> ei = new EventInfo(webview, "resize");
        ei->add("width", (int32_t)(rcWin.right - rcWin.left) * 96 / dpix);
        ei->add("height", (int32_t)(rcWin.bottom - rcWin.top) * 96 / dpiy);

        webview->_emit("resize", ei);

        ICoreWebView2Controller* controller = (ICoreWebView2Controller*)GetWindowLongPtr(hWnd, 0);
        if (controller != nullptr) {
            RECT bounds;
            GetClientRect(hWnd, &bounds);
            controller->put_Bounds(bounds);
        }
        break;
    }
    case WM_CLOSE: {
        ICoreWebView2Controller* controller = (ICoreWebView2Controller*)GetWindowLongPtr(hWnd, 0);
        if (controller != nullptr) {
            controller->Release();
            SetWindowLongPtr(hWnd, 0, 0);
            webview->release();
        }
        break;
    }
    }

    return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}

void WebView::internal_close()
{
    HWND hWndParent = (HWND)m_window;
    SendMessage(hWndParent, WM_CLOSE, 0, 0);
}

void WebView::config()
{
    HWND hWndParent = (HWND)m_window;
    DWORD dwStyle = WS_OVERLAPPED;
    int x = CW_USEDEFAULT;
    int y = CW_USEDEFAULT;
    int nWidth = CW_USEDEFAULT;
    int nHeight = CW_USEDEFAULT;
    bool _maximize = false;
    bool _fullscreen = false;

    if (m_opt) {
        Variant v;

        if (m_opt->get("left", v) == 0)
            x = v.intVal();
        if (m_opt->get("top", v) == 0)
            y = v.intVal();
        if (m_opt->get("width", v) == 0)
            nWidth = v.intVal();
        if (m_opt->get("height", v) == 0)
            nHeight = v.intVal();

        if (!(m_opt->get("frame", v) == 0 && !v.boolVal())) {
            dwStyle |= WS_BORDER | WS_THICKFRAME;

            if (!(m_opt->get("caption", v) == 0 && !v.boolVal()))
                dwStyle |= WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX;

            if (m_opt->get("resizable", v) == 0 && !v.boolVal()) {
                dwStyle &= ~(WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX);

                HMENU hSysMenu = GetSystemMenu(hWndParent, FALSE);
                RemoveMenu(hSysMenu, SC_RESTORE, MF_BYCOMMAND);
                RemoveMenu(hSysMenu, SC_MAXIMIZE, MF_BYCOMMAND);
                RemoveMenu(hSysMenu, SC_MINIMIZE, MF_BYCOMMAND);
                RemoveMenu(hSysMenu, SC_SIZE, MF_BYCOMMAND);
            }
        }

        if (m_opt->get("maximize", v) == 0 && v.boolVal())
            _maximize = true;

        if (m_opt->get("fullscreen", v) == 0 && v.boolVal())
            _fullscreen = true;
    } else
        dwStyle = WS_OVERLAPPEDWINDOW;

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
    } else if (_maximize) {
        dwStyle |= WS_MAXIMIZE;
    } else {
        SetWindowLong(hWndParent, GWL_STYLE, dwStyle);
        SetWindowPos(hWndParent, HWND_TOP, x, y, nWidth, nHeight, 0);
    }

    dwStyle |= WS_VISIBLE;
    SetWindowLong(hWndParent, GWL_STYLE, dwStyle);

    SetWindowSubclass(hWndParent, &mySubClassProc, 1, (DWORD_PTR)this);
    Ref();
}
}

#endif