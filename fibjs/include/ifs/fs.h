/***************************************************************************
 *                                                                         *
 *   This file was automatically generated using idlc.js                   *
 *   PLEASE DO NOT EDIT!!!!                                                *
 *                                                                         *
 ***************************************************************************/

#pragma once

/**
 @author Leo Hoo <lion@9465.net>
 */

#include "../object.h"

namespace fibjs {

class fs_constants_base;
class Stat_base;
class FileHandle_base;
class Buffer_base;
class SeekableStream_base;
class BufferedStream_base;
class FSWatcher_base;
class StatsWatcher_base;

class fs_base : public object_base {
    DECLARE_CLASS(fs_base);

public:
    enum {
        C_SEEK_SET = 0,
        C_SEEK_CUR = 1,
        C_SEEK_END = 2
    };

public:
    // fs_base
    static result_t exists(exlib::string path, bool& retVal, AsyncEvent* ac);
    static result_t access(exlib::string path, int32_t mode, AsyncEvent* ac);
    static result_t link(exlib::string oldPath, exlib::string newPath, AsyncEvent* ac);
    static result_t unlink(exlib::string path, AsyncEvent* ac);
    static result_t mkdir(exlib::string path, int32_t mode, AsyncEvent* ac);
    static result_t mkdir(exlib::string path, v8::Local<v8::Object> opt, AsyncEvent* ac);
    static result_t rmdir(exlib::string path, AsyncEvent* ac);
    static result_t rename(exlib::string from, exlib::string to, AsyncEvent* ac);
    static result_t copyFile(exlib::string from, exlib::string to, int32_t mode, AsyncEvent* ac);
    static result_t chmod(exlib::string path, int32_t mode, AsyncEvent* ac);
    static result_t lchmod(exlib::string path, int32_t mode, AsyncEvent* ac);
    static result_t chown(exlib::string path, int32_t uid, int32_t gid, AsyncEvent* ac);
    static result_t lchown(exlib::string path, int32_t uid, int32_t gid, AsyncEvent* ac);
    static result_t stat(exlib::string path, obj_ptr<Stat_base>& retVal, AsyncEvent* ac);
    static result_t lstat(exlib::string path, obj_ptr<Stat_base>& retVal, AsyncEvent* ac);
    static result_t fstat(FileHandle_base* fd, obj_ptr<Stat_base>& retVal, AsyncEvent* ac);
    static result_t readlink(exlib::string path, exlib::string& retVal, AsyncEvent* ac);
    static result_t realpath(exlib::string path, exlib::string& retVal, AsyncEvent* ac);
    static result_t symlink(exlib::string target, exlib::string linkpath, exlib::string type, AsyncEvent* ac);
    static result_t truncate(exlib::string path, int32_t len, AsyncEvent* ac);
    static result_t read(FileHandle_base* fd, Buffer_base* buffer, int32_t offset, int32_t length, int32_t position, int32_t& retVal, AsyncEvent* ac);
    static result_t fchmod(FileHandle_base* fd, int32_t mode, AsyncEvent* ac);
    static result_t fchown(FileHandle_base* fd, int32_t uid, int32_t gid, AsyncEvent* ac);
    static result_t fdatasync(FileHandle_base* fd, AsyncEvent* ac);
    static result_t fsync(FileHandle_base* fd, AsyncEvent* ac);
    static result_t readdir(exlib::string path, obj_ptr<NArray>& retVal, AsyncEvent* ac);
    static result_t readdir(exlib::string path, v8::Local<v8::Object> opts, obj_ptr<NArray>& retVal, AsyncEvent* ac);
    static result_t openFile(exlib::string fname, exlib::string flags, obj_ptr<SeekableStream_base>& retVal, AsyncEvent* ac);
    static result_t open(exlib::string fname, exlib::string flags, int32_t mode, obj_ptr<FileHandle_base>& retVal, AsyncEvent* ac);
    static result_t close(FileHandle_base* fd, AsyncEvent* ac);
    static result_t openTextStream(exlib::string fname, exlib::string flags, obj_ptr<BufferedStream_base>& retVal, AsyncEvent* ac);
    static result_t readTextFile(exlib::string fname, exlib::string& retVal, AsyncEvent* ac);
    static result_t readFile(exlib::string fname, exlib::string encoding, Variant& retVal, AsyncEvent* ac);
    static result_t readFile(exlib::string fname, v8::Local<v8::Object> options, Variant& retVal, AsyncEvent* ac);
    static result_t readLines(exlib::string fname, int32_t maxlines, v8::Local<v8::Array>& retVal);
    static result_t write(FileHandle_base* fd, Buffer_base* buffer, int32_t offset, int32_t length, int32_t position, int32_t& retVal, AsyncEvent* ac);
    static result_t write(FileHandle_base* fd, exlib::string string, int32_t position, exlib::string encoding, int32_t& retVal, AsyncEvent* ac);
    static result_t writeTextFile(exlib::string fname, exlib::string txt, AsyncEvent* ac);
    static result_t writeFile(exlib::string fname, Buffer_base* data, exlib::string opt, AsyncEvent* ac);
    static result_t writeFile(exlib::string fname, Buffer_base* data, v8::Local<v8::Object> options, AsyncEvent* ac);
    static result_t writeFile(exlib::string fname, exlib::string data, exlib::string opt, AsyncEvent* ac);
    static result_t writeFile(exlib::string fname, exlib::string data, v8::Local<v8::Object> options, AsyncEvent* ac);
    static result_t appendFile(exlib::string fname, Buffer_base* data, AsyncEvent* ac);
    static result_t setZipFS(exlib::string fname, Buffer_base* data);
    static result_t clearZipFS(exlib::string fname);
    static result_t watch(exlib::string fname, obj_ptr<FSWatcher_base>& retVal);
    static result_t watch(exlib::string fname, v8::Local<v8::Function> callback, obj_ptr<FSWatcher_base>& retVal);
    static result_t watch(exlib::string fname, v8::Local<v8::Object> options, obj_ptr<FSWatcher_base>& retVal);
    static result_t watch(exlib::string fname, v8::Local<v8::Object> options, v8::Local<v8::Function> callback, obj_ptr<FSWatcher_base>& retVal);
    static result_t watchFile(exlib::string fname, v8::Local<v8::Function> callback, obj_ptr<StatsWatcher_base>& retVal);
    static result_t watchFile(exlib::string fname, v8::Local<v8::Object> options, v8::Local<v8::Function> callback, obj_ptr<StatsWatcher_base>& retVal);
    static result_t unwatchFile(exlib::string fname);
    static result_t unwatchFile(exlib::string fname, v8::Local<v8::Function> callback);

public:
    static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        CONSTRUCT_INIT();

