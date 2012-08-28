/*
 * Variant.h
 *
 *  Created on: Jul 26, 2012
 *      Author: lion
 */

#include <v8/v8.h>
#include <string>
#include "date.h"
#include "obj_ptr.h"

#ifndef VARIANT_H_
#define VARIANT_H_

namespace fibjs
{

inline bool IsEmpty(v8::Handle<v8::Value>& v)
{
	return v.IsEmpty() || v->IsUndefined() || v->IsNull();
}

class object_base;

class Variant
{
public:
	enum Type
	{
		VT_Null = 0,
		VT_Boolean = 1,
		VT_Integer = 2,
		VT_Long = 3,
		VT_Number = 4,
		VT_Date = 5,
		VT_String = 6,
		VT_Object = 7
	};

public:
	Variant() :
			m_type(VT_Null)
	{
	}

	Variant(const Variant& v) :
			m_type(VT_Null)
	{
		operator=(v);
	}

	~Variant()
	{
		clear();
	}

	void clear()
	{
		if (m_type == VT_String)
			((std::string*) m_Val.strVal)->~basic_string();
		else if (m_type == VT_Object && m_Val.objVal)
			m_Val.objVal->Unref();

		m_type = VT_Null;
	}

	Variant& operator=(const Variant& v)
	{
		if (v.m_type == VT_String)
			return operator=(*(std::string*) v.m_Val.strVal);

		if (v.m_type == VT_Object)
			return operator=(v.m_Val.objVal);

		clear();
		m_type = v.m_type;
		m_Val.longVal = v.m_Val.longVal;

		return *this;
	}

	Variant& operator=(int32_t v)
	{
		clear();

		m_type = VT_Integer;
		m_Val.intVal = v;

		return *this;
	}

	Variant& operator=(int64_t v)
	{
		clear();

		if (v >= -2147483648ll && v <= 2147483647ll)
		{
			m_type = VT_Integer;
			m_Val.intVal = (int32_t) v;
		}
		else
		{
			m_type = VT_Long;
			m_Val.longVal = v;
		}

		return *this;
	}

	Variant& operator=(double v)
	{
		clear();

		m_type = VT_Number;
		m_Val.dblVal = v;

		return *this;
	}

	Variant& operator=(date_t& v)
	{
		clear();

		m_type = VT_Date;
		*(date_t*) m_Val.dateVal = v;

		return *this;
	}

	Variant& operator=(const std::string& v)
	{
		if (m_type != VT_String)
		{
			clear();
			m_type = VT_String;
			new (((std::string*) m_Val.strVal)) std::string(v);
		}
		else
			*(std::string*) m_Val.strVal = v;

		return *this;
	}

	Variant& operator=(const char* v)
	{
		std::string s(v);

		return operator=(s);
	}

	Variant& operator=(object_base* v)
	{
		return operator=((obj_base*) v);
	}

	bool assign(v8::Handle<v8::Value> v);

	Type type() const
	{
		return m_type;
	}

	size_t size() const
	{
		if (m_type != VT_String)
			return sizeof(Variant);

		return sizeof(Variant) + ((std::string*) m_Val.strVal)->length();
	}

	operator bool() const
	{
		return m_Val.boolVal;
	}

	operator int32_t() const
	{
		return m_Val.intVal;
	}

	operator int64_t() const
	{
		return m_Val.longVal;
	}

	operator double() const
	{
		return m_Val.dblVal;
	}

	operator const date_t&() const
	{
		return *(date_t*) m_Val.dateVal;
	}

	operator const std::string&() const
	{
		return *(std::string*) m_Val.strVal;
	}

	operator const object_base*() const
	{
		return (object_base*) m_Val.objVal;
	}

	operator v8::Handle<v8::Value>() const;

	void parseNumber(const char* str, int len = -1);
	void parseDate(const char* str, int len = -1)
	{
		m_type = VT_Date;
		((date_t*) m_Val.dateVal)->parse(str, len);
	}

	bool toString(std::string& retVal);

private:
	Variant& operator=(obj_base* v)
	{
		clear();

		m_type = VT_Object;
		m_Val.objVal = v;
		if (m_Val.objVal)
			m_Val.objVal->Ref();

		return *this;
	}

private:
	Type m_type;
	union
	{
		bool boolVal;
		int32_t intVal;
		int64_t longVal;
		double dblVal;
		char dateVal[sizeof(date_t)];
		char strVal[sizeof(std::string)];
		obj_base* objVal;
	} m_Val;
};

} /* namespace fibjs */
#endif /* VARIANT_H_ */
