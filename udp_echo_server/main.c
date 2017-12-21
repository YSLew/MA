#include <stdio.h>
#include "net/sock/udp.h"
#include "net/sock/util.h"

uint8_t buf[128];
char server_addr[50] = "[fe80::7b78:3f01:b6a3:c2e]:12345";

int main(void)
{
    sock_udp_ep_t local = SOCK_IPV6_EP_ANY;
    sock_udp_t sock;
    local.port = 8791;
    if (sock_udp_create(&sock, &local, NULL, 0) < 0) {
        puts("Error creating UDP sock");
        return 1;
    }
    while (1) {
        sock_udp_ep_t remote;
        sock_udp_str2ep(&remote, server_addr);        
        ssize_t res;
        if ((res = sock_udp_recv(&sock, buf, sizeof(buf), SOCK_NO_TIMEOUT,
                                 &remote)) >= 0) {
            puts("Received a message");
            if (sock_udp_send(&sock, "Server Received", sizeof("Server Received"), &remote) < 0) {
                puts("Error sending reply");
            }
        }
    }
    return 0;
}