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
class Buffer_base;
class SeekableStream_base;
class BufferedStream_base;
class FSWatcher_base;
class StatsWatcher_base;

class fs_base : public object_base {
    DECLARE_CLASS(fs_base);

public:
    // fs_base
    static result_t exists(exlib::string path, bool& retVal, AsyncEvent* ac);
    static result_t access(exlib::string path, int32_t mode, AsyncEvent* ac);
    static result_t link(exlib::string oldPath, exlib::string newPath, AsyncEvent* ac);
    static result_t unlink(exlib::string path, AsyncEvent* ac);
    static result_t mkdir(exlib::string path, int32_t mode, AsyncEvent* ac);
    static result_t rmdir(exlib::string path, AsyncEvent* ac);
    static result_t rename(exlib::string from, exlib::string to, AsyncEvent* ac);
    static result_t copy(exlib::string from, exlib::string to, AsyncEvent* ac);
    static result_t chmod(exlib::string path, int32_t mode, AsyncEvent* ac);
    static result_t lchmod(exlib::string path, int32_t mode, AsyncEvent* ac);
    static result_t chown(exlib::string path, int32_t uid, int32_t gid, AsyncEvent* ac);
    static result_t lchown(exlib::string path, int32_t uid, int32_t gid, AsyncEvent* ac);
    static result_t stat(exlib::string path, obj_ptr<Stat_base>& retVal, AsyncEvent* ac);
    static result_t lstat(exlib::string path, obj_ptr<Stat_base>& retVal, AsyncEvent* ac);
    static result_t readlink(exlib::string path, exlib::string& retVal, AsyncEvent* ac);
    static result_t realpath(exlib::string path, exlib::string& retVal, AsyncEvent* ac);
    static result_t symlink(exlib::string target, exlib::string linkpath, exlib::string type, AsyncEvent* ac);
    static result_t truncate(exlib::string path, int32_t len, AsyncEvent* ac);
    static result_t read(int32_t fd, Buffer_base* buffer, int32_t offset, int32_t length, int32_t position, int32_t& retVal, AsyncEvent* ac);
    static result_t fchmod(int32_t fd, int32_t mode, AsyncEvent* ac);
    static result_t fchown(int32_t fd, int32_t uid, int32_t gid, AsyncEvent* ac);
    static result_t fdatasync(int32_t fd, AsyncEvent* ac);
    static result_t fsync(int32_t fd, AsyncEvent* ac);
    static result_t readdir(exlib::string path, obj_ptr<NArray>& retVal, AsyncEvent* ac);
    static result_t openFile(exlib::string fname, exlib::string flags, obj_ptr<SeekableStream_base>& retVal, AsyncEvent* ac);
    static result_t open(exlib::string fname, exlib::string flags, int32_t mode, int32_t& retVal, AsyncEvent* ac);
    static result_t close(int32_t fd, AsyncEvent* ac);
    static result_t openTextStream(exlib::string fname, exlib::string flags, obj_ptr<BufferedStream_base>& retVal, AsyncEvent* ac);
    static result_t readTextFile(exlib::string fname, exlib::string& retVal, AsyncEvent* ac);
    static result_t readFile(exlib::string fname, exlib::string encoding, Variant& retVal, AsyncEvent* ac);
    static result_t readLines(exlib::string fname, int32_t maxlines, v8::Local<v8::Array>& retVal);
    static result_t writeTextFile(exlib::string fname, exlib::string txt, AsyncEvent* ac);
    static result_t writeFile(exlib::string fname, Buffer_base* data, AsyncEvent* ac);
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

