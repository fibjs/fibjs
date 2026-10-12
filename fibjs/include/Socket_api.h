/*
 * Socket_api.h
 *
 *  Created on: Oct 24, 2012
 *      Author: lion
 */

#pragma once

#include <stdint.h>
#include "result.h"

namespace fibjs {

namespace socket {

    // The C ABI reports failures by code only (it cannot return a value):
    // the callback stores the failure code in a per-thread slot and the driver
    // reads it back on the same thread (mysql.h::error). The description cannot
    // cross the C boundary - the code is the whole message.
    void set_last_error(result_t hr);
    result_t last_error();

    void* create();
    void destroy(void* sock);

    int32_t connect(void* sock, const char* host, int32_t port);
    void* connect(const char* host, int32_t port);
    int32_t recv(void* sock, void* buffer, int32_t cbBuffer);
    int32_t read(void* sock, void* buffer, int32_t cbBuffer);
    int32_t send(void* sock, const void* buffer, int32_t cbBuffer);

    int32_t c_connect(void* sock, const char* host, int32_t port);
    void* c_connect(const char* host, int32_t port);
    int32_t c_recv(void* sock, void* buffer, int32_t cbBuffer);
    int32_t c_read(void* sock, void* buffer, int32_t cbBuffer);
    int32_t c_send(void* sock, const void* buffer, int32_t cbBuffer);
}
}
