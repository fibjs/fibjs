/***************************************************************************
 *                                                                         *
 *   This file was automatically generated using idlc.js                   *
 *   PLEASE DO NOT EDIT!!!!                                                *
 *                                                                         *
 ***************************************************************************/

#ifndef _Stat_base_H_
#define _Stat_base_H_

/**
 @author Leo Hoo <lion@9465.net>
 */

#include "../object.h"

namespace fibjs
{

class Stat_base : public object_base
{
public:
	Stat_base()
	{
		Stat_base::class_info().Ref();
	}

	virtual ~Stat_base()
	{
		Stat_base::class_info().Unref();
	}

public:
	// Stat_base
	virtual result_t get_name(std::string& retVal) = 0;
	virtual result_t get_size(int64_t& retVal) = 0;
	virtual result_t get_mode(int32_t& retVal) = 0;
	virtual result_t get_mtime(date_t& retVal) = 0;
	virtual result_t get_atime(date_t& retVal) = 0;
	virtual result_t get_ctime(date_t& retVal) = 0;
	virtual result_t isWritable(bool& retVal) = 0;
	virtual result_t isReadable(bool& retVal) = 0;
	virtual result_t isExecutable(bool& retVal) = 0;
	virtual result_t isHidden(bool& retVal) = 0;
	virtual result_t isDirectory(bool& retVal) = 0;
	virtual result_t isFile(bool& retVal) = 0;
	virtual result_t isSymbolicLink(bool& retVal) = 0;
	virtual result_t isMemory(bool& retVal) = 0;
	virtual result_t isSocket(bool& retVal) = 0;

	DECLARE_CLASSINFO(Stat_base);

public:
	template<typename T>
	static void __new(const T &args){}

public:
	static void s_get_name(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
	static void s_get_size(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
	static void s_get_mode(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
	static void s_get_mtime(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
	static void s_get_atime(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
	static void s_get_ctime(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
	static void s_isWritable(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_isReadable(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_isExecutable(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_isHidden(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_isDirectory(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_isFile(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_isSymbolicLink(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_isMemory(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_isSocket(const v8::FunctionCallbackInfo<v8::Value>& args);
};

}

namespace fibjs
{
	inline ClassInfo& Stat_base::class_info()
	{
		static ClassData::ClassMethod s_method[] = 
		{
			{"isWritable", s_isWritable},
			{"isReadable", s_isReadable},
			{"isExecutable", s_isExecutable},
			{"isHidden", s_isHidden},
			{"isDirectory", s_isDirectory},
			{"isFile", s_isFile},
			{"isSymbolicLink", s_isSymbolicLink},
			{"isMemory", s_isMemory},
			{"isSocket", s_isSocket}
		};

		static ClassData::ClassProperty s_property[] = 
		{
			{"name", s_get_name, block_set},
			{"size", s_get_size, block_set},
			{"mode", s_get_mode, block_set},
			{"mtime", s_get_mtime, block_set},
			{"atime", s_get_atime, block_set},
			{"ctime", s_get_ctime, block_set}
		};

		static ClassData s_cd = 
		{ 
			"Stat", NULL, 
			9, s_method, 0, NULL, 6, s_property, NULL, NULL,
			&object_base::class_info()
		};

		static ClassInfo s_ci(s_cd);
		return s_ci;
	}

	inline void Stat_base::s_get_name(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
	{
		std::string vr;

		PROPERTY_ENTER();
		PROPERTY_INSTANCE(Stat_base);

		hr = pInst->get_name(vr);

		METHOD_RETURN();
	}

	inline void Stat_base::s_get_size(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
	{
		int64_t vr;

		PROPERTY_ENTER();
		PROPERTY_INSTANCE(Stat_base);

		hr = pInst->get_size(vr);

		METHOD_RETURN();
	}

	inline void Stat_base::s_get_mode(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
	{
		int32_t vr;

		PROPERTY_ENTER();
		PROPERTY_INSTANCE(Stat_base);

		hr = pInst->get_mode(vr);

		METHOD_RETURN();
	}

	inline void Stat_base::s_get_mtime(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
	{
		date_t vr;

		PROPERTY_ENTER();
		PROPERTY_INSTANCE(Stat_base);

		hr = pInst->get_mtime(vr);

		METHOD_RETURN();
	}

	inline void Stat_base::s_get_atime(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
	{
		date_t vr;

		PROPERTY_ENTER();
		PROPERTY_INSTANCE(Stat_base);

		hr = pInst->get_atime(vr);

		METHOD_RETURN();
	}

	inline void Stat_base::s_get_ctime(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
	{
		date_t vr;

		PROPERTY_ENTER();
		PROPERTY_INSTANCE(Stat_base);

		hr = pInst->get_ctime(vr);

		METHOD_RETURN();
	}

	inline void Stat_base::s_isWritable(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		bool vr;

		METHOD_INSTANCE(Stat_base);
		METHOD_ENTER(0, 0);

		hr = pInst->isWritable(vr);

		METHOD_RETURN();
	}

	inline void Stat_base::s_isReadable(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		bool vr;

		METHOD_INSTANCE(Stat_base);
		METHOD_ENTER(0, 0);

		hr = pInst->isReadable(vr);

		METHOD_RETURN();
	}

	inline void Stat_base::s_isExecutable(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		bool vr;

		METHOD_INSTANCE(Stat_base);
		METHOD_ENTER(0, 0);

		hr = pInst->isExecutable(vr);

		METHOD_RETURN();
	}

	inline void Stat_base::s_isHidden(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		bool vr;

		METHOD_INSTANCE(Stat_base);
		METHOD_ENTER(0, 0);

		hr = pInst->isHidden(vr);

		METHOD_RETURN();
	}

	inline void Stat_base::s_isDirectory(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		bool vr;

		METHOD_INSTANCE(Stat_base);
		METHOD_ENTER(0, 0);

		hr = pInst->isDirectory(vr);

		METHOD_RETURN();
	}

	inline void Stat_base::s_isFile(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		bool vr;

		METHOD_INSTANCE(Stat_base);
		METHOD_ENTER(0, 0);

		hr = pInst->isFile(vr);

		METHOD_RETURN();
	}

	inline void Stat_base::s_isSymbolicLink(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		bool vr;

		METHOD_INSTANCE(Stat_base);
		METHOD_ENTER(0, 0);

		hr = pInst->isSymbolicLink(vr);

		METHOD_RETURN();
	}

	inline void Stat_base::s_isMemory(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		bool vr;

		METHOD_INSTANCE(Stat_base);
		METHOD_ENTER(0, 0);

		hr = pInst->isMemory(vr);

		METHOD_RETURN();
	}

	inline void Stat_base::s_isSocket(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		bool vr;

		METHOD_INSTANCE(Stat_base);
		METHOD_ENTER(0, 0);

		hr = pInst->isSocket(vr);

		METHOD_RETURN();
	}

}

#endif