        Isolate* isolate = Isolate::current();

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
    static void s_static_copy(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_chmod(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_lchmod(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_chown(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_lchown(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_stat(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_lstat(const v8::FunctionCallbackInfo<v8::Value>& args);
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
    ASYNC_STATIC1(fs_base, rmdir, exlib::string);
    ASYNC_STATIC2(fs_base, rename, exlib::string, exlib::string);
    ASYNC_STATIC2(fs_base, copy, exlib::string, exlib::string);
    ASYNC_STATIC2(fs_base, chmod, exlib::string, int32_t);
    ASYNC_STATIC2(fs_base, lchmod, exlib::string, int32_t);
    ASYNC_STATIC3(fs_base, chown, exlib::string, int32_t, int32_t);
    ASYNC_STATIC3(fs_base, lchown, exlib::string, int32_t, int32_t);
    ASYNC_STATICVALUE2(fs_base, stat, exlib::string, obj_ptr<Stat_base>);
    ASYNC_STATICVALUE2(fs_base, lstat, exlib::string, obj_ptr<Stat_base>);
    ASYNC_STATICVALUE2(fs_base, readlink, exlib::string, exlib::string);
    ASYNC_STATICVALUE2(fs_base, realpath, exlib::string, exlib::string);
    ASYNC_STATIC3(fs_base, symlink, exlib::string, exlib::string, exlib::string);
    ASYNC_STATIC2(fs_base, truncate, exlib::string, int32_t);
    ASYNC_STATICVALUE6(fs_base, read, int32_t, Buffer_base*, int32_t, int32_t, int32_t, int32_t);
    ASYNC_STATIC2(fs_base, fchmod, int32_t, int32_t);
    ASYNC_STATIC3(fs_base, fchown, int32_t, int32_t, int32_t);
    ASYNC_STATIC1(fs_base, fdatasync, int32_t);
    ASYNC_STATIC1(fs_base, fsync, int32_t);
    ASYNC_STATICVALUE2(fs_base, readdir, exlib::string, obj_ptr<NArray>);
    ASYNC_STATICVALUE3(fs_base, openFile, exlib::string, exlib::string, obj_ptr<SeekableStream_base>);
    ASYNC_STATICVALUE4(fs_base, open, exlib::string, exlib::string, int32_t, int32_t);
    ASYNC_STATIC1(fs_base, close, int32_t);
    ASYNC_STATICVALUE3(fs_base, openTextStream, exlib::string, exlib::string, obj_ptr<BufferedStream_base>);
    ASYNC_STATICVALUE2(fs_base, readTextFile, exlib::string, exlib::string);
    ASYNC_STATICVALUE3(fs_base, readFile, exlib::string, exlib::string, Variant);
    ASYNC_STATIC2(fs_base, writeTextFile, exlib::string, exlib::string);
    ASYNC_STATIC2(fs_base, writeFile, exlib::string, Buffer_base*);
    ASYNC_STATIC2(fs_base, appendFile, exlib::string, Buffer_base*);
};
}

#include "ifs/fs_constants.h"
#include "ifs/Stat.h"
#include "ifs/Buffer.h"
#include "ifs/SeekableStream.h"
#include "ifs/BufferedStream.h"
#include "ifs/FSWatcher.h"
#include "ifs/StatsWatcher.h"

namespace fibjs {
inline ClassInfo& fs_base::class_info()
{
    static ClassData::ClassMethod s_method[] = {
        { "exists", s_static_exists, true },
        { "existsSync", s_static_exists, true },
        { "access", s_static_access, true },
        { "accessSync", s_static_access, true },
        { "link", s_static_link, true },
        { "linkSync", s_static_link, true },
        { "unlink", s_static_unlink, true },
        { "unlinkSync", s_static_unlink, true },
        { "mkdir", s_static_mkdir, true },
        { "mkdirSync", s_static_mkdir, true },
        { "rmdir", s_static_rmdir, true },
        { "rmdirSync", s_static_rmdir, true },
        { "rename", s_static_rename, true },
        { "renameSync", s_static_rename, true },
        { "copy", s_static_copy, true },
        { "copySync", s_static_copy, true },
        { "chmod", s_static_chmod, true },
        { "chmodSync", s_static_chmod, true },
        { "lchmod", s_static_lchmod, true },
        { "lchmodSync", s_static_lchmod, true },
        { "chown", s_static_chown, true },
        { "chownSync", s_static_chown, true },
        { "lchown", s_static_lchown, true },
        { "lchownSync", s_static_lchown, true },
        { "stat", s_static_stat, true },
        { "statSync", s_static_stat, true },
        { "lstat", s_static_lstat, true },
        { "lstatSync", s_static_lstat, true },
        { "readlink", s_static_readlink, true },
        { "readlinkSync", s_static_readlink, true },
        { "realpath", s_static_realpath, true },
        { "realpathSync", s_static_realpath, true },
        { "symlink", s_static_symlink, true },
        { "symlinkSync", s_static_symlink, true },
        { "truncate", s_static_truncate, true },
        { "truncateSync", s_static_truncate, true },
        { "read", s_static_read, true },
        { "readSync", s_static_read, true },
        { "fchmod", s_static_fchmod, true },
        { "fchmodSync", s_static_fchmod, true },
        { "fchown", s_static_fchown, true },
        { "fchownSync", s_static_fchown, true },
        { "fdatasync", s_static_fdatasync, true },
        { "fdatasyncSync", s_static_fdatasync, true },
        { "fsync", s_static_fsync, true },
        { "fsyncSync", s_static_fsync, true },
        { "readdir", s_static_readdir, true },
        { "readdirSync", s_static_readdir, true },
        { "openFile", s_static_openFile, true },
        { "openFileSync", s_static_openFile, true },
        { "open", s_static_open, true },
        { "openSync", s_static_open, true },
        { "close", s_static_close, true },
        { "closeSync", s_static_close, true },
        { "openTextStream", s_static_openTextStream, true },
        { "openTextStreamSync", s_static_openTextStream, true },
        { "readTextFile", s_static_readTextFile, true },
        { "readTextFileSync", s_static_readTextFile, true },
        { "readFile", s_static_readFile, true },
        { "readFileSync", s_static_readFile, true },
        { "readLines", s_static_readLines, true },
        { "writeTextFile", s_static_writeTextFile, true },
        { "writeTextFileSync", s_static_writeTextFile, true },
        { "writeFile", s_static_writeFile, true },
        { "writeFileSync", s_static_writeFile, true },
        { "appendFile", s_static_appendFile, true },
        { "appendFileSync", s_static_appendFile, true },
        { "setZipFS", s_static_setZipFS, true },
        { "clearZipFS", s_static_clearZipFS, true },
        { "watch", s_static_watch, true },
        { "watchFile", s_static_watchFile, true },
        { "unwatchFile", s_static_unwatchFile, true }
    };

    static ClassData::ClassObject s_object[] = {
        { "constants", fs_constants_base::class_info }
    };

    static ClassData s_cd = {
        "fs", true, s__new, NULL,
        ARRAYSIZE(s_method), s_method, ARRAYSIZE(s_object), s_object, 0, NULL, 0, NULL, NULL, NULL,
        &object_base::class_info()
    };

    static ClassInfo s_ci(s_cd);
    return s_ci;
}

inline void fs_base::s_static_exists(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    bool vr;

    METHOD_NAME("fs.exists");
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
    METHOD_NAME("fs.access");
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
    METHOD_NAME("fs.link");
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
    METHOD_NAME("fs.unlink");
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
    METHOD_NAME("fs.mkdir");
    METHOD_ENTER();

    ASYNC_METHOD_OVER(2, 1);

    ARG(exlib::string, 0);
    OPT_ARG(int32_t, 1, 0777);

    if (!cb.IsEmpty())
        hr = acb_mkdir(v0, v1, cb, args);
    else
        hr = ac_mkdir(v0, v1);

    METHOD_VOID();
}

inline void fs_base::s_static_rmdir(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_NAME("fs.rmdir");
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
    METHOD_NAME("fs.rename");
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

inline void fs_base::s_static_copy(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_NAME("fs.copy");
    METHOD_ENTER();

    ASYNC_METHOD_OVER(2, 2);

    ARG(exlib::string, 0);
    ARG(exlib::string, 1);

    if (!cb.IsEmpty())
        hr = acb_copy(v0, v1, cb, args);
    else
        hr = ac_copy(v0, v1);

    METHOD_VOID();
}

inline void fs_base::s_static_chmod(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_NAME("fs.chmod");
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
    METHOD_NAME("fs.lchmod");
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
    METHOD_NAME("fs.chown");
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
    METHOD_NAME("fs.lchown");
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

    METHOD_NAME("fs.stat");
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

    METHOD_NAME("fs.lstat");
    METHOD_ENTER();

    ASYNC_METHOD_OVER(1, 1);

    ARG(exlib::string, 0);

    if (!cb.IsEmpty())
        hr = acb_lstat(v0, cb, args);
    else
        hr = ac_lstat(v0, vr);

    METHOD_RETURN();
}

inline void fs_base::s_static_readlink(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_NAME("fs.readlink");
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

    METHOD_NAME("fs.realpath");
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
    METHOD_NAME("fs.symlink");
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
    METHOD_NAME("fs.truncate");
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

    METHOD_NAME("fs.read");
    METHOD_ENTER();

    ASYNC_METHOD_OVER(5, 2);

    ARG(int32_t, 0);
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
    METHOD_NAME("fs.fchmod");
    METHOD_ENTER();

    ASYNC_METHOD_OVER(2, 2);

    ARG(int32_t, 0);
    ARG(int32_t, 1);

    if (!cb.IsEmpty())
        hr = acb_fchmod(v0, v1, cb, args);
    else
        hr = ac_fchmod(v0, v1);

    METHOD_VOID();
}

inline void fs_base::s_static_fchown(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_NAME("fs.fchown");
    METHOD_ENTER();

    ASYNC_METHOD_OVER(3, 3);

    ARG(int32_t, 0);
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
    METHOD_NAME("fs.fdatasync");
    METHOD_ENTER();

    ASYNC_METHOD_OVER(1, 1);

    ARG(int32_t, 0);

    if (!cb.IsEmpty())
        hr = acb_fdatasync(v0, cb, args);
    else
        hr = ac_fdatasync(v0);

    METHOD_VOID();
}

inline void fs_base::s_static_fsync(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_NAME("fs.fsync");
    METHOD_ENTER();

    ASYNC_METHOD_OVER(1, 1);

    ARG(int32_t, 0);

    if (!cb.IsEmpty())
        hr = acb_fsync(v0, cb, args);
    else
        hr = ac_fsync(v0);

    METHOD_VOID();
}

inline void fs_base::s_static_readdir(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<NArray> vr;

    METHOD_NAME("fs.readdir");
    METHOD_ENTER();

    ASYNC_METHOD_OVER(1, 1);

    ARG(exlib::string, 0);

    if (!cb.IsEmpty())
        hr = acb_readdir(v0, cb, args);
    else
        hr = ac_readdir(v0, vr);

    METHOD_RETURN();
}

inline void fs_base::s_static_openFile(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<SeekableStream_base> vr;

    METHOD_NAME("fs.openFile");
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
    int32_t vr;

    METHOD_NAME("fs.open");
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
    METHOD_NAME("fs.close");
    METHOD_ENTER();

    ASYNC_METHOD_OVER(1, 1);

    ARG(int32_t, 0);

    if (!cb.IsEmpty())
        hr = acb_close(v0, cb, args);
    else
        hr = ac_close(v0);

    METHOD_VOID();
}

inline void fs_base::s_static_openTextStream(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<BufferedStream_base> vr;

    METHOD_NAME("fs.openTextStream");
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

    METHOD_NAME("fs.readTextFile");
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

    METHOD_NAME("fs.readFile");
    METHOD_ENTER();

    ASYNC_METHOD_OVER(2, 1);

    ARG(exlib::string, 0);
    OPT_ARG(exlib::string, 1, "");

    if (!cb.IsEmpty())
        hr = acb_readFile(v0, v1, cb, args);
    else
        hr = ac_readFile(v0, v1, vr);

    METHOD_RETURN();
}

inline void fs_base::s_static_readLines(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Array> vr;

    METHOD_NAME("fs.readLines");
    METHOD_ENTER();

    METHOD_OVER(2, 1);

    ARG(exlib::string, 0);
    OPT_ARG(int32_t, 1, -1);

    hr = readLines(v0, v1, vr);

    METHOD_RETURN();
}

inline void fs_base::s_static_writeTextFile(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_NAME("fs.writeTextFile");
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
    METHOD_NAME("fs.writeFile");
    METHOD_ENTER();

    ASYNC_METHOD_OVER(2, 2);

    ARG(exlib::string, 0);
    ARG(obj_ptr<Buffer_base>, 1);

    if (!cb.IsEmpty())
        hr = acb_writeFile(v0, v1, cb, args);
    else
        hr = ac_writeFile(v0, v1);

    METHOD_VOID();
}

inline void fs_base::s_static_appendFile(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_NAME("fs.appendFile");
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
    METHOD_NAME("fs.setZipFS");
    METHOD_ENTER();

    METHOD_OVER(2, 2);

    ARG(exlib::string, 0);
    ARG(obj_ptr<Buffer_base>, 1);

    hr = setZipFS(v0, v1);

    METHOD_VOID();
}

inline void fs_base::s_static_clearZipFS(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_NAME("fs.clearZipFS");
    METHOD_ENTER();

    METHOD_OVER(1, 0);

    OPT_ARG(exlib::string, 0, "");

    hr = clearZipFS(v0);

    METHOD_VOID();
}

inline void fs_base::s_static_watch(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<FSWatcher_base> vr;

    METHOD_NAME("fs.watch");
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

    METHOD_NAME("fs.watchFile");
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
    METHOD_NAME("fs.unwatchFile");
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
