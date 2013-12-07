/*
 * LruCache.h
 *
 *  Created on: Dec 7, 2013
 *      Author: lion
 */

#include "ifs/LruCache.h"
#include "map"

#ifndef LRUCACHE_H_
#define LRUCACHE_H_

namespace fibjs
{

class LruCache: public LruCache_base
{
public:
	LruCache(int32_t size, int32_t timeout) :
			m_size(size), m_timeout(timeout)
	{
		m_begin = m_datas.end();
		m_end = m_datas.end();
	}

public:
	// object_base
	virtual result_t toJSON(const char* key, v8::Handle<v8::Value>& retVal);

public:
	// LruCache_base
	virtual result_t get_size(int32_t& retVal);
	virtual result_t clear();
	virtual result_t has(const char* name, bool& retVal);
	virtual result_t get(const char* name, v8::Handle<v8::Value>& retVal);
	virtual result_t set(const char* name, v8::Handle<v8::Value> value);
	virtual result_t put(const char* name, v8::Handle<v8::Value> value);
	virtual result_t put(v8::Handle<v8::Object> map);
	virtual result_t remove(const char* name);
	virtual result_t isEmpty(bool& retVal);

private:
	class _linkedNode
	{
	public:
		std::map<std::string, _linkedNode>::iterator& last()
		{
			return *(std::map<std::string, _linkedNode>::iterator*) &m_last;
		}

		std::map<std::string, _linkedNode>::iterator& next()
		{
			return *(std::map<std::string, _linkedNode>::iterator*) &m_next;
		}

	public:
		VariantEx value;
		date_t update;
		std::map<std::string, int>::iterator m_last, m_next;
	};

	void remove(std::map<std::string, _linkedNode>::iterator it)
	{
		std::map<std::string, _linkedNode>::iterator& last = it->second.last();
		std::map<std::string, _linkedNode>::iterator& next = it->second.next();

		if (last != m_datas.end())
			last->second.next() = next;
		else
			m_begin = next;

		if (next != m_datas.end())
			next->second.last() = last;
		else
			m_end = last;
	}

	void insert(std::map<std::string, _linkedNode>::iterator it)
	{
		it->second.next() = m_begin;
		it->second.last() = m_datas.end();

		if (m_begin != m_datas.end())
			m_begin->second.last() = it;
		else
			m_end = it;

		m_begin = it;
	}

	void cleanup();

	std::map<std::string, _linkedNode> m_datas;
	std::map<std::string, _linkedNode>::iterator m_begin;
	std::map<std::string, _linkedNode>::iterator m_end;
	int32_t m_size;
	int32_t m_timeout;
};

} /* namespace fibjs */

#endif /* LRUCACHE_H_ */
