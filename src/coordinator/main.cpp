
#include "log.h"
#include <sys/socket.h>
#include <netinet/in.h>  // sockaddr_in
#include <arpa/inet.h>   // htons, htonl
#include <unistd.h>      // close
#include <cstring>       // memset
#include <cstdio>        // perror

int main() {
    logutil::info("Coordinator starting...");
    // TODO: listen socket, accept, recv message, reply
    
    // Creates a new TCP socket (two-way endpoint to allow communication)
    int sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (sock_fd == -1) {
        perror("Failed to create socket");
        return -1;
    }
    // Now we want to bind to that socket (coordinator)
    /**
    MUST create a sockaddr_in struct, fill it in, then pass it
    */
   int opt = -1;
    setsockopt(sock_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    // We make sure that this is in correct network byte order (across diff comp. archs. = store in diff. orders)
    addr.sin_port = htons(5000);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    // Bind address and port number to socket (so OS knows how to receive data sent here)
    if (bind(sock_fd, (sockaddr *) &addr, sizeof(addr)) == -1) {
        perror("Bind error");
        close(sock_fd);
        return -1;
    }
    logutil::info("Bind success");
    // Now we have to prepare the socket to accept incoming connection requests ==> "passive socket"
    if (listen(sock_fd, 20) == -1) {
        perror("Listen failed");
        close(sock_fd);
        return -1;
    }
    logutil::info("Listen success");

    // Accepts new connection from a client or outer entity ==> gets first connection req. and creates NEW socket for that connection
    while (true) {
        logutil::info("Waiting to accept connections...");

        // At some point we need to store client_fd somewhere OR spawn a thread to handle it

        int client_fd = accept(sock_fd, nullptr, nullptr);
        if (client_fd == -1) {
            perror("Accept error");
            close(sock_fd);
            return -1;
        }
        logutil::info("[Server] Established connection");
    }

    logutil::info("[Server] Coordinator exiting...");

    close(sock_fd);

    return 0;
}
