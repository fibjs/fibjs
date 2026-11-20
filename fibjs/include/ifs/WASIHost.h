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

class WASIHostOptions;

class WASIHost_base : public object_base {
    DECLARE_CLASS(WASIHost_base);

public:
    // WASIHost_base
    static result_t _new(WASIHostOptions* options, obj_ptr<WASIHost_base>& retVal, v8::Local<v8::Object> This = v8::Local<v8::Object>());
    virtual result_t getImportObject(v8::Local<v8::Object>& retVal) = 0;
    virtual result_t start(v8::Local<v8::Value> instance, int32_t& retVal) = 0;
    virtual result_t initialize(v8::Local<v8::Value> instance) = 0;
    virtual result_t finalizeBindings(v8::Local<v8::Value> instance, v8::Local<v8::Object> options) = 0;
    virtual result_t registerAsImport() = 0;
    virtual result_t unregisterAsImport() = 0;
    virtual result_t get_wasiImport(v8::Local<v8::Object>& retVal) = 0;
    virtual result_t fd_read(int32_t fd, int32_t iovs, int32_t iovs_len, int32_t nread, int32_t& retVal, AsyncHandle ac) = 0;
    virtual result_t fd_write(int32_t fd, int32_t iovs, int32_t iovs_len, int32_t nwritten, int32_t& retVal, AsyncHandle ac) = 0;
    virtual result_t poll_oneoff(int32_t in_ptr, int32_t out_ptr, int32_t nsubscriptions, int32_t nevents, int32_t& retVal, AsyncHandle ac) = 0;
    virtual result_t args_sizes_get(int32_t argc, int32_t argv_buf_size, int32_t& retVal) = 0;
    virtual result_t args_get(int32_t argv, int32_t argv_buf, int32_t& retVal) = 0;
    virtual result_t environ_sizes_get(int32_t environ_count, int32_t environ_buf_size, int32_t& retVal) = 0;
    virtual result_t environ_get(int32_t environ, int32_t environ_buf, int32_t& retVal) = 0;
    virtual result_t clock_res_get(int32_t clock_id, int32_t resolution, int32_t& retVal) = 0;
    virtual result_t clock_time_get(int32_t clock_id, UInt64 precision, int32_t time, int32_t& retVal) = 0;
    virtual result_t random_get(int32_t buf, int32_t buf_len, int32_t& retVal) = 0;
    virtual result_t sched_yield(int32_t& retVal) = 0;
    virtual result_t proc_exit(int32_t code) = 0;
    virtual result_t proc_raise(int32_t sig, int32_t& retVal) = 0;
    virtual result_t fd_close(int32_t fd, int32_t& retVal, AsyncHandle ac) = 0;
    virtual result_t fd_fdstat_get(int32_t fd, int32_t buf, int32_t& retVal, AsyncHandle ac) = 0;
    virtual result_t fd_filestat_get(int32_t fd, int32_t buf, int32_t& retVal, AsyncHandle ac) = 0;
    virtual result_t fd_prestat_get(int32_t fd, int32_t buf, int32_t& retVal) = 0;
    virtual result_t fd_prestat_dir_name(int32_t fd, int32_t path, int32_t path_len, int32_t& retVal) = 0;
    virtual result_t path_open(int32_t fd, int32_t dirflags, int32_t path, int32_t path_len, int32_t oflags, UInt64 rights_base, UInt64 rights_inheriting, int32_t fdflags, int32_t opened_fd, int32_t& retVal, AsyncHandle ac) = 0;
    virtual result_t path_filestat_get(int32_t fd, int32_t flags, int32_t path, int32_t path_len, int32_t buf, int32_t& retVal, AsyncHandle ac) = 0;
    virtual result_t fd_seek(int32_t fd, Int64 offset, int32_t whence, int32_t newoffset, int32_t& retVal, AsyncHandle ac) = 0;
    virtual result_t fd_tell(int32_t fd, int32_t offset, int32_t& retVal) = 0;
    virtual result_t fd_pread(int32_t fd, int32_t iovs, int32_t iovs_len, Int64 offset, int32_t nread, int32_t& retVal, AsyncHandle ac) = 0;
    virtual result_t fd_pwrite(int32_t fd, int32_t iovs, int32_t iovs_len, Int64 offset, int32_t nwritten, int32_t& retVal, AsyncHandle ac) = 0;
    virtual result_t fd_readdir(int32_t fd, int32_t buf, int32_t buf_len, UInt64 cookie, int32_t bufused, int32_t& retVal, AsyncHandle ac) = 0;
    virtual result_t path_create_directory(int32_t fd, int32_t path, int32_t path_len, int32_t& retVal, AsyncHandle ac) = 0;
    virtual result_t path_remove_directory(int32_t fd, int32_t path, int32_t path_len, int32_t& retVal, AsyncHandle ac) = 0;
    virtual result_t path_unlink_file(int32_t fd, int32_t path, int32_t path_len, int32_t& retVal, AsyncHandle ac) = 0;
    virtual result_t path_rename(int32_t fd, int32_t old_path, int32_t old_path_len, int32_t new_fd, int32_t new_path, int32_t new_path_len, int32_t& retVal, AsyncHandle ac) = 0;
    virtual result_t path_link(int32_t old_fd, int32_t old_flags, int32_t old_path, int32_t old_path_len, int32_t new_fd, int32_t new_path, int32_t new_path_len, int32_t& retVal, AsyncHandle ac) = 0;
    virtual result_t path_readlink(int32_t fd, int32_t path, int32_t path_len, int32_t buf, int32_t buf_len, int32_t bufused, int32_t& retVal, AsyncHandle ac) = 0;
    virtual result_t path_symlink(int32_t old_path, int32_t old_path_len, int32_t fd, int32_t new_path, int32_t new_path_len, int32_t& retVal, AsyncHandle ac) = 0;
    virtual result_t fd_filestat_set_size(int32_t fd, Int64 size, int32_t& retVal, AsyncHandle ac) = 0;
    virtual result_t fd_sync(int32_t fd, int32_t& retVal, AsyncHandle ac) = 0;
    virtual result_t fd_datasync(int32_t fd, int32_t& retVal, AsyncHandle ac) = 0;
    virtual result_t fd_advise(int32_t fd, Int64 offset, Int64 len, int32_t advice, int32_t& retVal) = 0;
    virtual result_t fd_allocate(int32_t fd, Int64 offset, Int64 len, int32_t& retVal) = 0;
    virtual result_t fd_fdstat_set_flags(int32_t fd, int32_t flags, int32_t& retVal) = 0;
    virtual result_t fd_fdstat_set_rights(int32_t fd, UInt64 rights_base, UInt64 rights_inheriting, int32_t& retVal) = 0;
    virtual result_t fd_filestat_set_times(int32_t fd, UInt64 atim, UInt64 mtim, int32_t flags, int32_t& retVal, AsyncHandle ac) = 0;
    virtual result_t path_filestat_set_times(int32_t fd, int32_t flags, int32_t path, int32_t path_len, UInt64 atim, UInt64 mtim, int32_t fst_flags, int32_t& retVal, AsyncHandle ac) = 0;
    virtual result_t fd_renumber(int32_t from, int32_t to, int32_t& retVal, AsyncHandle ac) = 0;
    virtual result_t sock_accept(int32_t fd, int32_t flags, int32_t fd_out, int32_t& retVal, AsyncHandle ac) = 0;
    virtual result_t sock_recv(int32_t fd, int32_t ri_data, int32_t ri_data_len, int32_t ri_flags, int32_t ro_datalen, int32_t ro_flags, int32_t& retVal, AsyncHandle ac) = 0;
    virtual result_t sock_send(int32_t fd, int32_t si_data, int32_t si_data_len, int32_t si_flags, int32_t so_datalen, int32_t& retVal, AsyncHandle ac) = 0;
    virtual result_t sock_shutdown(int32_t fd, int32_t how, int32_t& retVal, AsyncHandle ac) = 0;

public:
    static void __new(const v8::FunctionCallbackInfo<v8::Value>& args);
    static result_t load(v8::Local<v8::Value> v, obj_ptr<WASIHost_base>& retVal);

public:
    static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_getImportObject(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_start(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_initialize(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_finalizeBindings(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_registerAsImport(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_unregisterAsImport(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_wasiImport(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_fd_read(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_fd_write(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_poll_oneoff(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_args_sizes_get(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_args_get(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_environ_sizes_get(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_environ_get(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_clock_res_get(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_clock_time_get(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_random_get(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_sched_yield(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_proc_exit(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_proc_raise(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_fd_close(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_fd_fdstat_get(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_fd_filestat_get(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_fd_prestat_get(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_fd_prestat_dir_name(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_path_open(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_path_filestat_get(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_fd_seek(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_fd_tell(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_fd_pread(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_fd_pwrite(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_fd_readdir(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_path_create_directory(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_path_remove_directory(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_path_unlink_file(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_path_rename(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_path_link(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_path_readlink(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_path_symlink(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_fd_filestat_set_size(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_fd_sync(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_fd_datasync(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_fd_advise(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_fd_allocate(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_fd_fdstat_set_flags(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_fd_fdstat_set_rights(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_fd_filestat_set_times(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_path_filestat_set_times(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_fd_renumber(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_sock_accept(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_sock_recv(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_sock_send(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_sock_shutdown(const v8::FunctionCallbackInfo<v8::Value>& args);

public:
    ASYNC_MEMBERVALUE5(WASIHost_base, fd_read, int32_t, int32_t, int32_t, int32_t, int32_t);
    ASYNC_MEMBERVALUE5(WASIHost_base, fd_write, int32_t, int32_t, int32_t, int32_t, int32_t);
    ASYNC_MEMBERVALUE5(WASIHost_base, poll_oneoff, int32_t, int32_t, int32_t, int32_t, int32_t);
    ASYNC_MEMBERVALUE2(WASIHost_base, fd_close, int32_t, int32_t);
    ASYNC_MEMBERVALUE3(WASIHost_base, fd_fdstat_get, int32_t, int32_t, int32_t);
    ASYNC_MEMBERVALUE3(WASIHost_base, fd_filestat_get, int32_t, int32_t, int32_t);
    ASYNC_MEMBERVALUE10(WASIHost_base, path_open, int32_t, int32_t, int32_t, int32_t, int32_t, UInt64, UInt64, int32_t, int32_t, int32_t);
    ASYNC_MEMBERVALUE6(WASIHost_base, path_filestat_get, int32_t, int32_t, int32_t, int32_t, int32_t, int32_t);
    ASYNC_MEMBERVALUE5(WASIHost_base, fd_seek, int32_t, Int64, int32_t, int32_t, int32_t);
    ASYNC_MEMBERVALUE6(WASIHost_base, fd_pread, int32_t, int32_t, int32_t, Int64, int32_t, int32_t);
    ASYNC_MEMBERVALUE6(WASIHost_base, fd_pwrite, int32_t, int32_t, int32_t, Int64, int32_t, int32_t);
    ASYNC_MEMBERVALUE6(WASIHost_base, fd_readdir, int32_t, int32_t, int32_t, UInt64, int32_t, int32_t);
    ASYNC_MEMBERVALUE4(WASIHost_base, path_create_directory, int32_t, int32_t, int32_t, int32_t);
    ASYNC_MEMBERVALUE4(WASIHost_base, path_remove_directory, int32_t, int32_t, int32_t, int32_t);
    ASYNC_MEMBERVALUE4(WASIHost_base, path_unlink_file, int32_t, int32_t, int32_t, int32_t);
    ASYNC_MEMBERVALUE7(WASIHost_base, path_rename, int32_t, int32_t, int32_t, int32_t, int32_t, int32_t, int32_t);
    ASYNC_MEMBERVALUE8(WASIHost_base, path_link, int32_t, int32_t, int32_t, int32_t, int32_t, int32_t, int32_t, int32_t);
    ASYNC_MEMBERVALUE7(WASIHost_base, path_readlink, int32_t, int32_t, int32_t, int32_t, int32_t, int32_t, int32_t);
    ASYNC_MEMBERVALUE6(WASIHost_base, path_symlink, int32_t, int32_t, int32_t, int32_t, int32_t, int32_t);
    ASYNC_MEMBERVALUE3(WASIHost_base, fd_filestat_set_size, int32_t, Int64, int32_t);
    ASYNC_MEMBERVALUE2(WASIHost_base, fd_sync, int32_t, int32_t);
    ASYNC_MEMBERVALUE2(WASIHost_base, fd_datasync, int32_t, int32_t);
    ASYNC_MEMBERVALUE5(WASIHost_base, fd_filestat_set_times, int32_t, UInt64, UInt64, int32_t, int32_t);
    ASYNC_MEMBERVALUE8(WASIHost_base, path_filestat_set_times, int32_t, int32_t, int32_t, int32_t, UInt64, UInt64, int32_t, int32_t);
    ASYNC_MEMBERVALUE3(WASIHost_base, fd_renumber, int32_t, int32_t, int32_t);
    ASYNC_MEMBERVALUE4(WASIHost_base, sock_accept, int32_t, int32_t, int32_t, int32_t);
    ASYNC_MEMBERVALUE7(WASIHost_base, sock_recv, int32_t, int32_t, int32_t, int32_t, int32_t, int32_t, int32_t);
    ASYNC_MEMBERVALUE6(WASIHost_base, sock_send, int32_t, int32_t, int32_t, int32_t, int32_t, int32_t);
    ASYNC_MEMBERVALUE3(WASIHost_base, sock_shutdown, int32_t, int32_t, int32_t);
};
}

#include "ifs/WASIHostOptions.h"

namespace fibjs {
inline ClassInfo& WASIHost_base::class_info()
{
    static ClassData::ClassMethod s_method[] = {
        { "getImportObject", s_getImportObject, false, ClassData::ASYNC_SYNC },
        { "start", s_start, false, ClassData::ASYNC_SYNC },
        { "initialize", s_initialize, false, ClassData::ASYNC_SYNC },
        { "finalizeBindings", s_finalizeBindings, false, ClassData::ASYNC_SYNC },
        { "registerAsImport", s_registerAsImport, false, ClassData::ASYNC_SYNC },
        { "unregisterAsImport", s_unregisterAsImport, false, ClassData::ASYNC_SYNC },
        { "fd_read", s_fd_read, false, ClassData::ASYNC_ASYNC },
        { "fd_write", s_fd_write, false, ClassData::ASYNC_ASYNC },
        { "poll_oneoff", s_poll_oneoff, false, ClassData::ASYNC_ASYNC },
        { "args_sizes_get", s_args_sizes_get, false, ClassData::ASYNC_SYNC },
        { "args_get", s_args_get, false, ClassData::ASYNC_SYNC },
        { "environ_sizes_get", s_environ_sizes_get, false, ClassData::ASYNC_SYNC },
        { "environ_get", s_environ_get, false, ClassData::ASYNC_SYNC },
        { "clock_res_get", s_clock_res_get, false, ClassData::ASYNC_SYNC },
        { "clock_time_get", s_clock_time_get, false, ClassData::ASYNC_SYNC },
        { "random_get", s_random_get, false, ClassData::ASYNC_SYNC },
        { "sched_yield", s_sched_yield, false, ClassData::ASYNC_SYNC },
        { "proc_exit", s_proc_exit, false, ClassData::ASYNC_SYNC },
        { "proc_raise", s_proc_raise, false, ClassData::ASYNC_SYNC },
        { "fd_close", s_fd_close, false, ClassData::ASYNC_ASYNC },
        { "fd_fdstat_get", s_fd_fdstat_get, false, ClassData::ASYNC_ASYNC },
        { "fd_filestat_get", s_fd_filestat_get, false, ClassData::ASYNC_ASYNC },
        { "fd_prestat_get", s_fd_prestat_get, false, ClassData::ASYNC_SYNC },
        { "fd_prestat_dir_name", s_fd_prestat_dir_name, false, ClassData::ASYNC_SYNC },
        { "path_open", s_path_open, false, ClassData::ASYNC_ASYNC },
        { "path_filestat_get", s_path_filestat_get, false, ClassData::ASYNC_ASYNC },
        { "fd_seek", s_fd_seek, false, ClassData::ASYNC_ASYNC },
        { "fd_tell", s_fd_tell, false, ClassData::ASYNC_SYNC },
        { "fd_pread", s_fd_pread, false, ClassData::ASYNC_ASYNC },
        { "fd_pwrite", s_fd_pwrite, false, ClassData::ASYNC_ASYNC },
        { "fd_readdir", s_fd_readdir, false, ClassData::ASYNC_ASYNC },
        { "path_create_directory", s_path_create_directory, false, ClassData::ASYNC_ASYNC },
        { "path_remove_directory", s_path_remove_directory, false, ClassData::ASYNC_ASYNC },
        { "path_unlink_file", s_path_unlink_file, false, ClassData::ASYNC_ASYNC },
        { "path_rename", s_path_rename, false, ClassData::ASYNC_ASYNC },
        { "path_link", s_path_link, false, ClassData::ASYNC_ASYNC },
        { "path_readlink", s_path_readlink, false, ClassData::ASYNC_ASYNC },
        { "path_symlink", s_path_symlink, false, ClassData::ASYNC_ASYNC },
        { "fd_filestat_set_size", s_fd_filestat_set_size, false, ClassData::ASYNC_ASYNC },
        { "fd_sync", s_fd_sync, false, ClassData::ASYNC_ASYNC },
        { "fd_datasync", s_fd_datasync, false, ClassData::ASYNC_ASYNC },
        { "fd_advise", s_fd_advise, false, ClassData::ASYNC_SYNC },
        { "fd_allocate", s_fd_allocate, false, ClassData::ASYNC_SYNC },
        { "fd_fdstat_set_flags", s_fd_fdstat_set_flags, false, ClassData::ASYNC_SYNC },
        { "fd_fdstat_set_rights", s_fd_fdstat_set_rights, false, ClassData::ASYNC_SYNC },
        { "fd_filestat_set_times", s_fd_filestat_set_times, false, ClassData::ASYNC_ASYNC },
        { "path_filestat_set_times", s_path_filestat_set_times, false, ClassData::ASYNC_ASYNC },
        { "fd_renumber", s_fd_renumber, false, ClassData::ASYNC_ASYNC },
        { "sock_accept", s_sock_accept, false, ClassData::ASYNC_ASYNC },
        { "sock_recv", s_sock_recv, false, ClassData::ASYNC_ASYNC },
        { "sock_send", s_sock_send, false, ClassData::ASYNC_ASYNC },
        { "sock_shutdown", s_sock_shutdown, false, ClassData::ASYNC_ASYNC }
    };

    static ClassData::ClassProperty s_property[] = {
        { "wasiImport", s_get_wasiImport, block_set, false }
    };

    static ClassData s_cd = {
        "WASIHost", false, s__new, NULL,
        ARRAYSIZE(s_method), s_method, 0, NULL, ARRAYSIZE(s_property), s_property, 0, NULL, NULL, NULL,
        &object_base::class_info(),
        true
    };

    static ClassInfo s_ci(s_cd);
    return s_ci;
}

inline void WASIHost_base::s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    CONSTRUCT_INIT();
    __new(args);
}

inline void WASIHost_base::__new(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<WASIHost_base> vr;

    CONSTRUCT_ENTER();

    METHOD_OVER_NAMED(1, 1, "options");

    ARG_NAMED(obj_ptr<WASIHostOptions>, 0, "options");

    hr = _new((WASIHostOptions*)v0, vr, args.This());

    CONSTRUCT_RETURN();
}

inline result_t WASIHost_base::load(v8::Local<v8::Value> v, obj_ptr<WASIHost_base>& retVal)
{
    obj_ptr<WASIHost_base> vr;

    LOAD_ENTER();

    METHOD_OVER_NAMED(1, 1, "options");

    ARG_NAMED(obj_ptr<WASIHostOptions>, 0, "options");

    hr = _new((WASIHostOptions*)v0, vr, args.This());

    LOAD_RETURN();
}

inline void WASIHost_base::s_getImportObject(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Object> vr;

    METHOD_INSTANCE(WASIHost_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->getImportObject(vr);

    METHOD_RETURN();
}

inline void WASIHost_base::s_start(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    int32_t vr;

    METHOD_INSTANCE(WASIHost_base);
    METHOD_ENTER();

    METHOD_OVER_NAMED(1, 1, "instance");

    ARG_NAMED(v8::Local<v8::Value>, 0, "instance");

    hr = pInst->start(v0, vr);

    METHOD_RETURN();
}

inline void WASIHost_base::s_initialize(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(WASIHost_base);
    METHOD_ENTER();

    METHOD_OVER_NAMED(1, 1, "instance");

    ARG_NAMED(v8::Local<v8::Value>, 0, "instance");

    hr = pInst->initialize(v0);

    METHOD_VOID();
}

inline void WASIHost_base::s_finalizeBindings(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(WASIHost_base);
    METHOD_ENTER();

    METHOD_OVER_NAMED(2, 1, "instance");

    ARG_NAMED(v8::Local<v8::Value>, 0, "instance");
    OPT_ARG_NAMED(v8::Local<v8::Object>, 1, v8::Object::New(isolate->m_isolate), "options");

    hr = pInst->finalizeBindings(v0, v1);

    METHOD_VOID();
}

inline void WASIHost_base::s_registerAsImport(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(WASIHost_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->registerAsImport();

    METHOD_VOID();
}

inline void WASIHost_base::s_unregisterAsImport(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(WASIHost_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->unregisterAsImport();

    METHOD_VOID();
}

inline void WASIHost_base::s_get_wasiImport(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Object> vr;

    METHOD_INSTANCE(WASIHost_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->get_wasiImport(vr);

    METHOD_RETURN();
}

inline void WASIHost_base::s_fd_read(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    int32_t vr;

    ASYNC_METHOD_INSTANCE(WASIHost_base);
    ASYNC_METHOD_ENTER("WASIHost.fd_read");

    METHOD_OVER_NAMED(4, 4, "nread");

    ARG_NAMED(int32_t, 0, "fd");
    ARG_NAMED(int32_t, 1, "iovs");
    ARG_NAMED(int32_t, 2, "iovs_len");
    ARG_NAMED(int32_t, 3, "nread");

    if (!cb.IsEmpty())
        hr = pInst->acb_fd_read(v0, v1, v2, v3, cb, args);
    else
        hr = pInst->ac_fd_read(v0, v1, v2, v3, vr);

    ASYNC_METHOD_RETURN();
}

inline void WASIHost_base::s_fd_write(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    int32_t vr;

    ASYNC_METHOD_INSTANCE(WASIHost_base);
    ASYNC_METHOD_ENTER("WASIHost.fd_write");

    METHOD_OVER_NAMED(4, 4, "nwritten");

    ARG_NAMED(int32_t, 0, "fd");
    ARG_NAMED(int32_t, 1, "iovs");
    ARG_NAMED(int32_t, 2, "iovs_len");
    ARG_NAMED(int32_t, 3, "nwritten");

    if (!cb.IsEmpty())
        hr = pInst->acb_fd_write(v0, v1, v2, v3, cb, args);
    else
        hr = pInst->ac_fd_write(v0, v1, v2, v3, vr);

    ASYNC_METHOD_RETURN();
}

inline void WASIHost_base::s_poll_oneoff(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    int32_t vr;

    ASYNC_METHOD_INSTANCE(WASIHost_base);
    ASYNC_METHOD_ENTER("WASIHost.poll_oneoff");

    METHOD_OVER_NAMED(4, 4, "nevents");

    ARG_NAMED(int32_t, 0, "in_ptr");
    ARG_NAMED(int32_t, 1, "out_ptr");
    ARG_NAMED(int32_t, 2, "nsubscriptions");
    ARG_NAMED(int32_t, 3, "nevents");

    if (!cb.IsEmpty())
        hr = pInst->acb_poll_oneoff(v0, v1, v2, v3, cb, args);
    else
        hr = pInst->ac_poll_oneoff(v0, v1, v2, v3, vr);

    ASYNC_METHOD_RETURN();
}

inline void WASIHost_base::s_args_sizes_get(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    int32_t vr;

    METHOD_INSTANCE(WASIHost_base);
    METHOD_ENTER();

    METHOD_OVER_NAMED(2, 2, "argv_buf_size");

    ARG_NAMED(int32_t, 0, "argc");
    ARG_NAMED(int32_t, 1, "argv_buf_size");

    hr = pInst->args_sizes_get(v0, v1, vr);

    METHOD_RETURN();
}

inline void WASIHost_base::s_args_get(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    int32_t vr;

    METHOD_INSTANCE(WASIHost_base);
    METHOD_ENTER();

    METHOD_OVER_NAMED(2, 2, "argv_buf");

    ARG_NAMED(int32_t, 0, "argv");
    ARG_NAMED(int32_t, 1, "argv_buf");

    hr = pInst->args_get(v0, v1, vr);

    METHOD_RETURN();
}

inline void WASIHost_base::s_environ_sizes_get(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    int32_t vr;

    METHOD_INSTANCE(WASIHost_base);
    METHOD_ENTER();

    METHOD_OVER_NAMED(2, 2, "environ_buf_size");

    ARG_NAMED(int32_t, 0, "environ_count");
    ARG_NAMED(int32_t, 1, "environ_buf_size");

    hr = pInst->environ_sizes_get(v0, v1, vr);

    METHOD_RETURN();
}

inline void WASIHost_base::s_environ_get(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    int32_t vr;

    METHOD_INSTANCE(WASIHost_base);
    METHOD_ENTER();

    METHOD_OVER_NAMED(2, 2, "environ_buf");

    ARG_NAMED(int32_t, 0, "environ");
    ARG_NAMED(int32_t, 1, "environ_buf");

    hr = pInst->environ_get(v0, v1, vr);

    METHOD_RETURN();
}

inline void WASIHost_base::s_clock_res_get(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    int32_t vr;

    METHOD_INSTANCE(WASIHost_base);
    METHOD_ENTER();

    METHOD_OVER_NAMED(2, 2, "resolution");

    ARG_NAMED(int32_t, 0, "clock_id");
    ARG_NAMED(int32_t, 1, "resolution");

    hr = pInst->clock_res_get(v0, v1, vr);

    METHOD_RETURN();
}

inline void WASIHost_base::s_clock_time_get(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    int32_t vr;

    METHOD_INSTANCE(WASIHost_base);
    METHOD_ENTER();

    METHOD_OVER_NAMED(3, 3, "time");

    ARG_NAMED(int32_t, 0, "clock_id");
    ARG_NAMED(UInt64, 1, "precision");
    ARG_NAMED(int32_t, 2, "time");

    hr = pInst->clock_time_get(v0, v1, v2, vr);

    METHOD_RETURN();
}

inline void WASIHost_base::s_random_get(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    int32_t vr;

    METHOD_INSTANCE(WASIHost_base);
    METHOD_ENTER();

    METHOD_OVER_NAMED(2, 2, "buf_len");

    ARG_NAMED(int32_t, 0, "buf");
    ARG_NAMED(int32_t, 1, "buf_len");

    hr = pInst->random_get(v0, v1, vr);

    METHOD_RETURN();
}

inline void WASIHost_base::s_sched_yield(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    int32_t vr;

    METHOD_INSTANCE(WASIHost_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->sched_yield(vr);

    METHOD_RETURN();
}

inline void WASIHost_base::s_proc_exit(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(WASIHost_base);
    METHOD_ENTER();

    METHOD_OVER_NAMED(1, 1, "code");

    ARG_NAMED(int32_t, 0, "code");

    hr = pInst->proc_exit(v0);

    METHOD_VOID();
}

inline void WASIHost_base::s_proc_raise(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    int32_t vr;

    METHOD_INSTANCE(WASIHost_base);
    METHOD_ENTER();

    METHOD_OVER_NAMED(1, 1, "sig");

    ARG_NAMED(int32_t, 0, "sig");

    hr = pInst->proc_raise(v0, vr);

    METHOD_RETURN();
}

inline void WASIHost_base::s_fd_close(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    int32_t vr;

    ASYNC_METHOD_INSTANCE(WASIHost_base);
    ASYNC_METHOD_ENTER("WASIHost.fd_close");

    METHOD_OVER_NAMED(1, 1, "fd");

    ARG_NAMED(int32_t, 0, "fd");

    if (!cb.IsEmpty())
        hr = pInst->acb_fd_close(v0, cb, args);
    else
        hr = pInst->ac_fd_close(v0, vr);

    ASYNC_METHOD_RETURN();
}

inline void WASIHost_base::s_fd_fdstat_get(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    int32_t vr;

    ASYNC_METHOD_INSTANCE(WASIHost_base);
    ASYNC_METHOD_ENTER("WASIHost.fd_fdstat_get");

    METHOD_OVER_NAMED(2, 2, "buf");

    ARG_NAMED(int32_t, 0, "fd");
    ARG_NAMED(int32_t, 1, "buf");

    if (!cb.IsEmpty())
        hr = pInst->acb_fd_fdstat_get(v0, v1, cb, args);
    else
        hr = pInst->ac_fd_fdstat_get(v0, v1, vr);

    ASYNC_METHOD_RETURN();
}

inline void WASIHost_base::s_fd_filestat_get(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    int32_t vr;

    ASYNC_METHOD_INSTANCE(WASIHost_base);
    ASYNC_METHOD_ENTER("WASIHost.fd_filestat_get");

    METHOD_OVER_NAMED(2, 2, "buf");

    ARG_NAMED(int32_t, 0, "fd");
    ARG_NAMED(int32_t, 1, "buf");

    if (!cb.IsEmpty())
        hr = pInst->acb_fd_filestat_get(v0, v1, cb, args);
    else
        hr = pInst->ac_fd_filestat_get(v0, v1, vr);

    ASYNC_METHOD_RETURN();
}

inline void WASIHost_base::s_fd_prestat_get(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    int32_t vr;

    METHOD_INSTANCE(WASIHost_base);
    METHOD_ENTER();

    METHOD_OVER_NAMED(2, 2, "buf");

    ARG_NAMED(int32_t, 0, "fd");
    ARG_NAMED(int32_t, 1, "buf");

    hr = pInst->fd_prestat_get(v0, v1, vr);

    METHOD_RETURN();
}

inline void WASIHost_base::s_fd_prestat_dir_name(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    int32_t vr;

    METHOD_INSTANCE(WASIHost_base);
    METHOD_ENTER();

    METHOD_OVER_NAMED(3, 3, "path_len");

    ARG_NAMED(int32_t, 0, "fd");
    ARG_NAMED(int32_t, 1, "path");
    ARG_NAMED(int32_t, 2, "path_len");

    hr = pInst->fd_prestat_dir_name(v0, v1, v2, vr);

    METHOD_RETURN();
}

inline void WASIHost_base::s_path_open(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    int32_t vr;

    ASYNC_METHOD_INSTANCE(WASIHost_base);
    ASYNC_METHOD_ENTER("WASIHost.path_open");

    METHOD_OVER_NAMED(9, 9, "opened_fd");

    ARG_NAMED(int32_t, 0, "fd");
    ARG_NAMED(int32_t, 1, "dirflags");
    ARG_NAMED(int32_t, 2, "path");
    ARG_NAMED(int32_t, 3, "path_len");
    ARG_NAMED(int32_t, 4, "oflags");
    ARG_NAMED(UInt64, 5, "rights_base");
    ARG_NAMED(UInt64, 6, "rights_inheriting");
    ARG_NAMED(int32_t, 7, "fdflags");
    ARG_NAMED(int32_t, 8, "opened_fd");

    if (!cb.IsEmpty())
        hr = pInst->acb_path_open(v0, v1, v2, v3, v4, v5, v6, v7, v8, cb, args);
    else
        hr = pInst->ac_path_open(v0, v1, v2, v3, v4, v5, v6, v7, v8, vr);

    ASYNC_METHOD_RETURN();
}

inline void WASIHost_base::s_path_filestat_get(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    int32_t vr;

    ASYNC_METHOD_INSTANCE(WASIHost_base);
    ASYNC_METHOD_ENTER("WASIHost.path_filestat_get");

    METHOD_OVER_NAMED(5, 5, "buf");

    ARG_NAMED(int32_t, 0, "fd");
    ARG_NAMED(int32_t, 1, "flags");
    ARG_NAMED(int32_t, 2, "path");
    ARG_NAMED(int32_t, 3, "path_len");
    ARG_NAMED(int32_t, 4, "buf");

    if (!cb.IsEmpty())
        hr = pInst->acb_path_filestat_get(v0, v1, v2, v3, v4, cb, args);
    else
        hr = pInst->ac_path_filestat_get(v0, v1, v2, v3, v4, vr);

    ASYNC_METHOD_RETURN();
}

inline void WASIHost_base::s_fd_seek(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    int32_t vr;

    ASYNC_METHOD_INSTANCE(WASIHost_base);
    ASYNC_METHOD_ENTER("WASIHost.fd_seek");

    METHOD_OVER_NAMED(4, 4, "newoffset");

    ARG_NAMED(int32_t, 0, "fd");
    ARG_NAMED(Int64, 1, "offset");
    ARG_NAMED(int32_t, 2, "whence");
    ARG_NAMED(int32_t, 3, "newoffset");

    if (!cb.IsEmpty())
        hr = pInst->acb_fd_seek(v0, v1, v2, v3, cb, args);
    else
        hr = pInst->ac_fd_seek(v0, v1, v2, v3, vr);

    ASYNC_METHOD_RETURN();
}

inline void WASIHost_base::s_fd_tell(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    int32_t vr;

    METHOD_INSTANCE(WASIHost_base);
    METHOD_ENTER();

    METHOD_OVER_NAMED(2, 2, "offset");

    ARG_NAMED(int32_t, 0, "fd");
    ARG_NAMED(int32_t, 1, "offset");

    hr = pInst->fd_tell(v0, v1, vr);

    METHOD_RETURN();
}

inline void WASIHost_base::s_fd_pread(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    int32_t vr;

    ASYNC_METHOD_INSTANCE(WASIHost_base);
    ASYNC_METHOD_ENTER("WASIHost.fd_pread");

    METHOD_OVER_NAMED(5, 5, "nread");

    ARG_NAMED(int32_t, 0, "fd");
    ARG_NAMED(int32_t, 1, "iovs");
    ARG_NAMED(int32_t, 2, "iovs_len");
    ARG_NAMED(Int64, 3, "offset");
    ARG_NAMED(int32_t, 4, "nread");

    if (!cb.IsEmpty())
        hr = pInst->acb_fd_pread(v0, v1, v2, v3, v4, cb, args);
    else
        hr = pInst->ac_fd_pread(v0, v1, v2, v3, v4, vr);

    ASYNC_METHOD_RETURN();
}

inline void WASIHost_base::s_fd_pwrite(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    int32_t vr;

    ASYNC_METHOD_INSTANCE(WASIHost_base);
    ASYNC_METHOD_ENTER("WASIHost.fd_pwrite");

    METHOD_OVER_NAMED(5, 5, "nwritten");

    ARG_NAMED(int32_t, 0, "fd");
    ARG_NAMED(int32_t, 1, "iovs");
    ARG_NAMED(int32_t, 2, "iovs_len");
    ARG_NAMED(Int64, 3, "offset");
    ARG_NAMED(int32_t, 4, "nwritten");

    if (!cb.IsEmpty())
        hr = pInst->acb_fd_pwrite(v0, v1, v2, v3, v4, cb, args);
    else
        hr = pInst->ac_fd_pwrite(v0, v1, v2, v3, v4, vr);

    ASYNC_METHOD_RETURN();
}

inline void WASIHost_base::s_fd_readdir(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    int32_t vr;

    ASYNC_METHOD_INSTANCE(WASIHost_base);
    ASYNC_METHOD_ENTER("WASIHost.fd_readdir");

    METHOD_OVER_NAMED(5, 5, "bufused");

    ARG_NAMED(int32_t, 0, "fd");
    ARG_NAMED(int32_t, 1, "buf");
    ARG_NAMED(int32_t, 2, "buf_len");
    ARG_NAMED(UInt64, 3, "cookie");
    ARG_NAMED(int32_t, 4, "bufused");

    if (!cb.IsEmpty())
        hr = pInst->acb_fd_readdir(v0, v1, v2, v3, v4, cb, args);
    else
        hr = pInst->ac_fd_readdir(v0, v1, v2, v3, v4, vr);

    ASYNC_METHOD_RETURN();
}

inline void WASIHost_base::s_path_create_directory(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    int32_t vr;

    ASYNC_METHOD_INSTANCE(WASIHost_base);
    ASYNC_METHOD_ENTER("WASIHost.path_create_directory");

    METHOD_OVER_NAMED(3, 3, "path_len");

    ARG_NAMED(int32_t, 0, "fd");
    ARG_NAMED(int32_t, 1, "path");
    ARG_NAMED(int32_t, 2, "path_len");

    if (!cb.IsEmpty())
        hr = pInst->acb_path_create_directory(v0, v1, v2, cb, args);
    else
        hr = pInst->ac_path_create_directory(v0, v1, v2, vr);

    ASYNC_METHOD_RETURN();
}

inline void WASIHost_base::s_path_remove_directory(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    int32_t vr;

    ASYNC_METHOD_INSTANCE(WASIHost_base);
    ASYNC_METHOD_ENTER("WASIHost.path_remove_directory");

    METHOD_OVER_NAMED(3, 3, "path_len");

    ARG_NAMED(int32_t, 0, "fd");
    ARG_NAMED(int32_t, 1, "path");
    ARG_NAMED(int32_t, 2, "path_len");

    if (!cb.IsEmpty())
        hr = pInst->acb_path_remove_directory(v0, v1, v2, cb, args);
    else
        hr = pInst->ac_path_remove_directory(v0, v1, v2, vr);

    ASYNC_METHOD_RETURN();
}

inline void WASIHost_base::s_path_unlink_file(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    int32_t vr;

    ASYNC_METHOD_INSTANCE(WASIHost_base);
    ASYNC_METHOD_ENTER("WASIHost.path_unlink_file");

    METHOD_OVER_NAMED(3, 3, "path_len");

    ARG_NAMED(int32_t, 0, "fd");
    ARG_NAMED(int32_t, 1, "path");
    ARG_NAMED(int32_t, 2, "path_len");

    if (!cb.IsEmpty())
        hr = pInst->acb_path_unlink_file(v0, v1, v2, cb, args);
    else
        hr = pInst->ac_path_unlink_file(v0, v1, v2, vr);

    ASYNC_METHOD_RETURN();
}

inline void WASIHost_base::s_path_rename(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    int32_t vr;

    ASYNC_METHOD_INSTANCE(WASIHost_base);
    ASYNC_METHOD_ENTER("WASIHost.path_rename");

    METHOD_OVER_NAMED(6, 6, "new_path_len");

    ARG_NAMED(int32_t, 0, "fd");
    ARG_NAMED(int32_t, 1, "old_path");
    ARG_NAMED(int32_t, 2, "old_path_len");
    ARG_NAMED(int32_t, 3, "new_fd");
    ARG_NAMED(int32_t, 4, "new_path");
    ARG_NAMED(int32_t, 5, "new_path_len");

    if (!cb.IsEmpty())
        hr = pInst->acb_path_rename(v0, v1, v2, v3, v4, v5, cb, args);
    else
        hr = pInst->ac_path_rename(v0, v1, v2, v3, v4, v5, vr);

    ASYNC_METHOD_RETURN();
}

inline void WASIHost_base::s_path_link(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    int32_t vr;

    ASYNC_METHOD_INSTANCE(WASIHost_base);
    ASYNC_METHOD_ENTER("WASIHost.path_link");

    METHOD_OVER_NAMED(7, 7, "new_path_len");

    ARG_NAMED(int32_t, 0, "old_fd");
    ARG_NAMED(int32_t, 1, "old_flags");
    ARG_NAMED(int32_t, 2, "old_path");
    ARG_NAMED(int32_t, 3, "old_path_len");
    ARG_NAMED(int32_t, 4, "new_fd");
    ARG_NAMED(int32_t, 5, "new_path");
    ARG_NAMED(int32_t, 6, "new_path_len");

    if (!cb.IsEmpty())
        hr = pInst->acb_path_link(v0, v1, v2, v3, v4, v5, v6, cb, args);
    else
        hr = pInst->ac_path_link(v0, v1, v2, v3, v4, v5, v6, vr);

    ASYNC_METHOD_RETURN();
}

inline void WASIHost_base::s_path_readlink(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    int32_t vr;

    ASYNC_METHOD_INSTANCE(WASIHost_base);
    ASYNC_METHOD_ENTER("WASIHost.path_readlink");

    METHOD_OVER_NAMED(6, 6, "bufused");

    ARG_NAMED(int32_t, 0, "fd");
    ARG_NAMED(int32_t, 1, "path");
    ARG_NAMED(int32_t, 2, "path_len");
    ARG_NAMED(int32_t, 3, "buf");
    ARG_NAMED(int32_t, 4, "buf_len");
    ARG_NAMED(int32_t, 5, "bufused");

    if (!cb.IsEmpty())
        hr = pInst->acb_path_readlink(v0, v1, v2, v3, v4, v5, cb, args);
    else
        hr = pInst->ac_path_readlink(v0, v1, v2, v3, v4, v5, vr);

    ASYNC_METHOD_RETURN();
}

inline void WASIHost_base::s_path_symlink(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    int32_t vr;

    ASYNC_METHOD_INSTANCE(WASIHost_base);
    ASYNC_METHOD_ENTER("WASIHost.path_symlink");

    METHOD_OVER_NAMED(5, 5, "new_path_len");

    ARG_NAMED(int32_t, 0, "old_path");
    ARG_NAMED(int32_t, 1, "old_path_len");
    ARG_NAMED(int32_t, 2, "fd");
    ARG_NAMED(int32_t, 3, "new_path");
    ARG_NAMED(int32_t, 4, "new_path_len");

    if (!cb.IsEmpty())
        hr = pInst->acb_path_symlink(v0, v1, v2, v3, v4, cb, args);
    else
        hr = pInst->ac_path_symlink(v0, v1, v2, v3, v4, vr);

    ASYNC_METHOD_RETURN();
}

inline void WASIHost_base::s_fd_filestat_set_size(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    int32_t vr;

    ASYNC_METHOD_INSTANCE(WASIHost_base);
    ASYNC_METHOD_ENTER("WASIHost.fd_filestat_set_size");

    METHOD_OVER_NAMED(2, 2, "size");

    ARG_NAMED(int32_t, 0, "fd");
    ARG_NAMED(Int64, 1, "size");

    if (!cb.IsEmpty())
        hr = pInst->acb_fd_filestat_set_size(v0, v1, cb, args);
    else
        hr = pInst->ac_fd_filestat_set_size(v0, v1, vr);

    ASYNC_METHOD_RETURN();
}

inline void WASIHost_base::s_fd_sync(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    int32_t vr;

    ASYNC_METHOD_INSTANCE(WASIHost_base);
    ASYNC_METHOD_ENTER("WASIHost.fd_sync");

    METHOD_OVER_NAMED(1, 1, "fd");

    ARG_NAMED(int32_t, 0, "fd");

    if (!cb.IsEmpty())
        hr = pInst->acb_fd_sync(v0, cb, args);
    else
        hr = pInst->ac_fd_sync(v0, vr);

    ASYNC_METHOD_RETURN();
}

inline void WASIHost_base::s_fd_datasync(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    int32_t vr;

    ASYNC_METHOD_INSTANCE(WASIHost_base);
    ASYNC_METHOD_ENTER("WASIHost.fd_datasync");

    METHOD_OVER_NAMED(1, 1, "fd");

    ARG_NAMED(int32_t, 0, "fd");

    if (!cb.IsEmpty())
        hr = pInst->acb_fd_datasync(v0, cb, args);
    else
        hr = pInst->ac_fd_datasync(v0, vr);

    ASYNC_METHOD_RETURN();
}

inline void WASIHost_base::s_fd_advise(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    int32_t vr;

    METHOD_INSTANCE(WASIHost_base);
    METHOD_ENTER();

    METHOD_OVER_NAMED(4, 4, "advice");

    ARG_NAMED(int32_t, 0, "fd");
    ARG_NAMED(Int64, 1, "offset");
    ARG_NAMED(Int64, 2, "len");
    ARG_NAMED(int32_t, 3, "advice");

    hr = pInst->fd_advise(v0, v1, v2, v3, vr);

    METHOD_RETURN();
}

inline void WASIHost_base::s_fd_allocate(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    int32_t vr;

    METHOD_INSTANCE(WASIHost_base);
    METHOD_ENTER();

    METHOD_OVER_NAMED(3, 3, "len");

    ARG_NAMED(int32_t, 0, "fd");
    ARG_NAMED(Int64, 1, "offset");
    ARG_NAMED(Int64, 2, "len");

    hr = pInst->fd_allocate(v0, v1, v2, vr);

    METHOD_RETURN();
}

inline void WASIHost_base::s_fd_fdstat_set_flags(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    int32_t vr;

    METHOD_INSTANCE(WASIHost_base);
    METHOD_ENTER();

    METHOD_OVER_NAMED(2, 2, "flags");

    ARG_NAMED(int32_t, 0, "fd");
    ARG_NAMED(int32_t, 1, "flags");

    hr = pInst->fd_fdstat_set_flags(v0, v1, vr);

    METHOD_RETURN();
}

inline void WASIHost_base::s_fd_fdstat_set_rights(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    int32_t vr;

    METHOD_INSTANCE(WASIHost_base);
    METHOD_ENTER();

    METHOD_OVER_NAMED(3, 3, "rights_inheriting");

    ARG_NAMED(int32_t, 0, "fd");
    ARG_NAMED(UInt64, 1, "rights_base");
    ARG_NAMED(UInt64, 2, "rights_inheriting");

    hr = pInst->fd_fdstat_set_rights(v0, v1, v2, vr);

    METHOD_RETURN();
}

inline void WASIHost_base::s_fd_filestat_set_times(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    int32_t vr;

    ASYNC_METHOD_INSTANCE(WASIHost_base);
    ASYNC_METHOD_ENTER("WASIHost.fd_filestat_set_times");

    METHOD_OVER_NAMED(4, 4, "flags");

    ARG_NAMED(int32_t, 0, "fd");
    ARG_NAMED(UInt64, 1, "atim");
    ARG_NAMED(UInt64, 2, "mtim");
    ARG_NAMED(int32_t, 3, "flags");

    if (!cb.IsEmpty())
        hr = pInst->acb_fd_filestat_set_times(v0, v1, v2, v3, cb, args);
    else
        hr = pInst->ac_fd_filestat_set_times(v0, v1, v2, v3, vr);

    ASYNC_METHOD_RETURN();
}

inline void WASIHost_base::s_path_filestat_set_times(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    int32_t vr;

    ASYNC_METHOD_INSTANCE(WASIHost_base);
    ASYNC_METHOD_ENTER("WASIHost.path_filestat_set_times");

    METHOD_OVER_NAMED(7, 7, "fst_flags");

    ARG_NAMED(int32_t, 0, "fd");
    ARG_NAMED(int32_t, 1, "flags");
    ARG_NAMED(int32_t, 2, "path");
    ARG_NAMED(int32_t, 3, "path_len");
    ARG_NAMED(UInt64, 4, "atim");
    ARG_NAMED(UInt64, 5, "mtim");
    ARG_NAMED(int32_t, 6, "fst_flags");

    if (!cb.IsEmpty())
        hr = pInst->acb_path_filestat_set_times(v0, v1, v2, v3, v4, v5, v6, cb, args);
    else
        hr = pInst->ac_path_filestat_set_times(v0, v1, v2, v3, v4, v5, v6, vr);

    ASYNC_METHOD_RETURN();
}

inline void WASIHost_base::s_fd_renumber(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    int32_t vr;

    ASYNC_METHOD_INSTANCE(WASIHost_base);
    ASYNC_METHOD_ENTER("WASIHost.fd_renumber");

    METHOD_OVER_NAMED(2, 2, "to");

    ARG_NAMED(int32_t, 0, "from");
    ARG_NAMED(int32_t, 1, "to");

    if (!cb.IsEmpty())
        hr = pInst->acb_fd_renumber(v0, v1, cb, args);
    else
        hr = pInst->ac_fd_renumber(v0, v1, vr);

    ASYNC_METHOD_RETURN();
}

inline void WASIHost_base::s_sock_accept(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    int32_t vr;

    ASYNC_METHOD_INSTANCE(WASIHost_base);
    ASYNC_METHOD_ENTER("WASIHost.sock_accept");

    METHOD_OVER_NAMED(3, 3, "fd_out");

    ARG_NAMED(int32_t, 0, "fd");
    ARG_NAMED(int32_t, 1, "flags");
    ARG_NAMED(int32_t, 2, "fd_out");

    if (!cb.IsEmpty())
        hr = pInst->acb_sock_accept(v0, v1, v2, cb, args);
    else
        hr = pInst->ac_sock_accept(v0, v1, v2, vr);

    ASYNC_METHOD_RETURN();
}

inline void WASIHost_base::s_sock_recv(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    int32_t vr;

    ASYNC_METHOD_INSTANCE(WASIHost_base);
    ASYNC_METHOD_ENTER("WASIHost.sock_recv");

    METHOD_OVER_NAMED(6, 6, "ro_flags");

    ARG_NAMED(int32_t, 0, "fd");
    ARG_NAMED(int32_t, 1, "ri_data");
    ARG_NAMED(int32_t, 2, "ri_data_len");
    ARG_NAMED(int32_t, 3, "ri_flags");
    ARG_NAMED(int32_t, 4, "ro_datalen");
    ARG_NAMED(int32_t, 5, "ro_flags");

    if (!cb.IsEmpty())
        hr = pInst->acb_sock_recv(v0, v1, v2, v3, v4, v5, cb, args);
    else
        hr = pInst->ac_sock_recv(v0, v1, v2, v3, v4, v5, vr);

    ASYNC_METHOD_RETURN();
}

inline void WASIHost_base::s_sock_send(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    int32_t vr;

    ASYNC_METHOD_INSTANCE(WASIHost_base);
    ASYNC_METHOD_ENTER("WASIHost.sock_send");

    METHOD_OVER_NAMED(5, 5, "so_datalen");

    ARG_NAMED(int32_t, 0, "fd");
    ARG_NAMED(int32_t, 1, "si_data");
    ARG_NAMED(int32_t, 2, "si_data_len");
    ARG_NAMED(int32_t, 3, "si_flags");
    ARG_NAMED(int32_t, 4, "so_datalen");

    if (!cb.IsEmpty())
        hr = pInst->acb_sock_send(v0, v1, v2, v3, v4, cb, args);
    else
        hr = pInst->ac_sock_send(v0, v1, v2, v3, v4, vr);

    ASYNC_METHOD_RETURN();
}

inline void WASIHost_base::s_sock_shutdown(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    int32_t vr;

    ASYNC_METHOD_INSTANCE(WASIHost_base);
    ASYNC_METHOD_ENTER("WASIHost.sock_shutdown");

    METHOD_OVER_NAMED(2, 2, "how");

    ARG_NAMED(int32_t, 0, "fd");
    ARG_NAMED(int32_t, 1, "how");

    if (!cb.IsEmpty())
        hr = pInst->acb_sock_shutdown(v0, v1, cb, args);
    else
        hr = pInst->ac_sock_shutdown(v0, v1, vr);

    ASYNC_METHOD_RETURN();
}
}
