/*
 * WebView.cpp
 *
 *  Created on: Oct 11, 2024
 *      Author: lion
 */

#ifdef _WIN32

#include "object.h"
#include <shlwapi.h>
#include <gdiplus.h>
#include "image.h"

#pragma comment(lib, "gdiplus.lib")
#pragma comment(lib, "msimg32.lib")

namespace fibjs {

HICON LoadPngIcon(const BYTE* pngData, size_t pngSize)
{
    Gdiplus::GdiplusStartupInput gdiplusStartupInput;
    ULONG_PTR gdiplusToken;
    Gdiplus::GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

    HICON hIcon = NULL;
    IStream* pStream = SHCreateMemStream(pngData, pngSize);
    if (pStream) {
        Gdiplus::Bitmap* bitmap = Gdiplus::Bitmap::FromStream(pStream);
        if (bitmap) {
            bitmap->GetHICON(&hIcon);
            delete bitmap;
        }
        pStream->Release();
    }

    Gdiplus::GdiplusShutdown(gdiplusToken);
    return hIcon;
}

HBITMAP LoadPngBitmap(const BYTE* pngData, size_t pngSize)
{
    Gdiplus::GdiplusStartupInput gdiplusStartupInput;
    ULONG_PTR gdiplusToken;
    Gdiplus::GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

    HBITMAP hBitmap = NULL;
    IStream* pStream = SHCreateMemStream(pngData, pngSize);
    if (pStream) {
        Gdiplus::Bitmap* bitmap = Gdiplus::Bitmap::FromStream(pStream);
        if (bitmap) {
            bitmap->GetHBITMAP(Gdiplus::Color::White, &hBitmap);
            delete bitmap;
        }
        pStream->Release();
    }

    Gdiplus::GdiplusShutdown(gdiplusToken);
    return hBitmap;
}

HBITMAP ResizeBitmap(HBITMAP hBitmap, int newWidth, int newHeight)
{
    HDC hdcScreen = GetDC(NULL);
    HDC hdcSrc = CreateCompatibleDC(hdcScreen);
    HDC hdcDst = CreateCompatibleDC(hdcScreen);

    BITMAP bm;
    GetObject(hBitmap, sizeof(bm), &bm);

    BITMAPINFO bmi = { 0 };
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = newWidth;
    bmi.bmiHeader.biHeight = -newHeight;
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biCompression = BI_RGB;

    void* pvBits;
    HBITMAP hbmNew = CreateDIBSection(hdcScreen, &bmi, DIB_RGB_COLORS, &pvBits, NULL, 0);
    if (!hbmNew) {
        DeleteDC(hdcSrc);
        DeleteDC(hdcDst);
        ReleaseDC(NULL, hdcScreen);
        return NULL;
    }

    HBITMAP hbmOldSrc = (HBITMAP)SelectObject(hdcSrc, hBitmap);
    HBITMAP hbmOldDst = (HBITMAP)SelectObject(hdcDst, hbmNew);

    SetStretchBltMode(hdcDst, HALFTONE);

    BLENDFUNCTION blendFunction = { AC_SRC_OVER, 0, 255, AC_SRC_ALPHA };
    AlphaBlend(hdcDst, 0, 0, newWidth, newHeight, hdcSrc, 0, 0, bm.bmWidth, bm.bmHeight, blendFunction);

    SelectObject(hdcSrc, hbmOldSrc);
    SelectObject(hdcDst, hbmOldDst);

    DeleteDC(hdcSrc);
    DeleteDC(hdcDst);
    ReleaseDC(NULL, hdcScreen);

    return hbmNew;
}

}

#endif
