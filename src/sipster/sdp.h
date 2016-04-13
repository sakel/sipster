#ifndef SDP_H
#define SDP_H

#include <sipster/sdp_enums.h>
#include <sipster/sdp_headers.h>

#define SDP_HEADER(x) (SipsterSdpHeader *) x

typedef struct _SipsterSdpMedia SipsterSdpMedia;
typedef struct _SipsterSdp SipsterSdp;

struct _SipsterSdpMedia {
    SipsterSdpHeaderMedia *mediaHeader;
    SipsterSdpHeaderSessionInfo *title;
    SipsterSdpHeaderConnection *connection;
    //TODO Bandwidth
    //TODO encryption key
    unsigned char attributeCount;
    SipsterSdpHeaderAttribute *attributes[20];
};

struct _SipsterSdp {
    SipsterSdpHeaderVersion *version;
    SipsterSdpHeaderOrigin *origin;
    SipsterSdpHeaderSessionName *sessionName;
    SipsterSdpHeaderSessionInfo *sessionInfo;
    SipsterSdpHeaderUri *uri;
    SipsterSdpHeaderEmail *email;
    SipsterSdpHeaderPhone *phone;
    SipsterSdpHeaderConnection *connection;
    //TODO bandwidth info
    //SipsterSdpHeaderBandwidth *bandwidth;
    //TODO There should be multiple TimeDesc combinations
    SipsterSdpHeaderTiming *timing;
    //TODO Time zone missing
    //TODO Encryption key
    unsigned char attributeCount;
    SipsterSdpHeaderAttribute *attributes[10];
    unsigned char mediaCount;
    SipsterSdpMedia media[5];
};

int sipster_sdp_parse(const char * sdps, SipsterSdp ** sdpp);
SipsterSdp * sipster_sdp_create();
void sipster_sdp_destroy(SipsterSdp * sdp);
char * sipster_sdp_print(SipsterSdp * sdp);

SipsterSdpHeader * sipster_sdp_parse_header(const char *input);
char * sipster_sdp_print_header(SipsterSdpHeader * header);
void sipster_sdp_destroy_header(SipsterSdpHeader * header);
SipsterSdpAttribute * sipster_sdp_parse_attribute(const char *input);
SipsterSdpAttribute * sipster_sdp_attribute_create(SipsterSdpAttributeEnum type, size_t size);

#endif // SDP_H
