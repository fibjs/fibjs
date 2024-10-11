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
#include "loader/WebView2EnvironmentOptions.h"

#include <commctrl.h>
#include <shlobj.h>

#include "object.h"
#include "EventInfo.h"
#include "WebView.h"

namespace fibjs {

const wchar_t* szWndClassMain = L"fibjs_window";
ICoreWebView2Environment* g_env = nullptr;

static HWND s_worker;
void on_click_menu(uint32_t id);

#define WM_ASYNC_EVENT (WM_USER + 1000)

HICON LoadPngIcon(const BYTE* pngData, size_t pngSize);

void putGuiPool(AsyncEvent* ac)
{
    PostMessage(s_worker, WM_ASYNC_EVENT, 0, (LPARAM)ac);
}

LRESULT CALLBACK WorkerProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData)
{
    if (uMsg == WM_ASYNC_EVENT) {
        AsyncEvent* ac = (AsyncEvent*)lParam;
        ac->invoke();
    }

    return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}

std::wstring GetUserDataFolderPath()
{
    wchar_t path[MAX_PATH];
    if (SUCCEEDED(SHGetFolderPathW(NULL, CSIDL_LOCAL_APPDATA, NULL, 0, path))) {
        return std::wstring(path) + L"\\.fibjs";
    }
    return L"";
}

static void RegMainClass()
{
    HINSTANCE hInstance = GetModuleHandle(NULL);

    WNDCLASSW wc = {};
    wc.cbWndExtra = sizeof(void*);
    wc.hbrBackground = (HBRUSH)COLOR_WINDOW;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hIcon = nullptr;
    wc.hInstance = hInstance;
    wc.lpfnWndProc = DefWindowProc;
    wc.lpszClassName = szWndClassMain;

    RegisterClassW(&wc);

    s_worker = CreateWindowExW(0, szWndClassMain, L"", 0,
        CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);
    SetWindowSubclass(s_worker, WorkerProc, 0, 0);
}

Microsoft::WRL::ComPtr<CoreWebView2EnvironmentOptions> GetWebView2Options()
{
    auto options = Microsoft::WRL::Make<CoreWebView2EnvironmentOptions>();
    Microsoft::WRL::ComPtr<ICoreWebView2EnvironmentOptions4> options4;
    if (SUCCEEDED(options.As(&options4))) {
        auto scheme = Microsoft::WRL::Make<CoreWebView2CustomSchemeRegistration>(L"fs");
        const wchar_t* everything = L"*";
        scheme->SetAllowedOrigins(1, &everything);
        scheme->put_TreatAsSecure(TRUE);
        scheme->put_HasAuthorityComponent(FALSE);

        std::vector<ICoreWebView2CustomSchemeRegistration*> registrations;
        registrations.push_back(scheme.Get());
        options4->SetCustomSchemeRegistrations(registrations.size(),
            registrations.data());
    }
    return options;
}

