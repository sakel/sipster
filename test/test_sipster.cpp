#include <sipster/log.h>
#include <sipster/sip.h>
#include <sipster/sip_headers.h>

#include <utils.h>
#include <stdlib.h>

#include <gtest/gtest.h>

#include <string>
#include <iostream>

using namespace std;

TEST(test_tokenizer, test_tokenizer_1) {
    std::size_t pos = 0;
    string input = "INVITE sip:bob@biloxi.com SIP/2.0";

    setenv("SIPSTER_LOG_LEVEL", "5", 1);

    string token = nextToken(input, " ", &pos);
    SIPSTER_DEBUG(token.c_str());

    string token2 = nextToken(input, " ", &pos);
    SIPSTER_DEBUG(token2.c_str());

    string token3 = getRest(input, &pos);
    SIPSTER_DEBUG(token3.c_str());

    EXPECT_EQ("INVITE", token);
    EXPECT_EQ("sip:bob@biloxi.com", token2);
    EXPECT_EQ("SIP/2.0", token3);
}

TEST(test_tokenizer_corner, test_tokenizer_2) {
    std::size_t pos = 0;

    string input = "rport";
    string token = nextToken(input, ";", &pos);
    SIPSTER_DEBUG("%s __ %d", token.c_str(), pos);
    EXPECT_STREQ("rport", token.c_str());

    token = nextToken(input, ";", &pos);
    SIPSTER_DEBUG("%s __ %d", token.c_str(), pos);
    EXPECT_TRUE(token.empty());
}

TEST(test_parse_params, test_parse_params_1) {
    std::size_t pos = 0;

    string input = "Bob Marley <sip:bob@marley.com>;tag=sdfsssdkgj;rport";
    string token = nextToken(input, ";", &pos);
    SIPSTER_DEBUG("%s __ %d", token.c_str(), pos);
    EXPECT_STREQ("Bob Marley <sip:bob@marley.com>", token.c_str());

    token = nextToken(input, ";", &pos);
    SIPSTER_DEBUG("%s __ %d", token.c_str(), pos);
    EXPECT_STREQ("tag=sdfsssdkgj", token.c_str());

    token = nextToken(input, ";", &pos);
    SIPSTER_DEBUG("%s __ %d", token.c_str(), pos);
    EXPECT_STREQ("rport", token.c_str());

    token = nextToken(input, ";", &pos);
    SIPSTER_DEBUG("%s __ %d", token.c_str(), pos);
    EXPECT_TRUE(token.empty());
}


TEST(test_req_line_parser, test_req_line_parser_1) {

    SipsterSipRequest *request;
    char a[] = "INVITE sip:bob@biloxi.com SIP/2.0";

    request = sipster_request_create();

    EXPECT_TRUE(request);

    int res = sipster_request_parse_line(a, request);

    SIPSTER_DEBUG(request->requestLine.method.method);

    EXPECT_EQ(res, 0);
    EXPECT_STREQ("INVITE", request->requestLine.method.method);
    EXPECT_EQ(request->requestLine.method.methodId, SIP_METHOD_INVITE);
    EXPECT_STREQ("sip:bob@biloxi.com", request->requestLine.requestUri);
    EXPECT_STREQ("SIP/2.0", request->requestLine.version);

    sipster_request_destroy(request);
}

TEST(test_res_line_parser, test_res_line_parser_1) {
    SipsterSipResponse * response;
    char a[] = "SIP/2.0 404 The number you have dialed is not in service";

    response = sipster_response_create();

    EXPECT_TRUE(response);

    int res = sipster_response_parse_line(a, response);

    SIPSTER_DEBUG(response->responseLine.status.reason);

    EXPECT_EQ(res, 0);
    EXPECT_STREQ("SIP/2.0", response->responseLine.version);
    EXPECT_EQ(SIP_STATUS_404_NOT_FOUND, response->responseLine.status.status);
    EXPECT_EQ(404, response->responseLine.status.statusCode);

    //sipster_response_destroy(response);
}

