/*
 * assert.cpp
 *
 *  Created on: Apr 7, 2012
 *      Author: lion
 */

#include "utils.h"
#include "object.h"
#include "ifs/assert_strict.h"
#include "ifs/assert.h"
#include "ifs/encoding.h"
#include "ifs/util.h"
#include "QuickArray.h"
#include "../util/util.h"
#include "assert.h"

namespace fibjs {

result_t assert_strict_base::_function(v8::Local<v8::Value> actual, exlib::string msg)
{
    return assert_base::_function(actual, msg);
}

result_t assert_strict_base::notOk(v8::Local<v8::Value> actual, exlib::string msg)
{
    return assert_base::notOk(actual, msg);
}

result_t assert_strict_base::equal(v8::Local<v8::Value> actual,
    v8::Local<v8::Value> expected, exlib::string msg)
{
    return assert_base::strictEqual(actual, expected, msg);
}

result_t assert_strict_base::notEqual(v8::Local<v8::Value> actual,
    v8::Local<v8::Value> expected, exlib::string msg)
{
    return assert_base::notStrictEqual(actual, expected, msg);
}

result_t assert_strict_base::strictEqual(v8::Local<v8::Value> actual,
    v8::Local<v8::Value> expected, exlib::string msg)
{
    return assert_base::strictEqual(actual, expected, msg);
}

result_t assert_strict_base::notStrictEqual(v8::Local<v8::Value> actual,
    v8::Local<v8::Value> expected, exlib::string msg)
{
    return assert_base::notStrictEqual(actual, expected, msg);
}

result_t assert_strict_base::deepEqual(v8::Local<v8::Value> actual,
    v8::Local<v8::Value> expected, exlib::string msg)
{
    return assert_base::deepStrictEqual(actual, expected, msg);
}

result_t assert_strict_base::notDeepEqual(v8::Local<v8::Value> actual,
    v8::Local<v8::Value> expected, exlib::string msg)
{
    return assert_base::notDeepStrictEqual(actual, expected, msg);
}

result_t assert_strict_base::deepStrictEqual(v8::Local<v8::Value> actual,
    v8::Local<v8::Value> expected, exlib::string msg)
{
    return assert_base::deepStrictEqual(actual, expected, msg);
}

result_t assert_strict_base::notDeepStrictEqual(v8::Local<v8::Value> actual,
    v8::Local<v8::Value> expected, exlib::string msg)
{
    return assert_base::notDeepStrictEqual(actual, expected, msg);
}

result_t assert_strict_base::match(exlib::string actual, v8::Local<v8::RegExp> expected, exlib::string msg)
{
    return assert_base::match(actual, expected, msg);
}

result_t assert_strict_base::doesNotMatch(exlib::string actual, v8::Local<v8::RegExp> expected, exlib::string msg)
{
    return assert_base::doesNotMatch(actual, expected, msg);
}

result_t assert_strict_base::closeTo(v8::Local<v8::Value> actual,
    v8::Local<v8::Value> expected, v8::Local<v8::Value> delta,
    exlib::string msg)
{
    return assert_base::closeTo(actual, expected, delta, msg);
}

result_t assert_strict_base::notCloseTo(v8::Local<v8::Value> actual,
    v8::Local<v8::Value> expected, v8::Local<v8::Value> delta,
    exlib::string msg)
{
    return assert_base::notCloseTo(actual, expected, delta, msg);
}

result_t assert_strict_base::lessThan(v8::Local<v8::Value> actual,
    v8::Local<v8::Value> expected, exlib::string msg)
{
    return assert_base::lessThan(actual, expected, msg);
}

result_t assert_strict_base::notLessThan(v8::Local<v8::Value> actual,
    v8::Local<v8::Value> expected, exlib::string msg)
{
    return assert_base::notLessThan(actual, expected, msg);
}

result_t assert_strict_base::greaterThan(v8::Local<v8::Value> actual,
    v8::Local<v8::Value> expected, exlib::string msg)
{
    return assert_base::greaterThan(actual, expected, msg);
}

result_t assert_strict_base::notGreaterThan(v8::Local<v8::Value> actual,
    v8::Local<v8::Value> expected, exlib::string msg)
{
    return assert_base::notGreaterThan(actual, expected, msg);
}

result_t assert_strict_base::exist(v8::Local<v8::Value> actual, exlib::string msg)
{
    return assert_base::exist(actual, msg);
}

result_t assert_strict_base::notExist(v8::Local<v8::Value> actual, exlib::string msg)
{
    return assert_base::notExist(actual, msg);
}

result_t assert_strict_base::isTrue(v8::Local<v8::Value> actual, exlib::string msg)
{
    return assert_base::isTrue(actual, msg);
}

result_t assert_strict_base::isNotTrue(v8::Local<v8::Value> actual, exlib::string msg)
{
    return assert_base::isNotTrue(actual, msg);
}

result_t assert_strict_base::isFalse(v8::Local<v8::Value> actual, exlib::string msg)
{
    return assert_base::isFalse(actual, msg);
}

result_t assert_strict_base::isNotFalse(v8::Local<v8::Value> actual, exlib::string msg)
{
    return assert_base::isNotFalse(actual, msg);
}

result_t assert_strict_base::isNull(v8::Local<v8::Value> actual, exlib::string msg)
{
    return assert_base::isNull(actual, msg);
}

result_t assert_strict_base::isNotNull(v8::Local<v8::Value> actual, exlib::string msg)
{
    return assert_base::isNotNull(actual, msg);
}

result_t assert_strict_base::isUndefined(v8::Local<v8::Value> actual, exlib::string msg)
{
    return assert_base::isUndefined(actual, msg);
}

result_t assert_strict_base::isDefined(v8::Local<v8::Value> actual, exlib::string msg)
{
    return assert_base::isDefined(actual, msg);
}

result_t assert_strict_base::isFunction(v8::Local<v8::Value> actual, exlib::string msg)
{
    return assert_base::isFunction(actual, msg);
}

result_t assert_strict_base::isNotFunction(v8::Local<v8::Value> actual, exlib::string msg)
{
    return assert_base::isNotFunction(actual, msg);
}

result_t assert_strict_base::isObject(v8::Local<v8::Value> actual, exlib::string msg)
{
    return assert_base::isObject(actual, msg);
}

result_t assert_strict_base::isNotObject(v8::Local<v8::Value> actual, exlib::string msg)
{
    return assert_base::isNotObject(actual, msg);
}

result_t assert_strict_base::isArray(v8::Local<v8::Value> actual, exlib::string msg)
{
    return assert_base::isArray(actual, msg);
}

result_t assert_strict_base::isNotArray(v8::Local<v8::Value> actual, exlib::string msg)
{
    return assert_base::isNotArray(actual, msg);
}

result_t assert_strict_base::isString(v8::Local<v8::Value> actual, exlib::string msg)
{
    return assert_base::isString(actual, msg);
}

result_t assert_strict_base::isNotString(v8::Local<v8::Value> actual, exlib::string msg)
{
    return assert_base::isNotString(actual, msg);
}

result_t assert_strict_base::isNumber(v8::Local<v8::Value> actual, exlib::string msg)
{
    return assert_base::isNumber(actual, msg);
}

result_t assert_strict_base::isNotNumber(v8::Local<v8::Value> actual, exlib::string msg)
{
    return assert_base::isNotNumber(actual, msg);
}

result_t assert_strict_base::isBoolean(v8::Local<v8::Value> actual, exlib::string msg)
{
    return assert_base::isBoolean(actual, msg);
}

result_t assert_strict_base::isNotBoolean(v8::Local<v8::Value> actual, exlib::string msg)
{
    return assert_base::isNotBoolean(actual, msg);
}

result_t assert_strict_base::typeOf(v8::Local<v8::Value> actual, exlib::string type,
    exlib::string msg)
{
    return assert_base::typeOf(actual, type, msg);
}

result_t assert_strict_base::notTypeOf(v8::Local<v8::Value> actual, exlib::string type,
    exlib::string msg)
{
    return assert_base::notTypeOf(actual, type, msg);
}

result_t assert_strict_base::property(v8::Local<v8::Value> object,
    v8::Local<v8::Value> prop, exlib::string msg)
{
    return assert_base::property(object, prop, msg);
}

result_t assert_strict_base::notProperty(v8::Local<v8::Value> object,
    v8::Local<v8::Value> prop, exlib::string msg)
{
    return assert_base::notProperty(object, prop, msg);
}

result_t assert_strict_base::deepProperty(v8::Local<v8::Value> object,
    v8::Local<v8::Value> prop, exlib::string msg)
{
    return assert_base::deepProperty(object, prop, msg);
}

result_t assert_strict_base::notDeepProperty(v8::Local<v8::Value> object,
    v8::Local<v8::Value> prop, exlib::string msg)
{
    return assert_base::notDeepProperty(object, prop, msg);
}

result_t assert_strict_base::propertyVal(v8::Local<v8::Value> object,
    v8::Local<v8::Value> prop, v8::Local<v8::Value> value,
    exlib::string msg)
{
    return assert_base::propertyVal(object, prop, value, msg);
}

result_t assert_strict_base::propertyNotVal(v8::Local<v8::Value> object,
    v8::Local<v8::Value> prop, v8::Local<v8::Value> value,
    exlib::string msg)
{
    return assert_base::propertyNotVal(object, prop, value, msg);
}

result_t assert_strict_base::deepPropertyVal(v8::Local<v8::Value> object,
    v8::Local<v8::Value> prop, v8::Local<v8::Value> value,
    exlib::string msg)
{
    return assert_base::deepPropertyVal(object, prop, value, msg);
}

result_t assert_strict_base::deepPropertyNotVal(v8::Local<v8::Value> object,
    v8::Local<v8::Value> prop, v8::Local<v8::Value> value,
    exlib::string msg)
{
    return assert_base::deepPropertyNotVal(object, prop, value, msg);
}

result_t assert_strict_base::ifError(v8::Local<v8::Value> v)
{
    return assert_base::ifError(v);
}

result_t assert_strict_base::get_AssertionError(v8::Local<v8::Function>& retVal)
{
    return assert_base::get_AssertionError(retVal);
}

result_t assert_strict_base::throws(v8::Local<v8::Function> block, v8::Local<v8::Value> error, exlib::string msg)
{
    return assert_base::throws(block, error, msg);
}

result_t assert_strict_base::throws(v8::Local<v8::Function> block, exlib::string msg)
{
    return assert_base::throws(block, msg);
}

result_t assert_strict_base::doesNotThrow(v8::Local<v8::Function> block, exlib::string msg)
{
    return assert_base::doesNotThrow(block, msg);
}

result_t assert_strict_base::rejects(v8::Local<v8::Promise> result, v8::Local<v8::Value> error, exlib::string msg, v8::Local<v8::Promise>& retVal)
{
    return assert_base::rejects(result, error, msg, retVal);
}

result_t assert_strict_base::rejects(v8::Local<v8::Promise> result, exlib::string msg, v8::Local<v8::Promise>& retVal)
{
    return assert_base::rejects(result, msg, retVal);
}

result_t assert_strict_base::rejects(v8::Local<v8::Function> block, v8::Local<v8::Value> error, exlib::string msg, v8::Local<v8::Promise>& retVal)
{
    return assert_base::rejects(block, error, msg, retVal);
}

result_t assert_strict_base::rejects(v8::Local<v8::Function> block, exlib::string msg, v8::Local<v8::Promise>& retVal)
{
    return assert_base::rejects(block, msg, retVal);
}

}
