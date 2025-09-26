#include <iostream>
#include <thread>
#include <cstdlib>
#include <cstring>

#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>

#include "helpers.hpp"

int main() {
    int status = 0;

    struct addrinfo hints, *servinfo;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    if ((status = getaddrinfo(nullptr, "3007", &hints, &servinfo)) != 0) {
        std::cerr << "getaddrinfo failed for server!\n";
        exit(EXIT_FAILURE);
    }

    int socket_fd = -1;

    struct addrinfo* result = networking::bind_to_first_res(servinfo, socket_fd);

    if (!result) {
        std::cerr << "Failed to get file descriptor and bind for server socket!\n";
        exit(EXIT_FAILURE);
    }

    int yes = 1;
    if (setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof yes) == -1) {
        std::cerr << "could not set sock opt\n";
        exit(EXIT_FAILURE);
    }

    if (bind(socket_fd, result->ai_addr, result->ai_addrlen) == -1) {
        std::cerr << "could not bind socket\n";
        exit(EXIT_FAILURE);
    }

    if ((status = listen(socket_fd, 5)) != 0) {
        std::cerr << "listen failed for server socket!\n";
        exit(EXIT_FAILURE);
    }

    networking::print_ai_result(result);
    freeaddrinfo(servinfo);

    while (true) {
        sockaddr_storage client_sa = {0};
        socklen_t client_len = sizeof client_sa;

        int client_fd = accept(socket_fd, (struct sockaddr*)&client_sa, &client_len);
        
        if (client_fd < 0) {
            std::cerr << "accept failed for server socket!\n";
            exit(EXIT_FAILURE);
        }

        std::cout << "Client " << client_fd << " connected\n";
        
        auto handle_client_connection = [client_fd, socket_fd]() {
            const size_t buff_len = 1024UL;
            char buff[buff_len] = {0};
            int bytes_recv = 0;

            while ((bytes_recv = recv(client_fd, &buff, buff_len, 0)) != 0) {
                if (bytes_recv == -1) {
                    std::cerr
                        << "Server failed to recieve message from client "
                        << client_fd << "\n";

                    return;
                }
                std::cout << "client (" << client_fd << "): ";
                std::cout.write(buff, bytes_recv);
                std::cout.flush();

                ssize_t total_sent = 0;
                while (total_sent < bytes_recv) {

                    ssize_t sent = send(client_fd, buff, bytes_recv, 0);

                    if (sent == -1) {
                        std::cerr << "failed send to client (" << client_fd << ")\n";
                        break;
                    }

                    total_sent += sent;
                }
            }

            std::cout << "Client (" << client_fd << ") disconnecting\n";
            close(client_fd);
        };

        std::thread(handle_client_connection).detach();
    }

    return EXIT_SUCCESS;
}
