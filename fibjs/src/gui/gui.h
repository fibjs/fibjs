/*
 * gui.h
 *
 *  Created on: Oct 25, 2024
 *      Author: lion
 */

#include "object.h"
#include "ifs/gui.h"
#include "ifs/fs.h"

namespace fibjs {

extern exlib::Event g_gui_ready;

void start_gui();
void run_os_gui();

} // namespace fibjs
