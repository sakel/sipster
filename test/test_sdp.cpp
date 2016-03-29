//
// Created by dejan on 14.3.2016.
//
#include <sipster/log.h>
#include <sipster/sdp.h>
#include <utils.h>
#include <stdlib.h>

#include <gtest/gtest.h>

#include <string>
#include <iostream>

using namespace std;


TEST(test_sdp_v_parse_print, test_sdp_v_parse_print_1) {

    char header_str[] = "v=0";

    SIPSTER_SDP_INFO("Parsing SDP version header");

    SipsterSdpHeader * header = sipster_sdp_parse_header(header_str);
    EXPECT_TRUE(header);
    EXPECT_EQ(SDP_HEADER_VERSION, header->headerType);
    EXPECT_EQ('v', header->header);

    SipsterSdpHeaderVersion * headerVersion = (SipsterSdpHeaderVersion *) header;

    EXPECT_EQ((unsigned int) 0, headerVersion->version);

    char * pHeader = sipster_sdp_print_header(header);
    SIPSTER_SDP_DEBUG(pHeader);
    EXPECT_STREQ(header_str, pHeader);

    sipster_free(pHeader);
    sipster_sdp_destroy_header(header);
}

TEST(test_sdp_o_parse_print, test_sdp_o_parse_print_1) {

    char header_str[] = "o=jdoe 2890844526 2890842807 IN IP4 10.47.16.5";

    SIPSTER_SDP_INFO("Parsing SDP origin header");

    SipsterSdpHeader * header = sipster_sdp_parse_header(header_str);
    EXPECT_TRUE(header);
    EXPECT_EQ(SDP_HEADER_ORIGIN, header->headerType);
    EXPECT_EQ('o', header->header);

    SipsterSdpHeaderOrigin * headerParsed = (SipsterSdpHeaderOrigin *) header;

    EXPECT_STREQ("jdoe", headerParsed->username);
    EXPECT_STREQ("10.47.16.5", headerParsed->address);
    EXPECT_STREQ("IN", headerParsed->nettype);
    EXPECT_STREQ("IP4", headerParsed->addrtype);
    EXPECT_EQ((unsigned long) 2890844526, headerParsed->sessionId);
    EXPECT_EQ((unsigned long) 2890842807, headerParsed->sessionVersion);

    char * pHeader = sipster_sdp_print_header(header);
    SIPSTER_SDP_DEBUG(pHeader);
    EXPECT_STREQ(header_str, pHeader);


    sipster_free(pHeader);
    sipster_sdp_destroy_header(header);
}

TEST(test_sdp_s_parse_print, test_sdp_s_parse_print_1) {

    char header_str[] = "s=SDP Seminar";

    SIPSTER_SDP_INFO("Parsing SDP session name header");

    SipsterSdpHeader * header = sipster_sdp_parse_header(header_str);
    EXPECT_TRUE(header);
    EXPECT_EQ(SDP_HEADER_SESSION_NAME, header->headerType);
    EXPECT_EQ('s', header->header);

    SipsterSdpHeaderSessionName * headerVersion = (SipsterSdpHeaderSessionName *) header;

    EXPECT_STREQ("SDP Seminar", headerVersion->data);

    char * pHeader = sipster_sdp_print_header(header);
    SIPSTER_SDP_DEBUG(pHeader);
    EXPECT_STREQ(header_str, pHeader);

    sipster_free(pHeader);
    sipster_sdp_destroy_header(header);
}

TEST(test_sdp_i_parse_print, test_sdp_i_parse_print_1) {

    char header_str[] = "i=A Seminar on the session description protocol";

    SIPSTER_SDP_INFO("Parsing SDP session info header");

    SipsterSdpHeader * header = sipster_sdp_parse_header(header_str);
    EXPECT_TRUE(header);
    EXPECT_EQ(SDP_HEADER_SESSION_INFO, header->headerType);
    EXPECT_EQ('i', header->header);

    SipsterSdpHeaderSessionInfo * headerVersion = (SipsterSdpHeaderSessionInfo *) header;

    EXPECT_STREQ("A Seminar on the session description protocol", headerVersion->data);

    char * pHeader = sipster_sdp_print_header(header);
    SIPSTER_SDP_DEBUG(pHeader);
    EXPECT_STREQ(header_str, pHeader);

    sipster_free(pHeader);
    sipster_sdp_destroy_header(header);
}

