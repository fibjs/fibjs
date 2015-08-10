/*
 * HeapGraphEdge.h
 *
 *  Created on: Aug 9, 2015
 *      Author: lion
 */

#include "ifs/HeapGraphEdge.h"
#include <v8/include/v8-profiler.h>

#ifndef _fj_HEAPGRAPHEDGE_H
#define _fj_HEAPGRAPHEDGE_H

namespace fibjs
{

class HeapSnapshot;
class HeapGraphEdge : public HeapGraphEdge_base
{
public:
	HeapGraphEdge(const v8::HeapGraphEdge* graphedge)
		: m_graphedge(graphedge)
	{}

public:
	// HeapGraphEdge_base
	virtual result_t get_type(std::string& retVal);
	virtual result_t get_name(std::string& retVal);
	virtual result_t getFromNode(obj_ptr<HeapGraphNode_base>& retVal);
	virtual result_t getToNode(obj_ptr<HeapGraphNode_base>& retVal);

private:
	const v8::HeapGraphEdge* m_graphedge;
};

}

#endif // _fj_HEAPGRAPHEDGE_H
