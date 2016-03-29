#include <gtest/gtest.h>
#include <sipster/sipster.h>
#include <sipster/log.h>
#include <uv.h>
#include <utils.h>
#include <stdio.h>

using namespace std;

uv_async_t as;

TEST(DISABLED_test_sipster_init_deinit, test_sipster_init_deinit_1) {

    setenv("SIPSTER_LOG_LEVEL", "5", 1);

    Sipster * sipster = NULL;
    sipster_init(&sipster, NULL);
    SIPSTER_DEBUG("DONE INIT");

//    int ret = sipster_loop_run(sipster);
//    SIPSTER_DEBUG("RUNNING");
//    EXPECT_EQ(0, ret);

//    sipster_loop_end(sipster);
//    SIPSTER_DEBUG("ENDING");

    sipster_deinit(sipster);
    SIPSTER_DEBUG("DONE DEINIT");
}

void *threadSender(void *data) {
    int i = 0;
    Sipster *sipster = (Sipster *)data;

    sleep(5);
    for(i = 0; i < 100; i++) {


        SipsterSipCallLeg *leg;
//        memset(&leg, 0, sizeof(leg));
        leg = (SipsterSipCallLeg *) sipster_allocator(sizeof(SipsterSipCallLeg));
        leg->requestHandler = NULL;
        leg->responseHandler = NULL;

        SipsterSipRequest *request;
//        memset(&request, 0, sizeof(request));
        request = (SipsterSipRequest *) sipster_allocator(sizeof(SipsterSipCallLeg));
        request->requestLine.method.method = "INVITE";
        request->requestLine.method.methodId = SIP_METHOD_INVITE;
        request->requestLine.version = (char *) "SIP/2.0";
        request->requestLine.requestUri = (char *) "sip:192.168.1.138:5061";

        SIPSTER_DEBUG("Sending msg");
        int st = sipster_sip_request_send((SipsterSipHandle *) sipster, leg, request);
        EXPECT_EQ(0, st);
        sleep(2);
    }

    return NULL;
}

TEST(DISABLED_test_sipster_udp_send, test_sipster_udp_send_1) {

    setenv("SIPSTER_LOG_LEVEL", "5", 1);

    Sipster * sipster = NULL;
    sipster_init(&sipster, NULL);
    SIPSTER_DEBUG("DONE INIT");

    pthread_t thr;
    pthread_create(&thr, NULL, threadSender, sipster);

    SIPSTER_DEBUG("STARTING LOOP");
    sipster_loop_run(sipster);

    sleep(10);

    SIPSTER_DEBUG("Ending");
}

TEST(DISABLED_test_header_finder, test_header_finder_1) {
    SipsterSipHeaderLeaf *startLeaf = NULL;
    SipsterSipHeaderLeaf *endLeaf = NULL;

    SipsterSipHeader * startHeader = (SipsterSipHeader *) sipster_allocator(sizeof(SipsterSipHeader));
    startHeader->headerId = SIP_HEADER_TO;
    SipsterSipHeader * midHeader = (SipsterSipHeader *) sipster_allocator(sizeof(SipsterSipHeader));
    midHeader->headerId = SIP_HEADER_CALL_ID;
    SipsterSipHeader * endHeader = (SipsterSipHeader *) sipster_allocator(sizeof(SipsterSipHeader));
    endHeader->headerId = SIP_HEADER_CALL_ID;

    startLeaf = endLeaf = sipster_append_new_header(endLeaf, startHeader);
    endLeaf = sipster_append_new_header(endLeaf, midHeader);
    endLeaf = sipster_append_new_header(endLeaf, endHeader);

    EXPECT_TRUE(startLeaf);
    EXPECT_TRUE(endLeaf);
    EXPECT_TRUE(startLeaf->metadata);
    EXPECT_TRUE(endLeaf->metadata);
    EXPECT_EQ(3, (int)startLeaf->metadata->count);
    EXPECT_EQ(3, (int)endLeaf->metadata->count);

    SipsterSipHeaderLeaf *l = sipster_get_header(SIP_HEADER_TO, startLeaf, endLeaf);
    EXPECT_TRUE(l);
    EXPECT_EQ(1, (int)l->metadata->count);
    EXPECT_EQ(SIP_HEADER_TO, l->header->headerId);

    l = sipster_get_header(SIP_HEADER_FROM, startLeaf, endLeaf);
    EXPECT_FALSE(l);

    l = sipster_get_header(SIP_HEADER_CALL_ID, startLeaf, endLeaf);
    EXPECT_TRUE(l);
    EXPECT_EQ(2, (int)l->metadata->count);
    EXPECT_EQ(SIP_HEADER_CALL_ID, l->header->headerId);
    EXPECT_EQ(SIP_HEADER_CALL_ID, l->next->header->headerId);
}

TEST(test_parse_addr, test_parse_addr_1) {
    SipsterSipAddress * addr1 = sipster_address_parse("Pinco Pallino <sip:pinco@pallino.com>");
    SipsterSipAddress * addr2 = sipster_address_parse("\"Pinco Pallino\" <sip:pinco@pallino.com>");
    SipsterSipAddress * addr3 = sipster_address_parse("sip:pinco@pallino.com");
    SipsterSipAddress * addr4 = sipster_address_parse("sdjgsdflkjmsdfl ksfdkljglsdkj glskdjf lksdjf lksdmjf");
    SipsterSipAddress * addr5 = sipster_address_parse("sdfds sdfsdf <sip:pinco@pallino.com:5060;skdjfsdl=sdlkjfdsl>");
    SipsterSipAddress * addr6 = sipster_address_parse("sdfds sdfsdf sip:pinco@pallino.com:5060;skdjfsdl=sdlkjfdsl>");
    SipsterSipAddress * addr7 = sipster_address_parse("sdfds sdfsdf <sip:pinco@pallino.com:5060>");

    EXPECT_TRUE(addr1);
    EXPECT_TRUE(addr2);
    EXPECT_TRUE(addr3);
    EXPECT_FALSE(addr4);
    EXPECT_TRUE(addr5);
    EXPECT_FALSE(addr6);
    EXPECT_TRUE(addr7);

    EXPECT_STREQ("sdfds sdfsdf", addr5->name);
    EXPECT_STREQ("pallino.com", addr5->domain);
    EXPECT_STREQ("sip", addr5->schema);
    EXPECT_STREQ("pinco", addr5->user);
    EXPECT_EQ(5060, addr5->port);

    EXPECT_STREQ("sdfds sdfsdf", addr7->name);
    EXPECT_STREQ("pallino.com", addr7->domain);
    EXPECT_STREQ("sip", addr7->schema);
    EXPECT_STREQ("pinco", addr7->user);
    EXPECT_EQ(5060, addr7->port);

}
