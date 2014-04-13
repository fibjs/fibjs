/*
 * utils.cpp
 *
 *  Created on: Dec 10, 2013
 *      Author: lion
 */

#include "ifs/utils.h"
#include <v8/v8.h>
#include <zlib.h>
#include <sqlite3.h>
#include <ev.h>
#include <gd.h>
#include <jpeglib.h>
#include <png.h>
#include <pcre.h>
#include <tiffvers.h>
#include <mongo/mongo.h>
#include <polarssl/version.h>

namespace fibjs
{

#define _STR(s) #s
#define STR(s)  _STR(s)

static const char s_version[] = "0.1.0";

result_t utils_base::buildInfo(v8::Local<v8::Object> &retVal)
{
    retVal = v8::Object::New(isolate);

    retVal->Set(v8::String::NewFromUtf8(isolate, "fibjs"), v8::String::NewFromUtf8(isolate, s_version));

#ifdef SVN_INFO
    retVal->Set(v8::String::NewFromUtf8(isolate, "svn"), v8::Integer::New(isolate, SVN_INFO));
#endif

    retVal->Set(v8::String::NewFromUtf8(isolate, "build"),
                v8::String::NewFromUtf8(isolate, __DATE__ " " __TIME__));

#ifndef NDEBUG
    retVal->Set(v8::String::NewFromUtf8(isolate, "debug"), v8::True(isolate));
#endif

    {
        v8::Local<v8::Object> vender = v8::Object::New(isolate);

        retVal->Set(v8::String::NewFromUtf8(isolate, "vender"), vender);

        vender->Set(v8::String::NewFromUtf8(isolate, "ev"),
                    v8::String::NewFromUtf8(isolate,  STR(EV_VERSION_MAJOR) "." STR(EV_VERSION_MINOR)));
        vender->Set(v8::String::NewFromUtf8(isolate, "gd"), v8::String::NewFromUtf8(isolate, GD_VERSION_STRING));
        vender->Set(v8::String::NewFromUtf8(isolate, "jpeg"), v8::String::NewFromUtf8(isolate,
                    STR(JPEG_LIB_VERSION_MAJOR) "." STR(JPEG_LIB_VERSION_MINOR)));
        vender->Set(v8::String::NewFromUtf8(isolate, "log4cpp"), v8::String::NewFromUtf8(isolate, "1.0"));
        vender->Set(v8::String::NewFromUtf8(isolate, "mongo"), v8::String::NewFromUtf8(isolate,
                    STR(MONGO_MAJOR) "." STR(MONGO_MINOR)));
        vender->Set(v8::String::NewFromUtf8(isolate, "pcre"), v8::String::NewFromUtf8(isolate,
                    STR(PCRE_MAJOR) "." STR(PCRE_MINOR)));
        vender->Set(v8::String::NewFromUtf8(isolate, "png"), v8::String::NewFromUtf8(isolate, PNG_LIBPNG_VER_STRING));
        vender->Set(v8::String::NewFromUtf8(isolate, "polarssl"), v8::String::NewFromUtf8(isolate, POLARSSL_VERSION_STRING));
        vender->Set(v8::String::NewFromUtf8(isolate, "sqlite"), v8::String::NewFromUtf8(isolate, SQLITE_VERSION));
        vender->Set(v8::String::NewFromUtf8(isolate, "tiff"), v8::String::NewFromUtf8(isolate, TIFFLIB_VERSION_STR));
        vender->Set(v8::String::NewFromUtf8(isolate, "uuid"), v8::String::NewFromUtf8(isolate, "1.6.2"));
        vender->Set(v8::String::NewFromUtf8(isolate, "v8"), v8::String::NewFromUtf8(isolate, v8::V8::GetVersion()));
        vender->Set(v8::String::NewFromUtf8(isolate, "zlib"), v8::String::NewFromUtf8(isolate, ZLIB_VERSION));
    }

    return 0;
}

}
