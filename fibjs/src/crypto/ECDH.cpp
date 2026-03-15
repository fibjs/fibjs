/*
 * ECDH.cpp
 *
 *  Created on: Oct 17, 2025
 *      Author: lion
 */

#include "object.h"
#include "ifs/util.h"
#include "ifs/crypto.h"
#include "crypto_util.h"
#include "ECDH.h"
#include "Buffer.h"
#include "encoding.h"
#include "SimpleObject.h"
#include <string.h>
#include <openssl/ec.h>
#include <openssl/bn.h>
#include <openssl/obj_mac.h>

namespace fibjs {

result_t crypto_base::createECDH(exlib::string curve, obj_ptr<ECDH_base>& retVal)
{
    // Check for undefined/null curve parameter
    if (curve.empty())
        return Runtime::setError("The \"curve\" argument must be of type string. Received undefined");

    int cid = GetCurveFromName(curve.c_str());
    if (cid == NID_undef)
        return Runtime::setError("Invalid EC curve name");

    retVal = new ECDH(curve);
    return 0;
}

// Helper function to ensure EC key is created and ready
result_t ECDH::ensureKey()
{
    if (!m_ec) {
        int cid = GetCurveFromName(m_curve.c_str());
        if (cid == NID_undef)
            return Runtime::setError("Invalid curve name: '%s'.", m_curve.c_str());

        m_ec = EC_KEY_new_by_curve_name(cid);
        if (!m_ec)
            return openssl_error();

        m_keysGenerated = false;
        m_publicKeySet = false;
        m_privateKeySet = false;
    }
    return 0;
}

// Helper function to validate point format
result_t ECDH::validatePointFormat(const exlib::string& format)
{
    if (format.empty() || format == "uncompressed") {
        return 0; // Default format is uncompressed
    }
    if (format == "compressed" || format == "hybrid") {
        return 0; // Valid formats
    }

    return Runtime::setError("Invalid ECDH format: " + format);
}

result_t ECDH::generateKeys(exlib::string outputEncoding, exlib::string format, v8::Local<v8::Value>& retVal)
{
    result_t hr = ensureKey();
    if (hr < 0)
        return hr;

    hr = validatePointFormat(format);
    if (hr < 0)
        return hr;

    // Generate new key pair
    if (EC_KEY_generate_key(m_ec) != 1)
        return openssl_error();

    m_keysGenerated = true;
    m_privateKeySet = true;
    m_publicKeySet = false; // Public key is generated with private key

    // Return the public key in the requested format
    return getPublicKey(outputEncoding, format, retVal);
}

result_t ECDH::getPrivateKey(exlib::string encoding, v8::Local<v8::Value>& retVal)
{
    if (!m_ec || !m_keysGenerated || !m_privateKeySet)
        return Runtime::setError("Failed to get ECDH private key");

    const BIGNUM* priv_key = EC_KEY_get0_private_key(m_ec);
    if (!priv_key)
        return Runtime::setError("Failed to get ECDH private key");

    // Use the curve order size to determine the fixed-length output,
    // so that leading zeros are preserved (matching Node.js behavior).
    const EC_GROUP* group = EC_KEY_get0_group(m_ec);
    if (!group)
        return Runtime::setError("Failed to get ECDH private key");

    BIGNUM* order = BN_new();
    if (!order)
        return Runtime::setError("Failed to get ECDH private key");

    if (!EC_GROUP_get_order(group, order, NULL)) {
        BN_free(order);
        return Runtime::setError("Failed to get ECDH private key");
    }

    int key_len = BN_num_bytes(order);
    BN_free(order);

    obj_ptr<Buffer> key_buf = new Buffer(NULL, key_len);
    BN_bn2binpad(priv_key, (unsigned char*)key_buf->data(), key_len);

    return key_buf->toValue(encoding, retVal);
}

result_t ECDH::getPublicKey(exlib::string encoding, exlib::string format, v8::Local<v8::Value>& retVal)
{
    if (!m_ec || (!m_keysGenerated && !m_publicKeySet))
        return Runtime::setError("Failed to get ECDH public key");

    result_t hr = validatePointFormat(format);
    if (hr < 0)
        return hr;

    const EC_POINT* point = EC_KEY_get0_public_key(m_ec);
    if (!point)
        return Runtime::setError("Failed to get ECDH public key");

    const EC_GROUP* group = EC_KEY_get0_group(m_ec);
    if (!group)
        return Runtime::setError("Failed to get ECDH public key");

    // Determine point conversion format
    point_conversion_form_t form = POINT_CONVERSION_UNCOMPRESSED;
    if (format == "compressed")
        form = POINT_CONVERSION_COMPRESSED;
    else if (format == "hybrid")
        form = POINT_CONVERSION_HYBRID;

    // Get the size needed for the public key
    size_t pub_key_len = EC_POINT_point2oct(group, point, form, nullptr, 0, nullptr);
    if (pub_key_len == 0)
        return openssl_error();

    // Convert point to octets
    obj_ptr<Buffer> pub_key_buf = new Buffer(nullptr, pub_key_len);
    if (EC_POINT_point2oct(group, point, form, (unsigned char*)pub_key_buf->data(), pub_key_len, nullptr) != pub_key_len)
        return openssl_error();

    return pub_key_buf->toValue(encoding, retVal);
}

result_t ECDH::setPrivateKey(v8::Local<v8::Value> privateKey, exlib::string encoding)
{
    result_t hr = ensureKey();
    if (hr < 0)
        return hr;

    // Use GetArgumentValue to handle encoding
    Isolate* isolate = holder();
    obj_ptr<Buffer_base> key_buffer;
    hr = GetArgumentValue(isolate, privateKey, key_buffer, false, encoding.c_str());
    if (hr < 0)
        return hr;

    Buffer* key_buf = Buffer::Cast(key_buffer);
    if (!key_buf)
        return Runtime::setError("Invalid buffer type");

    // Convert buffer to BIGNUM
    BignumPointer priv_bn = BN_bin2bn((const unsigned char*)key_buf->data(), key_buf->length(), nullptr);
    if (!priv_bn)
        return openssl_error();

    // Get the curve order for validation
    const EC_GROUP* group = EC_KEY_get0_group(m_ec);
    BignumPointer order = BN_new();
    if (!EC_GROUP_get_order(group, order, nullptr))
        return openssl_error();

    // Validate private key: must be in range [1, order-1]
    if (BN_is_zero(priv_bn) || BN_cmp(priv_bn, order) >= 0)
        return Runtime::setError("Private key is not valid for specified curve");

    // Set the private key
    if (EC_KEY_set_private_key(m_ec, priv_bn) != 1)
        return openssl_error();

    // Generate corresponding public key
    const EC_GROUP* ec_group = EC_KEY_get0_group(m_ec);
    ECPointPointer pub_point = EC_POINT_new(ec_group);
    if (!pub_point)
        return openssl_error();

    if (EC_POINT_mul(ec_group, pub_point, priv_bn, nullptr, nullptr, nullptr) != 1)
        return openssl_error();

    if (EC_KEY_set_public_key(m_ec, pub_point) != 1)
        return openssl_error();

    m_keysGenerated = true;
    m_privateKeySet = true;
    m_publicKeySet = false; // Public key generated from private key

    return 0;
}

result_t ECDH::setPublicKey(v8::Local<v8::Value> publicKey, exlib::string encoding)
{
    result_t hr = ensureKey();
    if (hr < 0)
        return hr;

    // Use GetArgumentValue to handle encoding
    Isolate* isolate = holder();
    obj_ptr<Buffer_base> key_buffer;
    hr = GetArgumentValue(isolate, publicKey, key_buffer, false, encoding.c_str());
    if (hr < 0)
        return hr;

    Buffer* key_buf = Buffer::Cast(key_buffer);
    if (!key_buf)
        return Runtime::setError("Invalid buffer type");

    const EC_GROUP* group = EC_KEY_get0_group(m_ec);
    ECPointPointer point = EC_POINT_new(group);
    if (!point)
        return openssl_error();

    // Convert octets to point
    if (EC_POINT_oct2point(group, point, key_buf->data(), key_buf->length(), nullptr) != 1)
        return Runtime::setError("Failed to convert Buffer to EC_POINT");

    // Validate the point is on the curve
    if (EC_POINT_is_on_curve(group, point, nullptr) != 1)
        return Runtime::setError("Public key is not valid for specified curve");

    if (EC_KEY_set_public_key(m_ec, point) != 1)
        return openssl_error();

    m_publicKeySet = true;

    return 0;
}

result_t ECDH::computeSecret(v8::Local<v8::Value> otherKey, exlib::string inputEncoding, exlib::string outputEncoding,
    v8::Local<v8::Value>& retVal)
{
    if (!m_ec || !m_privateKeySet)
        return Runtime::setError("Private key not set");

    // Check if we have a public key set that doesn't match the private key
    if (m_publicKeySet) {
        // This means setPublicKey was called after setPrivateKey, which breaks the key pair
        return Runtime::setError("Invalid key pair");
    }

    // Use GetArgumentValue to handle input encoding
    Isolate* isolate = holder();
    obj_ptr<Buffer_base> key_buffer;
    result_t hr = GetArgumentValue(isolate, otherKey, key_buffer, false, inputEncoding.c_str());
    if (hr < 0)
        return hr;

    Buffer* other_pub_buf = Buffer::Cast(key_buffer);
    if (!other_pub_buf)
        return Runtime::setError("Invalid buffer type");

    const EC_GROUP* group = EC_KEY_get0_group(m_ec);
    ECPointPointer other_point = EC_POINT_new(group);
    if (!other_point)
        return openssl_error();

    // Convert other party's public key to point
    if (EC_POINT_oct2point(group, other_point, (const unsigned char*)other_pub_buf->data(), other_pub_buf->length(), nullptr) != 1)
        return Runtime::setError("Public key is not valid for specified curve");

    // Validate the other party's public key is on the same curve
    if (EC_POINT_is_on_curve(group, other_point, nullptr) != 1)
        return Runtime::setError("Public key is not valid for specified curve");

    // Get field size to determine shared secret length
    int field_size = EC_GROUP_get_degree(group);
    int secret_len = (field_size + 7) / 8;

    // Compute shared secret
    obj_ptr<Buffer> secret_buf = new Buffer(nullptr, secret_len);
    int computed_len = ECDH_compute_key((unsigned char*)secret_buf->data(), secret_len, other_point, m_ec, nullptr);
    if (computed_len <= 0)
        return openssl_error();

    // Trim the buffer to actual computed length if needed
    if (computed_len < secret_len) {
        obj_ptr<Buffer> trimmed_buf = new Buffer(secret_buf->data(), computed_len);
        secret_buf = trimmed_buf;
    }

    return secret_buf->toValue(outputEncoding, retVal);
}

result_t ECDH_base::convertKey(v8::Local<v8::Value> key, exlib::string curve, exlib::string inputEncoding,
    exlib::string outputEncoding, exlib::string format, v8::Local<v8::Value>& retVal)
{
    if (key.IsEmpty())
        return Runtime::setError("The \"key\" argument must be of type string. Received undefined");

    if (curve.empty())
        return Runtime::setError("The \"curve\" argument must be of type string. Received undefined");

    int cid = GetCurveFromName(curve.c_str());
    if (cid == NID_undef)
        return Runtime::setError("Invalid EC curve name: '%s'.", curve.c_str());

    // Validate format
    if (!format.empty() && format != "uncompressed" && format != "compressed" && format != "hybrid")
        return Runtime::setError("Invalid ECDH format: " + format);

    // Create temporary ECDH object to use helper functions
    ECDH temp_ecdh(curve);
    result_t hr = temp_ecdh.ensureKey();
    if (hr < 0)
        return hr;

    // Use GetArgumentValue to handle input encoding
    Isolate* isolate = Isolate::current();
    obj_ptr<Buffer_base> key_buffer;
    hr = GetArgumentValue(isolate, key, key_buffer, false, inputEncoding.c_str());
    if (hr < 0)
        return hr;

    Buffer* key_buf = Buffer::Cast(key_buffer);
    if (!key_buf)
        return Runtime::setError("Invalid buffer type");

    // Get EC_GROUP through a public method
    const EC_GROUP* group = temp_ecdh.getGroup();
    ECPointPointer point = EC_POINT_new(group);
    if (!point)
        return openssl_error();

    // Convert input key to point
    if (EC_POINT_oct2point(group, point, key_buf->data(), key_buf->length(), nullptr) != 1)
        return Runtime::setError("Failed to convert Buffer to EC_POINT");

    // Determine output point format
    point_conversion_form_t form = POINT_CONVERSION_UNCOMPRESSED;
    if (format == "compressed")
        form = POINT_CONVERSION_COMPRESSED;
    else if (format == "hybrid")
        form = POINT_CONVERSION_HYBRID;

    // Convert point back to octets in requested format
    size_t out_len = EC_POINT_point2oct(group, point, form, nullptr, 0, nullptr);
    if (out_len == 0)
        return openssl_error();

    obj_ptr<Buffer> out_buf = new Buffer(nullptr, out_len);
    if (EC_POINT_point2oct(group, point, form, out_buf->data(), out_len, nullptr) != out_len)
        return openssl_error();

    return out_buf->toValue(outputEncoding, retVal);
}

result_t ECDH::get_curveName(exlib::string& retVal)
{
    retVal = m_curve;
    return 0;
}

}