TEST(test_to_parse_print, test_to_parse_print_1) {

    char to[] = "To: Alice <sip:alice@atlanta.com>;tag=1928301774";

    SIPSTER_INFO("Parsing header");

    SipsterSipHeader * header = sipster_request_parse_header(to);
    EXPECT_TRUE(header);
    EXPECT_EQ(SIP_HEADER_TO, header->headerId);
    EXPECT_STREQ("To", header->headerName);

    SipsterSipHeaderTo * headerTo = (SipsterSipHeaderTo *) header;

    EXPECT_STREQ("Alice <sip:alice@atlanta.com>", headerTo->address);

    SipsterSipParameter * param = headerTo->header.first;
    EXPECT_STREQ("tag", param->name);
    EXPECT_STREQ("1928301774", param->value);
    EXPECT_FALSE(param->next);

    char * pHeader = sipster_request_print_header(header);
    SIPSTER_DEBUG(pHeader);
    EXPECT_STREQ(to, pHeader);

}


TEST(test_from_parse_print, test_from_parse_print_1) {
    char from[] = "From: Alice <sip:alice@atlanta.com>;tag=1928301774";

    SIPSTER_INFO("Parsing header");

    SipsterSipHeader * header = sipster_request_parse_header(from);
    EXPECT_TRUE(header);
    EXPECT_EQ(SIP_HEADER_FROM, header->headerId);
    EXPECT_STREQ("From", header->headerName);

    SipsterSipHeaderFrom * headerFrom = (SipsterSipHeaderFrom *) header;

    EXPECT_STREQ("Alice <sip:alice@atlanta.com>", headerFrom->address);

    SipsterSipParameter * param = headerFrom->header.first;
    EXPECT_STREQ("tag", param->name);
    EXPECT_STREQ("1928301774", param->value);
    EXPECT_FALSE(param->next);

    char * pHeader = sipster_request_print_header(header);
    SIPSTER_DEBUG(pHeader);
    EXPECT_STREQ(from, pHeader);
}


TEST(test_via_parse_print, test_via_parse_print_1) {
    char via[] = "Via: SIP/2.0/UDP pc33.atlanta.com;branch=z9hG4bKnashds8";

    SIPSTER_INFO("Parsing header");

    SipsterSipHeader * header = sipster_request_parse_header(via);
    EXPECT_TRUE(header);
    EXPECT_EQ(SIP_HEADER_VIA, header->headerId);
    EXPECT_STREQ("Via", header->headerName);

    SipsterSipHeaderVia * headerVia = (SipsterSipHeaderVia *) header;
    EXPECT_STREQ("pc33.atlanta.com", headerVia->address);
    EXPECT_STREQ("SIP/2.0/UDP", headerVia->protocol);

    SipsterSipParameter * param = headerVia->header.first;
    EXPECT_STREQ("branch", param->name);
    EXPECT_STREQ("z9hG4bKnashds8", param->value);
    EXPECT_FALSE(param->next);

    char * pHeader = sipster_request_print_header(header);
    SIPSTER_DEBUG(pHeader);
    EXPECT_STREQ(via, pHeader);
}

TEST(test_cl_parse_print, test_cl_parse_print_1) {
    char clen[] = "Content-Length: 349";

    SIPSTER_INFO("Parsing header");

    SipsterSipHeader * header = sipster_request_parse_header(clen);
    EXPECT_TRUE(header);
    EXPECT_EQ(SIP_HEADER_CONTENT_LENGTH, header->headerId);
    EXPECT_STREQ("Content-Length", header->headerName);

    SipsterSipHeaderContentLength * headerCl = (SipsterSipHeaderContentLength *) header;
    EXPECT_EQ((unsigned int)349, headerCl->length);

    char * pHeader = sipster_request_print_header(header);
    SIPSTER_DEBUG(pHeader);
    EXPECT_STREQ(clen, pHeader);
}

