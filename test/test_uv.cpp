#include <uv.h>

void sent_request_cb(uv_udp_send_t* req, int status) {
    printf("SENT CB: %d\n", status);
    free(req);
}

void *sendPacket(void *data) {

    uv_udp_t *handle = (uv_udp_t *) data;
    int i = 0;

    for(i = 0; i < 100; i++) {

        uv_udp_send_t *send_request;
        uv_buf_t buffer;

        sleep(5);

        printf("Sending message\n");

        send_request = (uv_udp_send_t *) calloc(1, sizeof(uv_udp_send_t));
        buffer = uv_buf_init((char *)"message\n", 8);

        struct sockaddr_in dest;

        int status = uv_ip4_addr("192.168.1.138", 5001, &dest);
        if(status != 0) {
            printf("MessageA: %s\n", uv_strerror(status));
        }

        status = uv_udp_send(send_request, handle, &buffer, 1, (struct sockaddr*) &dest, sent_request_cb);
        //status = uv_udp_try_send(handle, &buffer, 1, (struct sockaddr*) &dest);
        if(status != 0) {
            printf("MessageB: %s\n", uv_strerror(status));
        }

        uv_async_send(&as);
    }

    return NULL;
}

void udp_receive(uv_udp_t* handle, ssize_t nread, const uv_buf_t* buf, const struct sockaddr* addr, unsigned flags) {
    printf("Receiving\n");

    if(nread < 1) {
        return;
    }
}

void udp_allocation(uv_handle_t* handle, size_t suggested_size, uv_buf_t* buf) {
    printf("Allocation %lu\n", suggested_size);
    if(buf) {
        buf->base = (char *) calloc(1, suggested_size);
        buf->len = suggested_size;
    }
}

void async_cb(uv_async_t* handle) {

}

TEST(DISABLED_test_uv_udp, test_uv_udp_1) {
    uv_loop_t * loop;
    uv_udp_t handle;
    struct sockaddr_in udp_addr;
    int status;

    loop = uv_default_loop();

    status = uv_udp_init(loop, &handle);
    if(status) {
        printf("Could not init UDP server: %s\n", uv_strerror(status));
    }

    status = uv_ip4_addr("0.0.0.0", 5000, &udp_addr);
    if(status) {
        printf("Could not init UDP server: %s\n", uv_strerror(status));
    }

    status = uv_udp_bind(&handle, (struct sockaddr *) &udp_addr, UV_UDP_REUSEADDR);
    if(status) {
        printf("Could not bind port for UDP server: %s\n", uv_strerror(status));
    }

    status = uv_udp_recv_start(&handle, udp_allocation, udp_receive);
    if(status) {
        printf("Could not start reception on UDP server: %s\n", uv_strerror(status));
    }

    uv_async_init(loop, &as, async_cb);

    pthread_t th;
    pthread_create(&th, NULL, sendPacket, &handle);

    uv_run(loop, UV_RUN_DEFAULT);
}

