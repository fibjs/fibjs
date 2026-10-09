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
class DirEntry_base;
class Dir_base;
class FileHandle_base;
class Buffer_base;
class SeekableStream_base;
class BufferedStream_base;
class FSWatcher_base;
class StatsWatcher_base;

class fs_base : public object_base {
    DECLARE_CLASS(fs_base);

public:
    using Union_mkdir_mode = std::variant<int32_t, v8::Local<v8::Object>, Variant>;
    using Union_chmod_mode = std::variant<int32_t, Variant>;
    using Union_lchmod_mode = std::variant<int32_t, Variant>;
    using Union_fstat_fd = std::variant<int32_t, obj_ptr<FileHandle_base>>;
    using Union_readlink_options = std::variant<v8::Local<v8::Object>, exlib::string>;
    using Union_realpath_options = std::variant<v8::Local<v8::Object>, exlib::string>;
    using Union_read_fd = std::variant<int32_t, obj_ptr<FileHandle_base>>;
    using Union_fchmod_fd = std::variant<int32_t, obj_ptr<FileHandle_base>>;
    using Union_fchown_fd = std::variant<int32_t, obj_ptr<FileHandle_base>>;
    using Union_futimes_fd = std::variant<int32_t, obj_ptr<FileHandle_base>>;
    using Union_fdatasync_fd = std::variant<int32_t, obj_ptr<FileHandle_base>>;
    using Union_fsync_fd = std::variant<int32_t, obj_ptr<FileHandle_base>>;
    using Union_ftruncate_fd = std::variant<int32_t, obj_ptr<FileHandle_base>>;
    using Union_readdir_opts = std::variant<v8::Local<v8::Object>, exlib::string>;
    using Union_openFile_flags = std::variant<exlib::string, int32_t>;
    using Union_close_fd = std::variant<int32_t, obj_ptr<FileHandle_base>>;
    using Union_readFile_fname = std::variant<obj_ptr<FileHandle_base>, exlib::string, int32_t>;
    using Union_readFile_options = std::variant<v8::Local<v8::Object>, exlib::string>;
    using Union_write_fd = std::variant<int32_t, obj_ptr<FileHandle_base>>;
    using Union_writeFile_fname = std::variant<obj_ptr<FileHandle_base>, exlib::string, int32_t>;
    using Union_writeFile_data = std::variant<obj_ptr<Buffer_base>, exlib::string>;
    using Union_writeFile_opt = std::variant<v8::Local<v8::Object>, exlib::string>;
    using Union_appendFile_fname = std::variant<obj_ptr<FileHandle_base>, exlib::string, int32_t>;
    using Union_appendFile_data = std::variant<obj_ptr<Buffer_base>, exlib::string>;
    using Union_appendFile_options = std::variant<v8::Local<v8::Object>, exlib::string>;
    using Union_setZipFS_data = std::variant<obj_ptr<Buffer_base>, exlib::string>;

public:
    enum {
        C_SEEK_SET = 0,
        C_SEEK_CUR = 1,
        C_SEEK_END = 2,
        C_F_OK = 0,
        C_R_OK = 4,
        C_W_OK = 2,
        C_X_OK = 1
    };

public:
    class StatfsType : public NType {
    public:
        virtual void to_value(Isolate* isolate, v8::Local<v8::Object>& retVal)
        {
            v8::Local<v8::Context> context = isolate->context();
            retVal->Set(context, isolate->NewString("type"), GetReturnValue(isolate, type)).Check();
            retVal->Set(context, isolate->NewString("bsize"), GetReturnValue(isolate, bsize)).Check();
            retVal->Set(context, isolate->NewString("blocks"), GetReturnValue(isolate, blocks)).Check();
            retVal->Set(context, isolate->NewString("bfree"), GetReturnValue(isolate, bfree)).Check();
            retVal->Set(context, isolate->NewString("bavail"), GetReturnValue(isolate, bavail)).Check();
            retVal->Set(context, isolate->NewString("files"), GetReturnValue(isolate, files)).Check();
            retVal->Set(context, isolate->NewString("ffree"), GetReturnValue(isolate, ffree)).Check();
        }

        virtual void to_args(Isolate* isolate, std::vector<v8::Local<v8::Value>>& args)
        {
            args.push_back(GetReturnValue(isolate, type));
            args.push_back(GetReturnValue(isolate, bsize));
            args.push_back(GetReturnValue(isolate, blocks));
            args.push_back(GetReturnValue(isolate, bfree));
            args.push_back(GetReturnValue(isolate, bavail));
            args.push_back(GetReturnValue(isolate, files));
            args.push_back(GetReturnValue(isolate, ffree));
        }

