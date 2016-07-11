/*
 * Expect.h
 *
 *  Created on: Nov 8, 2013
 *      Author: lion
 */

#include "ifs/Expect.h"

#ifndef EXPECT_H_
#define EXPECT_H_

namespace fibjs
{

class Expect: public Expect_base
{
public:
    Expect(v8::Local<v8::Value> actual, const char *msg) :
        m_msg(msg), m_not(false), m_deep(false)
    {
        m_actual = actual;
    }

public:
    // Expect_base
    virtual result_t get_to(obj_ptr<Expect_base> &retVal);
    virtual result_t get_be(obj_ptr<Expect_base> &retVal);
    virtual result_t get_been(obj_ptr<Expect_base> &retVal);
    virtual result_t get_is(obj_ptr<Expect_base> &retVal);
    virtual result_t get_that(obj_ptr<Expect_base> &retVal);
    virtual result_t get_and(obj_ptr<Expect_base> &retVal);
    virtual result_t get_have(obj_ptr<Expect_base> &retVal);
    virtual result_t get_with(obj_ptr<Expect_base> &retVal);
    virtual result_t get_at(obj_ptr<Expect_base> &retVal);
    virtual result_t get_of(obj_ptr<Expect_base> &retVal);
    virtual result_t get_same(obj_ptr<Expect_base> &retVal);
    virtual result_t get_not(obj_ptr<Expect_base> &retVal);
    virtual result_t get_deep(obj_ptr<Expect_base> &retVal);
    virtual result_t a(const char *type);
    virtual result_t an(const char *type);
    virtual result_t get_ok(bool &retVal);
    virtual result_t get_true(bool &retVal);
    virtual result_t get_false(bool &retVal);
    virtual result_t get_null(bool &retVal);
    virtual result_t get_undefined(bool &retVal);
    virtual result_t get_function(bool &retVal);
    virtual result_t get_object(bool &retVal);
    virtual result_t get_array(bool &retVal);
    virtual result_t get_string(bool &retVal);
    virtual result_t get_number(bool &retVal);
    virtual result_t get_boolean(bool &retVal);
    virtual result_t get_exist(bool &retVal);
    virtual result_t equal(v8::Local<v8::Value> expected);
    virtual result_t eql(v8::Local<v8::Value> expected);
    virtual result_t above(v8::Local<v8::Value> expected);
    virtual result_t greaterThan(v8::Local<v8::Value> expected);
    virtual result_t least(v8::Local<v8::Value> expected);
    virtual result_t below(v8::Local<v8::Value> expected);
    virtual result_t lessThan(v8::Local<v8::Value> expected);
    virtual result_t most(v8::Local<v8::Value> expected);
    virtual result_t property(v8::Local<v8::Value> prop);
    virtual result_t property(v8::Local<v8::Value> prop, v8::Local<v8::Value> value);
    virtual result_t closeTo(v8::Local<v8::Value> expected,
                             v8::Local<v8::Value> delta);

private:
    VariantEx m_actual;
    qstring m_msg;
    bool m_not;
    bool m_deep;
};

}

#endif /* EXPECT_H_ */
