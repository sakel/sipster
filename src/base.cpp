//
// Created by dejan on 28.3.2016.
//

#include <sipster/base.h>
#include <utils.h>
#include <netinet/in.h>
#include <string.h>

SipsterInetAddress *sipster_base_sockaddr_to_sipster(struct sockaddr *addr) {
    if(!addr) {
        return NULL;
    }

    SipsterInetAddress *nAddr = (SipsterInetAddress *) sipster_allocator(sizeof(SipsterInetAddress));

    if(addr->sa_family == AF_INET) {
        struct sockaddr_in *ipv4addr = (struct sockaddr_in *) addr;
        nAddr->addressType = SIPSTER_IPV4;
        memcpy(nAddr->address, &ipv4addr->sin_addr, sizeof(ipv4addr->sin_addr));
        nAddr->port = ipv4addr->sin_port;
    } else {
        struct sockaddr_in6 *ipv6addr = (struct sockaddr_in6 *) addr;
        nAddr->addressType = SIPSTER_IPV6;
        memcpy(nAddr->address, &ipv6addr->sin6_addr, sizeof(ipv6addr->sin6_addr));
        nAddr->port = ipv6addr->sin6_port;
    }

    return nAddr;
}

struct sockaddr *sipster_base_sipster_to_sockaddr(SipsterInetAddress *addr) {
    struct sockaddr *sock;

    if(addr->addressType == SIPSTER_IPV4) {
        struct sockaddr_in *ipv4sock = (sockaddr_in *) sipster_allocator(sizeof(struct sockaddr_in));
        ipv4sock->sin_port = addr->port;
        memcpy(&ipv4sock->sin_addr, addr->address, sizeof(ipv4sock->sin_addr));
        sock = (sockaddr *) ipv4sock;
    } else {
        struct sockaddr_in6 *ipv6sock = (sockaddr_in6 *) sipster_allocator(sizeof(struct sockaddr_in6));
        ipv6sock->sin6_port = addr->port;
        memcpy(&ipv6sock->sin6_addr, addr->address, sizeof(ipv6sock->sin6_addr));
        sock = (sockaddr *) ipv6sock;
    }

    return sock;
}

void sipster_base_inet_address_destroy(SipsterInetAddress *addr) {
    if(addr) {
        sipster_free(addr);
    }
}