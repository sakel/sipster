//
// Created by dejan on 14.3.2016.
//

#ifndef SIPSTER_SDP_PRIVATE_H
#define SIPSTER_SDP_PRIVATE_H

#include <sipster/sdp_enums.h>
#include <sipster/sdp_headers.h>

#define NULL_STRING "\0"

typedef SipsterSdpHeader* (*ParseSdpHeader)(SipsterSdpHeaderEnum, const char *);
typedef char* (*PrintSdpHeader)(SipsterSdpHeader *);
typedef void(*DestroySdpHeader)(SipsterSdpHeader *);
typedef SipsterSdpHeader* (*CloneSdpHeader)(SipsterSdpHeader *);

typedef SipsterSdpAttribute* (*ParseSdpAttribute)(SipsterSdpAttributeEnum, const char *);
typedef char* (*PrintSdpAttribute)(SipsterSdpAttribute *);
typedef SipsterSdpAttribute* (*CloneSdpAttribute)(SipsterSdpAttribute *);

typedef struct _SipsterSipHeaderMap {
    const char headerName[2];

    SipsterSdpHeaderEnum headerId;
    ParseSdpHeader parse;
    PrintSdpHeader print;
    DestroySdpHeader destroy;
    CloneSdpHeader clone;

} SipsterSdpHeaderMap;

typedef struct _SipsterSdpMediaTypeMap {
    const char media[20];
    SipsterSdpMediaTypeEnum type;
} SipsterSdpMediaTypeMap;

typedef struct _SipsterSdpAttributeMap {
    const char name[10];
    SipsterSdpAttributeEnum type;
    ParseSdpAttribute parse;
    PrintSdpAttribute print;
    CloneSdpAttribute clone;
} SipsterSdpAttributeMap;

/*
SipsterSdpHeader* parseSdpHeader(SipsterSdpHeaderEnum enum, const char * data);
char* printSdpHeader(SipsterSdpHeader * header);
void destroySdpHeader(SipsterSdpHeader * header);
SipsterSdpHeader* cloneSdpHeader(SipsterSdpHeader * header);
 */

void sdp_no_destroy(SipsterSdpHeader * header);
SipsterSdpHeader* sdp_no_parse(SipsterSdpHeaderEnum headerType, const char * input);
char* sdp_no_print(SipsterSdpHeader * header);
SipsterSdpHeader* sdp_no_clone(SipsterSdpHeader * header);

SipsterSdpHeader* sdp_version_parse(SipsterSdpHeaderEnum headerType, const char * data);
char* sdp_version_print(SipsterSdpHeader * header);
SipsterSdpHeader* sdp_version_clone(SipsterSdpHeader * header);

void sdp_origin_destroy(SipsterSdpHeader * header);
SipsterSdpHeader* sdp_origin_parse(SipsterSdpHeaderEnum headerType, const char * input);
char* sdp_origin_print(SipsterSdpHeader * header);
SipsterSdpHeader* sdp_origin_clone(SipsterSdpHeader * header);

void sdp_string_destroy(SipsterSdpHeader * header);
SipsterSdpHeader* sdp_string_parse(SipsterSdpHeaderEnum headerType, const char * input);
char* sdp_string_print(SipsterSdpHeader * header);
SipsterSdpHeader* sdp_string_clone(SipsterSdpHeader * header);

void sdp_connection_destroy(SipsterSdpHeader * header);
SipsterSdpHeader* sdp_connection_parse(SipsterSdpHeaderEnum headerType, const char * input);
char* sdp_connection_print(SipsterSdpHeader * header);
SipsterSdpHeader* sdp_connection_clone(SipsterSdpHeader * header);

void sdp_timing_destroy(SipsterSdpHeader * header);
SipsterSdpHeader* sdp_timing_parse(SipsterSdpHeaderEnum headerType, const char * input);
char* sdp_timing_print(SipsterSdpHeader * header);
SipsterSdpHeader* sdp_timing_clone(SipsterSdpHeader * header);

