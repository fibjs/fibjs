/*
 * encode.h
 *
 *  Created on: Jul 1, 2023
 *      Author: lion
 */

#pragma once

#include "defs.h"
#include <stdint.h>
#include <string.h>
#include <vector>
#include <blst/include/blst.h>

namespace fibjs {

namespace codec_impl {
    template <typename T>
    class span {
    public:
        span(const T* data, size_t size)
            : m_data(data)
            , m_size(size)
        {
        }

    public:
        const T* data() const
        {
            return m_data;
        }

        size_t size() const
        {
            return m_size;
        }

    private:
        const T* m_data;
        size_t m_size;
    };

    // uint64_t
    namespace {
        void encode_one(std::vector<uint8_t>& result, uint64_t v)
        {
            const size_t pos = result.size();
            result.resize(pos + sizeof(uint64_t));

            uint8_t* p = result.data() + pos;
            p[0] = (uint8_t)(v >> 56);
            p[1] = (uint8_t)(v >> 48);
            p[2] = (uint8_t)(v >> 40);
            p[3] = (uint8_t)(v >> 32);
            p[4] = (uint8_t)(v >> 24);
            p[5] = (uint8_t)(v >> 16);
            p[6] = (uint8_t)(v >> 8);
            p[7] = (uint8_t)(v);
        }

        void decode_one(const uint8_t*& m_data, uint64_t* v)
        {
            *v = ((uint64_t)m_data[0] << 56)
                | ((uint64_t)m_data[1] << 48)
                | ((uint64_t)m_data[2] << 40)
                | ((uint64_t)m_data[3] << 32)
                | ((uint64_t)m_data[4] << 24)
                | ((uint64_t)m_data[5] << 16)
                | ((uint64_t)m_data[6] << 8)
                | ((uint64_t)m_data[7]);
            m_data += sizeof(uint64_t);
        }

        size_t codec_length(uint64_t v)
        {
            return sizeof(uint64_t);
        }
    }

    // blst_scalar
    namespace {
        void encode_one(std::vector<uint8_t>& result, const blst_scalar& v)
        {
            const size_t pos = result.size();
            result.resize(pos + FR_BYTE_SIZE);

            blst_bendian_from_scalar(result.data() + pos, &v);
        }

        void decode_one(const uint8_t*& m_data, blst_scalar* v)
        {
            blst_scalar_from_bendian(v, m_data);
            m_data += FR_BYTE_SIZE;
        }

        size_t codec_length(const blst_scalar& v)
        {
            return FR_BYTE_SIZE;
        }
    }

    // blst_p1
    namespace {
        void encode_one(std::vector<uint8_t>& result, const blst_p1& v)
        {
            const size_t pos = result.size();
            result.resize(pos + G1_COMPRESSED_SIZE);

            blst_p1_affine a;

            blst_p1_to_affine(&a, &v);
            blst_p1_affine_compress(result.data() + pos, &a);
        }

        void decode_one(const uint8_t*& m_data, blst_p1* v)
        {
            blst_p1_affine a;

            blst_p1_uncompress(&a, m_data);
            blst_p1_from_affine(v, &a);

            m_data += G1_COMPRESSED_SIZE;
        }

        size_t codec_length(const blst_p1& v)
        {
            return G1_COMPRESSED_SIZE;
        }
    }

    // blst_p2
    namespace {
        void encode_one(std::vector<uint8_t>& result, const blst_p2& v)
        {
            const size_t pos = result.size();
            result.resize(pos + G2_COMPRESSED_SIZE);

            blst_p2_affine a;

            blst_p2_to_affine(&a, &v);
            blst_p2_affine_compress(result.data() + pos, &a);
        }

        void decode_one(const uint8_t*& m_data, blst_p2* v)
        {
            blst_p2_affine a;

            blst_p2_uncompress(&a, m_data);
            blst_p2_from_affine(v, &a);

            m_data += G2_COMPRESSED_SIZE;
        }

        size_t codec_length(const blst_p2& v)
        {
            return G2_COMPRESSED_SIZE;
        }
    }

    // std::pair
    namespace {
        template <typename T1, typename T2>
        void encode_one(std::vector<uint8_t>& result, const std::pair<T1, T2>& v)
        {
            encode_one(result, v.first);
            encode_one(result, v.second);
        }

        template <typename T1, typename T2>
        size_t codec_length(const std::pair<T1, T2>& v)
        {
            return codec_length(v.first) + codec_length(v.second);
        }
    }

    // std::vector
    namespace {
        template <typename T>
        void encode_one(std::vector<uint8_t>& result, const std::vector<T>& v)
        {
            for (const auto& i : v)
                encode_one(result, i);
        }

        template <typename T>
        size_t codec_length(const std::vector<T>& v)
        {
            if (v.size() == 0)
                return 0;
            return codec_length(*v.data()) * v.size();
        }

        void encode_one(std::vector<uint8_t>& result, const std::vector<uint8_t>& v)
        {
            const size_t pos = result.size();
            result.resize(pos + v.size());

            memcpy(result.data() + pos, v.data(), v.size());
        }

        size_t codec_length(const std::vector<uint8_t>& v)
        {
            return v.size();
        }
    }

    // span
    namespace {
        template <typename T>
        void encode_one(std::vector<uint8_t>& result, const span<T>& v)
        {
            for (size_t i = 0; i < v.size(); ++i)
                encode_one(result, v.data()[i]);
        }

        template <typename T>
        size_t codec_length(const span<T>& v)
        {
            if (v.size() == 0)
                return 0;
            return codec_length(*v.data()) * v.size();
        }

        void encode_one(std::vector<uint8_t>& result, const span<uint8_t>& v)
        {
            const size_t pos = result.size();
            result.resize(pos + v.size());

            memcpy(result.data() + pos, v.data(), v.size());
        }

        size_t codec_length(const span<uint8_t>& v)
        {
            return v.size();
        }
    }
}

template <typename... Ts>
std::vector<uint8_t> encode(Ts&&... ts)
{
    std::vector<uint8_t> result;

    result.reserve((codec_impl::codec_length(ts) + ...));
    (codec_impl::encode_one(result, ts), ...);

    return result;
}

class decoder {
public:
    decoder(const uint8_t* data, size_t size)
        : m_data(data)
        , m_size(size)
    {
    }

public:
    template <typename... Ts>
    intptr_t decode(Ts&&... ts)
    {
        size_t sz = (codec_impl::codec_length(*ts) + ...);
        if (sz > m_size)
            return (intptr_t)m_size - (intptr_t)sz;

        (codec_impl::decode_one(m_data, ts), ...);
        m_size -= sz;

        return (intptr_t)m_size;
    }

private:
    const uint8_t* m_data;
    size_t m_size;
};

}
