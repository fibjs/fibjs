/*
 * PacketHandler.h
 *
 *  Created on: Dec 26, 2013
 *      Author: lion
 */

#include "ifs/PacketHandler.h"
#include "Stats.h"

#ifndef PACKETHANDLER_H_
#define PACKETHANDLER_H_

namespace fibjs
{

class PacketHandler: public PacketHandler_base
{
public:
    PacketHandler(Handler_base *hdlr);

public:
    // Handler_base
    virtual result_t invoke(object_base *v, obj_ptr<Handler_base> &retVal,
                            exlib::AsyncEvent *ac);

public:
    // PacketHandler_base
	virtual result_t get_maxSize(int32_t& retVal);
	virtual result_t set_maxSize(int32_t newVal);
    virtual result_t get_stats(obj_ptr<Stats_base> &retVal);

public:
    obj_ptr<Stream_base> m_stm;
    obj_ptr<Stats> m_stats;

private:
    obj_ptr<Handler_base> m_hdlr;
    int32_t m_maxSize;
};

} /* namespace fibjs */
#endif /* PACKETHANDLER_H_ */
