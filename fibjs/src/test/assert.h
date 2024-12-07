/*
 * assert.g
 *
 *  Created on: Dec 8, 2024
 *      Author: lion
 */

#include "utils.h"

#pragma once

namespace fibjs {

class _msg {
public:
    _msg(exlib::string s0, const char* s1)
    {
        if (!s0.empty()) {
            msg = s0;
        } else {
            strs[0] = s1;
            strs[1] = NULL;
        }
    }

    _msg(exlib::string s0, const char* s1, v8::Local<v8::Value>& v1,
        const char* s2)
    {
        if (!s0.empty()) {
            msg = s0;
        } else {
            strs[0] = s1;
            strs[1] = s2;
            strs[2] = NULL;

            vs[0] = &v1;
        }
    }

    _msg(exlib::string s0, const char* s1, v8::Local<v8::Value>& v1,
        const char* s2, v8::Local<v8::Value>& v2, const char* s3 = "")
    {
        if (!s0.empty()) {
            msg = s0;
        } else {
            strs[0] = s1;
            strs[1] = s2;
            strs[2] = s3;
            strs[3] = NULL;

            vs[0] = &v1;
            vs[1] = &v2;
        }
    }

    _msg(exlib::string s0, const char* s1, v8::Local<v8::Value>& v1,
        const char* s2, v8::Local<v8::Value>& v2, const char* s3,
        v8::Local<v8::Value>& v3, const char* s4 = "")
    {
        if (!s0.empty()) {
            msg = s0;
        } else {
            strs[0] = s1;
            strs[1] = s2;
            strs[2] = s3;
            strs[3] = s4;
            strs[4] = NULL;

            vs[0] = &v1;
            vs[1] = &v2;
            vs[2] = &v3;
        }
    }

    _msg(exlib::string s0, const char* s1, v8::Local<v8::Value>& v1,
        const char* s2, v8::Local<v8::Value>& v2, const char* s3,
        v8::Local<v8::Value>& v3, const char* s4,
        v8::Local<v8::Value>& v4, const char* s5 = "")
    {
        if (!s0.empty()) {
            msg = s0;
        } else {
            strs[0] = s1;
            strs[1] = s2;
            strs[2] = s3;
            strs[3] = s4;
            strs[4] = s5;

            vs[0] = &v1;
            vs[1] = &v2;
            vs[2] = &v3;
            vs[3] = &v4;
        }
    }

    exlib::string str()
    {
        Isolate* isolate = Isolate::current();
        exlib::string str(msg);

        if (str.empty()) {
            str = strs[0];

            if (strs[1]) {
                str.append(json_format(isolate, *vs[0]));
                str.append(strs[1]);

                if (strs[2]) {
                    str.append(json_format(isolate, *vs[1]));
                    str.append(strs[2]);

                    if (strs[3]) {
                        str.append(json_format(isolate, *vs[2]));
                        str.append(strs[3]);

                        if (strs[4]) {
                            str.append(json_format(isolate, *vs[3]));
                            str.append(strs[4]);
                        }
                    }
                }
            }
        }

        return str;
    }

private:
    exlib::string msg;
    const char* strs[5];
    v8::Local<v8::Value>* vs[4];
};

inline result_t _test(bool value, _msg msg)
{
    if (!value) {
        ThrowError(msg.str());
        return CALL_E_JAVASCRIPT;
    }

    return 0;
}

}
