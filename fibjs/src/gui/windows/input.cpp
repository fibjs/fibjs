/*
 * input.cpp
 *
 *  Created on: Nov 22, 2024
 *      Author: lion
 */

#ifdef _WIN32

#include "object.h"
#include "ifs/gui.h"
#include "../gui.h"
#include <windows.h>
#include <windowsx.h>
#include <commctrl.h>
#include <algorithm>
#include <vector>

#pragma comment(lib, "comctl32.lib")

namespace fibjs {

struct DialogParams {
    exlib::string* retVal;
    bool dialogResult;
    exlib::string message;
    bool password;
};

static UINT GetWindowDPI(HWND hwnd)
{
    HDC hdc = GetDC(hwnd);
    int dpi = GetDeviceCaps(hdc, LOGPIXELSX);
    ReleaseDC(hwnd, hdc);
    return (UINT)dpi;
}

static int ScaleForDpi(int value, UINT dpi)
{
    return MulDiv(value, dpi, 96);
}

static void GetDialogMetrics(HWND hwnd, int& margin, int& buttonWidth, int& buttonHeight,
    int& editHeight, int& staticHeight, int& spacing, int& buttonSpacing)
{
    UINT dpi = GetWindowDPI(hwnd);

    NONCLIENTMETRICS ncm;
    ncm.cbSize = sizeof(NONCLIENTMETRICS);
    SystemParametersInfo(SPI_GETNONCLIENTMETRICS, sizeof(NONCLIENTMETRICS), &ncm, 0);

    margin = ScaleForDpi(16, dpi);
    buttonWidth = ScaleForDpi(80, dpi);

    int baseHeight = std::max(ncm.iMenuHeight, ScaleForDpi(26, dpi));
    buttonHeight = baseHeight;
    editHeight = baseHeight;

    staticHeight = abs(ncm.lfMessageFont.lfHeight) + ScaleForDpi(8, dpi);
    spacing = ScaleForDpi(8, dpi);
    buttonSpacing = ScaleForDpi(6, dpi);
}

static bool EnsureMessageLoop()
{
    static bool initialized = false;
    if (!initialized) {
        INITCOMMONCONTROLSEX icc;
        icc.dwSize = sizeof(icc);
        icc.dwICC = ICC_WIN95_CLASSES;
        InitCommonControlsEx(&icc);
        initialized = true;
    }
    return true;
}

static LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    static HWND hEdit = NULL;
    static HWND hOK = NULL;
    static HWND hStatic = NULL;
    static HWND hCancel = NULL;
    static DialogParams* params = NULL;
    static HFONT hFont = NULL;

