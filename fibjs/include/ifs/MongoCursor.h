/***************************************************************************
 *   Copyright (C) 2012 by Leo Hoo                                         *
 *   lion@9465.net                                                         *
 *                                                                         *
 ***************************************************************************/

#ifndef _MongoCursor_base_H_
#define _MongoCursor_base_H_

/**
 @author Leo Hoo <lion@9465.net>
 */

#include "../object.h"

namespace fibjs
{

class MongoCursor_base : public object_base
{
public:
	// MongoCursor_base
	virtual result_t batchSize(int32_t size, obj_ptr<MongoCursor_base>& retVal) = 0;
	virtual result_t hint(v8::Handle<v8::Object> opts, obj_ptr<MongoCursor_base>& retVal) = 0;
	virtual result_t limit(int32_t size, obj_ptr<MongoCursor_base>& retVal) = 0;
	virtual result_t readPref(const char* mode, v8::Handle<v8::Array> tagSet, obj_ptr<MongoCursor_base>& retVal) = 0;
	virtual result_t showDiskLoc(obj_ptr<MongoCursor_base>& retVal) = 0;
	virtual result_t count(bool applySkipLimit, int32_t& retVal) = 0;
	virtual result_t explain(v8::Handle<v8::Object>& retVal) = 0;
	virtual result_t forEach(v8::Handle<v8::Function> func) = 0;
	virtual result_t map(v8::Handle<v8::Function> func, v8::Handle<v8::Array>& retVal) = 0;
	virtual result_t hasNext(bool& retVal) = 0;
	virtual result_t next(v8::Handle<v8::Object>& retVal) = 0;
	virtual result_t size(int32_t& retVal) = 0;
	virtual result_t skip(int32_t num, obj_ptr<MongoCursor_base>& retVal) = 0;
	virtual result_t snapshot(obj_ptr<MongoCursor_base>& retVal) = 0;
	virtual result_t sort(v8::Handle<v8::Object> opts, obj_ptr<MongoCursor_base>& retVal) = 0;
	virtual result_t min(v8::Handle<v8::Object> opts, obj_ptr<MongoCursor_base>& retVal) = 0;
	virtual result_t max(v8::Handle<v8::Object> opts, obj_ptr<MongoCursor_base>& retVal) = 0;
	virtual result_t _addSpecial(const char* name, v8::Handle<v8::Value> opts, obj_ptr<MongoCursor_base>& retVal) = 0;
	virtual result_t toArray(v8::Handle<v8::Array>& retVal) = 0;

