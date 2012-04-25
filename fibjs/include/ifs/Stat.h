/***************************************************************************
 *   Copyright (C) 2012 by Leo Hoo                                         *
 *   lion@9465.net                                                         *
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
	// Stat_base
	virtual result_t get_name(std::string& retVal) = 0;
	virtual result_t get_size(double& retVal) = 0;
	virtual result_t get_mtime(int64_t& retVal) = 0;
	virtual result_t get_atime(int64_t& retVal) = 0;
	virtual result_t get_ctime(int64_t& retVal) = 0;
	virtual result_t isWritable(bool& retVal) = 0;
	virtual result_t isReadable(bool& retVal) = 0;
	virtual result_t isExecutable(bool& retVal) = 0;
	virtual result_t isHidden(bool& retVal) = 0;
	virtual result_t isDirectory(bool& retVal) = 0;
	virtual result_t isFile(bool& retVal) = 0;
	virtual result_t isSymbolicLink(bool& retVal) = 0;

public:
	static ClassInfo& class_info();

	virtual ClassInfo& Classinfo()
	{
		return class_info();
	}

	virtual result_t toJSON(const char* key, v8::Handle<v8::Object>& retVal)
	{
		result_t hr = object_base::toJSON(key, retVal);
		if(hr < 0)return hr;

		CLONE_String(name);
		CLONE(size, double);
		CLONE(mtime, int64_t);
		CLONE(atime, int64_t);
		CLONE(ctime, int64_t);

		return 0;
	}

private:
	static v8::Handle<v8::Value> s_get_name(v8::Local<v8::String> property, const v8::AccessorInfo &info);
	static v8::Handle<v8::Value> s_get_size(v8::Local<v8::String> property, const v8::AccessorInfo &info);
	static v8::Handle<v8::Value> s_get_mtime(v8::Local<v8::String> property, const v8::AccessorInfo &info);
	static v8::Handle<v8::Value> s_get_atime(v8::Local<v8::String> property, const v8::AccessorInfo &info);
	static v8::Handle<v8::Value> s_get_ctime(v8::Local<v8::String> property, const v8::AccessorInfo &info);
	static v8::Handle<v8::Value> s_isWritable(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_isReadable(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_isExecutable(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_isHidden(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_isDirectory(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_isFile(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_isSymbolicLink(const v8::Arguments& args);
};

}

namespace fibjs
{
	inline ClassInfo& Stat_base::class_info()
	{
		static ClassMethod s_method[] = 
		{
			{"isWritable", s_isWritable},
			{"isReadable", s_isReadable},
			{"isExecutable", s_isExecutable},
			{"isHidden", s_isHidden},
			{"isDirectory", s_isDirectory},
			{"isFile", s_isFile},
			{"isSymbolicLink", s_isSymbolicLink}
		};

		static ClassProperty s_property[] = 
		{
			{"name", s_get_name},
			{"size", s_get_size},
			{"mtime", s_get_mtime},
			{"atime", s_get_atime},
			{"ctime", s_get_ctime}
		};

		static ClassData s_cd = 
		{ 
			"Stat", NULL, 
			7, s_method, 0, NULL, 5, s_property, NULL,
			&object_base::class_info()
		};

		static ClassInfo s_ci(s_cd);
		return s_ci;
	}

	inline v8::Handle<v8::Value> Stat_base::s_get_name(v8::Local<v8::String> property, const v8::AccessorInfo &info)
	{
		std::string vr;

		PROPERTY_ENTER();
		PROPERTY_INSTANCE(Stat_base);

		hr = pInst->get_name(vr);

		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> Stat_base::s_get_size(v8::Local<v8::String> property, const v8::AccessorInfo &info)
	{
		double vr;

		PROPERTY_ENTER();
		PROPERTY_INSTANCE(Stat_base);

		hr = pInst->get_size(vr);

		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> Stat_base::s_get_mtime(v8::Local<v8::String> property, const v8::AccessorInfo &info)
	{
		int64_t vr;

		PROPERTY_ENTER();
		PROPERTY_INSTANCE(Stat_base);

		hr = pInst->get_mtime(vr);

		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> Stat_base::s_get_atime(v8::Local<v8::String> property, const v8::AccessorInfo &info)
	{
		int64_t vr;

		PROPERTY_ENTER();
		PROPERTY_INSTANCE(Stat_base);

		hr = pInst->get_atime(vr);

		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> Stat_base::s_get_ctime(v8::Local<v8::String> property, const v8::AccessorInfo &info)
	{
		int64_t vr;

		PROPERTY_ENTER();
		PROPERTY_INSTANCE(Stat_base);

		hr = pInst->get_ctime(vr);

		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> Stat_base::s_isWritable(const v8::Arguments& args)
	{
		bool vr;

		METHOD_INSTANCE(Stat_base);
		METHOD_ENTER(0, 0);

		hr = pInst->isWritable(vr);

		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> Stat_base::s_isReadable(const v8::Arguments& args)
	{
		bool vr;

		METHOD_INSTANCE(Stat_base);
		METHOD_ENTER(0, 0);

		hr = pInst->isReadable(vr);

		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> Stat_base::s_isExecutable(const v8::Arguments& args)
	{
		bool vr;

		METHOD_INSTANCE(Stat_base);
		METHOD_ENTER(0, 0);

		hr = pInst->isExecutable(vr);

		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> Stat_base::s_isHidden(const v8::Arguments& args)
	{
		bool vr;

		METHOD_INSTANCE(Stat_base);
		METHOD_ENTER(0, 0);

		hr = pInst->isHidden(vr);

		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> Stat_base::s_isDirectory(const v8::Arguments& args)
	{
		bool vr;

		METHOD_INSTANCE(Stat_base);
		METHOD_ENTER(0, 0);

		hr = pInst->isDirectory(vr);

		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> Stat_base::s_isFile(const v8::Arguments& args)
	{
		bool vr;

		METHOD_INSTANCE(Stat_base);
		METHOD_ENTER(0, 0);

		hr = pInst->isFile(vr);

		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> Stat_base::s_isSymbolicLink(const v8::Arguments& args)
	{
		bool vr;

		METHOD_INSTANCE(Stat_base);
		METHOD_ENTER(0, 0);

		hr = pInst->isSymbolicLink(vr);

		METHOD_RETURN();
	}

}

#endif

