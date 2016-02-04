#ifndef SIP_HEADERS_H
#define SIP_HEADERS_H

#include <stdlib.h>
#include <sipster/sip_enums.h>

#define NULL_STRING "\0"

#define SIP_PROTOCOL_AND_TRANSPORT "SIP/2.0/UDP"
#define SIP_PROTOCOL "SIP/2.0"

#define SIP_HEADER(x) (SipsterSipHeader *) x

#define SIP_HEADER_CREATE(id, htype) (htype *) sipster_init_basic_header(id, sizeof(htype))
#define SIP_HEADER_WITH_PARAMS_CREATE(id, htype) (htype *) sipster_init_params_header(id, sizeof(htype))

#define SIP_HEADER_TAG_CREATE() sipster_generate_random_string(13)
#define SIP_HEADER_BRANCH_CREATE() sipster_generate_random_string(20)

#define SIP_HEADER_FIELD_COPY(field, data) strncpy(field, data, sizeof(field)-1)

#define SIP_ADDRESS_NONE {NULL_STRING, NULL_STRING, NULL_STRING, NULL_STRING, 0}

typedef struct _SipsterSipAddress SipsterSipAddress;
typedef struct _SipsterSipHeaderAddress SipsterSipHeaderAddress;
typedef struct _SipsterSipHeaderVia SipsterSipHeaderVia;
typedef struct _SipsterSipHeader SipsterSipHeader;
typedef struct _SipsterSipHeaderWithParams SipsterSipHeaderWithParams;
typedef struct _SipsterSipParameter SipsterSipParameter;
typedef struct _SipsterSipHeaderAddress SipsterSipHeaderTo;
typedef struct _SipsterSipHeaderAddress SipsterSipHeaderContact;
typedef struct _SipsterSipHeaderAddress SipsterSipHeaderFrom;
typedef struct _SipsterSipHeaderCSeq SipsterSipHeaderCSeq;
typedef struct _SipsterSipHeaderLeaf SipsterSipHeaderLeaf;
typedef struct _SipsterSipHeaderLeafMetadata SipsterSipHeaderLeafMetadata;
typedef struct _SipsterSipHeaderInteger SipsterSipHeaderContentLength;
typedef struct _SipsterSipHeaderContentType SipsterSipHeaderContentType;
typedef struct _SipsterSipHeaderString SipsterSipHeaderCallID;
typedef struct _SipsterSipHeaderString SipsterSipHeaderUserAgent;
typedef struct _SipsterSipHeaderString SipsterSipHeaderString;
typedef struct _SipsterSipHeaderString SipsterSipHeaderAccept;
typedef struct _SipsterSipHeaderString SipsterSipHeaderAllow;
typedef struct _SipsterSipHeaderString SipsterSipHeaderSupported;
typedef struct _SipsterSipHeaderInteger SipsterSipHeaderInteger;
typedef struct _SipsterSipHeaderInteger SipsterSipHeaderMaxFwds;

struct _SipsterSipParameter {
    char name[20];
    char value[30];
    SipsterSipParameter * next;
};

struct _SipsterSipAddress {
    char schema[10];
    char name[254];
    char user[254];
    char domain[254];
    unsigned short port;
};

struct _SipsterSipHeader {
    const char *headerName;
    const char *shortName;
    SipsterSipHeaderEnum headerId;
};

typedef struct _SipsterSipHeaderSessionID {
    SipsterSipHeader header;
    char * sessionID;
} SipsterSipHeaderSessionID;

struct _SipsterSipHeaderWithParams {
    SipsterSipHeader header;
    SipsterSipParameter * first;
};

struct _SipsterSipHeaderAddress {
    SipsterSipHeaderWithParams header;
    char address[100];
};

struct _SipsterSipHeaderVia {
   SipsterSipHeaderWithParams header;
    const char *protocol;
    char address[60];
};