    public:
        double type;
        double bsize;
        double blocks;
        double bfree;
        double bavail;
        double files;
        double ffree;
    };

public:
    // fs_base
    static result_t exists(exlib::string path, bool& retVal, AsyncHandle ac);
    static result_t exists(exlib::string path, v8::Local<v8::Object> options, bool& retVal, AsyncHandle ac);
    static result_t access(exlib::string path, int32_t mode, AsyncHandle ac);
    static result_t link(exlib::string oldPath, exlib::string newPath, AsyncHandle ac);
    static result_t unlink(exlib::string path, AsyncHandle ac);
    static result_t mkdir(exlib::string path, Union_mkdir_mode mode, Variant& retVal, AsyncHandle ac);
    static result_t mkdtemp(exlib::string prefix, exlib::string& retVal, AsyncHandle ac);
    static result_t rmdir(exlib::string path, v8::Local<v8::Object> opt, AsyncHandle ac);
    static result_t rm(exlib::string path, v8::Local<v8::Object> opt, AsyncHandle ac);
    static result_t rename(exlib::string from, exlib::string to, AsyncHandle ac);
    static result_t copyFile(exlib::string from, exlib::string to, int32_t mode, AsyncHandle ac);
    static result_t cp(exlib::string src, exlib::string dest, v8::Local<v8::Object> opts, AsyncHandle ac);
    static result_t chmod(exlib::string path, Union_chmod_mode mode, AsyncHandle ac);
    static result_t lchmod(exlib::string path, Union_lchmod_mode mode, AsyncHandle ac);
    static result_t chown(exlib::string path, int32_t uid, int32_t gid, AsyncHandle ac);
    static result_t lchown(exlib::string path, int32_t uid, int32_t gid, AsyncHandle ac);
    static result_t utimes(exlib::string path, Variant atime, Variant mtime, AsyncHandle ac);
    static result_t lutimes(exlib::string path, Variant atime, Variant mtime, AsyncHandle ac);
    static result_t stat(exlib::string path, obj_ptr<Stat_base>& retVal, AsyncHandle ac);
    static result_t stat(exlib::string path, v8::Local<v8::Object> options, obj_ptr<Stat_base>& retVal, AsyncHandle ac);
    static result_t lstat(exlib::string path, obj_ptr<Stat_base>& retVal, AsyncHandle ac);
    static result_t lstat(exlib::string path, v8::Local<v8::Object> options, obj_ptr<Stat_base>& retVal, AsyncHandle ac);
    static result_t fstat(Union_fstat_fd fd, obj_ptr<Stat_base>& retVal, AsyncHandle ac);
    static result_t fstat(Union_fstat_fd fd, v8::Local<v8::Object> options, obj_ptr<Stat_base>& retVal, AsyncHandle ac);
    static result_t readlink(exlib::string path, Variant& retVal, AsyncHandle ac);
    static result_t readlink(exlib::string path, Union_readlink_options options, Variant& retVal, AsyncHandle ac);
    static result_t realpath(exlib::string path, Variant& retVal, AsyncHandle ac);
    static result_t realpath(exlib::string path, Union_realpath_options options, Variant& retVal, AsyncHandle ac);
    static result_t symlink(exlib::string target, exlib::string linkpath, exlib::string type, AsyncHandle ac);
    static result_t truncate(exlib::string path, int32_t len, AsyncHandle ac);
    static result_t read(Union_read_fd fd, Buffer_base* buffer, int32_t offset, int32_t length, int32_t position, int32_t& retVal, AsyncHandle ac);
    static result_t fchmod(Union_fchmod_fd fd, int32_t mode, AsyncHandle ac);
    static result_t fchown(Union_fchown_fd fd, int32_t uid, int32_t gid, AsyncHandle ac);
    static result_t futimes(Union_futimes_fd fd, Variant atime, Variant mtime, AsyncHandle ac);
    static result_t fdatasync(Union_fdatasync_fd fd, AsyncHandle ac);
    static result_t fsync(Union_fsync_fd fd, AsyncHandle ac);
    static result_t ftruncate(Union_ftruncate_fd fd, int32_t len, AsyncHandle ac);
    static result_t statfs(exlib::string path, obj_ptr<StatfsType>& retVal, AsyncHandle ac);
    static result_t readdir(exlib::string path, obj_ptr<NArray>& retVal, AsyncHandle ac);
    static result_t readdir(exlib::string path, Union_readdir_opts opts, obj_ptr<NArray>& retVal, AsyncHandle ac);
    static result_t opendir(exlib::string path, obj_ptr<Dir_base>& retVal, AsyncHandle ac);
    static result_t glob(exlib::string pattern, v8::Local<v8::Object> opts, obj_ptr<NArray>& retVal, AsyncHandle ac);
    static result_t glob(std::vector<exlib::string>& patterns, v8::Local<v8::Object> opts, obj_ptr<NArray>& retVal, AsyncHandle ac);
    static result_t createReadStream(exlib::string fname, v8::Local<v8::Object> options, obj_ptr<SeekableStream_base>& retVal, AsyncHandle ac);
    static result_t createWriteStream(exlib::string fname, v8::Local<v8::Object> options, obj_ptr<SeekableStream_base>& retVal, AsyncHandle ac);
    static result_t openFile(exlib::string fname, Union_openFile_flags flags, obj_ptr<SeekableStream_base>& retVal, AsyncHandle ac);
    static result_t open(exlib::string fname, int32_t flags, int32_t mode, obj_ptr<FileHandle_base>& retVal, AsyncHandle ac);
    static result_t open(exlib::string fname, exlib::string flags, Variant mode, obj_ptr<FileHandle_base>& retVal, AsyncHandle ac);
    static result_t open(exlib::string fname, exlib::string flags, int32_t mode, obj_ptr<FileHandle_base>& retVal, AsyncHandle ac);
    static result_t close(Union_close_fd fd, AsyncHandle ac);
    static result_t openTextStream(exlib::string fname, exlib::string flags, obj_ptr<BufferedStream_base>& retVal, AsyncHandle ac);
    static result_t readTextFile(exlib::string fname, exlib::string& retVal, AsyncHandle ac);
    static result_t readFile(Union_readFile_fname fname, Union_readFile_options options, Variant& retVal, AsyncHandle ac);
    static result_t readLines(exlib::string fname, int32_t maxlines, std::vector<exlib::string>& retVal);
    static result_t write(Union_write_fd fd, Buffer_base* buffer, int32_t offset, int32_t length, int32_t position, int32_t& retVal, AsyncHandle ac);
    static result_t write(Union_write_fd fd, exlib::string string, int32_t position, exlib::string encoding, int32_t& retVal, AsyncHandle ac);
    static result_t writeTextFile(exlib::string fname, exlib::string txt, int32_t& retVal, AsyncHandle ac);
    static result_t writeFile(Union_writeFile_fname fname, Union_writeFile_data data, Union_writeFile_opt opt, int32_t& retVal, AsyncHandle ac);
    static result_t appendFile(Union_appendFile_fname fname, Union_appendFile_data data, Union_appendFile_options options, int32_t& retVal, AsyncHandle ac);
    static result_t setZipFS(exlib::string fname, Union_setZipFS_data data);
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

        ThrowTypeError("not a constructor");
    }

    static result_t load(v8::Local<v8::Value> v, obj_ptr<fs_base>& retVal)
    { return CALL_E_TYPEMISMATCH; }

