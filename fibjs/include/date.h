/*
 * date.h
 *
 *  Created on: Jul 27, 2012
 *      Author: lion
 */

#pragma once

#include <string>
#include <stdint.h>
#include <chrono>

namespace fibjs {

class date_t {
public:
    enum {
        _YEAR = 0,
        _MONTH = 1,
        _DAY = 2,
        _HOUR = 3,
        _MINUTE = 4,
        _SECOND = 5,
        _MICROSECOND = 6
    };

    class Part {
    public:
        int32_t wYear, wMonth, wHour, wMinute,
            wSecond, wDayOfWeek, wDay, wMillisecond;
    };

public:
    date_t(double v = NAN)
        : d(v)
    {
    }

    date_t(const date_t& v)
        : d(v.d)
    {
    }

    date_t(v8::Local<v8::Value> v)
        : d(NAN)
    {
        operator=(v);
    }

    void clear()
    {
        d = NAN;
    }

    void now()
    {
        d = (double)std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now().time_since_epoch())
                .count();
    }

    void create(int32_t Y, int32_t M, int32_t D, int32_t h, int32_t m, int32_t s, int32_t ms);
    void fromDosTime(int32_t tm);

    Part getdate() const;

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

    date_t& operator=(v8::Local<v8::Value> v)
    {
        d = Isolate::current()->toNumber(v);
        return *this;
    }

    template <typename ts>
    void set_timestamp(const ts& v)
    {
        d = (double)v.tv_sec * 1000 + (double)v.tv_nsec / 1000000.0;
    }

    template <typename ts>
    void get_timestamp(ts& v)
    {
        v.tv_sec = (int64_t)floor(d / 1000);
        v.tv_nsec = (uint32_t)((d - v.tv_sec * 1000) * 1000000);

        int32_t sec1 = (int32_t)floor((double)v.tv_nsec / 1000000000);
        v.tv_sec += sec1;
        v.tv_nsec -= (uint32_t)(sec1 * 1000000000);
    }

    v8::Local<v8::Value> value(v8::Isolate* isolate) const
    {
        v8::Local<v8::Value> v;

        v8::Date::New(isolate->GetCurrentContext(), d).ToLocal(&v);
        return v;
    }

    bool empty() const
    {
        return std::isnan(d);
    }

    double diff(date_t d1) const
    {
        return d - d1.d;
    }

    double date() const
    {
        return d;
    }

    void add(int32_t num, int32_t part = _SECOND);
    void fix(int32_t part = _SECOND);

    void toLocal();
    void toUTC();

    void parse(const char* str, int32_t len = -1);
    void parse(exlib::string str)
    {
        parse(str.c_str(), (int32_t)str.length());
    }

    void toGMTString(exlib::string& retVal);
    void toX509String(exlib::string& retVal);
    void sqlString(exlib::string& retVal);
    void stamp(exlib::string& retVal);

    static int32_t timezone();

private:
    double d;
};
}
