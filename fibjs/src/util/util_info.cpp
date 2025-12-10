/*
 * utils_info.cpp
 *
 *  Created on: Jun 26, 2017
 *      Author: lion
 */

#include "object.h"
#include "version.h"
#include "ifs/util.h"
#include "ifs/os.h"
#include <zlib/include/zlib.h>
#include <sqlite/sqlite3.h>
#include <ev/ev.h>
#define PCRE2_CODE_UNIT_WIDTH 8
#include <pcre2/pcre2.h>
#include <openssl/opensslv.h>
#include <snappy/include/snappy.h>
#include <leveldb/db.h>
#include <expat/include/expat.h>
#include <uv/include/uv/version.h>
#include <unicode/uchar.h>
#include <unicode/uvernum.h>
#include <msgpack/version.hpp>
#include "addons/node_api/node_version.h"

#ifdef Linux
    #define _GNU_SOURCE
    #include <features.h>
    #ifndef __USE_GNU
        #ifndef __MUSL__
            #define __MUSL__
        #endif
    #endif
    #undef _GNU_SOURCE
#endif

namespace v8 {
namespace internal {
    class StartupData;
    class Snapshot {
    public:
        static const v8::StartupData* DefaultSnapshotBlob();
    };
}
}

#include "gitinfo.h"

namespace fibjs {

#define _STR(s) #s
#define STR(s) _STR(s)

#define UV_VERSION_STRING_BASE \
    STR(UV_VERSION_MAJOR)      \
    "." STR(UV_VERSION_MINOR) "." STR(UV_VERSION_PATCH)

static void init_info(Isolate* isolate)
{
    isolate->m_info = new NObject();

    isolate->m_info->add("fibjs", fibjs_version);
    isolate->m_info->add("node", STR(NODE_MAJOR_VERSION) "." STR(NODE_MINOR_VERSION) "." STR(NODE_PATCH_VERSION));

    exlib::string str;

    os_base::platform(str);
    isolate->m_info->add("platform", str);

    os_base::arch(str);
    isolate->m_info->add("arch", str);

#ifdef GIT_INFO
    isolate->m_info->add("git", GIT_INFO);
#endif

#if defined(__clang__)
    isolate->m_info->add("clang", STR(__clang_major__) "." STR(__clang_minor__));
#elif defined(__GNUC__)
    isolate->m_info->add("gcc", STR(__GNUC__) "." STR(__GNUC_MINOR__) "." STR(__GNUC_PATCHLEVEL__));
#ifdef __MUSL__
    isolate->m_info->add("musl", true);
#endif
#elif defined(_MSC_VER)
    isolate->m_info->add("msvc", STR(_MSC_VER));
#endif

    isolate->m_info->add("date", __DATE__ " " __TIME__);

#ifndef NDEBUG
    isolate->m_info->add("debug", true);
#endif

    isolate->m_info->add("modules", "115");
    isolate->m_info->add("napi", NODE_API_DEFAULT_MODULE_API_VERSION);

    obj_ptr<NObject> vender_list = new NObject();
    isolate->m_info->add("vender", vender_list);

    {
        char str[64];

        vender_list->add("ev", STR(EV_VERSION_MAJOR) "." STR(EV_VERSION_MINOR));
        vender_list->add("expat", STR(XML_MAJOR_VERSION) "." STR(XML_MINOR_VERSION) "." STR(XML_MICRO_VERSION));
        vender_list->add("gumbo", "0.10.0");
        vender_list->add("icu", U_ICU_VERSION);
        vender_list->add("jemalloc", "5.2.1");
        snprintf(str, sizeof(str), "%d.%d", leveldb::kMajorVersion, leveldb::kMinorVersion);
        vender_list->add("leveldb", str);
        vender_list->add("msgpack", msgpack_version());
        vender_list->add("openssl", OPENSSL_FULL_VERSION_STR);
        vender_list->add("pcre2", STR(PCRE2_MAJOR) "." STR(PCRE2_MINOR));
        vender_list->add("snappy", STR(SNAPPY_MAJOR) "." STR(SNAPPY_MINOR) "." STR(SNAPPY_PATCHLEVEL));
        vender_list->add("sqlite", SQLITE_VERSION);
        vender_list->add("unicode", U_UNICODE_VERSION);
        vender_list->add("uuid", "1.6.2");

#if UV_VERSION_IS_RELEASE
        vender_list->add("uv", UV_VERSION_STRING_BASE);
#else
        vender_list->add("uv", UV_VERSION_STRING_BASE "-" UV_VERSION_SUFFIX);
#endif

        vender_list->add("v8", v8::V8::GetVersion());

        vender_list->add("v8-snapshot", (bool)v8::internal::Snapshot::DefaultSnapshotBlob());

        vender_list->add("zlib", ZLIB_VERSION);
    }
}

result_t util_base::buildInfo(v8::Local<v8::Object>& retVal)
{
    Isolate* isolate = Isolate::current();
    v8::Local<v8::Context> context = isolate->context();

    if (!isolate->m_info)
        init_info(isolate);

    isolate->m_info->valueOf(retVal);

    {
        v8::Local<v8::Array> modules = v8::Array::New(isolate->m_isolate);
        retVal->Set(context, isolate->NewString("builtins"), modules).IsJust();

        RootModule* pModule = RootModule::g_root;
        intptr_t icnt = 0;

        modules->Set(context, (int32_t)(icnt++), isolate->NewString("buffer")).IsJust();

        while (pModule) {
            modules->Set(context, (int32_t)(icnt++), isolate->NewString(pModule->name())).IsJust();
            pModule = pModule->m_next;
        }
    }

    return 0;
}
}
