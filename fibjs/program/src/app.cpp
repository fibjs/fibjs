/*
 * app.cpp
 *
 *  Created on: May 28, 2012
 *      Author: lion
 */

#include "fibjs.h"
#include "object.h"

namespace fibjs {

int32_t main(int32_t argc, char** argv)
{
    importBuiltinModule();

    start(argc, argv, FiberProcJsEntry);
    run_gui(argc, argv);

    return 0;
}

}

#ifdef _WIN32

int32_t wmain(int32_t argc, wchar_t* argv[])
{
    std::vector<exlib::string> strArgList;
    std::vector<char*> ptrArgList;

    strArgList.resize(argc);
    ptrArgList.resize(argc);

    for (int32_t i = 0; i < argc; i++) {
        strArgList[i] = fibjs::utf16to8String((const char16_t*)argv[i]);
        ptrArgList[i] = strArgList[i].data();
    }

    return fibjs::main(argc, ptrArgList.data());
}

#else

int32_t main(int32_t argc, char* argv[])
{
    return fibjs::main(argc, argv);
}

#endif