    switch (msg) {
    case WM_CREATE: {
        CREATESTRUCT* cs = (CREATESTRUCT*)lParam;
        params = (DialogParams*)cs->lpCreateParams;

        NONCLIENTMETRICS ncm;
        ncm.cbSize = sizeof(NONCLIENTMETRICS);
        SystemParametersInfo(SPI_GETNONCLIENTMETRICS, sizeof(NONCLIENTMETRICS), &ncm, 0);
        hFont = CreateFontIndirect(&ncm.lfMessageFont);

        RECT rc;
        GetClientRect(hwnd, &rc);
        int width = rc.right - rc.left;
        int height = rc.bottom - rc.top;

        int margin, buttonWidth, buttonHeight, editHeight, staticHeight, spacing, buttonSpacing;
        GetDialogMetrics(hwnd, margin, buttonWidth, buttonHeight, editHeight, staticHeight, spacing, buttonSpacing);

        exlib::wstring wmessage = utf8to16String(params->message);
        hStatic = CreateWindowEx(0, L"STATIC", (LPCWSTR)wmessage.c_str(),
            WS_CHILD | WS_VISIBLE | SS_LEFT | SS_NOPREFIX,
            margin, margin,
            width - 2 * margin, staticHeight,
            hwnd, NULL, GetModuleHandle(NULL), NULL);

        SendMessage(hStatic, WM_SETFONT, (WPARAM)hFont, TRUE);

        LONG_PTR style = GetWindowLongPtr(hStatic, GWL_STYLE);
        style |= SS_NOTIFY;
        SetWindowLongPtr(hStatic, GWL_STYLE, style);

        HDC hdc = GetDC(hStatic);
        HFONT oldFont = (HFONT)SelectObject(hdc, hFont);
        RECT textRect = { 0, 0, width - 2 * margin, 0 };
        DrawText(hdc, (LPCWSTR)wmessage.c_str(), -1, &textRect,
            DT_WORDBREAK | DT_CALCRECT | DT_EXPANDTABS | DT_LEFT);
        SelectObject(hdc, oldFont);
        ReleaseDC(hStatic, hdc);

        int actualStaticHeight = textRect.bottom + ScaleForDpi(8, GetWindowDPI(hwnd));
        SetWindowPos(hStatic, NULL, margin, margin,
            width - 2 * margin, actualStaticHeight,
            SWP_NOZORDER);

        DWORD editStyle = WS_CHILD | WS_VISIBLE | WS_BORDER | WS_TABSTOP | ES_AUTOHSCROLL;
        if (params->password)
            editStyle |= ES_PASSWORD;

        hEdit = CreateWindowEx(WS_EX_CLIENTEDGE, L"EDIT", L"",
            editStyle, margin, margin + actualStaticHeight + spacing,
            width - 2 * margin, editHeight,
            hwnd, NULL, GetModuleHandle(NULL), NULL);

        int newHeight = margin + actualStaticHeight + spacing + editHeight + spacing + buttonHeight + margin;
        RECT windowRect = { 0, 0, width, newHeight };
        AdjustWindowRectEx(&windowRect, GetWindowLong(hwnd, GWL_STYLE),
            FALSE, GetWindowLong(hwnd, GWL_EXSTYLE));
        SetWindowPos(hwnd, NULL, 0, 0,
            windowRect.right - windowRect.left,
            windowRect.bottom - windowRect.top,
            SWP_NOMOVE | SWP_NOZORDER);

        GetClientRect(hwnd, &rc);
        width = rc.right - rc.left;
        height = rc.bottom - rc.top;

        hOK = CreateWindowEx(0, L"BUTTON", L"OK",
            WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON | WS_TABSTOP,
            width - 2 * buttonWidth - margin - buttonSpacing, height - buttonHeight - margin,
            buttonWidth, buttonHeight,
            hwnd, (HMENU)IDOK, GetModuleHandle(NULL), NULL);

        hCancel = CreateWindowEx(0, L"BUTTON", L"Cancel",
            WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | WS_TABSTOP,
            width - buttonWidth - margin, height - buttonHeight - margin,
            buttonWidth, buttonHeight,
            hwnd, (HMENU)IDCANCEL, GetModuleHandle(NULL), NULL);

        if (!hStatic || !hEdit || !hOK || !hCancel) {
            return -1;
        }

        if (hFont) {
            SendMessage(hStatic, WM_SETFONT, (WPARAM)hFont, TRUE);
            SendMessage(hEdit, WM_SETFONT, (WPARAM)hFont, TRUE);
            SendMessage(hOK, WM_SETFONT, (WPARAM)hFont, TRUE);
            SendMessage(hCancel, WM_SETFONT, (WPARAM)hFont, TRUE);
        }

        SetFocus(hEdit);

        SetWindowSubclass(hStatic, [](HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData) -> LRESULT {
                switch (msg) {
                    case WM_PAINT: {
                        PAINTSTRUCT ps;
                        HDC hdc = BeginPaint(hwnd, &ps);
                        
                        
                        RECT rc;
                        GetClientRect(hwnd, &rc);

                        
                        SetBkMode(hdc, TRANSPARENT);
                        HBRUSH hBrush = (HBRUSH)(COLOR_BTNFACE + 1);
                        FillRect(hdc, &rc, hBrush);
                        
                        
                        int len = GetWindowTextLength(hwnd);
                        std::vector<wchar_t> text(len + 1);
                        GetWindowText(hwnd, text.data(), len + 1);
                        
                        
                        HFONT hFont = (HFONT)SendMessage(hwnd, WM_GETFONT, 0, 0);
                        HFONT hOldFont = (HFONT)SelectObject(hdc, hFont);
                        
                        
                        SetTextColor(hdc, GetSysColor(COLOR_WINDOWTEXT));
                        
                        
                        DrawText(hdc, text.data(), -1, &rc, 
                            DT_WORDBREAK | DT_EXPANDTABS | DT_LEFT);
                        
                        SelectObject(hdc, hOldFont);
                        EndPaint(hwnd, &ps);
                        return 0;
                    }
                    case WM_ERASEBKGND:
                        return 1; 
                }
                return DefSubclassProc(hwnd, msg, wParam, lParam); }, 1, 0);

        return 0;
    }

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL) {
            if (LOWORD(wParam) == IDOK && params && params->retVal && hEdit) {
                wchar_t buf[1024] = { 0 };
                GetWindowTextW(hEdit, buf, 1024);
                *(params->retVal) = utf16to8String((char16_t*)buf);
                params->dialogResult = true;
            }
            DestroyWindow(hwnd);
            return 0;
        }
        break;

