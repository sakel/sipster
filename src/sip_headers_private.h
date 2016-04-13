#ifndef SIP_HEADER_PRIVATE_H
#define SIP_HEADER_PRIVATE_H

#include <sipster/sip_headers.h>

typedef SipsterSipHeader* (*ParseSipHeader)(SipsterSipHeaderEnum, const char *);
typedef char* (*PrintSipHeader)(SipsterSipHeader *);
typedef void(*DestroySipHeader)(SipsterSipHeader *);
typedef SipsterSipHeader* (*CloneSipHeader)(SipsterSipHeader *);

typedef struct _SipsterSipHeaderMap {
    const char headerName[20];
    const char shortName[2];
    SipsterSipHeaderEnum headerId;
    ParseSipHeader parse;
    PrintSipHeader print;
    DestroySipHeader destroy;
    CloneSipHeader clone;
} SipsterSipHeaderMap;

void no_destroy(SipsterSipHeader * header);
SipsterSipHeader* no_parse(SipsterSipHeaderEnum id, const char * input);
char* no_print(SipsterSipHeader * header);
SipsterSipHeader *hclone(SipsterSipHeader *header);

SipsterSipHeader* addr_parse(SipsterSipHeaderEnum, const char *);
char* addr_print(SipsterSipHeader * header);
void addr_destroy(SipsterSipHeader * header);
SipsterSipHeader *addr_hclone(SipsterSipHeader *header);

SipsterSipHeader* via_parse(SipsterSipHeaderEnum, const char *);
char* via_print(SipsterSipHeader * header);
void via_destroy(SipsterSipHeader * header);
SipsterSipHeader *via_hclone(SipsterSipHeader *header);

SipsterSipHeader* cseq_parse(SipsterSipHeaderEnum, const char *);
char* cseq_print(SipsterSipHeader * header);
SipsterSipHeader *cseq_hclone(SipsterSipHeader *header);

SipsterSipHeader* ct_parse(SipsterSipHeaderEnum, const char *);
char* ct_print(SipsterSipHeader * header);
void ct_destroy(SipsterSipHeader * header);
SipsterSipHeader *ct_hclone(SipsterSipHeader *header);

SipsterSipHeader* ci_parse(SipsterSipHeaderEnum, const char *);
char* ci_print(SipsterSipHeader * header);
SipsterSipHeader *ci_hclone(SipsterSipHeader *header);

SipsterSipHeader* string_parse(SipsterSipHeaderEnum, const char *);
char* string_print(SipsterSipHeader * header);
SipsterSipHeader *string_hclone(SipsterSipHeader *header);

SipsterSipHeader* uint_parse(SipsterSipHeaderEnum, const char *);
char* uint_print(SipsterSipHeader * header);
SipsterSipHeader *uint_hclone(SipsterSipHeader *header);

#define SIPSTER_SIP_HEADER_MAP_SIZE 16
#define HMAP_NULL {NULL_STRING, NULL_STRING, SIP_HEADER_NONE, NULL, NULL, NULL}
const SipsterSipHeaderMap header_prototypes[] = {
    {"To", "t", SIP_HEADER_TO, addr_parse, addr_print, addr_destroy, addr_hclone},
    {"From", "f", SIP_HEADER_FROM, addr_parse, addr_print, addr_destroy, addr_hclone},
    {"Via", "v", SIP_HEADER_VIA, via_parse, via_print, via_destroy, via_hclone},
    {"CSeq", NULL_STRING, SIP_HEADER_CSEQ, cseq_parse, cseq_print, no_destroy, cseq_hclone},
    {"Accept", NULL_STRING, SIP_HEADER_ACCEPT, string_parse, string_print, no_destroy, string_hclone},
    {"Content-Length", "l", SIP_HEADER_CONTENT_LENGTH, uint_parse, uint_print, no_destroy, uint_hclone},
    {"Content-Type", "c", SIP_HEADER_CONTENT_TYPE, ct_parse, ct_print, ct_destroy, ct_hclone},
    {"Contact", "m", SIP_HEADER_CONTACT, addr_parse, addr_print, addr_destroy, addr_hclone},
    {"Call-ID", "i", SIP_HEADER_CALL_ID, ci_parse, ci_print, no_destroy, ci_hclone},
    {"Call-Info", NULL_STRING, SIP_HEADER_CALL_INFO, no_parse, no_print, no_destroy, hclone},
    {"User-Agent", NULL_STRING, SIP_HEADER_USER_AGENT, string_parse, string_print, no_destroy, string_hclone},
    {"Session-ID", NULL_STRING, SIP_HEADER_SESSION_ID, no_parse, no_print, no_destroy, hclone},
    {"Max-Forwards", NULL_STRING, SIP_HEADER_MAX_FORWARDS, uint_parse, uint_print, no_destroy, uint_hclone},
    {"Allow", NULL_STRING, SIP_HEADER_ALLOW, string_parse, string_print, no_destroy, string_hclone},
    {"Supported", NULL_STRING, SIP_HEADER_SUPPORTED, string_parse, string_print, no_destroy, string_hclone},
    {"Content-Disposition", NULL_STRING, SIP_HEADER_CONTENT_DISPOSITION, string_parse, string_print, no_destroy, string_hclone},
    HMAP_NULL
};

#endif // SIP_HEADER_PRIVATE_H

