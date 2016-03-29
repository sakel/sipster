#include "sipster/sip.h"
#include <sipster/log.h>
#include <string>
#include "utils.h"
#include "sip_headers_private.h"

using namespace std;

int sipster_sip_request_parse_line(const char *input, SipsterSipRequest *sipRequest) {
    string inputData;
    std::size_t pos = 0;
    int i;

    if(!input || !sipRequest) {
        SIPSTER_SIP_ERROR("NULL input parameters");
        return MESSAGE_ERROR;
    }

    inputData = input;

    string methodString = nextToken(inputData, " ", &pos);
    sipRequest->requestLine.method.methodId = SIP_METHOD_NONE;
    for(i = 0; i < NUM_SIP_METHODS; i++) {
        if(methodString == method_names[i]) {
            sipRequest->requestLine.method.method = method_names[i];
            sipRequest->requestLine.method.methodId = method_ids[i];
            break;
        }
    }
    if(sipRequest->requestLine.method.methodId == SIP_METHOD_NONE) {
        return MESSAGE_PARSE_ERROR;
    }

    string destinationString = nextToken(inputData, " ", &pos);
    string versionString = getRest(inputData, &pos);

    sipRequest->requestLine.requestUri = strdup(destinationString.c_str());
    sipRequest->requestLine.version = strdup(versionString.c_str());

    return OK;
}

SipsterSipRequest *sipster_sip_request_create() {
    SipsterSipRequest * request = (SipsterSipRequest *) sipster_allocator(sizeof(SipsterSipRequest));

    return request;
}

void sipster_sip_request_destroy(SipsterSipRequest *request) {
    if(request) {
        if(request->requestLine.requestUri) {
            free(request->requestLine.requestUri);
        }
        sipster_free(request);
    }
}

SipsterSipResponse *sipster_sip_response_create() {
    return (SipsterSipResponse *) sipster_allocator(sizeof(SipsterSipResponse));
}

void sipster_sip_response_destroy(SipsterSipResponse *response) {
    if(response) {
        sipster_free(response);
    }

}

int sipster_sip_response_parse_line(const char *input, SipsterSipResponse *sipResponse) {
    // Status-Line  =  SIP-Version SP Status-Code SP Reason-Phrase CRLF
    string inputData;
    std::size_t pos = 0;
    int i;

    if(!input || !sipResponse) {
        SIPSTER_SIP_ERROR("NULL input parameters");
        return MESSAGE_ERROR;
    }
    inputData = input;

    string versionString = nextToken(inputData, " ", &pos);
    string statusCodeString = nextToken(inputData, " ", &pos);
    string phraseString = getRest(inputData, &pos);

    sipResponse->responseLine.version = strdup(versionString.c_str());
    sipResponse->responseLine.status.statusCode = SIP_STATUS_NONE;
    for(i = 0; i < NUM_SIP_STATUSES; i++) {
        if(atoi(statusCodeString.c_str()) == statuses[i]) {
            sipResponse->responseLine.status.statusCode = statuses[i];
            sipResponse->responseLine.status.status = (SipsterSipStatusEnum)i;
            sipResponse->responseLine.status.reason = status_phrases[i];
            break;
        }
    }

    if(sipResponse->responseLine.status.statusCode == SIP_STATUS_NONE) {
        return MESSAGE_PARSE_ERROR;
    }

    return OK;
}

SipsterSipHeader *sipster_sip_request_parse_header(const char *input) {
    std::size_t pos = 0;
    int i = 0;
    string headerString = nextToken(input, ":", &pos);
    string rest = getRest(input, &pos);
    string cleanRest = ltrim(rest);

    for(i = 0; header_prototypes[i].headerId != SIP_HEADER_NONE; i++) {
        const SipsterSipHeaderMap hmap = header_prototypes[i];
        if(headerString.compare(hmap.headerName) == 0
                || headerString.compare(hmap.shortName) == 0) {
            SIPSTER_SIP_DEBUG("PARSING HEADER %s", hmap.headerName);
            SipsterSipHeader * header = header_prototypes[i].parse(header_prototypes[i].headerId, cleanRest.c_str());
            return header;
        }
    }
    return NULL;
}

