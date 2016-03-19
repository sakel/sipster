#ifndef SDP_H
#define SDP_H

#include <sipster/sdp_enums.h>
#include <sipster/sdp_headers.h>

typedef struct _SipsterSdp SipsterSdp;

struct _SipsterSdp {

};

int sipster_sdp_parse(const char * sdps, SipsterSdp ** sdp);
SipsterSdp * sipster_sdp_create();
void sipster_sdp_destroy(SipsterSdp * sdp);
SipsterSdpHeader * sipster_sdp_parse_header(const char *input);
char * sipster_sdp_print_header(SipsterSdpHeader * header);
SipsterSdpAttribute * sipster_sdp_parse_attribute(const char *input);

#endif // SDP_H
