#include "sipster/sdp.h"
#include "sdp_private.h"
#include <utils.h>
#include <sipster/sdp_enums.h>
#include <sipster/sdp_headers.h>
#include <sipster/log.h>
#include <string.h>
#include <sipster/sip_headers.h>

SipsterSdpAttribute *cloneAttribute(const SipsterSdpAttribute *attr, size_t size);
SipsterSdpMediaTypeEnum sipster_sdp_media_type_decode(const char * input);

SipsterSdpHeader * sipster_init_basic_header(SipsterSdpHeaderEnum id, size_t allocation_size) {
    SipsterSdpHeader * header = (SipsterSdpHeader *) sipster_sdp_header_create(allocation_size);
    header->headerType = id;
    header->header = sdp_header_prototypes[id].headerName[0];

    return header;
}

int sipster_sdp_parse(const char * sdps, SipsterSdp ** sdp) {
    return 0;
}

SipsterSdp * sipster_sdp_create() {
    SipsterSdp * sdp;
    sdp = (SipsterSdp *) sipster_allocator(sizeof(SipsterSdp));
    return sdp;
}

void sipster_sdp_destroy(SipsterSdp * sdp) {
    if(sdp) {
        //TODO implement other lower level cleanups
        sipster_free(sdp);
    }
}

void sdp_no_destroy(SipsterSdpHeader * header) {

}

SipsterSdpHeader* sdp_no_clone(SipsterSdpHeader * header) {
    SipsterSdpHeader * cpy = (SipsterSdpHeader *) sipster_allocator(sizeof(SipsterSdpHeader));
    cpy->header = header->header;
    cpy->headerType = header->headerType;
    return cpy;
}

SipsterSdpHeader* sdp_no_parse(SipsterSdpHeaderEnum headerType, const char * input) {
    return NULL;
}

char* sdp_no_print(SipsterSdpHeader * header) {
    return NULL;
}

SipsterSdpHeader* sdp_version_parse(SipsterSdpHeaderEnum headerType, const char * data) {
    SipsterSdpHeaderVersion * header = (SipsterSdpHeaderVersion *) sipster_init_basic_header(headerType, sizeof(SipsterSdpHeaderVersion));

    header->version = atoi(data);

    return (SipsterSdpHeader *) header;
}

char* sdp_version_print(SipsterSdpHeader * header) {
    SipsterSdpHeaderVersion * version = (SipsterSdpHeaderVersion *) header;
    char *output;
    string params = "";
    const char * const format = "%c=%d";

    output = (char *) sipster_allocator(20);

    snprintf(output, 20, format, version->header.header, version->version);

    return output;
}

SipsterSdpHeader* sdp_version_clone(SipsterSdpHeader * header) {
    SipsterSdpHeaderVersion * version = (SipsterSdpHeaderVersion *) sipster_allocator(sizeof(SipsterSdpHeaderVersion));
    SipsterSdpHeaderVersion * casted = (SipsterSdpHeaderVersion *) header;

    version->header.header = casted->header.header;
    version->header.headerType = casted->header.headerType;
    version->version = version->version;

    return (SipsterSdpHeader *) version;
}

void sdp_origin_destroy(SipsterSdpHeader * header) {
    sdp_no_destroy(header);
}

SipsterSdpHeader* sdp_origin_parse(SipsterSdpHeaderEnum headerType, const char * input) {

    std::size_t pos = 0;
    SipsterSdpHeaderOrigin* origin_header = (SipsterSdpHeaderOrigin*) sipster_init_basic_header(headerType, sizeof(SipsterSdpHeaderOrigin));

    string username = nextToken(input, " ", &pos);
    strncpy(origin_header->username, username.c_str(), username.length()+1);

    string sessionId = nextToken(input, " ", &pos);
    origin_header->sessionId = atol(sessionId.c_str());

    string sessionVersion = nextToken(input, " ", &pos);
    origin_header->sessionVersion = atol(sessionVersion.c_str());

    string nettype = nextToken(input, " ", &pos);
    strncpy(origin_header->nettype, nettype.c_str(), nettype.length()+1);

    string addrtype = nextToken(input, " ", &pos);
    strncpy(origin_header->addrtype, addrtype.c_str(), addrtype.length()+1);

    string address = getRest(input, &pos);
    strncpy(origin_header->address, address.c_str(), address.length()+1);

    return (SipsterSdpHeader *) origin_header;
}

