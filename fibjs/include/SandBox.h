/*
 * SandBox.h
 *
 *  Created on: Oct 22, 2012
 *      Author: lion
 */

#include "ifs/SandBox.h"
#include <map>

#ifndef SANDBOX_H_
#define SANDBOX_H_

namespace fibjs
{

class SandBox: public fibjs::SandBox_base
{
public:
    ~SandBox()
    {
        m_require.Reset();
    }

public:
    // SandBox_base
    virtual result_t add(const char *id, v8::Local<v8::Value> mod);
    virtual result_t add(v8::Local<v8::Object> mods);
    virtual result_t addScript(const char *srcname, const char *script, v8::Local<v8::Value> &retVal);
    virtual result_t remove(const char *id);
    virtual result_t run(const char *fname);
    virtual result_t require(const char *id, v8::Local<v8::Value> &retVal);

public:
    void initRoot();
    void initRequire(v8::Local<v8::Function> func)
    {
        m_require.Reset(isolate, func);
    }

    void InstallModule(std::string fname, v8::Local<v8::Value> o);
    inline void InstallNativeModule(const char *fname, ClassInfo &ci)
    {
        InstallModule(fname, ci.CreateInstance());
    }

private:
    std::map<std::string, VariantEx > m_mods;
    v8::Persistent<v8::Function> m_require;
};

} /* namespace fibjs */
#endif /* SANDBOX_H_ */