TEST(test_ct_parse_print, test_ct_parse_print_1) {
    char ct[] = "Content-Type: application/sdp;charset=ISO-8859-4";

    SIPSTER_INFO("Parsing header");

    SipsterSipHeader * header = sipster_request_parse_header(ct);
    EXPECT_TRUE(header);
    EXPECT_EQ(SIP_HEADER_CONTENT_TYPE, header->headerId);
    EXPECT_STREQ("Content-Type", header->headerName);

    SipsterSipHeaderContentType * headerCt = (SipsterSipHeaderContentType *) header;
    EXPECT_STREQ("application/sdp", headerCt->contentType);

    SipsterSipParameter * param = headerCt->header.first;
    EXPECT_STREQ("charset", param->name);
    EXPECT_STREQ("ISO-8859-4", param->value);
    EXPECT_FALSE(param->next);

    char * pHeader = sipster_request_print_header(header);
    SIPSTER_DEBUG(pHeader);
    EXPECT_STREQ(ct, pHeader);
}

TEST(test_request_parse_print, test_request_parse_print_1) {
    int result = 0;

    SIPSTER_DEBUG("PARSING REQUEST");

    string invite = "";
            invite = invite + "INVITE sip:bob@biloxi.com SIP/2.0\r\n"+
            "Via: SIP/2.0/UDP pc33.atlanta.com;branch=z9hG4bKnashds8\r\n"+
            "To: Bob <bob@biloxi.com>\r\n"+
            "From: Alice <alice@atlanta.com>;tag=1928301774\r\n"+
//            "Call-ID: a84b4c76e66710\r\n"+
            "CSeq: 314159 INVITE\r\n"+
//            "Max-Forwards: 70\r\n"+
//            "Date: Thu, 21 Feb 2002 13:02:03 GMT\r\n"+
            "Contact: <sip:alice@pc33.atlanta.com>\r\n"+
            "Content-Type: application/sdp\r\n"+
            "Content-Length: 147\r\n"+
            "\r\n"+
            "v=0\r\n"+
            "o=UserA 2890844526 2890844526 IN IP4 here.com\r\n"+
            "s=Session SDP\r\n"+
            "c=IN IP4 pc33.atlanta.com\r\n"+
            "t=0 0\r\n"+
            "m=audio 49172 RTP/AVP 0\r\n"+
            "a=rtpmap:0 PCMU/8000\r\n"+
            "\r\n";

    printf("%s\n", invite.c_str());


    SipsterSipRequest * req = sipster_request_parse(invite.c_str(), invite.length(), &result);
    EXPECT_TRUE(req);
    EXPECT_EQ(SIP_METHOD_INVITE, req->requestLine.method.methodId);

    EXPECT_TRUE(req->firstHeader);
    EXPECT_TRUE(req->lastHeader);
    EXPECT_TRUE(req->content);

    EXPECT_EQ((unsigned int) 147, req->content->length);
    EXPECT_STREQ("application/sdp", req->content->contentType);



//    SipsterSipHeader * header = sipster_request_parse_header(ct);
//    EXPECT_TRUE(header);
//    EXPECT_EQ(SIP_HEADER_CONTENT_TYPE, header->headerId);
//    EXPECT_STREQ("Content-Type", header->headerName);

//    SipsterSipHeaderContentType * headerCt = (SipsterSipHeaderContentType *) header;
//    EXPECT_STREQ("application/sdp", headerCt->contentType);

//    SipsterSipParameter * param = headerCt->header.first;
//    EXPECT_STREQ("charset", param->name);
//    EXPECT_STREQ("ISO-8859-4", param->value);
//    EXPECT_FALSE(param->next);

//    char * pHeader = sipster_request_print_header(header);
//    SIPSTER_DEBUG(pHeader);
//    EXPECT_STREQ(ct, pHeader);
}