char* sdp_origin_print(SipsterSdpHeader * header) {
    SipsterSdpHeaderOrigin * origin_h = (SipsterSdpHeaderOrigin *) header;
    char * output;
    string params = "";
    const char * const format = "%c=%s %lu %lu %s %s %s";

    output = (char *) sipster_allocator(sizeof(SipsterSdpHeaderOrigin));

    snprintf(output, sizeof(SipsterSdpHeaderOrigin), format, sdp_header_prototypes[origin_h->header.headerType].headerName[0],
             origin_h->username, origin_h->sessionId, origin_h->sessionVersion, origin_h->nettype, origin_h->addrtype, origin_h->address);

    return output;
}
SipsterSdpHeader* sdp_origin_clone(SipsterSdpHeader * header) {
    SipsterSdpHeaderOrigin *cpy = (SipsterSdpHeaderOrigin *) sipster_allocator(sizeof(SipsterSdpHeaderOrigin));

    memcpy(cpy, header, sizeof(SipsterSdpHeaderOrigin));

    return (SipsterSdpHeader *) cpy;
}

void sdp_string_destroy(SipsterSdpHeader * header) {
    sdp_no_destroy(header);
}

SipsterSdpHeader* sdp_string_parse(SipsterSdpHeaderEnum headerType, const char * input) {
    std::size_t pos = 0;
    SipsterSdpHeaderString * str_header = (SipsterSdpHeaderString *) sipster_init_basic_header(headerType, sizeof(SipsterSdpHeaderString));

    SIPSTER_SDP_DEBUG(str_header->data);

    string dataString = getRest(input, &pos);
    strncpy(str_header->data, dataString.c_str(), sizeof(str_header->data)-1);

    return (SipsterSdpHeader *) str_header;
}

char* sdp_string_print(SipsterSdpHeader * header) {
    SipsterSdpHeaderString * str_hdr = (SipsterSdpHeaderString *) header;
    char * output;
    string params = "";
    const char * const format = "%c=%s";

    output = (char *) sipster_allocator(sizeof(SipsterSipHeaderString));

    snprintf(output, sizeof(SipsterSipHeaderString), format, str_hdr->header.header, str_hdr->data);

    return output;
}

SipsterSdpHeader* sdp_string_clone(SipsterSdpHeader * header) {
    SipsterSdpHeaderString *cpy = (SipsterSdpHeaderString *) sipster_allocator(sizeof(SipsterSdpHeaderString));

    memcpy(cpy, header, sizeof(SipsterSdpHeaderString));

    return (SipsterSdpHeader *) cpy;
}

void sdp_connection_destroy(SipsterSdpHeader * header) {
    sdp_no_destroy(header);
}

SipsterSdpHeader* sdp_connection_parse(SipsterSdpHeaderEnum headerType, const char * input) {

    std::size_t pos = 0;
    std::size_t pos2 = 0;
    SipsterSdpHeaderConnection* connection_header = (SipsterSdpHeaderConnection*) sipster_init_basic_header(headerType, sizeof(SipsterSdpHeaderConnection));

    string nettype = nextToken(input, " ", &pos);
    strncpy(connection_header->nettype, nettype.c_str(), nettype.length()+1);

    string addrtype = nextToken(input, " ", &pos);
    strncpy(connection_header->addrtype, addrtype.c_str(), addrtype.length()+1);

    string addressPart = getRest(input, &pos);
    string address = nextToken(addressPart.c_str(), "/", &pos2);
    strncpy(connection_header->address, address.c_str(), address.length()+1);

    string ttl = nextToken(addressPart.c_str(), "/", &pos2);
    connection_header->ttl = atoi(ttl.c_str());

    /*
     * TODO ADD TTL and addres range
     */

    return (SipsterSdpHeader *) connection_header;
}

char* sdp_connection_print(SipsterSdpHeader * header) {
    SipsterSdpHeaderConnection *conn_h = (SipsterSdpHeaderConnection *) header;
    char * output;
    string params = "";
    //const char * const format1 = "%c=%s %s %s";
    const char * const format2 = "%c=%s %s %s/%d";
    //const char * const format3 = "%c=%s %s %s/%d/%d";

    output = (char *) sipster_allocator(sizeof(SipsterSdpHeaderConnection));

    //TODO Add IPV6 formats
    snprintf(output, sizeof(SipsterSdpHeaderConnection), format2, sdp_header_prototypes[conn_h->header.headerType].headerName[0],
             conn_h->nettype, conn_h->addrtype, conn_h->address, conn_h->ttl > 1 ? conn_h->ttl : 127);

    return output;
}

