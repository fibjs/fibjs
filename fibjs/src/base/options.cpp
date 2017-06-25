/*
 * options.cpp
 *
 *  Created on: Jul 23, 2015
 *      Author: lion
 */

#include "version.h"
#include "console.h"
#include "path.h"
#include "Fiber.h"

namespace v8 {
namespace internal {
    class FlagList {
    public:
        static void PrintHelp();
    };
}
}

namespace fibjs {

bool g_perf;
bool g_root = false;

#ifdef amd64
int32_t stack_size = 512;
#else
int32_t stack_size = 256;
#endif
extern exlib::string s_root;

#ifdef DEBUG
#define GUARD_SIZE 32
#else
#define GUARD_SIZE 16
#endif

static void printHelp()
{
    printf("Usage: fibjs [options] [script.js] [arguments] \n"
           "\n"
           "Options:\n"
           "  --trace_fiber        allow user to query the non-current\n"
           "                       fiber's stack infomation\n"
           "  --approot path       set application root directory\n"
           "  --root               enable application root directory\n"
           "  -h, --help           print fibjs command line options\n"
           "  -v, --version        print fibjs version\n"
           "  --v8-options         print v8 command line options\n"
           "\n"
           "Documentation can be found at http://fibjs.org/\n");
}

bool options(int32_t* argc, char* argv[])
{
    char s_opts[64];
    char s_sharmony[] = "--harmony --use_strict --nolazy";

    int32_t df = 0;

    for (int32_t i = 0; i < *argc; i++) {
        char* arg = argv[i];

        if (df)
            argv[i - df] = arg;

        if (!qstrcmp(arg, "--trace_fiber")) {
            df++;
            Isolate::rt::g_trace = true;
        } else if (!qstrcmp(arg, "--root")) {
            df++;
            g_root = true;
        } else if (!qstrcmp(arg, "--approot") && i + 1 < *argc) {
            i++;
            df += 2;
            s_root.clear();
            path_base::fullpath(argv[i], s_root);
        } else if (!qstrcmp(arg, "--help") || !qstrcmp(arg, "-h")) {
            printHelp();
            return true;
        } else if (!qstrcmp(arg, "--version") || !qstrcmp(arg, "-v")) {
            printf("v%s\n", fibjs_version);
            return true;
        } else if (!qstrcmp(arg, "--v8-options")) {
            v8::internal::FlagList::PrintHelp();
            return true;
        }
    }

    v8::V8::SetFlagsFromString(s_sharmony, sizeof(s_sharmony) - 1);
    v8::V8::SetFlagsFromString(s_opts,
        sprintf(s_opts, "--stack_size=%d", stack_size - GUARD_SIZE));
    v8::V8::SetFlagsFromCommandLine(argc, argv, true);

    if (df)
        *argc -= df;

#ifdef DEBUG
    Isolate::rt::g_trace = true;
#endif

    return false;
}
}
