#ifndef SIP_HEADERS_H
#define SIP_HEADERS_H

#include <stdlib.h>

#define NULL_STRING "\0"

typedef struct _SipsterSipHeaderAddress SipsterSipHeaderAddress;
typedef struct _SipsterSipHeaderVia SipsterSipHeaderVia;
typedef struct _SipsterSipHeader SipsterSipHeader;
typedef struct _SipsterSipHeaderWithParams SipsterSipHeaderWithParams;
typedef struct _SipsterSipParameter SipsterSipParameter;
typedef struct _SipsterSipHeaderAddress SipsterSipHeaderTo;
typedef struct _SipsterSipHeaderAddress SipsterSipHeaderFrom;
typedef struct _SipsterSipHeaderCSeq SipsterSipHeaderCSeq;
typedef struct _SipsterSipHeaderLeaf SipsterSipHeaderLeaf;
typedef struct _SipsterSipHeaderContentLength SipsterSipHeaderContentLength;
typedef struct _SipsterSipHeaderContentType SipsterSipHeaderContentType;

typedef enum _SipsterSipHeaderEnum {
    SIP_HEADER_TO = 0,
    SIP_HEADER_FROM,
    SIP_HEADER_VIA,
    SIP_HEADER_CSEQ,
    SIP_HEADER_ACCEPT,
    SIP_HEADER_CONTENT_LENGTH,
    SIP_HEADER_CONTENT_TYPE,
    SIP_HEADER_CONTACT,
    SIP_HEADER_CALL_ID,
    SIP_HEADER_CALL_INFO,
    SIP_HEADER_ACCEPT_CONTACT,
    SIP_HEADER_ACCEPT_ENCODING,
    SIP_HEADER_ACCEPT_LANGUAGE,
    SIP_HEADER_ACCEPT_RESOURCE_PRIORITY,
    SIP_HEADER_ALERT_INFO,
    SIP_HEADER_ALLOW,
    SIP_HEADER_ALLOW_EVENTS,
    SIP_HEADER_ANSWER_MODE,
    SIP_HEADER_AUTHENTICATION_INFO,
    SIP_HEADER_AUTHORIZATION,
    SIP_HEADER_CONTENT_DISPOSITION,
    SIP_HEADER_CONTENT_ENCODING,
    SIP_HEADER_CONTENT_LANGUAGE,
    SIP_HEADER_DATE,
    SIP_HEADER_ERROR_INFO,
    SIP_HEADER_EVENT,
    SIP_HEADER_EXPIRES,
    SIP_HEADER_FEATURE_CAPS,
    SIP_HEADER_FLOW_TIMER,
    SIP_HEADER_GEOLOCATION,
    SIP_HEADER_GEOLOCATION_ERROR,
    SIP_HEADER_GEOLOCATION_ROUTING,
    SIP_HEADER_HISTORY_INFO,
    SIP_HEADER_IDENTITY,
    SIP_HEADER_IDENTITY_INFO,
    SIP_HEADER_INFO_PACKAGE,
    SIP_HEADER_IN_REPLY_TO,
    SIP_HEADER_JOIN,
    SIP_HEADER_MAX_BREADTH,
    SIP_HEADER_MAX_FORWARDS,
    SIP_HEADER_MIME_VERSION,
    SIP_HEADER_MIN_EXPIRES,
    SIP_HEADER_MIN_SE,
    SIP_HEADER_ORGANIZATION,
    SIP_HEADER_PATH,
    SIP_HEADER_PERMISSION_MISSING,
    SIP_HEADER_POLICY_CONTACT,
    SIP_HEADER_POLICY_ID,
    SIP_HEADER_PRIORITY,
    SIP_HEADER_PRIVACY,
    SIP_HEADER_PRIV_ANSWER_MODE,
    SIP_HEADER_PROXY_AUTHENTICATE,
    SIP_HEADER_PROXY_AUTHORIZATION,
    SIP_HEADER_PROXY_REQUIRE,
    SIP_HEADER_RACK,
    SIP_HEADER_REASON,
    SIP_HEADER_RECORD_ROUTE,
    SIP_HEADER_RECV_INFO,
    SIP_HEADER_REFER_EVENTS_AT,
    SIP_HEADER_REFER_SUB,
    SIP_HEADER_REFER_TO,
    SIP_HEADER_REFERRED_BY,
    SIP_HEADER_REJECT_CONTACT,
    SIP_HEADER_REPLACES,
    SIP_HEADER_REPLY_TO,
    SIP_HEADER_REQUEST_DISPOSITION,
    SIP_HEADER_REQUIRE,
    SIP_HEADER_RESOURCE_PRIORITY,
    SIP_HEADER_RETRY_AFTER,
    SIP_HEADER_ROUTE,
    SIP_HEADER_RSEQ,
    SIP_HEADER_SECURITY_CLIENT,
    SIP_HEADER_SECURITY_SERVER,
    SIP_HEADER_SECURITY_VERIFY,
    SIP_HEADER_SERVER,
    SIP_HEADER_SERVICE_ROUTE,
    SIP_HEADER_SESSION_EXPIRES,
    SIP_HEADER_SESSION_ID,
    SIP_HEADER_SIP_ETAG,
    SIP_HEADER_SIP_IF_MATCH,
    SIP_HEADER_SUBJECT,
    SIP_HEADER_SUBSCRIPTION_STATE,
    SIP_HEADER_SUPPORTED,
    SIP_HEADER_SUPPRESS_IF_MATCH,
    SIP_HEADER_TARGET_DIALOG,
    SIP_HEADER_TIMESTAMP,
    SIP_HEADER_TRIGGER_CONSENT,
    SIP_HEADER_UNSUPPORTED,
    SIP_HEADER_USER_AGENT,
    SIP_HEADER_USER_TO_USER,
    SIP_HEADER_WARNING,
    SIP_HEADER_WWW_AUTHENTICATE
} SipsterSipHeaderEnum;