struct _SipsterSipHeaderCSeq {
    SipsterSipHeader header;
    unsigned long seq;
    const char *requestMethod;
    SipsterSipMethodEnum methodId;
};

struct _SipsterSipHeaderContentType {
    SipsterSipHeaderWithParams header;
    char contentType[150];
};

struct _SipsterSipHeaderString {
    SipsterSipHeader header;
    char data[300];
};

struct _SipsterSipHeaderInteger {
    SipsterSipHeader header;
    unsigned int number;
};

struct _SipsterSipHeaderLeafMetadata {
    size_t count;
};

struct _SipsterSipHeaderLeaf {
    SipsterSipHeaderLeafMetadata *metadata;
    unsigned short index;
    SipsterSipHeader * header;
    SipsterSipHeaderLeaf * next;
};

typedef SipsterSipHeader* (*ParseSipHeader)(SipsterSipHeaderEnum, const char *);
typedef char* (*PrintSipHeader)(SipsterSipHeader *);
typedef void(*DestroySipHeader)(SipsterSipHeader *);

typedef struct _SipsterSipHeaderMap {
    const char headerName[20];
    const char shortName[2];
    SipsterSipHeaderEnum headerId;
    ParseSipHeader parse;
    PrintSipHeader print;
    DestroySipHeader destroy;
} SipsterSipHeaderMap;

void no_destroy(SipsterSipHeader * header);
SipsterSipHeader* no_parse(SipsterSipHeaderEnum id, const char * input);
char* no_print(SipsterSipHeader * header);

SipsterSipHeader* addr_parse(SipsterSipHeaderEnum, const char *);
char* addr_print(SipsterSipHeader * header);
void addr_destroy(SipsterSipHeader * header);

SipsterSipHeader* via_parse(SipsterSipHeaderEnum, const char *);
char* via_print(SipsterSipHeader * header);
void via_destroy(SipsterSipHeader * header);

SipsterSipHeader* cseq_parse(SipsterSipHeaderEnum, const char *);
char* cseq_print(SipsterSipHeader * header);

SipsterSipHeader* ct_parse(SipsterSipHeaderEnum, const char *);
char* ct_print(SipsterSipHeader * header);
void ct_destroy(SipsterSipHeader * header);

SipsterSipHeader* ci_parse(SipsterSipHeaderEnum, const char *);
char* ci_print(SipsterSipHeader * header);

SipsterSipHeader* string_parse(SipsterSipHeaderEnum, const char *);
char* string_print(SipsterSipHeader * header);

SipsterSipHeader* uint_parse(SipsterSipHeaderEnum, const char *);
char* uint_print(SipsterSipHeader * header);

#define SIPSTER_SIP_HEADER_MAP_SIZE 15
#define HMAP_NULL {NULL_STRING, NULL_STRING, SIP_HEADER_NONE, NULL, NULL, NULL}
const SipsterSipHeaderMap header_prototypes[] = {
    {"To", "t", SIP_HEADER_TO, addr_parse, addr_print, addr_destroy},
    {"From", "f", SIP_HEADER_FROM, addr_parse, addr_print, addr_destroy},
    {"Via", "v", SIP_HEADER_VIA, via_parse, via_print, via_destroy},
    {"CSeq", NULL_STRING, SIP_HEADER_CSEQ, cseq_parse, cseq_print, no_destroy},
    {"Accept", NULL_STRING, SIP_HEADER_ACCEPT, string_parse, string_print, no_destroy},
    {"Content-Length", "l", SIP_HEADER_CONTENT_LENGTH, uint_parse, uint_print, no_destroy},
    {"Content-Type", "c", SIP_HEADER_CONTENT_TYPE, ct_parse, ct_print, ct_destroy},
    {"Contact", "m", SIP_HEADER_CONTACT, addr_parse, addr_print, addr_destroy},
    {"Call-ID", "i", SIP_HEADER_CALL_ID, ci_parse, ci_print, no_destroy},
    {"Call-Info", NULL_STRING, SIP_HEADER_CALL_INFO, no_parse, no_print, no_destroy},
    {"User-Agent", NULL_STRING, SIP_HEADER_USER_AGENT, string_parse, string_print, no_destroy},
    {"Session-ID", NULL_STRING, SIP_HEADER_SESSION_ID, no_parse, no_print, no_destroy},
    {"Max-Forwards", NULL_STRING, SIP_HEADER_MAX_FORWARDS, uint_parse, uint_print, no_destroy},
    {"Allow", NULL_STRING, SIP_HEADER_ALLOW, string_parse, string_print, no_destroy},
    {"Supported", NULL_STRING, SIP_HEADER_SUPPORTED, string_parse, string_print, no_destroy},
    HMAP_NULL
};

