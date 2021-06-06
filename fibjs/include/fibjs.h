/*
 * fibjs.h
 *
 *  Created on: May 28, 2018
 *      Author: lion
 */

#pragma once

#include "utils.h"

namespace fibjs {

typedef std::unique_ptr<v8::Platform> (*platform_creator)();
void run_gui(int argc, char* argv[]);
void start(int32_t argc, char** argv, result_t (*main)(Isolate*), platform_creator get_platform = NULL);
result_t FiberProcJsEntry(Isolate* isolate);

} /* namespace fibjs */
