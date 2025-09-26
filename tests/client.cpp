#include <cstdlib>

#include <cstring>
#include <iostream>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>

int main() {
    int status = 0;

    struct addrinfo hints, *res;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    if ((status = getaddrinfo("127.0.0.1", "3007", &hints, &res)) != 0) {
        std::cerr << "getaddrinfo failed for client!\n";
        exit(EXIT_FAILURE);
    }

    int client_fd = socket(
        res->ai_family,
        res->ai_socktype,
        res->ai_protocol
    );

    if (client_fd == -1) {
        std::cerr << "Failed to get file descriptor for client socket!\n";
        exit(EXIT_FAILURE);
    }
    
    if ((status = connect(client_fd, res->ai_addr, res->ai_addrlen)) != 0) {
        std::cerr << "Failed to connect to server!\n";
        exit(EXIT_FAILURE);
    }


    std::string message{};
    std::getline(std::cin, message);
    while (message != "exit") {
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

            if (bytes_sent < 0) {
                std::cerr << "Client failed to send a part of the message!\n";
                exit(EXIT_FAILURE);
            }

            sent_bytes_total += bytes_sent;
        }

        std::getline(std::cin, message);
    }

    return 0;
}
