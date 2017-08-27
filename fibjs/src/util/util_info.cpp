/*
 * utils_info.cpp
 *
 *  Created on: Jun 26, 2017
 *      Author: lion
 */

#include "object.h"
#include "version.h"
#include "ifs/util.h"
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
#include <zmq/include/zmq.h>

namespace v8 {
namespace internal {
    class StartupData;
    class Snapshot {
    public:
        static const v8::StartupData* DefaultSnapshotBlob();
    };
}
}

#ifndef _WIN32
#include "gitinfo.h"
#endif

namespace fibjs {

#define _STR(s) #s
#define STR(s) _STR(s)

result_t util_base::buildInfo(v8::Local<v8::Object>& retVal)
{
    Isolate* isolate = Isolate::current();
    retVal = v8::Object::New(isolate->m_isolate);

    retVal->Set(isolate->NewString("fibjs"), isolate->NewString(fibjs_version));

#ifdef GIT_INFO
    retVal->Set(isolate->NewString("git"), isolate->NewString(GIT_INFO));
#endif

#if defined(__clang__)
    retVal->Set(isolate->NewString("clang"),
        isolate->NewString(STR(__clang_major__) "." STR(__clang_minor__)));
#elif defined(__GNUC__)
    retVal->Set(isolate->NewString("gcc"),
        isolate->NewString(STR(__GNUC__) "." STR(__GNUC_MINOR__) "." STR(__GNUC_PATCHLEVEL__)));
#elif defined(_MSC_VER)
    retVal->Set(isolate->NewString("msvc"),
        isolate->NewString(STR(_MSC_VER)));
#endif

    retVal->Set(isolate->NewString("date"),
        isolate->NewString(__DATE__ " " __TIME__));

#ifndef NDEBUG
    retVal->Set(isolate->NewString("debug"), v8::True(isolate->m_isolate));
#endif

    {
        v8::Local<v8::Object> vender = v8::Object::New(isolate->m_isolate);
        char str[64];

        retVal->Set(isolate->NewString("vender"), vender);

        vender->Set(isolate->NewString("ev"),
            isolate->NewString(STR(EV_VERSION_MAJOR) "." STR(EV_VERSION_MINOR)));

        vender->Set(isolate->NewString("expat"),
            isolate->NewString(STR(XML_MAJOR_VERSION) "." STR(XML_MINOR_VERSION) "." STR(XML_MICRO_VERSION)));

        vender->Set(isolate->NewString("gd"), isolate->NewString(GD_VERSION_STRING));
        vender->Set(isolate->NewString("jpeg"), isolate->NewString(STR(JPEG_LIB_VERSION_MAJOR) "." STR(JPEG_LIB_VERSION_MINOR)));
        sprintf(str, "%d.%d", leveldb::kMajorVersion, leveldb::kMinorVersion);
        vender->Set(isolate->NewString("leveldb"), isolate->NewString(str));
        vender->Set(isolate->NewString("mongo"), isolate->NewString(STR(MONGO_MAJOR) "." STR(MONGO_MINOR)));
        vender->Set(isolate->NewString("pcre"), isolate->NewString(STR(PCRE_MAJOR) "." STR(PCRE_MINOR)));
        vender->Set(isolate->NewString("png"), isolate->NewString(PNG_LIBPNG_VER_STRING));
        vender->Set(isolate->NewString("mbedtls"), isolate->NewString(MBEDTLS_VERSION_STRING));
        vender->Set(isolate->NewString("snappy"),
            isolate->NewString(STR(SNAPPY_MAJOR) "." STR(SNAPPY_MINOR) "." STR(SNAPPY_PATCHLEVEL)));
        vender->Set(isolate->NewString("sqlite"), isolate->NewString(SQLITE_VERSION));
        vender->Set(isolate->NewString("tiff"), isolate->NewString(TIFFLIB_VERSION_STR));
        vender->Set(isolate->NewString("uuid"), isolate->NewString("1.6.2"));
        vender->Set(isolate->NewString("v8"), isolate->NewString(v8::V8::GetVersion()));

        vender->Set(isolate->NewString("v8-snapshot"),
            v8::internal::Snapshot::DefaultSnapshotBlob() ? v8::True(isolate->m_isolate) : v8::False(isolate->m_isolate));

        vender->Set(isolate->NewString("zlib"), isolate->NewString(ZLIB_VERSION));
        vender->Set(isolate->NewString("zmq"), isolate->NewString(STR(ZMQ_VERSION_MAJOR) "." STR(ZMQ_VERSION_MINOR)));
    }

    return 0;
}
}