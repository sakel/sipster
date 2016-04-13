#include <sipster/sipster.h>
#include <uv.h>
#include <sipster/utils.h>
#include <sipster/log.h>
#include <sipster/sdp.h>
#include <map>

using namespace std;

typedef map<string, SipsterSipCallLeg *> tagged_legs_t;

typedef map<string,  tagged_legs_t*> call_id_t;

struct _Sipster {
    SipsterSipHandle handle;
    uv_loop_t *loop;
    uv_udp_t udp_server;
    uv_async_t asyncTrigger;
    struct sockaddr *udp_server_socket;
    call_id_t *legs;
    SipsterSipCallLeg *defaultLeg;
};

typedef struct {
    Sipster *sipster;
    char * message;
    size_t size;
    struct sockaddr *dest;
    char freeAddr;
    uv_udp_send_cb sentCallback;
} SipsterUdpJob;

SipsterSipCallLeg *sipster_sip_call_leg_create_from_str_address(Sipster *sipster, SipsterSipLegDirection direction,
                                                                const char *callId, const char *fromUri,
                                                                const char *fromTag, const char *toUri,
                                                                const char *toTag,
                                                                SipsterInetAddress *local, SipsterInetAddress *peer,
                                                                leg_request_handler requestHandler,
                                                                leg_response_handler responseHandler, void *data) {
    SipsterSipCallLeg * callLeg = (SipsterSipCallLeg *) sipster_allocator(sizeof(SipsterSipCallLeg));

    callLeg->direction = direction;

    strncpy(callLeg->callId, callId, sizeof(callLeg->callId));

    sipster_address_parse_inPlace(fromUri, &callLeg->from);
    sipster_address_parse_inPlace(toUri, &callLeg->to);

    memcpy(&callLeg->localAddress, local, sizeof(callLeg->localAddress));
    memcpy(&callLeg->peerAddress, &peer, sizeof(callLeg->peerAddress));

    if(fromTag) {
        strncpy(callLeg->fromTag, fromTag, sizeof(callLeg->fromTag)-1);
    }

    if(toTag) {
        strncpy(callLeg->toTag, toTag, sizeof(callLeg->toTag)-1);
    }

    if(!sipster->legs) {
        sipster->legs = new call_id_t();
    }

    string sCallId = callId;

    tagged_legs_t *tags = NULL;
    call_id_t::iterator tagsIterator = sipster->legs->find(sCallId);
    if(tagsIterator != sipster->legs->end()) {
        tags = tagsIterator->second;
    } else {
        tags = new tagged_legs_t();
    }
    tags->insert(pair<string, SipsterSipCallLeg *>(direction == SIP_LEG_INBOUND ? toTag : fromTag, callLeg));
    sipster->legs->insert(pair<string, tagged_legs_t *>(callId, tags));
    callLeg->requestHandler = requestHandler;
    callLeg->responseHandler = responseHandler;
    callLeg->data = data;

    return callLeg;
}

SipsterSipCallLeg *sipster_sip_call_leg_create(Sipster *sipster, SipsterSipLegDirection direction, const char *callId,
                                               SipsterSipAddress *fromUri, const char *fromTag,
                                               SipsterSipAddress *toUri, const char *toTag,
                                               SipsterInetAddress *local, SipsterInetAddress *peer,
                                               leg_request_handler requestHandler,
                                               leg_response_handler responseHandler, void *data) {
    SipsterSipCallLeg * callLeg = (SipsterSipCallLeg *) sipster_allocator(sizeof(SipsterSipCallLeg));

    callLeg->direction = direction;

    strncpy(callLeg->callId, callId, sizeof(callLeg->callId));

    memcpy(&callLeg->from, fromUri, sizeof(SipsterSipAddress));
    memcpy(&callLeg->to, toUri, sizeof(SipsterSipAddress));

    memcpy(&callLeg->localAddress, local, sizeof(callLeg->localAddress));
    memcpy(&callLeg->peerAddress, peer, sizeof(callLeg->peerAddress));

    if(fromTag) {
        strncpy(callLeg->fromTag, fromTag, sizeof(callLeg->fromTag)-1);
    }

    if(toTag) {
        strncpy(callLeg->toTag, toTag, sizeof(callLeg->toTag)-1);
    }

    if(!sipster->legs) {
        sipster->legs = new call_id_t();
    }

    string sCallId = callId;

    tagged_legs_t *tags = NULL;
    call_id_t::iterator tagsIterator = sipster->legs->find(sCallId);
    if(tagsIterator != sipster->legs->end()) {
        tags = tagsIterator->second;
    } else {
        tags = new tagged_legs_t();
    }
    tags->insert(pair<string, SipsterSipCallLeg *>(direction == SIP_LEG_INBOUND ? toTag : fromTag, callLeg));
    sipster->legs->insert(pair<string, tagged_legs_t *>(callId, tags));
    callLeg->requestHandler = requestHandler;
    callLeg->responseHandler = responseHandler;
    callLeg->data = data;

    return callLeg;
}

