#include <stdio.h>
#include <string.h>
#include "net/sock/udp.h"
#include "msg.h"
#include "shell.h"
#include "net/gnrc/ipv6/netif.h"
#include "net/gnrc/rpl.h"

#define SC_NETIF_IPV6_DEFAULT_PREFIX_LEN (64)

uint8_t buf[128];
kernel_pid_t iface_pid = 7;                             //interface
char *addr_str = "2001:db8::1";                         //ipv6 address
uint8_t rpl_instance_id = 1;

static int add_addr(kernel_pid_t dev, char *addr_str);                          //add an IPv6 address to the interface
static int rpl_dodag_root(uint8_t instance_id, char *addr_str);                 //initiate the interface as dodag root

int main(void){

    sock_udp_ep_t local = SOCK_IPV6_EP_ANY;
    sock_udp_t sock;
    local.port = 8791;
    if (sock_udp_create(&sock, &local, NULL, 0) < 0) {
        puts("Error creating UDP sock");
        return 1;
    }

    add_addr(iface_pid, addr_str);
    gnrc_rpl_init(iface_pid);
    printf("successfully initialized RPL on interface %d\n", iface_pid);
    rpl_dodag_root(rpl_instance_id, addr_str);

/*
    static const shell_command_t shell_commands[] = {
        { NULL, NULL, NULL }
    };

        puts("All up, running the shell now");
        char line_buf[SHELL_DEFAULT_BUFSIZE];
        shell_run(shell_commands, line_buf, SHELL_DEFAULT_BUFSIZE);
*/
    while (1) {


        sock_udp_ep_t remote;
        ssize_t res;
        if ((res = sock_udp_recv(&sock, buf, sizeof(buf), SOCK_NO_TIMEOUT,
                                 &remote)) >= 0) {
            puts("Received a message");
            if (buf[9] > 1)
            {
                printf ("sensor_id :: %d\n\r", buf[0]);
                printf ("%d %4d %4d %4d %4d %4d %4d %4d\n\r",  buf[1],  buf[2],  buf[3],  buf[4],  buf[5],  buf[6],  buf[7],  buf[8]);
            }
            else if(buf[17] > 1)
            {
                printf ("sensor_id :: %d\n\r", buf[0]);
                printf ("%d %d %d %d\n\r",  buf[1],  buf[2],  buf[3],  buf[4]);
                printf ("%d %d %d %d\n\r",  buf[5],  buf[6],  buf[7],  buf[8]);
                printf ("%d %d %d %d\n\r",  buf[9],  buf[10], buf[11], buf[12]);
                printf ("%d %d %d %d\n\r",  buf[13], buf[14], buf[15], buf[16]);                
            }
            else
                puts("Unknown data received.");
            puts("------------------------------------------");
            if (sock_udp_send(&sock, buf, res, &remote) < 0) {
                puts("Error sending reply");
            }
        }

    }
    return 0;
}

static int add_addr(kernel_pid_t dev, char *addr_str)
{
    enum {
        _UNICAST = 0,
        _MULTICAST,     /* multicast value just to check if given addr is mc */
        _ANYCAST
    } type = _UNICAST;

    ipv6_addr_t addr;
    ipv6_addr_t *ifaddr;
    uint8_t flags = 0;

    uint8_t prefix_len = ipv6_addr_split(addr_str, '/', SC_NETIF_IPV6_DEFAULT_PREFIX_LEN);

    if ((prefix_len < 1) || (prefix_len > IPV6_ADDR_BIT_LEN))
        prefix_len = SC_NETIF_IPV6_DEFAULT_PREFIX_LEN;

    if (ipv6_addr_from_str(&addr, addr_str) == NULL) {
        puts("error: unable to parse IPv6 address.");
           return 0;
        }

    flags = GNRC_IPV6_NETIF_ADDR_FLAGS_NDP_AUTO;
    if (type == _ANYCAST) {
        flags |= GNRC_IPV6_NETIF_ADDR_FLAGS_NON_UNICAST;
        }
    else {
        flags |= GNRC_IPV6_NETIF_ADDR_FLAGS_UNICAST;
        }

    if ((ifaddr = gnrc_ipv6_netif_add_addr(dev, &addr, prefix_len, flags)) == NULL) {
        printf("error: unable to add IPv6 address\n");
        return 0;
        }

    /* Address shall be valid infinitely */
    gnrc_ipv6_netif_addr_get(ifaddr)->valid = UINT32_MAX;
    /* Address shall be preferred infinitely */
    gnrc_ipv6_netif_addr_get(ifaddr)->preferred = UINT32_MAX;

    printf("success: added %s/%d to interface %" PRIkernel_pid "\n", addr_str,
               prefix_len, dev);
    return 1;
    
}

static int rpl_dodag_root(uint8_t instance_id, char *addr_str)
{
    ipv6_addr_t dodag_id;

    if (ipv6_addr_from_str(&dodag_id, addr_str) == NULL) {
        puts("error: <dodag_id> must be a valid IPv6 address");
        return 1;
    }

    gnrc_rpl_instance_t *inst = gnrc_rpl_root_init(instance_id, &dodag_id, false, false);
    if (inst == NULL) {
        char addr_str[IPV6_ADDR_MAX_STR_LEN];
        printf("error: could not add DODAG (%s) to instance (%d)\n",
                ipv6_addr_to_str(addr_str, &dodag_id, sizeof(addr_str)), instance_id);
        return 1;
    }

    printf("successfully added a new RPL DODAG\n");
    return 0;
}