char *sipster_sip_request_print_header(SipsterSipHeader *header) {
    SIPSTER_SIP_DEBUG("PRINTING HEADER: %s", header->headerName);
    const SipsterSipHeaderMap hmap = header_prototypes[header->headerId];
    char * headerLine = hmap.print(header);
    sipster_sip_header_destroy(header);

    return headerLine;
}

char *sipster_sip_request_print_line(SipsterSipRequestLine *line) {
    const char * const format = "%s %s %s";
    char * output = (char *) sipster_allocator(150);

    snprintf(output, 150, format, line->method.method, line->requestUri, line->version);

    return output;
}

char *sipster_sip_response_print_line(SipsterSipResponseLine *line) {
    const char * const format = "%s %d %s";
    char * output = (char *) sipster_allocator(150);

    snprintf(output, 150, format, line->version, line->status.statusCode, line->status.reason);

    return output;
}

SipsterSipHeader *sipster_sip_response_parse_header(const char *input) {
    return sipster_sip_request_parse_header(input);
}

char *sipster_sip_response_print_header(SipsterSipHeader *header) {
    return sipster_sip_request_print_header(header);
}

SipsterSipRequest *sipster_sip_request_parse(const char *input, size_t size, int *result) {
    int res = -1;
    std::size_t pos = 0;
    SipsterSipHeaderContentLength * content_length;
    SipsterSipHeaderContentType * content_type;

    content_length = NULL;
    content_type = NULL;

    SipsterSipRequest * request = sipster_sip_request_create();

    string line = nextToken(input, "\r\n", &pos);

    res = sipster_sip_request_parse_line(line.c_str(), request);
    if(res != 0) {
        sipster_sip_request_destroy(request);
        goto completed;
    }

    line = nextToken(input, "\r\n", &pos);
    SIPSTER_SIP_DEBUG(line.c_str());

    while(!line.empty()) {

        //TODO --> should validate data
        SipsterSipHeader * header = sipster_sip_request_parse_header(line.c_str());

        //TODO --> should proceed only if data is valid

        if(header) {
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
        }

        line = nextToken(input, "\r\n", &pos);
    }

    if(content_length) {
        if(content_length->number > 0 && pos != std::string::npos && pos+content_length->number <= size) {
            //TODO read body
            SipsterSipContentBody * body = (SipsterSipContentBody *) sipster_allocator(sizeof(SipsterSipContentBody));

            if(content_type) {
                strncpy(body->contentType, content_type->contentType, strlen(content_type->contentType));
            }
            string d = getRest(input, &pos);
            d = ltrim(d);
            body->length = content_length->number == d.length() ? content_length->number : d.length();
            strncpy(body->data, d.c_str(), d.length());
            request->content = body;
        }
    }


completed:
    *result = res;
    return request;
}

