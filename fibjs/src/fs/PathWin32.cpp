#include "PathWin32.h"
#include "PathPosix.h"
#include "utf8.h"

namespace fibjs {

extern result_t _normalize(exlib::string path, exlib::string& retVal);
extern result_t _basename(exlib::string path, exlib::string ext, exlib::string& retVal);
extern result_t _extname(exlib::string path, exlib::string& retVal);
extern result_t _dirname(exlib::string path, exlib::string& retVal);
extern result_t _join(const v8::FunctionCallbackInfo<v8::Value>& args, exlib::string& retVal);
extern result_t _resolve(const v8::FunctionCallbackInfo<v8::Value>& args, exlib::string& retVal);
extern result_t _sep(exlib::string& retVal);
extern result_t _delimiter(exlib::string& retVal);

inline result_t _fullpath_win32(exlib::string path, exlib::string& retVal)
{
#ifdef _WIN32
    exlib::wstring str = utf8to16String(path);
    exlib::wchar utf16_buffer[MAX_PATH];

    DWORD utf16_len = GetFullPathNameW(str.c_str(), MAX_PATH, utf16_buffer, NULL);
    if (!utf16_len)
        return CHECK_ERROR(LastError());

    retVal = utf16to8String(utf16_buffer, (int32_t)utf16_len);
#else
    ThrowError("not supported on none Win32 platform !");
#endif
    return 0;
}

result_t PathWin32::normalize(exlib::string path, exlib::string& retVal)
{
    return _normalize(path, retVal);
}

result_t PathWin32::basename(exlib::string path, exlib::string ext, exlib::string& retVal)
{
    return _basename(path, ext, retVal);
}

result_t PathWin32::extname(exlib::string path, exlib::string& retVal)
{
    return _extname(path, retVal);
}

result_t PathWin32::dirname(exlib::string path, exlib::string& retVal)
{
    return _dirname(path, retVal);
}

result_t PathWin32::fullpath(exlib::string path, exlib::string& retVal)
{
    return _fullpath_win32(path, retVal);
}

result_t PathWin32::join(const v8::FunctionCallbackInfo<v8::Value>& args, exlib::string& retVal)
{
    return _join(args, retVal);
}

result_t PathWin32::resolve(const v8::FunctionCallbackInfo<v8::Value>& args, exlib::string& retVal)
{
    return _resolve(args, retVal);
}

result_t PathWin32::get_sep(exlib::string& retVal)
{
    retVal = "\\";
    return 0;
}

result_t PathWin32::get_delimiter(exlib::string& retVal)
{
    retVal = ";";
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