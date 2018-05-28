/*
 * fibjs.cpp
 *
 *  Created on: May 28, 2012
 *      Author: lion
 */

#include "fibjs.h"

namespace fibjs {

void main(int32_t argc, char** argv)
{
    start(argc, argv, main_fiber);
    run_gui();
}
}

#ifdef _WIN32

#ifdef _CONSOLE
int32_t main()
#else
int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
#endif
{
    int32_t argc;
    char** argv;

    LPWSTR* szArglist = CommandLineToArgvW(GetCommandLineW(), &argc);
    std::vector<exlib::string> strArgList;
    std::vector<char*> ptrArgList;
    int32_t i;

    strArgList.resize(argc);
    ptrArgList.resize(argc);
    for (i = 0; i < argc; i++) {
        strArgList[i] = fibjs::utf16to8String(szArglist[i]);
        ptrArgList[i] = strArgList[i].c_buffer();
    }

    argv = ptrArgList.data();

    fibjs::main(argc, argv);
    return 0;
}

#else

int32_t main(int32_t argc, char* argv[])
{
    fibjs::main(argc, argv);
    return 0;
}
#endif
