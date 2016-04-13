#ifndef SIPSTER_H
#define SIPSTER_H

#include <sipster/base.h>
#include <sipster/sip.h>
#include <sipster/sip_enums.h>
#include <sipster/sip_headers.h>
#include <sipster/sdp.h>
#include <sipster/sdp_enums.h>
#include <sipster/sdp_headers.h>

#define SIPSTER(s) (Sipster *) s

int sipster_init(Sipster **sipster, SipsterSipCallLeg *defaultLeg);

int sipster_loop_run(Sipster *sipster);

void sipster_loop_end(Sipster *sipster);

void sipster_deinit(Sipster *sipster);

SipsterSipRequest *sipster_sip_request_create_template(SipsterSipCallLeg *leg, SipsterSipMethodEnum method,
                                                       char *content, const char *contentType, unsigned int contentLen);

SipsterSipCallLeg *sipster_sip_call_leg_create(Sipster *sipster, SipsterSipLegDirection direction, const char *callId,
                                               SipsterSipAddress *fromUri, const char *fromTag,
                                               SipsterSipAddress *toUri, const char *toTag,
                                               SipsterInetAddress *local, SipsterInetAddress *peer,
                                               leg_request_handler requestHandler,
                                               leg_response_handler responseHandler, void *data);

SipsterSipCallLeg *sipster_sip_call_leg_create_from_str_address(Sipster *sipster, SipsterSipLegDirection direction,
                                                                const char *callId, const char *fromUri,
                                                                const char *fromTag, const char *toUri,
                                                                const char *toTag,
                                                                SipsterInetAddress *local, SipsterInetAddress *peer,
                                                                leg_request_handler requestHandler,
                                                                leg_response_handler responseHandler, void *data);

SipsterSipCallLeg *sipster_sip_call_leg_create_default(SipsterSipLegDirection direction, SipsterInetAddress *local,
                                                       SipsterInetAddress *peer, leg_request_handler requestHandler,
                                                       leg_response_handler responseHandler, void *data);

void sipster_sip_call_leg_destroy(SipsterSipCallLeg *leg);

#endif // SIPSTER_H