SipsterSipCallLeg *sipster_sip_call_leg_create_default(SipsterSipLegDirection direction, SipsterInetAddress *local,
                                                       SipsterInetAddress *peer, leg_request_handler requestHandler,
                                                       leg_response_handler responseHandler, void *data) {
    SipsterSipCallLeg *callLeg = (SipsterSipCallLeg *) sipster_allocator(sizeof(SipsterSipCallLeg));

    callLeg->requestHandler = requestHandler;
    callLeg->responseHandler = responseHandler;
    callLeg->data = data;
    memcpy(&callLeg->localAddress, local, sizeof(callLeg->localAddress));
    memcpy(&callLeg->peerAddress, &peer, sizeof(callLeg->peerAddress));

    return callLeg;
}

int sipster_get_call_leg_for_request(Sipster *sipster, SipsterSipRequest *request, SipsterSipCallLeg ** existingCallLeg, int createLeg) {
    SipsterSipCallLeg *callLeg = NULL;

    SipsterSipHeaderLeaf * callIdLeaf = sipster_sip_get_header(SIP_HEADER_CALL_ID, request->firstHeader, request->lastHeader);
    if(!callIdLeaf || callIdLeaf->metadata->count != 1) {
        SIPSTER_WARN("Invalid number of call id headers");
        return SIPSTER_RETURN_MESSAGE_ERROR;
    }
    SipsterSipHeaderCallID *callId = (SipsterSipHeaderCallID *) callIdLeaf->header;
    SIPSTER_DEBUG("Processing callId: %s", callId->data);

    string legsKey = callId->data;
    map<string, SipsterSipCallLeg *> *legs = NULL;
    if(!sipster->legs) {
        sipster->legs = new call_id_t();
    }
    call_id_t::iterator legsIterator = sipster->legs->find(legsKey);
    if(legsIterator != sipster->legs->end()) {
        legs = legsIterator->second;
    }

    SipsterSipHeaderLeaf * toLeaf = sipster_sip_get_header(SIP_HEADER_TO, request->firstHeader, request->lastHeader);
    if(!toLeaf || toLeaf->metadata->count != 1) {
        SIPSTER_WARN("Invalid number of to headers");
        return SIPSTER_RETURN_MESSAGE_ERROR;
    }
    SipsterSipHeaderTo * toHeader = (SipsterSipHeaderTo *) toLeaf->header;
    SipsterSipParameter *tag = sipster_sip_parameter_get("tag", toHeader->header.first);

    if(legs && legs->size() > 0 && tag) {
        string tagValue = tag->value;
        tagged_legs_t::iterator legIter = legs->find(tagValue);

        if(legIter != legs->end()) {
            callLeg = legIter->second;
        }
    }

    if(!callLeg && tag) {
        SIPSTER_DEBUG("No call leg found for tag");
        return SIPSTER_RETURN_CALL_NOT_FOUND;
    } else if(!callLeg) {
        if(!createLeg) {
            return SIPSTER_RETURN_CALL_NOT_FOUND;
        }
        SIPSTER_DEBUG("Creating new call leg");
        SipsterSipHeaderLeaf * fromLeaf = sipster_sip_get_header(SIP_HEADER_FROM, request->firstHeader, request->lastHeader);
        if(!fromLeaf || fromLeaf->metadata->count != 1) {
            SIPSTER_WARN("Invalid number of to headers");
            return SIPSTER_RETURN_MESSAGE_ERROR;
        }
        SipsterSipHeaderFrom * fromHeader = (SipsterSipHeaderFrom *) fromLeaf->header;
        SipsterSipParameter *fromTag = sipster_sip_parameter_get("tag", fromHeader->header.first);

        char * toTag = sipster_generate_random_string(13);
        SipsterInetAddress *localAddress = sipster_base_sockaddr_to_sipster(sipster->udp_server_socket);
        callLeg = sipster_sip_call_leg_create_from_str_address(sipster, SIP_LEG_INBOUND, callId->data, fromHeader->address, fromTag->value, toHeader->address, toTag, localAddress, request->remoteAddr, sipster->defaultLeg->requestHandler, sipster->defaultLeg->responseHandler, sipster->defaultLeg->data);
        sipster_free(localAddress);
        if(!callLeg) {
            return SIPSTER_RETURN_MESSAGE_PARSE_ERROR;
        }


    } else {
        SIPSTER_DEBUG("Found matching call leg");
    }

    *existingCallLeg = callLeg;
    return SIPSTER_RETURN_OK;
}

