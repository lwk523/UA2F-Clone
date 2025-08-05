#ifndef UA2F_UTILS_H
#define UA2F_UTILS_H

#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>
#include <linux/ip.h>
#include <linux/ipv6.h>
#include <linux/tcp.h>

size_t strcasestr_n(const char *haystack, size_t haystack_len,
                    const char *needle, size_t needle_len);

void update_tcp_checksum_ipv4(struct iphdr *iph, struct tcphdr *th);
void update_tcp_checksum_ipv6(struct ipv6hdr *ip6, struct tcphdr *th);

bool is_private_ipv4(uint32_t ip);
bool is_local_ipv6(const struct in6_addr *ip6);

#endif // UA2F_UTILS_H
