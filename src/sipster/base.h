//
// Created by dejan on 28.3.2016.
//

#ifndef SIPSTER_BASE_H
#define SIPSTER_BASE_H

#include <sys/socket.h>

typedef struct _Sipster Sipster;

typedef struct _SipsterInetAddress SipsterInetAddress;

typedef enum {
    SIPSTER_IPV4,
    SIPSTER_IPV6
} SipsterInetAddressType;

struct _SipsterInetAddress {
    SipsterInetAddressType addressType;
    unsigned short port;
    unsigned char address[16];
    char fqdn[256];
};

SipsterInetAddress *sipster_base_inet_address_create(SipsterInetAddressType addrType, char *addr, unsigned short port);
SipsterInetAddress *sipster_base_sockaddr_to_sipster(struct sockaddr *addr);
struct sockaddr *sipster_base_sipster_to_sockaddr(SipsterInetAddress *addr);
const char * sipster_base_inet_address_get_fqdn(SipsterInetAddress *addr);
SipsterInetAddress *sipster_base_inet_addres_clone(SipsterInetAddress *addr);
SipsterInetAddress * sipster_base_inet_address_parse(const char * address);


#endif //SIPSTER_BASE_H
