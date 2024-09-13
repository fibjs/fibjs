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

class Stat_base : public object_base {
    DECLARE_CLASS(Stat_base);

public:
    // Stat_base
    virtual result_t get_name(exlib::string& retVal) = 0;
    virtual result_t get_dev(int32_t& retVal) = 0;
    virtual result_t get_ino(int32_t& retVal) = 0;
    virtual result_t get_mode(int32_t& retVal) = 0;
    virtual result_t get_nlink(int32_t& retVal) = 0;
    virtual result_t get_uid(int32_t& retVal) = 0;
    virtual result_t get_gid(int32_t& retVal) = 0;
    virtual result_t get_rdev(int32_t& retVal) = 0;
    virtual result_t get_size(double& retVal) = 0;
    virtual result_t get_blksize(int32_t& retVal) = 0;
    virtual result_t get_blocks(int32_t& retVal) = 0;
    virtual result_t get_mtime(date_t& retVal) = 0;
    virtual result_t get_mtimeMs(double& retVal) = 0;
    virtual result_t get_atime(date_t& retVal) = 0;
    virtual result_t get_atimeMs(double& retVal) = 0;
    virtual result_t get_ctime(date_t& retVal) = 0;
    virtual result_t get_ctimeMs(double& retVal) = 0;
    virtual result_t get_birthtime(date_t& retVal) = 0;
    virtual result_t get_birthtimeMs(double& retVal) = 0;
    virtual result_t isWritable(bool& retVal) = 0;
    virtual result_t isReadable(bool& retVal) = 0;
    virtual result_t isExecutable(bool& retVal) = 0;
    virtual result_t isHidden(bool& retVal) = 0;
    virtual result_t isBlockDevice(bool& retVal) = 0;
    virtual result_t isCharacterDevice(bool& retVal) = 0;
    virtual result_t isDirectory(bool& retVal) = 0;
    virtual result_t isFIFO(bool& retVal) = 0;
    virtual result_t isFile(bool& retVal) = 0;
    virtual result_t isSymbolicLink(bool& retVal) = 0;
    virtual result_t isMemory(bool& retVal) = 0;
    virtual result_t isSocket(bool& retVal) = 0;

public:
    static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        CONSTRUCT_INIT();

        isolate->m_isolate->ThrowException(
            isolate->NewString("not a constructor"));
    }

