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

namespace fibjs {

class TryCatch : public v8::TryCatch {
public:
    TryCatch()
        : v8::TryCatch(Isolate::current()->m_isolate)
    {
    }
};

inline bool IsEmpty(v8::Local<v8::Value>& v)
{
    return v.IsEmpty() || v->IsUndefined() || v->IsNull();
}

inline void extend(const v8::Local<v8::Object> src,
    v8::Local<v8::Object>& dest, bool bDataOnly = true)
{
    TryCatch try_catch;
    JSArray ks = src->GetPropertyNames();
    int32_t len = ks->Length();
    int32_t i;

    for (i = 0; i < len; i++) {
        JSValue k = ks->Get(i);
        JSValue v = src->Get(k);

        if (!bDataOnly || (!v.IsEmpty() && !v->IsFunction()))
            dest->Set(k, v);
    }
}

class object_base;

class Variant {
public:
    enum Type {
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
        VT_UNBOUND_ARRAY,
        VT_UNBOUND_OBJECT,
        VT_Type = 255,
        VT_Global = 256
    };

public:
    Variant()
        : m_type(VT_Undefined)
    {
    }

    Variant(const Variant& v)
        : m_type(VT_Undefined)
    {
        operator=(v);
    }

    Variant(const exlib::string& v)
        : m_type(VT_Undefined)
    {
        operator=(v);
    }

    Variant(const char* v)
        : m_type(VT_Undefined)
    {
        operator=(v);
    }

    Variant(obj_base* v)
        : m_type(VT_Undefined)
    {
        operator=(v);
    }

    Variant(int32_t v)
        : m_type(VT_Undefined)
    {
        operator=(v);
    }

    Variant(int64_t v)
        : m_type(VT_Undefined)
    {
        operator=(v);
    }

    Variant(double v)
        : m_type(VT_Undefined)
    {
        operator=(v);
    }

    Variant(date_t v)
        : m_type(VT_Undefined)
    {
        operator=(v);
    }

    Variant(bool v)
        : m_type(VT_Undefined)
    {
        operator=(v);
    }

    template <typename T>
    Variant(obj_ptr<T>& v)
        : m_type(VT_Undefined)
    {
        operator=((T*)v);
    }

    Variant(v8::Local<v8::Value> v)
        : m_type(VT_Undefined)
    {
        operator=(v);
    }

    ~Variant()
    {
        clear();
    }

    void clear()
    {
        Type _t = type();

        if (_t == VT_UNBOUND_ARRAY || _t == VT_UNBOUND_OBJECT)
            clearUnbind();
        else if (_t == VT_String || _t == VT_JSON)
            strVal().~basic_string();
        else if (_t == VT_Object && m_Val.objVal)
            m_Val.objVal->Unref();
        else if (_t == VT_JSValue) {
            if (isGlobal()) {
                v8::Global<v8::Value>& jsobj = jsValEx();
                jsobj.Reset();
                jsobj.~Global();
            } else {
                v8::Local<v8::Value>& jsobj = jsVal();
                jsobj.~Local();
            }
        }

        set_type(VT_Undefined);
    }

    Variant& operator=(const Variant& v)
    {
        Type _t = v.type();

        if (_t == VT_String)
            return operator=(v.strVal());

        if (_t == VT_Object)
            return operator=(v.m_Val.objVal);

        if (_t == VT_JSValue) {
            if (v.isGlobal())
                return operator=(v8::Local<v8::Value>::New(Isolate::current()->m_isolate, v.jsValEx()));
            else
                return operator=(v.jsVal());
        }

        assert(_t != VT_UNBOUND_ARRAY && _t != VT_UNBOUND_OBJECT);

        clear();
        set_type(_t);
        m_Val.longVal = v.m_Val.longVal;

        return *this;
    }

    bool strictEqual(const Variant& v)
    {
        Type _t = type();

        if (_t != v.type())
            return false;

        switch (_t) {
        case VT_Undefined:
            return true;
        case VT_Null:
            return true;
        case VT_Number:
            return (m_Val.dblVal == v.m_Val.dblVal);
        case VT_Integer:
            return (m_Val.intVal == v.m_Val.intVal);
        case VT_Long:
            return (m_Val.longVal == v.m_Val.longVal);
        case VT_String:
            return (strVal() == v.strVal());
        case VT_Boolean:
            return (m_Val.boolVal == v.m_Val.boolVal);
        case VT_Object:
            return (object() == v.object());
        case VT_JSValue:
            return jsVal()->StrictEquals(v.jsVal());
        default:
            return false;
        }
    }

    Variant& operator=(int32_t v)
    {
        clear();

        set_type(VT_Integer);
        m_Val.intVal = v;

        return *this;
    }

    Variant& operator=(int64_t v)
    {
        clear();

        set_type(VT_Long);
        m_Val.longVal = v;

        return *this;
    }

    Variant& operator=(bool v)
    {
        clear();

        set_type(VT_Boolean);
        m_Val.boolVal = v;

        return *this;
    }

    Variant& operator=(double v)
    {
        clear();

        set_type(VT_Number);
        m_Val.dblVal = v;

        return *this;
    }

