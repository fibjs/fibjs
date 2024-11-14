/*
 * gui.cpp
 *
 *  Created on: Feb 24, 2021
 *      Author: lion
 */

#ifdef OS_DESKTOP

#include "object.h"
#include "ifs/gui.h"
#include "ifs/fs.h"
#include "gui.h"

#include "WebView.h"
#include "Tray.h"

namespace fibjs {

DECLARE_MODULE(gui);

static exlib::Event s_gui;
exlib::Event g_gui_ready;

void putGuiPool(AsyncEvent* ac)
{
    s_gui.set();
    g_gui_ready.wait();

    os_putGuiPool(ac);
}

void run_gui(int argc, char* argv[])
{
    exlib::OSThread th;
    th.bindCurrent();

    Runtime rt(NULL);

    s_gui.wait();
    run_os_gui();
}

}

#endif
