//
// Created by dejan on 14.3.2016.
//

#include <sipster/sdp_headers.h>
#include <sdp_private.h>
#include <utils.h>


SipsterSdpHeader * sipster_sdp_header_create(size_t size) {
    return (SipsterSdpHeader *) sipster_allocator(size);
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
