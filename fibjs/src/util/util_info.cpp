/*
 * utils_info.cpp
 *
 *  Created on: Jun 26, 2017
 *      Author: lion
 */

#include "object.h"
#include "version.h"
#include "ifs/util.h"
#include <src/snapshot/snapshot.h>
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

    retVal->Set(isolate->NewFromUtf8("fibjs"), isolate->NewFromUtf8(fibjs_version));

#ifdef GIT_INFO
    retVal->Set(isolate->NewFromUtf8("git"), isolate->NewFromUtf8(GIT_INFO));
#endif

#if defined(__clang__)
    retVal->Set(isolate->NewFromUtf8("clang"),
        isolate->NewFromUtf8(STR(__clang_major__) "." STR(__clang_minor__)));
#elif defined(__GNUC__)
    retVal->Set(isolate->NewFromUtf8("gcc"),
        isolate->NewFromUtf8(STR(__GNUC__) "." STR(__GNUC_MINOR__) "." STR(__GNUC_PATCHLEVEL__)));
#elif defined(_MSC_VER)
    retVal->Set(isolate->NewFromUtf8("msvc"),
        isolate->NewFromUtf8(STR(_MSC_VER)));
#endif

    retVal->Set(isolate->NewFromUtf8("date"),
        isolate->NewFromUtf8(__DATE__ " " __TIME__));

#ifndef NDEBUG
    retVal->Set(isolate->NewFromUtf8("debug"), v8::True(isolate->m_isolate));
#endif

    {
        v8::Local<v8::Object> vender = v8::Object::New(isolate->m_isolate);
        char str[64];

        retVal->Set(isolate->NewFromUtf8("vender"), vender);

        vender->Set(isolate->NewFromUtf8("ev"),
            isolate->NewFromUtf8(STR(EV_VERSION_MAJOR) "." STR(EV_VERSION_MINOR)));

        vender->Set(isolate->NewFromUtf8("expat"),
            isolate->NewFromUtf8(STR(XML_MAJOR_VERSION) "." STR(XML_MINOR_VERSION) "." STR(XML_MICRO_VERSION)));

        vender->Set(isolate->NewFromUtf8("gd"), isolate->NewFromUtf8(GD_VERSION_STRING));
        vender->Set(isolate->NewFromUtf8("jpeg"), isolate->NewFromUtf8(STR(JPEG_LIB_VERSION_MAJOR) "." STR(JPEG_LIB_VERSION_MINOR)));
        sprintf(str, "%d.%d", leveldb::kMajorVersion, leveldb::kMinorVersion);
        vender->Set(isolate->NewFromUtf8("leveldb"), isolate->NewFromUtf8(str));
        vender->Set(isolate->NewFromUtf8("mongo"), isolate->NewFromUtf8(STR(MONGO_MAJOR) "." STR(MONGO_MINOR)));
        vender->Set(isolate->NewFromUtf8("pcre"), isolate->NewFromUtf8(STR(PCRE_MAJOR) "." STR(PCRE_MINOR)));
        vender->Set(isolate->NewFromUtf8("png"), isolate->NewFromUtf8(PNG_LIBPNG_VER_STRING));
        vender->Set(isolate->NewFromUtf8("mbedtls"), isolate->NewFromUtf8(MBEDTLS_VERSION_STRING));
        vender->Set(isolate->NewFromUtf8("snappy"),
            isolate->NewFromUtf8(STR(SNAPPY_MAJOR) "." STR(SNAPPY_MINOR) "." STR(SNAPPY_PATCHLEVEL)));
        vender->Set(isolate->NewFromUtf8("sqlite"), isolate->NewFromUtf8(SQLITE_VERSION));
        vender->Set(isolate->NewFromUtf8("tiff"), isolate->NewFromUtf8(TIFFLIB_VERSION_STR));
        vender->Set(isolate->NewFromUtf8("uuid"), isolate->NewFromUtf8("1.6.2"));
        vender->Set(isolate->NewFromUtf8("v8"), isolate->NewFromUtf8(v8::V8::GetVersion()));

        vender->Set(isolate->NewFromUtf8("v8-snapshot"),
            v8::internal::Snapshot::DefaultSnapshotBlob() ? v8::True(isolate->m_isolate) : v8::False(isolate->m_isolate));

        vender->Set(isolate->NewFromUtf8("zlib"), isolate->NewFromUtf8(ZLIB_VERSION));
        vender->Set(isolate->NewFromUtf8("zmq"), isolate->NewFromUtf8(STR(ZMQ_VERSION_MAJOR) "." STR(ZMQ_VERSION_MINOR)));
    }

    return 0;
}
}