#include <sipster/sip.h>
#include <string>
#include <iostream>
#include <sipster/sipster.h>
#include <cstdlib>
#include <sipster/log.h>
#include <unistd.h>
#include <sipster/utils.h>

using namespace std;

int request_handler(SipsterSipHandle * sipsterHandle, SipsterSipCallLeg * leg, SipsterSipRequest *request, void *data);
int response_handler(SipsterSipHandle * sipsterHandle, SipsterSipCallLeg * leg, SipsterSipResponse *response, void *data);


static const char * sdp = "v=0\r\n"
        "o=bla 123456 123456 IN IP4 %s\r\n"
        "i=A Seminar on the session description protocol\r\n"
        "c=IN IP4 %s\r\n"
        "t=0 0\r\n"
        "m=audio %d RTP/AVP 8\r\n"
        "a=rtpmap:8 PCMA/%d\r\n"
        "m=video %d RTP/AVP 96\r\n"
        "a=rtpmap:96 H264/%d\r\n\r\n";

void *threadSender(void *data) {
//    int i = 0;
    Sipster *sipster = (Sipster *)data;

    sleep(20);
    //for(i = 0; i < 100; i++) {


        SipsterSipCallLeg *leg;
//        memset(&leg, 0, sizeof(leg));
        leg = sipster_sip_call_leg_create_from_str_address(sipster, SIP_LEG_OUTBOUND, sipster_generate_uuid(),
                                          "sip:me@me.com", sipster_generate_random_string(13),
                                          "sip:you@you.com", NULL,
                                          request_handler, response_handler, sipster);
        SipsterSipRequest *request;
//        memset(&request, 0, sizeof(request));
        request = sipster_sip_request_create();
        request->requestLine.method.method = method_names[SIP_METHOD_INVITE];
        request->requestLine.method.methodId = SIP_METHOD_INVITE;
        request->requestLine.version = SIP_PROTOCOL;
        request->requestLine.requestUri = (char *) "sip:192.168.1.138:5061";

        SipsterSipHeaderVia * via = SIP_HEADER_WITH_PARAMS_CREATE(SIP_HEADER_VIA, SipsterSipHeaderVia);
        via->protocol = SIP_PROTOCOL_AND_TRANSPORT;
        SIP_HEADER_FIELD_COPY(via->address, "192.168.1.138:5060");
//        SipsterSipParameter *rport = sipster_sip_parameter_create("rport", "");
//        sipster_sip_header_append_parameter((SipsterSipHeaderWithParams *) via, rport);
        SipsterSipParameter *branch = sipster_sip_parameter_create("branch", SIP_HEADER_BRANCH_CREATE());
        sipster_sip_header_append_parameter((SipsterSipHeaderWithParams *) via, branch);

        SipsterSipHeaderFrom * from = SIP_HEADER_WITH_PARAMS_CREATE(SIP_HEADER_FROM, SipsterSipHeaderFrom);
        SIP_HEADER_FIELD_COPY(from->address, "sip:me@me.com");
        SipsterSipParameter *fromTag = sipster_sip_parameter_create("tag", leg->fromTag);
        sipster_sip_header_append_parameter((SipsterSipHeaderWithParams *) from, fromTag);

        SipsterSipHeaderTo *to = SIP_HEADER_WITH_PARAMS_CREATE(SIP_HEADER_TO, SipsterSipHeaderTo);
        SIP_HEADER_FIELD_COPY(to->address, "sip:you@you.com");

        SipsterSipHeaderCSeq *cseq = SIP_HEADER_CREATE(SIP_HEADER_CSEQ, SipsterSipHeaderCSeq);
        cseq->methodId = SIP_METHOD_INVITE;
        cseq->seq = 1l;
        cseq->requestMethod = SIP_METHOD_NAME(SIP_METHOD_INVITE);

        SipsterSipHeaderCallID * callId = SIP_HEADER_CREATE(SIP_HEADER_CALL_ID, SipsterSipHeaderCallID);
        SIP_HEADER_FIELD_COPY(callId->data, leg->callId);

        SipsterSipHeaderContact * contact = SIP_HEADER_CREATE(SIP_HEADER_CONTACT, SipsterSipHeaderContact);
        SIP_HEADER_FIELD_COPY(contact->address, "<sip:me@192.168.1.138:5061>");

        SipsterSipHeaderUserAgent * ua = SIP_HEADER_CREATE(SIP_HEADER_USER_AGENT, SipsterSipHeaderUserAgent);
        SIP_HEADER_FIELD_COPY(ua->data, "Test UA");

        SipsterSipHeaderMaxFwds * mf = SIP_HEADER_CREATE(SIP_HEADER_MAX_FORWARDS, SipsterSipHeaderMaxFwds);
        mf->number = 10;


        char final_sdp[1000];

        snprintf(final_sdp, sizeof(final_sdp), sdp, "192.168.1.138", "192.168.1.138", 3000, 8000, 3002, 90000);


        SipsterSipHeaderContentLength * cl = SIP_HEADER_CREATE(SIP_HEADER_CONTENT_LENGTH, SipsterSipHeaderContentLength);
        cl->number = strlen(final_sdp)+2;

        SipsterSipContentBody * body;
        int len = strlen(final_sdp)+2;
        SIP_BODY_CREATE(body, "application/sdp", final_sdp, len);

        request->firstHeader = request->lastHeader = sipster_sip_append_new_header(request->lastHeader, SIP_HEADER(via));
        request->lastHeader = sipster_sip_append_new_header(request->lastHeader, SIP_HEADER(from));
        request->lastHeader = sipster_sip_append_new_header(request->lastHeader, SIP_HEADER(to));
        request->lastHeader = sipster_sip_append_new_header(request->lastHeader, SIP_HEADER(cseq));
        request->lastHeader = sipster_sip_append_new_header(request->lastHeader, SIP_HEADER(callId));
        request->lastHeader = sipster_sip_append_new_header(request->lastHeader, SIP_HEADER(contact));
        request->lastHeader = sipster_sip_append_new_header(request->lastHeader, SIP_HEADER(ua));
        request->lastHeader = sipster_sip_append_new_header(request->lastHeader, SIP_HEADER(mf));
        request->lastHeader = sipster_sip_append_new_header(request->lastHeader, SIP_HEADER(cl));
        request->content = body;

/*
REGISTER sip:192.168.1.138 SIP/2.0
Via: SIP/2.0/UDP 192.168.1.138:5061;rport;branch=z9hG4bK8K9751K7ga81r
Max-Forwards: 70
From: <sip:dejan@liblab.si>;tag=8r6cyFgSD32KK
To: <sip:test@liblab.si>
Call-ID: 3bff6fef-4499-1234-a59e-78acc03daec5
CSeq: 915859595 REGISTER
Contact: <sip:dejan@192.168.1.138:5061>
User-Agent: LTek client sofia-sip/1.12.11devel
Allow: INVITE, TRYING, RINGING, OK, ACK, BYE, CANCEL, OPTIONS, PRACK, MESSAGE, UPDATE
Supported: timer, 100rel, path
Content-Length: 0
*/


        SIPSTER_DEBUG("Sending msg");
    sipster_sip_request_send((SipsterSipHandle *) sipster, leg, request);
//        sleep(2);
//}

    return NULL;
}