public:
    static void s_static_exists(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_access(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_link(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_unlink(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_mkdir(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_mkdtemp(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_rmdir(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_rm(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_rename(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_copyFile(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_cp(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_chmod(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_lchmod(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_chown(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_lchown(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_utimes(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_lutimes(const v8::FunctionCallbackInfo<v8::Value>& args);
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
    static void s_static_futimes(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_fdatasync(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_fsync(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_ftruncate(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_statfs(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_readdir(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_opendir(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_glob(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_createReadStream(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_createWriteStream(const v8::FunctionCallbackInfo<v8::Value>& args);
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
    ASYNC_STATICVALUE3(fs_base, exists, exlib::string, v8::Local<v8::Object>, bool);
    ASYNC_STATIC2(fs_base, access, exlib::string, int32_t);
    ASYNC_STATIC2(fs_base, link, exlib::string, exlib::string);
    ASYNC_STATIC1(fs_base, unlink, exlib::string);
    ASYNC_STATICVALUE3(fs_base, mkdir, exlib::string, Union_mkdir_mode, Variant);
    ASYNC_STATICVALUE2(fs_base, mkdtemp, exlib::string, exlib::string);
    ASYNC_STATIC2(fs_base, rmdir, exlib::string, v8::Local<v8::Object>);
    ASYNC_STATIC2(fs_base, rm, exlib::string, v8::Local<v8::Object>);
    ASYNC_STATIC2(fs_base, rename, exlib::string, exlib::string);
    ASYNC_STATIC3(fs_base, copyFile, exlib::string, exlib::string, int32_t);
    ASYNC_STATIC3(fs_base, cp, exlib::string, exlib::string, v8::Local<v8::Object>);
    ASYNC_STATIC2(fs_base, chmod, exlib::string, Union_chmod_mode);
    ASYNC_STATIC2(fs_base, lchmod, exlib::string, Union_lchmod_mode);
    ASYNC_STATIC3(fs_base, chown, exlib::string, int32_t, int32_t);
    ASYNC_STATIC3(fs_base, lchown, exlib::string, int32_t, int32_t);
    ASYNC_STATIC3(fs_base, utimes, exlib::string, Variant, Variant);
    ASYNC_STATIC3(fs_base, lutimes, exlib::string, Variant, Variant);
    ASYNC_STATICVALUE2(fs_base, stat, exlib::string, obj_ptr<Stat_base>);
    ASYNC_STATICVALUE3(fs_base, stat, exlib::string, v8::Local<v8::Object>, obj_ptr<Stat_base>);
    ASYNC_STATICVALUE2(fs_base, lstat, exlib::string, obj_ptr<Stat_base>);
    ASYNC_STATICVALUE3(fs_base, lstat, exlib::string, v8::Local<v8::Object>, obj_ptr<Stat_base>);
    ASYNC_STATICVALUE2(fs_base, fstat, Union_fstat_fd, obj_ptr<Stat_base>);
    ASYNC_STATICVALUE3(fs_base, fstat, Union_fstat_fd, v8::Local<v8::Object>, obj_ptr<Stat_base>);
    ASYNC_STATICVALUE2(fs_base, readlink, exlib::string, Variant);
    ASYNC_STATICVALUE3(fs_base, readlink, exlib::string, Union_readlink_options, Variant);
    ASYNC_STATICVALUE2(fs_base, realpath, exlib::string, Variant);
    ASYNC_STATICVALUE3(fs_base, realpath, exlib::string, Union_realpath_options, Variant);
    ASYNC_STATIC3(fs_base, symlink, exlib::string, exlib::string, exlib::string);
    ASYNC_STATIC2(fs_base, truncate, exlib::string, int32_t);
    ASYNC_STATICVALUE6(fs_base, read, Union_read_fd, Buffer_base*, int32_t, int32_t, int32_t, int32_t);
    ASYNC_STATIC2(fs_base, fchmod, Union_fchmod_fd, int32_t);
    ASYNC_STATIC3(fs_base, fchown, Union_fchown_fd, int32_t, int32_t);
    ASYNC_STATIC3(fs_base, futimes, Union_futimes_fd, Variant, Variant);
    ASYNC_STATIC1(fs_base, fdatasync, Union_fdatasync_fd);
    ASYNC_STATIC1(fs_base, fsync, Union_fsync_fd);
    ASYNC_STATIC2(fs_base, ftruncate, Union_ftruncate_fd, int32_t);
    ASYNC_STATICVALUE2(fs_base, statfs, exlib::string, obj_ptr<StatfsType>);
    ASYNC_STATICVALUE2(fs_base, readdir, exlib::string, obj_ptr<NArray>);
    ASYNC_STATICVALUE3(fs_base, readdir, exlib::string, Union_readdir_opts, obj_ptr<NArray>);
    ASYNC_STATICVALUE2(fs_base, opendir, exlib::string, obj_ptr<Dir_base>);
    ASYNC_STATICVALUE3(fs_base, glob, exlib::string, v8::Local<v8::Object>, obj_ptr<NArray>);
    ASYNC_STATICVALUE3(fs_base, glob, std::vector<exlib::string>, v8::Local<v8::Object>, obj_ptr<NArray>);
    ASYNC_STATICVALUE3(fs_base, createReadStream, exlib::string, v8::Local<v8::Object>, obj_ptr<SeekableStream_base>);
    ASYNC_STATICVALUE3(fs_base, createWriteStream, exlib::string, v8::Local<v8::Object>, obj_ptr<SeekableStream_base>);
    ASYNC_STATICVALUE3(fs_base, openFile, exlib::string, Union_openFile_flags, obj_ptr<SeekableStream_base>);
    ASYNC_STATICVALUE4(fs_base, open, exlib::string, int32_t, int32_t, obj_ptr<FileHandle_base>);
    ASYNC_STATICVALUE4(fs_base, open, exlib::string, exlib::string, Variant, obj_ptr<FileHandle_base>);
    ASYNC_STATICVALUE4(fs_base, open, exlib::string, exlib::string, int32_t, obj_ptr<FileHandle_base>);
    ASYNC_STATIC1(fs_base, close, Union_close_fd);
    ASYNC_STATICVALUE3(fs_base, openTextStream, exlib::string, exlib::string, obj_ptr<BufferedStream_base>);
    ASYNC_STATICVALUE2(fs_base, readTextFile, exlib::string, exlib::string);
    ASYNC_STATICVALUE3(fs_base, readFile, Union_readFile_fname, Union_readFile_options, Variant);
    ASYNC_STATICVALUE6(fs_base, write, Union_write_fd, Buffer_base*, int32_t, int32_t, int32_t, int32_t);
    ASYNC_STATICVALUE5(fs_base, write, Union_write_fd, exlib::string, int32_t, exlib::string, int32_t);
    ASYNC_STATICVALUE3(fs_base, writeTextFile, exlib::string, exlib::string, int32_t);
    ASYNC_STATICVALUE4(fs_base, writeFile, Union_writeFile_fname, Union_writeFile_data, Union_writeFile_opt, int32_t);
    ASYNC_STATICVALUE4(fs_base, appendFile, Union_appendFile_fname, Union_appendFile_data, Union_appendFile_options, int32_t);
};
}

#include "ifs/fs_constants.h"
#include "ifs/Stat.h"
#include "ifs/DirEntry.h"
#include "ifs/Dir.h"
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
        { "exists", s_static_exists, true, ClassData::ASYNC_ASYNC },
        { "access", s_static_access, true, ClassData::ASYNC_ASYNC },
        { "link", s_static_link, true, ClassData::ASYNC_ASYNC },
        { "unlink", s_static_unlink, true, ClassData::ASYNC_ASYNC },
        { "mkdir", s_static_mkdir, true, ClassData::ASYNC_ASYNC },
        { "mkdtemp", s_static_mkdtemp, true, ClassData::ASYNC_ASYNC },
        { "rmdir", s_static_rmdir, true, ClassData::ASYNC_ASYNC },
        { "rm", s_static_rm, true, ClassData::ASYNC_ASYNC },
        { "rename", s_static_rename, true, ClassData::ASYNC_ASYNC },
        { "copyFile", s_static_copyFile, true, ClassData::ASYNC_ASYNC },
        { "cp", s_static_cp, true, ClassData::ASYNC_ASYNC },
        { "chmod", s_static_chmod, true, ClassData::ASYNC_ASYNC },
        { "lchmod", s_static_lchmod, true, ClassData::ASYNC_ASYNC },
        { "chown", s_static_chown, true, ClassData::ASYNC_ASYNC },
        { "lchown", s_static_lchown, true, ClassData::ASYNC_ASYNC },
        { "utimes", s_static_utimes, true, ClassData::ASYNC_ASYNC },
        { "lutimes", s_static_lutimes, true, ClassData::ASYNC_ASYNC },
        { "stat", s_static_stat, true, ClassData::ASYNC_ASYNC },
        { "lstat", s_static_lstat, true, ClassData::ASYNC_ASYNC },
        { "fstat", s_static_fstat, true, ClassData::ASYNC_ASYNC },
        { "readlink", s_static_readlink, true, ClassData::ASYNC_ASYNC },
        { "realpath", s_static_realpath, true, ClassData::ASYNC_ASYNC },
        { "symlink", s_static_symlink, true, ClassData::ASYNC_ASYNC },
        { "truncate", s_static_truncate, true, ClassData::ASYNC_ASYNC },
        { "read", s_static_read, true, ClassData::ASYNC_ASYNC },
        { "fchmod", s_static_fchmod, true, ClassData::ASYNC_ASYNC },
        { "fchown", s_static_fchown, true, ClassData::ASYNC_ASYNC },
        { "futimes", s_static_futimes, true, ClassData::ASYNC_ASYNC },
        { "fdatasync", s_static_fdatasync, true, ClassData::ASYNC_ASYNC },
        { "fsync", s_static_fsync, true, ClassData::ASYNC_ASYNC },
        { "ftruncate", s_static_ftruncate, true, ClassData::ASYNC_ASYNC },
        { "statfs", s_static_statfs, true, ClassData::ASYNC_ASYNC },
        { "readdir", s_static_readdir, true, ClassData::ASYNC_ASYNC },
        { "opendir", s_static_opendir, true, ClassData::ASYNC_ASYNC },
        { "glob", s_static_glob, true, ClassData::ASYNC_ASYNC },
        { "createReadStream", s_static_createReadStream, true, ClassData::ASYNC_ASYNC },
        { "createWriteStream", s_static_createWriteStream, true, ClassData::ASYNC_ASYNC },
        { "openFile", s_static_openFile, true, ClassData::ASYNC_ASYNC },
        { "open", s_static_open, true, ClassData::ASYNC_ASYNC },
        { "close", s_static_close, true, ClassData::ASYNC_ASYNC },
        { "openTextStream", s_static_openTextStream, true, ClassData::ASYNC_ASYNC },
        { "readTextFile", s_static_readTextFile, true, ClassData::ASYNC_ASYNC },
        { "readFile", s_static_readFile, true, ClassData::ASYNC_ASYNC },
        { "readLines", s_static_readLines, true, ClassData::ASYNC_SYNC },
        { "write", s_static_write, true, ClassData::ASYNC_ASYNC },
        { "writeTextFile", s_static_writeTextFile, true, ClassData::ASYNC_ASYNC },
        { "writeFile", s_static_writeFile, true, ClassData::ASYNC_ASYNC },
        { "appendFile", s_static_appendFile, true, ClassData::ASYNC_ASYNC },
        { "setZipFS", s_static_setZipFS, true, ClassData::ASYNC_SYNC },
        { "clearZipFS", s_static_clearZipFS, true, ClassData::ASYNC_SYNC },
        { "watch", s_static_watch, true, ClassData::ASYNC_SYNC },
        { "watchFile", s_static_watchFile, true, ClassData::ASYNC_SYNC },
        { "unwatchFile", s_static_unwatchFile, true, ClassData::ASYNC_SYNC }
    };

    static ClassData::ClassObject s_object[] = {
        { "constants", fs_constants_base::class_info },
        { "Stats", Stat_base::class_info },
        { "Dirent", DirEntry_base::class_info },
        { "Dir", Dir_base::class_info }
    };

    static ClassData::ClassConst s_const[] = {
        { "SEEK_SET", ClassData::CONST_Integer, { .intValue = C_SEEK_SET } },
        { "SEEK_CUR", ClassData::CONST_Integer, { .intValue = C_SEEK_CUR } },
        { "SEEK_END", ClassData::CONST_Integer, { .intValue = C_SEEK_END } },
        { "F_OK", ClassData::CONST_Integer, { .intValue = C_F_OK } },
        { "R_OK", ClassData::CONST_Integer, { .intValue = C_R_OK } },
        { "W_OK", ClassData::CONST_Integer, { .intValue = C_W_OK } },
        { "X_OK", ClassData::CONST_Integer, { .intValue = C_X_OK } }
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

    ASYNC_METHOD_ENTER("fs.exists");

    METHOD_OVER(1, 1);

    ARG(exlib::string, 0);

    if (!cb.IsEmpty())
        hr = acb_exists(v0, cb, args);
    else
        hr = ac_exists(v0, vr);

    METHOD_OVER(2, 2);

    ARG(exlib::string, 0);
    ARG(v8::Local<v8::Object>, 1);

    if (!cb.IsEmpty())
        hr = acb_exists(v0, v1, cb, args);
    else
        hr = ac_exists(v0, v1, vr);

    ASYNC_METHOD_RETURN();
}

inline void fs_base::s_static_access(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    ASYNC_METHOD_ENTER("fs.access");

    METHOD_OVER(2, 1);

    ARG(exlib::string, 0);
    OPT_ARG(int32_t, 1, 0);

    if (!cb.IsEmpty())
        hr = acb_access(v0, v1, cb, args);
    else
        hr = ac_access(v0, v1);

    ASYNC_METHOD_VOID();
}

inline void fs_base::s_static_link(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    ASYNC_METHOD_ENTER("fs.link");

    METHOD_OVER(2, 2);

    ARG(exlib::string, 0);
    ARG(exlib::string, 1);

    if (!cb.IsEmpty())
        hr = acb_link(v0, v1, cb, args);
    else
        hr = ac_link(v0, v1);

    ASYNC_METHOD_VOID();
}

inline void fs_base::s_static_unlink(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    ASYNC_METHOD_ENTER("fs.unlink");

    METHOD_OVER(1, 1);

    ARG(exlib::string, 0);

    if (!cb.IsEmpty())
        hr = acb_unlink(v0, cb, args);
    else
        hr = ac_unlink(v0);

    ASYNC_METHOD_VOID();
}

inline void fs_base::s_static_mkdir(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    Variant vr;

    ASYNC_METHOD_ENTER("fs.mkdir");

    METHOD_OVER(2, 1);

    ARG(exlib::string, 0);
    OPT_ARG(Union_mkdir_mode, 1, 0777);

    if (!cb.IsEmpty())
        hr = acb_mkdir(v0, v1, cb, args);
    else
        hr = ac_mkdir(v0, v1, vr);

    ASYNC_METHOD_RETURN();
}

inline void fs_base::s_static_mkdtemp(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    ASYNC_METHOD_ENTER("fs.mkdtemp");

    METHOD_OVER(1, 1);

    ARG(exlib::string, 0);

    if (!cb.IsEmpty())
        hr = acb_mkdtemp(v0, cb, args);
    else
        hr = ac_mkdtemp(v0, vr);

    ASYNC_METHOD_RETURN();
}

inline void fs_base::s_static_rmdir(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    ASYNC_METHOD_ENTER("fs.rmdir");

    METHOD_OVER(2, 1);

    ARG(exlib::string, 0);
    OPT_ARG(v8::Local<v8::Object>, 1, v8::Object::New(isolate->m_isolate));

    if (!cb.IsEmpty())
        hr = acb_rmdir(v0, v1, cb, args);
    else
        hr = ac_rmdir(v0, v1);

    ASYNC_METHOD_VOID();
}

inline void fs_base::s_static_rm(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    ASYNC_METHOD_ENTER("fs.rm");

    METHOD_OVER(2, 1);

    ARG(exlib::string, 0);
    OPT_ARG(v8::Local<v8::Object>, 1, v8::Object::New(isolate->m_isolate));

    if (!cb.IsEmpty())
        hr = acb_rm(v0, v1, cb, args);
    else
        hr = ac_rm(v0, v1);

    ASYNC_METHOD_VOID();
}

inline void fs_base::s_static_rename(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    ASYNC_METHOD_ENTER("fs.rename");

    METHOD_OVER(2, 2);

    ARG(exlib::string, 0);
    ARG(exlib::string, 1);

    if (!cb.IsEmpty())
        hr = acb_rename(v0, v1, cb, args);
    else
        hr = ac_rename(v0, v1);

    ASYNC_METHOD_VOID();
}

inline void fs_base::s_static_copyFile(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    ASYNC_METHOD_ENTER("fs.copyFile");

    METHOD_OVER(3, 2);

    ARG(exlib::string, 0);
    ARG(exlib::string, 1);
    OPT_ARG(int32_t, 2, 0);

    if (!cb.IsEmpty())
        hr = acb_copyFile(v0, v1, v2, cb, args);
    else
        hr = ac_copyFile(v0, v1, v2);

    ASYNC_METHOD_VOID();
}

inline void fs_base::s_static_cp(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    ASYNC_METHOD_ENTER("fs.cp");

    METHOD_OVER(3, 2);

    ARG(exlib::string, 0);
    ARG(exlib::string, 1);
    OPT_ARG(v8::Local<v8::Object>, 2, v8::Object::New(isolate->m_isolate));

    if (!cb.IsEmpty())
        hr = acb_cp(v0, v1, v2, cb, args);
    else
        hr = ac_cp(v0, v1, v2);

    ASYNC_METHOD_VOID();
}

inline void fs_base::s_static_chmod(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    ASYNC_METHOD_ENTER("fs.chmod");

    METHOD_OVER(2, 2);

    ARG(exlib::string, 0);
    ARG(Union_chmod_mode, 1);

    if (!cb.IsEmpty())
        hr = acb_chmod(v0, v1, cb, args);
    else
        hr = ac_chmod(v0, v1);

    ASYNC_METHOD_VOID();
}

inline void fs_base::s_static_lchmod(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    ASYNC_METHOD_ENTER("fs.lchmod");

    METHOD_OVER(2, 2);

    ARG(exlib::string, 0);
    ARG(Union_lchmod_mode, 1);

    if (!cb.IsEmpty())
        hr = acb_lchmod(v0, v1, cb, args);
    else
        hr = ac_lchmod(v0, v1);

    ASYNC_METHOD_VOID();
}

inline void fs_base::s_static_chown(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    ASYNC_METHOD_ENTER("fs.chown");

    METHOD_OVER(3, 3);

    ARG(exlib::string, 0);
    ARG(int32_t, 1);
    ARG(int32_t, 2);

    if (!cb.IsEmpty())
        hr = acb_chown(v0, v1, v2, cb, args);
    else
        hr = ac_chown(v0, v1, v2);

    ASYNC_METHOD_VOID();
}

inline void fs_base::s_static_lchown(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    ASYNC_METHOD_ENTER("fs.lchown");

    METHOD_OVER(3, 3);

    ARG(exlib::string, 0);
    ARG(int32_t, 1);
    ARG(int32_t, 2);

    if (!cb.IsEmpty())
        hr = acb_lchown(v0, v1, v2, cb, args);
    else
        hr = ac_lchown(v0, v1, v2);

    ASYNC_METHOD_VOID();
}

inline void fs_base::s_static_utimes(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    ASYNC_METHOD_ENTER("fs.utimes");

    METHOD_OVER(3, 3);

    ARG(exlib::string, 0);
    ARG(Variant, 1);
    ARG(Variant, 2);

    if (!cb.IsEmpty())
        hr = acb_utimes(v0, v1, v2, cb, args);
    else
        hr = ac_utimes(v0, v1, v2);

    ASYNC_METHOD_VOID();
}

inline void fs_base::s_static_lutimes(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    ASYNC_METHOD_ENTER("fs.lutimes");

    METHOD_OVER(3, 3);

    ARG(exlib::string, 0);
    ARG(Variant, 1);
    ARG(Variant, 2);

    if (!cb.IsEmpty())
        hr = acb_lutimes(v0, v1, v2, cb, args);
    else
        hr = ac_lutimes(v0, v1, v2);

    ASYNC_METHOD_VOID();
}

inline void fs_base::s_static_stat(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<Stat_base> vr;

    ASYNC_METHOD_ENTER("fs.stat");

    METHOD_OVER(1, 1);

    ARG(exlib::string, 0);

    if (!cb.IsEmpty())
        hr = acb_stat(v0, cb, args);
    else
        hr = ac_stat(v0, vr);

    METHOD_OVER(2, 2);

    ARG(exlib::string, 0);
    ARG(v8::Local<v8::Object>, 1);

    if (!cb.IsEmpty())
        hr = acb_stat(v0, v1, cb, args);
    else
        hr = ac_stat(v0, v1, vr);

    ASYNC_METHOD_RETURN();
}

inline void fs_base::s_static_lstat(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<Stat_base> vr;

    ASYNC_METHOD_ENTER("fs.lstat");

    METHOD_OVER(1, 1);

    ARG(exlib::string, 0);

    if (!cb.IsEmpty())
        hr = acb_lstat(v0, cb, args);
    else
        hr = ac_lstat(v0, vr);

    METHOD_OVER(2, 2);

    ARG(exlib::string, 0);
    ARG(v8::Local<v8::Object>, 1);

    if (!cb.IsEmpty())
        hr = acb_lstat(v0, v1, cb, args);
    else
        hr = ac_lstat(v0, v1, vr);

    ASYNC_METHOD_RETURN();
}

inline void fs_base::s_static_fstat(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<Stat_base> vr;

    ASYNC_METHOD_ENTER("fs.fstat");

    METHOD_OVER(1, 1);

    ARG(Union_fstat_fd, 0);

    if (!cb.IsEmpty())
        hr = acb_fstat(v0, cb, args);
    else
        hr = ac_fstat(v0, vr);

    METHOD_OVER(2, 2);

    ARG(Union_fstat_fd, 0);
    ARG(v8::Local<v8::Object>, 1);

    if (!cb.IsEmpty())
        hr = acb_fstat(v0, v1, cb, args);
    else
        hr = ac_fstat(v0, v1, vr);

    ASYNC_METHOD_RETURN();
}

inline void fs_base::s_static_readlink(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    Variant vr;

    ASYNC_METHOD_ENTER("fs.readlink");

    METHOD_OVER(1, 1);

    ARG(exlib::string, 0);

    if (!cb.IsEmpty())
        hr = acb_readlink(v0, cb, args);
    else
        hr = ac_readlink(v0, vr);

    METHOD_OVER(2, 2);

    ARG(exlib::string, 0);
    ARG(Union_readlink_options, 1);

    if (!cb.IsEmpty())
        hr = acb_readlink(v0, v1, cb, args);
    else
        hr = ac_readlink(v0, v1, vr);

    ASYNC_METHOD_RETURN();
}

inline void fs_base::s_static_realpath(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    Variant vr;

    ASYNC_METHOD_ENTER("fs.realpath");

    METHOD_OVER(1, 1);

    ARG(exlib::string, 0);

    if (!cb.IsEmpty())
        hr = acb_realpath(v0, cb, args);
    else
        hr = ac_realpath(v0, vr);

    METHOD_OVER(2, 2);

    ARG(exlib::string, 0);
    ARG(Union_realpath_options, 1);

    if (!cb.IsEmpty())
        hr = acb_realpath(v0, v1, cb, args);
    else
        hr = ac_realpath(v0, v1, vr);

    ASYNC_METHOD_RETURN();
}

inline void fs_base::s_static_symlink(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    ASYNC_METHOD_ENTER("fs.symlink");

    METHOD_OVER(3, 2);

    ARG(exlib::string, 0);
    ARG(exlib::string, 1);
    OPT_ARG(exlib::string, 2, "file");

    if (!cb.IsEmpty())
        hr = acb_symlink(v0, v1, v2, cb, args);
    else
        hr = ac_symlink(v0, v1, v2);

    ASYNC_METHOD_VOID();
}

inline void fs_base::s_static_truncate(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    ASYNC_METHOD_ENTER("fs.truncate");

    METHOD_OVER(2, 2);

    ARG(exlib::string, 0);
    ARG(int32_t, 1);

    if (!cb.IsEmpty())
        hr = acb_truncate(v0, v1, cb, args);
    else
        hr = ac_truncate(v0, v1);

    ASYNC_METHOD_VOID();
}

inline void fs_base::s_static_read(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    int32_t vr;

    ASYNC_METHOD_ENTER("fs.read");

    METHOD_OVER(5, 2);

    ARG(Union_read_fd, 0);
    ARG(obj_ptr<Buffer_base>, 1);
    OPT_ARG(int32_t, 2, 0);
    OPT_ARG(int32_t, 3, 0);
    OPT_ARG(int32_t, 4, -1);

    if (!cb.IsEmpty())
        hr = acb_read(v0, v1.get(), v2, v3, v4, cb, args);
    else
        hr = ac_read(v0, v1.get(), v2, v3, v4, vr);

    ASYNC_METHOD_RETURN();
}

inline void fs_base::s_static_fchmod(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    ASYNC_METHOD_ENTER("fs.fchmod");

    METHOD_OVER(2, 2);

    ARG(Union_fchmod_fd, 0);
    ARG(int32_t, 1);

    if (!cb.IsEmpty())
        hr = acb_fchmod(v0, v1, cb, args);
    else
        hr = ac_fchmod(v0, v1);

    ASYNC_METHOD_VOID();
}

inline void fs_base::s_static_fchown(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    ASYNC_METHOD_ENTER("fs.fchown");

    METHOD_OVER(3, 3);

    ARG(Union_fchown_fd, 0);
    ARG(int32_t, 1);
    ARG(int32_t, 2);

    if (!cb.IsEmpty())
        hr = acb_fchown(v0, v1, v2, cb, args);
    else
        hr = ac_fchown(v0, v1, v2);

    ASYNC_METHOD_VOID();
}

inline void fs_base::s_static_futimes(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    ASYNC_METHOD_ENTER("fs.futimes");

    METHOD_OVER(3, 3);

    ARG(Union_futimes_fd, 0);
    ARG(Variant, 1);
    ARG(Variant, 2);

    if (!cb.IsEmpty())
        hr = acb_futimes(v0, v1, v2, cb, args);
    else
        hr = ac_futimes(v0, v1, v2);

    ASYNC_METHOD_VOID();
}

inline void fs_base::s_static_fdatasync(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    ASYNC_METHOD_ENTER("fs.fdatasync");

    METHOD_OVER(1, 1);

    ARG(Union_fdatasync_fd, 0);

    if (!cb.IsEmpty())
        hr = acb_fdatasync(v0, cb, args);
    else
        hr = ac_fdatasync(v0);

    ASYNC_METHOD_VOID();
}

inline void fs_base::s_static_fsync(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    ASYNC_METHOD_ENTER("fs.fsync");

    METHOD_OVER(1, 1);

    ARG(Union_fsync_fd, 0);

    if (!cb.IsEmpty())
        hr = acb_fsync(v0, cb, args);
    else
        hr = ac_fsync(v0);

    ASYNC_METHOD_VOID();
}

inline void fs_base::s_static_ftruncate(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    ASYNC_METHOD_ENTER("fs.ftruncate");

    METHOD_OVER(2, 1);

    ARG(Union_ftruncate_fd, 0);
    OPT_ARG(int32_t, 1, 0);

    if (!cb.IsEmpty())
        hr = acb_ftruncate(v0, v1, cb, args);
    else
        hr = ac_ftruncate(v0, v1);

    ASYNC_METHOD_VOID();
}

inline void fs_base::s_static_statfs(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<StatfsType> vr;

    ASYNC_METHOD_ENTER("fs.statfs");

    METHOD_OVER(1, 1);

    ARG(exlib::string, 0);

    if (!cb.IsEmpty())
        hr = acb_statfs(v0, cb, args);
    else
        hr = ac_statfs(v0, vr);

    ASYNC_METHOD_RETURN();
}

inline void fs_base::s_static_readdir(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<NArray> vr;

    ASYNC_METHOD_ENTER("fs.readdir");

    METHOD_OVER(1, 1);

    ARG(exlib::string, 0);

    if (!cb.IsEmpty())
        hr = acb_readdir(v0, cb, args);
    else
        hr = ac_readdir(v0, vr);

    METHOD_OVER(2, 1);

    ARG(exlib::string, 0);
    OPT_ARG(Union_readdir_opts, 1, v8::Object::New(isolate->m_isolate));

    if (!cb.IsEmpty())
        hr = acb_readdir(v0, v1, cb, args);
    else
        hr = ac_readdir(v0, v1, vr);

    ASYNC_METHOD_RETURN();
}

inline void fs_base::s_static_opendir(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<Dir_base> vr;

    ASYNC_METHOD_ENTER("fs.opendir");

    METHOD_OVER(1, 1);

    ARG(exlib::string, 0);

    if (!cb.IsEmpty())
        hr = acb_opendir(v0, cb, args);
    else
        hr = ac_opendir(v0, vr);

    ASYNC_METHOD_RETURN();
}

inline void fs_base::s_static_glob(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<NArray> vr;

    ASYNC_METHOD_ENTER("fs.glob");

    METHOD_OVER(2, 1);

    ARG(exlib::string, 0);
    OPT_ARG(v8::Local<v8::Object>, 1, v8::Object::New(isolate->m_isolate));

    if (!cb.IsEmpty())
        hr = acb_glob(v0, v1, cb, args);
    else
        hr = ac_glob(v0, v1, vr);

    METHOD_OVER(2, 1);

    ARG(std::vector<exlib::string>, 0);
    OPT_ARG(v8::Local<v8::Object>, 1, v8::Object::New(isolate->m_isolate));

    if (!cb.IsEmpty())
        hr = acb_glob(v0, v1, cb, args);
    else
        hr = ac_glob(v0, v1, vr);

    ASYNC_METHOD_RETURN();
}

inline void fs_base::s_static_createReadStream(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<SeekableStream_base> vr;

    ASYNC_METHOD_ENTER("fs.createReadStream");

    METHOD_OVER(2, 1);

    ARG(exlib::string, 0);
    OPT_ARG(v8::Local<v8::Object>, 1, v8::Object::New(isolate->m_isolate));

    if (!cb.IsEmpty())
        hr = acb_createReadStream(v0, v1, cb, args);
    else
        hr = ac_createReadStream(v0, v1, vr);

    ASYNC_METHOD_RETURN();
}

inline void fs_base::s_static_createWriteStream(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<SeekableStream_base> vr;

    ASYNC_METHOD_ENTER("fs.createWriteStream");

    METHOD_OVER(2, 1);

    ARG(exlib::string, 0);
    OPT_ARG(v8::Local<v8::Object>, 1, v8::Object::New(isolate->m_isolate));

    if (!cb.IsEmpty())
        hr = acb_createWriteStream(v0, v1, cb, args);
    else
        hr = ac_createWriteStream(v0, v1, vr);

    ASYNC_METHOD_RETURN();
}

inline void fs_base::s_static_openFile(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<SeekableStream_base> vr;

    ASYNC_METHOD_ENTER("fs.openFile");

    METHOD_OVER(2, 1);

    ARG(exlib::string, 0);
    OPT_ARG(Union_openFile_flags, 1, exlib::string("r"));

    if (!cb.IsEmpty())
        hr = acb_openFile(v0, v1, cb, args);
    else
        hr = ac_openFile(v0, v1, vr);

    ASYNC_METHOD_RETURN();
}

inline void fs_base::s_static_open(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<FileHandle_base> vr;

    ASYNC_METHOD_ENTER("fs.open");

    METHOD_OVER(3, 2);

    ARG(exlib::string, 0);
    ARG(int32_t, 1);
    OPT_ARG(int32_t, 2, 0666);

    if (!cb.IsEmpty())
        hr = acb_open(v0, v1, v2, cb, args);
    else
        hr = ac_open(v0, v1, v2, vr);

    METHOD_OVER(3, 3);

    ARG(exlib::string, 0);
    ARG(exlib::string, 1);
    ARG(Variant, 2);

    if (!cb.IsEmpty())
        hr = acb_open(v0, v1, v2, cb, args);
    else
        hr = ac_open(v0, v1, v2, vr);

    METHOD_OVER(3, 1);

    ARG(exlib::string, 0);
    OPT_ARG(exlib::string, 1, "r");
    OPT_ARG(int32_t, 2, 0666);

    if (!cb.IsEmpty())
        hr = acb_open(v0, v1, v2, cb, args);
    else
        hr = ac_open(v0, v1, v2, vr);

    ASYNC_METHOD_RETURN();
}

inline void fs_base::s_static_close(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    ASYNC_METHOD_ENTER("fs.close");

    METHOD_OVER(1, 1);

    ARG(Union_close_fd, 0);

    if (!cb.IsEmpty())
        hr = acb_close(v0, cb, args);
    else
        hr = ac_close(v0);

    ASYNC_METHOD_VOID();
}

inline void fs_base::s_static_openTextStream(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<BufferedStream_base> vr;

    ASYNC_METHOD_ENTER("fs.openTextStream");

    METHOD_OVER(2, 1);

    ARG(exlib::string, 0);
    OPT_ARG(exlib::string, 1, "r");

    if (!cb.IsEmpty())
        hr = acb_openTextStream(v0, v1, cb, args);
    else
        hr = ac_openTextStream(v0, v1, vr);

    ASYNC_METHOD_RETURN();
}

inline void fs_base::s_static_readTextFile(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    ASYNC_METHOD_ENTER("fs.readTextFile");

    METHOD_OVER(1, 1);

    ARG(exlib::string, 0);

    if (!cb.IsEmpty())
        hr = acb_readTextFile(v0, cb, args);
    else
        hr = ac_readTextFile(v0, vr);

    ASYNC_METHOD_RETURN();
}

inline void fs_base::s_static_readFile(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    Variant vr;

    ASYNC_METHOD_ENTER("fs.readFile");

    METHOD_OVER(2, 1);

    ARG(Union_readFile_fname, 0);
    OPT_ARG(Union_readFile_options, 1, exlib::string(""));

    if (!cb.IsEmpty())
        hr = acb_readFile(v0, v1, cb, args);
    else
        hr = ac_readFile(v0, v1, vr);

    ASYNC_METHOD_RETURN();
}

inline void fs_base::s_static_readLines(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    std::vector<exlib::string> vr;

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

    ASYNC_METHOD_ENTER("fs.write");

    METHOD_OVER(5, 2);

    ARG(Union_write_fd, 0);
    ARG(obj_ptr<Buffer_base>, 1);
    OPT_ARG(int32_t, 2, 0);
    OPT_ARG(int32_t, 3, -1);
    OPT_ARG(int32_t, 4, -1);

    if (!cb.IsEmpty())
        hr = acb_write(v0, v1.get(), v2, v3, v4, cb, args);
    else
        hr = ac_write(v0, v1.get(), v2, v3, v4, vr);

    METHOD_OVER(4, 2);

    ARG(Union_write_fd, 0);
    ARG(exlib::string, 1);
    OPT_ARG(int32_t, 2, -1);
    OPT_ARG(exlib::string, 3, "utf8");

    if (!cb.IsEmpty())
        hr = acb_write(v0, v1, v2, v3, cb, args);
    else
        hr = ac_write(v0, v1, v2, v3, vr);

    ASYNC_METHOD_RETURN();
}

inline void fs_base::s_static_writeTextFile(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    int32_t vr;

    ASYNC_METHOD_ENTER("fs.writeTextFile");

    METHOD_OVER(2, 2);

    ARG(exlib::string, 0);
    ARG(exlib::string, 1);

    if (!cb.IsEmpty())
        hr = acb_writeTextFile(v0, v1, cb, args);
    else
        hr = ac_writeTextFile(v0, v1, vr);

    ASYNC_METHOD_RETURN();
}

inline void fs_base::s_static_writeFile(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    int32_t vr;

    ASYNC_METHOD_ENTER("fs.writeFile");

    METHOD_OVER(3, 2);

    ARG(Union_writeFile_fname, 0);
    ARG(Union_writeFile_data, 1);
    OPT_ARG(Union_writeFile_opt, 2, exlib::string("utf8"));

    if (!cb.IsEmpty())
        hr = acb_writeFile(v0, v1, v2, cb, args);
    else
        hr = ac_writeFile(v0, v1, v2, vr);

    ASYNC_METHOD_RETURN();
}

inline void fs_base::s_static_appendFile(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    int32_t vr;

    ASYNC_METHOD_ENTER("fs.appendFile");

    METHOD_OVER(3, 2);

    ARG(Union_appendFile_fname, 0);
    ARG(Union_appendFile_data, 1);
    OPT_ARG(Union_appendFile_options, 2, exlib::string(""));

    if (!cb.IsEmpty())
        hr = acb_appendFile(v0, v1, v2, cb, args);
    else
        hr = ac_appendFile(v0, v1, v2, vr);

    ASYNC_METHOD_RETURN();
}

inline void fs_base::s_static_setZipFS(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_ENTER();

    METHOD_OVER(2, 2);

    ARG(exlib::string, 0);
    ARG(Union_setZipFS_data, 1);

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
