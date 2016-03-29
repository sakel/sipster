//
// Created by dejan on 28.3.2016.
//

#ifndef SIPSTER_BASE_H
#define SIPSTER_BASE_H

#include <sys/socket.h>

typedef struct _SipsterInetAddress SipsterInetAddress;

typedef enum {
    SIPSTER_IPV4,
    SIPSTER_IPV6
} SipsterInetAddressType;

struct _SipsterInetAddress {
    SipsterInetAddressType addressType;
    unsigned short port;
    unsigned char address[16];
};

SipsterInetAddress *sipster_base_sockaddr_to_sipster(struct sockaddr *addr);
struct sockaddr *sipster_base_sipster_to_sockaddr(SipsterInetAddress *addr);


#endif //SIPSTER_BASE_H