void WebView::run_os_gui(exlib::Event& gui_ready)
{
    exlib::OSThread* _thGUI = exlib::OSThread::current();

    SetProcessDPIAware();
    RegMainClass();

    OleInitialize(NULL);

    std::wstring userDataFolder = GetUserDataFolderPath();
    auto options = GetWebView2Options();
    HRESULT hr = CreateCoreWebView2EnvironmentWithOptions(nullptr, userDataFolder.c_str(), options.Get(),
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

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
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
    case WM_COMMAND: {
        on_click_menu(LOWORD(wParam));
        break;
    }
    case WM_DPICHANGED: {
        GetDPI(hWnd, &dpix, &dpiy);
        break;
    }
    case WM_NCCALCSIZE:
        if (wParam == TRUE) {
            DefSubclassProc(hWnd, WM_NCCALCSIZE, wParam, lParam);

            DWORD dwStyle = GetWindowLong(hWnd, GWL_STYLE);
            if ((dwStyle & (WS_CAPTION | WS_THICKFRAME)) == WS_THICKFRAME) {
                RECT borderThickness;
                SetRectEmpty(&borderThickness);
                AdjustWindowRectEx(&borderThickness, dwStyle, FALSE, NULL);

                NCCALCSIZE_PARAMS* sz = reinterpret_cast<NCCALCSIZE_PARAMS*>(lParam);
                sz->rgrc[0].top += borderThickness.top + dpiy / 96;
            }

            return 0;
        }
        break;
    case WM_NCHITTEST: {
        LRESULT hit = DefSubclassProc(hWnd, uMsg, wParam, lParam);
        if (!webview->m_options->resizable.value()) {
            if (hit == HTBOTTOM || hit == HTBOTTOMLEFT || hit == HTBOTTOMRIGHT || hit == HTLEFT
                || hit == HTRIGHT || hit == HTTOP || hit == HTTOPLEFT || hit == HTTOPRIGHT) {
                hit = 0;
            }
        }
        return hit;
    }
    case WM_ACTIVATE: {
        if (wParam == WA_ACTIVE || wParam == WA_CLICKACTIVE) {
            obj_ptr<EventInfo> ei = new EventInfo(webview, "focus");
            webview->_emit("focus", ei);
        } else if (wParam == WA_INACTIVE) {
            obj_ptr<EventInfo> ei = new EventInfo(webview, "blur");
            webview->_emit("blur", ei);
        }
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

void WebView::internal_minimize()
{
    HWND hWndParent = (HWND)m_window;
    ShowWindow(hWndParent, SW_MINIMIZE);
}

void WebView::internal_maximize()
{
    HWND hWndParent = (HWND)m_window;

    if (IsZoomed(hWndParent))
        ShowWindow(hWndParent, SW_RESTORE);
    else
        ShowWindow(hWndParent, SW_MAXIMIZE);
}

void WebView::config()
{
    HWND hWndParent = (HWND)m_window;
    DWORD dwStyle = WS_OVERLAPPED;
    int x = CW_USEDEFAULT;
    int y = CW_USEDEFAULT;
    int nWidth = CW_USEDEFAULT;
    int nHeight = CW_USEDEFAULT;

    if (m_options->left.has_value())
        x = m_options->left.value();
    if (m_options->top.has_value())
        y = m_options->top.value();
    if (m_options->width.has_value())
        nWidth = m_options->width.value();
    if (m_options->height.has_value())
        nHeight = m_options->height.value();

    if (!m_options->fullscreen.value()) {
        if (m_options->frame.value()) {
            dwStyle |= WS_THICKFRAME;

            if (m_options->caption.value())
                dwStyle |= WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX;

            if (!m_options->resizable.value()) {
                dwStyle &= ~(WS_MINIMIZEBOX | WS_MAXIMIZEBOX);

                HMENU hSysMenu = GetSystemMenu(hWndParent, FALSE);
                RemoveMenu(hSysMenu, SC_RESTORE, MF_BYCOMMAND);
                RemoveMenu(hSysMenu, SC_MAXIMIZE, MF_BYCOMMAND);
                RemoveMenu(hSysMenu, SC_MINIMIZE, MF_BYCOMMAND);
                RemoveMenu(hSysMenu, SC_SIZE, MF_BYCOMMAND);
            }
        }
    }

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

    SetWindowSubclass(hWndParent, &mySubClassProc, 1, (DWORD_PTR)this);

    if (m_options->menu.has_value()) {
        HMENU hMenu = (HMENU)m_options->menu.value()->create_os_menu(false);
        if (hMenu)
            SetMenu(hWndParent, hMenu);
    }

    if (m_icon) {
        HICON hIcon = LoadPngIcon(m_icon->data(), m_icon->length());
        if (hIcon)
            SendMessage(hWndParent, WM_SETICON, ICON_BIG, (LPARAM)hIcon);
    }

    if (m_options->fullscreen.value()) {
        SetWindowPos(hWndParent, HWND_TOP, 0, 0, actualDesktop.right, actualDesktop.bottom, 0);
        dwStyle = WS_VISIBLE;
        SetWindowLong(hWndParent, GWL_STYLE, dwStyle);
    } else {
        SetWindowLong(hWndParent, GWL_STYLE, dwStyle);
        SetWindowPos(hWndParent, HWND_TOP, x, y, nWidth, nHeight, 0);

        if (m_options->maximize.value())
            ShowWindow(hWndParent, SW_MAXIMIZE);
        else
            ShowWindow(hWndParent, SW_SHOWNORMAL);
    }

    Ref();
}
}

#endif