    Variant& operator=(date_t& v)
    {
        clear();

        set_type(VT_Date);
        dateVal() = v;

        return *this;
    }

    Variant& operator=(const exlib::string& v)
    {
        if (type() != VT_String) {
            clear();
            set_type(VT_String);
            new (m_Val.strVal) exlib::string(v);
        } else
            strVal() = v;

        return *this;
    }

    Variant& operator=(const char* v)
    {
        exlib::string s(v);

        return operator=(s);
    }

    Variant& operator=(object_base* v)
    {
        return operator=((obj_base*)v);
    }

    template <typename T>
    Variant& operator=(obj_ptr<T>& v)
    {
        return operator=((T*)v);
    }

    Variant& operator=(v8::Local<v8::Value> v);

    Variant& operator=(v8::Local<v8::Function> v)
    {
        return operator=(v8::Local<v8::Value>::Cast(v));
    }

    Variant& operator=(v8::Local<v8::Object> v)
    {
        return operator=(v8::Local<v8::Value>::Cast(v));
    }

    Variant& operator=(v8::Local<v8::Array> v)
    {
        return operator=(v8::Local<v8::Value>::Cast(v));
    }

    Variant& setNull()
    {
        clear();

        set_type(VT_Null);
        return *this;
    }

    Type type() const
    {
        return (Type)(m_type & VT_Type);
    }

    void set_type(Type t)
    {
        m_type = (Type)(t | (m_type & VT_Global));
    }

    bool isUndefined()
    {
        return type() == VT_Undefined;
    }

    void toGlobal()
    {
        m_type = (Type)(m_type | VT_Global);
    }

    bool isGlobal() const
    {
        return (m_type & VT_Global) == VT_Global;
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

    int32_t intVal() const
    {
        if (type() == VT_Integer)
            return m_Val.intVal;
        if (type() == VT_Long)
            return (int32_t)m_Val.longVal;
        if (type() == VT_Number)
            return (int32_t)m_Val.dblVal;
        return 0;
    }

    int64_t longVal() const
    {
        if (type() == VT_Integer)
            return m_Val.intVal;
        if (type() == VT_Long)
            return m_Val.longVal;
        if (type() == VT_Number)
            return (int64_t)m_Val.dblVal;
        return 0;
    }

    bool boolVal() const
    {
        if (type() != VT_Boolean)
            return 0;
        return m_Val.boolVal;
    }

    double dblVal() const
    {
        if (type() == VT_Integer)
            return m_Val.intVal;
        if (type() == VT_Long)
            return (double)m_Val.longVal;
        if (type() == VT_Number)
            return m_Val.dblVal;
        return 0;
    }

    void parseNumber(const char* str, int32_t len = -1);
    void parseDate(const char* str, int32_t len = -1)
    {
        set_type(VT_Date);
        dateVal().parse(str, len);
    }

    void toString(exlib::string& retVal) const;

    void toJSON();

    int32_t unbind();
    void clearUnbind();

    object_base* object() const
    {
        if (type() != VT_Object)
            return NULL;
        return (object_base*)m_Val.objVal;
    }

private:
    Variant& operator=(obj_base* v)
    {
        clear();

        set_type(VT_Object);
        m_Val.objVal = v;
        if (m_Val.objVal)
            m_Val.objVal->Ref();

        return *this;
    }

    exlib::string& strVal() const
    {
        exlib::string* pval = (exlib::string*)m_Val.strVal;
        return *pval;
    }

    date_t& dateVal() const
    {
        date_t* pval = (date_t*)m_Val.dateVal;
        return *pval;
    }

    v8::Global<v8::Value>& jsValEx() const
    {
        v8::Global<v8::Value>* pval = (v8::Global<v8::Value>*)m_Val.jsVal;
        return *pval;
    }

    v8::Local<v8::Value>& jsVal() const
    {
        v8::Local<v8::Value>* pval = (v8::Local<v8::Value>*)m_Val.jsVal;
        return *pval;
    }

private:
    struct buf {
        int32_t cnt;
        void* data;
    };

    Type m_type;
    union {
        bool boolVal;
        int32_t intVal;
        int64_t longVal;
        double dblVal;
        obj_base* objVal;
        char dateVal[sizeof(date_t)];
        char strVal[sizeof(exlib::string)];
        char jsVal[sizeof(v8::Global<v8::Value>)];
        buf buffer;
    } m_Val;
};

class VariantEx : public Variant {

public:
    VariantEx()
    {
        toGlobal();
    }

    VariantEx(const Variant& v)
    {
        toGlobal();
        operator=(v);
    }

    VariantEx(const VariantEx& v)
    {
        toGlobal();
        operator=(v);
    }

    VariantEx(v8::Local<v8::Value> v)
    {
        toGlobal();
        operator=(v);
    }

    VariantEx(const exlib::string& v)
    {
        toGlobal();
        operator=(v);
    }

    VariantEx(const char* v)
    {
        toGlobal();
        operator=(v);
    }

    Variant& operator=(const VariantEx& v)
    {
        Variant::operator=((const Variant)v);
        return *this;
    }

    template <typename T>
    VariantEx& operator=(T v)
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
