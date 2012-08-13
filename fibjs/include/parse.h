/*
 * parse.h
 *
 *  Created on: Aug 10, 2012
 *      Author: lion
 */

#include "utils.h"

#ifndef PARSE_H_
#define PARSE_H_

namespace fibjs
{

class _parser
{
public:
	_parser(const char* s, int n) :
			string(s), sz(n), pos(0)
	{
	}

	_parser(std::string& s) :
			string(s.c_str()), sz((int) s.length()), pos(0)
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
		while (pos < sz)
		{
			char ch = get();
			if (qisspace(ch))
				skip();
			else
				break;
		}
	}

	void skipWord()
	{
		while (pos < sz)
		{
			char ch = get();
			if (ch == 0 || qisspace(ch))
				break;
			else
				skip();
		}
	}

	void skipWord(char ch1)
	{
		while (pos < sz)
		{
			char ch = get();
			if (ch == 0 || qisspace(ch) || ch == ch1)
				break;
			else
				skip();
		}
	}

	void skipWord(char ch1, char ch2)
	{
		while (pos < sz)
		{
			char ch = get();
			if (ch == 0 || qisspace(ch) || ch == ch1 || ch == ch2)
				break;
			else
				skip();
		}
	}

	void skipWord(char ch1, char ch2, char ch3)
	{
		while (pos < sz)
		{
			char ch = get();
			if (ch == 0 || qisspace(ch) || ch == ch1 || ch == ch2 || ch == ch3)
				break;
			else
				skip();
		}
	}

	bool want(char ch)
	{
		skipSpace();

		if (get() != ch)
			return false;

		skip();
		skipSpace();

		return true;
	}

	bool end()
	{
		return pos >= sz;
	}

public:
	const char* string;
	int sz;
	int pos;
};

}

#endif /* PARSE_H_ */