int sipster_get_call_leg_for_response(Sipster *sipster, SipsterSipHeaderLeaf *firstHeader, SipsterSipHeaderLeaf *lastHeader, SipsterSipCallLeg ** existingCallLeg) {
    SipsterSipCallLeg *callLeg = NULL;

    SipsterSipHeaderLeaf * callIdLeaf = sipster_sip_get_header(SIP_HEADER_CALL_ID, firstHeader, lastHeader);
    if(!callIdLeaf || callIdLeaf->metadata->count != 1) {
        SIPSTER_WARN("Invalid number of call id headers");
        return SIPSTER_RETURN_MESSAGE_ERROR;
    }
    SipsterSipHeaderCallID *callId = (SipsterSipHeaderCallID *) callIdLeaf->header;
    SIPSTER_DEBUG("Processing callId: %s", callId->data);

    string legsKey = callId->data;
    map<string, SipsterSipCallLeg *> *legs = NULL;
    if(!sipster->legs) {
        sipster->legs = new call_id_t();
    }
    call_id_t::iterator legsIterator = sipster->legs->find(legsKey);
    if(legsIterator != sipster->legs->end()) {
        legs = legsIterator->second;
    }

    SipsterSipHeaderLeaf * fromLeaf = sipster_sip_get_header(SIP_HEADER_FROM, firstHeader, lastHeader);
    if(!fromLeaf || fromLeaf->metadata->count != 1) {
        SIPSTER_WARN("Invalid number of to headers");
        return SIPSTER_RETURN_MESSAGE_ERROR;
    }
    SipsterSipHeaderFrom * fromHeader = (SipsterSipHeaderFrom *) fromLeaf->header;
    SipsterSipParameter *fromTag = sipster_sip_parameter_get("tag", fromHeader->header.first);

    if(legs && legs->size() > 0 && fromTag) {
        string tagValue = fromTag->value;
        tagged_legs_t::iterator legIter = legs->find(tagValue);

        if(legIter != legs->end()) {
            callLeg = legIter->second;
        }
    }

    if(!callLeg && fromTag) {
        SIPSTER_DEBUG("No call leg found for tag");
        return SIPSTER_RETURN_CALL_NOT_FOUND;
    } else {
        SIPSTER_DEBUG("Found matching call leg");
    }


//    SipsterSipHeaderLeaf * toLeaf = sipster_sip_get_header(SIP_HEADER_TO, firstHeader, lastHeader);
//    if(!toLeaf || toLeaf->metadata->count != 1) {
//        SIPSTER_WARN("Invalid number of to headers");
//        return SIPSTER_RETURN_MESSAGE_ERROR;
//    }
//    SipsterSipHeaderTo * toHeader = (SipsterSipHeaderTo *) toLeaf->header;
//    SipsterSipParameter *tag = sipster_sip_parameter_get("tag", toHeader->header.first);




    *existingCallLeg = callLeg;
    return SIPSTER_RETURN_OK;
}

