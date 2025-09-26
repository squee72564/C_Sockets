#include "helpers.hpp"
#include <asm-generic/socket.h>
#include <cerrno>
#include <cstdlib>

#include <cstring>
#include <iostream>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>

int main() {
    int status = 0;

    struct addrinfo hints, *servinfo;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    if ((status = getaddrinfo("127.0.0.1", "3007", &hints, &servinfo)) != 0) {
        std::cerr << "getaddrinfo failed for client!\n";
        exit(EXIT_FAILURE);
    }

    int client_fd = -1;

    struct addrinfo * result = networking::bind_to_first_res(servinfo, client_fd);
    
    if (!result) {
        std::cerr << "Failed to get file descriptor and bind for client socket!\n";
        exit(EXIT_FAILURE);
    }
    
    if ((status = connect(client_fd, result->ai_addr, result->ai_addrlen)) != 0) {
        std::cerr << "Failed to connect to server!\n";
        exit(EXIT_FAILURE);
    }

    freeaddrinfo(servinfo);

    struct timeval tv = {0};
    tv.tv_sec = 5;
    tv.tv_usec = 0;
    setsockopt(client_fd, SOL_SOCKET, SO_SNDTIMEO, &tv, sizeof tv);
    setsockopt(client_fd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof tv);

    std::string message{};
    char buff[512] = {0};
    while (true) {
        std::cout << "> ";
        std::getline(std::cin, message);
        if (message == "exit") break;
        message.push_back('\n');

        const char *message_cstr = message.c_str();
        size_t message_size = message.size();
        size_t sent_bytes_total = 0UL;
        while (sent_bytes_total != message_size) {
            int bytes_sent = send(
                client_fd,
                &message_cstr[sent_bytes_total],
                message_size - sent_bytes_total,
                0
            );

            if (bytes_sent == 0) {
                std::cerr << "Connection closed by server\n";
                exit(EXIT_FAILURE);
            }

            if (bytes_sent < 0) {
                if (errno == EAGAIN || errno == EWOULDBLOCK) {
                    std::cerr << "Client send() timed out!\n";
                    exit(EXIT_FAILURE);
                }
                if (errno == EPIPE) {
                    std::cerr << "Connection closed by server\n";
                    exit(EXIT_FAILURE);
                }
                std::cerr << "Client failed to send to server\n";
                exit(EXIT_FAILURE);
            }

            sent_bytes_total += bytes_sent;
        }

        std::cout << "--";
        memset(buff, 0, sizeof(char) * 512);
        size_t total_recv = 0;
        while (total_recv < sent_bytes_total) {
            ssize_t recieved = recv(client_fd, buff, 512, 0);

            if (recieved == 0) {
                std::cerr << "Server closed connection\n";
                exit(EXIT_FAILURE);
            }

            if (recieved < 0) {
                if (errno == EAGAIN || errno == EWOULDBLOCK) {
                    std::cerr << "Client recv() timed out!\n";
                    exit(EXIT_FAILURE);
                }
                if (errno == EPIPE) {
                    std::cerr << "Connection closed by server\n";
                    exit(EXIT_FAILURE);
                }
                std::cerr << "Client failed to recv from server\n";
                exit(EXIT_FAILURE);
            }

            std::cout.write(buff, recieved);

            total_recv += recieved;
        }

        std::cout << std::endl;
    }

    close(client_fd);

    return 0;
}
