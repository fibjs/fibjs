/*
 * HeapGraphEdge.h
 *
 *  Created on: Aug 9, 2015
 *      Author: lion
 */

#include "ifs/HeapGraphEdge.h"
#include "ifs/HeapSnapshot.h"
#include <v8/include/v8-profiler.h>

#ifndef _fj_HEAPGRAPHEDGE_H
#define _fj_HEAPGRAPHEDGE_H

namespace fibjs
{

class HeapGraphEdge : public HeapGraphEdge_base
{
public:
	HeapGraphEdge(HeapSnapshot_base* snapshot, int32_t _type,
	              std::string& _name, int32_t _fromid, int32_t _toid) :
		m_snapshot(snapshot), m_type(_type), m_name(_name),
		m_fromId(_fromid), m_toId(_toid)
	{
	}

public:
	// HeapGraphEdge_base
	virtual result_t get_type(int32_t& retVal);
	virtual result_t get_name(std::string& retVal);
	virtual result_t getFromNode(obj_ptr<HeapGraphNode_base>& retVal);
	virtual result_t getToNode(obj_ptr<HeapGraphNode_base>& retVal);

public:
	int32_t toid()
	{
		return m_toId;
	}

private:
	weak_ptr<HeapSnapshot_base> m_snapshot;
	int32_t m_type;
	std::string m_name;
	int32_t m_fromId;
	int32_t m_toId;
};

}

#endif // _fj_HEAPGRAPHEDGE_H