SipsterSipMessagePrint *sipster_sip_request_print(SipsterSipRequest *request) {
    char * output;
    int offset = 0;
    int len = 0;
    SipsterSipMessagePrint * print = (SipsterSipMessagePrint *) sipster_allocator(sizeof(SipsterSipMessagePrint));
    size_t allocationSize = 500;

    if(request->lastHeader) {
        allocationSize += ((request->lastHeader->index+1)*200);
    }

    if(request->content) {
        allocationSize += (request->content->length);
    }
    output = (char *) sipster_allocator(allocationSize);

    char * requestLine = (char *) sipster_sip_request_print_line(&request->requestLine);
    len = strlen(requestLine);
    strncpy(output+offset, requestLine, len);
    offset += len;
    sipster_free(requestLine);

    strncpy(output+offset, "\r\n", 2);
    offset += 2;

    SipsterSipHeaderLeaf * header = request->firstHeader;
    while(header) {
        char * headerLine = sipster_sip_request_print_header(header->header);
        header->header = NULL;
        SIPSTER_SIP_DEBUG("FOR HEADER printing: %s", headerLine);
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

SipsterSipResponse *sipster_sip_response_parse(const char *input, size_t size, int *result) {
    int res = -1;
    std::size_t pos = 0;
    SipsterSipHeaderContentLength * content_length;
    SipsterSipHeaderContentType * content_type;

    content_length = NULL;
    content_type = NULL;

    SipsterSipResponse * response = sipster_sip_response_create();

    string line = nextToken(input, "\r\n", &pos);

    res = sipster_sip_response_parse_line(line.c_str(), response);
    if(res != 0) {
        sipster_sip_response_destroy(response);
        goto completed;
    }

    line = nextToken(input, "\r\n", &pos);
    SIPSTER_SIP_DEBUG(line.c_str());

    while(!line.empty()) {

        //TODO --> should validate data
        SipsterSipHeader * header = sipster_sip_request_parse_header(line.c_str());

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
        if(content_length->number > 0 && pos != std::string::npos && pos+content_length->number <= size) {
            //TODO read body
            SipsterSipContentBody * body = (SipsterSipContentBody *) sipster_allocator(sizeof(SipsterSipContentBody));
            body->length = content_length->number;
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

SipsterSipMessagePrint *sipster_sip_response_print(SipsterSipResponse *response) {
    char * output;
    int offset = 0;
    int len = 0;
    SipsterSipMessagePrint * print = (SipsterSipMessagePrint *) sipster_allocator(sizeof(SipsterSipMessagePrint));
    size_t allocationSize = 500;

    if(response->lastHeader) {
        allocationSize += ((response->lastHeader->index+1)*200);
    }

    if(response->content) {
        allocationSize += (response->content->length);
    }
    output = (char *) sipster_allocator(allocationSize);

    char * responseLine = (char *) sipster_sip_response_print_line(&response->responseLine);
    len = strlen(responseLine);
    strncpy(output+offset, responseLine, len);
    offset += len;
    sipster_free(responseLine);

    strncpy(output+offset, "\r\n", 2);
    offset += 2;

    SipsterSipHeaderLeaf * header = response->firstHeader;
    while(header) {
        char * headerLine = sipster_sip_response_print_header(header->header);
        header->header = NULL;
        SIPSTER_SIP_DEBUG("FOR HEADER printing: %s", headerLine);
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

    SIPSTER_SIP_TRACE("RESPONSE: %s", output);

    return print;
}

int sipster_sip_request_reply(SipsterSipHandle *handle, SipsterSipCallLeg *leg, SipsterSipResponse *response) {
    SipsterSipMessagePrint * message = sipster_sip_response_print(response);
    int status = handle->sendResponse(handle, leg, response, message->output, message->size);

    // Maybe do some other magic...

    return status;
}

int sipster_sip_request_send(SipsterSipHandle *handle, SipsterSipCallLeg *leg, SipsterSipRequest *request) {

    SipsterSipMessagePrint * message = sipster_sip_request_print(request);
    int status = handle->sendRequest(handle, leg, request, message->output, message->size);

    // Maybe do some other magic...

    return status;
}

SipsterSipResponse *sipster_sip_request_create_response(SipsterSipHandle *sipsterSipHandle, SipsterSipRequest *request,
                                                        SipsterSipCallLeg *leg, SipsterSipStatusEnum status,
                                                        SipsterSipContentBody *body) {
    char rportNumber[6];
    SipsterSipParameter *toTag = NULL;
    SipsterSipParameter *rport = NULL;
    SipsterSipResponse * response = (SipsterSipResponse *) sipster_allocator(sizeof(SipsterSipResponse));
    response->responseLine.status.status = status;
    response->responseLine.status.statusCode = statuses[status];
    response->responseLine.status.reason = status_phrases[status];
    response->remoteAddr = request->remoteAddr;
    response->responseLine.version = SIP_PROTOCOL;
    response->content = body;
    //only 2xx responses have complete tags RFC3261 -> 13.2.2
    if(status <= SIP_STATUS_204_NO_NOTIFICATION && status >= SIP_STATUS_200_OK) {
        toTag = sipster_sip_parameter_create("tag", leg->toTag);
    }
    SipsterSipHeaderLeaf * current = request->firstHeader;
    SipsterSipHeaderWithParams * t = NULL;
    SipsterSipHeader *headerCopy = NULL;
    while(current) {
        headerCopy = sipster_sip_header_clone(current->header);
        switch(headerCopy->headerId) {
        case SIP_HEADER_CONTACT:
            goto skip_header;
        case SIP_HEADER_VIA:

            rport = sipster_sip_parameter_get("rport", SIP_HEADER_WITH_PARAMS_FIRST(headerCopy));
            if(rport) {
                //RFC 3581
                snprintf(rportNumber, sizeof(rportNumber), "%u", sipsterSipHandle->rport);
                rport = sipster_sip_parameter_create("rport", rportNumber);
                sipster_sip_header_append_parameter((SipsterSipHeaderWithParams *) headerCopy, rport);
            }
            t = (SipsterSipHeaderWithParams *) headerCopy;
            SIPSTER_SIP_DEBUG("%s", t->header.headerName);
            break;
        case SIP_HEADER_TO:
            if(toTag) {
                SIPSTER_SIP_DEBUG("TO TAG: %s=%s", toTag->name, toTag->value);
                sipster_sip_header_append_parameter((SipsterSipHeaderWithParams *) headerCopy, toTag);
            }
            break;
        default:
            break;
        }

        response->lastHeader = sipster_append_new_header(response->lastHeader, headerCopy);
        if(!response->firstHeader) {
            response->firstHeader = response->lastHeader;
        }
skip_header:
        current = current->next;
    }

    int len = 0;
    if(response->content) {
        current = sipster_get_header(SIP_HEADER_CONTENT_TYPE, response->firstHeader, response->lastHeader);
        SipsterSipHeaderContentType * ct = NULL;
        if(current) {
            ct = (SipsterSipHeaderContentType *) current->header;
        } else {
            ct = (SipsterSipHeaderContentType *) sipster_sip_header_create(sizeof(SipsterSipHeaderContentType));
            response->lastHeader = sipster_append_new_header(response->lastHeader, (SipsterSipHeader *) ct);
        }
        ct->header.header.headerId = SIP_HEADER_CONTENT_TYPE;
        ct->header.header.headerName = header_prototypes[ct->header.header.headerId].headerName;
        ct->header.header.shortName = header_prototypes[ct->header.header.headerId].shortName;
        strncpy(ct->contentType, response->content->contentType, sizeof(ct->contentType));
        len = response->content->length;
    }

    current = sipster_get_header(SIP_HEADER_CONTENT_LENGTH, response->firstHeader, response->lastHeader);
    SipsterSipHeaderContentLength * cl = NULL;
    if(current) {
        cl = (SipsterSipHeaderContentLength *) current->header;
    } else {
        cl = (SipsterSipHeaderContentLength *) sipster_sip_header_create(sizeof(SipsterSipHeaderContentLength));
        response->lastHeader = sipster_append_new_header(response->lastHeader, (SipsterSipHeader *) cl);
    }
    cl->header.headerId = SIP_HEADER_CONTENT_LENGTH;
    cl->header.headerName = header_prototypes[cl->header.headerId].headerName;
    cl->header.shortName = header_prototypes[cl->header.headerId].shortName;
    cl->number = len;

    return response;
}

SipsterSipRequest *sipster_sip_request_create_template(SipsterSipCallLeg *leg, SipsterSipMethodEnum method) {
    SipsterSipRequest *request = (SipsterSipRequest *) sipster_allocator(sizeof(SipsterSipRequest));

    request->requestLine.method.methodId = method;
    request->requestLine.method.method = method_names[method];
    request->requestLine.requestUri = sipster_address_uri_print(&leg->to);
    request->requestLine.version = SIP_PROTOCOL;




    return request;
}
