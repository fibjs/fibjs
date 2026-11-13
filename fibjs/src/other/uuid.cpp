/*
 * uuidVar.cpp
 *
 *  Created on: Jul 6, 2012
 *      Author: lion
 */

#include "object.h"
#include "Buffer.h"
#include "ifs/uuid.h"
#include "ifs/coroutine.h"
#include <stdlib.h>
#include <chrono>
#include <random>
#include <cstring>
#include <openssl/rand.h>
#include <openssl/md5.h>
#include <openssl/sha.h>

#ifdef _WIN32
#include <windows.h>
#endif

namespace fibjs {

static bool uuid_option_bytes(const std::variant<obj_ptr<Buffer_base>, v8::Local<v8::Array>>& value,
    uint8_t* out, size_t size)
{
    if (std::holds_alternative<obj_ptr<Buffer_base>>(value)) {
        Buffer* p = Buffer::Cast(std::get<obj_ptr<Buffer_base>>(value).get());
        if (p->length() < (int32_t)size)
            return false;

        memcpy(out, p->data(), size);
        return true;
    }

    v8::Local<v8::Array> array = std::get<v8::Local<v8::Array>>(value);
    if (array->Length() < size)
        return false;

    Isolate* isolate = Isolate::current();
    v8::Local<v8::Context> context = isolate->context();

    for (uint32_t i = 0; i < size; i++) {
        v8::Local<v8::Value> v;
        if (!array->Get(context, i).ToLocal(&v) || !v->IsNumber())
            return false;

        out[i] = (uint8_t)(v->Int32Value(context).FromMaybe(0) & 0xFF);
    }

    return true;
}

DECLARE_MODULE(uuid);

// UUID epoch offset: 100-nanosecond intervals from Oct 15, 1582 to Jan 1, 1970
static const uint64_t GREGORIAN_OFFSET = 0x01B21DD213814000ULL;

// UUID namespace strings for v3 and v5
static const char* s_ns_str[4] = { "ns:DNS", "ns:URL", "ns:OID", "ns:X500" };
static uint8_t s_ns_uuid[4][16];
static bool s_ns_initialized = false;

// Initialize predefined namespace UUIDs
static void init_namespaces()
{
    if (s_ns_initialized)
        return;

    // DNS namespace: 6ba7b810-9dad-11d1-80b4-00c04fd430c8
    uint8_t dns[] = { 0x6b, 0xa7, 0xb8, 0x10, 0x9d, 0xad, 0x11, 0xd1, 0x80, 0xb4, 0x00, 0xc0, 0x4f, 0xd4, 0x30, 0xc8 };
    memcpy(s_ns_uuid[0], dns, 16);

    // URL namespace: 6ba7b811-9dad-11d1-80b4-00c04fd430c8
    uint8_t url[] = { 0x6b, 0xa7, 0xb8, 0x11, 0x9d, 0xad, 0x11, 0xd1, 0x80, 0xb4, 0x00, 0xc0, 0x4f, 0xd4, 0x30, 0xc8 };
    memcpy(s_ns_uuid[1], url, 16);

    // OID namespace: 6ba7b812-9dad-11d1-80b4-00c04fd430c8
    uint8_t oid[] = { 0x6b, 0xa7, 0xb8, 0x12, 0x9d, 0xad, 0x11, 0xd1, 0x80, 0xb4, 0x00, 0xc0, 0x4f, 0xd4, 0x30, 0xc8 };
    memcpy(s_ns_uuid[2], oid, 16);

    // X500 namespace: 6ba7b814-9dad-11d1-80b4-00c04fd430c8
    uint8_t x500[] = { 0x6b, 0xa7, 0xb8, 0x14, 0x9d, 0xad, 0x11, 0xd1, 0x80, 0xb4, 0x00, 0xc0, 0x4f, 0xd4, 0x30, 0xc8 };
    memcpy(s_ns_uuid[3], x500, 16);

    s_ns_initialized = true;
}

// Generate random bytes using OpenSSL
static void get_random_bytes(uint8_t* buffer, size_t size)
{
    if (RAND_bytes(buffer, size) != 1) {
        // Fallback to system random if OpenSSL fails
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(0, 255);
        for (size_t i = 0; i < size; i++) {
            buffer[i] = dis(gen);
        }
    }
}

// Get high-precision timestamp for UUID v1
static uint64_t get_uuid_timestamp()
{
    // UUID timestamp is 100-nanosecond intervals since Oct 15, 1582
    auto now = std::chrono::system_clock::now();
    auto duration = now.time_since_epoch();
    auto microseconds = std::chrono::duration_cast<std::chrono::microseconds>(duration).count();

    // Convert to 100-nanosecond intervals and add UUID epoch offset
    // UUID epoch starts at Oct 15, 1582, Unix epoch starts at Jan 1, 1970
    // Difference is 122192928000000000 100-nanosecond intervals
    uint64_t uuid_time = (microseconds * 10) + GREGORIAN_OFFSET;

#ifdef _WIN32
    // Add high-resolution counter for better precision on Windows
    static LARGE_INTEGER frequency = { 0 };
    LARGE_INTEGER counter;

    if (frequency.QuadPart == 0) {
        QueryPerformanceFrequency(&frequency);
    }
    QueryPerformanceCounter(&counter);

    // Add some precision from performance counter
    uuid_time += (counter.QuadPart % 10000);
#endif

    return uuid_time;
}

// Convert UUID binary to string
static void uuid_to_string(const uint8_t* uuid, exlib::string& str)
{
    char buf[37];
    snprintf(buf, sizeof(buf),
        "%02x%02x%02x%02x-%02x%02x-%02x%02x-%02x%02x-%02x%02x%02x%02x%02x%02x",
        uuid[0], uuid[1], uuid[2], uuid[3],
        uuid[4], uuid[5], uuid[6], uuid[7],
        uuid[8], uuid[9], uuid[10], uuid[11],
        uuid[12], uuid[13], uuid[14], uuid[15]);
    str = buf;
}

// Parse UUID string to binary
static bool uuid_from_string(const char* str, uint8_t* uuid)
{
    if (strlen(str) != 36)
        return false;

    // Check format
    if (str[8] != '-' || str[13] != '-' || str[18] != '-' || str[23] != '-') {
        return false;
    }

    // Parse hex bytes
    int pos = 0;
    for (int i = 0; i < 36; i++) {
        if (i == 8 || i == 13 || i == 18 || i == 23)
            continue;

        char c1 = str[i];
        char c2 = str[i + 1];

        int n1 = (c1 >= '0' && c1 <= '9') ? (c1 - '0') : (c1 >= 'a' && c1 <= 'f') ? (c1 - 'a' + 10)
            : (c1 >= 'A' && c1 <= 'F')                                            ? (c1 - 'A' + 10)
                                                                                  : -1;
        int n2 = (c2 >= '0' && c2 <= '9') ? (c2 - '0') : (c2 >= 'a' && c2 <= 'f') ? (c2 - 'a' + 10)
            : (c2 >= 'A' && c2 <= 'F')                                            ? (c2 - 'A' + 10)
                                                                                  : -1;

        if (n1 < 0 || n2 < 0)
            return false;

        uuid[pos++] = (n1 << 4) | n2;
        i++; // Skip second hex char
    }

    return pos == 16;
}

inline result_t toBuffer(const uint8_t* uuid_bytes, obj_ptr<Buffer_base>& retVal)
{
    retVal = new Buffer(uuid_bytes, 16);
    return result_t();
}

result_t makeUUID_v1(obj_ptr<Buffer_base>& retVal)
{
    uint8_t uuid[16];
    static uint64_t last_timestamp = 0;
    static uint16_t clock_seq = 0;
    static bool initialized = false;
    static uint8_t node_id[6];

    if (!initialized) {
        // Generate random node ID (MAC address alternative)
        get_random_bytes(node_id, 6);
        node_id[0] |= 0x01; // Set multicast bit to indicate random node ID

        // Initialize clock sequence
        get_random_bytes((uint8_t*)&clock_seq, sizeof(clock_seq));
        clock_seq &= 0x3FFF; // 14-bit clock sequence

        initialized = true;
    }

    uint64_t timestamp = get_uuid_timestamp();

    // Ensure monotonic timestamp
    if (timestamp <= last_timestamp) {
        timestamp = last_timestamp + 1;
        clock_seq = (clock_seq + 1) & 0x3FFF;
    }
    last_timestamp = timestamp;

    // Pack UUID v1 format
    // Time low (32 bits)
    uuid[0] = (timestamp >> 24) & 0xFF;
    uuid[1] = (timestamp >> 16) & 0xFF;
    uuid[2] = (timestamp >> 8) & 0xFF;
    uuid[3] = timestamp & 0xFF;

    // Time mid (16 bits)
    uuid[4] = (timestamp >> 40) & 0xFF;
    uuid[5] = (timestamp >> 32) & 0xFF;

    // Time high and version (16 bits)
    uuid[6] = ((timestamp >> 56) & 0x0F) | 0x10; // Version 1
    uuid[7] = (timestamp >> 48) & 0xFF;

    // Clock sequence and variant (16 bits)
    uuid[8] = ((clock_seq >> 8) & 0x3F) | 0x80; // Variant bits
    uuid[9] = clock_seq & 0xFF;

    // Node (48 bits)
    memcpy(&uuid[10], node_id, 6);

    return toBuffer(uuid, retVal);
}

result_t makeUUID_v3(int32_t ns, const exlib::string& name, obj_ptr<Buffer_base>& retVal)
{
    init_namespaces();

    if (ns < 0 || ns > 3) {
        return result_t::invalid_arg();
    }

    // Create MD5 hash of namespace UUID + name
    MD5_CTX ctx;
    MD5_Init(&ctx);
    MD5_Update(&ctx, s_ns_uuid[ns], 16);
    MD5_Update(&ctx, name.c_str(), name.length());

    uint8_t hash[MD5_DIGEST_LENGTH];
    MD5_Final(hash, &ctx);

    // Convert hash to UUID v3 format
    uint8_t uuid[16];
    memcpy(uuid, hash, 16);

    // Set version (3) and variant bits according to RFC 4122
    uuid[6] = (uuid[6] & 0x0F) | 0x30; // Version 3
    uuid[8] = (uuid[8] & 0x3F) | 0x80; // Variant bits

    return toBuffer(uuid, retVal);
}

result_t makeUUID_v5(int32_t ns, const exlib::string& name, obj_ptr<Buffer_base>& retVal)
{
    init_namespaces();

    if (ns < 0 || ns > 3) {
        return result_t::invalid_arg();
    }

    // Create SHA1 hash of namespace UUID + name
    SHA_CTX ctx;
    SHA1_Init(&ctx);
    SHA1_Update(&ctx, s_ns_uuid[ns], 16);
    SHA1_Update(&ctx, name.c_str(), name.length());

    uint8_t hash[SHA_DIGEST_LENGTH];
    SHA1_Final(hash, &ctx);

    // Convert hash to UUID v5 format (use first 16 bytes of 20-byte SHA1)
    uint8_t uuid[16];
    memcpy(uuid, hash, 16);

    // Set version (5) and variant bits according to RFC 4122
    uuid[6] = (uuid[6] & 0x0F) | 0x50; // Version 5
    uuid[8] = (uuid[8] & 0x3F) | 0x80; // Variant bits

    return toBuffer(uuid, retVal);
}

result_t makeUUID_v4(obj_ptr<Buffer_base>& retVal)
{
    uint8_t uuid[16];

    // Generate random bytes
    get_random_bytes(uuid, 16);

    // Set version (4) and variant bits according to RFC 4122
    uuid[6] = (uuid[6] & 0x0F) | 0x40; // Version 4
    uuid[8] = (uuid[8] & 0x3F) | 0x80; // Variant bits

    return toBuffer(uuid, retVal);
}

result_t makeUUID(int32_t vers, int32_t ns, exlib::string name,
    obj_ptr<Buffer_base>& retVal)
{
    if (ns < uuid_base::C_DNS || ns > uuid_base::C_X509)
        return result_t::invalid_arg();

    switch (vers) {
    case 1: // UUID_MAKE_V1
        return makeUUID_v1(retVal);

    case 3: // UUID_MAKE_V3 (MD5)
        return makeUUID_v3(ns, name, retVal);

    case 4: // UUID_MAKE_V4
        return makeUUID_v4(retVal);

    case 5: // UUID_MAKE_V5 (SHA1)
        return makeUUID_v5(ns, name, retVal);

    default:
        return makeUUID_v4(retVal);
    }
}

result_t uuid_base::parse(exlib::string uuid, obj_ptr<Buffer_base>& retVal)
{
    uint8_t uuid_bytes[16];

    if (!uuid_from_string(uuid.c_str(), uuid_bytes)) {
        return result_t::invalid_arg();
    }

    return toBuffer(uuid_bytes, retVal);
}

result_t uuid_base::validate(exlib::string uuid, bool& retVal)
{
    uint8_t uuid_bytes[16];
    retVal = uuid_from_string(uuid.c_str(), uuid_bytes);
    return result_t();
}

result_t uuid_base::stringify(Union_stringify_arr arr, int32_t offset, exlib::string& retVal)
{
    exlib::string data;

    if (std::holds_alternative<exlib::string>(arr))
        data = std::get<exlib::string>(arr);
    else
        std::get<obj_ptr<Buffer_base>>(arr)->toString(data);

    if ((size_t)(offset + 16) > data.length())
        return result_t::invalid_data();

    const uint8_t* uuid = (const uint8_t*)data.c_str() + offset;
    uuid_to_string(uuid, retVal);
    return result_t();
}

result_t uuid_base::v1(UuidV1Options* options, exlib::string& retVal)
{
    uint8_t uuid[16];
    static uint64_t last_timestamp = 0;
    static uint16_t clock_seq = 0;
    static bool initialized = false;
    static uint8_t node_id[6];

    if (!initialized) {
        // Generate random node ID (MAC address alternative)
        get_random_bytes(node_id, 6);
        node_id[0] |= 0x01; // Set multicast bit to indicate random node ID

        // Initialize clock sequence
        get_random_bytes((uint8_t*)&clock_seq, sizeof(clock_seq));
        clock_seq &= 0x3FFF; // 14-bit clock sequence

        initialized = true;
    }

    bool has_clockseq = false;
    uint64_t custom_msecs = 0;
    uint64_t custom_nsecs = 0;
    bool has_custom_time = false;

    if (options) {
        if (options->node.has_value())
            uuid_option_bytes(options->node.value(), node_id, 6);

        if (options->clockseq.has_value()) {
            clock_seq = options->clockseq.value() & 0x3FFF;
            has_clockseq = true;
        }

        if (options->msecs.has_value()) {
            custom_msecs = options->msecs.value();
            has_custom_time = true;
        }

        if (options->nsecs.has_value())
            custom_nsecs = options->nsecs.value();
    }

    uint64_t timestamp;
    if (has_custom_time) {
        // Convert msecs to UUID timestamp (100-nanosecond intervals since Oct 15, 1582)
        timestamp = (custom_msecs * 10000ULL) + GREGORIAN_OFFSET;
        if (custom_nsecs > 0) {
            timestamp += custom_nsecs / 100;
        }
    } else {
        timestamp = get_uuid_timestamp();
    }

    // Ensure monotonic timestamp if not using custom values
    if (!has_custom_time && !has_clockseq) {
        if (timestamp <= last_timestamp) {
            timestamp = last_timestamp + 1;
            clock_seq = (clock_seq + 1) & 0x3FFF;
        }
        last_timestamp = timestamp;
    }

    // Pack UUID v1 format
    // Time low (32 bits)
    uuid[0] = (timestamp >> 24) & 0xFF;
    uuid[1] = (timestamp >> 16) & 0xFF;
    uuid[2] = (timestamp >> 8) & 0xFF;
    uuid[3] = timestamp & 0xFF;

    // Time mid (16 bits)
    uuid[4] = (timestamp >> 40) & 0xFF;
    uuid[5] = (timestamp >> 32) & 0xFF;

    // Time high and version (16 bits)
    uuid[6] = ((timestamp >> 56) & 0x0F) | 0x10; // Version 1
    uuid[7] = (timestamp >> 48) & 0xFF;

    // Clock sequence and variant (16 bits)
    uuid[8] = ((clock_seq >> 8) & 0x3F) | 0x80; // Variant bits
    uuid[9] = clock_seq & 0xFF;

    // Node (48 bits)
    memcpy(&uuid[10], node_id, 6);

    uuid_to_string(uuid, retVal);
    return result_t();
}

result_t uuid_base::v4(UuidV4Options* options, exlib::string& retVal)
{
    uint8_t uuid[16];

    bool has_random = false;
    if (options && options->random.has_value())
        has_random = uuid_option_bytes(options->random.value(), uuid, 16);
    
    if (!has_random) {
        // Generate random bytes
        get_random_bytes(uuid, 16);
    }

    // Set version (4) and variant bits according to RFC 4122
    uuid[6] = (uuid[6] & 0x0F) | 0x40; // Version 4
    uuid[8] = (uuid[8] & 0x3F) | 0x80; // Variant bits

    uuid_to_string(uuid, retVal);
    return result_t();
}

result_t uuid_base::v3(exlib::string name, Union_v3_ns ns, exlib::string& retVal)
{
    uint8_t ns_bytes[16];
    const unsigned char* ns_data;

    if (std::holds_alternative<exlib::string>(ns)) {
        init_namespaces();

        if (!uuid_from_string(std::get<exlib::string>(ns).c_str(), ns_bytes))
            return result_t::invalid_arg();

        ns_data = ns_bytes;
    } else {
        Buffer* p = Buffer::Cast(std::get<obj_ptr<Buffer_base>>(ns));
        if (p->length() < 16)
            return result_t::invalid_arg();

        ns_data = p->data();
    }

    // Create MD5 hash of namespace UUID + name
    MD5_CTX ctx;
    MD5_Init(&ctx);
    MD5_Update(&ctx, ns_data, 16);
    MD5_Update(&ctx, name.c_str(), name.length());

    uint8_t hash[MD5_DIGEST_LENGTH];
    MD5_Final(hash, &ctx);

    // Convert hash to UUID v3 format
    uint8_t uuid[16];
    memcpy(uuid, hash, 16);

    // Set version (3) and variant bits according to RFC 4122
    uuid[6] = (uuid[6] & 0x0F) | 0x30; // Version 3
    uuid[8] = (uuid[8] & 0x3F) | 0x80; // Variant bits

    uuid_to_string(uuid, retVal);
    return result_t();
}


result_t uuid_base::v5(exlib::string name, Union_v5_ns ns, exlib::string& retVal)
{
    uint8_t ns_bytes[16];
    const unsigned char* ns_data;

    if (std::holds_alternative<exlib::string>(ns)) {
        init_namespaces();

        if (!uuid_from_string(std::get<exlib::string>(ns).c_str(), ns_bytes))
            return result_t::invalid_arg();

        ns_data = ns_bytes;
    } else {
        Buffer* p = Buffer::Cast(std::get<obj_ptr<Buffer_base>>(ns));
        if (p->length() < 16)
            return result_t::invalid_arg();

        ns_data = p->data();
    }

    // Create SHA1 hash of namespace UUID + name
    SHA_CTX ctx;
    SHA1_Init(&ctx);
    SHA1_Update(&ctx, ns_data, 16);
    SHA1_Update(&ctx, name.c_str(), name.length());

    uint8_t hash[SHA_DIGEST_LENGTH];
    SHA1_Final(hash, &ctx);

    // Convert hash to UUID v5 format (use first 16 bytes of 20-byte SHA1)
    uint8_t uuid[16];
    memcpy(uuid, hash, 16);

    // Set version (5) and variant bits according to RFC 4122
    uuid[6] = (uuid[6] & 0x0F) | 0x50; // Version 5
    uuid[8] = (uuid[8] & 0x3F) | 0x80; // Variant bits

    uuid_to_string(uuid, retVal);
    return result_t();
}


result_t uuid_base::version(exlib::string uuid, int32_t& retVal)
{
    uint8_t uuid_bytes[16];
    if (!uuid_from_string(uuid.c_str(), uuid_bytes)) {
        return result_t::invalid_arg();
    }
    
    retVal = (uuid_bytes[6] >> 4) & 0x0F;
    return result_t();
}

result_t uuid_base::v6(UuidV1Options* options, exlib::string& retVal)
{
    uint8_t uuid[16];
    static uint64_t last_timestamp = 0;
    static uint16_t clock_seq = 0;
    static bool initialized = false;
    static uint8_t node_id[6];

    if (!initialized) {
        get_random_bytes(node_id, 6);
        node_id[0] |= 0x01;
        get_random_bytes((uint8_t*)&clock_seq, sizeof(clock_seq));
        clock_seq &= 0x3FFF;
        initialized = true;
    }

    bool has_clockseq = false;
    uint64_t custom_msecs = 0;
    uint64_t custom_nsecs = 0;
    bool has_custom_time = false;

    if (options) {
        if (options->node.has_value())
            uuid_option_bytes(options->node.value(), node_id, 6);

        if (options->clockseq.has_value()) {
            clock_seq = options->clockseq.value() & 0x3FFF;
            has_clockseq = true;
        }

        if (options->msecs.has_value()) {
            custom_msecs = options->msecs.value();
            has_custom_time = true;
        }

        if (options->nsecs.has_value())
            custom_nsecs = options->nsecs.value();
    }

    uint64_t timestamp;
    if (has_custom_time) {
        timestamp = (custom_msecs * 10000ULL) + GREGORIAN_OFFSET;
        if (custom_nsecs > 0) {
            timestamp += custom_nsecs / 100;
        }
    } else {
        timestamp = get_uuid_timestamp();
    }

    if (!has_custom_time && !has_clockseq) {
        if (timestamp <= last_timestamp) {
            timestamp = last_timestamp + 1;
            clock_seq = (clock_seq + 1) & 0x3FFF;
        }
        last_timestamp = timestamp;
    }

    // UUID v6 format - reordered timestamp for better sorting
    // High 32 bits of timestamp
    uuid[0] = (timestamp >> 52) & 0xFF;
    uuid[1] = (timestamp >> 44) & 0xFF;
    uuid[2] = (timestamp >> 36) & 0xFF;
    uuid[3] = (timestamp >> 28) & 0xFF;

    // Mid 16 bits of timestamp
    uuid[4] = (timestamp >> 20) & 0xFF;
    uuid[5] = (timestamp >> 12) & 0xFF;

    // Version and low 12 bits of timestamp
    uuid[6] = ((timestamp >> 8) & 0x0F) | 0x60; // Version 6
    uuid[7] = timestamp & 0xFF;

    // Clock sequence and variant
    uuid[8] = ((clock_seq >> 8) & 0x3F) | 0x80;
    uuid[9] = clock_seq & 0xFF;

    // Node
    memcpy(&uuid[10], node_id, 6);

    uuid_to_string(uuid, retVal);
    return result_t();
}

result_t uuid_base::v7(UuidV7Options* options, exlib::string& retVal)
{
    uint8_t uuid[16];

    // Get Unix timestamp in milliseconds
    uint64_t timestamp_ms;

    if (options && options->msecs.has_value()) {
        timestamp_ms = options->msecs.value();
    } else {
        auto now = std::chrono::system_clock::now();
        timestamp_ms = std::chrono::duration_cast<std::chrono::milliseconds>(
            now.time_since_epoch()).count();
    }

    // 48-bit timestamp
    uuid[0] = (timestamp_ms >> 40) & 0xFF;
    uuid[1] = (timestamp_ms >> 32) & 0xFF;
    uuid[2] = (timestamp_ms >> 24) & 0xFF;
    uuid[3] = (timestamp_ms >> 16) & 0xFF;
    uuid[4] = (timestamp_ms >> 8) & 0xFF;
    uuid[5] = timestamp_ms & 0xFF;

    // Generate random data for the rest
    get_random_bytes(&uuid[6], 10);

    // Set version and variant
    uuid[6] = (uuid[6] & 0x0F) | 0x70; // Version 7
    uuid[8] = (uuid[8] & 0x3F) | 0x80; // Variant

    uuid_to_string(uuid, retVal);
    return result_t();
}

result_t uuid_base::v1ToV6(exlib::string uuid_v1, exlib::string& retVal)
{
    uint8_t v1_bytes[16];
    if (!uuid_from_string(uuid_v1.c_str(), v1_bytes)) {
        return result_t::invalid_arg();
    }

    // Check if it's actually a v1 UUID
    if (((v1_bytes[6] >> 4) & 0x0F) != 1) {
        return result_t::invalid_arg();
    }

    uint8_t v6_bytes[16];
    
    // Reorder timestamp fields from v1 to v6
    // v1: time_low(4) + time_mid(2) + time_hi_version(2)
    // v6: time_high(4) + time_mid(2) + time_low_version(2)
    
    v6_bytes[0] = v1_bytes[6] & 0x0F; // time_hi (low 4 bits)
    v6_bytes[0] |= (v1_bytes[7] & 0x0F) << 4;
    v6_bytes[1] = v1_bytes[4];
    v6_bytes[2] = v1_bytes[5];
    v6_bytes[3] = v1_bytes[0];
    
    v6_bytes[4] = v1_bytes[1];
    v6_bytes[5] = v1_bytes[2];
    
    v6_bytes[6] = (v1_bytes[3] & 0x0F) | 0x60; // Version 6
    v6_bytes[7] = (v1_bytes[6] & 0xF0) | (v1_bytes[7] >> 4);
    
    // Copy clock_seq and node unchanged
    memcpy(&v6_bytes[8], &v1_bytes[8], 8);
    
    uuid_to_string(v6_bytes, retVal);
    return result_t();
}

result_t uuid_base::v6ToV1(exlib::string uuid_v6, exlib::string& retVal)
{
    uint8_t v6_bytes[16];
    if (!uuid_from_string(uuid_v6.c_str(), v6_bytes)) {
        return result_t::invalid_arg();
    }

    // Check if it's actually a v6 UUID
    if (((v6_bytes[6] >> 4) & 0x0F) != 6) {
        return result_t::invalid_arg();
    }

    uint8_t v1_bytes[16];
    
    // Reorder timestamp fields from v6 to v1
    v1_bytes[0] = v6_bytes[3];
    v1_bytes[1] = v6_bytes[4];
    v1_bytes[2] = v6_bytes[5];
    v1_bytes[3] = (v6_bytes[6] & 0x0F) | ((v6_bytes[7] & 0x0F) << 4);
    
    v1_bytes[4] = v6_bytes[1];
    v1_bytes[5] = v6_bytes[2];
    
    v1_bytes[6] = ((v6_bytes[0] & 0x0F) | 0x10); // Version 1
    v1_bytes[7] = (v6_bytes[0] & 0xF0) | (v6_bytes[7] >> 4);
    
    // Copy clock_seq and node unchanged
    memcpy(&v1_bytes[8], &v6_bytes[8], 8);
    
    uuid_to_string(v1_bytes, retVal);
    return result_t();
}

result_t uuid_base::node(obj_ptr<Buffer_base>& retVal)
{
    return makeUUID(1, uuid_base::C_X509, "", retVal);
}

result_t uuid_base::md5(int32_t ns, exlib::string name,
    obj_ptr<Buffer_base>& retVal)
{
    return makeUUID(3, ns, name, retVal);
}

result_t uuid_base::random(obj_ptr<Buffer_base>& retVal)
{
    return makeUUID(4, uuid_base::C_X509, "", retVal);
}

result_t uuid_base::sha1(int32_t ns, exlib::string name,
    obj_ptr<Buffer_base>& retVal)
{
    return makeUUID(5, ns, name, retVal);
}

inline int64_t generateStamp()
{
    // Get current timestamp in milliseconds using chrono
    auto now = std::chrono::system_clock::now();
    auto timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(
        now.time_since_epoch())
                         .count();
    return timestamp;
}

result_t uuid_base::snowflake(obj_ptr<Buffer_base>& retVal)
{
    Isolate* isolate = Isolate::current();
    uint64_t tm = generateStamp();

    if (isolate->m_flake_tm != tm) {
        isolate->m_flake_tm = tm;
        isolate->m_flake_count = 0;
    } else if (isolate->m_flake_count == 0xfff) {
        uint64_t tm1 = tm;

        while (tm1 == tm) {
            coroutine_base::ac_sleep(0);
            tm = generateStamp();
        }

        if (isolate->m_flake_tm != tm) {
            isolate->m_flake_tm = tm;
            isolate->m_flake_count = 0;
        }
    }

    tm <<= 22;
    tm |= isolate->m_flake_host << 12;
    tm |= isolate->m_flake_count++;

    uint8_t buf[8];
    buf[0] = (uint8_t)(tm >> 56);
    buf[1] = (uint8_t)(tm >> 48);
    buf[2] = (uint8_t)(tm >> 40);
    buf[3] = (uint8_t)(tm >> 32);
    buf[4] = (uint8_t)(tm >> 24);
    buf[5] = (uint8_t)(tm >> 16);
    buf[6] = (uint8_t)(tm >> 8);
    buf[7] = (uint8_t)tm;

    obj_ptr<Buffer> data = new Buffer(buf, sizeof(buf));

    retVal = data;

    return result_t();
}

result_t uuid_base::get_hostID(int32_t& retVal)
{
    Isolate* isolate = Isolate::current();
    retVal = isolate->m_flake_host;
    return result_t();
}

result_t uuid_base::set_hostID(int32_t newVal)
{
    Isolate* isolate = Isolate::current();
    isolate->m_flake_host = newVal & 0x3ff;
    return result_t();
}

} /* namespace fibjs */