int main(int argc, char ** argv) {
    static SipsterSipCallLeg defaultLeg = {SIP_LEG_INBOUND, NULL_STRING,
                                                 {"sip", "Test", "test", "liblab.si", 5060},
                                                 NULL_STRING, SIP_ADDRESS_NONE, NULL_STRING, {SIPSTER_IPV4, 0, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}}, request_handler, response_handler, NULL};

    setenv("SIPSTER_LOG_LEVEL", "5", 1);

    Sipster * sipster = NULL;
    //SipsterSipCallLeg * defaultLeg = sipster_sip_call_leg_create(NULL, "1", "sip:this@local.uri", "", "", "", request_handler, response_handler, NULL);
    sipster_init(&sipster, &defaultLeg);

    SIPSTER_DEBUG("DONE INIT");

    pthread_t thr;
    pthread_create(&thr, NULL, threadSender, sipster);

    SIPSTER_DEBUG("STARTING LOOP");
    sipster_loop_run(sipster);

    sleep(10);

    SIPSTER_DEBUG("Ending");

    return 0;
}

int request_handler(SipsterSipHandle * sipsterHandle, SipsterSipCallLeg * leg, SipsterSipRequest *request, void *data) {
    SipsterSipResponse * response = NULL;
    int ret = 0;

    SIPSTER_DEBUG("GOT REQUEST OOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOO");
    switch(request->requestLine.method.methodId) {
        case SIP_METHOD_REGISTER:
        response = sipster_sip_request_create_response(sipsterHandle, request, leg, SIP_STATUS_200_OK, NULL);
        ret = sipster_sip_request_reply(sipsterHandle, leg, response);
        if(ret) {
            SIPSTER_ERROR("ERROR Sending reply");
        }
        break;
    default:
        break;
    }

    return SIPSTER_RETURN_OK;
}

int response_handler(SipsterSipHandle * sipsterHandle, SipsterSipCallLeg * leg, SipsterSipResponse *response, void *data) {
    SIPSTER_DEBUG("GOT RESPONSE AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA");
    switch (response->responseLine.status.status) {
    case SIP_STATUS_200_OK:
        SIPSTER_DEBUG("IT'S OK");
        break;
    default:
        SIPSTER_DEBUG("ANYTHING");
        break;
    }

    return SIPSTER_RETURN_OK;
}
