#ifndef SIPSTER_H
#define SIPSTER_H

#include <sipster/sip.h>

#define SIPSTER(s) (Sipster *) s

typedef struct _Sipster Sipster;

int sipster_init(Sipster ** sipster, SipsterSipCallLeg *defaultLeg);
int sipster_loop_run(Sipster * sipster);
void sipster_loop_end(Sipster * sipster);
void sipster_deinit(Sipster * sipster);

SipsterSipCallLeg * sipster_sip_call_leg_create(Sipster *sipster, SipsterSipLegDirection direction, const char * callId, const char * fromUri, const char * fromTag, const char *toUri, const char *toTag,
                                                leg_request_handler requestHandler, leg_response_handler responseHandler, void *data);
void sipster_sip_call_leg_destroy(SipsterSipCallLeg *leg);



#endif // SIPSTER_H