    case WM_CLOSE:
        DestroyWindow(hwnd);
        return 0;

    case WM_DESTROY:
        if (hFont) {
            DeleteObject(hFont);
            hFont = NULL;
        }
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProc(hwnd, msg, wParam, lParam);
}

result_t gui_base::input(exlib::string message, bool password, exlib::string& retVal, AsyncEvent* ac)
{
    return input("", message, password, retVal, ac);
}

result_t gui_base::input(exlib::string title, exlib::string message, bool password, exlib::string& retVal, AsyncEvent* ac)
{
    if (ac->isSync())
        return CHECK_ERROR(CALL_E_GUICALL);

    exlib::Event ev;
    result_t hr = CALL_RETURN_UNDEFINED;

    async([&message, &title, &password, &retVal, &hr, &ev]() {
        if (!EnsureMessageLoop()) {
            hr = CALL_RETURN_UNDEFINED;
            ev.set();
            return;
        }

        WNDCLASSEX wc = { 0 };
        wc.cbSize = sizeof(WNDCLASSEX);
        wc.lpfnWndProc = WndProc;
        wc.hInstance = GetModuleHandle(NULL);
        wc.lpszClassName = L"FibJSInputDialog";
        wc.hbrBackground = (HBRUSH)(COLOR_BTNFACE + 1);
        wc.hCursor = LoadCursor(NULL, IDC_ARROW);
        wc.style = CS_HREDRAW | CS_VREDRAW;

        if (!RegisterClassEx(&wc)) {
            hr = CALL_RETURN_UNDEFINED;
            ev.set();
            return;
        }

        DialogParams params = { &retVal, false, message, password };

        exlib::wstring wtitle = utf8to16String(title.empty() ? "Input" : title);

        HDC hdc = GetDC(NULL);
        UINT dpi = GetDeviceCaps(hdc, LOGPIXELSX);
        ReleaseDC(NULL, hdc);

        int windowWidth = ScaleForDpi(400, dpi);

        HDC hdcScreen = GetDC(NULL);
        NONCLIENTMETRICS ncm;
        ncm.cbSize = sizeof(NONCLIENTMETRICS);
        SystemParametersInfo(SPI_GETNONCLIENTMETRICS, sizeof(NONCLIENTMETRICS), &ncm, 0);
        HFONT hTempFont = CreateFontIndirect(&ncm.lfMessageFont);
        HFONT hOldFont = (HFONT)SelectObject(hdcScreen, hTempFont);

        RECT rcTemp = { 0, 0, windowWidth - 2 * ScaleForDpi(14, dpi), 0 };
        DrawText(hdcScreen, (LPCWSTR)utf8to16String(message).c_str(), -1, &rcTemp,
            DT_WORDBREAK | DT_CALCRECT | DT_EXPANDTABS | DT_LEFT);

        SelectObject(hdcScreen, hOldFont);
        DeleteObject(hTempFont);
        ReleaseDC(NULL, hdcScreen);

        int textHeight = rcTemp.bottom + ScaleForDpi(8, dpi);
        int buttonHeight = ScaleForDpi(26, dpi);
        int editHeight = ScaleForDpi(26, dpi);
        int margin = ScaleForDpi(16, dpi);
        int spacing = ScaleForDpi(8, dpi);
        int buttonSpacing = ScaleForDpi(6, dpi);

        int clientHeight = margin + textHeight + spacing + editHeight + spacing + buttonHeight + margin;

        RECT windowRect = { 0, 0, windowWidth, clientHeight };
        DWORD style = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX;
        DWORD exStyle = WS_EX_DLGMODALFRAME | WS_EX_TOPMOST;
        AdjustWindowRectEx(&windowRect, style, FALSE, exStyle);
        int windowHeight = windowRect.bottom - windowRect.top;

        HWND hwnd = CreateWindowEx(
            WS_EX_DLGMODALFRAME | WS_EX_TOPMOST,
            L"FibJSInputDialog",
            (LPCWSTR)wtitle.c_str(),
            WS_VISIBLE | WS_SYSMENU | WS_CAPTION | WS_MINIMIZEBOX,
            CW_USEDEFAULT, CW_USEDEFAULT,
            windowWidth, windowHeight,
            NULL, NULL,
            GetModuleHandle(NULL),
            &params);

        if (!hwnd) {
            UnregisterClass(L"FibJSInputDialog", GetModuleHandle(NULL));
            hr = CALL_RETURN_UNDEFINED;
            ev.set();
            return;
        }

        RECT rc;
        GetWindowRect(hwnd, &rc);
        int screenWidth = GetSystemMetrics(SM_CXSCREEN);
        int screenHeight = GetSystemMetrics(SM_CYSCREEN);
        SetWindowPos(hwnd, HWND_TOP,
            (screenWidth - (rc.right - rc.left)) / 2,
            (screenHeight - (rc.bottom - rc.top)) / 2,
            0, 0, SWP_NOSIZE);

        HWND hCancel = GetDlgItem(hwnd, IDCANCEL);

        MSG msg;
        while (GetMessage(&msg, NULL, 0, 0)) {

            if (msg.message == WM_KEYDOWN) {
                switch (msg.wParam) {
                case VK_RETURN: {
                    HWND focused = GetFocus();
                    if (focused == hCancel) {
                        SendMessage(hwnd, WM_COMMAND, MAKEWPARAM(IDCANCEL, BN_CLICKED), (LPARAM)NULL);
                    } else {
                        SendMessage(hwnd, WM_COMMAND, MAKEWPARAM(IDOK, BN_CLICKED), (LPARAM)NULL);
                    }
                    continue;
                }
                case VK_ESCAPE:
                    SendMessage(hwnd, WM_COMMAND, MAKEWPARAM(IDCANCEL, BN_CLICKED), (LPARAM)NULL);
                    continue;
                }
            }

            if (!IsDialogMessage(hwnd, &msg)) {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
        }

        UnregisterClass(L"FibJSInputDialog", GetModuleHandle(NULL));

        hr = params.dialogResult ? 0 : CALL_RETURN_UNDEFINED;
        ev.set();
    },
        CALL_E_LONGSYNC);

    ev.wait();
    return hr;
}

}

#endif