SipsterSdpHeader* sdp_connection_clone(SipsterSdpHeader * header) {
    SipsterSdpHeaderConnection *cpy = (SipsterSdpHeaderConnection *) sipster_allocator(sizeof(SipsterSdpHeaderConnection));

    memcpy(cpy, header, sizeof(SipsterSdpHeaderConnection));

    return (SipsterSdpHeader *) cpy;
}

void sdp_timing_destroy(SipsterSdpHeader * header) {
    sdp_no_destroy(header);
}

SipsterSdpHeader* sdp_timing_parse(SipsterSdpHeaderEnum headerType, const char * input) {
    std::size_t pos = 0;
    SipsterSdpHeaderTiming* timing_header = (SipsterSdpHeaderTiming*) sipster_init_basic_header(headerType, sizeof(SipsterSdpHeaderTiming));

    string start = nextToken(input, " ", &pos);
    timing_header->start = atol(start.c_str());

    string stop = getRest(input, &pos);
    timing_header->stop = atol(stop.c_str());

    return (SipsterSdpHeader *) timing_header;
}

char* sdp_timing_print(SipsterSdpHeader * header) {
    SipsterSdpHeaderTiming *time_h = (SipsterSdpHeaderTiming *) header;
    char * output;
    string params = "";
    const char * const format2 = "%c=%lu %lu";

    output = (char *) sipster_allocator(50);

    snprintf(output, 50, format2, sdp_header_prototypes[time_h->header.headerType].headerName[0], time_h->start, time_h->stop);

    return output;
}

SipsterSdpHeader* sdp_timing_clone(SipsterSdpHeader * header) {
    SipsterSdpHeaderTiming *cpy = (SipsterSdpHeaderTiming *) sipster_allocator(sizeof(SipsterSdpHeaderTiming));

    memcpy(cpy, header, sizeof(SipsterSdpHeaderTiming));

    return (SipsterSdpHeader *) cpy;
}

void sdp_media_destroy(SipsterSdpHeader * header) {

}

SipsterSdpHeader* sdp_media_parse(SipsterSdpHeaderEnum headerType, const char * input) {
    std::size_t pos = 0;
    std::size_t pos2 = 0;
    SipsterSdpHeaderMedia * media_header = (SipsterSdpHeaderMedia*) sipster_init_basic_header(headerType, sizeof(SipsterSdpHeaderMedia));

    string media = nextToken(input, " ", &pos);
    SipsterSdpMediaTypeEnum mt = sipster_sdp_media_type_decode(media.c_str());
    media_header->mediaType = mt;

    string portStr = nextToken(input, " ", &pos);
    string port = nextToken(portStr.c_str(), "/", &pos2);
    media_header->port = atoi(port.c_str());
    string portCount = nextToken(portStr.c_str(), "/", &pos2);
    if(portCount.length() > 0) {
        media_header->portCount = atoi(portCount.c_str());
    }

    string protocol = nextToken(input, " ", &pos);
    strncpy(media_header->protocol, protocol.c_str(), protocol.length() + 1);

    string ptsString = getRest(input, &pos);
    trim(ptsString);
    unsigned long len = ptsString.length();
    pos2 = 0;
    while(len > 0) {
        string ptString = nextToken(ptsString, " ", &pos2);
        len = ptString.length();
        if(len < 1) {
            break;
        }

        media_header->pts[media_header->ptsCount] = atoi(ptString.c_str());
        media_header->ptsCount++;
    }

    return (SipsterSdpHeader *) media_header;
}

char* sdp_media_print(SipsterSdpHeader * header) {
    return NULL;
}

SipsterSdpHeader* sdp_media_clone(SipsterSdpHeader * header) {
    return NULL;
}

void sdp_attr_destroy(SipsterSdpHeader * header) {
    if(header) {
        SipsterSdpHeaderAttribute * attr = (SipsterSdpHeaderAttribute *) header;
        if(attr->attribute) {
            sipster_free(attr->attribute);
        }
    }
}

