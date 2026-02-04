#include "log.h"
#include <sys/socket.h>
#include <netinet/in.h>  // sockaddr_in
#include <arpa/inet.h>   // htons, htonl
#include <unistd.h>      // close
#include <cstring>       // memset
#include <cstdio>        // perror

int main() {
    logutil::info("Worker starting...");
    // TODO: connect to coordinator, register, heartbeat loop
    int client_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (client_fd == -1) {
        perror("Socket error");
        return -1;
    }
    logutil::info("Socket created");

    sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(5000);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    // Attempt to establish a connection to specified server using the socket
    int success = connect(client_fd, (sockaddr *) &addr, sizeof(addr));
    if (success == -1) {
        perror("Connection error");
        close(client_fd);
        return -1;
    }

    logutil::info("[Client] Connection successful");

    close(client_fd);

    return 0;
}