TEST(test_sdp_u_parse_print, test_sdp_u_parse_print_1) {

    char header_str[] = "u=http://www.example.com/seminars/sdp.pdf";

    SIPSTER_SDP_INFO("Parsing SDP URI header");

    SipsterSdpHeader * header = sipster_sdp_parse_header(header_str);
    EXPECT_TRUE(header);
    EXPECT_EQ(SDP_HEADER_DESC_URI, header->headerType);
    EXPECT_EQ('u', header->header);

    SipsterSdpHeaderUri * headerVersion = (SipsterSdpHeaderUri *) header;

    EXPECT_STREQ("http://www.example.com/seminars/sdp.pdf", headerVersion->data);

    char * pHeader = sipster_sdp_print_header(header);
    SIPSTER_SDP_DEBUG(pHeader);
    EXPECT_STREQ(header_str, pHeader);

    sipster_free(pHeader);
    sipster_sdp_destroy_header(header);
}

TEST(test_sdp_e_parse_print, test_sdp_e_parse_print_1) {

    char header_str[] = "e=j.doe@example.com (Jane Doe)";

    SIPSTER_SDP_INFO("Parsing SDP Email header");

    SipsterSdpHeader * header = sipster_sdp_parse_header(header_str);
    EXPECT_TRUE(header);
    EXPECT_EQ(SDP_HEADER_EMAIL, header->headerType);
    EXPECT_EQ('e', header->header);

    SipsterSdpHeaderEmail * headerVersion = (SipsterSdpHeaderEmail *) header;

    EXPECT_STREQ("j.doe@example.com (Jane Doe)", headerVersion->data);

    char * pHeader = sipster_sdp_print_header(header);
    SIPSTER_SDP_DEBUG(pHeader);
    EXPECT_STREQ(header_str, pHeader);

    sipster_free(pHeader);
    sipster_sdp_destroy_header(header);
}

TEST(test_sdp_p_parse_print, test_sdp_p_parse_print_1) {

    char header_str[] = "p=+386 41 850 869";

    SIPSTER_SDP_INFO("Parsing SDP Phone header");

    SipsterSdpHeader * header = sipster_sdp_parse_header(header_str);
    EXPECT_TRUE(header);
    EXPECT_EQ(SDP_HEADER_PHONE, header->headerType);
    EXPECT_EQ('p', header->header);

    SipsterSdpHeaderPhone * headerVersion = (SipsterSdpHeaderPhone *) header;

    EXPECT_STREQ("+386 41 850 869", headerVersion->data);

    char * pHeader = sipster_sdp_print_header(header);
    SIPSTER_SDP_DEBUG(pHeader);
    EXPECT_STREQ(header_str, pHeader);

    sipster_free(pHeader);
    sipster_sdp_destroy_header(header);
}

TEST(test_sdp_c_parse_print, test_sdp_c_parse_print_1) {

    char header_str[] = "c=IN IP4 224.2.17.12/127";

    SIPSTER_SDP_INFO("Parsing SDP connection header");

    SipsterSdpHeader * header = sipster_sdp_parse_header(header_str);
    EXPECT_TRUE(header);
    EXPECT_EQ(SDP_HEADER_CONNECTION_INFO, header->headerType);
    EXPECT_EQ('c', header->header);

    SipsterSdpHeaderConnection * headerParsed = (SipsterSdpHeaderConnection *) header;

    EXPECT_STREQ("IN", headerParsed->nettype);
    EXPECT_STREQ("IP4", headerParsed->addrtype);
    EXPECT_STREQ("224.2.17.12", headerParsed->address);
    EXPECT_EQ(127, headerParsed->ttl);
    EXPECT_EQ(0, headerParsed->addressCount);

    char * pHeader = sipster_sdp_print_header(header);
    SIPSTER_SDP_DEBUG(pHeader);
    EXPECT_STREQ(header_str, pHeader);
}

TEST(test_sdp_t_parse_print, test_sdp_t_parse_print_1) {

    char header_str[] = "t=2873397496 2873404696";

    SIPSTER_SDP_INFO("Parsing SDP timing header");

    SipsterSdpHeader * header = sipster_sdp_parse_header(header_str);
    EXPECT_TRUE(header);
    EXPECT_EQ(SDP_HEADER_TIME_ACTIVE, header->headerType);
    EXPECT_EQ('t', header->header);

    SipsterSdpHeaderTiming * headerParsed = (SipsterSdpHeaderTiming *) header;

    EXPECT_EQ((unsigned long) 2873397496, headerParsed->start);
    EXPECT_EQ((unsigned long) 2873404696, headerParsed->stop);

    char * pHeader = sipster_sdp_print_header(header);
    SIPSTER_SDP_DEBUG(pHeader);
    EXPECT_STREQ(header_str, pHeader);
}

