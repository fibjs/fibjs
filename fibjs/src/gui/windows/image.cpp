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

}

#endif
