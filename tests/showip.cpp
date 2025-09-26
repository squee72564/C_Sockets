#include <cstdlib>
#include <iostream>

#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>

int main(int argc, char *argv[]) {

    if (argc != 2) {
        std::cerr << "usage: " << argv[0] << " <hostname>\n";
        exit(EXIT_FAILURE);
    }
    struct addrinfo hints, *res = nullptr, *p = nullptr;
    int status = 0;
    char ipstr[INET6_ADDRSTRLEN] = {0};
    memset(&hints, 0, sizeof hints);

    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    if ((status = getaddrinfo(argv[1], NULL, &hints, &res)) != 0) {
        std::cerr << "getaddrinfo failed: " << gai_strerror(status) << "\n";
        exit(EXIT_FAILURE);
    }

    std::cout << "IP addresses for " << argv[1] << ":\n\n";

    for (p = res; p != nullptr; p = p->ai_next) {
        void *addr = nullptr;
        const char *ipver = nullptr;

        if (p->ai_family == AF_INET) {
            struct sockaddr_in *ipv4 = (struct sockaddr_in *)p->ai_addr;
            addr = &(ipv4->sin_addr);
            ipver = "IPv4";
        } else {
            struct sockaddr_in6 *ipv6 = (struct sockaddr_in6 *)p->ai_addr;
            addr = &(ipv6->sin6_addr);
            ipver = "IPv6";
        }

        inet_ntop(p->ai_family, addr, ipstr, sizeof ipstr);
        std::cout << "  " << ipver << ": " << ipstr << "\n";
    }

    freeaddrinfo(res);
    return EXIT_SUCCESS; 
}