        isolate->m_isolate->ThrowException(
            isolate->NewString("not a constructor"));
    }

public:
    static void s_static_exists(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_access(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_link(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_unlink(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_mkdir(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_rmdir(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_rename(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_copyFile(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_chmod(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_lchmod(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_chown(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_lchown(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_stat(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_lstat(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_fstat(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_readlink(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_realpath(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_symlink(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_truncate(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_read(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_fchmod(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_fchown(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_fdatasync(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_fsync(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_readdir(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_openFile(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_open(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_close(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_openTextStream(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_readTextFile(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_readFile(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_readLines(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_write(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_writeTextFile(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_writeFile(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_appendFile(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_setZipFS(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_clearZipFS(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_watch(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_watchFile(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_unwatchFile(const v8::FunctionCallbackInfo<v8::Value>& args);

public:
    ASYNC_STATICVALUE2(fs_base, exists, exlib::string, bool);
    ASYNC_STATIC2(fs_base, access, exlib::string, int32_t);
    ASYNC_STATIC2(fs_base, link, exlib::string, exlib::string);
    ASYNC_STATIC1(fs_base, unlink, exlib::string);
    ASYNC_STATIC2(fs_base, mkdir, exlib::string, int32_t);
    ASYNC_STATIC2(fs_base, mkdir, exlib::string, v8::Local<v8::Object>);
    ASYNC_STATIC1(fs_base, rmdir, exlib::string);
    ASYNC_STATIC2(fs_base, rename, exlib::string, exlib::string);
    ASYNC_STATIC3(fs_base, copyFile, exlib::string, exlib::string, int32_t);
    ASYNC_STATIC2(fs_base, chmod, exlib::string, int32_t);
    ASYNC_STATIC2(fs_base, lchmod, exlib::string, int32_t);
    ASYNC_STATIC3(fs_base, chown, exlib::string, int32_t, int32_t);
    ASYNC_STATIC3(fs_base, lchown, exlib::string, int32_t, int32_t);
    ASYNC_STATICVALUE2(fs_base, stat, exlib::string, obj_ptr<Stat_base>);
    ASYNC_STATICVALUE2(fs_base, lstat, exlib::string, obj_ptr<Stat_base>);
    ASYNC_STATICVALUE2(fs_base, fstat, FileHandle_base*, obj_ptr<Stat_base>);
    ASYNC_STATICVALUE2(fs_base, readlink, exlib::string, exlib::string);
    ASYNC_STATICVALUE2(fs_base, realpath, exlib::string, exlib::string);
    ASYNC_STATIC3(fs_base, symlink, exlib::string, exlib::string, exlib::string);
    ASYNC_STATIC2(fs_base, truncate, exlib::string, int32_t);
    ASYNC_STATICVALUE6(fs_base, read, FileHandle_base*, Buffer_base*, int32_t, int32_t, int32_t, int32_t);
    ASYNC_STATIC2(fs_base, fchmod, FileHandle_base*, int32_t);
    ASYNC_STATIC3(fs_base, fchown, FileHandle_base*, int32_t, int32_t);
    ASYNC_STATIC1(fs_base, fdatasync, FileHandle_base*);
    ASYNC_STATIC1(fs_base, fsync, FileHandle_base*);
    ASYNC_STATICVALUE2(fs_base, readdir, exlib::string, obj_ptr<NArray>);
    ASYNC_STATICVALUE3(fs_base, readdir, exlib::string, v8::Local<v8::Object>, obj_ptr<NArray>);
    ASYNC_STATICVALUE3(fs_base, openFile, exlib::string, exlib::string, obj_ptr<SeekableStream_base>);
    ASYNC_STATICVALUE4(fs_base, open, exlib::string, exlib::string, int32_t, obj_ptr<FileHandle_base>);
    ASYNC_STATIC1(fs_base, close, FileHandle_base*);
    ASYNC_STATICVALUE3(fs_base, openTextStream, exlib::string, exlib::string, obj_ptr<BufferedStream_base>);
    ASYNC_STATICVALUE2(fs_base, readTextFile, exlib::string, exlib::string);
    ASYNC_STATICVALUE3(fs_base, readFile, exlib::string, exlib::string, Variant);
    ASYNC_STATICVALUE3(fs_base, readFile, exlib::string, v8::Local<v8::Object>, Variant);
    ASYNC_STATICVALUE6(fs_base, write, FileHandle_base*, Buffer_base*, int32_t, int32_t, int32_t, int32_t);
    ASYNC_STATICVALUE5(fs_base, write, FileHandle_base*, exlib::string, int32_t, exlib::string, int32_t);
    ASYNC_STATIC2(fs_base, writeTextFile, exlib::string, exlib::string);
    ASYNC_STATIC3(fs_base, writeFile, exlib::string, Buffer_base*, exlib::string);
    ASYNC_STATIC3(fs_base, writeFile, exlib::string, Buffer_base*, v8::Local<v8::Object>);
    ASYNC_STATIC3(fs_base, writeFile, exlib::string, exlib::string, exlib::string);
    ASYNC_STATIC3(fs_base, writeFile, exlib::string, exlib::string, v8::Local<v8::Object>);
    ASYNC_STATIC2(fs_base, appendFile, exlib::string, Buffer_base*);
};
}

#include "ifs/fs_constants.h"
#include "ifs/Stat.h"
#include "ifs/FileHandle.h"
#include "ifs/Buffer.h"
#include "ifs/SeekableStream.h"
#include "ifs/BufferedStream.h"
#include "ifs/FSWatcher.h"
#include "ifs/StatsWatcher.h"

namespace fibjs {
inline ClassInfo& fs_base::class_info()
{
    static ClassData::ClassMethod s_method[] = {
        { "exists", s_static_exists, true, true },
        { "existsSync", s_static_exists, true, false },
        { "access", s_static_access, true, true },
        { "accessSync", s_static_access, true, false },
        { "link", s_static_link, true, true },
        { "linkSync", s_static_link, true, false },
        { "unlink", s_static_unlink, true, true },
        { "unlinkSync", s_static_unlink, true, false },
        { "mkdir", s_static_mkdir, true, true },
        { "mkdirSync", s_static_mkdir, true, false },
        { "rmdir", s_static_rmdir, true, true },
        { "rmdirSync", s_static_rmdir, true, false },
        { "rename", s_static_rename, true, true },
        { "renameSync", s_static_rename, true, false },
        { "copyFile", s_static_copyFile, true, true },
        { "copyFileSync", s_static_copyFile, true, false },
        { "chmod", s_static_chmod, true, true },
        { "chmodSync", s_static_chmod, true, false },
        { "lchmod", s_static_lchmod, true, true },
        { "lchmodSync", s_static_lchmod, true, false },
        { "chown", s_static_chown, true, true },
        { "chownSync", s_static_chown, true, false },
        { "lchown", s_static_lchown, true, true },
        { "lchownSync", s_static_lchown, true, false },
        { "stat", s_static_stat, true, true },
        { "statSync", s_static_stat, true, false },
        { "lstat", s_static_lstat, true, true },
        { "lstatSync", s_static_lstat, true, false },
        { "fstat", s_static_fstat, true, true },
        { "fstatSync", s_static_fstat, true, false },
        { "readlink", s_static_readlink, true, true },
        { "readlinkSync", s_static_readlink, true, false },
        { "realpath", s_static_realpath, true, true },
        { "realpathSync", s_static_realpath, true, false },
        { "symlink", s_static_symlink, true, true },
        { "symlinkSync", s_static_symlink, true, false },
        { "truncate", s_static_truncate, true, true },
        { "truncateSync", s_static_truncate, true, false },
        { "read", s_static_read, true, true },
        { "readSync", s_static_read, true, false },
        { "fchmod", s_static_fchmod, true, true },
        { "fchmodSync", s_static_fchmod, true, false },
        { "fchown", s_static_fchown, true, true },
        { "fchownSync", s_static_fchown, true, false },
        { "fdatasync", s_static_fdatasync, true, true },
        { "fdatasyncSync", s_static_fdatasync, true, false },
        { "fsync", s_static_fsync, true, true },
        { "fsyncSync", s_static_fsync, true, false },
        { "readdir", s_static_readdir, true, true },
        { "readdirSync", s_static_readdir, true, false },
        { "openFile", s_static_openFile, true, true },
        { "openFileSync", s_static_openFile, true, false },
        { "open", s_static_open, true, true },
        { "openSync", s_static_open, true, false },
        { "close", s_static_close, true, true },
        { "closeSync", s_static_close, true, false },
        { "openTextStream", s_static_openTextStream, true, true },
        { "openTextStreamSync", s_static_openTextStream, true, false },
        { "readTextFile", s_static_readTextFile, true, true },
        { "readTextFileSync", s_static_readTextFile, true, false },
        { "readFile", s_static_readFile, true, true },
        { "readFileSync", s_static_readFile, true, false },
        { "readLines", s_static_readLines, true, false },
        { "write", s_static_write, true, true },
        { "writeSync", s_static_write, true, false },
        { "writeTextFile", s_static_writeTextFile, true, true },
        { "writeTextFileSync", s_static_writeTextFile, true, false },
        { "writeFile", s_static_writeFile, true, true },
        { "writeFileSync", s_static_writeFile, true, false },
        { "appendFile", s_static_appendFile, true, true },
        { "appendFileSync", s_static_appendFile, true, false },
        { "setZipFS", s_static_setZipFS, true, false },
        { "clearZipFS", s_static_clearZipFS, true, false },
        { "watch", s_static_watch, true, false },
        { "watchFile", s_static_watchFile, true, false },
        { "unwatchFile", s_static_unwatchFile, true, false }
    };

    static ClassData::ClassObject s_object[] = {
        { "constants", fs_constants_base::class_info }
    };

    static ClassData::ClassConst s_const[] = {
        { "SEEK_SET", C_SEEK_SET },
        { "SEEK_CUR", C_SEEK_CUR },
        { "SEEK_END", C_SEEK_END }
    };

    static ClassData s_cd = {
        "fs", true, s__new, NULL,
        ARRAYSIZE(s_method), s_method, ARRAYSIZE(s_object), s_object, 0, NULL, ARRAYSIZE(s_const), s_const, NULL, NULL,
        &object_base::class_info(),
        true
    };

    static ClassInfo s_ci(s_cd);
    return s_ci;
}

inline void fs_base::s_static_exists(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    bool vr;

    METHOD_ENTER();

    ASYNC_METHOD_OVER(1, 1);

    ARG(exlib::string, 0);

    if (!cb.IsEmpty())
        hr = acb_exists(v0, cb, args);
    else
        hr = ac_exists(v0, vr);

    METHOD_RETURN();
}

inline void fs_base::s_static_access(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_ENTER();

    ASYNC_METHOD_OVER(2, 1);

    ARG(exlib::string, 0);
    OPT_ARG(int32_t, 1, 0);

    if (!cb.IsEmpty())
        hr = acb_access(v0, v1, cb, args);
    else
        hr = ac_access(v0, v1);

    METHOD_VOID();
}

inline void fs_base::s_static_link(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_ENTER();

    ASYNC_METHOD_OVER(2, 2);

    ARG(exlib::string, 0);
    ARG(exlib::string, 1);

    if (!cb.IsEmpty())
        hr = acb_link(v0, v1, cb, args);
    else
        hr = ac_link(v0, v1);

    METHOD_VOID();
}

inline void fs_base::s_static_unlink(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_ENTER();

    ASYNC_METHOD_OVER(1, 1);

    ARG(exlib::string, 0);

    if (!cb.IsEmpty())
        hr = acb_unlink(v0, cb, args);
    else
        hr = ac_unlink(v0);

    METHOD_VOID();
}

inline void fs_base::s_static_mkdir(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_ENTER();

    ASYNC_METHOD_OVER(2, 1);

    ARG(exlib::string, 0);
    OPT_ARG(int32_t, 1, 0777);

    if (!cb.IsEmpty())
        hr = acb_mkdir(v0, v1, cb, args);
    else
        hr = ac_mkdir(v0, v1);

    ASYNC_METHOD_OVER(2, 2);

    ARG(exlib::string, 0);
    ARG(v8::Local<v8::Object>, 1);

    if (!cb.IsEmpty())
        hr = acb_mkdir(v0, v1, cb, args);
    else
        hr = ac_mkdir(v0, v1);

    METHOD_VOID();
}

inline void fs_base::s_static_rmdir(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_ENTER();

    ASYNC_METHOD_OVER(1, 1);

    ARG(exlib::string, 0);

    if (!cb.IsEmpty())
        hr = acb_rmdir(v0, cb, args);
    else
        hr = ac_rmdir(v0);

    METHOD_VOID();
}

inline void fs_base::s_static_rename(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_ENTER();

    ASYNC_METHOD_OVER(2, 2);

    ARG(exlib::string, 0);
    ARG(exlib::string, 1);

    if (!cb.IsEmpty())
        hr = acb_rename(v0, v1, cb, args);
    else
        hr = ac_rename(v0, v1);

    METHOD_VOID();
}

inline void fs_base::s_static_copyFile(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_ENTER();

    ASYNC_METHOD_OVER(3, 2);

    ARG(exlib::string, 0);
    ARG(exlib::string, 1);
    OPT_ARG(int32_t, 2, 0);

    if (!cb.IsEmpty())
        hr = acb_copyFile(v0, v1, v2, cb, args);
    else
        hr = ac_copyFile(v0, v1, v2);

    METHOD_VOID();
}

inline void fs_base::s_static_chmod(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_ENTER();

    ASYNC_METHOD_OVER(2, 2);

    ARG(exlib::string, 0);
    ARG(int32_t, 1);

    if (!cb.IsEmpty())
        hr = acb_chmod(v0, v1, cb, args);
    else
        hr = ac_chmod(v0, v1);

    METHOD_VOID();
}

inline void fs_base::s_static_lchmod(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_ENTER();

    ASYNC_METHOD_OVER(2, 2);

    ARG(exlib::string, 0);
    ARG(int32_t, 1);

    if (!cb.IsEmpty())
        hr = acb_lchmod(v0, v1, cb, args);
    else
        hr = ac_lchmod(v0, v1);

    METHOD_VOID();
}

inline void fs_base::s_static_chown(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_ENTER();

    ASYNC_METHOD_OVER(3, 3);

    ARG(exlib::string, 0);
    ARG(int32_t, 1);
    ARG(int32_t, 2);

    if (!cb.IsEmpty())
        hr = acb_chown(v0, v1, v2, cb, args);
    else
        hr = ac_chown(v0, v1, v2);

    METHOD_VOID();
}

inline void fs_base::s_static_lchown(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_ENTER();

    ASYNC_METHOD_OVER(3, 3);

    ARG(exlib::string, 0);
    ARG(int32_t, 1);
    ARG(int32_t, 2);

    if (!cb.IsEmpty())
        hr = acb_lchown(v0, v1, v2, cb, args);
    else
        hr = ac_lchown(v0, v1, v2);

    METHOD_VOID();
}

inline void fs_base::s_static_stat(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<Stat_base> vr;

    METHOD_ENTER();

    ASYNC_METHOD_OVER(1, 1);

    ARG(exlib::string, 0);

    if (!cb.IsEmpty())
        hr = acb_stat(v0, cb, args);
    else
        hr = ac_stat(v0, vr);

    METHOD_RETURN();
}

inline void fs_base::s_static_lstat(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<Stat_base> vr;

    METHOD_ENTER();

    ASYNC_METHOD_OVER(1, 1);

    ARG(exlib::string, 0);

    if (!cb.IsEmpty())
        hr = acb_lstat(v0, cb, args);
    else
        hr = ac_lstat(v0, vr);

    METHOD_RETURN();
}

inline void fs_base::s_static_fstat(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<Stat_base> vr;

    METHOD_ENTER();

    ASYNC_METHOD_OVER(1, 1);

    ARG(obj_ptr<FileHandle_base>, 0);

    if (!cb.IsEmpty())
        hr = acb_fstat(v0, cb, args);
    else
        hr = ac_fstat(v0, vr);

    METHOD_RETURN();
}

inline void fs_base::s_static_readlink(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_ENTER();

    ASYNC_METHOD_OVER(1, 1);

    ARG(exlib::string, 0);

    if (!cb.IsEmpty())
        hr = acb_readlink(v0, cb, args);
    else
        hr = ac_readlink(v0, vr);

    METHOD_RETURN();
}

inline void fs_base::s_static_realpath(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_ENTER();

    ASYNC_METHOD_OVER(1, 1);

    ARG(exlib::string, 0);

    if (!cb.IsEmpty())
        hr = acb_realpath(v0, cb, args);
    else
        hr = ac_realpath(v0, vr);

    METHOD_RETURN();
}

inline void fs_base::s_static_symlink(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_ENTER();

    ASYNC_METHOD_OVER(3, 2);

    ARG(exlib::string, 0);
    ARG(exlib::string, 1);
    OPT_ARG(exlib::string, 2, "file");

    if (!cb.IsEmpty())
        hr = acb_symlink(v0, v1, v2, cb, args);
    else
        hr = ac_symlink(v0, v1, v2);

    METHOD_VOID();
}

inline void fs_base::s_static_truncate(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_ENTER();

    ASYNC_METHOD_OVER(2, 2);

    ARG(exlib::string, 0);
    ARG(int32_t, 1);

    if (!cb.IsEmpty())
        hr = acb_truncate(v0, v1, cb, args);
    else
        hr = ac_truncate(v0, v1);

    METHOD_VOID();
}

inline void fs_base::s_static_read(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    int32_t vr;

    METHOD_ENTER();

    ASYNC_METHOD_OVER(5, 2);

    ARG(obj_ptr<FileHandle_base>, 0);
    ARG(obj_ptr<Buffer_base>, 1);
    OPT_ARG(int32_t, 2, 0);
    OPT_ARG(int32_t, 3, 0);
    OPT_ARG(int32_t, 4, -1);

    if (!cb.IsEmpty())
        hr = acb_read(v0, v1, v2, v3, v4, cb, args);
    else
        hr = ac_read(v0, v1, v2, v3, v4, vr);

    METHOD_RETURN();
}

inline void fs_base::s_static_fchmod(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_ENTER();

    ASYNC_METHOD_OVER(2, 2);

    ARG(obj_ptr<FileHandle_base>, 0);
    ARG(int32_t, 1);

    if (!cb.IsEmpty())
        hr = acb_fchmod(v0, v1, cb, args);
    else
        hr = ac_fchmod(v0, v1);

    METHOD_VOID();
}

inline void fs_base::s_static_fchown(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_ENTER();

    ASYNC_METHOD_OVER(3, 3);

    ARG(obj_ptr<FileHandle_base>, 0);
    ARG(int32_t, 1);
    ARG(int32_t, 2);

    if (!cb.IsEmpty())
        hr = acb_fchown(v0, v1, v2, cb, args);
    else
        hr = ac_fchown(v0, v1, v2);

    METHOD_VOID();
}

inline void fs_base::s_static_fdatasync(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_ENTER();

    ASYNC_METHOD_OVER(1, 1);

    ARG(obj_ptr<FileHandle_base>, 0);

    if (!cb.IsEmpty())
        hr = acb_fdatasync(v0, cb, args);
    else
        hr = ac_fdatasync(v0);

    METHOD_VOID();
}

inline void fs_base::s_static_fsync(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_ENTER();

    ASYNC_METHOD_OVER(1, 1);

    ARG(obj_ptr<FileHandle_base>, 0);

    if (!cb.IsEmpty())
        hr = acb_fsync(v0, cb, args);
    else
        hr = ac_fsync(v0);

    METHOD_VOID();
}

inline void fs_base::s_static_readdir(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<NArray> vr;

    METHOD_ENTER();

    ASYNC_METHOD_OVER(1, 1);

    ARG(exlib::string, 0);

    if (!cb.IsEmpty())
        hr = acb_readdir(v0, cb, args);
    else
        hr = ac_readdir(v0, vr);

    ASYNC_METHOD_OVER(2, 1);

    ARG(exlib::string, 0);
    OPT_ARG(v8::Local<v8::Object>, 1, v8::Object::New(isolate->m_isolate));

    if (!cb.IsEmpty())
        hr = acb_readdir(v0, v1, cb, args);
    else
        hr = ac_readdir(v0, v1, vr);

    METHOD_RETURN();
}

inline void fs_base::s_static_openFile(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<SeekableStream_base> vr;

    METHOD_ENTER();

    ASYNC_METHOD_OVER(2, 1);

    ARG(exlib::string, 0);
    OPT_ARG(exlib::string, 1, "r");

    if (!cb.IsEmpty())
        hr = acb_openFile(v0, v1, cb, args);
    else
        hr = ac_openFile(v0, v1, vr);

    METHOD_RETURN();
}

inline void fs_base::s_static_open(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<FileHandle_base> vr;

    METHOD_ENTER();

    ASYNC_METHOD_OVER(3, 1);

    ARG(exlib::string, 0);
    OPT_ARG(exlib::string, 1, "r");
    OPT_ARG(int32_t, 2, 0666);

    if (!cb.IsEmpty())
        hr = acb_open(v0, v1, v2, cb, args);
    else
        hr = ac_open(v0, v1, v2, vr);

    METHOD_RETURN();
}

inline void fs_base::s_static_close(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_ENTER();

    ASYNC_METHOD_OVER(1, 1);

    ARG(obj_ptr<FileHandle_base>, 0);

    if (!cb.IsEmpty())
        hr = acb_close(v0, cb, args);
    else
        hr = ac_close(v0);

    METHOD_VOID();
}

inline void fs_base::s_static_openTextStream(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<BufferedStream_base> vr;

    METHOD_ENTER();

    ASYNC_METHOD_OVER(2, 1);

    ARG(exlib::string, 0);
    OPT_ARG(exlib::string, 1, "r");

    if (!cb.IsEmpty())
        hr = acb_openTextStream(v0, v1, cb, args);
    else
        hr = ac_openTextStream(v0, v1, vr);

    METHOD_RETURN();
}

inline void fs_base::s_static_readTextFile(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_ENTER();

    ASYNC_METHOD_OVER(1, 1);

    ARG(exlib::string, 0);

    if (!cb.IsEmpty())
        hr = acb_readTextFile(v0, cb, args);
    else
        hr = ac_readTextFile(v0, vr);

    METHOD_RETURN();
}

inline void fs_base::s_static_readFile(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    Variant vr;

    METHOD_ENTER();

    ASYNC_METHOD_OVER(2, 1);

    ARG(exlib::string, 0);
    OPT_ARG(exlib::string, 1, "");

    if (!cb.IsEmpty())
        hr = acb_readFile(v0, v1, cb, args);
    else
        hr = ac_readFile(v0, v1, vr);

    ASYNC_METHOD_OVER(2, 2);

    ARG(exlib::string, 0);
    ARG(v8::Local<v8::Object>, 1);

    if (!cb.IsEmpty())
        hr = acb_readFile(v0, v1, cb, args);
    else
        hr = ac_readFile(v0, v1, vr);

    METHOD_RETURN();
}

inline void fs_base::s_static_readLines(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Array> vr;

    METHOD_ENTER();

    METHOD_OVER(2, 1);

    ARG(exlib::string, 0);
    OPT_ARG(int32_t, 1, -1);

    hr = readLines(v0, v1, vr);

    METHOD_RETURN();
}

inline void fs_base::s_static_write(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    int32_t vr;

    METHOD_ENTER();

    ASYNC_METHOD_OVER(5, 2);

    ARG(obj_ptr<FileHandle_base>, 0);
    ARG(obj_ptr<Buffer_base>, 1);
    OPT_ARG(int32_t, 2, 0);
    OPT_ARG(int32_t, 3, -1);
    OPT_ARG(int32_t, 4, -1);

    if (!cb.IsEmpty())
        hr = acb_write(v0, v1, v2, v3, v4, cb, args);
    else
        hr = ac_write(v0, v1, v2, v3, v4, vr);

    ASYNC_METHOD_OVER(4, 2);

    ARG(obj_ptr<FileHandle_base>, 0);
    ARG(exlib::string, 1);
    OPT_ARG(int32_t, 2, -1);
    OPT_ARG(exlib::string, 3, "utf8");

    if (!cb.IsEmpty())
        hr = acb_write(v0, v1, v2, v3, cb, args);
    else
        hr = ac_write(v0, v1, v2, v3, vr);

    METHOD_RETURN();
}

inline void fs_base::s_static_writeTextFile(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_ENTER();

    ASYNC_METHOD_OVER(2, 2);

    ARG(exlib::string, 0);
    ARG(exlib::string, 1);

    if (!cb.IsEmpty())
        hr = acb_writeTextFile(v0, v1, cb, args);
    else
        hr = ac_writeTextFile(v0, v1);

    METHOD_VOID();
}

inline void fs_base::s_static_writeFile(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_ENTER();

    ASYNC_METHOD_OVER(3, 2);

    ARG(exlib::string, 0);
    ARG(obj_ptr<Buffer_base>, 1);
    OPT_ARG(exlib::string, 2, "binary");

    if (!cb.IsEmpty())
        hr = acb_writeFile(v0, v1, v2, cb, args);
    else
        hr = ac_writeFile(v0, v1, v2);

    ASYNC_METHOD_OVER(3, 3);

    ARG(exlib::string, 0);
    ARG(obj_ptr<Buffer_base>, 1);
    ARG(v8::Local<v8::Object>, 2);

    if (!cb.IsEmpty())
        hr = acb_writeFile(v0, v1, v2, cb, args);
    else
        hr = ac_writeFile(v0, v1, v2);

    ASYNC_METHOD_OVER(3, 2);

    ARG(exlib::string, 0);
    ARG(exlib::string, 1);
    OPT_ARG(exlib::string, 2, "utf8");

    if (!cb.IsEmpty())
        hr = acb_writeFile(v0, v1, v2, cb, args);
    else
        hr = ac_writeFile(v0, v1, v2);

    ASYNC_METHOD_OVER(3, 3);

    ARG(exlib::string, 0);
    ARG(exlib::string, 1);
    ARG(v8::Local<v8::Object>, 2);

    if (!cb.IsEmpty())
        hr = acb_writeFile(v0, v1, v2, cb, args);
    else
        hr = ac_writeFile(v0, v1, v2);

    METHOD_VOID();
}

inline void fs_base::s_static_appendFile(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_ENTER();

    ASYNC_METHOD_OVER(2, 2);

    ARG(exlib::string, 0);
    ARG(obj_ptr<Buffer_base>, 1);

    if (!cb.IsEmpty())
        hr = acb_appendFile(v0, v1, cb, args);
    else
        hr = ac_appendFile(v0, v1);

    METHOD_VOID();
}

inline void fs_base::s_static_setZipFS(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_ENTER();

    METHOD_OVER(2, 2);

    ARG(exlib::string, 0);
    ARG(obj_ptr<Buffer_base>, 1);

    hr = setZipFS(v0, v1);

    METHOD_VOID();
}

inline void fs_base::s_static_clearZipFS(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_ENTER();

    METHOD_OVER(1, 0);

    OPT_ARG(exlib::string, 0, "");

    hr = clearZipFS(v0);

    METHOD_VOID();
}

inline void fs_base::s_static_watch(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<FSWatcher_base> vr;

    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(exlib::string, 0);

    hr = watch(v0, vr);

    METHOD_OVER(2, 2);

    ARG(exlib::string, 0);
    ARG(v8::Local<v8::Function>, 1);

    hr = watch(v0, v1, vr);

    METHOD_OVER(2, 2);

    ARG(exlib::string, 0);
    ARG(v8::Local<v8::Object>, 1);

    hr = watch(v0, v1, vr);

    METHOD_OVER(3, 3);

    ARG(exlib::string, 0);
    ARG(v8::Local<v8::Object>, 1);
    ARG(v8::Local<v8::Function>, 2);

    hr = watch(v0, v1, v2, vr);

    METHOD_RETURN();
}

inline void fs_base::s_static_watchFile(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<StatsWatcher_base> vr;

    METHOD_ENTER();

    METHOD_OVER(2, 2);

    ARG(exlib::string, 0);
    ARG(v8::Local<v8::Function>, 1);

    hr = watchFile(v0, v1, vr);

    METHOD_OVER(3, 3);

    ARG(exlib::string, 0);
    ARG(v8::Local<v8::Object>, 1);
    ARG(v8::Local<v8::Function>, 2);

    hr = watchFile(v0, v1, v2, vr);

    METHOD_RETURN();
}

inline void fs_base::s_static_unwatchFile(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(exlib::string, 0);

    hr = unwatchFile(v0);

    METHOD_OVER(2, 2);

    ARG(exlib::string, 0);
    ARG(v8::Local<v8::Function>, 1);

    hr = unwatchFile(v0, v1);

    METHOD_VOID();
}
}
