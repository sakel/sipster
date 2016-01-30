#include <sipster/sip_headers.h>
#include <utils.h>
#include <string.h>
#include <sipster/log.h>
#include <cstdio>

SipsterSipHeader * init_basic_header(SipsterSipHeaderEnum id, size_t allocation_size) {
    SipsterSipHeader * header = (SipsterSipHeader *) sipster_sip_header_create(allocation_size);
    header->headerId = id;
    header->headerName = header_prototypes[id].headerName;
    header->shortName = header_prototypes[id].shortName;

    return header;
}

void no_destroy(SipsterSipHeader * header) {

}

SipsterSipHeader* no_parse(SipsterSipHeaderEnum id, const char * input) {
    return NULL;
}

char* no_print(SipsterSipHeader * header) {
    return NULL;
}

void param_destroy(SipsterSipHeaderWithParams * headerWithParams) {
    SipsterSipParameter * p = headerWithParams->first;
    while(p) {
        SipsterSipParameter * next = p->next;
        sipster_sip_parameter_destroy(p);
        p = next;
    }
}

void addr_destroy(SipsterSipHeader * header) {
    SIPSTER_DEBUG("Destroying %s", header->headerName);
    SipsterSipHeaderAddress * addr = (SipsterSipHeaderAddress *) header;
    param_destroy((SipsterSipHeaderWithParams *) addr);
}

SipsterSipHeader* addr_parse(SipsterSipHeaderEnum id, const char * input) {
    std::size_t pos = 0;
    std::size_t ppos = 0;
    SipsterSipHeaderAddress * to_header = (SipsterSipHeaderAddress *) init_basic_header(id, sizeof(SipsterSipHeaderAddress));
    string addressString = nextToken(input, ";", &pos);

    //TODO Should do a trim of the whitespaces
    strncpy(to_header->address, addressString.c_str(), addressString.length()+1);

    string param = nextToken(input, ";", &pos);
    if(!param.empty()) {
        string key = nextToken(param, "=", &ppos);
        string val = nextToken(param, "=", &ppos);
        SipsterSipParameter * prev = sipster_sip_parameter_create(key.c_str(), val.c_str());
        to_header->header.first = prev;

        param = nextToken(input, ";", &pos);
        while(!param.empty()) {
            ppos = 0;
            string key = nextToken(param, "=", &ppos);
            string val = nextToken(param, "=", &ppos);
            SipsterSipParameter * next = sipster_sip_parameter_create(key.c_str(), val.c_str());
            prev->next = next;
            prev = next;

            param = nextToken(input, ";", &pos);
        }
    }


    return (SipsterSipHeader *) to_header;
}

char* addr_print(SipsterSipHeader * header) {
    SipsterSipHeaderAddress * addr = (SipsterSipHeaderAddress *) header;
    char output[300];
    string params = "";
    const char * const format = "%s: %s%s";
    SipsterSipParameter * param = addr->header.first;
    while(param) {
        params = params + ";" + param->name + "=" + param->value;
        param = param->next;
    }

    sprintf(output, format, addr->header.header.headerName, addr->address, params.c_str());

    return strdup(output);
}


SipsterSipHeader* via_parse(SipsterSipHeaderEnum id, const char * input) {
    std::size_t pos = 0;
    std::size_t ppos = 0;
    SipsterSipHeaderVia* via_header = (SipsterSipHeaderVia*) init_basic_header(id, sizeof(SipsterSipHeaderVia));
    via_header->header.first = NULL;

    string versionString = nextToken(input, " ", &pos);
    strncpy(via_header->protocol, versionString.c_str(), versionString.length()+1);
    SIPSTER_DEBUG(versionString.c_str());

    string addressString = nextToken(input, ";", &pos);
    strncpy(via_header->address, addressString.c_str(), addressString.length()+1);
    SIPSTER_DEBUG(addressString.c_str());

    string param = nextToken(input, ";", &pos);
    if(!param.empty()) {
        string key = nextToken(param, "=", &ppos);
        string val = nextToken(param, "=", &ppos);
        SipsterSipParameter * prev = sipster_sip_parameter_create(key.c_str(), val.c_str());
        via_header->header.first = prev;

        param = nextToken(input, ";", &pos);
        while(!param.empty()) {
            ppos = 0;
            string key = nextToken(param, "=", &ppos);
            string val = nextToken(param, "=", &ppos);
            SipsterSipParameter * next = sipster_sip_parameter_create(key.c_str(), val.c_str());
            prev->next = next;
            prev = next;

            param = nextToken(input, ";", &pos);
        }
    }


    return (SipsterSipHeader *) via_header;
}

char* via_print(SipsterSipHeader * header) {
    SipsterSipHeaderVia * via = (SipsterSipHeaderVia *) header;
    char output[300];
    string params = "";
    const char * const format = "%s: %s %s%s";
    SipsterSipParameter * param = via->header.first;
    while(param) {
        params = params + ";" + param->name + "=" + param->value;
        param = param->next;
    }

    sprintf(output, format, via->header.header.headerName, via->protocol, via->address, params.c_str());

    return strdup(output);
}

void via_destroy(SipsterSipHeader * header) {
    param_destroy((SipsterSipHeaderWithParams*) header);
}

SipsterSipHeader* cseq_parse(SipsterSipHeaderEnum id, const char * input) {
    std::size_t pos = 0;
    SipsterSipHeaderCSeq * cseq_header = (SipsterSipHeaderCSeq *) init_basic_header(id, sizeof(SipsterSipHeaderCSeq));

    string seqString = nextToken(input, " ", &pos);
    cseq_header->seq = atol(seqString.c_str());

    string reqMethod = nextToken(input, " ", &pos);
    strncpy(cseq_header->requestMethod, reqMethod.c_str(), reqMethod.length() +1);
    return (SipsterSipHeader *) cseq_header;
}

char* cseq_print(SipsterSipHeader * header) {
    SipsterSipHeaderCSeq * cseq_header = (SipsterSipHeaderCSeq *) header;
    char output[60];
    const char * const format = "%s: %lu %s";
    sprintf(output, format, cseq_header->header.headerName, cseq_header->seq, cseq_header->requestMethod);

    return strdup(output);
}

SipsterSipHeader * sipster_sip_header_create(size_t size) {
    SipsterSipHeader * header = (SipsterSipHeader *) malloc(size);
    return header;
}

void sipster_sip_header_destroy(SipsterSipHeader * header) {
    if(header) {
        header_prototypes[header->headerId].destroy(header);
        free(header);
    }
}

SipsterSipParameter * sipster_sip_parameter_create(const char * key, const char * value) {
    SipsterSipParameter * param = (SipsterSipParameter *) calloc(1, sizeof(SipsterSipParameter));
    strncpy(param->name, key, strlen(key)+1);
    strncpy(param->value, value, strlen(value)+1);

    return param;
}

void sipster_sip_parameter_destroy(SipsterSipParameter * param) {
    if(param) {
        free(param);
    }
}
