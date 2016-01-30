#ifndef SIP_H
#define SIP_H

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sipster/sip_headers.h>

#define SIP_PROTOCOL_AND_TRANSPORT "SIP/2.0/UDP"

typedef enum {
    SIP_METHOD_INVITE = 0,
    SIP_METHOD_ACK,
    SIP_METHOD_BYE,
    SIP_METHOD_CANCEL,
    SIP_METHOD_OPTIONS,
    SIP_METHOD_REGISTER,
    SIP_METHOD_PRACK,
    SIP_METHOD_SUBSCRIBE,
    SIP_METHOD_NOTIFY,
    SIP_METHOD_PUBLISH,
    SIP_METHOD_INFO,
    SIP_METHOD_REFER,
    SIP_METHOD_MESSAGE,
    SIP_METHOD_UPDATE
} SipsterSipMethodEnum;

typedef enum {
    // PROVISIONAL - 1xx
        SIP_STATUS_100_TRYING = 0,
        SIP_STATUS_180_RINGING,
        SIP_STATUS_181_CALL_FORWARDING,
        SIP_STATUS_182_QUEUED,
        SIP_STATUS_183_SESSION_IN_PROGRESS,
        SIP_STATUS_199_EARLY_DIALOG_TERMINATED,

        // SUCCESS - 2xx
        SIP_STATUS_200_OK,
        SIP_STATUS_202_ACCEPTED,
        SIP_STATUS_204_NO_NOTIFICATION,

        // REDIRECTS - 3xx
        SIP_STATUS_300_MULTIPLE_CHOICES,
        SIP_STATUS_301_MOVED_PERMANENTLY,
        SIP_STATUS_302_MOVED_TEMPORARILY,
        SIP_STATUS_305_USE_PROXY,
        SIP_STATUS_380_ALTERNATIVE_SERVICE,

        // CLIENT FAILURES - 4xx
        SIP_STATUS_400_BAD_REQUEST,
        SIP_STATUS_401_UNAUTHORIZED,
        SIP_STATUS_402_PAYMENT_REQUIRED,
        SIP_STATUS_403_FORBIDDEN,
        SIP_STATUS_404_NOT_FOUND,
        SIP_STATUS_405_METHOD_NOT_ALLOWED,
        SIP_STATUS_406_NOT_ACCEPTABLE,
        SIP_STATUS_407_PROXY_AUTHENTICATION_REQUIRED,
        SIP_STATUS_408_REQUEST_TIMEOUT,
        SIP_STATUS_409_CONFLICT,
        SIP_STATUS_410_GONE,
        SIP_STATUS_411_LENGTH_REQUIRED,
        SIP_STATUS_412_CONDITIONAL_REQUEST_FAILED,
        SIP_STATUS_413_REQUEST_ENTITY_TOO_LARGE,
        SIP_STATUS_414_REQUEST_URI_TOO_LONG,
        SIP_STATUS_415_UNSUPPORTED_MEDIA_TYPE,
        SIP_STATUS_416_UNSUPPORTED_URI_SCHEME,
        SIP_STATUS_417_UNKNOWN_RESOURCE_PRIORITY,
        SIP_STATUS_420_BAD_EXTENSION,
        SIP_STATUS_421_EXTENSION_REQUIRED,
        SIP_STATUS_422_SESSION_INTERVAL_TOO_SMALL,
        SIP_STATUS_423_INTERVAL_TOO_BRIEF,
        SIP_STATUS_424_BAD_LOCATION_INFORMATION,
        SIP_STATUS_428_USE_IDENTITY_HEADER,
        SIP_STATUS_429_PROVIDE_REFERRER_IDENTITY,
        SIP_STATUS_430_FLOW_FAILED,
        SIP_STATUS_433_ANONYMITY_DISALLOWED,
        SIP_STATUS_436_BAD_IDENTITY_INFO,
        SIP_STATUS_437_UNSUPPORTED_CERTIFICATE,
        SIP_STATUS_438_INVALID_IDENTITY_HEADER,
        SIP_STATUS_439_FIRST_HOP_LACKS_OUTBOUND_SUPPORT,
        SIP_STATUS_470_CONSENT_NEEDED,
        SIP_STATUS_480_TEMPORARILY_UNAVAILABLE,
        SIP_STATUS_481_CALL_TRANSACTION_DOES_NOT_EXIST,
        SIP_STATUS_482_LOOP_DETECTED,
        SIP_STATUS_483_TOO_MANY_HOPS,
        SIP_STATUS_484_ADDRESS_INCOMPLETE,
        SIP_STATUS_485_AMBIGUOUS,
        SIP_STATUS_486_BUSY_HERE,
        SIP_STATUS_487_REQUEST_TERMINATED,
        SIP_STATUS_488_NOT_ACCEPTABLE_HERE,
        SIP_STATUS_489_BAD_EVENT,
        SIP_STATUS_491_REQUEST_PENDING,
        SIP_STATUS_493_UNDECIPHERABLE,
        SIP_STATUS_494_SECURITY_AGREEMENT_REQUIRED,

        // SERVER FAILURES = 5xx
        SIP_STATUS_500_SERVER_INTERNAL_ERROR,
        SIP_STATUS_501_NOT_IMPLEMENTED,
        SIP_STATUS_502_BAD_GATEWAY,
        SIP_STATUS_503_SERVICE_UNAVAILABLE,
        SIP_STATUS_504_SERVER_TIME_OUT,
        SIP_STATUS_505_VERSION_NOT_SUPPORTED,
        SIP_STATUS_513_MESSAGE_TOO_LARGE,
        SIP_STATUS_580_PRECONDITION_FAILURE,

        // GLOBAL FAILURES
        SIP_STATUS_600_BUSY_EVERYWHERE,
        SIP_STATUS_603_DECLINE,
        SIP_STATUS_604_DOES_NOT_EXIST_ANYWHERE,
        SIP_STATUS_606_NOT_ACCEPTABLE,

} SipsterSipStatusEnum;

