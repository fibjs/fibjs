/*
 * parse.h
 *
 *  Created on: Aug 10, 2012
 *      Author: lion
 */

#pragma once

#include "utils.h"

namespace fibjs {

class _parser {
public:
    _parser(const char* s, int32_t n = -1)
        : string(s)
        , pos(0)
    {
        if (n < 0)
            n = (int32_t)qstrlen(s);
        sz = n;
    }

    _parser(const exlib::string& s)
        : string(s.c_str())
        , sz((int32_t)s.length())
        , pos(0)
    {
    }

    void skip()
    {
        if (pos < sz)
            pos++;
    }

    char get()
    {
        if (pos < sz)
            return string[pos];
        else
            return 0;
    }

    char peek()
    {
        if (pos + 1 < sz)
            return string[pos + 1];
        else
            return 0;
    }

    void skipSpace()
    {
        char ch;

        while (0 != (ch = get())) {
            if (qisspace(ch))
                skip();
            else
                break;
        }
    }

    void skipWord()
    {
        char ch;

        while (0 != (ch = get())) {
            if (qisspace(ch))
                break;
            else
                skip();
        }
    }

    void skipWord(char ch1)
    {
        char ch;

        while (0 != (ch = get())) {
            if (qisspace(ch) || ch == ch1)
                break;
            else
                skip();
        }
    }

    void skipWord(char ch1, char ch2)
    {
        char ch;

        while (0 != (ch = get())) {
            if (qisspace(ch) || ch == ch1 || ch == ch2)
                break;
            else
                skip();
        }
    }

    void skipWord(char ch1, char ch2, char ch3)
    {
        char ch;

        while (0 != (ch = get())) {
            if (qisspace(ch) || ch == ch1 || ch == ch2 || ch == ch3)
                break;
            else
                skip();
        }
    }

    void skipWord(char ch1, char ch2, char ch3, char ch4)
    {
        char ch;

        while (0 != (ch = get())) {
            if (qisspace(ch) || ch == ch1 || ch == ch2 || ch == ch3 || ch == ch4)
                break;
            else
                skip();
        }
    }

    void skipWord(char ch1, char ch2, char ch3, char ch4, char ch5)
    {
        char ch;

        while (0 != (ch = get())) {
            if (qisspace(ch) || ch == ch1 || ch == ch2 || ch == ch3 || ch == ch4 || ch == ch5)
                break;
            else
                skip();
        }
    }

    void skipWord(char ch1, char ch2, char ch3, char ch4, char ch5, char ch6)
    {
        char ch;

        while (0 != (ch = get())) {
            if (qisspace(ch) || ch == ch1 || ch == ch2 || ch == ch3 || ch == ch4 || ch == ch5 || ch == ch6)
                break;
            else
                skip();
        }
    }

    void skipWord(char ch1, char ch2, char ch3, char ch4, char ch5, char ch6, char ch7)
    {
        char ch;

        while (0 != (ch = get())) {
            if (qisspace(ch) || ch == ch1 || ch == ch2 || ch == ch3 || ch == ch4 || ch == ch5 || ch == ch6 || ch == ch7)
                break;
            else
                skip();
        }
    }

    void skipWord(char ch1, char ch2, char ch3, char ch4, char ch5, char ch6, char ch7, char ch8)
    {
        char ch;

        while (0 != (ch = get())) {
            if (qisspace(ch) || ch == ch1 || ch == ch2 || ch == ch3 || ch == ch4 || ch == ch5 || ch == ch6 || ch == ch7 || ch == ch8)
                break;
            else
                skip();
        }
    }

    void skipWord(char ch1, char ch2, char ch3, char ch4, char ch5, char ch6, char ch7, char ch8, char ch9)
    {
        char ch;

        while (0 != (ch = get())) {
            if (qisspace(ch) || ch == ch1 || ch == ch2 || ch == ch3 || ch == ch4 || ch == ch5 || ch == ch6 || ch == ch7 || ch == ch8 || ch == ch9)
                break;
            else
                skip();
        }
    }

    void skipWord(char ch1, char ch2, char ch3, char ch4, char ch5, char ch6, char ch7, char ch8, char ch9, char ch10)
    {
        char ch;

        while (0 != (ch = get())) {
            if (qisspace(ch) || ch == ch1 || ch == ch2 || ch == ch3 || ch == ch4 || ch == ch5 || ch == ch6 || ch == ch7 || ch == ch8 || ch == ch9 || ch == ch10)
                break;
            else
                skip();
        }
    }

    void skipWord(char ch1, char ch2, char ch3, char ch4, char ch5, char ch6, char ch7, char ch8, char ch9, char ch10, char ch11)
    {
        char ch;

        while (0 != (ch = get())) {
            if (qisspace(ch) || ch == ch1 || ch == ch2 || ch == ch3 || ch == ch4 || ch == ch5 || ch == ch6 || ch == ch7 || ch == ch8 || ch == ch9 || ch == ch10 || ch == ch11)
                break;
            else
                skip();
        }
    }

