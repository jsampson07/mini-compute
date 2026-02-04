// I want to write tests for net.cpp (send_all/recv_all)
#include <cassert>
#include "net.h"
#include "log.h"
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <cstring>
#include <cstdio> // perror
#include <cstdlib> //exit

static void test_send_recv_all() {
    logutil::info("Test A: basic send_all/recv_all");

    int fds[2];

    if (socketpair(AF_UNIX, SOCK_STREAM, 0, fds) == -1) {
        perror("Socketpair failed.");
        exit(EXIT_FAILURE);
    }

    char *send_data = "Hello, my name is Joshua";
    size_t n = strlen(send_data);
    bool success = send_all(fds[0], send_data, n);
    assert(success);

    // Enough space for receiving data
    char recv_buf[128];
    // This is good practice to initialize buf with \0 because if working with it later, will not have unpredictable/hard to catch bugs with no null term. ending
    memset(recv_buf, 0, 128);
    success = recv_all(fds[1], recv_buf, n);
    assert(success);

    assert(memcmp(send_data, recv_buf, n) == 0);
}

/**
 * This will test sending data in partial chunks and seeing if it works
 */
static void test_send_recv_all_partial() {
    // Let's first make it clear what test we are running and what we are testing for
    logutil::info("Test B: recv_all handles chunked sends");
    int fds[2];
    if (socketpair(AF_UNIX, SOCK_STREAM, 0, fds) == -1) {
        perror("Socketpair failed.");
        exit(EXIT_FAILURE);
    }
    const int payload_sz = 100;
    char send_buf[payload_sz];
    for (int i = 0; i < payload_sz; i++) send_buf[i] = (char) ('A' + (i % 26));

    // Now we want to send in chunks using raw send() to increase chance of multiple receive to test buffer writes
    ssize_t s1 = send(fds[0], send_buf, 7, 0);
    // What could fail? send()? so we want to see if it sent all the data
    assert (s1 == 7);
    ssize_t s2 = send(fds[0], send_buf + 7, 13, 0);
    assert(s2 == 13);
    ssize_t s3 = send(fds[0], send_buf + 20, payload_sz - 20, 0);
    assert(s3 == (payload_sz-20));

    char recv_buf[payload_sz];
    memset(recv_buf, 0, sizeof(recv_buf));

    bool success = recv_all(fds[1], recv_buf, payload_sz);
    assert(success);

    assert(memcmp(recv_buf, send_buf, payload_sz) == 0);

    close(fds[0]);
    close(fds[1]);
}

int main() {
    logutil::info("running TEST FOR net.cpp");
    test_send_recv_all();
    test_send_recv_all_partial();
    logutil::info("all net tests passed");
}