int sipster_process_request(Sipster * sipster, SipsterSipRequest * request) {
    SipsterSipResponse *response = NULL;
    SipsterSipCallLeg *leg = NULL;

    int ret = sipster_get_call_leg_for_request(sipster, request, &leg, 1);
    if(ret) {
        //TODO handle error
        switch(ret) {
            case SIPSTER_RETURN_MESSAGE_ERROR:
            case SIPSTER_RETURN_MESSAGE_PARSE_ERROR:
                response = sipster_sip_request_create_response(SIP_SIPSTER_HANDLE(sipster), request, leg,
                                                               SIP_STATUS_400_BAD_REQUEST, NULL);
            break;
            case SIPSTER_RETURN_CALL_NOT_FOUND:
                response = sipster_sip_request_create_response(SIP_SIPSTER_HANDLE(sipster), request, leg,
                                                               SIP_STATUS_481_CALL_TRANSACTION_DOES_NOT_EXIST, NULL);
            break;
        }

        ret = sipster_sip_request_reply(SIP_SIPSTER_HANDLE(sipster), leg, response);
        if(ret != SIPSTER_RETURN_OK) {
            SIPSTER_ERROR("Could not respond to request");
        }
        return ret;
    }

    if(request->content) {
        if(strcpy(request->content->contentType, "application/sdp")) {
            //TODO parse sdp
            SipsterSdp *sdp;
            int ret;

            ret = sipster_sdp_parse(request->content->data, &sdp);
            if(ret) {
                SIPSTER_ERROR("Could not parse SDP %s", request->content->data);
                return SIPSTER_RETURN_MESSAGE_PARSE_ERROR;
            }

            leg->remoteData = sdp;
        }
    }

    if(leg->requestHandler) {
        ret = leg->requestHandler(SIP_SIPSTER_HANDLE(sipster), leg, request, leg->data);
        return ret;
    }

    SIPSTER_DEBUG("CALL LEG OK");
    response = sipster_sip_request_create_response(SIP_SIPSTER_HANDLE(sipster), request, leg, SIP_STATUS_200_OK, NULL);
    ret = sipster_sip_request_reply(SIP_SIPSTER_HANDLE(sipster), leg, response);

    sipster_free(leg);

    return SIPSTER_RETURN_OK;
}

int sipster_process_response(Sipster * sipster, SipsterSipResponse * response) {
    SipsterSipHeaderLeaf *firstHeader = response->firstHeader;
    SipsterSipHeaderLeaf *lastHeader = response->lastHeader;
    SipsterSipCallLeg *leg = NULL;

    int ret = sipster_get_call_leg_for_response(sipster, firstHeader, lastHeader, &leg);
    if(ret) {
        return ret;
    }

    SipsterSipHeaderLeaf *toLeaf = sipster_sip_get_header(SIP_HEADER_TO, response->firstHeader, response->lastHeader);

    SipsterSipHeaderTo * to = (SipsterSipHeaderTo *) toLeaf->header;
    SipsterSipParameter *tag = sipster_sip_parameter_get("tag", to->header.first);
    if(tag) {
        strncpy(leg->toTag, tag->value, sizeof(leg->toTag));
    }

    if(response->content) {
        if(strcpy(response->content->contentType, "application/sdp")) {
            //TODO parse sdp
            SipsterSdp *sdp;
            int ret;

            ret = sipster_sdp_parse(response->content->data, &sdp);
            if(ret) {
                SIPSTER_ERROR("Could not parse SDP %s", response->content->data);
                return SIPSTER_RETURN_MESSAGE_PARSE_ERROR;
            }

            leg->remoteData = sdp;
        }
    }

    if(leg->responseHandler) {
        ret = leg->responseHandler(SIP_SIPSTER_HANDLE(sipster), leg, response, leg->data);
        return ret;
    }

    return SIPSTER_RETURN_OK;
}

void sent_request_cb(uv_udp_send_t* req, int status) {
    SIPSTER_DEBUG("SENT CB: %d", status);
    sipster_free(req);
}

void sent_response_cb(uv_udp_send_t* req, int status) {
    SIPSTER_DEBUG("SENT CB: %d", status);
    sipster_free(req);
}

void outbound_udp_sending(uv_work_t* req) {
    uv_udp_send_t *sendRequest;
    uv_buf_t buffer;
    int status;
    SipsterUdpJob * jobData;
    Sipster *sipster;

    sendRequest = (uv_udp_send_t *) sipster_allocator(sizeof(uv_udp_send_t));

    jobData = (SipsterUdpJob *) req->data;
    sipster = jobData->sipster;

    buffer = uv_buf_init(jobData->message, jobData->size);
    sendRequest->data = jobData;

    status = uv_udp_send(sendRequest, &sipster->udp_server, &buffer, 1, (struct sockaddr*) jobData->dest, jobData->sentCallback);
    if(status != 0) {
        SIPSTER_DEBUG("Message sending status: %s", uv_strerror(status));
    }
}

void after_outbound_udp_sending(uv_work_t* req, int status) {
    sipster_free(req->data);
    sipster_free(req);
}

