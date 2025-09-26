#ifndef C_SOCKET_HELPERS_HPP
#define C_SOCKET_HELPERS_HPP

#include <asm-generic/socket.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#include <iostream>

namespace networking {

inline void * get_in_addr(int family, struct sockaddr *sa) {
    if (!sa) return nullptr;

    if (family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    } else if (family == AF_INET6) {
        return &(((struct sockaddr_in6*)sa)->sin6_addr);
    }

    std::cout << "family is : " << family << "\n";

    return nullptr;
}

inline void print_ai_result(struct addrinfo *ai) {
    if (!ai) { std::cerr << "ai is nullptr\n"; return; }
    void *addr = networking::get_in_addr(ai->ai_family, ai->ai_addr);

    if (!addr) {
        std::cerr << "Unknown address family\n";
        return;
    }

    char server_address_buff[INET6_ADDRSTRLEN];
    const char *ret =
        inet_ntop(ai->ai_family, addr, server_address_buff, sizeof server_address_buff);

    if (!ret) {
        std::cerr << "inet_ntop failed!\n";
        return;
    }

    std::cout << "Listening on " << server_address_buff << "\n";
}

inline struct addrinfo*
bind_to_first_res(struct addrinfo *servinfo, int& sock_fd) {
    struct addrinfo *p = nullptr;

    for (p = servinfo; p != nullptr; p = p->ai_next) {
        if (p->ai_family != AF_INET && p->ai_family != AF_INET6) {
            continue;
        }

        if ((sock_fd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
            std::cerr << "could not connect to socket, trying again.\n";
            continue;
        }

        break;
    }

    return p;
}

} // namespace networking

#endif // C_SOCKET_HELPERS_HPP
