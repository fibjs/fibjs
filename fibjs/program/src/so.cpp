/*
 * so.cpp
 *
 *  Created on: Jun 19, 2023
 *      Author: lion
 */

#include "fibjs.h"
#include "object.h"

namespace fibjs {

void main(int32_t argc, char** argv)
{
    importBuiltinModule();

    start(argc, argv, FiberProcJsEntry);
    run_gui(argc, argv);
}
}

extern "C"
#ifdef WIN32
    __declspec(dllexport)
#else
    __attribute__((visibility("default")))
#endif
int32_t main(int32_t argc, char* argv[])
{
    fibjs::main(argc, argv);
    return 0;
}
