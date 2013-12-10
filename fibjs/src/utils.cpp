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
#include <zmq/zmq.h>

namespace fibjs
{

#define _STR(s) #s
#define STR(s)  _STR(s)

static const char s_version[] = "0.1.0";

result_t utils_base::buildInfo(v8::Handle<v8::Object>& retVal)
{
	retVal = v8::Object::New();

	retVal->Set(v8::String::New("fibjs"), v8::String::New(s_version));

#ifdef SVN_INFO
	retVal->Set(v8::String::New("svn"), v8::Integer::New(SVN_INFO));
#endif

	retVal->Set(v8::String::New("build"),
			v8::String::New(__DATE__ " " __TIME__));

	v8::Handle < v8::Object > vender = v8::Object::New();

	retVal->Set(v8::String::New("vender"), vender);

	vender->Set(v8::String::New("ev"),
			v8::String::New( STR(EV_VERSION_MAJOR) "." STR(EV_VERSION_MINOR)));
	vender->Set(v8::String::New("exif"), v8::String::New("0.6.21"));
	vender->Set(v8::String::New("gd"), v8::String::New(GD_VERSION_STRING));
	vender->Set(v8::String::New("jpeg"), v8::String::New(
			STR(JPEG_LIB_VERSION_MAJOR) "." STR(JPEG_LIB_VERSION_MINOR)));
	vender->Set(v8::String::New("log4cpp"), v8::String::New("1.0"));
	vender->Set(v8::String::New("mongo"), v8::String::New(
			STR(MONGO_MAJOR) "." STR(MONGO_MINOR)));
	vender->Set(v8::String::New("pcre"), v8::String::New(
			STR(PCRE_MAJOR) "." STR(PCRE_MINOR)));
	vender->Set(v8::String::New("png"), v8::String::New(PNG_LIBPNG_VER_STRING));
	vender->Set(v8::String::New("sqlite"), v8::String::New(SQLITE_VERSION));
	vender->Set(v8::String::New("tiff"), v8::String::New(TIFFLIB_VERSION_STR));
	vender->Set(v8::String::New("uuid"), v8::String::New("1.6.2"));
	vender->Set(v8::String::New("v8"), v8::String::New(v8::V8::GetVersion()));
	vender->Set(v8::String::New("zlib"), v8::String::New(ZLIB_VERSION));
	vender->Set(v8::String::New("zmp"), v8::String::New(
			STR(ZMQ_VERSION_MAJOR) "." STR(ZMQ_VERSION_MINOR)));

	return 0;
}

}
