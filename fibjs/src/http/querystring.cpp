/*
 * querystring.cpp
 *
 *  Created on: Feb 20, 2017
 *      Author: lion
 */

#include "object.h"
#include "ifs/querystring.h"
#include "ifs/encoding.h"
#include "HttpCollection.h"
#include "StringBuffer.h"

namespace fibjs
{

DECLARE_MODULE(querystring);

result_t querystring_base::escape(exlib::string str, exlib::string& retVal)
{
	return encoding_base::encodeURIComponent(str, retVal);
}

result_t querystring_base::unescape(exlib::string str, exlib::string& retVal)
{
	return encoding_base::decodeURI(str, retVal);
}

result_t querystring_base::parse(exlib::string str, exlib::string sep,
                                 exlib::string eq, v8::Local<v8::Object> opt,
                                 obj_ptr<HttpCollection_base>& retVal)
{
	result_t hr;

	obj_ptr<HttpCollection> c = new HttpCollection();
	hr = c->parse(str, sep.c_str(), eq.c_str());
	if (hr < 0)
		return hr;

	retVal = c;

	return 0;
}

result_t querystring_base::stringify(v8::Local<v8::Object> obj, exlib::string sep,
                                     exlib::string eq, v8::Local<v8::Object> opt,
                                     exlib::string& retVal)
{
	StringBuffer bufs;

	v8::Local<v8::Array> ks = obj->GetPropertyNames();
	int32_t len = ks->Length();
	int32_t i;
	result_t hr;

	for (i = 0; i < len; i++)
	{
		exlib::string strKey, strValue, str;
		v8::Local<v8::Array> vs;
		v8::Local<v8::Value> k = ks->Get(i);
		v8::Local<v8::Value> v = obj->Get(k);

		GetArgumentValue(k, strKey);
		encoding_base::encodeURIComponent(strKey, strKey);

		hr = GetArgumentValue(v, vs, true);
		if (hr >= 0)
		{
			int32_t len1 = vs->Length();
			int32_t i1;

			for (i1 = 0; i1 < len1; i1++)
			{
				hr = GetArgumentValue(vs->Get(i1), strValue);
				if (hr < 0)
					return hr;

				encoding_base::encodeURIComponent(strValue, strValue);

				if (bufs.size() > 0)
					bufs.append(sep);

				bufs.append(strKey);
				bufs.append(eq);
				bufs.append(strValue);
			}
		} else
		{
			hr = GetArgumentValue(v, strValue);
			if (hr < 0)
				return hr;

			encoding_base::encodeURIComponent(strValue, strValue);

			if (bufs.size() > 0)
				bufs.append(sep);

			bufs.append(strKey);
			bufs.append(eq);
			bufs.append(strValue);
		}
	}

	retVal = bufs.str();

	return 0;
}

}