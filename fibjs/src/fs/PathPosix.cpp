#include "ifs/process.h"
#include "PathWin32.h"
#include "PathPosix.h"

namespace fibjs {

extern result_t _normalize(exlib::string path, exlib::string& retVal);
extern result_t _basename(exlib::string path, exlib::string ext, exlib::string& retVal);
extern result_t _extname(exlib::string path, exlib::string& retVal);
extern result_t _dirname(exlib::string path, exlib::string& retVal);
extern result_t _join(const v8::FunctionCallbackInfo<v8::Value>& args, exlib::string& retVal);
extern result_t _resolve(const v8::FunctionCallbackInfo<v8::Value>& args, exlib::string& retVal);
extern result_t _sep(exlib::string& retVal);
extern result_t _delimiter(exlib::string& retVal);

inline result_t _fullpath_posix(exlib::string path, exlib::string& retVal)
{
    if (isPathSlash(path.c_str()[0]))
        return _normalize(path, retVal);

    exlib::string str;

    process_base::cwd(str);
    str.append(1, PATH_SLASH);
    str.append(path);

    return _normalize(str, retVal);
}

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
    return _fullpath_posix(path, retVal);
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
    retVal = ":";
    return 0;
}

result_t PathPosix::get_delimiter(exlib::string& retVal)
{
    retVal = ":";
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