public:
    static void s_get_name(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_dev(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_ino(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_mode(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_nlink(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_uid(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_gid(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_rdev(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_size(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_blksize(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_blocks(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_mtime(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_mtimeMs(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_atime(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_atimeMs(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_ctime(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_ctimeMs(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_birthtime(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_birthtimeMs(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_isWritable(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_isReadable(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_isExecutable(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_isHidden(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_isBlockDevice(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_isCharacterDevice(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_isDirectory(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_isFIFO(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_isFile(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_isSymbolicLink(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_isMemory(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_isSocket(const v8::FunctionCallbackInfo<v8::Value>& args);
};
}

namespace fibjs {
inline ClassInfo& Stat_base::class_info()
{
    static ClassData::ClassMethod s_method[] = {
        { "isWritable", s_isWritable, false, ClassData::ASYNC_SYNC },
        { "isReadable", s_isReadable, false, ClassData::ASYNC_SYNC },
        { "isExecutable", s_isExecutable, false, ClassData::ASYNC_SYNC },
        { "isHidden", s_isHidden, false, ClassData::ASYNC_SYNC },
        { "isBlockDevice", s_isBlockDevice, false, ClassData::ASYNC_SYNC },
        { "isCharacterDevice", s_isCharacterDevice, false, ClassData::ASYNC_SYNC },
        { "isDirectory", s_isDirectory, false, ClassData::ASYNC_SYNC },
        { "isFIFO", s_isFIFO, false, ClassData::ASYNC_SYNC },
        { "isFile", s_isFile, false, ClassData::ASYNC_SYNC },
        { "isSymbolicLink", s_isSymbolicLink, false, ClassData::ASYNC_SYNC },
        { "isMemory", s_isMemory, false, ClassData::ASYNC_SYNC },
        { "isSocket", s_isSocket, false, ClassData::ASYNC_SYNC }
    };

    static ClassData::ClassProperty s_property[] = {
        { "name", s_get_name, block_set, false },
        { "dev", s_get_dev, block_set, false },
        { "ino", s_get_ino, block_set, false },
        { "mode", s_get_mode, block_set, false },
        { "nlink", s_get_nlink, block_set, false },
        { "uid", s_get_uid, block_set, false },
        { "gid", s_get_gid, block_set, false },
        { "rdev", s_get_rdev, block_set, false },
        { "size", s_get_size, block_set, false },
        { "blksize", s_get_blksize, block_set, false },
        { "blocks", s_get_blocks, block_set, false },
        { "mtime", s_get_mtime, block_set, false },
        { "mtimeMs", s_get_mtimeMs, block_set, false },
        { "atime", s_get_atime, block_set, false },
        { "atimeMs", s_get_atimeMs, block_set, false },
        { "ctime", s_get_ctime, block_set, false },
        { "ctimeMs", s_get_ctimeMs, block_set, false },
        { "birthtime", s_get_birthtime, block_set, false },
        { "birthtimeMs", s_get_birthtimeMs, block_set, false }
    };

    static ClassData s_cd = {
        "Stat", false, s__new, NULL,
        ARRAYSIZE(s_method), s_method, 0, NULL, ARRAYSIZE(s_property), s_property, 0, NULL, NULL, NULL,
        &object_base::class_info(),
        false
    };

    static ClassInfo s_ci(s_cd);
    return s_ci;
}

inline void Stat_base::s_get_name(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_INSTANCE(Stat_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->get_name(vr);

    METHOD_RETURN();
}

inline void Stat_base::s_get_dev(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    int32_t vr;

    METHOD_INSTANCE(Stat_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->get_dev(vr);

    METHOD_RETURN();
}

inline void Stat_base::s_get_ino(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    int32_t vr;

    METHOD_INSTANCE(Stat_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->get_ino(vr);

    METHOD_RETURN();
}

inline void Stat_base::s_get_mode(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    int32_t vr;

    METHOD_INSTANCE(Stat_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->get_mode(vr);

    METHOD_RETURN();
}

inline void Stat_base::s_get_nlink(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    int32_t vr;

    METHOD_INSTANCE(Stat_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->get_nlink(vr);

    METHOD_RETURN();
}

inline void Stat_base::s_get_uid(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    int32_t vr;

    METHOD_INSTANCE(Stat_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->get_uid(vr);

    METHOD_RETURN();
}

inline void Stat_base::s_get_gid(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    int32_t vr;

    METHOD_INSTANCE(Stat_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->get_gid(vr);

    METHOD_RETURN();
}

inline void Stat_base::s_get_rdev(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    int32_t vr;

    METHOD_INSTANCE(Stat_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->get_rdev(vr);

    METHOD_RETURN();
}

inline void Stat_base::s_get_size(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    double vr;

    METHOD_INSTANCE(Stat_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->get_size(vr);

    METHOD_RETURN();
}

inline void Stat_base::s_get_blksize(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    int32_t vr;

    METHOD_INSTANCE(Stat_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->get_blksize(vr);

    METHOD_RETURN();
}

inline void Stat_base::s_get_blocks(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    int32_t vr;

    METHOD_INSTANCE(Stat_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->get_blocks(vr);

    METHOD_RETURN();
}

inline void Stat_base::s_get_mtime(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    date_t vr;

    METHOD_INSTANCE(Stat_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->get_mtime(vr);

    METHOD_RETURN();
}

inline void Stat_base::s_get_mtimeMs(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    double vr;

    METHOD_INSTANCE(Stat_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->get_mtimeMs(vr);

    METHOD_RETURN();
}

inline void Stat_base::s_get_atime(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    date_t vr;

    METHOD_INSTANCE(Stat_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->get_atime(vr);

    METHOD_RETURN();
}

inline void Stat_base::s_get_atimeMs(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    double vr;

    METHOD_INSTANCE(Stat_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->get_atimeMs(vr);

    METHOD_RETURN();
}

inline void Stat_base::s_get_ctime(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    date_t vr;

    METHOD_INSTANCE(Stat_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->get_ctime(vr);

    METHOD_RETURN();
}

inline void Stat_base::s_get_ctimeMs(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    double vr;

    METHOD_INSTANCE(Stat_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->get_ctimeMs(vr);

    METHOD_RETURN();
}

inline void Stat_base::s_get_birthtime(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    date_t vr;

    METHOD_INSTANCE(Stat_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->get_birthtime(vr);

    METHOD_RETURN();
}

inline void Stat_base::s_get_birthtimeMs(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    double vr;

    METHOD_INSTANCE(Stat_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->get_birthtimeMs(vr);

    METHOD_RETURN();
}

inline void Stat_base::s_isWritable(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    bool vr;

    METHOD_INSTANCE(Stat_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->isWritable(vr);

    METHOD_RETURN();
}

inline void Stat_base::s_isReadable(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    bool vr;

    METHOD_INSTANCE(Stat_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->isReadable(vr);

    METHOD_RETURN();
}

inline void Stat_base::s_isExecutable(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    bool vr;

    METHOD_INSTANCE(Stat_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->isExecutable(vr);

    METHOD_RETURN();
}

inline void Stat_base::s_isHidden(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    bool vr;

    METHOD_INSTANCE(Stat_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->isHidden(vr);

    METHOD_RETURN();
}

inline void Stat_base::s_isBlockDevice(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    bool vr;

    METHOD_INSTANCE(Stat_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->isBlockDevice(vr);

    METHOD_RETURN();
}

inline void Stat_base::s_isCharacterDevice(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    bool vr;

    METHOD_INSTANCE(Stat_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->isCharacterDevice(vr);

    METHOD_RETURN();
}

inline void Stat_base::s_isDirectory(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    bool vr;

    METHOD_INSTANCE(Stat_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->isDirectory(vr);

    METHOD_RETURN();
}

inline void Stat_base::s_isFIFO(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    bool vr;

    METHOD_INSTANCE(Stat_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->isFIFO(vr);

    METHOD_RETURN();
}

inline void Stat_base::s_isFile(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    bool vr;

    METHOD_INSTANCE(Stat_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->isFile(vr);

    METHOD_RETURN();
}

inline void Stat_base::s_isSymbolicLink(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    bool vr;

    METHOD_INSTANCE(Stat_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->isSymbolicLink(vr);

    METHOD_RETURN();
}

inline void Stat_base::s_isMemory(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    bool vr;

    METHOD_INSTANCE(Stat_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->isMemory(vr);

    METHOD_RETURN();
}

inline void Stat_base::s_isSocket(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    bool vr;

    METHOD_INSTANCE(Stat_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->isSocket(vr);

    METHOD_RETURN();
}
}
