#include "PathWin32.h"
#include "PathPosix.h"
#include "path.h"

namespace fibjs {

result_t PathPosix::normalize(exlib::string path, exlib::string& retVal)
{
    return _normalize(path, retVal);
}

result_t PathPosix::basename(exlib::string path, exlib::string ext, exlib::string& retVal)
{
    return _basename(path, ext, retVal);
}

result_t PathPosix::extname(exlib::string path, exlib::string& retVal)
{
    return _extname(path, retVal);
}

result_t PathPosix::dirname(exlib::string path, exlib::string& retVal)
{
    return _dirname(path, retVal);
}

result_t PathPosix::fullpath(exlib::string path, exlib::string& retVal)
{
    return _fullpath(path, retVal);
}

result_t PathPosix::join(const v8::FunctionCallbackInfo<v8::Value>& args, exlib::string& retVal)
{
    return _join(args, retVal);
}

result_t PathPosix::resolve(const v8::FunctionCallbackInfo<v8::Value>& args, exlib::string& retVal)
{
    return _resolve(args, retVal);
}

result_t PathPosix::get_sep(exlib::string& retVal)
{
    retVal.assign(1, PATH_SLASH_POSIX);
    return 0;
}

result_t PathPosix::get_delimiter(exlib::string& retVal)
{
    retVal.assign(1, PATH_DELIMITER_POSIX);
    return 0;
}

result_t PathPosix::get_posix(obj_ptr<PathBase_base>& retVal)
{
    static obj_ptr<PathBase_base> obj;
    if (!obj) {
        obj = new PathPosix();
    }
    retVal = obj;
    return 0;
}

result_t PathPosix::get_win32(obj_ptr<PathBase_base>& retVal)
{
    static obj_ptr<PathBase_base> obj;
    if (!obj) {
        obj = new PathWin32();
    }
    retVal = obj;
    return 0;
}
}