    void skipUntil(char ch1)
    {
        char ch;

        while (0 != (ch = get())) {
            if (ch == ch1)
                break;
            else
                skip();
        }
    }

    void skipUntil(char ch1, char ch2)
    {
        char ch;

        while (0 != (ch = get())) {
            if (ch == ch1 || ch == ch2)
                break;
            else
                skip();
        }
    }

    void skipUntil(char ch1, char ch2, char ch3)
    {
        char ch;

        while (0 != (ch = get())) {
            if (ch == ch1 || ch == ch2 || ch == ch3)
                break;
            else
                skip();
        }
    }

    void skipUntil(char ch1, char ch2, char ch3, char ch4)
    {
        char ch;

        while (0 != (ch = get())) {
            if (ch == ch1 || ch == ch2 || ch == ch3 || ch == ch4)
                break;
            else
                skip();
        }
    }

    void skipUntil(char ch1, char ch2, char ch3, char ch4, char ch5)
    {
        char ch;

        while (0 != (ch = get())) {
            if (ch == ch1 || ch == ch2 || ch == ch3 || ch == ch4 || ch == ch5)
                break;
            else
                skip();
        }
    }

    void skipUntil(char ch1, char ch2, char ch3, char ch4, char ch5, char ch6)
    {
        char ch;

        while (0 != (ch = get())) {
            if (ch == ch1 || ch == ch2 || ch == ch3 || ch == ch4 || ch == ch5 || ch == ch6)
                break;
            else
                skip();
        }
    }

    void skipUntil(const char* chs)
    {
        char ch, ch1;
        const char* p;

        while (0 != (ch = get())) {
            p = chs;
            while (0 != (ch1 = *p++))
                if (ch == ch1)
                    return;

            skip();
        }
    }

    void skipKeyWord()
    {
        char ch;

        while (0 != (ch = get())) {
            if (!qisascii(ch) && !qisdigit(ch) && (ch != '_'))
                break;
            else
                skip();
        }
    }

    char getChar()
    {
        char ch = get();
        if (ch)
            skip();
        return ch;
    }

    int32_t getWord(exlib::string& retVal)
    {
        int32_t p1, p2;

        p1 = pos;
        skipWord();
        p2 = pos - p1;

        retVal.assign(string + p1, p2);

        return p2;
    }

    int32_t getWord(exlib::string& retVal, char ch1)
    {
        int32_t p1, p2;

        p1 = pos;
        skipWord(ch1);
        p2 = pos - p1;

        retVal.assign(string + p1, p2);

        return p2;
    }

    int32_t getWord(exlib::string& retVal, char ch1, char ch2)
    {
        int32_t p1, p2;

        p1 = pos;
        skipWord(ch1, ch2);
        p2 = pos - p1;

        retVal.assign(string + p1, p2);

        return p2;
    }

    int32_t getWord(exlib::string& retVal, char ch1, char ch2, char ch3)
    {
        int32_t p1, p2;

        p1 = pos;
        skipWord(ch1, ch2, ch3);
        p2 = pos - p1;

        retVal.assign(string + p1, p2);

        return p2;
    }

    int32_t getWord(exlib::string& retVal, char ch1, char ch2, char ch3, char ch4)
    {
        int32_t p1, p2;

        p1 = pos;
        skipWord(ch1, ch2, ch3, ch4);
        p2 = pos - p1;

        retVal.assign(string + p1, p2);

        return p2;
    }

    int32_t getWord(exlib::string& retVal, char ch1, char ch2, char ch3, char ch4, char ch5)
    {
        int32_t p1, p2;

        p1 = pos;
        skipWord(ch1, ch2, ch3, ch4, ch5);
        p2 = pos - p1;

        retVal.assign(string + p1, p2);

        return p2;
    }

    int32_t getWord(exlib::string& retVal, char ch1, char ch2, char ch3, char ch4, char ch5, char ch6)
    {
        int32_t p1, p2;

        p1 = pos;
        skipWord(ch1, ch2, ch3, ch4, ch5, ch6);
        p2 = pos - p1;

        retVal.assign(string + p1, p2);

        return p2;
    }

    int32_t getWord(exlib::string& retVal, char ch1, char ch2, char ch3, char ch4, char ch5, char ch6, char ch7)
    {
        int32_t p1, p2;

        p1 = pos;
        skipWord(ch1, ch2, ch3, ch4, ch5, ch6, ch7);
        p2 = pos - p1;

        retVal.assign(string + p1, p2);

        return p2;
    }

    int32_t getWord(exlib::string& retVal, char ch1, char ch2, char ch3, char ch4, char ch5, char ch6, char ch7, char ch8)
    {
        int32_t p1, p2;

        p1 = pos;
        skipWord(ch1, ch2, ch3, ch4, ch5, ch6, ch7, ch8);
        p2 = pos - p1;

        retVal.assign(string + p1, p2);

        return p2;
    }