SipsterSdpHeader* sdp_attr_parse(SipsterSdpHeaderEnum headerType, const char * input) {

    SipsterSdpHeaderAttribute* attr_header = (SipsterSdpHeaderAttribute*) sipster_init_basic_header(headerType, sizeof(SipsterSdpHeaderAttribute));

    attr_header->attribute = sipster_sdp_parse_attribute(input);

    return (SipsterSdpHeader *) attr_header;
}

char* sdp_attr_print(SipsterSdpHeader * header) {
    SipsterSdpHeaderAttribute * attrHeader = (SipsterSdpHeaderAttribute *) header;
    return sdp_attribute_types[attrHeader->attribute->attributeType].print(attrHeader->attribute);
}

SipsterSdpHeader* sdp_attr_clone(SipsterSdpHeader * header) {
    SipsterSdpHeaderAttribute *cpy = (SipsterSdpHeaderAttribute *) sipster_allocator(sizeof(SipsterSdpHeaderAttribute));

    memcpy(cpy, header, sizeof(SipsterSdpHeaderAttribute));

    return (SipsterSdpHeader *) cpy;
}

//=====================================================================================================================================================
//====================================================== ATTRIBUTES ===================================================================================
//=====================================================================================================================================================

SipsterSdpAttribute* sdp_attr_no_parse(SipsterSdpAttributeEnum type, const char *input) {
    SipsterSdpAttribute * attr = (SipsterSdpAttribute *) sipster_allocator(sizeof(SipsterSdpAttribute));
    attr->attributeType = type;
    return attr;
}
char* sdp_attr_no_print(SipsterSdpAttribute *header) {
    char * output;
    const char *format = "%c=%s";
    output = (char *) sipster_allocator(50);
    snprintf(output, 50, format, sdp_header_prototypes[SDP_HEADER_ATTRIBUTE].headerName[0], sdp_attribute_types[header->attributeType].name);
    return output;
}

SipsterSdpAttribute* sdp_attr_no_clone(SipsterSdpAttribute *attr) {
    return cloneAttribute(attr, sizeof(SipsterSdpAttribute));
}

SipsterSdpAttribute* sdp_attr_rtpmap_parse(SipsterSdpAttributeEnum type, const char *input) {
    std::size_t pos = 0;
    std::size_t pos2 = 0;
    SipsterSdpAttributeRtpmap * attr = (SipsterSdpAttributeRtpmap *) sipster_allocator(sizeof(SipsterSdpAttributeRtpmap));
    attr->attribute.attributeType = type;

    string ptstr = nextToken(input, " ", &pos);
    attr->pt = atoi(ptstr.c_str());

    string encodingData = getRest(input, &pos);
    string encoding = nextToken(encodingData, "/", &pos2);
    strncpy(attr->ecoding, encoding.c_str(), encoding.length()+1);

    string clock = nextToken(encodingData, "/", &pos2);
    attr->clock = atoi(clock.c_str());

    string params = getRest(encodingData, &pos2);
    strncpy(attr->params, params.c_str(), params.length()+1);
    if(params.length() > 0) {
        attr->channels = atoi(params.c_str());
    }

    return (SipsterSdpAttribute *) attr;
}

char* sdp_attr_rtpmap_print(SipsterSdpAttribute *header) {
    char * output;
    const char *format = "%c=%s:%u %s/%u";
    const char *format2 = "%c=%s:%u %s/%u/%u";

    SipsterSdpAttributeRtpmap *attr = (SipsterSdpAttributeRtpmap *) header;
    output = (char *) sipster_allocator(sizeof(SipsterSdpAttributeRtpmap) + 10);
    if(attr->channels > 1) {
        snprintf(output, sizeof(SipsterSdpAttributeFmtp) + 10, format2, sdp_header_prototypes[SDP_HEADER_ATTRIBUTE].headerName[0], sdp_attribute_types[header->attributeType].name,
                 attr->pt, attr->ecoding, attr->clock, attr->channels);
    } else {
        snprintf(output, sizeof(SipsterSdpAttributeFmtp) + 10, format, sdp_header_prototypes[SDP_HEADER_ATTRIBUTE].headerName[0], sdp_attribute_types[header->attributeType].name,
                 attr->pt, attr->ecoding, attr->clock);
    }

    return output;
}

SipsterSdpAttribute* sdp_attr_rtpmap_clone(SipsterSdpAttribute *attr) {
    return cloneAttribute(attr, sizeof(SipsterSdpAttributeRtpmap));
}

