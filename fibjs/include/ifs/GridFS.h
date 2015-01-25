/***************************************************************************
 *                                                                         *
 *   This file was automatically generated using idlc.js                   *
 *   PLEASE DO NOT EDIT!!!!                                                *
 *                                                                         *
 ***************************************************************************/

#ifndef _GridFS_base_H_
#define _GridFS_base_H_

/**
 @author Leo Hoo <lion@9465.net>
 */

#include "../object.h"

namespace fibjs
{

class MemoryStream_base;
class Stream_base;
class Buffer_base;
class MongoCollection_base;

class GridFS_base : public object_base
{
	DECLARE_CLASS(GridFS_base);

public:
	// GridFS_base
	virtual result_t retrieve(const char* name, obj_ptr<MemoryStream_base>& retVal) = 0;
	virtual result_t store(const char* name, Stream_base* src) = 0;
	virtual result_t store(const char* name, Buffer_base* data) = 0;
	virtual result_t exists(const char* name, bool& retVal) = 0;
	virtual result_t remove(const char* name) = 0;
	virtual result_t get_files(obj_ptr<MongoCollection_base>& retVal) = 0;
	virtual result_t get_chunks(obj_ptr<MongoCollection_base>& retVal) = 0;

public:
	static void s_retrieve(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_store(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_exists(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_remove(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_get_files(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
	static void s_get_chunks(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
};

}

#include "MemoryStream.h"
#include "Stream.h"
#include "Buffer.h"
#include "MongoCollection.h"

namespace fibjs
{
	inline ClassInfo& GridFS_base::class_info()
	{
		static ClassData::ClassMethod s_method[] = 
		{
			{"retrieve", s_retrieve, false},
			{"store", s_store, false},
			{"exists", s_exists, false},
			{"remove", s_remove, false}
		};

		static ClassData::ClassProperty s_property[] = 
		{
			{"files", s_get_files, block_set},
			{"chunks", s_get_chunks, block_set}
		};

		static ClassData s_cd = 
		{ 
			"GridFS", NULL, 
			4, s_method, 0, NULL, 2, s_property, NULL, NULL,
			&object_base::class_info()
		};

		static ClassInfo s_ci(s_cd);
		return s_ci;
	}

	inline void GridFS_base::s_get_files(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
	{
		obj_ptr<MongoCollection_base> vr;

		PROPERTY_ENTER();
		PROPERTY_INSTANCE(GridFS_base);

		hr = pInst->get_files(vr);

		METHOD_RETURN();
	}

	inline void GridFS_base::s_get_chunks(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
	{
		obj_ptr<MongoCollection_base> vr;

		PROPERTY_ENTER();
		PROPERTY_INSTANCE(GridFS_base);

		hr = pInst->get_chunks(vr);

		METHOD_RETURN();
	}

	inline void GridFS_base::s_retrieve(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		obj_ptr<MemoryStream_base> vr;

		METHOD_INSTANCE(GridFS_base);
		METHOD_ENTER(1, 1);

		ARG(arg_string, 0);

		hr = pInst->retrieve(v0, vr);

		METHOD_RETURN();
	}

	inline void GridFS_base::s_store(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		METHOD_INSTANCE(GridFS_base);
		METHOD_ENTER(2, 2);

		ARG(arg_string, 0);
		ARG(obj_ptr<Stream_base>, 1);

		hr = pInst->store(v0, v1);

		METHOD_OVER(2, 2);

		ARG(arg_string, 0);
		ARG(obj_ptr<Buffer_base>, 1);

		hr = pInst->store(v0, v1);

		METHOD_VOID();
	}

	inline void GridFS_base::s_exists(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		bool vr;

		METHOD_INSTANCE(GridFS_base);
		METHOD_ENTER(1, 1);

		ARG(arg_string, 0);

		hr = pInst->exists(v0, vr);

		METHOD_RETURN();
	}

	inline void GridFS_base::s_remove(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		METHOD_INSTANCE(GridFS_base);
		METHOD_ENTER(1, 1);

		ARG(arg_string, 0);

		hr = pInst->remove(v0);

		METHOD_VOID();
	}

}

#endif

