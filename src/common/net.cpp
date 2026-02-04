// Be able to send_all and recv_all after a connection is established

#include "net.h"
#include <cerrno>
#include <cstring>
#include <sys/socket.h> // send, recv
#include <unistd.h> // close (to close the connection)

/**
 * @param fd: connection
 * @param data: data to write
 * @param len: length of data
 * @return true (success) false(failed)
 */
bool send_all(int fd, const void *data, size_t len) {
    const char *pckt = (const char *) (data);
    size_t sent = 0;
    // The problem is with send() we may not send all packets, so we must see how much we have sent and see if it is == len
    while (sent < len) {
        ssize_t n = send(fd, data, len-sent, 0);
        if (n > 0) {
            sent += size_t(n);
        } else if (n == 0) {
            return false; // If somehow we send 0 bytes, we can treat as failure (MAYBE RETRY AT SOME POINT FOR ONE TIME???)
        } else {
            if (errno == EINTR) {
                continue; // The connection was interrupted, let's retry
            }
            return false;
        }
    }
    return true;
}

/**
 * @param fd: connection
 * @param buf: buffer to write data into
 * @param len: length of data to receive
 * @return true(success) false(failed)
 *  What defines success? ==>
 *      1) Must return true IFF reads exactly N bytes
 *      2) Must store bytes in buf[0...N-1] in order
 *      3) Must NOT overwrite previously received data
 */
bool recv_all(int fd, void *buffer, size_t len) {
    // We will write retrieved data to this buffer
    char * buf = (char *) (buffer);
    size_t received = 0;
    while (received < len) {
        ssize_t n = recv(fd, buf + received, len - received, 0);
        if (n > 0) {
            received += size_t(n);
        } else if (n == 0) {
            return false; // No more connection
        } else {
            if (errno == EINTR) {
                continue;
            }
            return false;
        }
    }
    return true;
}