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

SipsterSipParameter * parse_header_params(const char * input, std::size_t *pos) {
    std::size_t ppos = 0;
    SipsterSipParameter* first;

    first = NULL;

    string param = nextToken(input, ";", pos);
    if(!param.empty()) {
        string key = nextToken(param, "=", &ppos);
        string val = nextToken(param, "=", &ppos);
        SipsterSipParameter * prev = sipster_sip_parameter_create(key.c_str(), val.c_str());
        first = prev;

        param = nextToken(input, ";", pos);
        while(!param.empty()) {
            ppos = 0;
            string key = nextToken(param, "=", &ppos);
            string val = nextToken(param, "=", &ppos);
            SipsterSipParameter * next = sipster_sip_parameter_create(key.c_str(), val.c_str());
            prev->next = next;
            prev = next;

            param = nextToken(input, ";", pos);
        }
    }

    return first;
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
    SipsterSipHeaderAddress * to_header = (SipsterSipHeaderAddress *) init_basic_header(id, sizeof(SipsterSipHeaderAddress));
    string addressString = nextToken(input, ";", &pos);

    //TODO Should do a trim of the whitespaces
    strncpy(to_header->address, addressString.c_str(), addressString.length()+1);

    SipsterSipParameter * params = parse_header_params(input, &pos);
    to_header->header.first = params;

    return (SipsterSipHeader *) to_header;
}

char* addr_print(SipsterSipHeader * header) {
    SipsterSipHeaderAddress * addr = (SipsterSipHeaderAddress *) header;
    char * output;
    string params = "";
    const char * const format = "%s: %s%s";
    SipsterSipParameter * param = addr->header.first;
    while(param) {
        params = params + ";" + param->name + "=" + param->value;
        param = param->next;
    }

    output = (char *) sipster_allocator(300);

    snprintf(output, 300, format, addr->header.header.headerName, addr->address, params.c_str());

    return output;
}


SipsterSipHeader* via_parse(SipsterSipHeaderEnum id, const char * input) {
    std::size_t pos = 0;
    SipsterSipHeaderVia* via_header = (SipsterSipHeaderVia*) init_basic_header(id, sizeof(SipsterSipHeaderVia));
    via_header->header.first = NULL;

    string versionString = nextToken(input, " ", &pos);
    strncpy(via_header->protocol, versionString.c_str(), versionString.length()+1);
    SIPSTER_DEBUG(versionString.c_str());

    string addressString = nextToken(input, ";", &pos);
    strncpy(via_header->address, addressString.c_str(), addressString.length()+1);
    SIPSTER_DEBUG(addressString.c_str());

    SipsterSipParameter * params = parse_header_params(input, &pos);
    via_header->header.first = params;

    return (SipsterSipHeader *) via_header;
}

char* via_print(SipsterSipHeader * header) {
    SipsterSipHeaderVia * via = (SipsterSipHeaderVia *) header;
    char *output;
    string params = "";
    const char * const format = "%s: %s %s%s";
    SipsterSipParameter * param = via->header.first;
    while(param) {
        params = params + ";" + param->name + "=" + param->value;
        param = param->next;
    }

    output = (char *) sipster_allocator(300);

    snprintf(output, 300, format, via->header.header.headerName, via->protocol, via->address, params.c_str());

    return output;
}

void via_destroy(SipsterSipHeader * header) {
    param_destroy((SipsterSipHeaderWithParams*) header);
}

SipsterSipHeader* cseq_parse(SipsterSipHeaderEnum id, const char * input) {
    int i = 0;
    std::size_t pos = 0;
    SipsterSipHeaderCSeq * cseq_header = (SipsterSipHeaderCSeq *) init_basic_header(id, sizeof(SipsterSipHeaderCSeq));
    SIPSTER_DEBUG(cseq_header->header.headerName);
    string seqString = nextToken(input, " ", &pos);
    cseq_header->seq = atol(seqString.c_str());

    string reqMethod = nextToken(input, " ", &pos);
    //strncpy(cseq_header->requestMethod, reqMethod.c_str(), reqMethod.length() +1);

    for(i = 0; i < NUM_SIP_METHODS; i++) {
        if(reqMethod == method_names[i]) {
            cseq_header->requestMethod = method_names[i];
            cseq_header->methodId = method_ids[i];
            break;
        }
    }

    return (SipsterSipHeader *) cseq_header;
}

char* cseq_print(SipsterSipHeader * header) {
    SipsterSipHeaderCSeq * cseq_header = (SipsterSipHeaderCSeq *) header;
    char *output;
    const char * const format = "%s: %lu %s";

    output = (char *) sipster_allocator(60);
    snprintf(output, 60, format, cseq_header->header.headerName, cseq_header->seq, cseq_header->requestMethod);

    return output;
}

