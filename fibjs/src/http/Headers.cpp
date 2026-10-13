/*
 * Headers.cpp
 *
 *  Created on: Jul 11, 2025
 *      Author: lion
 */

#include "object.h"
#include "Headers.h"

namespace fibjs {

result_t Headers_base::_new(obj_ptr<Headers_base>& retVal, v8::Local<v8::Object> This)
{
    retVal = new Headers();
    return result_t();
}

result_t Headers_base::_new(Union_Headers_init init, obj_ptr<Headers_base>& retVal, v8::Local<v8::Object> This)
{
    if (std::holds_alternative<v8::Local<v8::Array>>(init)) {
        retVal = new Headers();

        // an element of the sequence that is not a pair is a type error, the way
        // the DOM and node report it
        result_t hr = retVal->append(std::get<v8::Local<v8::Array>>(init));
        if (hr.is_bad_var_type())
            return result_t::make(CALL_E_EXCEPTION).with_message("Failed to construct 'Headers': sequence elements must be pairs.").with_result(result_t::type_mismatch());

        return hr;
    }

    if (std::holds_alternative<obj_ptr<Headers_base>>(init)) {
        obj_ptr<Headers> headers = new Headers();
        retVal = headers;
        return headers->init(std::get<obj_ptr<Headers_base>>(init).get());
    }

    retVal = new Headers();
    return retVal->append(std::get<v8::Local<v8::Object>>(init));
}

}