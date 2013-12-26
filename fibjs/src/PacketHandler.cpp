/*
 * PacketHandler.cpp
 *
 *  Created on: Aug 26, 2012
 *      Author: lion
 */

#include "ifs/mq.h"
#include "PacketHandler.h"
#include "BufferedStream.h"
#include "JSHandler.h"

namespace fibjs
{

static const char *s_staticCounter[] =
{ "total", "pendding" };
static const char *s_Counter[] =
{ "request", "response", "error" };

enum
{
    HTTP_TOTAL = 0,
    HTTP_PENDDING,
    HTTP_REQUEST,
    HTTP_RESPONSE,
    HTTP_ERROR
};

PacketHandler::PacketHandler(Handler_base *hdlr) :
    m_hdlr(hdlr)
{
    m_stats = new Stats();
    m_stats->init(s_staticCounter, 2, s_Counter, 6);
}

result_t PacketHandler::invoke(object_base *v, obj_ptr<Handler_base> &retVal,
                               exlib::AsyncEvent *ac)
{
    if (!ac)
        return CALL_E_NOSYNC;

    return CALL_E_BADVARTYPE;
}

result_t PacketHandler::get_maxSize(int32_t &retVal)
{
    retVal = m_maxSize;
    return 0;
}

result_t PacketHandler::set_maxSize(int32_t newVal)
{
    if (newVal < 0)
        return CALL_E_OUTRANGE;

    m_maxSize = newVal;
    return 0;
}

result_t PacketHandler::get_stats(obj_ptr<Stats_base> &retVal)
{
    retVal = m_stats;
    return 0;
}

}