	DECLARE_CLASSINFO(MongoCursor_base);

public:
	static v8::Handle<v8::Value> s_batchSize(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_hint(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_limit(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_readPref(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_showDiskLoc(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_count(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_explain(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_forEach(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_map(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_hasNext(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_next(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_size(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_skip(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_snapshot(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_sort(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_min(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_max(const v8::Arguments& args);
	static v8::Handle<v8::Value> s__addSpecial(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_toArray(const v8::Arguments& args);
};

}

namespace fibjs
{
	inline ClassInfo& MongoCursor_base::class_info()
	{
		static ClassData::ClassMethod s_method[] = 
		{
			{"batchSize", s_batchSize},
			{"hint", s_hint},
			{"limit", s_limit},
			{"readPref", s_readPref},
			{"showDiskLoc", s_showDiskLoc},
			{"count", s_count},
			{"explain", s_explain},
			{"forEach", s_forEach},
			{"map", s_map},
			{"hasNext", s_hasNext},
			{"next", s_next},
			{"size", s_size},
			{"skip", s_skip},
			{"snapshot", s_snapshot},
			{"sort", s_sort},
			{"min", s_min},
			{"max", s_max},
			{"_addSpecial", s__addSpecial},
			{"toArray", s_toArray}
		};

		static ClassData s_cd = 
		{ 
			"MongoCursor", NULL, 
			19, s_method, 0, NULL, 0, NULL, NULL, NULL,
			&object_base::class_info()
		};

		static ClassInfo s_ci(s_cd);
		return s_ci;
	}


	inline v8::Handle<v8::Value> MongoCursor_base::s_batchSize(const v8::Arguments& args)
	{
		obj_ptr<MongoCursor_base> vr;

		METHOD_INSTANCE(MongoCursor_base);
		METHOD_ENTER(1, 1);

		ARG(int32_t, 0);

		hr = pInst->batchSize(v0, vr);

		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> MongoCursor_base::s_hint(const v8::Arguments& args)
	{
		obj_ptr<MongoCursor_base> vr;

		METHOD_INSTANCE(MongoCursor_base);
		METHOD_ENTER(1, 1);

		ARG(v8::Handle<v8::Object>, 0);

		hr = pInst->hint(v0, vr);

		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> MongoCursor_base::s_limit(const v8::Arguments& args)
	{
		obj_ptr<MongoCursor_base> vr;

		METHOD_INSTANCE(MongoCursor_base);
		METHOD_ENTER(1, 1);

		ARG(int32_t, 0);

		hr = pInst->limit(v0, vr);

		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> MongoCursor_base::s_readPref(const v8::Arguments& args)
	{
		obj_ptr<MongoCursor_base> vr;

		METHOD_INSTANCE(MongoCursor_base);
		METHOD_ENTER(2, 1);

		ARG_String(0);
		OPT_ARG(v8::Handle<v8::Array>, 1, v8::Array::New());

		hr = pInst->readPref(v0, v1, vr);

		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> MongoCursor_base::s_showDiskLoc(const v8::Arguments& args)
	{
		obj_ptr<MongoCursor_base> vr;

		METHOD_INSTANCE(MongoCursor_base);
		METHOD_ENTER(0, 0);

		hr = pInst->showDiskLoc(vr);

		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> MongoCursor_base::s_count(const v8::Arguments& args)
	{
		int32_t vr;

		METHOD_INSTANCE(MongoCursor_base);
		METHOD_ENTER(1, 0);

		OPT_ARG(bool, 0, false);

		hr = pInst->count(v0, vr);

		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> MongoCursor_base::s_explain(const v8::Arguments& args)
	{
		v8::Handle<v8::Object> vr;

		METHOD_INSTANCE(MongoCursor_base);
		METHOD_ENTER(0, 0);

		hr = pInst->explain(vr);

		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> MongoCursor_base::s_forEach(const v8::Arguments& args)
	{
		METHOD_INSTANCE(MongoCursor_base);
		METHOD_ENTER(1, 1);

		ARG(v8::Handle<v8::Function>, 0);

		hr = pInst->forEach(v0);

		METHOD_VOID();
	}

	inline v8::Handle<v8::Value> MongoCursor_base::s_map(const v8::Arguments& args)
	{
		v8::Handle<v8::Array> vr;

		METHOD_INSTANCE(MongoCursor_base);
		METHOD_ENTER(1, 1);

		ARG(v8::Handle<v8::Function>, 0);

		hr = pInst->map(v0, vr);

		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> MongoCursor_base::s_hasNext(const v8::Arguments& args)
	{
		bool vr;

		METHOD_INSTANCE(MongoCursor_base);
		METHOD_ENTER(0, 0);

		hr = pInst->hasNext(vr);

		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> MongoCursor_base::s_next(const v8::Arguments& args)
	{
		v8::Handle<v8::Object> vr;

		METHOD_INSTANCE(MongoCursor_base);
		METHOD_ENTER(0, 0);

		hr = pInst->next(vr);

		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> MongoCursor_base::s_size(const v8::Arguments& args)
	{
		int32_t vr;

		METHOD_INSTANCE(MongoCursor_base);
		METHOD_ENTER(0, 0);

		hr = pInst->size(vr);

		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> MongoCursor_base::s_skip(const v8::Arguments& args)
	{
		obj_ptr<MongoCursor_base> vr;

		METHOD_INSTANCE(MongoCursor_base);
		METHOD_ENTER(1, 1);

		ARG(int32_t, 0);

		hr = pInst->skip(v0, vr);

		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> MongoCursor_base::s_snapshot(const v8::Arguments& args)
	{
		obj_ptr<MongoCursor_base> vr;

		METHOD_INSTANCE(MongoCursor_base);
		METHOD_ENTER(0, 0);

		hr = pInst->snapshot(vr);

		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> MongoCursor_base::s_sort(const v8::Arguments& args)
	{
		obj_ptr<MongoCursor_base> vr;

		METHOD_INSTANCE(MongoCursor_base);
		METHOD_ENTER(1, 1);

		ARG(v8::Handle<v8::Object>, 0);

		hr = pInst->sort(v0, vr);

		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> MongoCursor_base::s_min(const v8::Arguments& args)
	{
		obj_ptr<MongoCursor_base> vr;

		METHOD_INSTANCE(MongoCursor_base);
		METHOD_ENTER(1, 1);

		ARG(v8::Handle<v8::Object>, 0);

		hr = pInst->min(v0, vr);

		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> MongoCursor_base::s_max(const v8::Arguments& args)
	{
		obj_ptr<MongoCursor_base> vr;

		METHOD_INSTANCE(MongoCursor_base);
		METHOD_ENTER(1, 1);

		ARG(v8::Handle<v8::Object>, 0);

		hr = pInst->max(v0, vr);

		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> MongoCursor_base::s__addSpecial(const v8::Arguments& args)
	{
		obj_ptr<MongoCursor_base> vr;

		METHOD_INSTANCE(MongoCursor_base);
		METHOD_ENTER(2, 2);

		ARG_String(0);
		ARG(v8::Handle<v8::Value>, 1);

		hr = pInst->_addSpecial(v0, v1, vr);

		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> MongoCursor_base::s_toArray(const v8::Arguments& args)
	{
		v8::Handle<v8::Array> vr;

		METHOD_INSTANCE(MongoCursor_base);
		METHOD_ENTER(0, 0);

		hr = pInst->toArray(vr);

		METHOD_RETURN();
	}

}

#endif

