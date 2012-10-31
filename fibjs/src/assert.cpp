/*
 * assert.cpp
 *
 *  Created on: Apr 7, 2012
 *      Author: lion
 */

#include "ifs/assert.h"
#include "ifs/encoding.h"
#include <log4cpp/Category.hh>
#include <sstream>

namespace fibjs
{

static bool s_bThrow;

result_t assert_base::throwAssert(bool bThrow)
{
	s_bThrow = bThrow;
	return 0;
}

result_t assert_base::ok(bool value, const char* msg)
{
	if (!value)
	{
		std::stringstream strBuffer;

		strBuffer << "assert: " << msg;

		if (s_bThrow)
			ThrowError(strBuffer.str().c_str());
		else
		{
			strBuffer << traceInfo();
			asyncLog(log4cpp::Priority::WARN, strBuffer.str());
		}
	}

	return 0;
}

bool regexpEquals(v8::Handle<v8::Value> actual, v8::Handle<v8::Value> expected)
{
	v8::Handle<v8::RegExp> re1 = v8::Handle<v8::RegExp>::Cast(actual);
	v8::Handle<v8::String> src1 = re1->GetSource();
	v8::RegExp::Flags flgs1 = re1->GetFlags();

	v8::Handle<v8::RegExp> re2 = v8::Handle<v8::RegExp>::Cast(expected);
	v8::Handle<v8::String> src2 = re2->GetSource();
	v8::RegExp::Flags flgs2 = re2->GetFlags();

	return src1->StrictEquals(src2) && flgs1 == flgs2;
}

bool valueEquals(v8::Handle<v8::Value> actual, v8::Handle<v8::Value> expected)
{
	if (actual->IsDate() && expected->IsDate())
		return actual->NumberValue() == expected->NumberValue();

	if (actual->IsRegExp() && expected->IsRegExp())
		return regexpEquals(actual, expected);

	return actual->Equals(expected);
}

bool valueStrictEquals(v8::Handle<v8::Value> actual,
		v8::Handle<v8::Value> expected)
{
	if (actual->IsDate() && expected->IsDate())
		return actual->NumberValue() == expected->NumberValue();

	if (actual->IsRegExp() && expected->IsRegExp())
		return regexpEquals(actual, expected);

	return actual->StrictEquals(expected);
}

inline std::string buildString(v8::Handle<v8::Value> actual,
		v8::Handle<v8::Value> expected, const char* op)
{
	std::string str;
	std::string s;

	encoding_base::jsonEncode(expected, s);
	str = s;
	str += ' ';
	str += op;
	str += ' ';
	encoding_base::jsonEncode(actual, s);
	str += s;

	return str;
}

result_t assert_base::equal(v8::Handle<v8::Value> actual,
		v8::Handle<v8::Value> expected, const char* msg)
{
	if (!valueEquals(actual, expected))
	{
		std::string str;

		if (!*msg)
		{
			str = buildString(expected, actual, "==");
			msg = str.c_str();
		}

		ok(false, msg);
	}
	return 0;
}

result_t assert_base::notEqual(v8::Handle<v8::Value> actual,
		v8::Handle<v8::Value> expected, const char* msg)
{
	if (valueEquals(actual, expected))
	{
		std::string str;

		if (!*msg)
		{
			str = buildString(expected, actual, "!=");
			msg = str.c_str();
		}

		ok(false, msg);
	}
	return 0;
}

result_t assert_base::strictEqual(v8::Handle<v8::Value> actual,
		v8::Handle<v8::Value> expected, const char* msg)
{
	if (!valueStrictEquals(actual, expected))
	{
		std::string str;

		if (!*msg)
		{
			str = buildString(expected, actual, "===");
			msg = str.c_str();
		}

		ok(false, msg);
	}
	return 0;
}

result_t assert_base::notStrictEqual(v8::Handle<v8::Value> actual,
		v8::Handle<v8::Value> expected, const char* msg)
{
	if (valueStrictEquals(actual, expected))
	{
		std::string str;

		if (!*msg)
		{
			str = buildString(expected, actual, "!==");
			msg = str.c_str();
		}

		ok(false, msg);
	}
	return 0;
}

result_t assert_base::throws(v8::Handle<v8::Function> block, const char* msg)
{
	v8::TryCatch try_catch;
	block->Call(block, 0, NULL);
	if (!try_catch.HasCaught())
	{
		std::string str = "Missing expected exception. ";
		str += msg;
		ok(false, str.c_str());
	}

	return 0;
}

result_t assert_base::doesNotThrow(v8::Handle<v8::Function> block,
		const char* msg)
{
	v8::TryCatch try_catch;
	block->Call(block, 0, NULL);
	if (try_catch.HasCaught())
	{
		std::string str = "Got unwanted exception. ";
		str += msg;
		ok(false, str.c_str());
	}

	return 0;
}

}
