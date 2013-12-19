/*
 * Expect.cpp
 *
 *  Created on: Nov 8, 2013
 *      Author: lion
 */

#include "Expect.h"
#include "ifs/assert.h"

namespace fibjs
{

result_t Expect::get_to(obj_ptr<Expect_base> &retVal)
{
    retVal = this;
    return 0;
}

result_t Expect::get_be(obj_ptr<Expect_base> &retVal)
{
    retVal = this;
    return 0;
}

result_t Expect::get_been(obj_ptr<Expect_base> &retVal)
{
    retVal = this;
    return 0;
}

result_t Expect::get_is(obj_ptr<Expect_base> &retVal)
{
    retVal = this;
    return 0;
}

result_t Expect::get_that(obj_ptr<Expect_base> &retVal)
{
    retVal = this;
    return 0;
}

result_t Expect::get_and(obj_ptr<Expect_base> &retVal)
{
    retVal = this;
    return 0;
}

result_t Expect::get_have(obj_ptr<Expect_base> &retVal)
{
    retVal = this;
    return 0;
}

result_t Expect::get_with(obj_ptr<Expect_base> &retVal)
{
    retVal = this;
    return 0;
}

result_t Expect::get_at(obj_ptr<Expect_base> &retVal)
{
    retVal = this;
    return 0;
}

result_t Expect::get_of(obj_ptr<Expect_base> &retVal)
{
    retVal = this;
    return 0;
}

result_t Expect::get_same(obj_ptr<Expect_base> &retVal)
{
    retVal = this;
    return 0;
}

result_t Expect::get_not(obj_ptr<Expect_base> &retVal)
{
    m_not = !m_not;
    retVal = this;
    return 0;
}

result_t Expect::get_deep(obj_ptr<Expect_base> &retVal)
{
    m_deep = true;
    retVal = this;
    return 0;
}

result_t Expect::a(const char *type)
{
    return !m_not ?
           assert_base::typeOf(
               v8::Handle < v8::Value > ::New(isolate, m_actual), type,
               m_msg.c_str()) :
           assert_base::notTypeOf(
               v8::Handle < v8::Value > ::New(isolate, m_actual), type,
               m_msg.c_str());
    return 0;
}

result_t Expect::an(const char *type)
{
    return a(type);
}

result_t Expect::get_ok(bool &retVal)
{
    return !m_not ?
           assert_base::ok(v8::Handle < v8::Value > ::New(isolate, m_actual),
                           m_msg.c_str()) :
           assert_base::notOk(
               v8::Handle < v8::Value > ::New(isolate, m_actual),
               m_msg.c_str());
}

result_t Expect::get_true(bool &retVal)
{
    return !m_not ?
           assert_base::isTrue(
               v8::Handle < v8::Value > ::New(isolate, m_actual),
               m_msg.c_str()) :
           assert_base::isNotTrue(
               v8::Handle < v8::Value > ::New(isolate, m_actual),
               m_msg.c_str());
}

result_t Expect::get_false(bool &retVal)
{
    return !m_not ?
           assert_base::isFalse(
               v8::Handle < v8::Value > ::New(isolate, m_actual),
               m_msg.c_str()) :
           assert_base::isNotFalse(
               v8::Handle < v8::Value > ::New(isolate, m_actual),
               m_msg.c_str());
}

result_t Expect::get_null(bool &retVal)
{
    return !m_not ?
           assert_base::isNull(
               v8::Handle < v8::Value > ::New(isolate, m_actual),
               m_msg.c_str()) :
           assert_base::isNotNull(
               v8::Handle < v8::Value > ::New(isolate, m_actual),
               m_msg.c_str());
}

result_t Expect::get_undefined(bool &retVal)
{
    return !m_not ?
           assert_base::isUndefined(
               v8::Handle < v8::Value > ::New(isolate, m_actual),
               m_msg.c_str()) :
           assert_base::isDefined(
               v8::Handle < v8::Value > ::New(isolate, m_actual),
               m_msg.c_str());
}

result_t Expect::get_function(bool &retVal)
{
    return !m_not ?
           assert_base::isFunction(
               v8::Handle < v8::Value > ::New(isolate, m_actual),
               m_msg.c_str()) :
           assert_base::isNotFunction(
               v8::Handle < v8::Value > ::New(isolate, m_actual),
               m_msg.c_str());
}

result_t Expect::get_object(bool &retVal)
{
    return !m_not ?
           assert_base::isObject(
               v8::Handle < v8::Value > ::New(isolate, m_actual),
               m_msg.c_str()) :
           assert_base::isNotObject(
               v8::Handle < v8::Value > ::New(isolate, m_actual),
               m_msg.c_str());
}

result_t Expect::get_array(bool &retVal)
{
    return !m_not ?
           assert_base::isArray(
               v8::Handle < v8::Value > ::New(isolate, m_actual),
               m_msg.c_str()) :
           assert_base::isNotArray(
               v8::Handle < v8::Value > ::New(isolate, m_actual),
               m_msg.c_str());
}

result_t Expect::get_string(bool &retVal)
{
    return !m_not ?
           assert_base::isString(
               v8::Handle < v8::Value > ::New(isolate, m_actual),
               m_msg.c_str()) :
           assert_base::isNotString(
               v8::Handle < v8::Value > ::New(isolate, m_actual),
               m_msg.c_str());
}

result_t Expect::get_number(bool &retVal)
{
    return !m_not ?
           assert_base::isNumber(
               v8::Handle < v8::Value > ::New(isolate, m_actual),
               m_msg.c_str()) :
           assert_base::isNotNumber(
               v8::Handle < v8::Value > ::New(isolate, m_actual),
               m_msg.c_str());
}

result_t Expect::get_boolean(bool &retVal)
{
    return !m_not ?
           assert_base::isBoolean(
               v8::Handle < v8::Value > ::New(isolate, m_actual),
               m_msg.c_str()) :
           assert_base::isNotBoolean(
               v8::Handle < v8::Value > ::New(isolate, m_actual),
               m_msg.c_str());
}

result_t Expect::get_exist(bool &retVal)
{
    return !m_not ?
           assert_base::exist(
               v8::Handle < v8::Value > ::New(isolate, m_actual),
               m_msg.c_str()) :
           assert_base::notExist(
               v8::Handle < v8::Value > ::New(isolate, m_actual),
               m_msg.c_str());
}

result_t Expect::equal(v8::Handle<v8::Value> expected)
{
    if (m_deep)
        return !m_not ?
               assert_base::deepEqual(
                   v8::Handle < v8::Value > ::New(isolate, m_actual),
                   expected, m_msg.c_str()) :
               assert_base::notDeepEqual(
                   v8::Handle < v8::Value > ::New(isolate, m_actual),
                   expected, m_msg.c_str());

    return !m_not ?
           assert_base::strictEqual(
               v8::Handle < v8::Value > ::New(isolate, m_actual), expected,
               m_msg.c_str()) :
           assert_base::notStrictEqual(
               v8::Handle < v8::Value > ::New(isolate, m_actual), expected,
               m_msg.c_str());
}

result_t Expect::eql(v8::Handle<v8::Value> expected)
{
    m_deep = true;
    return equal(expected);
}

result_t Expect::above(v8::Handle<v8::Value> expected)
{
    return !m_not ?
           assert_base::greaterThan(
               v8::Handle < v8::Value > ::New(isolate, m_actual), expected,
               m_msg.c_str()) :
           assert_base::notGreaterThan(
               v8::Handle < v8::Value > ::New(isolate, m_actual), expected,
               m_msg.c_str());
}

result_t Expect::greaterThan(v8::Handle<v8::Value> expected)
{
    return above(expected);
}

result_t Expect::least(v8::Handle<v8::Value> expected)
{
    return !m_not ?
           assert_base::notLessThan(
               v8::Handle < v8::Value > ::New(isolate, m_actual), expected,
               m_msg.c_str()) :
           assert_base::lessThan(
               v8::Handle < v8::Value > ::New(isolate, m_actual), expected,
               m_msg.c_str());
}

result_t Expect::below(v8::Handle<v8::Value> expected)
{
    return !m_not ?
           assert_base::lessThan(
               v8::Handle < v8::Value > ::New(isolate, m_actual), expected,
               m_msg.c_str()) :
           assert_base::notLessThan(
               v8::Handle < v8::Value > ::New(isolate, m_actual), expected,
               m_msg.c_str());
}

result_t Expect::lessThan(v8::Handle<v8::Value> expected)
{
    return below(expected);
}

result_t Expect::most(v8::Handle<v8::Value> expected)
{
    return !m_not ?
           assert_base::notGreaterThan(
               v8::Handle < v8::Value > ::New(isolate, m_actual), expected,
               m_msg.c_str()) :
           assert_base::greaterThan(
               v8::Handle < v8::Value > ::New(isolate, m_actual), expected,
               m_msg.c_str());
}

result_t Expect::property(v8::Handle<v8::Value> prop)
{
    if (m_deep)
        return !m_not ?
               assert_base::deepProperty(
                   v8::Handle < v8::Value > ::New(isolate, m_actual), prop,
                   m_msg.c_str()) :
               assert_base::notDeepProperty(
                   v8::Handle < v8::Value > ::New(isolate, m_actual), prop,
                   m_msg.c_str());

    return !m_not ?
           assert_base::property(
               v8::Handle < v8::Value > ::New(isolate, m_actual), prop,
               m_msg.c_str()) :
           assert_base::notProperty(
               v8::Handle < v8::Value > ::New(isolate, m_actual), prop,
               m_msg.c_str());
}

result_t Expect::property(v8::Handle<v8::Value> prop,
                          v8::Handle<v8::Value> value)
{
    if (m_deep)
        return !m_not ?
               assert_base::deepPropertyVal(
                   v8::Handle < v8::Value > ::New(isolate, m_actual), prop,
                   value, m_msg.c_str()) :
               assert_base::deepPropertyNotVal(
                   v8::Handle < v8::Value > ::New(isolate, m_actual), prop,
                   value, m_msg.c_str());

    return !m_not ?
           assert_base::propertyVal(
               v8::Handle < v8::Value > ::New(isolate, m_actual), prop,
               value, m_msg.c_str()) :
           assert_base::propertyNotVal(
               v8::Handle < v8::Value > ::New(isolate, m_actual), prop,
               value, m_msg.c_str());
}

result_t Expect::closeTo(v8::Handle<v8::Value> expected,
                         v8::Handle<v8::Value> delta)
{
    return !m_not ?
           assert_base::closeTo(
               v8::Handle < v8::Value > ::New(isolate, m_actual), expected,
               delta, m_msg.c_str()) :
           assert_base::notCloseTo(
               v8::Handle < v8::Value > ::New(isolate, m_actual), expected,
               delta, m_msg.c_str());
}

}
