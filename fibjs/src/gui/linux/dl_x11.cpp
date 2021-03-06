/*
 * dl_x11.cpp
 *
  *  Created on: Feb 27, 2021
 *      Author: lion
 */

#if !defined(_WIN32) && !defined(__APPLE__)

#include <X11/Xlib.h>
#include <X11/Xatom.h>

#define __USE_GNU /* need RTLD_NEXT */
#include <dlfcn.h>
#include <stdlib.h>
#include <stdio.h>

#define X11_LIB "libX11.so.6"

static void* x11 = NULL;

#define x11_func(func) _func(x11, __func__, func, x11_init)

template <typename T>
T _func(void* handle, const char* name, T func, void (*_init)())
{
    if (_init)
        _init();

    T func1 = (T)::dlsym(handle, name);
    if (!func1) {
        fputs(dlerror(), stderr);
        exit(1);
    }

    return func1;
}

static void x11_init()
{
    if (!x11) {
        x11 = dlopen(X11_LIB, RTLD_LAZY);
        if (!x11) {
            fputs(dlerror(), stderr);
            exit(1);
        }
    }
}

Display* XOpenDisplay(_Xconst char* display_name)
{
    static auto s_func = x11_func(XOpenDisplay);
    return s_func(display_name);
}

int XConfigureWindow(Display* display, Window w, unsigned int value_mask,
    XWindowChanges* values)
{
    static auto s_func = x11_func(XConfigureWindow);
    return s_func(display, w, value_mask, values);
}

int XChangeProperty(Display* display, Window w, Atom property, Atom type, int format,
    int mode, _Xconst unsigned char* data, int nelements)
{
    static auto s_func = x11_func(XChangeProperty);
    return s_func(display, w, property, type, format, mode, data, nelements);
}

int XMoveResizeWindow(Display* display, Window w, int x, int y, unsigned width, unsigned height)
{
    static auto s_func = x11_func(XMoveResizeWindow);
    return s_func(display, w, x, y, width, height);
}

Status XSendEvent(Display* display, Window w, Bool propagate,
    long event_mask, XEvent* event_send)
{
    static auto s_func = x11_func(XSendEvent);
    return s_func(display, w, propagate, event_mask, event_send);
}

int XFlush(Display* display)
{
    static auto s_func = x11_func(XFlush);
    return s_func(display);
}

Atom XInternAtom(Display* display, _Xconst char* atom_name, Bool only_if_exists)
{
    static auto s_func = x11_func(XInternAtom);
    return s_func(display, atom_name, only_if_exists);
}

#endif