/*
 * Routing.h
 *
 *  Created on: Aug 26, 2012
 *      Author: lion
 */

#include "ifs/Routing.h"
#include "QuickArray.h"
#include "pcre.h"

#ifndef ROUTING_H_
#define ROUTING_H_

namespace fibjs
{

class Routing: public Routing_base
{
public:
	class rule: public obj_base
	{
	public:
		rule(pcre *re, pcre_extra *extra, Handler_base* hdlr) :
				m_re(re), m_extra(extra), m_hdlr(hdlr)
		{
		}

		~rule()
		{
			pcre_free(m_re);
			pcre_free_study(m_extra);
		}

	public:
		pcre *m_re;
		pcre_extra *m_extra;
		obj_ptr<Handler_base> m_hdlr;
	};

public:
	// Handler_base
	virtual result_t invoke(obj_ptr<object_base>& v,
			obj_ptr<Handler_base>& retVal, exlib::AsyncEvent* ac);

public:
	// Routing_base
	virtual result_t append(const char* pattern, obj_ptr<Handler_base>& hdlr);
	virtual result_t append(const char* pattern, v8::Handle<v8::Function> hdlr);
	virtual result_t append(v8::Handle<v8::Object> map);

private:
	QuickArray<obj_ptr<rule> > m_array;
};

} /* namespace fibjs */
#endif /* ROUTING_H_ */
