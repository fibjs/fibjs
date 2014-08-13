/***************************************************************************
 *                                                                         *
 *   This file was automatically generated using idlc.js                   *
 *   PLEASE DO NOT EDIT!!!!                                                *
 *                                                                         *
 ***************************************************************************/

#ifndef _HttpUploadData_base_H_
#define _HttpUploadData_base_H_

/**
 @author Leo Hoo <lion@9465.net>
 */

#include "../object.h"

namespace fibjs
{

class SeekableStream_base;

class HttpUploadData_base : public object_base
{
public:
	// HttpUploadData_base
	virtual result_t get_fileName(std::string& retVal) = 0;
	virtual result_t get_contentType(std::string& retVal) = 0;
	virtual result_t get_contentTransferEncoding(std::string& retVal) = 0;
	virtual result_t get_body(obj_ptr<SeekableStream_base>& retVal) = 0;

	DECLARE_CLASSINFO(HttpUploadData_base);

public:
	static void s_get_fileName(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
	static void s_get_contentType(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
	static void s_get_contentTransferEncoding(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
	static void s_get_body(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
};

}

#include "SeekableStream.h"

namespace fibjs
{
	inline ClassInfo& HttpUploadData_base::class_info()
	{
		static ClassData::ClassProperty s_property[] = 
		{
			{"fileName", s_get_fileName, block_set},
			{"contentType", s_get_contentType, block_set},
			{"contentTransferEncoding", s_get_contentTransferEncoding, block_set},
			{"body", s_get_body, block_set}
		};

		static ClassData s_cd = 
		{ 
			"HttpUploadData", NULL, 
			0, NULL, 0, NULL, 4, s_property, NULL, NULL,
			&object_base::class_info()
		};

		static ClassInfo s_ci(s_cd);
		return s_ci;
	}

	inline void HttpUploadData_base::s_get_fileName(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
	{
		std::string vr;

		PROPERTY_ENTER();
		PROPERTY_INSTANCE(HttpUploadData_base);

		hr = pInst->get_fileName(vr);

		METHOD_RETURN();
	}

	inline void HttpUploadData_base::s_get_contentType(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
	{
		std::string vr;

		PROPERTY_ENTER();
		PROPERTY_INSTANCE(HttpUploadData_base);

		hr = pInst->get_contentType(vr);

		METHOD_RETURN();
	}

	inline void HttpUploadData_base::s_get_contentTransferEncoding(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
	{
		std::string vr;

		PROPERTY_ENTER();
		PROPERTY_INSTANCE(HttpUploadData_base);

		hr = pInst->get_contentTransferEncoding(vr);

		METHOD_RETURN();
	}

	inline void HttpUploadData_base::s_get_body(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
	{
		obj_ptr<SeekableStream_base> vr;

		PROPERTY_ENTER();
		PROPERTY_INSTANCE(HttpUploadData_base);

		hr = pInst->get_body(vr);

		METHOD_RETURN();
	}

}

#endif

