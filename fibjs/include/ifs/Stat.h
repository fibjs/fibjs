/***************************************************************************
 *                                                                         *
 *   This file was automatically generated using idlc.js                   *
 *   PLEASE DO NOT EDIT!!!!                                                *
 *                                                                         *
 ***************************************************************************/

#ifndef _Stat_base_H_
#define _Stat_base_H_

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

        Isolate* isolate = Isolate::current();

        isolate->m_isolate->ThrowException(
            isolate->NewString("not a constructor"));
    }

public:
    static void s_get_name(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_get_dev(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_get_ino(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_get_mode(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_get_nlink(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_get_uid(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_get_gid(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_get_rdev(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_get_size(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_get_blksize(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_get_blocks(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_get_mtime(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_get_mtimeMs(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_get_atime(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_get_atimeMs(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_get_ctime(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_get_ctimeMs(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_get_birthtime(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_get_birthtimeMs(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args);
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
        { "isWritable", s_isWritable, false },
        { "isReadable", s_isReadable, false },
        { "isExecutable", s_isExecutable, false },
        { "isHidden", s_isHidden, false },
        { "isBlockDevice", s_isBlockDevice, false },
        { "isCharacterDevice", s_isCharacterDevice, false },
        { "isDirectory", s_isDirectory, false },
        { "isFIFO", s_isFIFO, false },
        { "isFile", s_isFile, false },
        { "isSymbolicLink", s_isSymbolicLink, false },
        { "isMemory", s_isMemory, false },
        { "isSocket", s_isSocket, false }
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
        &object_base::class_info()
    };

    static ClassInfo s_ci(s_cd);
    return s_ci;
}

inline void Stat_base::s_get_name(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_NAME("Stat.name");
    METHOD_INSTANCE(Stat_base);
    PROPERTY_ENTER();

    hr = pInst->get_name(vr);

    METHOD_RETURN();
}

inline void Stat_base::s_get_dev(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    int32_t vr;

    METHOD_NAME("Stat.dev");
    METHOD_INSTANCE(Stat_base);
    PROPERTY_ENTER();

    hr = pInst->get_dev(vr);

    METHOD_RETURN();
}

inline void Stat_base::s_get_ino(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    int32_t vr;

    METHOD_NAME("Stat.ino");
    METHOD_INSTANCE(Stat_base);
    PROPERTY_ENTER();

    hr = pInst->get_ino(vr);

    METHOD_RETURN();
}

inline void Stat_base::s_get_mode(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    int32_t vr;

    METHOD_NAME("Stat.mode");
    METHOD_INSTANCE(Stat_base);
    PROPERTY_ENTER();

    hr = pInst->get_mode(vr);

    METHOD_RETURN();
}

inline void Stat_base::s_get_nlink(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    int32_t vr;

    METHOD_NAME("Stat.nlink");
    METHOD_INSTANCE(Stat_base);
    PROPERTY_ENTER();

    hr = pInst->get_nlink(vr);

    METHOD_RETURN();
}

inline void Stat_base::s_get_uid(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    int32_t vr;

    METHOD_NAME("Stat.uid");
    METHOD_INSTANCE(Stat_base);
    PROPERTY_ENTER();

    hr = pInst->get_uid(vr);

    METHOD_RETURN();
}

inline void Stat_base::s_get_gid(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    int32_t vr;

    METHOD_NAME("Stat.gid");
    METHOD_INSTANCE(Stat_base);
    PROPERTY_ENTER();

    hr = pInst->get_gid(vr);

    METHOD_RETURN();
}

inline void Stat_base::s_get_rdev(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    int32_t vr;

    METHOD_NAME("Stat.rdev");
    METHOD_INSTANCE(Stat_base);
    PROPERTY_ENTER();

    hr = pInst->get_rdev(vr);

    METHOD_RETURN();
}

inline void Stat_base::s_get_size(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    double vr;

    METHOD_NAME("Stat.size");
    METHOD_INSTANCE(Stat_base);
    PROPERTY_ENTER();

    hr = pInst->get_size(vr);

    METHOD_RETURN();
}

inline void Stat_base::s_get_blksize(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    int32_t vr;

    METHOD_NAME("Stat.blksize");
    METHOD_INSTANCE(Stat_base);
    PROPERTY_ENTER();

    hr = pInst->get_blksize(vr);

    METHOD_RETURN();
}

inline void Stat_base::s_get_blocks(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    int32_t vr;

    METHOD_NAME("Stat.blocks");
    METHOD_INSTANCE(Stat_base);
    PROPERTY_ENTER();

    hr = pInst->get_blocks(vr);

    METHOD_RETURN();
}

inline void Stat_base::s_get_mtime(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    date_t vr;

    METHOD_NAME("Stat.mtime");
    METHOD_INSTANCE(Stat_base);
    PROPERTY_ENTER();

    hr = pInst->get_mtime(vr);

    METHOD_RETURN();
}

inline void Stat_base::s_get_mtimeMs(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    double vr;

    METHOD_NAME("Stat.mtimeMs");
    METHOD_INSTANCE(Stat_base);
    PROPERTY_ENTER();

    hr = pInst->get_mtimeMs(vr);

    METHOD_RETURN();
}

inline void Stat_base::s_get_atime(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    date_t vr;

    METHOD_NAME("Stat.atime");
    METHOD_INSTANCE(Stat_base);
    PROPERTY_ENTER();

    hr = pInst->get_atime(vr);

    METHOD_RETURN();
}

inline void Stat_base::s_get_atimeMs(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    double vr;

    METHOD_NAME("Stat.atimeMs");
    METHOD_INSTANCE(Stat_base);
    PROPERTY_ENTER();

    hr = pInst->get_atimeMs(vr);

    METHOD_RETURN();
}

inline void Stat_base::s_get_ctime(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    date_t vr;

    METHOD_NAME("Stat.ctime");
    METHOD_INSTANCE(Stat_base);
    PROPERTY_ENTER();

    hr = pInst->get_ctime(vr);

    METHOD_RETURN();
}

inline void Stat_base::s_get_ctimeMs(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    double vr;

    METHOD_NAME("Stat.ctimeMs");
    METHOD_INSTANCE(Stat_base);
    PROPERTY_ENTER();

    hr = pInst->get_ctimeMs(vr);

    METHOD_RETURN();
}

inline void Stat_base::s_get_birthtime(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    date_t vr;

    METHOD_NAME("Stat.birthtime");
    METHOD_INSTANCE(Stat_base);
    PROPERTY_ENTER();

    hr = pInst->get_birthtime(vr);

    METHOD_RETURN();
}

inline void Stat_base::s_get_birthtimeMs(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    double vr;

    METHOD_NAME("Stat.birthtimeMs");
    METHOD_INSTANCE(Stat_base);
    PROPERTY_ENTER();

    hr = pInst->get_birthtimeMs(vr);

    METHOD_RETURN();
}

inline void Stat_base::s_isWritable(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    bool vr;

    METHOD_NAME("Stat.isWritable");
    METHOD_INSTANCE(Stat_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->isWritable(vr);

    METHOD_RETURN();
}

inline void Stat_base::s_isReadable(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    bool vr;

    METHOD_NAME("Stat.isReadable");
    METHOD_INSTANCE(Stat_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->isReadable(vr);

    METHOD_RETURN();
}

inline void Stat_base::s_isExecutable(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    bool vr;

    METHOD_NAME("Stat.isExecutable");
    METHOD_INSTANCE(Stat_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->isExecutable(vr);

    METHOD_RETURN();
}

inline void Stat_base::s_isHidden(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    bool vr;

    METHOD_NAME("Stat.isHidden");
    METHOD_INSTANCE(Stat_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->isHidden(vr);

    METHOD_RETURN();
}

inline void Stat_base::s_isBlockDevice(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    bool vr;

    METHOD_NAME("Stat.isBlockDevice");
    METHOD_INSTANCE(Stat_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->isBlockDevice(vr);

    METHOD_RETURN();
}

inline void Stat_base::s_isCharacterDevice(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    bool vr;

    METHOD_NAME("Stat.isCharacterDevice");
    METHOD_INSTANCE(Stat_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->isCharacterDevice(vr);

    METHOD_RETURN();
}

inline void Stat_base::s_isDirectory(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    bool vr;

    METHOD_NAME("Stat.isDirectory");
    METHOD_INSTANCE(Stat_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->isDirectory(vr);

    METHOD_RETURN();
}

inline void Stat_base::s_isFIFO(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    bool vr;

    METHOD_NAME("Stat.isFIFO");
    METHOD_INSTANCE(Stat_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->isFIFO(vr);

    METHOD_RETURN();
}

inline void Stat_base::s_isFile(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    bool vr;

    METHOD_NAME("Stat.isFile");
    METHOD_INSTANCE(Stat_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->isFile(vr);

    METHOD_RETURN();
}

inline void Stat_base::s_isSymbolicLink(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    bool vr;

    METHOD_NAME("Stat.isSymbolicLink");
    METHOD_INSTANCE(Stat_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->isSymbolicLink(vr);

    METHOD_RETURN();
}

inline void Stat_base::s_isMemory(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    bool vr;

    METHOD_NAME("Stat.isMemory");
    METHOD_INSTANCE(Stat_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->isMemory(vr);

    METHOD_RETURN();
}

inline void Stat_base::s_isSocket(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    bool vr;

    METHOD_NAME("Stat.isSocket");
    METHOD_INSTANCE(Stat_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->isSocket(vr);

    METHOD_RETURN();
}
}

#endif
