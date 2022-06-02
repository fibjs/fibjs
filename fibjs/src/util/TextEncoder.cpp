/*
 * TextEncoder.cpp
 *
 *  Created on: Jun 2, 2022
 *      Author: lion
 */

#include "object.h"
#include <TextEncoder.h>
#include "ifs/iconv.h"

namespace fibjs {

result_t TextEncoder_base::_new(exlib::string codec, v8::Local<v8::Object> opts, obj_ptr<TextEncoder_base>& retVal,
    v8::Local<v8::Object> This)
{
    retVal = new TextEncoder(codec);
    return 0;
}

result_t TextEncoder::encode(exlib::string data, v8::Local<v8::Object> opts, obj_ptr<Buffer_base>& retVal)
{
    return iconv_base::encode(m_codec, data, retVal);
}

result_t TextEncoder::get_encoding(exlib::string& retVal)
{
    retVal = m_codec;
    return 0;
}

result_t TextDecoder_base::_new(exlib::string codec, v8::Local<v8::Object> opts, obj_ptr<TextDecoder_base>& retVal,
    v8::Local<v8::Object> This)
{
    retVal = new TextDecoder(codec);
    return 0;
}

result_t TextDecoder::decode(Buffer_base* data, v8::Local<v8::Object> opts, exlib::string& retVal)
{
    return iconv_base::decode(m_codec, data, retVal);
}

result_t TextDecoder::get_encoding(exlib::string& retVal)
{
    retVal = m_codec;
    return 0;
}

}
