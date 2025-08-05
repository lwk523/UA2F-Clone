#include "config.h"
#ifdef UA2F_ENABLE_UCI
#include <uci.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Global configuration instance */
ua2f_config_t config;

/* Helper to get environment variable as integer */
static int getenv_int(const char *name, int default_val) {
    char *env = getenv(name);
    return env ? atoi(env) : default_val;
}

/* Load configuration from environment and optionally UCI */
int config_load(void) {
    /* Default values */
    config.enabled = 1;
    config.handle_fw = 1;
    config.handle_tls = 0;
    config.handle_mmtls = 0;
    config.handle_intranet = 0;
    config.disable_connmark = 0;
    config.min_threads = 1;
    config.max_threads = 4;
    config.custom_ua[0] = '\0';

    /* Override from environment */
    config.enabled = getenv_int("UA2F_ENABLED", config.enabled);
    config.handle_fw = getenv_int("UA2F_HANDLE_FW", config.handle_fw);
    config.handle_tls = getenv_int("UA2F_HANDLE_TLS", config.handle_tls);
    config.handle_mmtls = getenv_int("UA2F_HANDLE_MMTLS", config.handle_mmtls);
    config.handle_intranet = getenv_int("UA2F_HANDLE_INTRAnet", config.handle_intranet);
    config.disable_connmark = getenv_int("UA2F_DISABLE_CONNMARK", config.disable_connmark);
    config.min_threads = (unsigned int)getenv_int("UA2F_MIN_THREADS", config.min_threads);
    config.max_threads = (unsigned int)getenv_int("UA2F_MAX_THREADS", config.max_threads);
    if (config.max_threads < config.min_threads) {
        config.max_threads = config.min_threads;
    }
    char *env_ua = getenv("UA2F_CUSTOM_UA");
    if (env_ua && env_ua[0] != '\0') {
        strncpy(config.custom_ua, env_ua, sizeof(config.custom_ua) - 1);
        config.custom_ua[sizeof(config.custom_ua) - 1] = '\0';
    }
#ifdef UA2F_ENABLE_UCI
    /* TODO: load from UCI if compiled with UCI support */
#endif
    return 0;
}
