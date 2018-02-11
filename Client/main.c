//
// Created by evgenii on 22.01.18.
//

#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <zconf.h>
#include <stdlib.h>
#include <net/if.h>
#include <sys/ioctl.h>

int main(int argc, char** argv) {
    char* interface = "enp7s0";
    char* multicast_addr = "239.0.12.35";
    uint16_t port = 1224;
    int sd;
    if ((sd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0) {
        perror("Error: Con't create socket");
        return 1;
    }
    int val = 1;
    setsockopt(sd, SOL_SOCKET, SO_BROADCAST, &val, sizeof(int));
    struct ifreq ifr;
    ifr.ifr_addr.sa_family = AF_INET;
    strncpy(ifr.ifr_name, interface, IFNAMSIZ-1);
    ioctl(sd, SIOCGIFADDR, &ifr);
    ((struct sockaddr_in *) (&ifr.ifr_addr))->sin_port = htons(port);
    (((struct sockaddr_in *) (&ifr.ifr_addr))->sin_addr.s_addr) = htonl(INADDR_ANY);
    //ioctl(sd, SIOCGIFBRDADDR, &ifr);
    socklen_t socklen = sizeof(struct sockaddr);
    bind(sd, (const struct sockaddr *) &ifr.ifr_addr, socklen);
    struct ip_mreq mreq;
    inet_aton(multicast_addr, &(mreq.imr_multiaddr));
    mreq.imr_interface.s_addr = (((struct sockaddr_in *) (&ifr.ifr_addr))->sin_addr.s_addr);
    setsockopt(sd, IPPROTO_IP, IP_ADD_MEMBERSHIP, &mreq, sizeof(mreq));
    ssize_t bytes_read;
    char buff[1024];
    while (1) {
        if ((bytes_read = recv(sd,buff, 1024, 0)) <= 0) {
            close(sd);
        }
        printf("Recv: ");
        printf("%s", buff);
    }
}