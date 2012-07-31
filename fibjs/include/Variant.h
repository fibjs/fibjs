/*
 * Variant.h
 *
 *  Created on: Jul 26, 2012
 *      Author: lion
 */

#include <v8/v8.h>
#include <string>
#include "date.h"

#ifndef VARIANT_H_
#define VARIANT_H_

namespace fibjs
{

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
		VT_String = 6
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

		m_type = VT_Null;
	}

	Variant& operator=(const Variant& v)
	{
		if (v.m_type == VT_String)
			return operator=(*(std::string*) v.m_Val.strVal);

		clear();
		m_type = v.m_type;
		m_Val.longVal = v.m_Val.longVal;

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

	Variant& operator=(v8::Handle<v8::Value> v)
	{
		clear();

		if (!v.IsEmpty() && !v->IsUndefined())
		{
			if (v->IsDate())
			{
				m_type = VT_Date;
				*(date_t*) m_Val.dateVal = v;
			}
			else if (v->IsBoolean())
			{
				m_type = VT_Boolean;
				m_Val.boolVal = v->BooleanValue();
			}
			else if (v->IsNumber())
			{
				double n = v->NumberValue();
				int64_t num = (int64_t) n;

				if (n == (double) num)
				{
					if (num >= -2147483648ll && num <= 2147483647ll)
					{
						m_type = VT_Integer;
						m_Val.intVal = (int32_t) num;
					}
					else
					{
						m_type = VT_Long;
						m_Val.longVal = num;
					}
				}
				else
				{
					m_type = VT_Number;
					m_Val.dblVal = n;
				}
			}
			else
			{
				v8::String::Utf8Value s(v);
				std::string str(*s, s.length());
				operator=(str);
			}
		}

		return *this;
	}

	Type type() const
	{
		return m_type;
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

	operator v8::Handle<v8::Value>() const
	{
		switch (m_type)
		{
		case VT_Null:
			return v8::Null();
		case VT_Boolean:
			return m_Val.boolVal ? v8::True() : v8::False();
		case VT_Integer:
			return v8::Int32::New(m_Val.intVal);
		case VT_Long:
			return v8::Number::New((double) m_Val.longVal);
		case VT_Number:
			return v8::Number::New(m_Val.dblVal);
		case VT_Date:
			return *(date_t*) m_Val.dateVal;
		case VT_String:
		{
			std::string& str = *(std::string*) m_Val.strVal;
			return v8::String::New(str.c_str(), (int) str.length());
		}
		}

		return v8::Undefined();
	}

	void parseNumber(const char* str, int len = -1);
	void parseDate(const char* str, int len = -1)
	{
		m_type = VT_Date;
		((date_t*) m_Val.dateVal)->parse(str, len);
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
	} m_Val;
};

} /* namespace fibjs */
#endif /* VARIANT_H_ */