SipsterSipHeader* cl_parse(SipsterSipHeaderEnum id, const char * input) {
    std::size_t pos = 0;
    SipsterSipHeaderContentLength * cl_header = (SipsterSipHeaderContentLength *) init_basic_header(id, sizeof(SipsterSipHeaderContentLength));

    SIPSTER_DEBUG(cl_header->header.headerName);

    string lenString = nextToken(input, " ", &pos);
    cl_header->length = atoi(lenString.c_str());

    return (SipsterSipHeader *) cl_header;
}

char* cl_print(SipsterSipHeader * header) {
    SipsterSipHeaderContentLength * cl_header = (SipsterSipHeaderContentLength *) header;
    char * output;
    const char * const format = "%s: %u";

    output = (char *) sipster_allocator(60);
    snprintf(output, 60, format, cl_header->header.headerName, cl_header->length);

    return output;
}

SipsterSipHeader* ci_parse(SipsterSipHeaderEnum id, const char * input) {
    std::size_t pos = 0;
    SipsterSipHeaderCallID * ci_header = (SipsterSipHeaderCallID *) init_basic_header(id, sizeof(SipsterSipHeaderCallID));

    SIPSTER_DEBUG(ci_header->header.headerName);

    string dataString = getRest(input, &pos);
    strncpy(ci_header->data, dataString.c_str(), dataString.length()+1);

    return (SipsterSipHeader *) ci_header;
}

char* ci_print(SipsterSipHeader * header) {
    SipsterSipHeaderCallID * ci_header = (SipsterSipHeaderCallID *) header;
    char * output;
    const char * const format = "%s: %s";

    output = (char *) sipster_allocator(60);
    snprintf(output, 60, format, ci_header->header.headerName, ci_header->data);

    return output;
}


SipsterSipHeader* ct_parse(SipsterSipHeaderEnum id, const char * input) {
    std::size_t pos = 0;

    SipsterSipHeaderContentType * ct_header = (SipsterSipHeaderContentType *) init_basic_header(id, sizeof(SipsterSipHeaderContentType));

    SIPSTER_DEBUG(ct_header->header.header.headerName);

    string ct = nextToken(input, ";", &pos);
    strncpy(ct_header->contentType, ct.c_str(), ct.length()+1);

    SipsterSipParameter * params = parse_header_params(input, &pos);
    ct_header->header.first = params;

    return (SipsterSipHeader *) ct_header;
}

char* ct_print(SipsterSipHeader * header) {
    SipsterSipHeaderContentType * ct = (SipsterSipHeaderContentType *) header;
    char *output;
    string params = "";
    const char * const format = "%s: %s%s";
    SipsterSipParameter * param = ct->header.first;
    while(param) {
        params = params + ";" + param->name + "=" + param->value;
        param = param->next;
    }
    output = (char *) sipster_allocator(300);
    snprintf(output, 300, format, ct->header.header.headerName, ct->contentType, params.c_str());

    return strdup(output);
}

void ct_destroy(SipsterSipHeader * header) {
    param_destroy((SipsterSipHeaderWithParams*) header);
}

SipsterSipHeader * sipster_sip_header_create(size_t size) {
    SipsterSipHeader * header = (SipsterSipHeader *) sipster_allocator(size);
    return header;
}

void sipster_sip_header_destroy(SipsterSipHeader * header) {
    if(header) {
        header_prototypes[header->headerId].destroy(header);
        sipster_free(header);
    }
}

SipsterSipParameter * sipster_sip_parameter_create(const char * key, const char * value) {
    SipsterSipParameter * param = (SipsterSipParameter *) sipster_allocator(sizeof(SipsterSipParameter));
    strncpy(param->name, key, strlen(key)+1);
    strncpy(param->value, value, strlen(value)+1);

    return param;
}

void sipster_sip_parameter_destroy(SipsterSipParameter * param) {
    if(param) {
        sipster_free(param);
    }
}

SipsterSipHeaderLeaf * sipster_append_new_header(SipsterSipHeaderLeaf *leaf, SipsterSipHeader * header) {
    SipsterSipHeaderLeaf * new_leaf = (SipsterSipHeaderLeaf *) sipster_allocator(sizeof(SipsterSipHeaderLeaf));
    new_leaf->header = header;

    if(leaf) {
        leaf->next = new_leaf;
        new_leaf->count = leaf->count + 1;
    }

    return new_leaf;
}

void sipster_append_destroy(SipsterSipHeaderLeaf * leaf) {
    while(leaf) {
        SipsterSipHeaderLeaf * next_leaf = leaf->next;
        sipster_sip_header_destroy(leaf->header);
        sipster_free(leaf);
        leaf = next_leaf;
    }
}
