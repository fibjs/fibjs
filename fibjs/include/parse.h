/*
 * parse.h
 *
 *  Created on: Aug 10, 2012
 *      Author: lion
 */

#include "utils.h"

#ifndef PARSE_H_
#define PARSE_H_

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

    _parser(exlib::string& s)
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

#endif /* PARSE_H_ */
