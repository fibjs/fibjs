/*
 * Variant.h
 *
 *  Created on: Jul 26, 2012
 *      Author: lion
 */

#include <v8/include/v8.h>
#include <string>
#include "date.h"
#include "obj_ptr.h"

#ifndef VARIANT_H_
#define VARIANT_H_

namespace fibjs
{

class TryCatch : public v8::TryCatch
{
public:
    ~TryCatch() {
        Reset();
    }
};

inline bool IsEmpty(v8::Local<v8::Value> &v)
{
    return v.IsEmpty() || v->IsUndefined() || v->IsNull();
}

inline void extend(const v8::Local<v8::Object> src,
                   v8::Local<v8::Object> &dest, bool bDataOnly = true)
{
    TryCatch try_catch;
    v8::Local<v8::Array> ks = src->GetPropertyNames();
    int32_t len = ks->Length();
    int32_t i;

    for (i = 0; i < len; i++)
    {
        v8::Local<v8::Value> k = ks->Get(i);
        v8::Local<v8::Value> v = src->Get(k);

        if (!bDataOnly || (!v.IsEmpty() && !v->IsFunction()))
            dest->Set(k, v);
    }
}

class object_base;

class Variant
{
public:
    enum Type
    {
        VT_Undefined = 0,
        VT_Null,
        VT_Boolean,
        VT_Integer,
        VT_Long,
        VT_Number,
        VT_Date,
        VT_String,
        VT_Object,
        VT_JSValue,
        VT_JSON,
        VT_Type = 255,
        VT_Persistent = 256
    };

public:
    Variant() :
        m_type(VT_Undefined)
    {
    }

    Variant(const Variant &v) :
        m_type(VT_Undefined)
    {
        operator=(v);
    }

    Variant(const exlib::string &v) :
        m_type(VT_Undefined)
    {
        operator=(v);
    }

    Variant(const char *v) :
        m_type(VT_Undefined)
    {
        operator=(v);
    }

    Variant(obj_base* v) :
        m_type(VT_Undefined)
    {
        operator=(v);
    }

    template<typename T>
    Variant(obj_ptr<T> &v) :
        m_type(VT_Undefined)
    {
        operator=((T*)v);
    }

    Variant(v8::Local<v8::Value> v) :
        m_type(VT_Undefined)
    {
        operator=(v);
    }

    ~Variant()
    {
        clear();
    }

    void clear()
    {
        if (type() == VT_String || type() == VT_JSON)
            strVal().~basic_string();
        else if (type() == VT_Object && m_Val.objVal)
            m_Val.objVal->Unref();
        else if (type() == VT_JSValue)
        {
            if (isPersistent())
            {
                v8::Persistent<v8::Value> &jsobj = jsValEx();
                jsobj.Reset();
                jsobj.~Persistent();
            }
            else
            {
                v8::Local<v8::Value> &jsobj = jsVal();
                jsobj.~Local();
            }
        }

        set_type(VT_Undefined);
    }

    Variant &operator=(const Variant &v)
    {
        if (v.type() == VT_String)
            return operator=(v.strVal());

        if (v.type() == VT_Object)
            return operator=(v.m_Val.objVal);

        if (v.type() == VT_JSValue)
        {
            if (v.isPersistent())
                return operator=(v8::Local<v8::Value>::New(Isolate::current()->m_isolate, v.jsValEx()));
            else
                return operator=(v.jsVal());
        }

        clear();
        set_type(v.type());
        m_Val.longVal = v.m_Val.longVal;

        return *this;
    }

    Variant &operator=(int32_t v)
    {
        clear();

        set_type(VT_Integer);
        m_Val.intVal = v;

        return *this;
    }

    Variant &operator=(int64_t v)
    {
        clear();

        if (v >= -2147483648ll && v <= 2147483647ll)
        {
            set_type(VT_Integer);
            m_Val.intVal = (int32_t) v;
        }
        else
        {
            set_type(VT_Long);
            m_Val.longVal = v;
        }

        return *this;
    }

    Variant &operator=(double v)
    {
        clear();

        set_type(VT_Number);
        m_Val.dblVal = v;

        return *this;
    }

    Variant &operator=(date_t &v)
    {
        clear();

        set_type(VT_Date);
        dateVal() = v;

        return *this;
    }

    Variant &operator=(const exlib::string &v)
    {
        if (type() != VT_String)
        {
            clear();
            set_type(VT_String);
            new (m_Val.strVal) exlib::string(v);
        }
        else
            strVal() = v;

        return *this;
    }

