/*
 * PathPosix.h
 *
 *  Created on: May 27, 2017
 *      Author: ngot
 */

#ifndef PATHPOSIX_H
#define PATHPOSIX_H

#include "ifs/PathPosix.h"

namespace fibjs {

class PathPosix : public PathPosix_base {

public:
    // PathPosix_base
    virtual result_t normalize(exlib::string path, exlib::string& retVal);
    virtual result_t basename(exlib::string path, exlib::string ext, exlib::string& retVal);
    virtual result_t extname(exlib::string path, exlib::string& retVal);
    virtual result_t dirname(exlib::string path, exlib::string& retVal);
    virtual result_t fullpath(exlib::string path, exlib::string& retVal);
    virtual result_t join(const v8::FunctionCallbackInfo<v8::Value>& args, exlib::string& retVal);
    virtual result_t resolve(const v8::FunctionCallbackInfo<v8::Value>& args, exlib::string& retVal);
    virtual result_t get_sep(exlib::string& retVal);
    virtual result_t get_delimiter(exlib::string& retVal);
    virtual result_t get_posix(obj_ptr<PathPosix_base>& retVal);
    virtual result_t get_win32(obj_ptr<PathWin32_base>& retVal);
};
}

#endif