TEST(test_sdp_a_parse_print_1, test_sdp_a_parse_print_1) {

    char header_str[] = "a=rtpmap:99 h263-1998/90000";

    SIPSTER_SDP_INFO("Parsing SDP video rtpmap attribute header");

    SipsterSdpHeader * header = sipster_sdp_parse_header(header_str);
    EXPECT_TRUE(header);
    EXPECT_EQ(SDP_HEADER_ATTRIBUTE, header->headerType);
    EXPECT_EQ('a', header->header);

    SipsterSdpHeaderAttribute * headerParsed = (SipsterSdpHeaderAttribute *) header;
    EXPECT_TRUE(headerParsed->attribute);
    EXPECT_EQ(SDP_ATTRIBUTE_RTPMAP, headerParsed->attribute->attributeType);

    SipsterSdpAttributeRtpmap * rtpmap = (SipsterSdpAttributeRtpmap *) headerParsed->attribute;
    EXPECT_EQ((unsigned short) 99, rtpmap->pt);
    EXPECT_EQ((unsigned int) 90000, rtpmap->clock);
    EXPECT_EQ((unsigned short) 0, rtpmap->channels);
    EXPECT_STREQ("h263-1998", rtpmap->ecoding);
    EXPECT_STREQ("", rtpmap->params);

    char * pHeader = sipster_sdp_print_header(header);
    SIPSTER_SDP_DEBUG(pHeader);
    EXPECT_STREQ(header_str, pHeader);
}

TEST(test_sdp_a_parse_print_2, test_sdp_a_parse_print_2) {

    char header_str[] = "a=rtpmap:98 PCMU/8000/2";

    SIPSTER_SDP_INFO("Parsing SDP aidio rtpmap attribute header");

    SipsterSdpHeader * header = sipster_sdp_parse_header(header_str);
    EXPECT_TRUE(header);
    EXPECT_EQ(SDP_HEADER_ATTRIBUTE, header->headerType);
    EXPECT_EQ('a', header->header);

    SipsterSdpHeaderAttribute * headerParsed = (SipsterSdpHeaderAttribute *) header;
    EXPECT_TRUE(headerParsed->attribute);
    EXPECT_EQ(SDP_ATTRIBUTE_RTPMAP, headerParsed->attribute->attributeType);

    SipsterSdpAttributeRtpmap * rtpmap = (SipsterSdpAttributeRtpmap *) headerParsed->attribute;
    EXPECT_EQ((unsigned short) 98, rtpmap->pt);
    EXPECT_EQ((unsigned int) 8000, rtpmap->clock);
    EXPECT_EQ((unsigned short) 2, rtpmap->channels);
    EXPECT_STREQ("PCMU", rtpmap->ecoding);
    EXPECT_STREQ("2", rtpmap->params);

    char * pHeader = sipster_sdp_print_header(header);
    SIPSTER_SDP_DEBUG(pHeader);
    EXPECT_STREQ(header_str, pHeader);
}

TEST(test_sdp_m_parse_print_1, test_sdp_m_parse_print_1) {

    char header_str[] = "m=video 51372 RTP/AVP 99";

    SIPSTER_SDP_INFO("Parsing SDP aidio meda header");

    SipsterSdpHeader * header = sipster_sdp_parse_header(header_str);
    EXPECT_TRUE(header);
    EXPECT_EQ(SDP_HEADER_MEDIA, header->headerType);
    EXPECT_EQ('m', header->header);

    SipsterSdpHeaderMedia * headerParsed = (SipsterSdpHeaderMedia *) header;
    EXPECT_EQ(SDP_MEDIA_VIDEO, headerParsed->mediaType);
    EXPECT_EQ((unsigned char) 1, headerParsed->ptsCount);
    EXPECT_EQ((unsigned short) 99 , headerParsed->pts[0]);
    EXPECT_EQ((unsigned short) 51372, headerParsed->port);
    EXPECT_EQ((unsigned short) 0, headerParsed->portCount);
    EXPECT_STREQ("RTP/AVP", headerParsed->protocol);

    char * pHeader = sipster_sdp_print_header(header);
    SIPSTER_SDP_DEBUG(pHeader);
    EXPECT_STREQ(header_str, pHeader);
}

