/*
 * Variant.cpp
 *
 *  Created on: Jul 31, 2012
 *      Author: lion
 */

#include <math.h>
#include "object.h"
#include "ifs/json.h"
#include "ifs/msgpack.h"
#include "Buffer.h"
#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>

namespace fibjs {

// Native sequence of JS iterable entries (Map, Set, generator, ...).
// valueOf() (inherited from NArray) yields a JS array of the entries, while
// m_string keeps the original String(value) ("[object Map]", ...) for the
// consumers that coerce a Variant to a string (FormData, headers).
class NIterableSnapshot : public NArray {
public:
    exlib::string m_string;
};

class UNBIND_DATA {
public:
    exlib::string k;
    Variant v;
};

Variant& Variant::operator=(v8::Local<v8::Value> v)
{
    clear();

    if (v.IsEmpty() || v->IsUndefined())
        set_type(VT_Undefined);
    else if (v->IsNull())
        set_type(VT_Null);
    else if (v->IsDate()) {
        set_type(VT_Date);
        dateVal() = v;
    } else if (v->IsBoolean() || v->IsBooleanObject()) {
        set_type(VT_Boolean);

        m_Val.boolVal = Isolate::current()->toBoolean(v);
    } else if (v->IsNumber() || v->IsNumberObject()) {
        double n = Isolate::current()->toNumber(v);

        set_type(VT_Number);
        m_Val.dblVal = n;
    } else if (v->IsBigInt() || v->IsBigIntObject()) {
        v8::Local<v8::BigInt> mv;
        bool less;

        mv = v->ToBigInt(Isolate::current()->context()).FromMaybe(v8::Local<v8::BigInt>());
        set_type(VT_Long);
        m_Val.longVal = mv->Int64Value(&less);
    } else if (v->IsString() || v->IsStringObject()) {
        exlib::string str;
        GetArgumentValue(Isolate::current(), v, str);
        return operator=(str);
    } else {
        object_base* obj = object_base::getInstance(v);

        if (obj)
            return operator=(obj);
        else if (IsJSBuffer(v))
            return operator=(new Buffer(v.As<v8::Uint8Array>()));
        else {
            set_type(VT_JSValue);

            new (m_Val.jsVal) v8::Local<v8::Value>(v);

            return *this;
        }
    }

    return *this;
}

Variant::operator v8::Local<v8::Value>() const
{
    Isolate* isolate = Isolate::current();
    v8::Local<v8::Context> context = isolate->context();

    switch (type()) {
    case VT_Undefined:
        return v8::Undefined(isolate->m_isolate);
    case VT_Null:
    case VT_Type:
        return v8::Null(isolate->m_isolate);
    case VT_Boolean:
        return m_Val.boolVal ? v8::True(isolate->m_isolate) : v8::False(isolate->m_isolate);
    case VT_Integer:
        return v8::Int32::New(isolate->m_isolate, m_Val.intVal);
    case VT_Long:
        return v8::BigInt::New(isolate->m_isolate, m_Val.longVal);
    case VT_Number:
        return v8::Number::New(isolate->m_isolate, m_Val.dblVal);
    case VT_Date:
        return dateVal().value(isolate->m_isolate);
    case VT_Object: {
        object_base* obj = (object_base*)m_Val.objVal;
        if (obj == NULL)
            break;

        v8::Local<v8::Value> v;
        obj->valueOf(v);
        return v;
    }
    case VT_ArrayBuffer:
        if (m_Val.arrayBuffer) {
            std::shared_ptr<v8::BackingStore> backingStore = *reinterpret_cast<const std::shared_ptr<v8::BackingStore>*>(m_Val.arrayBuffer);
            return v8::ArrayBuffer::New(isolate->m_isolate, backingStore);
        }
        break;
    case VT_JSValue:
        return jsVal();
    case VT_JSON: {
        v8::Local<v8::Value> v;
        json_base::decode(strVal(), v);
        return v;
    }
    case VT_MSGPACK: {
        v8::Local<v8::Value> v;
        const exlib::string& s = strVal();
        obj_ptr<Buffer_base> buf = new Buffer(s.c_str(), s.length());
        msgpack_base::decode(buf, v);
        return v;
    }
    case VT_String: {
        exlib::string& str = strVal();
        return isolate->NewString(str);
    }
    }

    return v8::Null(isolate->m_isolate);
}

inline void next(int32_t& len, int32_t& pos)
{
    pos++;
    if (len > 0)
        len--;
}

inline int64_t getInt(const char* s, int32_t& len, int32_t& pos)
{
    char ch;
    int64_t n = 0;

    while (len && (ch = s[pos]) && ch >= '0' && ch <= '9') {
        n = n * 10 + ch - '0';
        next(len, pos);
    }

    return n;
}

inline char pick(const char* s, int32_t& len, int32_t& pos)
{
    return len == 0 ? 0 : s[pos];
}

void Variant::parseInt(const char* str, int32_t len)
{
    int64_t digit;
    bool bNeg;
    int32_t pos = 0;

    bNeg = (pick(str, len, pos) == '-');
    if (bNeg)
        next(len, pos);

    digit = getInt(str, len, pos);

    if (bNeg)
        digit = -digit;

    set_type(VT_Long);
    m_Val.longVal = digit;
}

void Variant::parseNumber(const char* str, int32_t len)
{
    int64_t digit, frac = 0, exp = 0;
    double v, div = 1.0;
    bool bNeg, bExpNeg;
    int32_t pos = 0;
    char ch;

    bNeg = (pick(str, len, pos) == '-');
    if (bNeg)
        next(len, pos);

    digit = getInt(str, len, pos);

    ch = pick(str, len, pos);
    if (ch == '.') {
        next(len, pos);

        while ((ch = pick(str, len, pos)) && ch >= '0' && ch <= '9') {
            frac = frac * 10 + ch - '0';
            div *= 10;
            next(len, pos);
        }
    }

    if (ch == 'e' || ch == 'E') {
        next(len, pos);

        bExpNeg = false;
        ch = pick(str, len, pos);
        if (ch == '+')
            next(len, pos);
        else if (ch == '-') {
            bExpNeg = true;
            next(len, pos);
        }

        exp = getInt(str, len, pos);
        if (bExpNeg)
            exp = -exp;
    }

    if (div > 1 || exp != 0) {
        v = digit + (frac / div);
        if (bNeg)
            v = -v;

        if (exp != 0)
            v *= pow((double)10, (int32_t)exp);

        set_type(VT_Number);
        m_Val.dblVal = v;

        return;
    }

    if (bNeg)
        digit = -digit;

    set_type(VT_Number);
    m_Val.dblVal = (double)digit;
}

#define STRING_BUF_SIZE 1024

void Variant::toString(exlib::string& retVal) const
{
    switch (type()) {
    case VT_Undefined:
        retVal = "undefined";
        break;
    case VT_Null:
    case VT_Type:
        retVal = "null";
        break;
    case VT_Boolean:
        retVal = m_Val.boolVal ? "true" : "false";
        break;
    case VT_Integer: {
        char str[STRING_BUF_SIZE];

        snprintf(str, sizeof(str), "%d", m_Val.intVal);
        retVal = str;

        break;
    }
    case VT_Long: {
        char str[STRING_BUF_SIZE];

        snprintf(str, sizeof(str), "%" PRId64, m_Val.longVal);

        retVal = str;

        break;
    }
    case VT_Number: {
        char str[STRING_BUF_SIZE];

        snprintf(str, sizeof(str), "%.16g", m_Val.dblVal);
        retVal = str;

        break;
    }
    case VT_Date:
        dateVal().toGMTString(retVal);
        break;

    case VT_Object: {
        object_base* obj = (object_base*)m_Val.objVal;

        if (obj == NULL)
            break;

        // An iterable snapshot keeps its original String(value) form.
        NIterableSnapshot* iter = dynamic_cast<NIterableSnapshot*>(obj);
        if (iter) {
            retVal = iter->m_string;
            break;
        }

        // Render the object's value form: a native object wraps itself (its
        // own toString(), or its class tag), while the NObject/NArray
        // snapshots produced by setFromJSValue become a plain object/array,
        // preserving String(value) semantics ("[object Object]", "a,b").
        v8::Local<v8::Value> v;
        if (obj->valueOf(v).ok() && !v.IsEmpty())
            GetDOMStringValue(v, retVal);

        break;
    }
    case VT_String:
        retVal = strVal();
        break;

    case VT_JSValue: {
        v8::Local<v8::Value>& v = jsVal();

        // a value is rendered as the DOM renders it (a DOMString): a plain
        // object becomes "[object Object]", a native object without a
        // toString() its class tag, and a symbol raises the engine TypeError
        GetDOMStringValue(v, retVal);
        break;
    }

    case VT_JSON:
        retVal = strVal();
        break;
    }
}

static bool js_in_stack(v8::Local<v8::Value> v,
    std::vector<v8::Local<v8::Value>>& stack)
{
    for (auto& item : stack)
        if (item->StrictEquals(v))
            return true;
    return false;
}

// Array.from(value) through the value's own realm.
static result_t js_array_from(Isolate* isolate, v8::Local<v8::Context> context,
    v8::Local<v8::Value> value, v8::Local<v8::Value>& entries)
{
    v8::Local<v8::Value> arrCtor;
    v8::Local<v8::Value> fromFn;

    if (!context->Global()->Get(context, isolate->NewString("Array")).ToLocal(&arrCtor)
        || !arrCtor->IsObject())
        return result_t(CALL_E_JAVASCRIPT);

    if (!arrCtor.As<v8::Object>()->Get(context, isolate->NewString("from")).ToLocal(&fromFn)
        || !fromFn->IsFunction())
        return result_t(CALL_E_JAVASCRIPT);

    v8::Local<v8::Value> argv[] = { value };
    if (!fromFn.As<v8::Function>()->Call(context, arrCtor, 1, argv).ToLocal(&entries))
        return result_t(CALL_E_JAVASCRIPT);

    return result_t();
}

// Deep-copy a JS value into native Variants so the result outlives the JS
// thread. A cycle is turned into its string form instead of recursing.
static result_t js_to_variant(v8::Local<v8::Value> v, Variant& out,
    std::vector<v8::Local<v8::Value>>& stack)
{
    if (v.IsEmpty() || v->IsUndefined() || v->IsNull()
        || v->IsBoolean() || v->IsBooleanObject()
        || v->IsNumber() || v->IsNumberObject()
        || v->IsString() || v->IsStringObject()
        || v->IsBigInt() || v->IsBigIntObject() || v->IsDate()) {
        out = v;
        return result_t();
    }

    // Native fibjs objects (including a JS Buffer) keep their implementation.
    if (object_base::getInstance(v) || IsJSBuffer(v)) {
        out = v;
        return result_t();
    }

    // Binary views follow the Buffer GetArgumentValue overload: any
    // TypedArray (Uint8Array and friends), DataView, ArrayBuffer and
    // SharedArrayBuffer become a Buffer, so the snapshot is BLOB-safe and
    // usable from a fiber. Blob/File stay native objects (VT_Object).
    //
    // A SharedArrayBuffer (or a view over one) must be copied: a BackingStore
    // shared with JavaScript cannot be wrapped in a v8::ArrayBuffer, and V8
    // aborts with "Cannot construct ArrayBuffer with a BackingStore of
    // SharedArrayBuffer" as soon as the Buffer is exposed again.
    if (v->IsSharedArrayBuffer()) {
        v8::Local<v8::SharedArrayBuffer> sab = v.As<v8::SharedArrayBuffer>();
        std::shared_ptr<v8::BackingStore> store = sab->GetBackingStore();
        if (store && store->Data())
            out = new Buffer(store->Data(), store->ByteLength());
        else {
            obj_ptr<Buffer_base> buf;
            Buffer_base::allocUnsafe(0, buf);
            out = buf;
        }
        return result_t();
    }

    if (v->IsTypedArray() || v->IsDataView()) {
        v8::Local<v8::ArrayBufferView> view = v.As<v8::ArrayBufferView>();
        v8::Local<v8::ArrayBuffer> ab = view->Buffer();
        std::shared_ptr<v8::BackingStore> store = ab.IsEmpty()
            ? nullptr : ab->GetBackingStore();
        if (ab.IsEmpty() || (store && store->IsShared())) {
            size_t len = view->ByteLength();
            Buffer* buf = new Buffer(NULL, len);
            if (len > 0)
                view->CopyContents(buf->data(), len);
            out = buf;
            return result_t();
        }
    }

    if (v->IsTypedArray() || v->IsDataView()
        || v->IsArrayBuffer()) {
        obj_ptr<Buffer_base> buf;
        result_t hr = GetArgumentValue(Isolate::current(), v, buf);
        if (hr.failed())
            return hr;

        out = buf;
        return result_t();
    }

    if (v->IsFunction() || v->IsRegExp() || v->IsNativeError()
        || !v->IsObject() || stack.size() >= 64 || js_in_stack(v, stack)) {
        exlib::string s;
        result_t hr = GetDOMStringValue(v, s);
        if (hr.failed())
            return hr;

        out = s;
        return result_t();
    }

    Isolate* isolate = Isolate::current();
    v8::Local<v8::Context> context = isolate->context();
    v8::Local<v8::Object> obj = v.As<v8::Object>();

    // A non-native JS iterable (Map, Set, generator, ...) keeps its entries
    // as a native sequence. This is what URLSearchParams(init), FormData and
    // other WebIDL iterable consumers expect; stringifying it would lose the
    // entries. FormData/URLSearchParams/Headers are native objects and were
    // kept above; arrays are handled by the structured branch below.
    if (!v->IsArray()) {
        v8::Local<v8::Value> iterFn;
        if (obj->Get(context, v8::Symbol::GetIterator(isolate->m_isolate)).ToLocal(&iterFn)
            && iterFn->IsFunction()) {
            v8::Local<v8::Value> entries;
            result_t hr = js_array_from(isolate, context, v, entries);
            if (hr.failed())
                return hr;

            // The original String(value) is what FormData/headers coerce; the
            // entry sequence is what URLSearchParams and other iterable
            // consumers read.
            obj_ptr<NIterableSnapshot> snap = new NIterableSnapshot();
            hr = GetDOMStringValue(v, snap->m_string);
            if (hr.failed())
                return hr;

            stack.push_back(v);
            Variant nv;
            hr = js_to_variant(entries, nv, stack);
            stack.pop_back();
            if (hr.failed())
                return hr;

            NArray* arr = dynamic_cast<NArray*>(nv.object());
            if (arr)
                for (auto& item : arr->m_array)
                    snap->append(item);

            out = snap;
            return result_t();
        }
    }

    // Keep a custom String(value) (FormData, headers, ...) instead of turning
    // it into a structured snapshot: a plain object/array keeps the snapshot.
    // A value without a usable string form (for example Object.create(null))
    // fails here, exactly like String(value) would.
    if (!v->IsArray()) {
        exlib::string dom;
        result_t hr = GetDOMStringValue(v, dom);
        if (hr.failed())
            return hr;
        if (dom != "[object Object]") {
            out = dom;
            return result_t();
        }
    }

    result_t hr;

    stack.push_back(v);

    if (v->IsArray()) {
        obj_ptr<NArray> arr = new NArray();
        int32_t len = v.As<v8::Array>()->Length();

        for (int32_t i = 0; i < len; i++) {
            v8::Local<v8::Value> item;
            if (!obj->Get(context, (uint32_t)i).ToLocal(&item)) {
                hr = result_t(CALL_E_JAVASCRIPT);
                break;
            }

            Variant iv;
            hr = js_to_variant(item, iv, stack);
            if (hr.failed())
                break;

            arr->append(iv);
        }

        if (hr.ok())
            out = arr;
    } else {
        obj_ptr<NObject> nobj = new NObject();
        JSArray keys = obj->GetPropertyNames(context);
        int32_t len = keys.IsEmpty() ? 0 : keys->Length();

        for (int32_t i = 0; i < len; i++) {
            v8::Local<v8::Value> key;
            if (!keys->Get(context, (uint32_t)i).ToLocal(&key)) {
                hr = result_t(CALL_E_JAVASCRIPT);
                break;
            }

            v8::Local<v8::Value> val;
            if (!obj->Get(context, key).ToLocal(&val)) {
                hr = result_t(CALL_E_JAVASCRIPT);
                break;
            }

            Variant nv;
            hr = js_to_variant(val, nv, stack);
            if (hr.failed())
                break;

            nobj->add(isolate->toString(key), nv);
        }

        if (hr.ok())
            out = nobj;
    }

    stack.pop_back();
    return hr;
}

result_t Variant::setFromJSValue(v8::Local<v8::Value> v)
{
    std::vector<v8::Local<v8::Value>> stack;
    return js_to_variant(v, *this, stack);
}

}