struct _SipsterSipParameter {
    char name[20];
    char value[30];
    SipsterSipParameter * next;
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
    char name[30];
    char address[60];
};

struct _SipsterSipHeaderVia {
   SipsterSipHeaderWithParams header;
    char protocol[30];
    char address[60];
};

struct _SipsterSipHeaderCSeq {
    SipsterSipHeader header;
    unsigned long seq;
    char requestMethod[10];
};

struct _SipsterSipHeaderContentLength {
    SipsterSipHeader header;
    unsigned int length;
};

struct _SipsterSipHeaderContentType {
    SipsterSipHeaderWithParams header;
    char contentType[150];
};

struct _SipsterSipHeaderLeaf {
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

SipsterSipHeader* cl_parse(SipsterSipHeaderEnum, const char *);
char* cl_print(SipsterSipHeader * header);

SipsterSipHeader* ct_parse(SipsterSipHeaderEnum, const char *);
char* ct_print(SipsterSipHeader * header);
void ct_destroy(SipsterSipHeader * header);

#define SIPSTER_SIP_HEADER_MAP_SIZE 12
const SipsterSipHeaderMap header_prototypes[] = {
    {"To", "t", SIP_HEADER_TO, addr_parse, addr_print, addr_destroy},
    {"From", "f", SIP_HEADER_FROM, addr_parse, addr_print, addr_destroy},
    {"Via", "v", SIP_HEADER_VIA, via_parse, via_print, via_destroy},
    {"CSeq", NULL_STRING, SIP_HEADER_CSEQ, cseq_parse, cseq_print, no_destroy},
    {"Accept", NULL_STRING, SIP_HEADER_ACCEPT, no_parse, no_print, no_destroy},
    {"Content-Length", "l", SIP_HEADER_CONTENT_LENGTH, cl_parse, cl_print, no_destroy},
    {"Content-Type", "c", SIP_HEADER_CONTENT_TYPE, ct_parse, ct_print, ct_destroy},
    {"Contact", "m", SIP_HEADER_CONTACT, addr_parse, addr_print, addr_destroy},
    {"Call-ID", "i", SIP_HEADER_CALL_ID, no_parse, no_print, no_destroy},
    {"Call-Info", NULL_STRING, SIP_HEADER_CALL_INFO, no_parse, no_print, no_destroy},
    {"User-Agent", NULL_STRING, SIP_HEADER_USER_AGENT, no_parse, no_print, no_destroy},
    {"Session-ID", NULL_STRING, SIP_HEADER_SESSION_ID, no_parse, no_print, no_destroy}
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

SipsterSipHeaderLeaf * sipster_append_new_header(SipsterSipHeaderLeaf ** leaf, SipsterSipHeader * header);
void sipster_append_destroy(SipsterSipHeaderLeaf * leaf);

#endif // SIP_HEADERS_H