SipsterSdpAttribute* sdp_attr_fmtp_parse(SipsterSdpAttributeEnum type, const char *input) {
    std::size_t pos = 0;
    std::size_t pos2 = 0;
    SipsterSdpAttributeFmtp * attr = (SipsterSdpAttributeFmtp *) sipster_allocator(sizeof(SipsterSdpAttributeFmtp));
    attr->attribute.attributeType = type;

    string ptstr = nextToken(input, " ", &pos);
    attr->pt = atoi(ptstr.c_str());

    string params = getRest(input, &pos2);
    strncpy(attr->params, params.c_str(), params.length()+1);

    return (SipsterSdpAttribute *) attr;
}

char* sdp_attr_fmtp_print(SipsterSdpAttribute *header) {
    char * output;
    const char *format = "%c=%s:%u %s";

    SipsterSdpAttributeFmtp *attr = (SipsterSdpAttributeFmtp *) header;
    output = (char *) sipster_allocator(sizeof(SipsterSdpAttributeFmtp) + 10);
    snprintf(output, sizeof(SipsterSdpAttributeFmtp) + 10, format, sdp_header_prototypes[SDP_HEADER_ATTRIBUTE].headerName[0], sdp_attribute_types[header->attributeType].name,
    attr->pt, attr->params);
    return output;
}

SipsterSdpAttribute* sdp_attr_fmtp_clone(SipsterSdpAttribute *attr) {
    return cloneAttribute(attr, sizeof(SipsterSdpAttributeFmtp));
}

SipsterSdpMediaTypeEnum sipster_sdp_media_type_decode(const char * input) {
    int i = 0;
    string headerString = input;

    for(i = 0; sdp_media_types[i].type != SDP_MEDIA_NONE; i++) {
        const SipsterSdpMediaTypeMap mmap = sdp_media_types[i];
        if(headerString.compare(mmap.media) == 0) {
            return sdp_media_types[i].type;
        }
    }

    return SDP_MEDIA_NONE;
}

SipsterSdpAttribute * sipster_sdp_parse_attribute(const char *input) {

    std::size_t pos = 0;
    int i = 0;
    string attrString = nextToken(input, ":", &pos);
    string rest = getRest(input, &pos);
    string cleanRest = ltrim(rest);

    for(i = 0; sdp_attribute_types[i].type != SDP_ATTRIBUTE_NONE; i++) {
        const SipsterSdpAttributeMap amap = sdp_attribute_types[i];
        if(attrString.compare(amap.name) == 0) {
            SIPSTER_SDP_DEBUG("PARSING ATTRIBUTE %s", amap.name);
            SipsterSdpAttribute *attr = amap.parse(amap.type, cleanRest.c_str());
            return attr;
        }
    }

    return NULL;
}

SipsterSdpHeader * sipster_sdp_parse_header(const char *input) {

    std::size_t pos = 0;
    int i = 0;
    string headerString = nextToken(input, "=", &pos);
    string rest = getRest(input, &pos);
    string cleanRest = ltrim(rest);

    for(i = 0; sdp_header_prototypes[i].headerId != SDP_HEADER_NONE; i++) {
        const SipsterSdpHeaderMap hmap = sdp_header_prototypes[i];
        if(headerString.compare(hmap.headerName) == 0) {
            SIPSTER_SDP_DEBUG("PARSING HEADER %s", hmap.headerName);
            SipsterSdpHeader * header = hmap.parse(hmap.headerId, cleanRest.c_str());
            return header;
        }
    }

    return NULL;
}

char * sipster_sdp_print_header(SipsterSdpHeader * header) {
    SIPSTER_SDP_DEBUG("PRINTING HEADER: %c", header->header);
    const SipsterSdpHeaderMap hmap = sdp_header_prototypes[header->headerType];
    char * headerLine = hmap.print(header);
    sipster_sdp_header_destroy(header);

    return headerLine;
}

SipsterSdpAttribute *cloneAttribute(const SipsterSdpAttribute *attr, size_t size) {
    SipsterSdpAttributeRtpmap *cpy = (SipsterSdpAttributeRtpmap *) sipster_allocator(sizeof(SipsterSdpAttributeRtpmap));

    memcpy(cpy, attr, sizeof(SipsterSdpAttributeRtpmap));

    return (SipsterSdpAttribute *) cpy;
}