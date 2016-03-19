#include <sipster/sipster.h>
#include <uv.h>
#include <utils.h>
#include <sipster/log.h>
#include <stdio.h>
#include <string.h>
#include <sipster/sip.h>
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


int sipster_get_call_leg_for_request(Sipster *sipster, SipsterSipHeaderLeaf *firstHeader, SipsterSipHeaderLeaf *lastHeader, SipsterSipCallLeg ** existingCallLeg, int createLeg) {
    SipsterSipCallLeg *callLeg = NULL;

    SipsterSipHeaderLeaf * callIdLeaf = sipster_get_header(SIP_HEADER_CALL_ID, firstHeader, lastHeader);
    if(!callIdLeaf || callIdLeaf->metadata->count != 1) {
        SIPSTER_WARN("Invalid number of call id headers");
        return MESSAGE_ERROR;
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

    SipsterSipHeaderLeaf * toLeaf = sipster_get_header(SIP_HEADER_TO, firstHeader, lastHeader);
    if(!toLeaf || toLeaf->metadata->count != 1) {
        SIPSTER_WARN("Invalid number of to headers");
        return MESSAGE_ERROR;
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
        return CALL_NOT_FOUND;
    } else if(!callLeg) {
        if(!createLeg) {
            return CALL_NOT_FOUND;
        }
        SIPSTER_DEBUG("Creating new call leg");
        SipsterSipHeaderLeaf * fromLeaf = sipster_get_header(SIP_HEADER_FROM, firstHeader, lastHeader);
        if(!fromLeaf || fromLeaf->metadata->count != 1) {
            SIPSTER_WARN("Invalid number of to headers");
            return MESSAGE_ERROR;
        }
        SipsterSipHeaderFrom * fromHeader = (SipsterSipHeaderFrom *) fromLeaf->header;
        SipsterSipParameter *fromTag = sipster_sip_parameter_get("tag", fromHeader->header.first);

        char * toTag = sipster_generate_random_string(13);
        callLeg = sipster_sip_call_leg_create(sipster, SIP_LEG_INBOUND, callId->data, fromHeader->address, fromTag->value, toHeader->address, toTag, sipster->defaultLeg->requestHandler, sipster->defaultLeg->responseHandler, sipster->defaultLeg->data);
        if(!callLeg) {
            return MESSAGE_PARSE_ERROR;
        }


    } else {
        SIPSTER_DEBUG("Found matching call leg");
    }

    *existingCallLeg = callLeg;
    return OK;
}

int sipster_get_call_leg_for_response(Sipster *sipster, SipsterSipHeaderLeaf *firstHeader, SipsterSipHeaderLeaf *lastHeader, SipsterSipCallLeg ** existingCallLeg) {
    SipsterSipCallLeg *callLeg = NULL;

    SipsterSipHeaderLeaf * callIdLeaf = sipster_get_header(SIP_HEADER_CALL_ID, firstHeader, lastHeader);
    if(!callIdLeaf || callIdLeaf->metadata->count != 1) {
        SIPSTER_WARN("Invalid number of call id headers");
        return MESSAGE_ERROR;
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

    SipsterSipHeaderLeaf * fromLeaf = sipster_get_header(SIP_HEADER_FROM, firstHeader, lastHeader);
    if(!fromLeaf || fromLeaf->metadata->count != 1) {
        SIPSTER_WARN("Invalid number of to headers");
        return MESSAGE_ERROR;
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
        return CALL_NOT_FOUND;
    } else {
        SIPSTER_DEBUG("Found matching call leg");
    }


//    SipsterSipHeaderLeaf * toLeaf = sipster_get_header(SIP_HEADER_TO, firstHeader, lastHeader);
//    if(!toLeaf || toLeaf->metadata->count != 1) {
//        SIPSTER_WARN("Invalid number of to headers");
//        return MESSAGE_ERROR;
//    }
//    SipsterSipHeaderTo * toHeader = (SipsterSipHeaderTo *) toLeaf->header;
//    SipsterSipParameter *tag = sipster_sip_parameter_get("tag", toHeader->header.first);




    *existingCallLeg = callLeg;
    return OK;
}

int sipster_process_request(Sipster * sipster, SipsterSipRequest * request) {
    SipsterSipResponse *response = NULL;
    SipsterSipHeaderLeaf *firstHeader = request->firstHeader;
    SipsterSipHeaderLeaf *lastHeader = request->lastHeader;
    SipsterSipCallLeg *leg = NULL;

    int ret = sipster_get_call_leg_for_request(sipster, firstHeader, lastHeader, &leg, 1);
    if(ret) {
        //TODO handle error
        switch(ret) {
            case MESSAGE_ERROR:
            case MESSAGE_PARSE_ERROR:
                response = sipster_request_create_response(SIP_SIPSTER_HANDLE(sipster), request, leg, SIP_STATUS_400_BAD_REQUEST, NULL);
            break;
            case CALL_NOT_FOUND:
                response = sipster_request_create_response(SIP_SIPSTER_HANDLE(sipster), request, leg, SIP_STATUS_481_CALL_TRANSACTION_DOES_NOT_EXIST, NULL);
            break;
        }

        ret = sipster_request_reply(SIP_SIPSTER_HANDLE(sipster), leg, response);
        if(ret != OK) {
            SIPSTER_ERROR("Could not respond to request");
        }
        return ret;
    }

    if(leg->requestHandler) {
        ret = leg->requestHandler(SIP_SIPSTER_HANDLE(sipster), leg, request, leg->data);
        return ret;
    }

    SIPSTER_DEBUG("CALL LEG OK");
    response = sipster_request_create_response(SIP_SIPSTER_HANDLE(sipster), request, leg, SIP_STATUS_200_OK, NULL);
    ret = sipster_request_reply(SIP_SIPSTER_HANDLE(sipster), leg, response);

    sipster_free(leg);

    return OK;
}

int sipster_process_response(Sipster * sipster, SipsterSipResponse * response) {
    SipsterSipHeaderLeaf *firstHeader = response->firstHeader;
    SipsterSipHeaderLeaf *lastHeader = response->lastHeader;
    SipsterSipCallLeg *leg = NULL;

    int ret = sipster_get_call_leg_for_response(sipster, firstHeader, lastHeader, &leg);
    if(ret) {
        return ret;
    }

    if(leg->responseHandler) {
        ret = leg->responseHandler(SIP_SIPSTER_HANDLE(sipster), leg, response, leg->data);
        return ret;
    }

    return OK;
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
    struct sockaddr_in *dest;

    jobData = (SipsterUdpJob *) sipster_allocator(sizeof(SipsterUdpJob));
    jobData->sipster = sipster;
    jobData->message = message;
    jobData->size = size;
    jobData->sentCallback = sent_request_cb;

    dest = (struct sockaddr_in *) sipster_allocator(sizeof(struct sockaddr_in));
    int status = uv_ip4_addr("192.168.1.138", 5061, dest);
    if(status != 0) {
        SIPSTER_DEBUG("Message: %s", uv_strerror(status));
    }
    jobData->dest = (struct sockaddr *) dest;
    jobData->freeAddr = 1;

    job = (uv_work_t *) sipster_allocator(sizeof(uv_work_t));
    job->data = jobData;

    SIPSTER_TRACE("SEND MSG_LEN: %d\n ===================================\n\n%s ===================================\n\n", size, message);

    uv_queue_work(sipster->loop, job, outbound_udp_sending, after_outbound_udp_sending);

    uv_async_send(&sipster->asyncTrigger);

    return OK;
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
    jobData->dest = (struct sockaddr *) response->remoteAddr;
    jobData->sentCallback = sent_response_cb;

    job = (uv_work_t *) sipster_allocator(sizeof(uv_work_t));
    job->data = jobData;

    SIPSTER_TRACE("SEND MSG_LEN: %d\n ===================================\n\n%s ===================================\n\n", size, message);

    uv_queue_work(sipster->loop, job, outbound_udp_sending, after_outbound_udp_sending);

    uv_async_send(&sipster->asyncTrigger);

    return OK;
}

int sipster_sip_message_handler(Sipster * sipster, const struct sockaddr* addr, char * data, size_t size) {
    int parseStatus = MESSAGE_ERROR;
    SIPSTER_TRACE("RECV MSG_LEN: %d\n ===================================\n\n%s ===================================\n\n", size, data);

    //NOT SURE if this is the best solution
    if(strncmp(data, SIP_PROTOCOL_AND_TRANSPORT, 4) == 0) {
        SIPSTER_TRACE("Probably a Response message");
        SipsterSipResponse * response = sipster_response_parse(data, size, &parseStatus);
        response->remoteAddr = addr;
        if(parseStatus < OK) {
            SIPSTER_ERROR("Error parsing message - garbage");
            goto end;
        }
        sipster_free(data);
        parseStatus = sipster_process_response(sipster, response);
    } else {
        SIPSTER_TRACE("Probably a Request message");
        SipsterSipRequest * request = sipster_request_parse(data, size, &parseStatus);
        request->remoteAddr = addr;
        if(parseStatus < OK) {
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

    if(nread < 1) {
        return;
    }

    Sipster * sipster = (Sipster *) handle->data;

    if((unsigned int) nread < buf->len) {
        buf->base[nread] = 0;
    }

    //TODO check result
    int status = sipster_sip_message_handler(sipster, addr, buf->base, nread);
    if(status < 0) {
        SIPSTER_ERROR("Message error");
    }

    //sipster_free(buf->base);
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
                                                 NULL,
                                                 NULL,
                                                 NULL};
    uv_loop_t * loop;
    int status = OK;
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
        status = uv_ip4_addr("0.0.0.0", 5060, (struct sockaddr_in *) &sipster->udp_server_socket);
        if(status) {
            SIPSTER_ERROR("Could not init UDP server: %s", uv_strerror(status));
            status = -1;
            goto end_new_instance;
        }

        status = uv_udp_bind(&sipster->udp_server, (struct sockaddr *) &sipster->udp_server_socket, UV_UDP_REUSEADDR);
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

SipsterSipCallLeg * sipster_sip_call_leg_create(Sipster *sipster, SipsterSipLegDirection direction, const char * callId, const char * fromUri, const char * fromTag, const char *toUri, const char *toTag,
                                                leg_request_handler requestHandler, leg_response_handler responseHandler, void *data) {
    SipsterSipCallLeg * callLeg = (SipsterSipCallLeg *) sipster_allocator(sizeof(SipsterSipCallLeg));

    callLeg->direction = direction;

    strncpy(callLeg->callId, callId, sizeof(callLeg->callId));

    sipster_address_parse_inPlace(fromUri, &callLeg->from);
    sipster_address_parse_inPlace(toUri, &callLeg->to);

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

SipsterSipCallLeg * sipster_sip_call_leg_create_default(SipsterSipLegDirection direction, leg_request_handler requestHandler, leg_response_handler responseHandler, void *data) {
    SipsterSipCallLeg * callLeg = (SipsterSipCallLeg *) sipster_allocator(sizeof(SipsterSipCallLeg));

    callLeg->requestHandler = requestHandler;
    callLeg->responseHandler = responseHandler;
    callLeg->data = data;

    return callLeg;
}
