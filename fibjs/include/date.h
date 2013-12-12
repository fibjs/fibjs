/*
 * date.h
 *
 *  Created on: Jul 27, 2012
 *      Author: lion
 */

#include <string>
#include <exlib/fiber.h>

#ifndef DATE_H_
#define DATE_H_

namespace v8
{
namespace internal
{

class OS
{
public:
	static double LocalTimeOffset();
	static double DaylightSavingsOffset(double time);
	static const char* LocalTimezone(double time);
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
		d = exlib::FastCurrentMillis();
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

	void toLocal();
	void toUTC();

	void parse(const char* str, int len = -1);
	void toGMTString(std::string& retVal);
	void sqlString(std::string& retVal);
	static int LocalOffset()
	{
		return (int) v8::internal::OS::LocalTimeOffset() / 36000;
	}

private:
	double d;
};

}

#endif /* DATE_H_ */
