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

	int getWord(std::string& retVal)
	{
		int p1, p2;

		p1 = pos;
		skipWord();
		p2 = pos - p1;

		retVal.assign(string + p1, p2);

		return p2;
	}

	int getWord(std::string& retVal, char ch1)
	{
		int p1, p2;

		p1 = pos;
		skipWord(ch1);
		p2 = pos - p1;

		retVal.assign(string + p1, p2);

		return p2;
	}

	int getWord(std::string& retVal, char ch1, char ch2)
	{
		int p1, p2;

		p1 = pos;
		skipWord(ch1, ch2);
		p2 = pos - p1;

		retVal.assign(string + p1, p2);

		return p2;
	}

	int getWord(std::string& retVal, char ch1, char ch2, char ch3)
	{
		int p1, p2;

		p1 = pos;
		skipWord(ch1, ch2, ch3);
		p2 = pos - p1;

		retVal.assign(string + p1, p2);

		return p2;
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

	const char* now()
	{
		return string + pos;
	}

	int left()
	{
		return sz - pos;
	}

public:
	const char* string;
	int sz;
	int pos;
};

}

#endif /* PARSE_H_ */
