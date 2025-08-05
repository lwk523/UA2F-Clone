#include "utils.h"

#include <ctype.h>
#include <string.h>
#include <arpa/inet.h>

// Case-insensitive substring search within a buffer
size_t strcasestr_n(const char *haystack, size_t haystack_len,
                    const char *needle, size_t needle_len) {
    if (needle_len == 0 || haystack_len < needle_len) {
        return (size_t)-1;
    }
    for (size_t i = 0; i <= haystack_len - needle_len; i++) {
        size_t j;
        for (j = 0; j < needle_len; j++) {
            char c1 = haystack[i + j];
            char c2 = needle[j];
            if (tolower((unsigned char)c1) != tolower((unsigned char)c2)) {
                break;
            }
        }
        if (j == needle_len) {
            return i;
        }
    }
    return (size_t)-1;
}

// Update TCP checksum for IPv4 packet after modification
void update_tcp_checksum_ipv4(struct iphdr *iph, struct tcphdr *th) {
    th->check = 0;
    uint32_t sum = 0;
    uint16_t *src = (uint16_t *)&iph->saddr;
    uint16_t *dst = (uint16_t *)&iph->daddr;
    // pseudo header
    sum += ntohs(src[0]) + ntohs(src[1]);
    sum += ntohs(dst[0]) + ntohs(dst[1]);
    sum += IPPROTO_TCP;
    uint16_t tcp_len = ntohs(iph->tot_len) - (iph->ihl << 2);
    sum += tcp_len;
    // TCP header and payload
    uint16_t *ptr = (uint16_t *)th;
    for (int i = 0; i < (tcp_len / 2); i++) {
        sum += ntohs(ptr[i]);
    }
    if (tcp_len & 1) {
        sum += ((uint8_t *)th)[tcp_len - 1] << 8;
    }
    // fold to 16 bits
    while (sum >> 16) {
        sum = (sum & 0xFFFF) + (sum >> 16);
    }
    th->check = htons((uint16_t)(~sum));
}

// Update TCP checksum for IPv6 packet after modification
void update_tcp_checksum_ipv6(struct ipv6hdr *ip6, struct tcphdr *th) {
    th->check = 0;
    uint32_t sum = 0;
    // pseudo header: 16-byte src and dest addresses
    uint16_t *addr = (uint16_t *)&ip6->saddr;
    for (int i = 0; i < 8; i++) {
        sum += ntohs(addr[i]);
    }
    addr = (uint16_t *)&ip6->daddr;
    for (int i = 0; i < 8; i++) {
        sum += ntohs(addr[i]);
    }
    uint32_t payload_len = ntohs(ip6->payload_len);
    sum += (payload_len);
    sum += IPPROTO_TCP;
    // TCP header and payload
    uint16_t *ptr = (uint16_t *)th;
    for (uint32_t i = 0; i < (payload_len / 2); i++) {
        sum += ntohs(ptr[i]);
    }
    if (payload_len & 1) {
        sum += ((uint8_t *)th)[payload_len - 1] << 8;
    }
    while (sum >> 16) {
        sum = (sum & 0xFFFF) + (sum >> 16);
    }
    th->check = htons((uint16_t)(~sum));
}

// Check if IPv4 address is in private ranges (10/8, 172.16/12, 192.168/16)
bool is_private_ipv4(uint32_t ip) {
    ip = ntohl(ip);
    if ((ip & 0xFF000000) == 0x0A000000) return true;
    if ((ip & 0xFFF00000) == 0xAC100000) return true;
    if ((ip & 0xFFFF0000) == 0xC0A80000) return true;
    return false;
}

// Check if IPv6 address is link-local or unique local
bool is_local_ipv6(const struct in6_addr *ip6) {
    // Unique local (fc00::/7) and link-local (fe80::/10)
    if ((ip6->s6_addr[0] & 0xFE) == 0xFC) return true;
    if (ip6->s6_addr[0] == 0xFE && (ip6->s6_addr[1] & 0xC0) == 0x80) return true;
    // loopback ::1
    bool all_zero = true;
    for (int i = 0; i < 15; i++) {
        if (ip6->s6_addr[i] != 0) {
            all_zero = false;
            break;
        }
    }
    if (all_zero && ip6->s6_addr[15] == 1) return true;
    return false;
}
