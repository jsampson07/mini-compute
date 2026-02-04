#pragma once
#include <cstddef>
#include <cstdint>
#include <string>

/*
send() ==> does NOT necessarily send entire message in one go
recv() ==> may return partial data (instead of entire intended payload)

How to address?
    - implement helper methods send_all() and recv_all()
        - implement send and recv here but until they are sent/received entirely

Cycle of connection:
    Server:
        socket --> bind --> listen --> accept
    Client:
        socket --> connect

Failure signs:
    recv() - returns 0 on success; other side closed connection
    send()/recv() - returns -1 with errno set (print it) 
 */

bool send_all(int fd, const void *data, size_t len);
bool recv_all(int fd, void *buf, size_t len);