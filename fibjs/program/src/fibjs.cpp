/*
 * fibjs.cpp
 *
 *  Created on: May 28, 2012
 *      Author: lion
 */

#include "fibjs.h"
#include "object.h"

namespace fibjs {

void importModule()
{
    IMPORT_MODULE(assert);
    IMPORT_MODULE(base32);
    IMPORT_MODULE(base64);
    IMPORT_MODULE(base64vlq);
    IMPORT_MODULE(bson);
    IMPORT_MODULE(buffer);
    IMPORT_MODULE(coroutine);
    IMPORT_MODULE(crypto);
    IMPORT_MODULE(db);
    IMPORT_MODULE(dgram);
    IMPORT_MODULE(dns);
    IMPORT_MODULE(encoding);
    IMPORT_MODULE(events);
    IMPORT_MODULE(fs);
    IMPORT_MODULE(gd);
    IMPORT_MODULE(hash);
    IMPORT_MODULE(hex);
    IMPORT_MODULE(http);
    IMPORT_MODULE(iconv);
    IMPORT_MODULE(io);
    IMPORT_MODULE(json);
    IMPORT_MODULE(mq);
    IMPORT_MODULE(net);
    IMPORT_MODULE(os);
    IMPORT_MODULE(path);
    IMPORT_MODULE(process);
    IMPORT_MODULE(profiler);
    IMPORT_MODULE(punycode);
    IMPORT_MODULE(querystring);
    IMPORT_MODULE(ssl);
    IMPORT_MODULE(string_decoder);
    IMPORT_MODULE(test);
    IMPORT_MODULE(timers);
    IMPORT_MODULE(tty);
    IMPORT_MODULE(url);
    IMPORT_MODULE(util);
    IMPORT_MODULE(uuid);
    IMPORT_MODULE(vm);
    IMPORT_MODULE(ws);
    IMPORT_MODULE(xml);
    IMPORT_MODULE(zip);
    IMPORT_MODULE(zlib);
    IMPORT_MODULE(zmq);

#ifdef _WIN32
    IMPORT_MODULE(gui);
    IMPORT_MODULE(registry);
#endif
}

void main(int32_t argc, char** argv)
{
    importModule();

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