TEST(test_sdp_m_parse_print_2, test_sdp_m_parse_print_2) {

    char header_str[] = "m=video 51372 RTP/AVP 34 97 98 99";

    SIPSTER_SDP_INFO("Parsing SDP aidio meda header");

    SipsterSdpHeader * header = sipster_sdp_parse_header(header_str);
    EXPECT_TRUE(header);
    EXPECT_EQ(SDP_HEADER_MEDIA, header->headerType);
    EXPECT_EQ('m', header->header);

    SipsterSdpHeaderMedia * headerParsed = (SipsterSdpHeaderMedia *) header;
    EXPECT_EQ(SDP_MEDIA_VIDEO, headerParsed->mediaType);
    EXPECT_EQ((unsigned char) 4, headerParsed->ptsCount);
    EXPECT_EQ((unsigned short) 34, headerParsed->pts[0]);
    EXPECT_EQ((unsigned short) 97, headerParsed->pts[1]);
    EXPECT_EQ((unsigned short) 98, headerParsed->pts[2]);
    EXPECT_EQ((unsigned short) 99, headerParsed->pts[3]);
    EXPECT_EQ((unsigned short) 51372, headerParsed->port);
    EXPECT_EQ((unsigned short) 0, headerParsed->portCount);
    EXPECT_STREQ("RTP/AVP", headerParsed->protocol);

    char * pHeader = sipster_sdp_print_header(header);
    SIPSTER_SDP_DEBUG(pHeader);
    EXPECT_STREQ(header_str, pHeader);
}

TEST(test_sdp_m_parse_print_3, test_sdp_m_parse_print_3) {

    char header_str[] = "m=video 51372/2 RTP/AVP 34 97 98 99";

    SIPSTER_SDP_INFO("Parsing SDP aidio meda header");

    SipsterSdpHeader * header = sipster_sdp_parse_header(header_str);
    EXPECT_TRUE(header);
    EXPECT_EQ(SDP_HEADER_MEDIA, header->headerType);
    EXPECT_EQ('m', header->header);

    SipsterSdpHeaderMedia * headerParsed = (SipsterSdpHeaderMedia *) header;
    EXPECT_EQ(SDP_MEDIA_VIDEO, headerParsed->mediaType);
    EXPECT_EQ((unsigned char) 4, headerParsed->ptsCount);
    EXPECT_EQ((unsigned short) 34, headerParsed->pts[0]);
    EXPECT_EQ((unsigned short) 97, headerParsed->pts[1]);
    EXPECT_EQ((unsigned short) 98, headerParsed->pts[2]);
    EXPECT_EQ((unsigned short) 99, headerParsed->pts[3]);
    EXPECT_EQ((unsigned short) 51372, headerParsed->port);
    EXPECT_EQ((unsigned short) 2, headerParsed->portCount);
    EXPECT_STREQ("RTP/AVP", headerParsed->protocol);

    char * pHeader = sipster_sdp_print_header(header);
    SIPSTER_SDP_DEBUG(pHeader);
    EXPECT_STREQ(header_str, pHeader);
}

TEST(test_sdp_parse_print_1, test_sdp_parse_print_1) {
    SipsterSdp *sdp;

    string sdps = "";
    sdps = sdps + "v=0\r\n" +
                "o=jdoe 2890844526 2890842807 IN IP4 10.47.16.5\r\n" +
                "s=SDP Seminar\r\n" +
                "i=A Seminar on the session description protocol\r\n" +
                "u=http://www.example.com/seminars/sdp.pdf\r\n" +
                "e=j.doe@example.com (Jane Doe)\r\n" +
                "c=IN IP4 224.2.17.12/127\r\n" +
                "t=2873397496 2873404696\r\n" +
                "a=recvonly\r\n" +
                "m=audio 49170 RTP/AVP 0\r\n" +
                "m=video 51372 RTP/AVP 99\r\n" +
                "a=rtpmap:99 h263-1998/90000\r\n\r\n";

    SIPSTER_SDP_INFO("Parsing SDP");

    int ret = sipster_sdp_parse(sdps.c_str(), &sdp);
    EXPECT_EQ(0, ret);
    EXPECT_TRUE(sdp);
    EXPECT_EQ((unsigned int) 0, sdp->version->version);
    EXPECT_EQ((unsigned short) 49170, sdp->media[0].mediaHeader->port);
    EXPECT_EQ((unsigned short) 51372, sdp->media[1].mediaHeader->port);
//    EXPECT_EQ('m', header->header);

    char * sdps2 = sipster_sdp_print(sdp);
    SIPSTER_SDP_DEBUG(sdps2);
    EXPECT_STREQ(sdps.c_str(), sdps2);

    sipster_sdp_destroy(sdp);
    sipster_free(sdps2);
}

/*
v=0
o=jdoe 2890844526 2890842807 IN IP4 10.47.16.5
s=SDP Seminar
i=A Seminar on the session description protocol
u=http://www.example.com/seminars/sdp.pdf
e=j.doe@example.com (Jane Doe)
c=IN IP4 224.2.17.12/127
t=2873397496 2873404696
a=recvonly
m=audio 49170 RTP/AVP 0
m=video 51372 RTP/AVP 99
a=rtpmap:99 h263-1998/90000
 */