    int32_t getWord(exlib::string& retVal, char ch1, char ch2, char ch3, char ch4, char ch5, char ch6, char ch7, char ch8, char ch9)
    {
        int32_t p1, p2;

        p1 = pos;
        skipWord(ch1, ch2, ch3, ch4, ch5, ch6, ch7, ch8, ch9);
        p2 = pos - p1;

        retVal.assign(string + p1, p2);

        return p2;
    }

    int32_t getWord(exlib::string& retVal, char ch1, char ch2, char ch3, char ch4, char ch5, char ch6, char ch7, char ch8, char ch9, char ch10)
    {
        int32_t p1, p2;

        p1 = pos;
        skipWord(ch1, ch2, ch3, ch4, ch5, ch6, ch7, ch8, ch9, ch10);
        p2 = pos - p1;

        retVal.assign(string + p1, p2);

        return p2;
    }

    int32_t getWord(exlib::string& retVal, char ch1, char ch2, char ch3, char ch4, char ch5, char ch6, char ch7, char ch8, char ch9, char ch10, char ch11)
    {
        int32_t p1, p2;

        p1 = pos;
        skipWord(ch1, ch2, ch3, ch4, ch5, ch6, ch7, ch8, ch9, ch10, ch11);
        p2 = pos - p1;

        retVal.assign(string + p1, p2);

        return p2;
    }

    int32_t getString(exlib::string& retVal, char ch1)
    {
        int32_t p1, p2;

        p1 = pos;
        skipUntil(ch1);
        p2 = pos - p1;

        retVal.assign(string + p1, p2);

        return p2;
    }

    int32_t getString(exlib::string& retVal, char ch1, char ch2)
    {
        int32_t p1, p2;

        p1 = pos;
        skipUntil(ch1, ch2);
        p2 = pos - p1;

        retVal.assign(string + p1, p2);

        return p2;
    }

    int32_t getString(exlib::string& retVal, char ch1, char ch2, char ch3)
    {
        int32_t p1, p2;

        p1 = pos;
        skipUntil(ch1, ch2, ch3);
        p2 = pos - p1;

        retVal.assign(string + p1, p2);

        return p2;
    }

    int32_t getString(exlib::string& retVal, char ch1, char ch2, char ch3, char ch4)
    {
        int32_t p1, p2;

        p1 = pos;
        skipUntil(ch1, ch2, ch3, ch4);
        p2 = pos - p1;

        retVal.assign(string + p1, p2);

        return p2;
    }

    int32_t getString(exlib::string& retVal, char ch1, char ch2, char ch3, char ch4, char ch5)
    {
        int32_t p1, p2;

        p1 = pos;
        skipUntil(ch1, ch2, ch3, ch4, ch5);
        p2 = pos - p1;

        retVal.assign(string + p1, p2);

        return p2;
    }

    int32_t getString(exlib::string& retVal, char ch1, char ch2, char ch3, char ch4, char ch5, char ch6)
    {
        int32_t p1, p2;

        p1 = pos;
        skipUntil(ch1, ch2, ch3, ch4, ch5, ch6);
        p2 = pos - p1;

        retVal.assign(string + p1, p2);

        return p2;
    }

    int32_t getString(exlib::string& retVal, const char* chs)
    {
        int32_t p1, p2;

        p1 = pos;
        skipUntil(chs);
        p2 = pos - p1;

        retVal.assign(string + p1, p2);

        return p2;
    }

    int32_t getKeyWord(exlib::string& retVal)
    {
        int32_t p1, p2;

        p1 = pos;
        skipKeyWord();
        p2 = pos - p1;

        retVal.assign(string + p1, p2);

        return p2;
    }

    int32_t getLeft(exlib::string& retVal)
    {
        int32_t p2 = sz - pos;
        retVal.assign(string + pos, p2);
        pos = sz;

        return p2;
    }

    int32_t getLine(exlib::string& retVal)
    {
        int32_t p1, p2;
        char ch;

        p1 = pos;

        while (0 != (ch = get())) {
            if (ch == '\r' || ch == '\n')
                break;
            else
                skip();
        }
        p2 = pos - p1;

        if (ch) {
            skip();
            if (get() + ch == '\r' + '\n')
                skip();
        }

        retVal.assign(string + p1, p2);

        return p2;
    }

    bool want(char ch)
    {
        skipSpace();

        if (get() != ch)
            return false;

        skip();

        return true;
    }

    bool end()
    {
        return pos >= sz;
    }

    const char* now()
    {
        return string + pos;
    }

    int32_t left()
    {
        return sz - pos;
    }

public:
    const char* string;
    int32_t sz;
    int32_t pos;
};
}
