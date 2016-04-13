//
// Created by dejan on 14.3.2016.
//

#include <sipster/sdp_headers.h>
#include <sdp_private.h>
#include <sipster/utils.h>


SipsterSdpHeader *sipster_sdp_header_create(SipsterSdpHeaderEnum headerType, size_t size) {
    SipsterSdpHeader * header = (SipsterSdpHeader *) sipster_allocator(size);
    header->headerType = headerType;
    header->header = sdp_header_prototypes[headerType].headerName[0];
    return header;
}

void sipster_sdp_header_destroy(SipsterSdpHeader * header) {
    if(header) {
        sdp_header_prototypes[header->headerType].destroy(header);
        sipster_free(header);
    }
}

SipsterSdpHeader * sipster_sdp_header_clone(SipsterSdpHeader *header) {
    if(!header)
        return NULL;
    SipsterSdpHeader * copy = sdp_header_prototypes[header->headerType].clone(header);
    return copy;
}
