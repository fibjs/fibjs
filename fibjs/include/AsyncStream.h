/*
 * AsyncStream
.h
 *
 *  Created on: Jun 15, 2025
 *      Author: lion
 */

#pragma once

namespace fibjs {

result_t startRecvStream(Stream_base* stream, exlib::atomic& readState);
enum ReadState {
    C_OPEN = 0,
    C_READING = 1,
    C_CLOSED = 2
};

template <typename T>
class AsyncStream : public T {
public:
    // object_base
    virtual result_t onEventChange(exlib::string type, exlib::string ev, v8::Local<v8::Function> func)
    {
        startRecvStream(this, m_readState);
        return 0;
    }

private:
    exlib::atomic m_readState;
};

}