void sdp_media_destroy(SipsterSdpHeader * header);
SipsterSdpHeader* sdp_media_parse(SipsterSdpHeaderEnum headerType, const char * input);
char* sdp_media_print(SipsterSdpHeader * header);
SipsterSdpHeader* sdp_media_clone(SipsterSdpHeader * header);

void sdp_attr_destroy(SipsterSdpHeader * header);
SipsterSdpHeader* sdp_attr_parse(SipsterSdpHeaderEnum headerType, const char * input);
char* sdp_attr_print(SipsterSdpHeader * header);
SipsterSdpHeader* sdp_attr_clone(SipsterSdpHeader * header);

#define SDP_HMAP_NULL {NULL_STRING, SDP_HEADER_NONE /*, NULL, NULL, NULL*/}
const SipsterSdpHeaderMap sdp_header_prototypes[] = {
        {"v", SDP_HEADER_VERSION, sdp_version_parse, sdp_version_print, sdp_no_destroy, sdp_version_clone},
        {"o", SDP_HEADER_ORIGIN, sdp_origin_parse, sdp_origin_print, sdp_origin_destroy, sdp_origin_clone},
        {"s", SDP_HEADER_SESSION_NAME, sdp_string_parse, sdp_string_print, sdp_string_destroy, sdp_string_clone},
        {"i", SDP_HEADER_SESSION_INFO, sdp_string_parse, sdp_string_print, sdp_string_destroy, sdp_string_clone},
        {"u", SDP_HEADER_DESC_URI, sdp_string_parse, sdp_string_print, sdp_string_destroy, sdp_string_clone},
        {"e", SDP_HEADER_EMAIL, sdp_string_parse, sdp_string_print, sdp_string_destroy, sdp_string_clone},
        {"p", SDP_HEADER_PHONE, sdp_string_parse, sdp_string_print, sdp_string_destroy, sdp_string_clone},
        {"c", SDP_HEADER_CONNECTION_INFO, sdp_connection_parse, sdp_connection_print, sdp_connection_destroy, sdp_connection_clone},
        {"b", SDP_HEADER_BANDWIDTH_INFO, sdp_no_parse, sdp_no_print, sdp_no_destroy, sdp_no_clone},
        {"t", SDP_HEADER_TIME_ACTIVE, sdp_timing_parse, sdp_timing_print, sdp_timing_destroy, sdp_timing_clone},
        {"r", SDP_HEADER_TIME_REPEAT, sdp_no_parse, sdp_no_print, sdp_no_destroy, sdp_no_clone},
        {"z", SDP_HEADER_TIME_ZONE, sdp_no_parse, sdp_no_print, sdp_no_destroy, sdp_no_clone},
        {"k", SDP_HEADER_ENCRYPTION_KEY, sdp_no_parse, sdp_no_print, sdp_no_destroy, sdp_no_clone},
        {"a", SDP_HEADER_ATTRIBUTE, sdp_attr_parse, sdp_attr_print, sdp_attr_destroy, sdp_attr_clone},
        {"m", SDP_HEADER_MEDIA, sdp_media_parse, sdp_media_print, sdp_media_destroy, sdp_media_clone},
        SDP_HMAP_NULL
};

SipsterSdpMediaTypeEnum sdp_media_type_decode(char * mt);

#define SDP_MTYPE_NULL {NULL_STRING, SDP_MEDIA_NONE}
const SipsterSdpMediaTypeMap sdp_media_types[] {
        {"audio", SDP_MEDIA_AUDIO},
        {"video", SDP_MEDIA_VIDEO},
        {"text", SDP_MEDIA_TEXT},
        {"application", SDP_MEDIA_APP},
        {"message", SDP_MEDIA_MSG},
        SDP_MTYPE_NULL
};

SipsterSdpAttribute* sdp_attr_no_parse(SipsterSdpAttributeEnum type, const char *input);
char* sdp_attr_no_print(SipsterSdpAttribute *attr);
SipsterSdpAttribute* sdp_attr_no_clone(SipsterSdpAttribute *attr);

