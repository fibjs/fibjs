/*
 * JsonRpcHandler.cpp
 *
 *  Created on: Sep 3, 2012
 *      Author: lion
 */

#include "ifs/mq.h"
#include "ifs/encoding.h"
#include "ifs/HttpRequest.h"
#include "ifs/console.h"

#include "JsonRpcHandler.h"
#include "Buffer.h"
#include "MemoryStream.h"
#include "JSHandler.h"

namespace fibjs
{

result_t JsonRpcHandler::invoke(object_base *v, obj_ptr<Handler_base> &retVal,
                                AsyncEvent *ac)
{
    if (ac)
        return CHECK_ERROR(CALL_E_NOASYNC);

    obj_ptr<Message_base> msg = Message_base::getInstance(v);
    if (msg == NULL)
        return CHECK_ERROR(CALL_E_BADVARTYPE);

    Isolate* isolate = holder();
    obj_ptr<HttpRequest_base> htreq = HttpRequest_base::getInstance(v);
    obj_ptr<SeekableStream_base> body;
    obj_ptr<Buffer_base> buf;
    v8::Local<v8::Value> jsval;
    v8::Local<v8::Object> o;
    Variant result;
    std::string str;
    int64_t len;
    int32_t sz, i;
    result_t hr;
    obj_ptr<List_base> params;

    if (htreq != NULL)
    {
        if (htreq->firstHeader("Content-Type", result) == CALL_RETURN_NULL)
            return CHECK_ERROR(Runtime::setError("jsonrpc: Content-Type is missing."));

        str = result.string();
        if (qstricmp(str.c_str(), "application/json", 16))
            return CHECK_ERROR(Runtime::setError("jsonrpc: Invalid Content-Type."));
    }

    msg->get_body(body);

    body->size(len);
    sz = (int32_t) len;

    body->rewind();
    hr = body->ac_read(sz, buf);
    if (hr < 0)
        return hr;
    if (hr == CALL_RETURN_NULL)
        return CHECK_ERROR(Runtime::setError("jsonrpc: request body is empty."));
    body.Release();

    buf->toString(str);
    buf.Release();

    hr = encoding_base::jsonDecode(str.c_str(), jsval);
    if (hr < 0)
        return hr;

    if (!jsval->IsObject())
        return CHECK_ERROR(Runtime::setError("jsonrpc: Invalid rpc request."));

    o = v8::Local<v8::Object>::Cast(jsval);

    jsval = o->Get(isolate->NewFromUtf8("method", 6));
    if (IsEmpty(jsval))
        return CHECK_ERROR(Runtime::setError("jsonrpc: method is missing."));

    msg->get_value(str);
    str += '/';
    str.append(*v8::String::Utf8Value(jsval));
    msg->set_value(str.c_str());

    jsval = o->Get(isolate->NewFromUtf8("params", 6));
    if (!jsval.IsEmpty() && jsval->IsArray())
    {
        v8::Local<v8::Array> jsparams = v8::Local<v8::Array>::Cast(jsval);

        sz = jsparams->Length();
        msg->get_params(params);
        params->resize(sz);

        for (i = 0; i < sz; i++)
            params->_indexed_setter(i, jsparams->Get(i));
    }

    obj_ptr<Handler_base> hdlr1;
    hr = JSHandler::js_invoke(m_handler, v, hdlr1, NULL);
    if (hr >= 0 && hr != CALL_RETURN_NULL)
        hr = mq_base::ac_invoke(hdlr1, v);

    v8::Local<v8::String> strId = isolate->NewFromUtf8("id", 2);
    jsval = o->Get(strId);

    o = v8::Object::New(isolate->m_isolate);
    o->Set(strId, jsval);

    if (hr < 0)
    {
        asyncLog(console_base::_ERROR, "JsonRpcHandler: " + getResultMessage(hr));

        result_t hr1 = encoding_base::jsonEncode(o, str);
        if (hr1 < 0)
            return hr1;

        if (str.length() <= 2)
            str.assign("{", 1);
        else
        {
            str.resize(str.length() - 1);
            str += ',';
        }

        if (hr == CALL_E_INVALID_CALL)
            str.append(
                "\"error\": {\"code\": -32601, \"message\": \"Method not found.\"}}");
        else
            str.append(
                "\"error\": {\"code\": -32603, \"message\": \"Internal error.\"}}");
    }
    else
    {
        msg->get_result(result);
        o->Set(isolate->NewFromUtf8("result", 6), result);

        hr = encoding_base::jsonEncode(o, str);

        if (hr < 0)
            return hr;
    }

    body = new MemoryStream();

    buf = new Buffer(str);
    hr = body->ac_write(buf);
    if (hr < 0)
        return hr;


    obj_ptr<Message_base> rep;
    hr = msg->get_response(rep);
    if (hr < 0)
        return hr;

    rep->set_body(body);

    if (htreq)
        ((HttpMessage_base *)(Message_base *)rep)->setHeader(
            "Content-Type", "application/json");

    return CALL_RETURN_NULL;
}

} /* namespace fibjs */
