#include <sipster/sip_headers.h>
#include <sipster/utils.h>
#include <string.h>
#include <sipster/log.h>
#include <regex>
#include <sip_headers_private.h>

#define SIP_ADDRESS_URI_FORMAT_NO_PORT "%s:%s@%s"
#define SIP_ADDRESS_URI_FORMAT_WITH_PORT "%s:%s@%s:%u"
#define SIP_ADDRESS_FORMAT_NO_PORT "%s%s<%s:%s@%s>"
#define SIP_ADDRESS_FORMAT_WITH_PORT "%s%s<%s:%s@%s:%u>"

SipsterSipHeader * sipster_init_basic_header(SipsterSipHeaderEnum id, size_t allocation_size) {
    SipsterSipHeader * header = (SipsterSipHeader *) sipster_sip_header_create(allocation_size);
    header->headerId = id;
    header->headerName = header_prototypes[id].headerName;
    header->shortName = header_prototypes[id].shortName;

    return header;
}

SipsterSipHeader * sipster_init_params_header(SipsterSipHeaderEnum id, size_t allocation_size) {
    SipsterSipHeaderWithParams * header = (SipsterSipHeaderWithParams *) sipster_sip_header_create(allocation_size);
    header->header.headerId = id;
    header->header.headerName = header_prototypes[id].headerName;
    header->header.shortName = header_prototypes[id].shortName;

    return (SipsterSipHeader *) header;
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

SipsterSipHeader *hclone(SipsterSipHeader *header) {
    SipsterSipHeader *cpy = (SipsterSipHeader *) sipster_allocator(sizeof(SipsterSipHeader));

    cpy->headerId = header->headerId;
    cpy->headerName = header->headerName;
    cpy->shortName = header->shortName;

    return cpy;
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
    SIPSTER_SIP_DEBUG("Destroying %s", header->headerName);
    SipsterSipHeaderAddress * addr = (SipsterSipHeaderAddress *) header;
    if(addr->parsedAddress) {
        sipster_free(addr->parsedAddress);
    }
    param_destroy((SipsterSipHeaderWithParams *) addr);
}

SipsterSipHeader* addr_parse(SipsterSipHeaderEnum id, const char * input) {
    std::size_t pos = 0;
    SipsterSipHeaderAddress * to_header = (SipsterSipHeaderAddress *) sipster_init_params_header(id, sizeof(SipsterSipHeaderAddress));
    string addressString = nextToken(input, ";", &pos);

    //TODO Should do a trim of the whitespaces
    strncpy(to_header->address, addressString.c_str(), addressString.length()+1);
    SipsterSipAddress *addr = sipster_address_parse(addressString.c_str());
    to_header->parsedAddress = addr;

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

SipsterSipHeader *addr_hclone(SipsterSipHeader *header) {
    SipsterSipHeaderAddress *cpy = (SipsterSipHeaderAddress *) sipster_allocator(sizeof(SipsterSipHeaderAddress));

    memcpy(cpy, header, sizeof(SipsterSipHeaderAddress));
    cpy->parsedAddress = sipster_address_clone(((SipsterSipHeaderAddress *) header)->parsedAddress);

    sipster_sip_parameters_clone(SIP_HEADER_WITH_PARAMS(header), SIP_HEADER_WITH_PARAMS(cpy));

    return (SipsterSipHeader *) cpy;
}

SipsterSipHeader* via_parse(SipsterSipHeaderEnum id, const char * input) {
    std::size_t pos = 0;
    SipsterSipHeaderVia* via_header = (SipsterSipHeaderVia*) sipster_init_params_header(id, sizeof(SipsterSipHeaderVia));
    via_header->header.first = NULL;

    string versionString = nextToken(input, " ", &pos);
    //strncpy(via_header->protocol, versionString.c_str(), versionString.length()+1);
    via_header->protocol = SIP_PROTOCOL_AND_TRANSPORT;
    SIPSTER_SIP_DEBUG(versionString.c_str());

    string addressString = nextToken(input, ";", &pos);
    strncpy(via_header->address, addressString.c_str(), sizeof(via_header->address)-1);
    SIPSTER_SIP_DEBUG(addressString.c_str());

    via_header->parsedAddress = sipster_base_inet_address_parse(addressString.c_str());

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

SipsterSipHeader *via_hclone(SipsterSipHeader *header) {
    SipsterSipHeaderVia *cpy = (SipsterSipHeaderVia *) sipster_allocator(sizeof(SipsterSipHeaderVia));

    memcpy(cpy, header, sizeof(SipsterSipHeaderVia));
    cpy->parsedAddress = sipster_base_inet_addres_clone(((SipsterSipHeaderVia *) header)->parsedAddress);
    sipster_sip_parameters_clone(SIP_HEADER_WITH_PARAMS(header), SIP_HEADER_WITH_PARAMS(cpy));

    return (SipsterSipHeader *) cpy;
}

SipsterSipHeader* cseq_parse(SipsterSipHeaderEnum id, const char * input) {
    int i = 0;
    std::size_t pos = 0;
    SipsterSipHeaderCSeq * cseq_header = (SipsterSipHeaderCSeq *) sipster_init_basic_header(id, sizeof(SipsterSipHeaderCSeq));
    SIPSTER_SIP_DEBUG(cseq_header->header.headerName);
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

SipsterSipHeader *cseq_hclone(SipsterSipHeader *header) {
    SipsterSipHeaderCSeq *cpy = (SipsterSipHeaderCSeq *) sipster_allocator(sizeof(SipsterSipHeaderCSeq));

    memcpy(cpy, header, sizeof(SipsterSipHeaderCSeq));

    return (SipsterSipHeader *) cpy;
}

SipsterSipHeader* ci_parse(SipsterSipHeaderEnum id, const char * input) {
    std::size_t pos = 0;
    SipsterSipHeaderCallID * ci_header = (SipsterSipHeaderCallID *) sipster_init_basic_header(id, sizeof(SipsterSipHeaderCallID));

    SIPSTER_SIP_DEBUG(ci_header->header.headerName);

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

SipsterSipHeader *ci_hclone(SipsterSipHeader *header) {
    SipsterSipHeaderCallID *cpy = (SipsterSipHeaderCallID *) sipster_allocator(sizeof(SipsterSipHeaderCallID));

    memcpy(cpy, header, sizeof(SipsterSipHeaderCallID));

    return (SipsterSipHeader *) cpy;
}

SipsterSipHeader* ct_parse(SipsterSipHeaderEnum id, const char * input) {
    std::size_t pos = 0;

    SipsterSipHeaderContentType * ct_header = (SipsterSipHeaderContentType *) sipster_init_basic_header(id, sizeof(SipsterSipHeaderContentType));

    SIPSTER_SIP_DEBUG(ct_header->header.header.headerName);

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

SipsterSipHeader *ct_hclone(SipsterSipHeader *header) {

    if(!header)
        return NULL;

    SipsterSipHeaderContentType *cpy = (SipsterSipHeaderContentType *) sipster_allocator(sizeof(SipsterSipHeaderContentType));

    memcpy(cpy, header, sizeof(SipsterSipHeaderContentType));

    sipster_sip_parameters_clone(SIP_HEADER_WITH_PARAMS(header), SIP_HEADER_WITH_PARAMS(cpy));

    return (SipsterSipHeader *) cpy;
}

SipsterSipHeader* string_parse(SipsterSipHeaderEnum id, const char * input) {
    std::size_t pos = 0;
    SipsterSipHeaderString * str_header = (SipsterSipHeaderString *) sipster_init_basic_header(id, sizeof(SipsterSipHeaderString));

    SIPSTER_SIP_DEBUG(str_header->header.headerName);

    string dataString = getRest(input, &pos);
    strncpy(str_header->data, dataString.c_str(), sizeof(str_header->data)-1);

    return (SipsterSipHeader *) str_header;
}

char* string_print(SipsterSipHeader * header) {
    SipsterSipHeaderString * str_header = (SipsterSipHeaderString *) header;
    char * output;
    const char * const format = "%s: %s";

    output = (char *) sipster_allocator(60);
    snprintf(output, 60, format, str_header->header.headerName, str_header->data);

    return output;
}

SipsterSipHeader *string_hclone(SipsterSipHeader *header) {
    SipsterSipHeaderString *cpy = (SipsterSipHeaderString *) sipster_allocator(sizeof(SipsterSipHeaderString));

    memcpy(cpy, header, sizeof(SipsterSipHeaderString));

    return (SipsterSipHeader *) cpy;
}

SipsterSipHeader* uint_parse(SipsterSipHeaderEnum id, const char * input) {
    std::size_t pos = 0;
    SipsterSipHeaderInteger * num_header = (SipsterSipHeaderInteger *) sipster_init_basic_header(id, sizeof(SipsterSipHeaderInteger));

    SIPSTER_SIP_DEBUG(num_header->header.headerName);

    string numString = nextToken(input, " ", &pos);
    num_header->number = atoi(numString.c_str());

    return (SipsterSipHeader *) num_header;
}

char* uint_print(SipsterSipHeader * header) {
    SipsterSipHeaderInteger * num_header = (SipsterSipHeaderInteger *) header;
    char * output;
    const char * const format = "%s: %u";

    output = (char *) sipster_allocator(60);
    snprintf(output, 60, format, num_header->header.headerName, num_header->number);

    return output;
}

SipsterSipHeader *uint_hclone(SipsterSipHeader *header) {
    SipsterSipHeaderInteger *cpy = (SipsterSipHeaderInteger *) sipster_allocator(sizeof(SipsterSipHeaderInteger));

    memcpy(cpy, header, sizeof(SipsterSipHeaderInteger));

    return (SipsterSipHeader *) cpy;
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
    strncpy(param->name, key, sizeof(param->name)-1);
    strncpy(param->value, value, sizeof(param->value)-1);

    return param;
}

SipsterSipParameter * sipster_sip_parameter_get(const char *name, SipsterSipParameter *first) {
    if(!first)
        return NULL;

    SipsterSipParameter *current = first;
    while(current) {
        if(strncmp(current->name, name, strlen(name)) == 0) {
            return current;
        }
        current = current->next;
    }

    return NULL;
}

void sipster_sip_parameter_destroy(SipsterSipParameter * param) {
    if(param) {
        sipster_free(param);
    }
}

SipsterSipHeaderLeaf *sipster_sip_append_new_header(SipsterSipHeaderLeaf *leaf, SipsterSipHeader *header) {
    SipsterSipHeaderLeaf * new_leaf = (SipsterSipHeaderLeaf *) sipster_allocator(sizeof(SipsterSipHeaderLeaf));
    new_leaf->header = header;

    if(!leaf) {
        new_leaf->metadata = (SipsterSipHeaderLeafMetadata *) sipster_allocator(sizeof(SipsterSipHeaderLeafMetadata));
        new_leaf->metadata->count++;
    }

    if(leaf) {
        new_leaf->metadata = leaf->metadata;
        leaf->metadata->count++;
        leaf->next = new_leaf;
        new_leaf->index = leaf->index + 1;
    }

    return new_leaf;
}

void sipster_sip_header_append_destroy(SipsterSipHeaderLeaf *leaf, int destroyHeader = 0) {
    if(leaf && leaf->metadata) {
        free(leaf->metadata);
    }
    while(leaf) {
        SipsterSipHeaderLeaf * next_leaf = leaf->next;
        if(destroyHeader) {
            sipster_sip_header_destroy(leaf->header);
        }
        sipster_free(leaf);
        leaf = next_leaf;
    }
}

SipsterSipHeaderLeaf *sipster_sip_get_header(SipsterSipHeaderEnum headerId, SipsterSipHeaderLeaf *first,
                                             SipsterSipHeaderLeaf *last) {
    SipsterSipHeaderLeaf *head = NULL;
    SipsterSipHeaderLeaf *tail = NULL;
    SipsterSipHeaderLeaf *current = first;
    while(current != NULL) {
        if(current->header && current->header->headerId == headerId) {
            SipsterSipHeaderLeaf * new_leaf = (SipsterSipHeaderLeaf *) sipster_allocator(sizeof(SipsterSipHeaderLeaf));
            new_leaf->header = current->header;
            if(!head) {
                head = new_leaf;
            } else {
                tail->index = head->index+1;
                tail->next = new_leaf;
            }
            if(!tail) {
                head->metadata = (SipsterSipHeaderLeafMetadata *) sipster_allocator(sizeof(SipsterSipHeaderLeafMetadata));
            }
            tail = new_leaf;
            tail->metadata = head->metadata;

            tail->metadata->count++;
        }

        current = current->next;
    }

    return head;
}

SipsterSipAddress * sipster_address_parse(const char * address) {
    SipsterSipAddress * addr = (SipsterSipAddress *) sipster_allocator(sizeof(SipsterSipAddress));

    //TODO -

    int status = sipster_address_parse_inPlace(address, addr);
    if(status != SIPSTER_RETURN_OK) {
        sipster_free(addr);
        return NULL;
    }
    return addr;
}

int sipster_address_parse_inPlace(const char * address, SipsterSipAddress * addrObject) {
    size_t pos = 0;

    if(!addrObject) {
        return -1;
    }
    string addressString = address;

    string uri = "";
    string name = "";
    if(std::regex_match (addressString, std::regex("([\"A-Za-z0-9 -+,]*[ ])?[<]([a-zA-Z]{3,5}):([a-zA-Z0-9.-_+]*)@([a-zA-Z0-9.-_+]*)[.]([a-zA-Z0-9.-_+]*)[>]"))) {
        name = nextToken(addressString, "<", &pos);
        name = trim(name);
        uri = nextToken(addressString, ">", &pos);
        uri = trim(uri);
    } else if(std::regex_match (addressString, std::regex("([a-zA-Z]{3,5}):([a-zA-Z0-9.-_+]*)@([a-zA-Z0-9.-_+]*)[.]([a-zA-Z0-9.-_+]*)"))) {
        uri = addressString;
    } else {
        SIPSTER_SIP_WARN("Invalid address format: ", address);
        return -1;
    }

    strncpy(addrObject->name, name.c_str(), name.length());

    pos = 0;
    string schema = nextToken(uri, ":", &pos);
    string user = nextToken(uri, "@", &pos);
    string domain = nextToken(uri, ":", &pos);
    string port = nextToken(uri, ";", &pos);

    strncpy(addrObject->user, user.c_str(), sizeof(addrObject->user));
    strncpy(addrObject->schema, schema.c_str(), sizeof(addrObject->schema));
    strncpy(addrObject->domain, domain.c_str(), sizeof(addrObject->domain));
    if(!port.empty()) {
        unsigned short portNumber = (unsigned short) strtoul(port.c_str(), NULL, 0);
        addrObject->port = portNumber;
    }

    return SIPSTER_RETURN_OK;
}

void sipster_sip_header_append_parameter(SipsterSipHeaderWithParams *header, SipsterSipParameter *param) {
    SipsterSipParameter * p = header->first;
    if(!p) {
        header->first = param;
    } else {
        if(strcmp(header->first->name, param->name) == 0) {
            SipsterSipParameter *tmpp = header->first;
            header->first = param;
            header->first->next = tmpp->next;
            sipster_sip_parameter_destroy(tmpp);
            return;
        }
        while(p) {
            if(p->next) {
                if(strcmp(p->next->name, param->name) == 0) {
                    SipsterSipParameter *tmpp = p->next;
                    p->next = param;
                    p->next->next = tmpp->next;
                    sipster_sip_parameter_destroy(tmpp);
                    break;
                }
                p = p->next;
            } else {
                p->next = param;
                break;
            }
        }
    }
}

char *sipster_address_uri_print(SipsterSipAddress *addr)
{
    if(!addr) {
        return NULL;
    }

    //addr->port < 1 ? SIP_ADDRESS_FORMAT_NO_PORT : SIP_ADDRESS_FORMAT_WITH_PORT
    char *uri = (char *) sipster_allocator(300);
    if(addr->port < 1) {
        snprintf(uri, 300, SIP_ADDRESS_URI_FORMAT_NO_PORT, addr->schema, addr->user, addr->domain);
    } else {
        snprintf(uri, 300, SIP_ADDRESS_URI_FORMAT_WITH_PORT, addr->schema, addr->user, addr->domain, addr->port);
    }

    return uri;
}

char *sipster_address_print(SipsterSipAddress *addr)
{
    if(!addr) {
        return NULL;
    }

    //addr->port < 1 ? SIP_ADDRESS_FORMAT_NO_PORT : SIP_ADDRESS_FORMAT_WITH_PORT
    char *uri = (char *) sipster_allocator(300);
    if(addr->port < 1) {
        snprintf(uri, 300, SIP_ADDRESS_FORMAT_NO_PORT, addr->name, addr->name && strlen(addr->name) > 0 ? " " : "", addr->schema, addr->user, addr->domain);
    } else {
        snprintf(uri, 300, SIP_ADDRESS_FORMAT_WITH_PORT, addr->name, addr->name && strlen(addr->name) > 0 ? " " : "", addr->schema, addr->user, addr->domain, addr->port);
    }

    return uri;
}

SipsterSipAddress * sipster_address_clone(SipsterSipAddress *addr) {
    if(!addr)
        return NULL;
    SipsterSipAddress * addr2 = (SipsterSipAddress *) sipster_allocator(sizeof(SipsterSipAddress));
    memcpy(addr2, addr, sizeof(SipsterSipAddress));
    return addr2;
}

SipsterSipHeader * sipster_sip_header_clone(SipsterSipHeader *header) {
    if(!header)
        return NULL;
    SipsterSipHeader * copy = header_prototypes[header->headerId].clone(header);
    return copy;
}

SipsterSipParameter * sipster_sip_parameter_clone(SipsterSipParameter *param) {
    if(!param)
        return NULL;
    SipsterSipParameter * param2 = (SipsterSipParameter *) sipster_allocator(sizeof(SipsterSipParameter));
    memcpy(param2, param, sizeof(SipsterSipParameter));
    return param2;
}

void sipster_sip_parameters_clone(SipsterSipHeaderWithParams *header, SipsterSipHeaderWithParams *cpy) {
    SipsterSipHeaderWithParams *oldHeader = (SipsterSipHeaderWithParams *) header;
    SipsterSipHeaderWithParams *newHeader = (SipsterSipHeaderWithParams *) cpy;
    if(oldHeader->first) {
        SipsterSipParameter *newFirst = NULL;
        SipsterSipParameter *newLast = NULL;
        SipsterSipParameter *current;

        current = oldHeader->first;

        while(current) {
            if(newLast) {
                newLast->next = sipster_sip_parameter_clone(current);
            } else {
                newLast = sipster_sip_parameter_clone(current);
                goto skip;
            }
            newLast = newLast->next;
            skip:
            if(!newFirst) {
                newFirst = newLast;
            }
            current = current->next;
        }

        newHeader->first = newFirst;
    }
}
