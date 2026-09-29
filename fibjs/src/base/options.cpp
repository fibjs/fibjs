/*
 * options.cpp
 *
 *  Created on: Jul 23, 2015
 *      Author: lion
 */

#include "v8/src/flags/flags.h"

#include "version.h"
#include "console.h"
#include "path.h"
#include "Fiber.h"
#include "options.h"
#include "unicode/locid.h"
#include "unicode/timezone.h"
#include "../util/dotenv_parser.h"
#include <sys/stat.h>
#include <time.h>
#include <uv/include/uv.h>

#ifdef _WIN32
#include <direct.h>
#define mkdir(path, mode) _mkdir(path)
#endif

namespace fibjs {

#if ARCH_BITS == 64
int32_t stack_size = 512;
#else
int32_t stack_size = 256;
#endif

bool g_prof = false;
int32_t g_prof_interval = 1000;

FILE* g_cov = nullptr;
exlib::string g_cov_exclude;

bool g_tcpdump = false;
bool g_ssldump = false;
bool g_pipedump = false;
bool g_no_deprecation = false;

bool g_uv_socket = false;

bool g_track_native_object = false;

bool g_openssl_legacy_provider = false;

bool g_use_env_proxy = false;

bool g_js_thread_affinity = true;

exlib::string g_exec_code;

struct EnvFileOption {
    exlib::string path;
    bool optional;
};

static bool readEnvFile(const exlib::string& path, exlib::string& retVal)
{
    FILE* fp = fopen(path.c_str(), "rb");
    if (fp == nullptr)
        return false;

    char buffer[8192];
    size_t bytes_read;

    retVal.clear();
    while ((bytes_read = fread(buffer, 1, sizeof(buffer), fp)) > 0)
        retVal.append(buffer, bytes_read);

    bool ok = ferror(fp) == 0;
    fclose(fp);
    return ok;
}

static bool hasEnvVar(const std::string& key)
{
    char buf[1];
    size_t sz = sizeof(buf);
    int32_t ret = uv_os_getenv(key.c_str(), buf, &sz);

    return ret == 0 || ret == UV_ENOBUFS;
}

static void applyEnvFileOptions(const std::vector<EnvFileOption>& env_files)
{
    dotenv_parser::store_t store;

    for (const auto& env_file : env_files) {
        exlib::string content;
        if (!readEnvFile(env_file.path, content)) {
            if (env_file.optional) {
                fprintf(stderr, "%s not found. Continuing without it.\n", env_file.path.c_str());
                continue;
            }

            fprintf(stderr, "%s: not found\n", env_file.path.c_str());
            fflush(stderr);
            _exit(1);
        }

        dotenv_parser::parse_content(std::string_view(content.c_str(), content.length()), store);
    }

    for (const auto& entry : store) {
        if (hasEnvVar(entry.first))
            continue;

        uv_os_setenv(entry.first.c_str(), entry.second.c_str());
    }
}

// Default file name for `--cov` without a value and for FIBJS_COV=1:
// fibjs-YYYYMMDD-HHMMSS-<pid>.lcov, local time (Node names its profiler files
// the same way). One file per process, so parallel runs never interleave their
// records into the same file; `--cov-process` merges the batch back into one
// report.
//
// The name used to be `fibjs-%d.lcov` with `(int32_t)date_t::date()`: that is
// the millisecond timestamp wrapped into int32, i.e. an unreadable (mostly
// negative) number that changed every millisecond, so every run wrote a new
// file and the append mode never accumulated anything.
//
// The OS is asked for the local time directly, because options() runs before
// the runtime creates the DateCache that date_t::toLocal() relies on.
static void defaultCovFilename(char* name, size_t size)
{
    time_t now = time(nullptr);
    struct tm tmv;

#ifdef _WIN32
    localtime_s(&tmv, &now);
#else
    localtime_r(&now, &tmv);
#endif

    snprintf(name, size, "fibjs-%04d%02d%02d-%02d%02d%02d-%d.lcov",
        tmv.tm_year + 1900, tmv.tm_mon + 1, tmv.tm_mday,
        tmv.tm_hour, tmv.tm_min, tmv.tm_sec, (int32_t)uv_os_getpid());
}

// `mkdir -p` for the directory part of a coverage file path: FIBJS_COV and
// --cov may point into a directory tree that does not exist yet (`cov/run.lcov`
// in a fresh checkout), just like the output directory of --cov-process is
// created on demand. A failure is not reported here, fopen below reports it if
// the path is still unusable.
static void makeCovDirs(const char* filename)
{
    exlib::string dir(filename);
    size_t pos = 1;

    while ((pos = dir.find_first_of("/\\", pos)) != exlib::string::npos) {
        exlib::string sub = dir.substr(0, pos);

        // skip the drive prefix of "C:\..." and a leading separator
        if (!sub.empty() && sub[sub.length() - 1] != ':')
            mkdir(sub.c_str(), 0755);

        pos++;
    }
}

static void openCovFile(const char* filename)
{
    makeCovDirs(filename);

    g_cov = fopen(filename, "a");
    if (g_cov == nullptr) {
        fprintf(stderr, "Cannot open coverage file: %s\n", filename);
        fflush(stderr);
        _exit(1);
    }
}

// Case insensitive match of a whole environment value against a keyword.
static bool envValueIs(const char* value, const char* keyword)
{
    while (*value && *keyword) {
        char c = *value++;
        if (c >= 'A' && c <= 'Z')
            c += 'a' - 'A';
        if (c != *keyword++)
            return false;
    }

    return *value == 0 && *keyword == 0;
}

// `--cov-exclude=<glob>` may be repeated and FIBJS_COV_EXCLUDE carries a `;`
// separated list, so the patterns are collected into one such list. An empty
// entry is dropped, and the spaces around it are not part of the pattern.
static void addCovExclude(const char* patterns)
{
    while (*patterns) {
        const char* end = strchr(patterns, ';');
        size_t len = end ? (size_t)(end - patterns) : strlen(patterns);

        while (len > 0 && (*patterns == ' ' || *patterns == '\t')) {
            patterns++;
            len--;
        }

        while (len > 0 && (patterns[len - 1] == ' ' || patterns[len - 1] == '\t'))
            len--;

        if (len > 0) {
            if (!g_cov_exclude.empty())
                g_cov_exclude.append(1, ';');
            g_cov_exclude.append(patterns, len);
        }

        if (end == nullptr)
            break;

        patterns = end + 1;
    }
}

// FIBJS_COV_EXCLUDE is read on its own: it also applies to a `--cov` run, where
// applyCovEnv() below returns before it would look at the environment.
static void applyCovExcludeEnv()
{
    char value[4096];
    size_t size = sizeof(value);

    int32_t ret = uv_os_getenv("FIBJS_COV_EXCLUDE", value, &size);
    if (ret == UV_ENOENT)
        return;

    if (ret != 0) {
        fprintf(stderr, "FIBJS_COV_EXCLUDE is too long\n");
        fflush(stderr);
        _exit(1);
    }

    addCovExclude(value);
}

// FIBJS_COV turns on code coverage from the environment, so that one global
// export covers every process of a test suite instead of passing `--cov` at
// each launch site: FIBJS_COV=1 uses the default file name, FIBJS_COV=<file>
// writes there, FIBJS_COV=0|false|no|off (or an empty value) keeps it off.
// The command line wins over the environment, and the variable is inherited by
// child processes like any other.
static void applyCovEnv()
{
    char value[4096];
    size_t size = sizeof(value);

    if (g_cov != nullptr)
        return;

    int32_t ret = uv_os_getenv("FIBJS_COV", value, &size);
    if (ret == UV_ENOENT)
        return;

    if (ret != 0) {
        fprintf(stderr, "FIBJS_COV is too long\n");
        fflush(stderr);
        _exit(1);
    }

    if (value[0] == 0
        || envValueIs(value, "0") || envValueIs(value, "false")
        || envValueIs(value, "no") || envValueIs(value, "off"))
        return;

    if (envValueIs(value, "1") || envValueIs(value, "true")
        || envValueIs(value, "yes") || envValueIs(value, "on")) {
        char name[64];

        defaultCovFilename(name, sizeof(name));
        openCovFile(name);

        return;
    }

    openCovFile(value);
}

#ifdef DEBUG
#define GUARD_SIZE 32
#else
#define GUARD_SIZE 16
#endif

// The top level help lists the runtime options and one line per command; the
// options of a command belong to the command and are printed by its own
// `fibjs --<command> --help`. See plans/cli-help-convention.md.
static void printHelp()
{
    puts("Usage: fibjs [options] [script.js] [arguments] \n"
         "\n"
         "Options:\n"
         "  -h, --help                  print fibjs command line options.\n"
         "  -v, --version               print fibjs version.\n"
         "\n"
         "  -e code                     evaluate script\n"
         "\n"
         "  --use-thread                run fibjs in thread mode.\n"
         "  --no-deprecation            silence deprecation warnings.\n"
         "  --no-js-thread-affinity     schedule the JS fibers of one isolate on a\n"
         "                              pool of worker threads instead of pinning\n"
         "                              them to one dedicated thread. Faster for CPU\n"
         "                              bound fibers, but N-API addons that keep\n"
         "                              state in thread-local storage may break.\n"
         "  --tcpdump                   print out the contents of the tcp package.\n"
         "  --ssldump                   print out the contents of the ssl package.\n"
         "  --pipedump                  print out the contents of the pipe package.\n"
         "\n"
         "  --use-uv-socket[=on|off]\n"
         "                              use uv as socket backend.\n"
         "\n"
         "  --use-env-proxy             parse proxy settings from\n"
         "                              HTTP_PROXY/HTTPS_PROXY/NO_PROXY environment\n"
         "                              variables and use them for connections.\n"
         "\n"
         "  --env-file=file             load environment variables from a dotenv file.\n"
         "  --env-file-if-exists=file   same as --env-file, but do not fail if the file is missing.\n"
         "\n"
         "  --openssl-legacy-provider   enable OpenSSL 3.0 legacy provider.\n"
         "\n"
         "  --prof                      log statistical profiling information.\n"
         "  --prof-interval=n           interval for --prof samples (in microseconds, default: 1000).\n"
         "  --track-native-object       track native object counts.\n"
         "\n"
         "  --cov[=filename]            collect code coverage information (only work on the main Worker).\n"
         "                              FIBJS_COV=<file> enables it from the environment,\n"
         "                              FIBJS_COV=1 writes fibjs-<date>-<time>-<pid>.lcov.\n"
         "  --cov-exclude=<glob>        leave the files the glob matches out of the report;\n"
         "                              repeatable, FIBJS_COV_EXCLUDE=<glob>[;<glob>...] too.\n"
         "\n"
         "  --v8-options                print v8 command line options.\n"
         "\n"
         "Commands:\n"
         "  --init                      write a package.json file.\n"
         "  --install [pkg]             install dependencies in the local node_modules folder.\n"
         "  --test [files|dirs|globs]   run test files with the built-in test module.\n"
         "  --check [files]             run the TypeScript checker (alias: -c).\n"
         "  --cov-process <glob> <dir>  merge lcov files and generate the coverage report.\n"
         "  --prof-process <log> <out>  render a --prof log as a flame graph SVG.\n"
         "\n"
         "Run `fibjs --<command> --help` for the options of a command.\n"
         "\n"
         "Documentation can be found at http://fibjs.org\n");
}

void options(int32_t& pos, char* argv[])
{
    int32_t i;
    int32_t df = 0;
    std::vector<EnvFileOption> env_files;

    for (i = 1; i < pos; i++) {
        char* arg = argv[i];

        if (df)
            argv[i - df] = arg;

        if (!qstrcmp(arg, "--")) {
            df++;
            i++;
            break;
        }

        if (arg[0] != '-')
            break;
        else if (arg[1] == '-') {
            int32_t j;
            exlib::string tmp("opt_tools/");
            tmp += arg + 2;

            for (j = 0; opt_tools[j].name && qstrcmp(opt_tools[j].name, tmp.c_str()); j++)
                ;

            if (opt_tools[j].name)
                break;
        } else if (!qstrcmp(arg, "-c")) {
            // Alias for --check (Node compatible). Rewrite the argument so the
            // rest of the pipeline (opt_tools lookup in run_main) sees --check.
            static char check_arg[] = "--check";
            argv[i] = check_arg;
            break;
        }

        if (!qstrcmp(arg, "--help") || !qstrcmp(arg, "-h")) {
            printHelp();
            fflush(stdout);
            _exit(0);
        } else if (!qstrcmp(arg, "--version") || !qstrcmp(arg, "-v")) {
            printf("v%s\n", fibjs_version);
            fflush(stdout);
            _exit(0);
        } else if (!qstrcmp(arg, "--use-thread")) {
            exlib::Service::use_thread = true;
            df++;
        } else if (!qstrcmp(arg, "--no-deprecation")) {
            g_no_deprecation = true;
            df++;
        } else if (!qstrcmp(arg, "--tcpdump")) {
            g_tcpdump = true;
            df++;
        } else if (!qstrcmp(arg, "--ssldump")) {
            g_ssldump = true;
            df++;
        } else if (!qstrcmp(arg, "--pipedump")) {
            g_pipedump = true;
            df++;
        } else if (!qstrcmp(arg, "--use-uv-socket", 15)) {
            g_uv_socket = (arg[15] == 0 || !qstrcmp(arg + 15, "=on"));
            df++;
        } else if (!qstrcmp(arg, "--prof")) {
            g_prof = true;
            df++;
        } else if (!qstrcmp(arg, "--prof-interval=", 16)) {
            g_prof_interval = atoi(arg + 16);
            if (g_prof_interval < 50)
                g_prof_interval = 50;
            df++;
        } else if (!qstrcmp(arg, "--track-native-object")) {
            g_track_native_object = true;
            df++;
        } else if (!qstrcmp(arg, "--use-env-proxy")) {
            g_use_env_proxy = true;
            df++;
        } else if (!qstrcmp(arg, "--no-js-thread-affinity")) {
            g_js_thread_affinity = false;
            df++;
        } else if (!qstrcmp(arg, "--js-thread-affinity")) {
            g_js_thread_affinity = true;
            df++;
        } else if (!qstrcmp(arg, "--env-file")) {
            if (i + 1 >= pos || argv[i + 1][0] == 0) {
                fprintf(stderr, "%s requires a path\n", arg);
                fflush(stderr);
                _exit(1);
            }

            env_files.push_back({ argv[i + 1], false });
            i++;
            df += 2;
        } else if (!qstrcmp(arg, "--env-file=", 11)) {
            if (arg[11] == 0) {
                fprintf(stderr, "%s requires a path\n", "--env-file");
                fflush(stderr);
                _exit(1);
            }

            env_files.push_back({ arg + 11, false });
            df++;
        } else if (!qstrcmp(arg, "--env-file-if-exists")) {
            if (i + 1 >= pos || argv[i + 1][0] == 0) {
                fprintf(stderr, "%s requires a path\n", arg);
                fflush(stderr);
                _exit(1);
            }

            env_files.push_back({ argv[i + 1], true });
            i++;
            df += 2;
        } else if (!qstrcmp(arg, "--env-file-if-exists=", 21)) {
            if (arg[21] == 0) {
                fprintf(stderr, "%s requires a path\n", "--env-file-if-exists");
                fflush(stderr);
                _exit(1);
            }

            env_files.push_back({ arg + 21, true });
            df++;
        } else if (!qstrcmp(arg, "--openssl-legacy-provider")) {
            g_openssl_legacy_provider = true;
            df++;
        } else if (!qstrcmp(arg, "--cov=", 6)) {
            openCovFile(arg + 6);
            df++;
        } else if (!qstrcmp(arg, "--cov-exclude=", 14)) {
            if (arg[14] == 0) {
                fprintf(stderr, "%s requires a glob\n", "--cov-exclude");
                fflush(stderr);
                _exit(1);
            }

            addCovExclude(arg + 14);
            df++;
        } else if (!qstrcmp(arg, "--cov")) {
            char name[64];

            defaultCovFilename(name, sizeof(name));
            openCovFile(name);
            df++;
        } else if (!qstrcmp(arg, "-e")) {
            if (i + 1 < pos) {
                g_exec_code = argv[i + 1];
                i++;
                df += 2;
            }
        } else if (!qstrcmp(arg, "--v8-options")) {
            v8::internal::FlagList::PrintHelp();
            _exit(0);
        }
    }

    pos = i;
    int32_t argc = pos - df;

    if (!env_files.empty())
        applyEnvFileOptions(env_files);

    // after the env files: `--env-file=.env` carrying FIBJS_COV works too
    applyCovEnv();
    applyCovExcludeEnv();

    v8::V8::SetFlagsFromCommandLine(&argc, argv, true);

    char* lang = getenv("LANG");
    if (lang) {
        icu::Locale locale(lang);
        UErrorCode error_code = U_ZERO_ERROR;
        icu::Locale::setDefault(locale, error_code);
    }

    char* tz = getenv("TZ");
    if (tz) {
        icu::TimeZone* zone = icu::TimeZone::createTimeZone(tz);
        icu::TimeZone::setDefault(*zone);
    }

    size_t sz = uv_get_total_memory() / 1024 / 1024;
    sz = sz * 3 / 4;

    // Disable lazy compilation and force eager compilation
    // v8::internal::v8_flags.lazy = false;
    // v8::internal::v8_flags.lazy_eval = false;
    // v8::internal::v8_flags.max_lazy = false;
    
    // // Disable lazy source positions to avoid source code dependency
    // v8::internal::v8_flags.enable_lazy_source_positions = false;
    // v8::internal::v8_flags.stress_lazy_source_positions = false;
    
    // // Disable lazy feedback allocation
    // v8::internal::v8_flags.lazy_feedback_allocation = false;

    v8::internal::v8_flags.max_heap_size = sz;
    v8::internal::v8_flags.stack_size = stack_size - GUARD_SIZE;
    v8::internal::v8_flags.wasm_code_gc = false;

    v8::internal::v8_flags.turbo_store_elimination = false;

    // v8::internal::v8_flags.harmony_import_assertions = false;
    v8::internal::v8_flags.harmony_import_attributes = true;

    v8::internal::v8_flags.expose_gc = true;
}
}
