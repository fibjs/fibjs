/*
 * fibjs.h
 *
 *  Created on: May 28, 2018
 *      Author: lion
 */

#pragma once

#include "utils.h"

namespace fibjs {

void importBuiltinModule();
void run_gui(int argc, char* argv[]);
void start(int32_t argc, char** argv, result_t (*main)(Isolate*), Isolate::platform_creator get_platform = NULL);
result_t FiberProcJsEntry(Isolate* isolate);

// Desktop builds: create the (unstarted) dedicated service the main isolate
// runs on, so that run_gui() can bind it to the process main thread. Must be
// called before start(). Non-desktop builds: no-op.
void gui_prepare_main_service();

} /* namespace fibjs */