int send_request(SipsterSipHandle *sipsterHandle, SipsterSipCallLeg *leg, SipsterSipRequest *request, char * message, size_t size) {
    uv_work_t *job;
    SipsterUdpJob * jobData;
    Sipster * sipster = (Sipster *) sipsterHandle->data;
    struct sockaddr *dest;

    jobData = (SipsterUdpJob *) sipster_allocator(sizeof(SipsterUdpJob));
    jobData->sipster = sipster;
    jobData->message = message;
    jobData->size = size;
    jobData->sentCallback = sent_request_cb;

    dest = sipster_base_sipster_to_sockaddr(request->remoteAddr);

    jobData->dest = dest;
    jobData->freeAddr = 1;

    job = (uv_work_t *) sipster_allocator(sizeof(uv_work_t));
    job->data = jobData;

    SIPSTER_TRACE("SEND MSG_LEN: %d\n ===================================\n\n%s ===================================\n\n", size, message);

    uv_queue_work(sipster->loop, job, outbound_udp_sending, after_outbound_udp_sending);

    uv_async_send(&sipster->asyncTrigger);

    return SIPSTER_RETURN_OK;
}

int send_response(SipsterSipHandle *sipsterHandle, SipsterSipCallLeg *leg, SipsterSipResponse *response, char * message, size_t size) {
    uv_work_t *job;
    SipsterUdpJob * jobData;
    Sipster * sipster = (Sipster *) sipsterHandle->data;

    jobData = (SipsterUdpJob *) sipster_allocator(sizeof(SipsterUdpJob));
    jobData->sipster = sipster;
    jobData->message = message;
    jobData->size = size;
    jobData->freeAddr = 0;
    jobData->dest = sipster_base_sipster_to_sockaddr(response->remoteAddr);
    jobData->sentCallback = sent_response_cb;

    job = (uv_work_t *) sipster_allocator(sizeof(uv_work_t));
    job->data = jobData;

    SIPSTER_TRACE("SEND MSG_LEN: %d\n ===================================\n\n%s ===================================\n\n", size, message);

    uv_queue_work(sipster->loop, job, outbound_udp_sending, after_outbound_udp_sending);

    uv_async_send(&sipster->asyncTrigger);

    return SIPSTER_RETURN_OK;
}

int sipster_sip_message_handler(Sipster * sipster, SipsterInetAddress *addr, char * data, size_t size) {
    int parseStatus = SIPSTER_RETURN_MESSAGE_ERROR;
    SIPSTER_TRACE("RECV MSG_LEN: %d\n ===================================\n\n%s ===================================\n\n", size, data);

    //NOT SURE if this is the best solution
    if(strncmp(data, SIP_PROTOCOL_AND_TRANSPORT, 4) == 0) {
        SIPSTER_TRACE("Probably a Response message");
        SipsterSipResponse * response = sipster_sip_response_parse(data, size, &parseStatus);
        response->remoteAddr = addr;
        if(parseStatus < SIPSTER_RETURN_OK) {
            SIPSTER_ERROR("Error parsing message - garbage");
            goto end;
        }
        sipster_free(data);
        parseStatus = sipster_process_response(sipster, response);
    } else {
        SIPSTER_TRACE("Probably a Request message");
        SipsterSipRequest * request = sipster_sip_request_parse(data, size, &parseStatus);
        request->remoteAddr = addr;
        if(parseStatus < SIPSTER_RETURN_OK) {
            SIPSTER_ERROR("Error parsing message - garbage");
            goto end;
        }
        sipster_free(data);
        parseStatus = sipster_process_request(sipster, request);
    }

end:
    return parseStatus;
}

void udp_receive(uv_udp_t* handle, ssize_t nread, const uv_buf_t* buf, const struct sockaddr* addr, unsigned flags) {
    SIPSTER_TRACE("Receiving");

    SipsterInetAddress *inet = sipster_base_sockaddr_to_sipster((sockaddr *) addr);

    if(nread < 1) {
        return;
    }

    Sipster * sipster = (Sipster *) handle->data;

    if((unsigned int) nread < buf->len) {
        buf->base[nread] = 0;
    }

    //TODO check result
    int status = sipster_sip_message_handler(sipster, inet, buf->base, nread);
    if(status < 0) {
        SIPSTER_ERROR("Message error");
    }
}

void udp_allocation(uv_handle_t* handle, size_t suggested_size, uv_buf_t* buf) {
    SIPSTER_TRACE("Allocation %d", suggested_size);
    if(buf) {
        buf->base = (char *) sipster_allocator(suggested_size);
        buf->len = suggested_size;
    }
}

void async_callback(uv_async_t *handle) {

}

