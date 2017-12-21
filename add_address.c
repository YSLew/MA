#include <stdio.h>
#include <string.h>
#include "msg.h"
#include "net/gnrc/ipv6/netif.h"

#define IFACE 7
#define SC_NETIF_IPV6_DEFAULT_PREFIX_LEN (64)


static int add_addr(kernel_pid_t dev)
{
    enum {
        _UNICAST = 0,
        _MULTICAST,     /* multicast value just to check if given addr is mc */
        _ANYCAST
    } type = _UNICAST;

    char *addr_str = "2001:db8::1";
    ipv6_addr_t addr;
    ipv6_addr_t *ifaddr;
    uint8_t flags = 0;

    uint8_t prefix_len = ipv6_addr_split(addr_str, '/', SC_NETIF_IPV6_DEFAULT_PREFIX_LEN);

    if ((prefix_len < 1) || (prefix_len > IPV6_ADDR_BIT_LEN)) {
        prefix_len = SC_NETIF_IPV6_DEFAULT_PREFIX_LEN;

    if (ipv6_addr_from_str(&addr, addr_str) == NULL) {
        puts("error: unable to parse IPv6 address.");
           return 1;
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
        return 1;
        }

    /* Address shall be valid infinitely */
    gnrc_ipv6_netif_addr_get(ifaddr)->valid = UINT32_MAX;
    /* Address shall be preferred infinitely */
    gnrc_ipv6_netif_addr_get(ifaddr)->preferred = UINT32_MAX;

    printf("success: added %s/%d to interface %" PRIkernel_pid "\n", addr_str,
               prefix_len, dev);
    return 1;
    }
}