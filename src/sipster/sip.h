#ifndef SIP_H
#define SIP_H

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sipster/sip_enums.h>
#include <sipster/sip_headers.h>
#include <sipster/base.h>

#define SIP_METHOD_NAME(id) method_names[id]

#define SIP_BODY_CREATE(body, cType, content, len) body = (SipsterSipContentBody *) sipster_allocator(sizeof(SipsterSipContentBody)); \
strncpy(body->contentType, cType, sizeof(body->contentType)); \
strncpy(body->data, content, sizeof(body->data));\
body->length = len \

#define SIP_SIPSTER_HANDLE(x) (SipsterSipHandle *) x

#define SIP_VIA_BRANCH_MAGIC_FORMAT "z9hG4bK%s"

typedef enum {
    SIP_LEG_INBOUND,
    SIP_LEG_OUTBOUND
} SipsterSipLegDirection;

typedef struct _SipsterSipRequest SipsterSipRequest;
typedef struct _SipsterSipResponse SipsterSipResponse;
typedef struct _SipsterSipMessagePrint SipsterSipMessagePrint;
typedef struct _SipsterSipResponseLine SipsterSipResponseLine;
typedef struct _SipsterSipResponseMnemonic SipsterSipResponseMnemonic;
typedef struct _SipsterSipRequestLine SipsterSipRequestLine;
typedef struct _SipsterSipRequestMnemonic SipsterSipRequestMnemonic;
typedef struct _SipsterSipContentBody SipsterSipContentBody;

typedef struct _SipsterSipCallLeg SipsterSipCallLeg;

typedef struct _SipsterSipHandle SipsterSipHandle;

typedef char * SipsterSipRequestUri;
typedef const char * SipsterSipVersion;

typedef int (*leg_request_handler)(SipsterSipHandle * sipsterHandle, SipsterSipCallLeg * leg, SipsterSipRequest *request, void *data);
typedef int (*leg_response_handler)(SipsterSipHandle * sipsterHandle, SipsterSipCallLeg * leg, SipsterSipResponse *response, void *data);

typedef int (*leg_send_request)(SipsterSipHandle *sipsterHandle, SipsterSipCallLeg *leg, SipsterSipRequest *request, char * message, size_t size);
typedef int (*leg_send_response)(SipsterSipHandle *sipsterHandle, SipsterSipCallLeg *leg, SipsterSipResponse *response, char * message, size_t size);

struct _SipsterSipContentBody {
    char contentType[150];
    size_t length;
    char data[1500];
};

struct _SipsterSipRequestMnemonic {
    const char * method;
    SipsterSipMethodEnum methodId;
};

// Request-Line  =  Method SP Request-URI SP SIP-Version CRLF
struct _SipsterSipRequestLine {
    SipsterSipRequestMnemonic method;
    SipsterSipRequestUri requestUri;
    SipsterSipVersion version;
};

struct _SipsterSipRequest {
    SipsterSipRequestLine requestLine;
    SipsterSipHeaderLeaf * firstHeader;
    SipsterSipHeaderLeaf * lastHeader;
    SipsterSipContentBody * content;
    SipsterInetAddress * remoteAddr;
};

struct _SipsterSipResponseMnemonic {
    int statusCode;
    SipsterSipStatusEnum status;
    const char * reason;
};

// Status-Line  =  SIP-Version SP Status-Code SP Reason-Phrase CRLF
struct _SipsterSipResponseLine {
    SipsterSipVersion version;
    SipsterSipResponseMnemonic status;
    SipsterSipMethodEnum method;
};

struct _SipsterSipResponse {
    SipsterSipResponseLine responseLine;
    SipsterSipHeaderLeaf * firstHeader;
    SipsterSipHeaderLeaf * lastHeader;
    SipsterSipContentBody * content;
    SipsterInetAddress * remoteAddr;
};

struct _SipsterSipMessagePrint {
    char * output;
    size_t size;
};

struct _SipsterSipCallLeg {
    SipsterSipLegDirection direction;
    char callId[150];
    SipsterSipAddress from;
    char fromTag[15];
    SipsterSipAddress to;
    char toTag[15];
    SipsterInetAddress localAddress;
    SipsterInetAddress peerAddress;
    leg_request_handler requestHandler;
    leg_response_handler responseHandler;
    void *data;
    void *localData; //FIXME Temporary hack - remove when call state machine added
    void *remoteData; //FIXME Temporary hack - remove when call state machine added
};

struct _SipsterSipHandle {
    void *data;
    leg_send_request sendRequest;
    leg_send_response sendResponse;
    unsigned int rport = 5060;
};

SipsterSipRequest *sipster_sip_request_parse(const char *input, size_t size, int *result);
SipsterSipMessagePrint *sipster_sip_request_print(SipsterSipRequest *request);
SipsterSipRequest *sipster_sip_request_create();
void sipster_sip_request_destroy(SipsterSipRequest *request);
SipsterSipResponse *sipster_sip_request_create_response(SipsterSipHandle *sipster, SipsterSipRequest *request,
                                                        SipsterSipCallLeg *leg, SipsterSipStatusEnum status,
                                                        SipsterSipContentBody *body);

SipsterSipResponse *sipster_sip_response_parse(const char *input, size_t size, int *result);
SipsterSipMessagePrint *sipster_sip_response_print(SipsterSipResponse *response);
SipsterSipResponse *sipster_sip_response_create();
void sipster_sip_response_destroy(SipsterSipResponse *response);

int sipster_sip_request_parse_line(const char *input, SipsterSipRequest *sipRequest);
SipsterSipHeader *sipster_sip_request_parse_header(const char *input);
char *sipster_sip_request_print_line(SipsterSipRequestLine *line);
char *sipster_sip_request_print_header(SipsterSipHeader *header);

int sipster_sip_response_parse_line(const char *input, SipsterSipResponse *sipResponse);
SipsterSipHeader *sipster_sip_response_parse_header(const char *input);
char *sipster_sip_response_print_line(SipsterSipResponseLine *line);
char *sipster_sip_response_print_header(SipsterSipHeader *header);

int sipster_sip_request_reply(SipsterSipHandle *handle, SipsterSipCallLeg *leg, SipsterSipResponse *response);
int sipster_sip_request_send(SipsterSipHandle *handle, SipsterSipCallLeg *leg, SipsterSipRequest *request);

#endif // SIP_H

