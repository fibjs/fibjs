/*
 * MessageChannel.h
 *
 *  Created on: Mar 16, 2026
 *      Author: lion
 */

#pragma once

#include "ifs/MessageChannel.h"
#include "MessagePort.h"

namespace fibjs {

class MessageChannel : public MessageChannel_base {
public:
    MessageChannel(obj_ptr<MessagePort> port1, obj_ptr<MessagePort> port2)
        : m_port1(port1)
        , m_port2(port2)
    {
    }

public:
    // MessageChannel_base
    virtual result_t get_port1(obj_ptr<MessagePort_base>& retVal)
    {
        retVal = m_port1;
        return 0;
    }

    virtual result_t get_port2(obj_ptr<MessagePort_base>& retVal)
    {
        retVal = m_port2;
        return 0;
    }

private:
    obj_ptr<MessagePort> m_port1;
    obj_ptr<MessagePort> m_port2;
};

} /* namespace fibjs */
