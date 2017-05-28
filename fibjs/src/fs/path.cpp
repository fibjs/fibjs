/*
 * path.cpp
 *
 *  Created on: Apr 7, 2012
 *      Author: lion
 */

#include "object.h"
#include "PathPosix.h"
#include "PathWin32.h"
#include "path.h"

namespace fibjs {

DECLARE_MODULE(path);

result_t path_base::normalize(exlib::string path, exlib::string& retVal)
{
    return _normalize(path, retVal);
}

result_t path_base::basename(exlib::string path, exlib::string ext,
    exlib::string& retVal)
{
    return _basename(path, ext, retVal);
}

result_t path_base::extname(exlib::string path, exlib::string& retVal)
{
    return _extname(path, retVal);
}

result_t path_base::dirname(exlib::string path, exlib::string& retVal)
{
    return _dirname(path, retVal);
}

result_t path_base::fullpath(exlib::string path, exlib::string& retVal)
{
#ifdef _WIN32
    return _fullpath_win32(path, retVal);
#else
    return _fullpath_posix(path, retVal);
#endif
}

result_t path_base::join(const v8::FunctionCallbackInfo<v8::Value>& args, exlib::string& retVal)
{
    return _join(args, retVal);
}

result_t path_base::resolve(const v8::FunctionCallbackInfo<v8::Value>& args, exlib::string& retVal)
{
    return _resolve(args, retVal);
}

result_t path_base::get_sep(exlib::string& retVal)
{
    return _sep(retVal);
}

result_t path_base::get_delimiter(exlib::string& retVal)
{
    return _delimiter(retVal);
}

result_t path_base::get_posix(obj_ptr<PathBase_base>& retVal)
{
    static obj_ptr<PathBase_base> obj;
    if (!obj) {
        obj = new PathPosix();
    }
    retVal = obj;
    return 0;
}

result_t path_base::get_win32(obj_ptr<PathBase_base>& retVal)
{
    static obj_ptr<PathBase_base> obj;
    if (!obj) {
        obj = new PathWin32();
    }
    retVal = obj;
    return 0;
}
}
