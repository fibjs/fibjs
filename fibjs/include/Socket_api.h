/*
 * Socket_api.h
 *
 *  Created on: Oct 24, 2012
 *      Author: lion
 */

#include <stdint.h>

#ifndef SOCKET_API_H_
#define SOCKET_API_H_

namespace fibjs {

namespace socket {

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

#endif /* SOCKET_API_H_ */
