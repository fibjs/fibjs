/*
 * gui.h
 *
 *  Created on: Oct 25, 2024
 *      Author: lion
 */

#include "object.h"
#include "ifs/gui.h"
#include "ifs/fs.h"
#include "ifs/DialogFilter.h"

namespace fibjs {

extern exlib::Event g_gui_ready;

void os_putGuiPool(AsyncEvent* ac);
void run_os_gui();

} // namespace fibjs
