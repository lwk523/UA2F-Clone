#ifndef UA2F_CONFIG_H
#define UA2F_CONFIG_H
#include <stdint.h>

/* Configuration structure for UA2F */
typedef struct ua2f_config {
    int enabled;            /* UA2F enabled flag */
    int handle_fw;          /* Auto add firewall rules */
    int handle_tls;         /* Handle TLS traffic */
    int handle_mmtls;       /* Handle WeChat (mmtls) traffic */
    int handle_intranet;    /* Handle intranet traffic */
    int disable_connmark;   /* Disable connmark usage */
    char custom_ua[1024];   /* Custom User-Agent string */
    unsigned int min_threads; /* Minimum worker threads */
    unsigned int max_threads; /* Maximum worker threads */
} ua2f_config_t;

/* Global config instance */
extern ua2f_config_t config;

/* Load configuration (from UCI or environment) */
int config_load(void);

#endif /* UA2F_CONFIG_H */
