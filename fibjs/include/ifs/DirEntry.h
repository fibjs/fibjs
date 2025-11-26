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

class DirEntry_base : public object_base {
    DECLARE_CLASS(DirEntry_base);

public:
    // DirEntry_base
    virtual result_t get_name(exlib::string& retVal) = 0;
    virtual result_t get_parentPath(exlib::string& retVal) = 0;
    virtual result_t isBlockDevice(bool& retVal) = 0;
    virtual result_t isCharacterDevice(bool& retVal) = 0;
    virtual result_t isDirectory(bool& retVal) = 0;
    virtual result_t isFIFO(bool& retVal) = 0;
    virtual result_t isFile(bool& retVal) = 0;
    virtual result_t isSymbolicLink(bool& retVal) = 0;
    virtual result_t isSocket(bool& retVal) = 0;

public:
    static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        CONSTRUCT_INIT();

        ThrowTypeError("not a constructor");
    }

    static result_t load(v8::Local<v8::Value> v, obj_ptr<DirEntry_base>& retVal)
    { return CALL_E_TYPEMISMATCH; }

public:
    static void s_get_name(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_parentPath(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_isBlockDevice(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_isCharacterDevice(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_isDirectory(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_isFIFO(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_isFile(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_isSymbolicLink(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_isSocket(const v8::FunctionCallbackInfo<v8::Value>& args);
};
}

namespace fibjs {
inline ClassInfo& DirEntry_base::class_info()
{
    static ClassData::ClassMethod s_method[] = {
        { "isBlockDevice", s_isBlockDevice, false, ClassData::ASYNC_SYNC },
        { "isCharacterDevice", s_isCharacterDevice, false, ClassData::ASYNC_SYNC },
        { "isDirectory", s_isDirectory, false, ClassData::ASYNC_SYNC },
        { "isFIFO", s_isFIFO, false, ClassData::ASYNC_SYNC },
        { "isFile", s_isFile, false, ClassData::ASYNC_SYNC },
        { "isSymbolicLink", s_isSymbolicLink, false, ClassData::ASYNC_SYNC },
        { "isSocket", s_isSocket, false, ClassData::ASYNC_SYNC }
    };

    static ClassData::ClassProperty s_property[] = {
        { "name", s_get_name, block_set, false },
        { "parentPath", s_get_parentPath, block_set, false }
    };

    static ClassData s_cd = {
        "DirEntry", false, s__new, NULL,
        ARRAYSIZE(s_method), s_method, 0, NULL, ARRAYSIZE(s_property), s_property, 0, NULL, NULL, NULL,
        &object_base::class_info(),
        false
    };

    static ClassInfo s_ci(s_cd);
    return s_ci;
}

inline void DirEntry_base::s_get_name(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_INSTANCE(DirEntry_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->get_name(vr);

    METHOD_RETURN();
}

inline void DirEntry_base::s_get_parentPath(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_INSTANCE(DirEntry_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->get_parentPath(vr);

    METHOD_RETURN();
}

inline void DirEntry_base::s_isBlockDevice(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    bool vr;

    METHOD_INSTANCE(DirEntry_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->isBlockDevice(vr);

    METHOD_RETURN();
}

inline void DirEntry_base::s_isCharacterDevice(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    bool vr;

    METHOD_INSTANCE(DirEntry_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->isCharacterDevice(vr);

    METHOD_RETURN();
}

inline void DirEntry_base::s_isDirectory(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    bool vr;

    METHOD_INSTANCE(DirEntry_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->isDirectory(vr);

    METHOD_RETURN();
}

inline void DirEntry_base::s_isFIFO(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    bool vr;

    METHOD_INSTANCE(DirEntry_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->isFIFO(vr);

    METHOD_RETURN();
}

inline void DirEntry_base::s_isFile(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    bool vr;

    METHOD_INSTANCE(DirEntry_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->isFile(vr);

    METHOD_RETURN();
}

inline void DirEntry_base::s_isSymbolicLink(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    bool vr;

    METHOD_INSTANCE(DirEntry_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->isSymbolicLink(vr);

    METHOD_RETURN();
}

inline void DirEntry_base::s_isSocket(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    bool vr;

    METHOD_INSTANCE(DirEntry_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->isSocket(vr);

    METHOD_RETURN();
}
}
