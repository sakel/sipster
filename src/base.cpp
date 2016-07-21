//
// Created by dejan on 28.3.2016.
//

#include <sipster/base.h>
#include <sipster/utils.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <iostream>
#include <sstream>
#include <algorithm>
#include <iterator>
#include <sipster/log.h>
#include <netdb.h>
#include <vector>

using namespace std;

SipsterInetAddress *sipster_base_inet_address_create(SipsterInetAddressType addrType, char *addr, unsigned short port) {
    SipsterInetAddress * sipsterAddr = (SipsterInetAddress *) sipster_allocator(sizeof(SipsterInetAddress));

    sipsterAddr->addressType = addrType;
    sipsterAddr->port = port;

    inet_pton(addrType == SIPSTER_IPV4 ? AF_INET : AF_INET6, addr, sipsterAddr->address);

    return sipsterAddr;
}

SipsterInetAddress *sipster_base_inet_address_parse(const char * address) {
    SipsterInetAddress *inetAddress;
    string item;
    vector<string> elems;

    stringstream ss(address);

    while (getline(ss, item, ':')) {
        elems.push_back(item);
    }

    if(elems.size() < 1) {
        SIPSTER_ERROR("Could not parse address: %s", address);
        return NULL;
    }

    // FIXME / TODO IPv6 address support

    if(elems.size() == 2) {
        struct addrinfo *result;
        int res;
        res = getaddrinfo(elems.at(0).c_str(), elems.at(1).c_str(), NULL, &result);
        if(res != 0) {
            SIPSTER_ERROR("Error getting address: %d", res);
            return NULL;
        }
        //TODO

        inetAddress = sipster_base_sockaddr_to_sipster(result->ai_addr);
        strncpy(inetAddress->fqdn, elems.at(0).c_str(), sizeof(inetAddress->fqdn));
        freeaddrinfo(result);
        return inetAddress;
    } else if(elems.size() == 1) {
        struct addrinfo *result;
        int res;
        res = getaddrinfo(elems.at(0).c_str(), NULL, NULL, &result);
        if(res != 0) {
            SIPSTER_ERROR("Error getting address: %d", res);
            return NULL;
        }

        inetAddress = sipster_base_sockaddr_to_sipster(result->ai_addr);
        strncpy(inetAddress->fqdn, elems.at(0).c_str(), sizeof(inetAddress->fqdn));
        freeaddrinfo(result);
        return inetAddress;
    }

    return NULL;
}

SipsterInetAddress *sipster_base_sockaddr_to_sipster(struct sockaddr *addr) {
    if(!addr) {
        return NULL;
    }

    SipsterInetAddress *nAddr = (SipsterInetAddress *) sipster_allocator(sizeof(SipsterInetAddress));

    if(addr->sa_family == AF_INET) {
        struct sockaddr_in *ipv4addr = (struct sockaddr_in *) addr;
        nAddr->addressType = SIPSTER_IPV4;
        memcpy(nAddr->address, &ipv4addr->sin_addr, sizeof(ipv4addr->sin_addr));
        nAddr->port = ntohs(ipv4addr->sin_port);
    } else {
        struct sockaddr_in6 *ipv6addr = (struct sockaddr_in6 *) addr;
        nAddr->addressType = SIPSTER_IPV6;
        memcpy(nAddr->address, &ipv6addr->sin6_addr, sizeof(ipv6addr->sin6_addr));
        nAddr->port = ntohs(ipv6addr->sin6_port);
    }

    return nAddr;
}

struct sockaddr *sipster_base_sipster_to_sockaddr(SipsterInetAddress *addr) {
    struct sockaddr *sock;

    if(addr->addressType == SIPSTER_IPV4) {
        struct sockaddr_in *ipv4sock = (sockaddr_in *) sipster_allocator(sizeof(struct sockaddr_in));
        ipv4sock->sin_port = htons(addr->port);
        memcpy(&ipv4sock->sin_addr, addr->address, sizeof(ipv4sock->sin_addr));
        ipv4sock->sin_family = AF_INET;
        sock = (sockaddr *) ipv4sock;
    } else {
        struct sockaddr_in6 *ipv6sock = (sockaddr_in6 *) sipster_allocator(sizeof(struct sockaddr_in6));
        ipv6sock->sin6_port = htons(addr->port);
        memcpy(&ipv6sock->sin6_addr, addr->address, sizeof(ipv6sock->sin6_addr));
        ipv6sock->sin6_family = AF_INET6;
        sock = (sockaddr *) ipv6sock;
    }

    return sock;
}

void sipster_base_inet_address_destroy(SipsterInetAddress *addr) {
    if(addr) {
        sipster_free(addr);
    }
}

const char *sipster_base_inet_address_get_fqdn(SipsterInetAddress *addr) {
    if(addr->fqdn[0] != '\0') {
        return addr->fqdn;
    } else {
        inet_ntop(addr->addressType == SIPSTER_IPV4 ? AF_INET : AF_INET6, addr->address, addr->fqdn, 255);
    }
    return addr->fqdn;
}

SipsterInetAddress *sipster_base_inet_addres_clone(SipsterInetAddress *addr) {
    SipsterInetAddress *address = (SipsterInetAddress *) sipster_allocator(sizeof(SipsterInetAddress));
    memcpy(address, addr, sizeof(SipsterInetAddress));
    return address;
}