    Variant &operator=(const char *v)
    {
        exlib::string s(v);

        return operator=(s);
    }

    Variant &operator=(object_base *v)
    {
        return operator=((obj_base *) v);
    }

    template<typename T>
    Variant &operator=(obj_ptr<T> &v)
    {
        return operator=((T*) v);
    }

    Variant &operator=(v8::Local<v8::Value> v);

    Variant &operator=(v8::Local<v8::Function> v)
    {
        return operator=(v8::Local<v8::Value>::Cast(v));
    }

    Variant &operator=(v8::Local<v8::Object> v)
    {
        return operator=(v8::Local<v8::Value>::Cast(v));
    }

    Variant &operator=(v8::Local<v8::Array> v)
    {
        return operator=(v8::Local<v8::Value>::Cast(v));
    }

    Variant &setNull()
    {
        clear();

        set_type(VT_Null);
        return *this;
    }

    Type type() const
    {
        return (Type) (m_type & VT_Type);
    }

    void set_type(Type t)
    {
        m_type = (Type) (t | (m_type & VT_Persistent));
    }

    bool isUndefined()
    {
        return type() == VT_Undefined;
    }

    void toPersistent()
    {
        m_type = (Type) (m_type | VT_Persistent);
    }

    bool isPersistent() const
    {
        return (m_type & VT_Persistent) == VT_Persistent;
    }

    size_t size() const
    {
        if (type() != VT_String)
            return sizeof(Variant);

        return sizeof(Variant) + strVal().length();
    }

    exlib::string string() const
    {
        if (type() != VT_String)
            return "";
        return strVal();
    }

    operator v8::Local<v8::Value>() const;

    operator int32_t() const
    {
        if (type() != VT_Integer)
            return 0;
        return m_Val.intVal;
    }

    bool boolVal() const
    {
        if (type() != VT_Boolean)
            return 0;
        return m_Val.boolVal;
    }

    void parseNumber(const char *str, int32_t len = -1);
    void parseDate(const char *str, int32_t len = -1)
    {
        set_type(VT_Date);
        dateVal().parse(str, len);
    }

    void toString(exlib::string &retVal) const;

    void toJSON();

    object_base *object() const
    {
        if (type() != VT_Object)
            return NULL;
        return (object_base *)m_Val.objVal;
    }

private:
    Variant &operator=(obj_base *v)
    {
        clear();

        set_type(VT_Object);
        m_Val.objVal = v;
        if (m_Val.objVal)
            m_Val.objVal->Ref();

        return *this;
    }

    exlib::string &strVal() const
    {
        exlib::string *pval = (exlib::string *)m_Val.strVal;
        return *pval;
    }

    date_t &dateVal() const
    {
        date_t *pval = (date_t *)m_Val.dateVal;
        return *pval;
    }

    v8::Persistent<v8::Value> &jsValEx() const
    {
        v8::Persistent<v8::Value> *pval = (v8::Persistent<v8::Value> *)m_Val.jsVal;
        return *pval;
    }

    v8::Local<v8::Value> &jsVal() const
    {
        v8::Local<v8::Value> *pval = (v8::Local<v8::Value> *)m_Val.jsVal;
        return *pval;
    }

private:
    Type m_type;
    union
    {
        bool boolVal;
        int32_t intVal;
        int64_t longVal;
        double dblVal;
        obj_base *objVal;
        char dateVal[sizeof(date_t)];
        char strVal[sizeof(exlib::string)];
        char jsVal[sizeof(v8::Persistent<v8::Value>)];
    } m_Val;
};

class VariantEx: public Variant
{

public:
    VariantEx()
    {
        toPersistent();
    }

    VariantEx(const Variant &v)
    {
        toPersistent();
        operator=(v);
    }

    VariantEx(const VariantEx &v)
    {
        toPersistent();
        operator=(v);
    }

    VariantEx(v8::Local<v8::Value> v)
    {
        toPersistent();
        operator=(v);
    }

    VariantEx(const exlib::string &v)
    {
        toPersistent();
        operator=(v);
    }

    VariantEx(const char *v)
    {
        toPersistent();
        operator=(v);
    }

    Variant &operator=(const VariantEx &v)
    {
        Variant::operator=((const Variant)v);
        return *this;
    }

    template<typename T>
    VariantEx &operator=(T v)
    {
        Variant::operator=(v);
        return *this;
    }

    operator v8::Local<v8::Value>() const
    {
        return Variant::operator v8::Local<v8::Value>();
    }
};

} /* namespace fibjs */
#endif /* VARIANT_H_ */