int sipster_init(Sipster ** sipsterPtr, SipsterSipCallLeg *defaultLeg) {
    static SipsterSipCallLeg staticDefaultLeg = {SIP_LEG_INBOUND,
                                                 NULL_STRING,
                                                 SIP_ADDRESS_NONE,
                                                 NULL_STRING,
                                                 SIP_ADDRESS_NONE,
                                                 NULL_STRING,
                                                 {SIPSTER_IPV4, 0, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, NULL_STRING},
                                                 {SIPSTER_IPV4, 0, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, NULL_STRING},
                                                 NULL,
                                                 NULL,
                                                 NULL};
    uv_loop_t * loop;
    struct sockaddr_in *ipv4Socket;
    int status = SIPSTER_RETURN_OK;
    if(!*sipsterPtr) {
        Sipster * sipster;
        sipster = (Sipster *) sipster_allocator(sizeof(Sipster));
        loop = (uv_loop_t *) sipster_allocator(sizeof(uv_loop_t));
        sipster->handle.data = (void *) sipster;
        sipster->handle.sendRequest = send_request;
        sipster->handle.sendResponse = send_response;
        sipster->defaultLeg = defaultLeg ? defaultLeg : &staticDefaultLeg;
        sipster->legs = new map<string, map<string, SipsterSipCallLeg *>*>();

        sipster->loop = loop = uv_default_loop();
        //uv_loop_init(loop);

        status = uv_udp_init(loop, &sipster->udp_server);
        if(status) {
            SIPSTER_ERROR("Could not init UDP server: %s", uv_strerror(status));
            status = -1;
            goto end_new_instance;
        }

        // Passing the handle to the main sipster instance object
        sipster->udp_server.data = (void *) sipster;

        //TODO Export to api

        ipv4Socket = (sockaddr_in *) sipster_allocator(sizeof(struct sockaddr_in));
        status = uv_ip4_addr("0.0.0.0", 5060, ipv4Socket);
        if(status) {
            SIPSTER_ERROR("Could not init UDP server: %s", uv_strerror(status));
            status = -1;
            goto end_new_instance;
        }
        sipster->udp_server_socket = (sockaddr *) ipv4Socket;

        status = uv_udp_bind(&sipster->udp_server, sipster->udp_server_socket, UV_UDP_REUSEADDR);
        if(status) {
            SIPSTER_ERROR("Could not bind port for UDP server: %s", uv_strerror(status));
            status = -1;
            goto end_new_instance;
        }

        status = uv_udp_recv_start(&sipster->udp_server, udp_allocation, udp_receive);
        if(status) {
            SIPSTER_ERROR("Could not start reception on UDP server: %s", uv_strerror(status));
            status = -1;
            goto end_new_instance;
        }

        status = uv_async_init(loop, &sipster->asyncTrigger, async_callback);

        *sipsterPtr = sipster;
        SIPSTER_INFO("Initialized SIPSTER");
        return status;

end_new_instance:
        sipster_free(sipster);
        sipster_free(loop);
        return status;
    }
    SIPSTER_INFO("SIPSTER already initialized");

    return -1;
}

void sipster_set_rport(Sipster *sipster, unsigned int rport) {
    sipster->handle.rport = rport;
}

int sipster_loop_run(Sipster * sipster) {
    return uv_run(sipster->loop, UV_RUN_DEFAULT);
}

void sipster_loop_end(Sipster * sipster) {
    uv_udp_recv_stop(&sipster->udp_server);
    uv_loop_close(sipster->loop);
}

void sipster_deinit(Sipster * sipster) {
    SIPSTER_INFO("DeInitializing SIPSTER");
    if(sipster) {
        if(sipster->loop)
            sipster_free(sipster->loop);
        if(sipster->defaultLeg)
            sipster_free(sipster->defaultLeg);
        if(sipster->legs) {
            if(sipster->legs->size() > 0) {
                typedef call_id_t::iterator it_type;
                typedef tagged_legs_t::iterator it_it_type;
                for(it_type iterator = sipster->legs->begin(); iterator != sipster->legs->end(); iterator++) {

                    tagged_legs_t *callLegs = iterator->second;
                    for(it_it_type iterator2 = callLegs->begin(); iterator2 != callLegs->end(); iterator2++) {
                        if(iterator2->second) {
                            sipster_free(iterator2->second);
                        }
                    }
                    delete callLegs;
                }
            }
            delete sipster->legs;
        }
        sipster_free(sipster);
    }
}


