/*
 * Socket_api.h
 *
 *  Created on: Oct 24, 2012
 *      Author: lion
 */

#ifndef SOCKET_API_H_
#define SOCKET_API_H_

namespace fibjs
{

namespace socket
{

void *create();
void destroy(void *sock);
void close(void *sock);
int connect(void *sock, const char *host, int port);
int recv(void *sock, char *buffer, int cbBuffer);
int send(void *sock, const char *buffer, int cbBuffer);

}

}

#endif /* SOCKET_API_H_ */
