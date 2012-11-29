/*
 * date.h
 *
 *  Created on: Jul 27, 2012
 *      Author: lion
 */

#include <string>

#ifndef DATE_H_
#define DATE_H_

namespace v8
{
namespace internal
{

class OS
{
public:
	static double TimeCurrentMillis();
	static double LocalTimeOffset();
};

}
}

namespace fibjs
{

class date_t
{
public:
	date_t(double v = 0) :
			d(v)
	{
	}

	date_t(const date_t& v) :
			d(v.d)
	{
	}

	date_t(v8::Handle<v8::Value> v) :
			d(0)
	{
		operator=(v);
	}

	void clear()
	{
		d = 0;
	}

	void now()
	{
		d = v8::internal::OS::TimeCurrentMillis();
	}

	date_t& operator=(double v)
	{
		d = v;
		return *this;
	}

	date_t& operator=(const date_t& v)
	{
		d = v.d;
		return *this;
	}

	date_t& operator=(v8::Handle<v8::Value> v)
	{
		d = v->NumberValue();
		return *this;
	}

	operator v8::Handle<v8::Value>() const
	{
		return v8::Date::New(d);
	}

	bool empty() const
	{
		return d == 0;
	}

	double diff(date_t d1)
	{
		return d - d1.d;
	}

	void parse(const char* str, int len = -1);
	void toGMTString(std::string& retVal, bool bTimeZone = false);
	void toString(std::string& retVal)
	{
		toGMTString(retVal, true);
	}
	void sqlString(std::string& retVal);
	static int LocalOffset()
	{
		return (int) v8::internal::OS::LocalTimeOffset() / 3600000;
	}

private:
	double d;
};

}

#endif /* DATE_H_ */