SipsterSdpAttribute* sdp_attr_rtpmap_parse(SipsterSdpAttributeEnum type, const char *input);
char* sdp_attr_rtpmap_print(SipsterSdpAttribute *header);
SipsterSdpAttribute* sdp_attr_rtpmap_clone(SipsterSdpAttribute *attr);

SipsterSdpAttribute* sdp_attr_fmtp_parse(SipsterSdpAttributeEnum type, const char *input);
char* sdp_attr_fmtp_print(SipsterSdpAttribute *header);
SipsterSdpAttribute* sdp_attr_fmtp_clone(SipsterSdpAttribute *attr);

SipsterSdpAttribute* sdp_attr_flag_parse(SipsterSdpAttributeEnum type, const char *input);
char* sdp_attr_flag_print(SipsterSdpAttribute *header);
SipsterSdpAttribute* sdp_attr_flag_clone(SipsterSdpAttribute *attr);

#define SDP_ATTRIBUTE_NULL {NULL_STRING, SDP_ATTRIBUTE_NONE, NULL, NULL, NULL}
const SipsterSdpAttributeMap sdp_attribute_types[] {
        {"cat", SDP_ATTRIBUTE_CATEGORY, sdp_attr_no_parse, sdp_attr_no_print, sdp_attr_no_clone},
        {"keywds", SDP_ATTRIBUTE_KEYWORDS, sdp_attr_no_parse, sdp_attr_no_print, sdp_attr_no_clone},
        {"tool", SDP_ATTRIBUTE_TOOL, sdp_attr_no_parse, sdp_attr_no_print, sdp_attr_no_clone},
        {"ptime", SDP_ATTRIBUTE_PTIME, sdp_attr_no_parse, sdp_attr_no_print, sdp_attr_no_clone},
        {"maxptime", SDP_ATTRIBUTE_MAXPTIME, sdp_attr_no_parse, sdp_attr_no_print, sdp_attr_no_clone},
        {"rtpmap", SDP_ATTRIBUTE_RTPMAP, sdp_attr_rtpmap_parse, sdp_attr_rtpmap_print, sdp_attr_rtpmap_clone},
        {"recvonly", SDP_ATTRIBUTE_RECVONLY, sdp_attr_no_parse, sdp_attr_no_print, sdp_attr_no_clone},
        {"sendrecv", SDP_ATTRIBUTE_SENDRECV, sdp_attr_no_parse, sdp_attr_no_print, sdp_attr_no_clone},
        {"sendonly", SDP_ATTRIBUTE_SENDONLY, sdp_attr_flag_parse, sdp_attr_flag_print, sdp_attr_flag_clone},
        {"inactive", SDP_ATTRIBUTE_INACTIVE, sdp_attr_no_parse, sdp_attr_no_print, sdp_attr_no_clone},
        {"orient", SDP_ATTRIBUTE_ORIENT, sdp_attr_no_parse, sdp_attr_no_print, sdp_attr_no_clone},
        {"type", SDP_ATTRIBUTE_TYPE, sdp_attr_no_parse, sdp_attr_no_print, sdp_attr_no_clone},
        {"charset", SDP_ATTRIBUTE_CHARSET, sdp_attr_no_parse, sdp_attr_no_print, sdp_attr_no_clone},
        {"sdplang", SDP_ATTRIBUTE_SDPLANG, sdp_attr_no_parse, sdp_attr_no_print, sdp_attr_no_clone},
        {"lang", SDP_ATTRIBUTE_LANG, sdp_attr_no_parse, sdp_attr_no_print, sdp_attr_no_clone},
        {"framerate", SDP_ATTRIBUTE_FRAMERATE, sdp_attr_no_parse, sdp_attr_no_print, sdp_attr_no_clone},
        {"quality", SDP_ATTRIBUTE_QUALITY, sdp_attr_no_parse, sdp_attr_no_print, sdp_attr_no_clone},
        {"fmtp", SDP_ATTRIBUTE_FMTP, sdp_attr_fmtp_parse, sdp_attr_fmtp_print, sdp_attr_fmtp_clone},
        SDP_ATTRIBUTE_NULL
};

#endif //SIPSTER_SDP_PRIVATE_H
