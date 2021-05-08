/*
 * dl_x11.cpp
 *
  *  Created on: Feb 27, 2021
 *      Author: lion
 */

#if !defined(_WIN32) && !defined(__APPLE__)

#include <exlib/include/dl_func.h>

#include <X11/Xlib.h>
#include <X11/Xatom.h>

static void* x11_handle = NULL;
#define x11_func(func) dl_def_func(x11_handle, "libX11.so.6", func)

Display* XOpenDisplay(_Xconst char* display_name)
{
    x11_func(XOpenDisplay);
    return s_XOpenDisplay(display_name);
}

int XConfigureWindow(Display* display, Window w, unsigned int value_mask,
    XWindowChanges* values)
{
    x11_func(XConfigureWindow);
    return s_XConfigureWindow(display, w, value_mask, values);
}

int XChangeProperty(Display* display, Window w, Atom property, Atom type, int format,
    int mode, _Xconst unsigned char* data, int nelements)
{
    x11_func(XChangeProperty);
    return s_XChangeProperty(display, w, property, type, format, mode, data, nelements);
}

int XMoveResizeWindow(Display* display, Window w, int x, int y, unsigned width, unsigned height)
{
    x11_func(XMoveResizeWindow);
    return s_XMoveResizeWindow(display, w, x, y, width, height);
}

Status XSendEvent(Display* display, Window w, Bool propagate,
    long event_mask, XEvent* event_send)
{
    x11_func(XSendEvent);
    return s_XSendEvent(display, w, propagate, event_mask, event_send);
}

int XFlush(Display* display)
{
    x11_func(XFlush);
    return s_XFlush(display);
}

Atom XInternAtom(Display* display, _Xconst char* atom_name, Bool only_if_exists)
{
    x11_func(XInternAtom);
    return s_XInternAtom(display, atom_name, only_if_exists);
}

#endif