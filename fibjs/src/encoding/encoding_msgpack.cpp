/*
 * encoding_msgpack.cpp
 *
 *  Created on: Apr 26, 2020
 *      Author: lion
 */

#include "object.h"
#include "ifs/encoding.h"
#include "Buffer.h"
#include <msgpack.h>

namespace fibjs {

DECLARE_MODULE(msgpack);

result_t msgpack_base::encode(v8::Local<v8::Value> data, obj_ptr<Buffer_base>& retVal)
{
    class MsgpackPacker {
    public:
        MsgpackPacker()
        {
            isolate = Isolate::current();
            msgpack_sbuffer_init(&sbuf);
            msgpack_packer_init(&pk, &sbuf, msgpack_sbuffer_write);
        }

        ~MsgpackPacker()
        {
            msgpack_sbuffer_destroy(&sbuf);
        }

        result_t pack(v8::Local<v8::Value> element)
        {
            if (element.IsEmpty() || element->IsFunction())
                return CHECK_ERROR(CALL_E_BADVARTYPE);
            else if (element->IsNull() || element->IsUndefined())
                msgpack_pack_nil(&pk);
            else if (element->IsBoolean() || element->IsBooleanObject()) {
                if (isolate->toBoolean(element))
                    msgpack_pack_true(&pk);
                else
                    msgpack_pack_false(&pk);
            } else if (element->IsNumber() || element->IsNumberObject()) {
                double num = isolate->toNumber(element);
                if (static_cast<double>(static_cast<int64_t>(num)) == num && num <= LLONG_MAX && num >= LLONG_MIN) {
                    msgpack_pack_int64(&pk, (int64_t)num);
                } else {
                    msgpack_pack_double(&pk, num);
                }
            } else if (element->IsBigInt() || element->IsBigIntObject()) {
                v8::MaybeLocal<v8::BigInt> mv;
                bool less;

                mv = element->ToBigInt(Isolate::current()->context());
                msgpack_pack_int64(&pk, mv.ToLocalChecked()->Int64Value(&less));
            } else if (element->IsDate()) {
                date_t d = isolate->toNumber(element);
                msgpack_timestamp _d;

                d.get_timestamp(_d);
                msgpack_pack_timestamp(&pk, &_d);
            } else if (element->IsArray()) {
                return pack(v8::Local<v8::Array>::Cast(element));
            } else if (element->IsSet()) {
                return pack(v8::Local<v8::Set>::Cast(element)->AsArray());
            } else if (element->IsMap()) {
                return pack(v8::Local<v8::Map>::Cast(element));
            } else if (element->IsObject() && !element->IsStringObject()) {
                return pack(v8::Local<v8::Object>::Cast(element));
            } else {
                v8::String::Utf8Value v(isolate->m_isolate, element);

                msgpack_pack_str(&pk, v.length());
                msgpack_pack_str_body(&pk, ToCString(v), v.length());
            }

            return 0;
        }

        result_t pack(v8::Local<v8::Object> element)
        {
            obj_ptr<Buffer_base> buf;
            v8::Local<v8::Context> context = isolate->context();

            if (element->IsTypedArray())
                Buffer_base::_new(v8::Local<v8::TypedArray>::Cast(element), buf, v8::Local<v8::Object>());
            else
                buf = Buffer_base::getInstance(element);

            if (buf) {
                exlib::string strBuf;

                buf->toString(strBuf);

                msgpack_pack_bin(&pk, strBuf.length());
                msgpack_pack_bin_body(&pk, strBuf.c_str(), strBuf.length());

                return 0;
            }

            JSValue jsonFun = element->Get(context, isolate->NewString("toJSON", 6));
            if (!IsEmpty(jsonFun) && jsonFun->IsFunction()) {
                JSValue p = isolate->NewString("");
                JSValue element1 = v8::Local<v8::Function>::Cast(jsonFun)->Call(context, element, 1, &p);

                if (!IsEmpty(element1)) {
                    if (element1->IsArray())
                        return pack(v8::Local<v8::Array>::Cast(element1));

                    if (!element1->IsObject())
                        return pack(element1);

                    element = v8::Local<v8::Object>::Cast(element1);
                }
            }

            JSArray ks = element->GetPropertyNames(context);
            int32_t len = ks->Length();
            int32_t i;
            result_t hr;

            std::vector<JSValue> ka;
            std::vector<JSValue> va;

            for (i = 0; i < len; i++) {
                JSValue k = ks->Get(context, i);
                JSValue v = element->Get(context, k);

                if (!v->IsFunction()) {
                    ka.push_back(k);
                    va.push_back(v);
                }
            }

            msgpack_pack_map(&pk, ka.size());
            for (i = 0; i < (int32_t)ka.size(); i++) {
                hr = pack(ka[i]);
                if (hr < 0)
                    return hr;

                hr = pack(va[i]);
                if (hr < 0)
                    return hr;
            }

            return 0;
        }

        result_t pack(v8::Local<v8::Array> element)
        {
            v8::Local<v8::Context> context = isolate->context();
            int32_t len = element->Length();
            int32_t i;
            result_t hr;

            msgpack_pack_array(&pk, len);
            for (i = 0; i < len; i++) {
                hr = pack((JSValue)element->Get(context, i));
                if (hr < 0)
                    return hr;
            }

            return 0;
        }

        result_t pack(v8::Local<v8::Map> element)
        {
            v8::Local<v8::Context> context = isolate->context();

            v8::Local<v8::Array> arr = element->AsArray();
            uint32_t size = element->Size();
            uint32_t len = arr->Length();

            uint32_t i;
            result_t hr;

            msgpack_pack_map(&pk, size);
            for (i = 0; i < len; i++) {
                hr = pack((JSValue)arr->Get(context, i));
                if (hr < 0)
                    return hr;
            }

            return 0;
        }

    public:
        Isolate* isolate;
        msgpack_sbuffer sbuf;
        msgpack_packer pk;
    };

    MsgpackPacker mp;
    result_t hr = mp.pack(data);
    if (hr < 0)
        return hr;

    retVal = new Buffer(mp.sbuf.data, mp.sbuf.size);

    return 0;
}

result_t msgpack_base::decode(Buffer_base* data, v8::Local<v8::Value>& retVal)
{
    class MsgpackUnPacker {
    public:
        MsgpackUnPacker()
        {
            isolate = Isolate::current();
            msgpack_zone_init(&mempool, 2048);
        }

        ~MsgpackUnPacker()
        {
            msgpack_zone_destroy(&mempool);
        }

        result_t unpack(Buffer_base* data)
        {
            data->toString(buf);
            msgpack_unpack_return ret = msgpack_unpack(buf.c_str(), buf.length(), NULL, &mempool, &deserialized);
            if (ret != 2)
                return -1;

            return 0;
        }

        v8::Local<v8::Value> map_js_value(msgpack_object* o)
        {
            v8::Local<v8::Context> context = isolate->context();
            v8::Local<v8::Value> v;

            switch (o->type) {
            case MSGPACK_OBJECT_NIL:
                v = v8::Null(isolate->m_isolate);
                break;
            case MSGPACK_OBJECT_BOOLEAN:
                v = o->via.boolean ? v8::True(isolate->m_isolate) : v8::False(isolate->m_isolate);
                break;
            case MSGPACK_OBJECT_FLOAT32:
            case MSGPACK_OBJECT_FLOAT64:
                v = v8::Number::New(isolate->m_isolate, o->via.f64);
                break;
            case MSGPACK_OBJECT_NEGATIVE_INTEGER:
                if (o->via.i64 <= 9007199254740992 && o->via.i64 >= -9007199254740992)
                    v = v8::Number::New(isolate->m_isolate, (double)o->via.i64);
                else
                    v = v8::BigInt::New(isolate->m_isolate, o->via.i64);
                break;
            case MSGPACK_OBJECT_POSITIVE_INTEGER:
                if (o->via.u64 <= 9007199254740992)
                    v = v8::Number::New(isolate->m_isolate, (double)o->via.u64);
                else
                    v = v8::BigInt::New(isolate->m_isolate, o->via.u64);
                break;
            case MSGPACK_OBJECT_STR:
                v = isolate->NewString(o->via.str.ptr, (int32_t)o->via.str.size);
                break;
            case MSGPACK_OBJECT_BIN: {
                obj_ptr<Buffer_base> buf = new Buffer(o->via.bin.ptr, (int32_t)o->via.bin.size);
                v = buf->wrap();
                break;
            }
            case MSGPACK_OBJECT_ARRAY: {
                v8::Local<v8::Array> arr = v8::Array::New(isolate->m_isolate, (int32_t)o->via.array.size);
                int32_t i;

                for (i = 0; i < (int32_t)o->via.array.size; i++)
                    arr->Set(context, i, map_js_value(o->via.array.ptr + i));
                v = arr;
                break;
            }
            case MSGPACK_OBJECT_MAP: {
                v8::Local<v8::Object> obj = v8::Object::New(isolate->m_isolate);
                int32_t i;

                for (i = 0; i < (int32_t)o->via.map.size; i++) {
                    msgpack_object_kv* p = o->via.map.ptr + i;

                    if (p->key.type == MSGPACK_OBJECT_STR) {
                        obj->Set(context, isolate->NewString(p->key.via.str.ptr, (int32_t)p->key.via.str.size),
                            map_js_value(&p->val));
                    } else {
                        obj->Set(context, map_js_value(&p->key), map_js_value(&p->val));
                    }
                }
                v = obj;
                break;
            }
            case MSGPACK_OBJECT_EXT: {
                if (o->via.ext.type == -1) {
                    msgpack_timestamp _d = { 0 };
                    date_t d;

                    msgpack_object_to_timestamp(o, &_d);
                    d.set_timestamp(_d);
                    v = d.value(isolate->m_isolate);
                } else {
                    obj_ptr<Buffer_base> buf = new Buffer(o->via.ext.ptr, (int32_t)o->via.ext.size);
                    v = buf->wrap();
                }
                break;
            }
            default:
                v = v8::Null(isolate->m_isolate);
                break;
            }

            return v;
        }

        v8::Local<v8::Value> jsValue()
        {
            return map_js_value(&deserialized);
        }

    public:
        Isolate* isolate;
        msgpack_zone mempool;
        msgpack_object deserialized;
        exlib::string buf;
    };

    MsgpackUnPacker mu;

    result_t hr = mu.unpack(data);
    if (hr < 0)
        return 0;

    retVal = mu.jsValue();

    return 0;
}
}
