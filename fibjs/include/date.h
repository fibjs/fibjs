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
};
}
}

namespace fibjs
{

class date_t
{
public:
	date_t() :
			d(0)
	{
	}

	date_t(const date_t& v) :
			d(v.d)
	{
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

	void parse(const char* str, int len = -1);
	void toString(std::string& retVal);

private:
	double d;
};

}

#endif /* DATE_H_ */
