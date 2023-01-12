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
#include <gd/include/gd.h>
#include <jpeg/include/jpeglib.h>
#include <png/include/png.h>
#include <pcre/pcre.h>
#include <tiff/include/tiffvers.h>
#include <mongo/include/mongo.h>
#include <mbedtls/mbedtls/version.h>
#include <snappy/include/snappy.h>
#include <leveldb/db.h>
#include <expat/include/expat.h>
#include <uv/include/uv/version.h>
#include <msgpack/version.hpp>

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

obj_ptr<NObject> g_info;
obj_ptr<NObject> g_vender;

#define _STR(s) #s
#define STR(s) _STR(s)

#define UV_VERSION_STRING_BASE \
    STR(UV_VERSION_MAJOR)      \
    "." STR(UV_VERSION_MINOR) "." STR(UV_VERSION_PATCH)

class init_info {
public:
    init_info()
    {
        g_info = new NObject();

        g_info->add("fibjs", fibjs_version);
        g_info->add("node", "0.0.0");

        exlib::string str;

        os_base::platform(str);
        g_info->add("platform", str);

        os_base::arch(str);
        g_info->add("arch", str);

#ifdef GIT_INFO
        g_info->add("git", GIT_INFO);
#endif

#if defined(__clang__)
        g_info->add("clang", STR(__clang_major__) "." STR(__clang_minor__));
#elif defined(__GNUC__)
        g_info->add("gcc", STR(__GNUC__) "." STR(__GNUC_MINOR__) "." STR(__GNUC_PATCHLEVEL__));
#elif defined(_MSC_VER)
        g_info->add("msvc", STR(_MSC_VER));
#endif

        g_info->add("date", __DATE__ " " __TIME__);

#ifndef NDEBUG
        g_info->add("debug", true);
#endif

        g_vender = new NObject();
        g_info->add("vender", g_vender);

        {
            char str[64];

            g_vender->add("ev", STR(EV_VERSION_MAJOR) "." STR(EV_VERSION_MINOR));
            g_vender->add("expat", STR(XML_MAJOR_VERSION) "." STR(XML_MINOR_VERSION) "." STR(XML_MICRO_VERSION));
            g_vender->add("gd", GD_VERSION_STRING);
            g_vender->add("gumbo", "0.10.0");
            g_vender->add("jpeg", STR(JPEG_LIB_VERSION_MAJOR) "." STR(JPEG_LIB_VERSION_MINOR));
            snprintf(str, sizeof(str), "%d.%d", leveldb::kMajorVersion, leveldb::kMinorVersion);
            g_vender->add("leveldb", str);
            g_vender->add("mbedtls", MBEDTLS_VERSION_STRING);
            g_vender->add("mongo", STR(MONGO_MAJOR) "." STR(MONGO_MINOR));
            g_vender->add("msgpack", msgpack_version());
            g_vender->add("pcre", STR(PCRE_MAJOR) "." STR(PCRE_MINOR));
            g_vender->add("png", PNG_LIBPNG_VER_STRING);
            g_vender->add("snappy", STR(SNAPPY_MAJOR) "." STR(SNAPPY_MINOR) "." STR(SNAPPY_PATCHLEVEL));
            g_vender->add("sqlite", SQLITE_VERSION);
            g_vender->add("tiff", TIFFLIB_VERSION_STR);
            g_vender->add("uuid", "1.6.2");

#if UV_VERSION_IS_RELEASE
            g_vender->add("uv", UV_VERSION_STRING_BASE);
#else
            g_vender->add("uv", UV_VERSION_STRING_BASE "-" UV_VERSION_SUFFIX);
#endif

            g_vender->add("v8", v8::V8::GetVersion());

            g_vender->add("v8-snapshot", (bool)v8::internal::Snapshot::DefaultSnapshotBlob());

            g_vender->add("zlib", ZLIB_VERSION);
        }
    }

} s_init_info;

result_t util_base::buildInfo(v8::Local<v8::Object>& retVal)
{
    Isolate* isolate = Isolate::current();
    v8::Local<v8::Context> context = isolate->context();

    v8::Local<v8::Value> v;
    g_info->valueOf(v);
    retVal = v8::Local<v8::Object>::Cast(v);

    {
        v8::Local<v8::Array> modules = v8::Array::New(isolate->m_isolate);
        retVal->Set(context, isolate->NewString("modules"), modules).Check();

        RootModule* pModule = RootModule::g_root;
        intptr_t icnt = 0;

        while (pModule) {
            modules->Set(context, (int32_t)(icnt++), isolate->NewString(pModule->name())).Check();
            pModule = pModule->m_next;
        }
    }

    return 0;
}
}
