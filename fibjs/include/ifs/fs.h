/***************************************************************************
 *                                                                         *
 *   This file was automatically generated using idlc.js                   *
 *   PLEASE DO NOT EDIT!!!!                                                *
 *                                                                         *
 ***************************************************************************/

#ifndef _fs_base_H_
#define _fs_base_H_

/**
 @author Leo Hoo <lion@9465.net>
 */

#include "../object.h"

namespace fibjs {

class Stat_base;
class Buffer_base;
class SeekableStream_base;
class BufferedStream_base;

class fs_base : public object_base {
    DECLARE_CLASS(fs_base);

public:
    enum {
        _SEEK_SET = 0,
        _SEEK_CUR = 1,
        _SEEK_END = 2
    };

public:
    // fs_base
    static result_t get_constants(v8::Local<v8::Object>& retVal);
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

public:
    static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        CONSTRUCT_INIT();

        Isolate* isolate = Isolate::current();

        isolate->m_isolate->ThrowException(
            isolate->NewString("not a constructor"));
    }

public:
    static void s_get_SEEK_SET(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_get_SEEK_CUR(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_get_SEEK_END(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_get_constants(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_exists(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_access(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_link(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_unlink(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_mkdir(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_rmdir(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_rename(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_copy(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_chmod(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_lchmod(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_chown(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_lchown(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_stat(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_lstat(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_readlink(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_realpath(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_symlink(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_truncate(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_read(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_fchmod(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_fchown(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_fdatasync(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_fsync(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_readdir(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_openFile(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_open(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_close(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_openTextStream(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_readTextFile(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_readFile(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_readLines(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_writeTextFile(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_writeFile(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_appendFile(const v8::FunctionCallbackInfo<v8::Value>& args);

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

#include "Stat.h"
#include "Buffer.h"
#include "SeekableStream.h"
#include "BufferedStream.h"

namespace fibjs {
inline ClassInfo& fs_base::class_info()
{
    static ClassData::ClassMethod s_method[] = {
        { "exists", s_exists, true },
        { "existsSync", s_exists, true },
        { "access", s_access, true },
        { "accessSync", s_access, true },
        { "link", s_link, true },
        { "linkSync", s_link, true },
        { "unlink", s_unlink, true },
        { "unlinkSync", s_unlink, true },
        { "mkdir", s_mkdir, true },
        { "mkdirSync", s_mkdir, true },
        { "rmdir", s_rmdir, true },
        { "rmdirSync", s_rmdir, true },
        { "rename", s_rename, true },
        { "renameSync", s_rename, true },
        { "copy", s_copy, true },
        { "copySync", s_copy, true },
        { "chmod", s_chmod, true },
        { "chmodSync", s_chmod, true },
        { "lchmod", s_lchmod, true },
        { "lchmodSync", s_lchmod, true },
        { "chown", s_chown, true },
        { "chownSync", s_chown, true },
        { "lchown", s_lchown, true },
        { "lchownSync", s_lchown, true },
        { "stat", s_stat, true },
        { "statSync", s_stat, true },
        { "lstat", s_lstat, true },
        { "lstatSync", s_lstat, true },
        { "readlink", s_readlink, true },
        { "readlinkSync", s_readlink, true },
        { "realpath", s_realpath, true },
        { "realpathSync", s_realpath, true },
        { "symlink", s_symlink, true },
        { "symlinkSync", s_symlink, true },
        { "truncate", s_truncate, true },
        { "truncateSync", s_truncate, true },
        { "read", s_read, true },
        { "readSync", s_read, true },
        { "fchmod", s_fchmod, true },
        { "fchmodSync", s_fchmod, true },
        { "fchown", s_fchown, true },
        { "fchownSync", s_fchown, true },
        { "fdatasync", s_fdatasync, true },
        { "fdatasyncSync", s_fdatasync, true },
        { "fsync", s_fsync, true },
        { "fsyncSync", s_fsync, true },
        { "readdir", s_readdir, true },
        { "readdirSync", s_readdir, true },
        { "openFile", s_openFile, true },
        { "openFileSync", s_openFile, true },
        { "open", s_open, true },
        { "openSync", s_open, true },
        { "close", s_close, true },
        { "closeSync", s_close, true },
        { "openTextStream", s_openTextStream, true },
        { "openTextStreamSync", s_openTextStream, true },
        { "readTextFile", s_readTextFile, true },
        { "readTextFileSync", s_readTextFile, true },
        { "readFile", s_readFile, true },
        { "readFileSync", s_readFile, true },
        { "readLines", s_readLines, true },
        { "writeTextFile", s_writeTextFile, true },
        { "writeTextFileSync", s_writeTextFile, true },
        { "writeFile", s_writeFile, true },
        { "writeFileSync", s_writeFile, true },
        { "appendFile", s_appendFile, true },
        { "appendFileSync", s_appendFile, true }
    };

    static ClassData::ClassProperty s_property[] = {
        { "SEEK_SET", s_get_SEEK_SET, block_set, true },
        { "SEEK_CUR", s_get_SEEK_CUR, block_set, true },
        { "SEEK_END", s_get_SEEK_END, block_set, true },
        { "constants", s_get_constants, block_set, true }
    };

    static ClassData s_cd = {
        "fs", true, s__new, NULL,
        ARRAYSIZE(s_method), s_method, 0, NULL, ARRAYSIZE(s_property), s_property, NULL, NULL,
        &object_base::class_info()
    };

    static ClassInfo s_ci(s_cd);
    return s_ci;
}

inline void fs_base::s_get_SEEK_SET(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    METHOD_NAME("fs.SEEK_SET");
    int32_t vr = _SEEK_SET;
    PROPERTY_ENTER();
    METHOD_RETURN();
}

inline void fs_base::s_get_SEEK_CUR(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    METHOD_NAME("fs.SEEK_CUR");
    int32_t vr = _SEEK_CUR;
    PROPERTY_ENTER();
    METHOD_RETURN();
}

inline void fs_base::s_get_SEEK_END(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    METHOD_NAME("fs.SEEK_END");
    int32_t vr = _SEEK_END;
    PROPERTY_ENTER();
    METHOD_RETURN();
}

inline void fs_base::s_get_constants(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Object> vr;

    METHOD_NAME("fs.constants");
    PROPERTY_ENTER();

    hr = get_constants(vr);

    METHOD_RETURN();
}

inline void fs_base::s_exists(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    bool vr;

    METHOD_NAME("fs.exists");
    METHOD_ENTER();

    ASYNC_METHOD_OVER(1, 1);

    ARG(exlib::string, 0);

    if (!cb.IsEmpty()) {
        acb_exists(v0, cb);
        hr = CALL_RETURN_NULL;
    } else
        hr = ac_exists(v0, vr);

    METHOD_RETURN();
}

inline void fs_base::s_access(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_NAME("fs.access");
    METHOD_ENTER();

    ASYNC_METHOD_OVER(2, 1);

    ARG(exlib::string, 0);
    OPT_ARG(int32_t, 1, 0);

    if (!cb.IsEmpty()) {
        acb_access(v0, v1, cb);
        hr = CALL_RETURN_NULL;
    } else
        hr = ac_access(v0, v1);

    METHOD_VOID();
}

inline void fs_base::s_link(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_NAME("fs.link");
    METHOD_ENTER();

    ASYNC_METHOD_OVER(2, 2);

    ARG(exlib::string, 0);
    ARG(exlib::string, 1);

    if (!cb.IsEmpty()) {
        acb_link(v0, v1, cb);
        hr = CALL_RETURN_NULL;
    } else
        hr = ac_link(v0, v1);

    METHOD_VOID();
}

inline void fs_base::s_unlink(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_NAME("fs.unlink");
    METHOD_ENTER();

    ASYNC_METHOD_OVER(1, 1);

    ARG(exlib::string, 0);

    if (!cb.IsEmpty()) {
        acb_unlink(v0, cb);
        hr = CALL_RETURN_NULL;
    } else
        hr = ac_unlink(v0);

    METHOD_VOID();
}

inline void fs_base::s_mkdir(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_NAME("fs.mkdir");
    METHOD_ENTER();

    ASYNC_METHOD_OVER(2, 1);

    ARG(exlib::string, 0);
    OPT_ARG(int32_t, 1, 0777);

    if (!cb.IsEmpty()) {
        acb_mkdir(v0, v1, cb);
        hr = CALL_RETURN_NULL;
    } else
        hr = ac_mkdir(v0, v1);

    METHOD_VOID();
}

inline void fs_base::s_rmdir(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_NAME("fs.rmdir");
    METHOD_ENTER();

    ASYNC_METHOD_OVER(1, 1);

    ARG(exlib::string, 0);

    if (!cb.IsEmpty()) {
        acb_rmdir(v0, cb);
        hr = CALL_RETURN_NULL;
    } else
        hr = ac_rmdir(v0);

    METHOD_VOID();
}

inline void fs_base::s_rename(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_NAME("fs.rename");
    METHOD_ENTER();

    ASYNC_METHOD_OVER(2, 2);

    ARG(exlib::string, 0);
    ARG(exlib::string, 1);

    if (!cb.IsEmpty()) {
        acb_rename(v0, v1, cb);
        hr = CALL_RETURN_NULL;
    } else
        hr = ac_rename(v0, v1);

    METHOD_VOID();
}

inline void fs_base::s_copy(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_NAME("fs.copy");
    METHOD_ENTER();

    ASYNC_METHOD_OVER(2, 2);

    ARG(exlib::string, 0);
    ARG(exlib::string, 1);

    if (!cb.IsEmpty()) {
        acb_copy(v0, v1, cb);
        hr = CALL_RETURN_NULL;
    } else
        hr = ac_copy(v0, v1);

    METHOD_VOID();
}

inline void fs_base::s_chmod(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_NAME("fs.chmod");
    METHOD_ENTER();

    ASYNC_METHOD_OVER(2, 2);

    ARG(exlib::string, 0);
    ARG(int32_t, 1);

    if (!cb.IsEmpty()) {
        acb_chmod(v0, v1, cb);
        hr = CALL_RETURN_NULL;
    } else
        hr = ac_chmod(v0, v1);

    METHOD_VOID();
}

inline void fs_base::s_lchmod(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_NAME("fs.lchmod");
    METHOD_ENTER();

    ASYNC_METHOD_OVER(2, 2);

    ARG(exlib::string, 0);
    ARG(int32_t, 1);

    if (!cb.IsEmpty()) {
        acb_lchmod(v0, v1, cb);
        hr = CALL_RETURN_NULL;
    } else
        hr = ac_lchmod(v0, v1);

    METHOD_VOID();
}

inline void fs_base::s_chown(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_NAME("fs.chown");
    METHOD_ENTER();

    ASYNC_METHOD_OVER(3, 3);

    ARG(exlib::string, 0);
    ARG(int32_t, 1);
    ARG(int32_t, 2);

    if (!cb.IsEmpty()) {
        acb_chown(v0, v1, v2, cb);
        hr = CALL_RETURN_NULL;
    } else
        hr = ac_chown(v0, v1, v2);

    METHOD_VOID();
}

inline void fs_base::s_lchown(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_NAME("fs.lchown");
    METHOD_ENTER();

    ASYNC_METHOD_OVER(3, 3);

    ARG(exlib::string, 0);
    ARG(int32_t, 1);
    ARG(int32_t, 2);

    if (!cb.IsEmpty()) {
        acb_lchown(v0, v1, v2, cb);
        hr = CALL_RETURN_NULL;
    } else
        hr = ac_lchown(v0, v1, v2);

    METHOD_VOID();
}

inline void fs_base::s_stat(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<Stat_base> vr;

    METHOD_NAME("fs.stat");
    METHOD_ENTER();

    ASYNC_METHOD_OVER(1, 1);

    ARG(exlib::string, 0);

    if (!cb.IsEmpty()) {
        acb_stat(v0, cb);
        hr = CALL_RETURN_NULL;
    } else
        hr = ac_stat(v0, vr);

    METHOD_RETURN();
}

inline void fs_base::s_lstat(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<Stat_base> vr;

    METHOD_NAME("fs.lstat");
    METHOD_ENTER();

    ASYNC_METHOD_OVER(1, 1);

    ARG(exlib::string, 0);

    if (!cb.IsEmpty()) {
        acb_lstat(v0, cb);
        hr = CALL_RETURN_NULL;
    } else
        hr = ac_lstat(v0, vr);

    METHOD_RETURN();
}

inline void fs_base::s_readlink(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_NAME("fs.readlink");
    METHOD_ENTER();

    ASYNC_METHOD_OVER(1, 1);

    ARG(exlib::string, 0);

    if (!cb.IsEmpty()) {
        acb_readlink(v0, cb);
        hr = CALL_RETURN_NULL;
    } else
        hr = ac_readlink(v0, vr);

    METHOD_RETURN();
}

inline void fs_base::s_realpath(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_NAME("fs.realpath");
    METHOD_ENTER();

    ASYNC_METHOD_OVER(1, 1);

    ARG(exlib::string, 0);

    if (!cb.IsEmpty()) {
        acb_realpath(v0, cb);
        hr = CALL_RETURN_NULL;
    } else
        hr = ac_realpath(v0, vr);

    METHOD_RETURN();
}

inline void fs_base::s_symlink(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_NAME("fs.symlink");
    METHOD_ENTER();

    ASYNC_METHOD_OVER(3, 2);

    ARG(exlib::string, 0);
    ARG(exlib::string, 1);
    OPT_ARG(exlib::string, 2, "file");

    if (!cb.IsEmpty()) {
        acb_symlink(v0, v1, v2, cb);
        hr = CALL_RETURN_NULL;
    } else
        hr = ac_symlink(v0, v1, v2);

    METHOD_VOID();
}

inline void fs_base::s_truncate(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_NAME("fs.truncate");
    METHOD_ENTER();

    ASYNC_METHOD_OVER(2, 2);

    ARG(exlib::string, 0);
    ARG(int32_t, 1);

    if (!cb.IsEmpty()) {
        acb_truncate(v0, v1, cb);
        hr = CALL_RETURN_NULL;
    } else
        hr = ac_truncate(v0, v1);

    METHOD_VOID();
}

inline void fs_base::s_read(const v8::FunctionCallbackInfo<v8::Value>& args)
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

    if (!cb.IsEmpty()) {
        acb_read(v0, v1, v2, v3, v4, cb);
        hr = CALL_RETURN_NULL;
    } else
        hr = ac_read(v0, v1, v2, v3, v4, vr);

    METHOD_RETURN();
}

inline void fs_base::s_fchmod(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_NAME("fs.fchmod");
    METHOD_ENTER();

    ASYNC_METHOD_OVER(2, 2);

    ARG(int32_t, 0);
    ARG(int32_t, 1);

    if (!cb.IsEmpty()) {
        acb_fchmod(v0, v1, cb);
        hr = CALL_RETURN_NULL;
    } else
        hr = ac_fchmod(v0, v1);

    METHOD_VOID();
}

inline void fs_base::s_fchown(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_NAME("fs.fchown");
    METHOD_ENTER();

    ASYNC_METHOD_OVER(3, 3);

    ARG(int32_t, 0);
    ARG(int32_t, 1);
    ARG(int32_t, 2);

    if (!cb.IsEmpty()) {
        acb_fchown(v0, v1, v2, cb);
        hr = CALL_RETURN_NULL;
    } else
        hr = ac_fchown(v0, v1, v2);

    METHOD_VOID();
}

inline void fs_base::s_fdatasync(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_NAME("fs.fdatasync");
    METHOD_ENTER();

    ASYNC_METHOD_OVER(1, 1);

    ARG(int32_t, 0);

    if (!cb.IsEmpty()) {
        acb_fdatasync(v0, cb);
        hr = CALL_RETURN_NULL;
    } else
        hr = ac_fdatasync(v0);

    METHOD_VOID();
}

inline void fs_base::s_fsync(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_NAME("fs.fsync");
    METHOD_ENTER();

    ASYNC_METHOD_OVER(1, 1);

    ARG(int32_t, 0);

    if (!cb.IsEmpty()) {
        acb_fsync(v0, cb);
        hr = CALL_RETURN_NULL;
    } else
        hr = ac_fsync(v0);

    METHOD_VOID();
}

inline void fs_base::s_readdir(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<NArray> vr;

    METHOD_NAME("fs.readdir");
    METHOD_ENTER();

    ASYNC_METHOD_OVER(1, 1);

    ARG(exlib::string, 0);

    if (!cb.IsEmpty()) {
        acb_readdir(v0, cb);
        hr = CALL_RETURN_NULL;
    } else
        hr = ac_readdir(v0, vr);

    METHOD_RETURN();
}

inline void fs_base::s_openFile(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<SeekableStream_base> vr;

    METHOD_NAME("fs.openFile");
    METHOD_ENTER();

    ASYNC_METHOD_OVER(2, 1);

    ARG(exlib::string, 0);
    OPT_ARG(exlib::string, 1, "r");

    if (!cb.IsEmpty()) {
        acb_openFile(v0, v1, cb);
        hr = CALL_RETURN_NULL;
    } else
        hr = ac_openFile(v0, v1, vr);

    METHOD_RETURN();
}

inline void fs_base::s_open(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    int32_t vr;

    METHOD_NAME("fs.open");
    METHOD_ENTER();

    ASYNC_METHOD_OVER(3, 1);

    ARG(exlib::string, 0);
    OPT_ARG(exlib::string, 1, "r");
    OPT_ARG(int32_t, 2, 0666);

    if (!cb.IsEmpty()) {
        acb_open(v0, v1, v2, cb);
        hr = CALL_RETURN_NULL;
    } else
        hr = ac_open(v0, v1, v2, vr);

    METHOD_RETURN();
}

inline void fs_base::s_close(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_NAME("fs.close");
    METHOD_ENTER();

    ASYNC_METHOD_OVER(1, 1);

    ARG(int32_t, 0);

    if (!cb.IsEmpty()) {
        acb_close(v0, cb);
        hr = CALL_RETURN_NULL;
    } else
        hr = ac_close(v0);

    METHOD_VOID();
}

inline void fs_base::s_openTextStream(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<BufferedStream_base> vr;

    METHOD_NAME("fs.openTextStream");
    METHOD_ENTER();

    ASYNC_METHOD_OVER(2, 1);

    ARG(exlib::string, 0);
    OPT_ARG(exlib::string, 1, "r");

    if (!cb.IsEmpty()) {
        acb_openTextStream(v0, v1, cb);
        hr = CALL_RETURN_NULL;
    } else
        hr = ac_openTextStream(v0, v1, vr);

    METHOD_RETURN();
}

inline void fs_base::s_readTextFile(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_NAME("fs.readTextFile");
    METHOD_ENTER();

    ASYNC_METHOD_OVER(1, 1);

    ARG(exlib::string, 0);

    if (!cb.IsEmpty()) {
        acb_readTextFile(v0, cb);
        hr = CALL_RETURN_NULL;
    } else
        hr = ac_readTextFile(v0, vr);

    METHOD_RETURN();
}

inline void fs_base::s_readFile(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    Variant vr;

    METHOD_NAME("fs.readFile");
    METHOD_ENTER();

    ASYNC_METHOD_OVER(2, 1);

    ARG(exlib::string, 0);
    OPT_ARG(exlib::string, 1, "");

    if (!cb.IsEmpty()) {
        acb_readFile(v0, v1, cb);
        hr = CALL_RETURN_NULL;
    } else
        hr = ac_readFile(v0, v1, vr);

    METHOD_RETURN();
}

inline void fs_base::s_readLines(const v8::FunctionCallbackInfo<v8::Value>& args)
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

inline void fs_base::s_writeTextFile(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_NAME("fs.writeTextFile");
    METHOD_ENTER();

    ASYNC_METHOD_OVER(2, 2);

    ARG(exlib::string, 0);
    ARG(exlib::string, 1);

    if (!cb.IsEmpty()) {
        acb_writeTextFile(v0, v1, cb);
        hr = CALL_RETURN_NULL;
    } else
        hr = ac_writeTextFile(v0, v1);

    METHOD_VOID();
}

inline void fs_base::s_writeFile(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_NAME("fs.writeFile");
    METHOD_ENTER();

    ASYNC_METHOD_OVER(2, 2);

    ARG(exlib::string, 0);
    ARG(obj_ptr<Buffer_base>, 1);

    if (!cb.IsEmpty()) {
        acb_writeFile(v0, v1, cb);
        hr = CALL_RETURN_NULL;
    } else
        hr = ac_writeFile(v0, v1);

    METHOD_VOID();
}

inline void fs_base::s_appendFile(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_NAME("fs.appendFile");
    METHOD_ENTER();

    ASYNC_METHOD_OVER(2, 2);

    ARG(exlib::string, 0);
    ARG(obj_ptr<Buffer_base>, 1);

    if (!cb.IsEmpty()) {
        acb_appendFile(v0, v1, cb);
        hr = CALL_RETURN_NULL;
    } else
        hr = ac_appendFile(v0, v1);

    METHOD_VOID();
}
}

#endif
