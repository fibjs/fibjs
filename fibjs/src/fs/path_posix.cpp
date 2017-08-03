/*
 * path.cpp
 *
 *  Created on: May 30, 2017
 *      Author: lion
 */

#include "object.h"
#include "path.h"

namespace fibjs {

#ifndef _WIN32
DECLARE_MODULE_EX(path, path_posix);
#endif

result_t path_posix_base::normalize(exlib::string path, exlib::string& retVal)
{
    return _normalize(path, retVal);
}

result_t path_posix_base::basename(exlib::string path, exlib::string ext, exlib::string& retVal)
{
    return _basename(path, ext, retVal);
}

result_t path_posix_base::extname(exlib::string path, exlib::string& retVal)
{
    return _extname(path, retVal);
}

result_t path_posix_base::dirname(exlib::string path, exlib::string& retVal)
{
    return _dirname(path, retVal);
}

result_t path_posix_base::fullpath(exlib::string path, exlib::string& retVal)
{
    return _fullpath(path, retVal);
}

result_t path_posix_base::isAbsolute(exlib::string path, bool& retVal)
{
    retVal = isPosixPathSlash(path.c_str()[0]);
    return 0;
}

result_t path_posix_base::join(v8::Local<v8::Array> ps, exlib::string& retVal)
{
    return _join(ps, retVal);
}

result_t path_posix_base::resolve(v8::Local<v8::Array> ps, exlib::string& retVal)
{
    return _resolve(ps, retVal);
}

result_t path_posix_base::get_sep(exlib::string& retVal)
{
    retVal.assign(1, PATH_SLASH_POSIX);
    return 0;
}

result_t path_posix_base::get_delimiter(exlib::string& retVal)
{
    retVal.assign(1, PATH_DELIMITER_POSIX);
    return 0;
}

result_t path_posix_base::get_posix(v8::Local<v8::Object>& retVal)
{
    retVal = path_posix_base::class_info().getModule(Isolate::current());
    return 0;
}

result_t path_posix_base::get_win32(v8::Local<v8::Object>& retVal)
{
    retVal = path_win32_base::class_info().getModule(Isolate::current());
    return 0;
}
}