const char * const header_names[] = {"To", "From", "Via", "CSeq", "Accept", "Content-Length", "Content-Type", "Call-ID", "Contact", "Contact", "Call-Info", "User-Agent", "Session-ID",
                                     "Accept-Contact", "Accept-Encoding", "Accept-Language", "Accept-Resource-Priority", "Alert-Info", "Allow", "Allow-Events", "Answer-Mode", "Authentication-Info",
                                     "Authorization", "Content-Disposition", "Content-Encoding", "Content-Language", "Date", "Error-Info", "Event", "Expires", "Feature-Caps", "Flow-Timer",
                                     "Geolocation", "Geolocation-Error", "Geolocation-Routing", "History-Info", "Identity", "Identity-Info", "Info-Package", "In-Reply-To", "Join",
                                     "Max-Breadth", "Max-Forwards", "MIME-Version", "Min-Expires", "Min-SE", "Organization", "Path", "Permission-Missing", "Policy-Contact", "Policy-ID",
                                     "Priority", "Privacy", "Priv-Answer-Mode", "Proxy-Authenticate", "Proxy-Authorization", "Proxy-Require", "RAck", "Reason", "Record-Route", "Recv-Info",
                                     "Refer-Events-At", "Refer-Sub", "Refer-To", "Referred-By", "Reject-Contact", "Replaces", "Reply-To", "Request-Disposition", "Require",
                                     "Resource-Priority", "Retry-After", "Route", "RSeq", "Security-Client", "Security-Server", "Security-Verify", "Server", "Service-Route",
                                     "Session-Expires", "SIP-ETag", "SIP-If-Match", "Subject", "Subscription-State", "Supported", "Suppress-If-Match", "Target-Dialog",
                                     "Timestamp", "Trigger-Consent", "Unsupported", "UUI (User-to-User)", "Warning", "WWW-Authenticate"};

SipsterSipHeader * sipster_sip_header_create(size_t size);
void sipster_sip_header_destroy(SipsterSipHeader * header);

SipsterSipParameter * sipster_sip_parameter_create(const char * key, const char * value);
void sipster_sip_parameter_destroy(SipsterSipParameter * param);
SipsterSipParameter * sipster_sip_parameter_get(const char *name, SipsterSipParameter *first);
void sipster_sip_header_append_parameter(SipsterSipHeaderWithParams *header, SipsterSipParameter *param);

SipsterSipHeaderLeaf * sipster_append_new_header(SipsterSipHeaderLeaf * leaf, SipsterSipHeader * header);
void sipster_append_destroy(SipsterSipHeaderLeaf * leaf, int destroyHeader);
SipsterSipHeaderLeaf * sipster_get_header(SipsterSipHeaderEnum headerId, SipsterSipHeaderLeaf *first, SipsterSipHeaderLeaf *last);

SipsterSipAddress * sipster_parse_address(const char * address);
int sipster_parse_address_inPlace(const char * address, SipsterSipAddress * addrObject);

SipsterSipHeader * sipster_init_basic_header(SipsterSipHeaderEnum id, size_t allocation_size);
SipsterSipHeader * sipster_init_params_header(SipsterSipHeaderEnum id, size_t allocation_size);
#endif // SIP_HEADERS_H