#define NUM_SIP_METHODS 14
#define NUM_SIP_STATUSES 71
const char * const method_names[] = { "INVITE", "ACK", "BYE", "CANCEL", "OPTIONS", "REGISTER", "PRACK", "SUBSCRIBE", "NOTIFY", "PUBLISH", "INFO", "REFER", "MESSAGE", "UPDATE" };
const SipsterSipMethodEnum method_ids[] = { SIP_METHOD_INVITE, SIP_METHOD_ACK, SIP_METHOD_BYE, SIP_METHOD_CANCEL, SIP_METHOD_OPTIONS, SIP_METHOD_REGISTER, SIP_METHOD_PRACK, SIP_METHOD_SUBSCRIBE, SIP_METHOD_NOTIFY, SIP_METHOD_PUBLISH, SIP_METHOD_INFO, SIP_METHOD_REFER, SIP_METHOD_MESSAGE, SIP_METHOD_UPDATE};

const int statuses[] = {100, 180, 181, 182, 183, 199, 200, 202, 204, 300, 301, 302, 305, 380, 400, 401, 402, 403, 404, 405, 406, 407, 408, 409, 410, 411, 412, 413, 414, 415, 416, 417, 420, 421, 422, 423, 424, 428, 429, 430, 433, 436, 437, 438, 439, 470, 480, 481, 482, 483, 484, 485, 486, 487, 488, 489, 491, 493, 494, 500, 501, 502, 503, 504, 505, 513, 580, 600, 603, 604, 606};
const char * const status_phrases[] = {"Trying", "Ringing", "Call Forwarding", "Queued", "Session In Progress", "Early Dialog Terminated", "Ok", "Accepted", "No Notification", "Multiple Choices", "Moved Permanently", "Moved Temporarily", "Use Procy", "Alternative Service", "Bad Request", "Unauthorized", "Payment Required", "Forbidden", "Not Found", "Method Not Allowed", "Not Acceptable", "Proxy Authentication Required", "Request Timeout", "Conflict", "Gone", "Length Required", "Conditional Request Failed", "Request Entity Too Large", "Request Uri Too Long", "Unsupported Media Type", "Unsupported Uri Scheme", "Unknown Resource Priority", "Bad Extension", "Extension Required", "Session Interval Too Small", "Interval Too Brief", "Bad Location Information", "Use Identity Header", "Provide Referrer Identity", "Flow Failed", "Anonymity Disallowed", "Bad Identity Info", "Unsupported Certificate", "Invalid Identity Header", "First Hop Lacks Outbound Support", "Consent Needed", "Temporarily Unavailable", "Call Transaction Does Not Exist", "Loop Detected", "Too Many Hops", "Address Incomplete", "Ambiguous", "Busy Here", "Request Terminated", "Not Acceptable Here", "Bad Event", "Request Pending", "Undecipherable", "Security Agreement Required", "Server Internal Error", "Not Implemented", "Bad Gateway", "Service Unavailable", "Server Time Out", "Version Not Supported", "Message Too Large", "Precondition Failure", "Busy Everywhere", "Decline", "Does Not Exist Anywhere", "Not Acceptable"};

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
} SipsterSipResponse;

SipsterSipRequest *sipster_request_parse(const char *input, size_t size, int *result);

SipsterSipRequest *sipster_request_create();
void sipster_request_destroy(SipsterSipRequest *request);

SipsterSipResponse *sipster_response_create();
void sipster_response_destroy(SipsterSipResponse *response);

int sipster_request_parse_line(const char * input, SipsterSipRequest * sipRequest);
SipsterSipHeader * sipster_request_parse_header(const char * input);
char * sipster_request_print_header(SipsterSipHeader * header);

int sipster_response_parse_line(const char * input, SipsterSipResponse * sipResponse);
SipsterSipHeader * sipster_response_parse_header(const char * input);
char * sipster_response_print_header(SipsterSipHeader * header);

#endif // SIP_H

