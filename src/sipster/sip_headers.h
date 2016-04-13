#ifndef SIP_HEADERS_H
#define SIP_HEADERS_H

#include <stdlib.h>
#include <sipster/sip_enums.h>
#include <sipster/base.h>

#define NULL_STRING "\0"

#define SIP_PROTOCOL_AND_TRANSPORT "SIP/2.0/UDP"
#define SIP_PROTOCOL "SIP/2.0"

#define SIP_HEADER(x) (SipsterSipHeader *) x
#define SIP_HEADER_WITH_PARAMS(x) (SipsterSipHeaderWithParams *) x
#define SIP_HEADER_WITH_PARAMS_FIRST(x) ((SipsterSipHeaderWithParams *) x)->first

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
    char schema[16];
    char name[256];
    char user[256];
    char domain[256];
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
    SipsterSipAddress *parsedAddress;
};

struct _SipsterSipHeaderVia {
    SipsterSipHeaderWithParams header;
    const char *protocol;
    char address[256];
    SipsterInetAddress *parsedAddress;
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
SipsterSipHeader * sipster_sip_header_clone(SipsterSipHeader *header);

SipsterSipParameter * sipster_sip_parameter_create(const char * key, const char * value);
void sipster_sip_parameter_destroy(SipsterSipParameter * param);
SipsterSipParameter * sipster_sip_parameter_get(const char *name, SipsterSipParameter *first);
void sipster_sip_header_append_parameter(SipsterSipHeaderWithParams *header, SipsterSipParameter *param);
SipsterSipParameter * sipster_sip_parameter_clone(SipsterSipParameter *param);
void sipster_sip_parameters_clone(SipsterSipHeaderWithParams *header, SipsterSipHeaderWithParams *cpy);

SipsterSipHeaderLeaf *sipster_sip_append_new_header(SipsterSipHeaderLeaf *leaf, SipsterSipHeader *header);
void sipster_sip_header_append_destroy(SipsterSipHeaderLeaf *leaf, int destroyHeader);
SipsterSipHeaderLeaf *sipster_sip_get_header(SipsterSipHeaderEnum headerId, SipsterSipHeaderLeaf *first,
                                             SipsterSipHeaderLeaf *last);

SipsterSipAddress * sipster_address_parse(const char * address);
int sipster_address_parse_inPlace(const char * address, SipsterSipAddress * addrObject);
char *sipster_address_uri_print(SipsterSipAddress *addr);
char *sipster_address_print(SipsterSipAddress *addr);
SipsterSipAddress * sipster_address_clone(SipsterSipAddress *addr);

SipsterSipHeader * sipster_init_basic_header(SipsterSipHeaderEnum id, size_t allocation_size);
SipsterSipHeader * sipster_init_params_header(SipsterSipHeaderEnum id, size_t allocation_size);
#endif // SIP_HEADERS_H

