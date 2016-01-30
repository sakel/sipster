#include "sipster/sip.h"
#include <sipster/log.h>
#include <string>
#include "utils.h"

using namespace std;



int sipster_request_parse_line(const char * input, SipsterSipRequest * sipRequest) {
    string inputData;
    std::size_t pos = 0;
    int i;

    if(!input || !sipRequest) {
        SIPSTER_ERROR("NULL input parameters");
        return -1;
    }

    inputData = input;

    string methodString = nextToken(inputData, " ", &pos);
    string destinationString = nextToken(inputData, " ", &pos);
    string versionString = getRest(inputData, &pos);

    for(i = 0; i < NUM_SIP_METHODS; i++) {
        if(methodString == method_names[i]) {
            sipRequest->requestLine.method.method = method_names[i];
            sipRequest->requestLine.method.methodId = method_ids[i];
            break;
        }
    }

    sipRequest->requestLine.requestUri = strdup(destinationString.c_str());
    sipRequest->requestLine.version = strdup(versionString.c_str());

    return 0;
}

SipsterSipRequest *sipster_request_create() {
    return (SipsterSipRequest *) calloc(1, sizeof(SipsterSipRequest));
}

void sipster_request_destroy(SipsterSipRequest *request) {
    if(request) {
        if(request->requestLine.requestUri) {
            free(request->requestLine.requestUri);
        }
        if(request->requestLine.version) {
            free(request->requestLine.version);
        }
        free(request);
    }
}

SipsterSipResponse *sipster_response_create() {
    return (SipsterSipResponse *) calloc(1, sizeof(SipsterSipResponse));
}

void sipster_response_destroy(SipsterSipResponse *response) {
    if(response) {
        if(response->responseLine.version) {
            free(response->responseLine.version);
        }
        free(response);
    }

}

int sipster_response_parse_line(const char * input, SipsterSipResponse * sipResponse) {
    // Status-Line  =  SIP-Version SP Status-Code SP Reason-Phrase CRLF
    string inputData;
    std::size_t pos = 0;
    int i;

    if(!input || !sipResponse) {
        SIPSTER_ERROR("NULL input parameters");
        return -1;
    }
    inputData = input;

    string versionString = nextToken(inputData, " ", &pos);
    string statusCodeString = nextToken(inputData, " ", &pos);
    string phraseString = getRest(inputData, &pos);

    sipResponse->responseLine.version = strdup(versionString.c_str());
    for(i = 0; i < NUM_SIP_STATUSES; i++) {
        if(atoi(statusCodeString.c_str()) == statuses[i]) {
            sipResponse->responseLine.status.statusCode = statuses[i];
            sipResponse->responseLine.status.status = (SipsterSipStatusEnum)i;
            sipResponse->responseLine.status.reason = status_phrases[i];
            break;
        }
    }

    return 0;
}

SipsterSipHeader * sipster_request_parse_header(const char * input) {
    std::size_t pos = 0;
    int i = 0;
    string headerString = nextToken(input, ":", &pos);
    string rest = getRest(input, &pos);
    string cleanRest = ltrim(rest);

    for(i = 0; i < SIPSTER_SIP_HEADER_MAP_SIZE; i++) {
        const SipsterSipHeaderMap hmap = header_prototypes[i];
        if(headerString.compare(hmap.headerName) == 0
                || headerString.compare(hmap.shortName) == 0) {
            SIPSTER_DEBUG("PARSING HEADER %s", hmap.headerName);
            SipsterSipHeader * header = header_prototypes[i].parse(header_prototypes[i].headerId, cleanRest.c_str());
            return header;
        }
    }
    return NULL;
}

char * sipster_request_print_header(SipsterSipHeader * header) {
    SIPSTER_DEBUG("PRINTING HEADER: %s", header->headerName);
    const SipsterSipHeaderMap hmap = header_prototypes[header->headerId];
    char * headerLine = hmap.print(header);
    sipster_sip_header_destroy(header);

    return headerLine;
}

SipsterSipHeader * sipster_response_parse_header(const char * input) {
    return sipster_request_parse_header(input);
}

char * sipster_response_print_header(SipsterSipHeader * header) {
    return sipster_request_print_header(header);
}

SipsterSipRequest *sipster_request_parse(const char *input, size_t size, int *result) {
    int res = -1;
    std::size_t pos = 0;
    SipsterSipHeaderContentLength * content_length;
    SipsterSipHeaderContentType * content_type;

    content_length = NULL;
    content_type = NULL;

    SipsterSipRequest * request = sipster_request_create();

    string line = nextToken(input, "\r\n", &pos);

    res = sipster_request_parse_line(line.c_str(), request);
    if(res != 0) {
        sipster_request_destroy(request);
        goto completed;
    }

    line = nextToken(input, "\r\n", &pos);
    SIPSTER_DEBUG(line.c_str());

    while(!line.empty()) {

        //TODO --> should validate data
        SipsterSipHeader * header = sipster_request_parse_header(line.c_str());

        //TODO --> should proceed only if data is valid

        switch(header->headerId) {
        case SIP_HEADER_CONTENT_LENGTH:
            content_length = (SipsterSipHeaderContentLength *) header;
            break;
        case SIP_HEADER_CONTENT_TYPE:
            content_type = (SipsterSipHeaderContentType *) header;
            break;
        default:
            break;
        }

        request->lastHeader = sipster_append_new_header(&request->firstHeader, header);
        line = nextToken(input, "\r\n", &pos);
    }

    if(content_length) {
        if(content_length->length > 0 && pos != std::string::npos && pos+content_length->length <= size) {
            //TODO read body
            SipsterSipContentBody * body = (SipsterSipContentBody *) calloc(1, sizeof(SipsterSipContentBody));
            body->length = content_length->length;
            if(content_type) {
                strncpy(body->contentType, content_type->contentType, strlen(content_type->contentType));
            }
            string d = getRest(input, &pos);
            strncpy(body->data, d.c_str(), body->length);

            request->content = body;
        }
    }


completed:
    *result = res;
    return request;
}
