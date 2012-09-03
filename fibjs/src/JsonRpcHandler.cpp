/*
 * JsonRpcHandler.cpp
 *
 *  Created on: Sep 3, 2012
 *      Author: lion
 */

#include "JsonRpcHandler.h"
#include "ifs/mq.h"
#include "ifs/encoding.h"
#include "ifs/HttpRequest.h"
#include "Buffer.h"
#include "MemoryStream.h"

namespace fibjs
{

result_t JsonRpcHandler::invoke(object_base* v, obj_ptr<Handler_base>& retVal,
		exlib::AsyncEvent* ac)
{
	if (ac)
		return CALL_E_NOASYNC;

	obj_ptr<Message_base> msg = Message_base::getInstance(v);
	if (msg == NULL)
		return CALL_E_BADVARTYPE;

	obj_ptr<HttpRequest_base> htreq = HttpRequest_base::getInstance(v);
	obj_ptr<SeekableStream_base> body;
	obj_ptr<Buffer_base> buf;
	v8::Handle<v8::Value> jsval;
	v8::Handle<v8::Object> o;
	Variant result;
	std::string strBuf;
	int64_t len;
	int32_t sz, i;
	result_t hr;

	msg->get_body(body);

	body->size(len);
	sz = (int32_t) len;

	body->rewind();
	hr = body->ac_read(sz, buf);
	if (hr < 0)
		return hr;
	if (hr == CALL_RETURN_NULL)
		return CALL_E_INVALID_CALL;
	body.Release();

	buf->toString(strBuf);
	buf.Release();

	hr = encoding_base::jsonDecode(strBuf.c_str(), jsval);
	if (hr < 0)
		return hr;

	if (!jsval->IsObject())
		return CALL_E_INVALID_CALL;

	o = v8::Handle<v8::Object>::Cast(jsval);

	jsval = o->Get(v8::String::NewSymbol("method", 6));
	if (IsEmpty(jsval))
		return CALL_E_INVALID_CALL;

	msg->set_value(*v8::String::Utf8Value(jsval));

	jsval = o->Get(v8::String::NewSymbol("params", 6));
	if (!jsval.IsEmpty() && jsval->IsArray())
	{
		v8::Handle<v8::Array> jsparams = v8::Handle<v8::Array>::Cast(jsval);
		obj_ptr<List_base> params;

		sz = jsparams->Length();
		msg->get_params(params);
		params->resize(sz);

		for (i = 0; i < sz; i++)
			params->_indexed_setter(i, jsparams->Get(i));
	}

	obj_ptr<Handler_base> hdlr1;

	hr = mq_base::js_invoke(m_hdlr, v, hdlr1, NULL);
	if (hr >= 0 && hr != CALL_RETURN_NULL)
		hr = mq_base::ac_invoke(m_hdlr, v);

	msg->get_result(result);

	v8::Handle<v8::String> strId = v8::String::NewSymbol("id", 2);
	jsval = o->Get(strId);

	o = v8::Object::New();
	o->Set(strId, jsval);
	o->Set(v8::String::NewSymbol("result", 6), result);

	hr = encoding_base::jsonEncode(o, strBuf);
	if (hr < 0)
		return hr;

	buf = new Buffer(strBuf);
	body = new MemoryStream();
	hr = body->ac_write(buf);
	if (hr < 0)
		return hr;

	msg->set_body(body);

	if (htreq)
	{
		obj_ptr<HttpResponse_base> htrep;

		htreq->get_response(htrep);
		htrep->set_body(body);
	}

	return CALL_RETURN_NULL;
}

} /* namespace fibjs */
