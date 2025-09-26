#include <iostream>
#include <vector>
#include <cstdlib>

#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>

int main() {
    std::vector<int> client_fds;
    int status = 0;

    struct addrinfo hints, *res;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    if ((status = getaddrinfo(nullptr, "3007", &hints, &res)) != 0) {
        std::cerr << "getaddrinfo failed for server!\n";
        exit(EXIT_FAILURE);
    }

    int socket_fd = socket(
        res->ai_family,
        res->ai_socktype,
        res->ai_protocol
    );

    if (socket_fd == -1) {
        std::cerr << "Failed to get file descriptor for server socket!\n";
        exit(EXIT_FAILURE);
    }

    if ((status = bind(socket_fd, res->ai_addr, res->ai_addrlen)) != 0) {
        std::cerr << "bind failed for server socket!\n";
        exit(EXIT_FAILURE);
    }

    int yes = 1;
    if ((status = setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof yes)) != 0) {
        std::cerr << "setsockopt failed for server socket!\n";
        exit(EXIT_FAILURE);
    }

    if ((status = listen(socket_fd, 1)) != 0) {
        std::cerr << "listen failed for server socket!\n";
        exit(EXIT_FAILURE);
    }

    sockaddr_storage client_sa;
    socklen_t client_len = sizeof client_sa;
    int client_fd = accept(socket_fd, (struct sockaddr*)&client_sa, &client_len);
    
    if (client_fd < 0) {
        std::cerr << "accept failed for server socket!\n";
        exit(EXIT_FAILURE);
    }

    const size_t buff_len = 512UL;
    char buff[buff_len] = {0};
    int bytes_recv = 0;

    while ((bytes_recv = recv(client_fd, &buff, buff_len, 0)) != 0) {
        if (bytes_recv == -1) {
            std::cerr << "Server failed to recieve message from client!\n";
            exit(EXIT_FAILURE);
        }
        std::cout.write(buff, bytes_recv);
        std::cout.flush();
    }

    return EXIT_SUCCESS;
}
