/*
 * config_win.cpp
 *
 *  Created on: Feb 26, 2021
 *      Author: lion
 */

#include "object.h"
#include "EventInfo.h"
#include "ifs/WebView.h"

#ifdef Linux
#include <X11/Xlib.h>
#include <X11/Xatom.h>

namespace fibjs {

#define CW_USEDEFAULT -1

void os_config_window(WebView_base* webview, void* _window, NObject* opt)
{
    static Display* _display;
    static Atom wmState;
    static Atom wmAddState;
    static Atom wmVMaximizedState;
    static Atom wmHMaximizedState;

    static Atom wmWindowType;
    static Atom wmDock;
    static Atom wmFullscreen;

    if (!_display) {
        _display = XOpenDisplay(NULL);

        wmState = XInternAtom(_display, "_NET_WM_STATE", false);
        wmAddState = XInternAtom(_display, "_NET_WM_STATE_ADD", false);
        wmVMaximizedState = XInternAtom(_display, "_NET_WM_STATE_MAXIMIZED_VERT", false);
        wmHMaximizedState = XInternAtom(_display, "_NET_WM_STATE_MAXIMIZED_HORZ", false);

        wmWindowType = XInternAtom(_display, "_NET_WM_WINDOW_TYPE", false);
        wmDock = XInternAtom(_display, "_NET_WM_WINDOW_TYPE_DOCK", false);

        wmFullscreen = XInternAtom(_display, "_NET_WM_STATE_FULLSCREEN", false);
    }

    unsigned long window = (unsigned long)_window;
    int32_t x = CW_USEDEFAULT;
    int32_t y = CW_USEDEFAULT;
    int32_t nWidth = CW_USEDEFAULT;
    int32_t nHeight = CW_USEDEFAULT;
    bool _maximize = false;
    bool _border = true;
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

        if ((opt->get("border", v) == 0 && !v.boolVal())
            || (opt->get("caption", v) == 0 && !v.boolVal()))
            _border = false;

        if (opt->get("maximize", v) == 0 && v.boolVal())
            _maximize = true;

        if (opt->get("fullscreen", v) == 0 && v.boolVal())
            _fullscreen = true;
    }

    Screen* _screen = DefaultScreenOfDisplay(_display);

    if (nWidth == CW_USEDEFAULT)
        nWidth = _screen->width * 3 / 4;

    if (nHeight == CW_USEDEFAULT)
        nHeight = _screen->height * 3 / 4;

    if (x == CW_USEDEFAULT)
        x = (_screen->width - nWidth) / 2;

    if (y == CW_USEDEFAULT)
        y = (_screen->height - nHeight) / 2;

    if (_fullscreen) {
        XEvent xev;
        memset(&xev, 0, sizeof(xev));
        xev.xclient.type = ClientMessage;
        xev.xclient.display = _display;
        xev.xclient.window = window;
        xev.xclient.message_type = wmState;
        xev.xclient.format = 32;
        xev.xclient.data.l[0] = 1; //xev.xclient.data.l[0] = wmAddState;
        xev.xclient.data.l[1] = wmFullscreen;
        xev.xclient.data.l[2] = 0;
        xev.xclient.data.l[3] = 0;

        XSendEvent(_display, DefaultRootWindow(_display), False,
            SubstructureNotifyMask | SubstructureRedirectMask, &xev);
    } else {
        if (_maximize) {
            XEvent xev;
            memset(&xev, 0, sizeof(xev));
            xev.xclient.type = ClientMessage;
            xev.xclient.display = _display;
            xev.xclient.window = window;
            xev.xclient.message_type = wmState;
            xev.xclient.format = 32;
            xev.xclient.data.l[0] = 1; //xev.xclient.data.l[0] = wmAddState;
            xev.xclient.data.l[1] = wmVMaximizedState;
            xev.xclient.data.l[2] = wmHMaximizedState;
            xev.xclient.data.l[3] = 1;

            XSendEvent(_display, DefaultRootWindow(_display), False,
                SubstructureNotifyMask | SubstructureRedirectMask, &xev);
        }

        if (!_border)
            XChangeProperty(_display, window, wmWindowType, XA_ATOM, 32, PropModeReplace, (unsigned char*)&wmDock, 1);

        XMoveResizeWindow(_display, window, x, y, nWidth, nHeight);
    }

    XFlush(_display);

    webview->_emit("open");
}
}

#endif