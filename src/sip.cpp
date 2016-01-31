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
    return (SipsterSipRequest *) sipster_allocator(sizeof(SipsterSipRequest));
}

void sipster_request_destroy(SipsterSipRequest *request) {
    if(request) {
        if(request->requestLine.requestUri) {
            free(request->requestLine.requestUri);
        }
        if(request->requestLine.version) {
            free(request->requestLine.version);
        }
        sipster_free(request);
    }
}

SipsterSipResponse *sipster_response_create() {
    return (SipsterSipResponse *) sipster_allocator(sizeof(SipsterSipResponse));
}

void sipster_response_destroy(SipsterSipResponse *response) {
    if(response) {
        if(response->responseLine.version) {
            free(response->responseLine.version);
        }
        sipster_free(response);
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

char * sipster_request_print_line(SipsterSipRequestLine * line) {
    const char * const format = "%s %s %s";
    char * output = (char *) sipster_allocator(150);

    snprintf(output, 150, format, line->method.method, line->requestUri, line->version);

    return output;
}

char * sipster_response_print_line(SipsterSipResponseLine * line) {
    const char * const format = "%s %d %s";
    char * output = (char *) sipster_allocator(150);

    snprintf(output, 150, format, line->version, line->status.statusCode, line->status.reason);

    return output;
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

        request->lastHeader = sipster_append_new_header(request->lastHeader, header);
        if(!request->firstHeader) {
            request->firstHeader = request->lastHeader;
        }
        line = nextToken(input, "\r\n", &pos);
    }

    if(content_length) {
        if(content_length->length > 0 && pos != std::string::npos && pos+content_length->length <= size) {
            //TODO read body
            SipsterSipContentBody * body = (SipsterSipContentBody *) sipster_allocator(sizeof(SipsterSipContentBody));

            if(content_type) {
                strncpy(body->contentType, content_type->contentType, strlen(content_type->contentType));
            }
            string d = getRest(input, &pos);
            d = ltrim(d);
            body->length = content_length->length == d.length() ? content_length->length : d.length();
            strncpy(body->data, d.c_str(), d.length());
            request->content = body;
        }
    }


completed:
    *result = res;
    return request;
}

SipsterSipMessagePrint * sipster_request_print(SipsterSipRequest * request) {
    char * output;
    int offset = 0;
    int len = 0;
    SipsterSipMessagePrint * print = (SipsterSipMessagePrint *) sipster_allocator(sizeof(SipsterSipMessagePrint));
    size_t allocationSize = 500;

    if(request->lastHeader) {
        allocationSize += ((request->lastHeader->count+1)*200);
    }

    if(request->content) {
        allocationSize += (request->content->length);
    }
    output = (char *) sipster_allocator(allocationSize);

    char * requestLine = (char *) sipster_request_print_line(&request->requestLine);
    len = strlen(requestLine);
    strncpy(output+offset, requestLine, len);
    offset += len;
    sipster_free(requestLine);

    strncpy(output+offset, "\r\n", 2);
    offset += 2;

    SipsterSipHeaderLeaf * header = request->firstHeader;
    while(header) {
        char * headerLine = sipster_request_print_header(header->header);
        header->header = NULL;
        SIPSTER_DEBUG("FOR HEADER printing: %s", headerLine);
        len = strlen(headerLine);
        strncpy(output+offset, headerLine, len);
        offset += len;

        strncpy(output+offset, "\r\n", 2);
        offset += 2;

        header = header->next;
        sipster_free(headerLine);
    }
    strncpy(output+offset, "\r\n", 2);
    offset += 2;

    if(request->content) {
        memcpy(output+offset, request->content->data, request->content->length);
        offset += request->content->length;
    }

    output[offset] = '\0';

    print->output = output;
    print->size = offset;

    return print;
}

SipsterSipResponse *sipster_response_parse(const char *input, size_t size, int *result) {
    int res = -1;
    std::size_t pos = 0;
    SipsterSipHeaderContentLength * content_length;
    SipsterSipHeaderContentType * content_type;

    content_length = NULL;
    content_type = NULL;

    SipsterSipResponse * response = sipster_response_create();

    string line = nextToken(input, "\r\n", &pos);

    res = sipster_response_parse_line(line.c_str(), response);
    if(res != 0) {
        sipster_response_destroy(response);
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

        response->lastHeader = sipster_append_new_header(response->lastHeader, header);
        if(!response->firstHeader) {
            response->firstHeader = response->lastHeader;
        }
        line = nextToken(input, "\r\n", &pos);
    }

    if(content_length) {
        if(content_length->length > 0 && pos != std::string::npos && pos+content_length->length <= size) {
            //TODO read body
            SipsterSipContentBody * body = (SipsterSipContentBody *) sipster_allocator(sizeof(SipsterSipContentBody));
            body->length = content_length->length;
            if(content_type) {
                strncpy(body->contentType, content_type->contentType, strlen(content_type->contentType));
            }
            string d = getRest(input, &pos);
            d = ltrim(d);
            strncpy(body->data, d.c_str(), body->length);

            response->content = body;
        }
    }


completed:
    *result = res;
    return response;
}

SipsterSipMessagePrint * sipster_response_print(SipsterSipResponse * response) {
    char * output;
    int offset = 0;
    int len = 0;
    SipsterSipMessagePrint * print = (SipsterSipMessagePrint *) sipster_allocator(sizeof(SipsterSipMessagePrint));
    size_t allocationSize = 500;

    if(response->lastHeader) {
        allocationSize += ((response->lastHeader->count+1)*200);
    }

    if(response->content) {
        allocationSize += (response->content->length);
    }
    output = (char *) sipster_allocator(allocationSize);

    char * responseLine = (char *) sipster_response_print_line(&response->responseLine);
    len = strlen(responseLine);
    strncpy(output+offset, responseLine, len);
    offset += len;
    sipster_free(responseLine);

    strncpy(output+offset, "\r\n", 2);
    offset += 2;

    SipsterSipHeaderLeaf * header = response->firstHeader;
    while(header) {
        char * headerLine = sipster_request_print_header(header->header);
        header->header = NULL;
        SIPSTER_DEBUG("FOR HEADER printing: %s", headerLine);
        len = strlen(headerLine);
        strncpy(output+offset, headerLine, len);
        offset += len;

        strncpy(output+offset, "\r\n", 2);
        offset += 2;

        header = header->next;
        sipster_free(headerLine);
    }
    strncpy(output+offset, "\r\n", 2);
    offset += 2;

    if(response->content) {
        memcpy(output+offset, response->content->data, response->content->length);
        offset += response->content->length;
    }

    output[offset] = '\0';

    print->output = output;
    print->size = offset;

    return print;
}
