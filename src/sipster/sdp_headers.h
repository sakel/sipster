//
// Created by dejan on 14.3.2016.
//

#ifndef SIPSTER_SDP_HEADERS_H_H
#define SIPSTER_SDP_HEADERS_H_H

#include <stddef.h>
#include <sipster/sdp_enums.h>

typedef struct _SipsterSdpAttribute SipsterSdpAttribute;
typedef struct _SipsterSdpAttributeRtpmap SipsterSdpAttributeRtpmap;
typedef struct _SipsterSdpAttributeFmtp SipsterSdpAttributeFmtp;

typedef struct _SipsterSdpHeader SipsterSdpHeader;
typedef struct _SipsterSdpHeaderVersion SipsterSdpHeaderVersion;
typedef struct _SipsterSdpHeaderOrigin SipsterSdpHeaderOrigin;
typedef struct _SipsterSdpHeaderString SipsterSdpHeaderString;
typedef struct _SipsterSdpHeaderString SipsterSdpHeaderSessionName;
typedef struct _SipsterSdpHeaderString SipsterSdpHeaderSessionInfo;
typedef struct _SipsterSdpHeaderString SipsterSdpHeaderUri;
typedef struct _SipsterSdpHeaderString SipsterSdpHeaderEmail;
typedef struct _SipsterSdpHeaderString SipsterSdpHeaderPhone;
typedef struct _SipsterSdpHeaderConnection SipsterSdpHeaderConnection;
typedef struct _SipsterSdpHeaderTiming SipsterSdpHeaderTiming;
typedef struct _SipsterSdpHeaderMedia SipsterSdpHeaderMedia;
typedef struct _SipsterSdpHeaderAttribute SipsterSdpHeaderAttribute;

struct _SipsterSdpHeader {
    char header;
    SipsterSdpHeaderEnum headerType;
};

struct _SipsterSdpHeaderVersion {
    SipsterSdpHeader header;
    unsigned int version;
};

struct _SipsterSdpHeaderOrigin {
    SipsterSdpHeader header;
    char username[150];
    unsigned long sessionId;
    unsigned long sessionVersion;
    char nettype[3];
    char addrtype[4]; // IP4, IP6
    char address[46];
};

struct _SipsterSdpHeaderString {
    SipsterSdpHeader header;
    char data[400];
};

struct _SipsterSdpHeaderConnection {
    SipsterSdpHeader header;
    char nettype[3];
    char addrtype[4];
    char address[46];
    unsigned char ttl;
    unsigned char addressCount;
};

struct _SipsterSdpHeaderTiming {
    SipsterSdpHeader header;
    unsigned long start;
    unsigned long stop;
};

struct _SipsterSdpHeaderMedia {
    SipsterSdpHeader header;
    SipsterSdpMediaTypeEnum mediaType;
    unsigned short port;
    unsigned short portCount;
    char protocol[10];
    unsigned char ptsCount;
    unsigned char pts[20];
};

struct _SipsterSdpHeaderAttribute {
    SipsterSdpHeader header;
    SipsterSdpAttribute *attribute;
};

struct _SipsterSdpAttribute {
    SipsterSdpAttributeEnum attributeType;
};

struct _SipsterSdpAttributeRtpmap {
    SipsterSdpAttribute attribute;
    unsigned short pt;
    char ecoding[20];
    unsigned int clock;
    unsigned short channels;
    char params[40];
};

struct _SipsterSdpAttributeFmtp {
    SipsterSdpAttribute attribute;
    unsigned short pt;
    char params[80];
};

SipsterSdpHeader *sipster_sdp_header_create(SipsterSdpHeaderEnum headerType, size_t size);
void sipster_sdp_header_destroy(SipsterSdpHeader * header);
SipsterSdpHeader * sipster_sdp_header_clone(SipsterSdpHeader *header);

#endif //SIPSTER_SDP_HEADERS_H_H
