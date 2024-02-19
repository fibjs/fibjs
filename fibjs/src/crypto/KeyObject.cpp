/*
 * KeyObject.cpp
 *
 *  Created on: Feb 19, 2024
 *      Author: lion
 */

#include "encoding.h"
#include "ifs/crypto.h"
#include "KeyObject.h"
#include "Buffer.h"

namespace fibjs {

static result_t _createSecretKey(const unsigned char* key, size_t size, obj_ptr<KeyObject_base>& retVal)
{
    obj_ptr<KeyObject> keyObj = new KeyObject();
    result_t hr = keyObj->createSecretKey(key, size);
    if (hr < 0)
        return hr;

    retVal = keyObj;
    return 0;
}

result_t crypto_base::createSecretKey(Buffer_base* key, exlib::string encoding, obj_ptr<KeyObject_base>& retVal)
{
    Buffer* key_buf = Buffer::Cast(key);
    return _createSecretKey((const unsigned char*)key_buf->data(), key_buf->length(), retVal);
}

result_t crypto_base::createSecretKey(exlib::string key, exlib::string encoding, obj_ptr<KeyObject_base>& retVal)
{
    result_t hr = commonDecode(encoding, key, key);
    if (hr < 0)
        return hr;
    return _createSecretKey((const unsigned char*)key.c_str(), key.length(), retVal);
}

result_t KeyObject::createSecretKey(const unsigned char* key, size_t size)
{
    m_keyType = kKeyTypeSecret;
    m_key.assign(key, key + size);

    return 0;
}

result_t KeyObject::get_asymmetricKeyDetails(v8::Local<v8::Object>& retVal)
{
    if (m_keyType == kKeyTypeSecret)
        return CALL_RETURN_UNDEFINED;

    retVal = v8::Object::New(holder()->m_isolate);

    return 0;
}

result_t KeyObject::get_asymmetricKeyType(exlib::string& retVal)
{
    if (m_keyType == kKeyTypeSecret)
        return CALL_RETURN_UNDEFINED;

    retVal = "pkey";

    return 0;
}

result_t KeyObject::get_symmetricKeySize(int32_t& retVal)
{
    retVal = m_key.size();
    return 0;
}

result_t KeyObject::get_type(exlib::string& retVal)
{
    switch (m_keyType) {
    case kKeyTypeSecret:
        retVal = "secret";
        break;
    case kKeyTypePublic:
        retVal = "public";
        break;
    case kKeyTypePrivate:
        retVal = "private";
        break;
    }

    return 0;
}

result_t KeyObject::_export(v8::Local<v8::Object> options, v8::Local<v8::Value>& retVal)
{
    result_t hr;
    Isolate* isolate = holder();
    v8::Local<v8::Context> context = isolate->context();

    if (m_keyType == kKeyTypeSecret) {
        exlib::string format = "buffer";
        hr = GetConfigValue(isolate, options, "format", format, true);
        if (hr < 0 && hr != CALL_E_PARAMNOTOPTIONAL)
            return hr;

        if (format == "jwk") {
            v8::Local<v8::Object> jwk = v8::Object::New(isolate->m_isolate);
            exlib::string oct;

            base64Encode((const char*)m_key.data(), m_key.size(), true, oct);
            jwk->Set(context, isolate->NewString("kty", 3), isolate->NewString("oct", 3)).FromJust();
            jwk->Set(context, isolate->NewString("k", 1), isolate->NewString(oct)).FromJust();

            retVal = jwk;

            return 0;
        }

        if (format == "buffer") {
            obj_ptr<Buffer_base> buf = new Buffer(m_key.data(), m_key.size());
            retVal = buf->wrap(holder());
            return 0;
        }

        return CHECK_ERROR(Runtime::setError("The property 'options.format' must be one of: undefined, 'buffer', 'jwk'."));
    } else {
    }

    return 0;
}

result_t KeyObject::equals(KeyObject_base* otherKey, bool& retVal)
{
    KeyObject* other = (KeyObject*)otherKey;

    if (m_keyType != other->m_keyType)
        retVal = false;
    else if (m_key.size() != other->m_key.size())
        retVal = false;
    else
        retVal = memcmp(m_key.data(), other->m_key.data(), m_key.size()) == 0;

    return 0;
}

}
