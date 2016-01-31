#ifndef SIP_H
#define SIP_H

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sipster/sip_enums.h>
#include <sipster/sip_headers.h>

#define SIP_PROTOCOL_AND_TRANSPORT "SIP/2.0/UDP"

typedef char * SipsterSipRequestUri;
typedef char * SipsterSipVersion;

typedef struct _SipsterSipContentBody {
    char contentType[150];
    size_t length;
    char data[1500];
} SipsterSipContentBody;

typedef struct _SipsterSipRequestMnemonic {
    const char * method;
    SipsterSipMethodEnum methodId;
} SipsterSipRequestMnemonic;

// Request-Line  =  Method SP Request-URI SP SIP-Version CRLF
typedef struct _SipsterSipRequestLine {
    SipsterSipRequestMnemonic method;
    SipsterSipRequestUri requestUri;
    SipsterSipVersion version;
} SipsterSipRequestLine;

typedef struct _SipsterSipRequest {
    SipsterSipRequestLine requestLine;
    SipsterSipHeaderLeaf * firstHeader;
    SipsterSipHeaderLeaf * lastHeader;
    SipsterSipContentBody * content;
} SipsterSipRequest;

typedef struct _SipsterSipResponseMnemonic {
    int statusCode;
    SipsterSipStatusEnum status;
    const char * reason;
} SipsterSipResponseMnemonic;

// Status-Line  =  SIP-Version SP Status-Code SP Reason-Phrase CRLF
typedef struct _SipsterSipResponseLine {
    SipsterSipVersion version;
    SipsterSipResponseMnemonic status;
} SipsterSipResponseLine;

typedef struct _SipsterSipResponse {
    SipsterSipResponseLine responseLine;
    SipsterSipHeaderLeaf * firstHeader;
    SipsterSipHeaderLeaf * lastHeader;
    SipsterSipContentBody * content;
} SipsterSipResponse;

typedef struct _SipsterSipMessagePrint {
    char * output;
    size_t size;
} SipsterSipMessagePrint;

SipsterSipRequest *sipster_request_parse(const char *input, size_t size, int *result);
SipsterSipMessagePrint * sipster_request_print(SipsterSipRequest * request);
SipsterSipRequest *sipster_request_create();
void sipster_request_destroy(SipsterSipRequest *request);

SipsterSipResponse *sipster_response_parse(const char *input, size_t size, int *result);
SipsterSipMessagePrint * sipster_response_print(SipsterSipResponse * response);
SipsterSipResponse *sipster_response_create();
void sipster_response_destroy(SipsterSipResponse *response);

int sipster_request_parse_line(const char * input, SipsterSipRequest * sipRequest);
SipsterSipHeader * sipster_request_parse_header(const char * input);
char * sipster_request_print_line(SipsterSipRequestLine * line);
char * sipster_request_print_header(SipsterSipHeader * header);

int sipster_response_parse_line(const char * input, SipsterSipResponse * sipResponse);
SipsterSipHeader * sipster_response_parse_header(const char * input);
char * sipster_response_print_line(SipsterSipResponseLine * line);
char * sipster_response_print_header(SipsterSipHeader * header);



#endif // SIP_H

