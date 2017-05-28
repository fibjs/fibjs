#include "PathWin32.h"
#include "PathPosix.h"
#include "path.h"

namespace fibjs {

result_t PathWin32::normalize(exlib::string path, exlib::string& retVal)
{
    return _normalize_win32(path, retVal);
}

result_t PathWin32::basename(exlib::string path, exlib::string ext, exlib::string& retVal)
{
    return _basename_win32(path, ext, retVal);
}

result_t PathWin32::extname(exlib::string path, exlib::string& retVal)
{
    return _extname_win32(path, retVal);
}

result_t PathWin32::dirname(exlib::string path, exlib::string& retVal)
{
    return _dirname_win32(path, retVal);
}

result_t PathWin32::fullpath(exlib::string path, exlib::string& retVal)
{
    return _fullpath_win32(path, retVal);
}

result_t PathWin32::join(const v8::FunctionCallbackInfo<v8::Value>& args, exlib::string& retVal)
{
    return _join_win32(args, retVal);
}

result_t PathWin32::resolve(const v8::FunctionCallbackInfo<v8::Value>& args, exlib::string& retVal)
{
    return _resolve_win32(args, retVal);
}

result_t PathWin32::get_sep(exlib::string& retVal)
{
    retVal.assign(1, PATH_SLASH_WIN32);
    return 0;
}

result_t PathWin32::get_delimiter(exlib::string& retVal)
{
    retVal.assign(1, PATH_DELIMITER_WIN32);
    return 0;
}

result_t PathWin32::get_posix(obj_ptr<PathBase_base>& retVal)
{
    static obj_ptr<PathBase_base> obj;
    if (!obj) {
        obj = new PathPosix();
    }
    retVal = obj;
    return 0;
}

result_t PathWin32::get_win32(obj_ptr<PathBase_base>& retVal)
{
    static obj_ptr<PathBase_base> obj;
    if (!obj) {
        obj = new PathWin32();
    }
    retVal = obj;
    return 0;
}
}